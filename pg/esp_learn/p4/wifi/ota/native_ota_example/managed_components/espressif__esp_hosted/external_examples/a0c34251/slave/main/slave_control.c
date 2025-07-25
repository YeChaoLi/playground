// SPDX-License-Identifier: Apache-2.0
// Copyright 2015-2021 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_private/wifi.h"
#include "slave_control.h"
#include "esp_hosted_rpc.pb-c.h"
#include "esp_ota_ops.h"
#include "esp_hosted_rpc.h"
#include "esp_hosted_transport.h"
#include "esp_hosted_bitmasks.h"
#include "slave_wifi_config.h"

/* Slave-side: Always support reserved field decoding for maximum compatibility
 * The host may or may not have CONFIG_ESP_HOSTED_DECODE_WIFI_RESERVED_FIELD enabled
 */
#define H_DECODE_WIFI_RESERVED_FIELD 1

#include "coprocessor_fw_version.h"

#define MAC_STR_LEN                 17
#define MAC2STR(a)                  (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR                      "%02x:%02x:%02x:%02x:%02x:%02x"
#define SUCCESS                     0
#define FAILURE                     -1
#define SSID_LENGTH                 32
#define PASSWORD_LENGTH             64
#define MIN_TX_POWER                8
#define MAX_TX_POWER                84

/* Bits for wifi connect event */
#define WIFI_CONNECTED_BIT          BIT0
#define WIFI_FAIL_BIT               BIT1
#define WIFI_NO_AP_FOUND_BIT        BIT2
#define WIFI_WRONG_PASSWORD_BIT     BIT3
#define WIFI_HOST_REQUEST_BIT       BIT4

#define MAX_STA_CONNECT_ATTEMPTS    3

#define TIMEOUT_IN_MIN              (60*TIMEOUT_IN_SEC)
#define TIMEOUT_IN_HOUR             (60*TIMEOUT_IN_MIN)
#define TIMEOUT                     (2*TIMEOUT_IN_MIN)
#define RESTART_TIMEOUT             (5*TIMEOUT_IN_SEC)

#define MIN_HEARTBEAT_INTERVAL      (10)
#define MAX_HEARTBEAT_INTERVAL      (60*60)

#define mem_free(x)                 \
        {                           \
            if (x) {                \
                free(x);            \
                x = NULL;           \
            }                       \
        }


#define NTFY_TEMPLATE(NtFy_MsgId, NtFy_TyPe, NtFy_StRuCt, InIt_FuN)             \
	NtFy_TyPe *ntfy_payload = NULL;                                             \
	ntfy_payload = (NtFy_TyPe*)calloc(1,sizeof(NtFy_TyPe));                     \
	if (!ntfy_payload) {                                                        \
		ESP_LOGE(TAG,"Failed to allocate memory");                              \
		return ESP_ERR_NO_MEM;                                                  \
	}                                                                           \
	InIt_FuN(ntfy_payload);                                                     \
	ntfy->payload_case = NtFy_MsgId;                                            \
	ntfy->NtFy_StRuCt = ntfy_payload;                                           \
	ntfy_payload->resp = SUCCESS;

#define RPC_TEMPLATE(RspTyPe, RspStRuCt, ReqType, ReqStruct, InIt_FuN)         \
  RspTyPe *resp_payload = NULL;                                                 \
  ReqType *req_payload = NULL;                                                  \
  if (!req || !resp || !req->ReqStruct) {                                      \
    ESP_LOGE(TAG, "Invalid parameters");                                        \
    return ESP_FAIL;                                                            \
  }                                                                             \
  req_payload = req->ReqStruct;                                                 \
  resp_payload = (RspTyPe *)calloc(1, sizeof(RspTyPe));                         \
  if (!resp_payload) {                                                          \
      ESP_LOGE(TAG, "Failed to alloc mem for resp.%s\n",#RspStRuCt);            \
      return ESP_ERR_NO_MEM;                                                    \
  }                                                                             \
  resp->RspStRuCt = resp_payload;                                               \
  InIt_FuN(resp_payload);                                                       \
  resp_payload->resp = SUCCESS;                                                 \


/* Simple is same above just, we dod not need req_payload unused warning */
#define RPC_TEMPLATE_SIMPLE(RspTyPe, RspStRuCt, ReqType, ReqStruct, InIt_FuN)  \
  RspTyPe *resp_payload = NULL;                                                 \
  if (!req || !resp) {                                                          \
    ESP_LOGE(TAG, "Invalid parameters");                                        \
    return ESP_FAIL;                                                            \
  }                                                                             \
  resp_payload = (RspTyPe *)calloc(1, sizeof(RspTyPe));                         \
  if (!resp_payload) {                                                          \
      ESP_LOGE(TAG, "Failed to alloc mem for resp.%s\n",#RspStRuCt);            \
      return ESP_ERR_NO_MEM;                                                    \
  }                                                                             \
  resp->RspStRuCt = resp_payload;                                               \
  InIt_FuN(resp_payload);                                                       \
  resp_payload->resp = SUCCESS;                                                 \

#define RPC_RESP_ASSIGN_FIELD(PaRaM)                                            \
  resp_payload->PaRaM = PaRaM

#define RPC_RET_FAIL_IF(ConDiTiOn) do {                                         \
  int rEt = (ConDiTiOn);                                                        \
  if (rEt) {                                                                    \
    resp_payload->resp = rEt;                                                   \
    ESP_LOGE(TAG, "%s:%u failed [%s] = [%d]", __func__,__LINE__,#ConDiTiOn, rEt); \
    return ESP_OK;                                                              \
  }                                                                             \
} while(0);


#define RPC_ALLOC_ELEMENT(TyPe,MsG_StRuCt,InIt_FuN) {                        \
    TyPe *NeW_AllocN = (TyPe *)calloc(1, sizeof(TyPe));                       \
    if (!NeW_AllocN) {                                                        \
        ESP_LOGI(TAG,"Failed to allocate memory for req.%s\n",#MsG_StRuCt);   \
        resp_payload->resp = RPC_ERR_MEMORY_FAILURE;                         \
		goto err;                                                             \
    }                                                                         \
    MsG_StRuCt = NeW_AllocN;                                                  \
    InIt_FuN(MsG_StRuCt);                                                     \
}

#define NTFY_ALLOC_ELEMENT(TyPe,MsG_StRuCt,InIt_FuN) {                        \
    TyPe *NeW_AllocN = (TyPe *)calloc(1, sizeof(TyPe));                       \
    if (!NeW_AllocN) {                                                        \
        ESP_LOGI(TAG,"Failed to allocate memory for req.%s\n",#MsG_StRuCt);   \
        ntfy_payload->resp = RPC_ERR_MEMORY_FAILURE;                         \
		goto err;                                                             \
    }                                                                         \
    MsG_StRuCt = NeW_AllocN;                                                  \
    InIt_FuN(MsG_StRuCt);                                                     \
}

#define NTFY_COPY_BYTES(dest, src, num)                                         \
  do {                                                                          \
    if (num) {                                                                  \
      dest.data = (uint8_t *)calloc(1, num);                                    \
      if (!dest.data) {                                                         \
        ESP_LOGE(TAG, "%s:%u Failed to duplicate bytes\n",__func__,__LINE__);   \
        ntfy_payload->resp = FAILURE;                                           \
        return ESP_OK;                                                          \
      }                                                                         \
      memcpy(dest.data, src, num);                                              \
	  dest.len = num;                                                           \
    }                                                                           \
  } while(0)

#define RPC_REQ_COPY_BYTES(dest, src, num_bytes)                               \
  if (src.len && src.data)                                                      \
    memcpy((char*)dest, src.data, min(min(sizeof(dest), num_bytes), src.len));

#define RPC_REQ_COPY_STR RPC_REQ_COPY_BYTES


#define RPC_RESP_COPY_STR(dest, src, max_len)                                  \
  if (src) {                                                                    \
    dest.data = (uint8_t*)strndup((char*)src, max_len);                         \
    if (!dest.data) {                                                           \
      ESP_LOGE(TAG, "%s:%u Failed to duplicate bytes\n",__func__,__LINE__);     \
      resp_payload->resp = FAILURE;                                             \
      return ESP_OK;                                                            \
    }                                                                           \
	dest.len = min(max_len,strlen((char*)src)+1);                               \
  }

#define RPC_RESP_COPY_BYTES_SRC_UNCHECKED(dest, src, num)                      \
  do {                                                                          \
    if (num) {                                                                  \
      dest.data = (uint8_t *)calloc(1, num);                                    \
      if (!dest.data) {                                                         \
        ESP_LOGE(TAG, "%s:%u Failed to duplicate bytes\n",__func__,__LINE__);   \
        resp_payload->resp = FAILURE;                                           \
        return ESP_OK;                                                          \
      }                                                                         \
      memcpy(dest.data, src, num);                                              \
	  dest.len = num;                                                           \
    }                                                                           \
  } while(0)

#define RPC_RESP_COPY_BYTES(dest, src, num)                                    \
  if (src) {                                                                    \
    RPC_RESP_COPY_BYTES_SRC_UNCHECKED(dest, src, num);                         \
  }

#define RPC_COPY_STR(dest, src, max_len)                                        \
  if (src) {                                                                    \
    dest.data = (uint8_t*)strndup((char*)src, max_len);                         \
    if (!dest.data) {                                                           \
      ESP_LOGE(TAG, "%s:%u Failed to duplicate bytes\n",__func__,__LINE__);     \
      return FAILURE;                                                           \
    }                                                                           \
	dest.len = min(max_len,strlen((char*)src)+1);                               \
  }

#define RPC_COPY_BYTES(dest, src, num)                                          \
  do {                                                                          \
    if (num) {                                                                  \
      dest.data = (uint8_t *)calloc(1, num);                                    \
      if (!dest.data) {                                                         \
        ESP_LOGE(TAG, "%s:%u Failed to duplicate bytes\n",__func__,__LINE__);   \
        return FAILURE;                                                         \
      }                                                                         \
      memcpy(dest.data, src, num);                                              \
	  dest.len = num;                                                           \
    }                                                                           \
  } while(0)

typedef struct esp_rpc_cmd {
	int req_num;
	esp_err_t (*command_handler)(Rpc *req,
			Rpc *resp, void *priv_data);
} esp_rpc_req_t;


static const char* TAG = "slave_ctrl";
static TimerHandle_t handle_heartbeat_task;
static uint32_t hb_num;

/* FreeRTOS event group to signal when we are connected*/
static esp_event_handler_instance_t instance_any_id;

static esp_ota_handle_t handle;
const esp_partition_t* update_partition = NULL;
static int ota_msg = 0;

extern esp_err_t wlan_sta_rx_callback(void *buffer, uint16_t len, void *eb);
extern esp_err_t wlan_ap_rx_callback(void *buffer, uint16_t len, void *eb);

extern volatile uint8_t station_connected;
extern volatile uint8_t softap_started;

/* OTA end timer callback */
void vTimerCallback( TimerHandle_t xTimer )
{
	xTimerDelete(xTimer, 0);
	esp_restart();
}

/* Function returns mac address of station/softap */
static esp_err_t req_wifi_get_mac(Rpc *req,
		Rpc *resp, void *priv_data)
{
	uint8_t mac[BSSID_BYTES_SIZE] = {0};

	RPC_TEMPLATE_SIMPLE(RpcRespGetMacAddress, resp_get_mac_address,
			RpcReqGetMacAddress, req_get_mac_address,
			rpc__resp__get_mac_address__init);

	RPC_RET_FAIL_IF(esp_wifi_get_mac(req->req_get_mac_address->mode, mac));

	ESP_LOGI(TAG,"mac [" MACSTR "]", MAC2STR(mac));

	RPC_RESP_COPY_BYTES_SRC_UNCHECKED(resp_payload->mac, mac, BSSID_BYTES_SIZE);

	ESP_LOGD(TAG, "resp mac [" MACSTR "]", MAC2STR(resp_payload->mac.data));

	return ESP_OK;
}

/* Function returns wifi mode */
static esp_err_t req_wifi_get_mode(Rpc *req,
		Rpc *resp, void *priv_data)
{
	wifi_mode_t mode = 0;

	RPC_TEMPLATE_SIMPLE(RpcRespGetMode, resp_get_wifi_mode,
			RpcReqGetMode, req_get_wifi_mode,
			rpc__resp__get_mode__init);

	RPC_RET_FAIL_IF(esp_wifi_get_mode(&mode));

	resp_payload->mode = mode;

	return ESP_OK;
}

/* Function sets wifi mode */
static esp_err_t req_wifi_set_mode(Rpc *req,
		Rpc *resp, void *priv_data)
{
	wifi_mode_t num = 0;

	RPC_TEMPLATE(RpcRespSetMode, resp_set_wifi_mode,
			RpcReqSetMode, req_set_wifi_mode,
			rpc__resp__set_mode__init);

	num = req_payload->mode;
	RPC_RET_FAIL_IF(esp_wifi_set_mode(num));

	return ESP_OK;
}

/* Function sets MAC address for station/softap */
static esp_err_t req_wifi_set_mac(Rpc *req,
		Rpc *resp, void *priv_data)
{
	uint8_t * mac = NULL;

	RPC_TEMPLATE(RpcRespSetMacAddress, resp_set_mac_address,
			RpcReqSetMacAddress, req_set_mac_address,
			rpc__resp__set_mac_address__init);

	if (!req_payload->mac.data || (req_payload->mac.len != BSSID_BYTES_SIZE)) {
		ESP_LOGE(TAG, "Invalid MAC address data or len: %d", req->req_set_mac_address->mac.len);
		resp_payload->resp = ESP_ERR_INVALID_ARG;
		goto err;
	}

	mac = req_payload->mac.data;
	ESP_LOGD(TAG, "mac: " MACSTR, MAC2STR(mac));

	RPC_RET_FAIL_IF(esp_wifi_set_mac(req_payload->mode, mac));
err:
	return ESP_OK;
}

/* Function sets power save mode */
static esp_err_t req_wifi_set_ps(Rpc *req,
		Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespSetPs, resp_wifi_set_ps,
			RpcReqSetPs, req_wifi_set_ps,
			rpc__resp__set_ps__init);
	RPC_RET_FAIL_IF(esp_wifi_set_ps(req_payload->type));
	return ESP_OK;
}

/* Function returns current power save mode */
static esp_err_t req_wifi_get_ps(Rpc *req,
		Rpc *resp, void *priv_data)
{
	wifi_ps_type_t ps_type = 0;

	RPC_TEMPLATE_SIMPLE(RpcRespGetPs, resp_wifi_get_ps,
			RpcReqGetPs, req_wifi_get_ps,
			rpc__resp__get_ps__init);
	RPC_RET_FAIL_IF(esp_wifi_get_ps(&ps_type));
	resp_payload->type = ps_type;
	return ESP_OK;
}

/* Function OTA begin */
static esp_err_t req_ota_begin_handler (Rpc *req,
		Rpc *resp, void *priv_data)
{
	esp_err_t ret = ESP_OK;
	RpcRespOTABegin *resp_payload = NULL;

	if (!req || !resp) {
		ESP_LOGE(TAG, "Invalid parameters");
		return ESP_FAIL;
	}

	ESP_LOGI(TAG, "OTA update started");

	resp_payload = (RpcRespOTABegin *)
		calloc(1,sizeof(RpcRespOTABegin));
	if (!resp_payload) {
		ESP_LOGE(TAG,"Failed to allocate memory");
		return ESP_ERR_NO_MEM;
	}
	rpc__resp__otabegin__init(resp_payload);
	resp->payload_case = RPC__PAYLOAD_RESP_OTA_BEGIN;
	resp->resp_ota_begin = resp_payload;

	/* Identify next OTA partition */
	update_partition = esp_ota_get_next_update_partition(NULL);
	if (update_partition == NULL) {
		ESP_LOGE(TAG, "Failed to get next update partition");
		ret = -1;
		goto err;
	}

	ESP_LOGI(TAG, "Prepare partition for OTA\n");
	ret = esp_ota_begin(update_partition, OTA_SIZE_UNKNOWN, &handle);
	if (ret) {
		ESP_LOGE(TAG, "OTA begin failed[%d]", ret);
		goto err;
	}

	ota_msg = 1;

	resp_payload->resp = SUCCESS;
	return ESP_OK;
err:
	resp_payload->resp = ret;
	return ESP_OK;

}

/* Function OTA write */
static esp_err_t req_ota_write_handler (Rpc *req,
		Rpc *resp, void *priv_data)
{
	esp_err_t ret = ESP_OK;
	RpcRespOTAWrite *resp_payload = NULL;

	if (!req || !resp) {
		ESP_LOGE(TAG, "Invalid parameters");
		return ESP_FAIL;
	}

	resp_payload = (RpcRespOTAWrite *)calloc(1,sizeof(RpcRespOTAWrite));
	if (!resp_payload) {
		ESP_LOGE(TAG,"Failed to allocate memory");
		return ESP_ERR_NO_MEM;
	}

	if (ota_msg) {
		ESP_LOGI(TAG, "Flashing image\n");
		ota_msg = 0;
	}
	rpc__resp__otawrite__init(resp_payload);
	resp->payload_case = RPC__PAYLOAD_RESP_OTA_WRITE;
	resp->resp_ota_write = resp_payload;

	ret = esp_ota_write( handle, (const void *)req->req_ota_write->ota_data.data,
			req->req_ota_write->ota_data.len);
	if (ret != ESP_OK) {
		ESP_LOGE(TAG, "OTA write failed with return code 0x%x",ret);
		resp_payload->resp = ret;
		return ESP_OK;
	}
	resp_payload->resp = SUCCESS;
	return ESP_OK;
}

/* Function OTA end */
static esp_err_t req_ota_end_handler (Rpc *req,
		Rpc *resp, void *priv_data)
{
	esp_err_t ret = ESP_OK;
	RpcRespOTAEnd *resp_payload = NULL;
	TimerHandle_t xTimer = NULL;

	if (!req || !resp) {
		ESP_LOGE(TAG, "Invalid parameters");
		return ESP_FAIL;
	}

	resp_payload = (RpcRespOTAEnd *)calloc(1,sizeof(RpcRespOTAEnd));
	if (!resp_payload) {
		ESP_LOGE(TAG,"Failed to allocate memory");
		return ESP_ERR_NO_MEM;
	}
	rpc__resp__otaend__init(resp_payload);
	resp->payload_case = RPC__PAYLOAD_RESP_OTA_END;
	resp->resp_ota_end = resp_payload;

	ret = esp_ota_end(handle);
	if (ret != ESP_OK) {
		if (ret == ESP_ERR_OTA_VALIDATE_FAILED) {
			ESP_LOGE(TAG, "Image validation failed, image is corrupted");
		} else {
			ESP_LOGE(TAG, "OTA update failed in end (%s)!", esp_err_to_name(ret));
		}
		goto err;
	}

	/* set OTA partition for next boot */
	ret = esp_ota_set_boot_partition(update_partition);
	if (ret != ESP_OK) {
		ESP_LOGE(TAG, "esp_ota_set_boot_partition failed (%s)!", esp_err_to_name(ret));
		goto err;
	}
	xTimer = xTimerCreate("Timer", RESTART_TIMEOUT , pdFALSE, 0, vTimerCallback);
	if (xTimer == NULL) {
		ESP_LOGE(TAG, "Failed to create timer to restart system");
		ret = -1;
		goto err;
	}
	ret = xTimerStart(xTimer, 0);
	if (ret != pdPASS) {
		ESP_LOGE(TAG, "Failed to start timer to restart system");
		ret = -2;
		goto err;
	}
	ESP_LOGE(TAG, "**** OTA updated successful, ESP32 will reboot in 5 sec ****");
	resp_payload->resp = SUCCESS;
	return ESP_OK;
err:
	resp_payload->resp = ret;
	return ESP_OK;
}

#if 0
/* Function vendor specific ie */
static esp_err_t req_set_softap_vender_specific_ie_handler (Rpc *req,
		Rpc *resp, void *priv_data)
{
	esp_err_t ret = ESP_OK;
	RpcRespSetSoftAPVendorSpecificIE *resp_payload = NULL;
	RpcReqSetSoftAPVendorSpecificIE *p_vsi = req->req_set_softap_vendor_specific_ie;
	RpcReqVendorIEData *p_vid = NULL;
	vendor_ie_data_t *v_data = NULL;

	if (!req || !resp || !p_vsi) {
		ESP_LOGE(TAG, "Invalid parameters");
		return ESP_FAIL;
	}
	p_vid = p_vsi->vendor_ie_data;

	if (!p_vsi->enable) {

		ESP_LOGI(TAG,"Disable softap vendor IE\n");

	} else {

		ESP_LOGI(TAG,"Enable softap vendor IE\n");

		if (!p_vid ||
			!p_vid->payload.len ||
			!p_vid->payload.data) {
			ESP_LOGE(TAG, "Invalid parameters");
			return ESP_FAIL;
		}

		v_data = (vendor_ie_data_t*)calloc(1,sizeof(vendor_ie_data_t)+p_vid->payload.len);
		if (!v_data) {
			ESP_LOGE(TAG, "Malloc failed at %s:%u\n", __func__, __LINE__);
			return ESP_FAIL;
		}

		v_data->length = p_vid->length;
		v_data->element_id = p_vid->element_id;
		v_data->vendor_oui_type = p_vid->vendor_oui_type;

		memcpy(v_data->vendor_oui, p_vid->vendor_oui.data, VENDOR_OUI_BUF);

		if (p_vid->payload.len && p_vid->payload.data) {
			memcpy(v_data->payload, p_vid->payload.data, p_vid->payload.len);
		}
	}


	resp_payload = (RpcRespSetSoftAPVendorSpecificIE *)
		calloc(1,sizeof(RpcRespSetSoftAPVendorSpecificIE));
	if (!resp_payload) {
		ESP_LOGE(TAG,"Failed to allocate memory");
		if (v_data)
			mem_free(v_data);
		return ESP_ERR_NO_MEM;
	}

	rpc__resp__set_soft_apvendor_specific_ie__init(resp_payload);
	resp->payload_case = RPC__PAYLOAD_RESP_SET_SOFTAP_VENDOR_SPECIFIC_IE;
	resp->resp_set_softap_vendor_specific_ie = resp_payload;


	ret = esp_wifi_set_vendor_ie(p_vsi->enable,
			p_vsi->type,
			p_vsi->idx,
			v_data);

	if (v_data)
		mem_free(v_data);

	if (ret != ESP_OK) {
		ESP_LOGE(TAG, "Failed to set vendor information element %d \n", ret);
		resp_payload->resp = FAILURE;
		return ESP_OK;
	}
	resp_payload->resp = SUCCESS;
	return ESP_OK;
}
#endif

/* Function set wifi maximum TX power */
static esp_err_t req_wifi_set_max_tx_power(Rpc *req,
		Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespWifiSetMaxTxPower, resp_set_wifi_max_tx_power,
			RpcReqWifiSetMaxTxPower, req_set_wifi_max_tx_power,
			rpc__resp__wifi_set_max_tx_power__init);
	RPC_RET_FAIL_IF(esp_wifi_set_max_tx_power(req_payload->power));
	return ESP_OK;
}

/* Function get wifi TX current power */
static esp_err_t req_wifi_get_max_tx_power(Rpc *req,
		Rpc *resp, void *priv_data)
{
	int8_t power = 0;

	RPC_TEMPLATE_SIMPLE(RpcRespWifiGetMaxTxPower, resp_get_wifi_max_tx_power,
			RpcReqWifiGetMaxTxPower, req_get_wifi_max_tx_power,
			rpc__resp__wifi_get_max_tx_power__init);
	RPC_RET_FAIL_IF(esp_wifi_get_max_tx_power(&power));
	resp_payload->power = power;
	return ESP_OK;
}

static void heartbeat_timer_cb(TimerHandle_t xTimer)
{
	send_event_to_host(RPC_ID__Event_Heartbeat);
	hb_num++;
}

static void stop_heartbeat(void)
{
	if (handle_heartbeat_task &&
		xTimerIsTimerActive(handle_heartbeat_task)) {
		ESP_LOGI(TAG, "Stopping HB timer");
		xTimerStop(handle_heartbeat_task, portMAX_DELAY);
		xTimerDelete(handle_heartbeat_task, portMAX_DELAY);
		handle_heartbeat_task = NULL;
	}
	hb_num = 0;
}

static esp_err_t start_heartbeat(int duration)
{
	esp_err_t ret = ESP_OK;

	handle_heartbeat_task = xTimerCreate("HB_Timer",
			duration*TIMEOUT_IN_SEC, pdTRUE, 0, heartbeat_timer_cb);
	if (handle_heartbeat_task == NULL) {
		ESP_LOGE(TAG, "Failed to Heartbeat");
		return ESP_FAIL;
	}

	ret = xTimerStart(handle_heartbeat_task, 0);
	if (ret != pdPASS) {
		ESP_LOGE(TAG, "Failed to start Heartbeat");
		return ESP_FAIL;
	}
	ESP_LOGI(TAG, "HB timer started for %u sec\n", duration);

	return ESP_OK;
}

static esp_err_t configure_heartbeat(bool enable, int hb_duration)
{
	esp_err_t ret = ESP_OK;
	int duration = hb_duration ;

	if (!enable) {
		ESP_LOGI(TAG, "Stop Heatbeat");
		stop_heartbeat();

	} else {
		if (duration < MIN_HEARTBEAT_INTERVAL)
			duration = MIN_HEARTBEAT_INTERVAL;
		if (duration > MAX_HEARTBEAT_INTERVAL)
			duration = MAX_HEARTBEAT_INTERVAL;

		stop_heartbeat();

		ret = start_heartbeat(duration);
	}

	return ret;
}

/* Function to config heartbeat */
static esp_err_t req_config_heartbeat(Rpc *req,
		Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespConfigHeartbeat,
			resp_config_heartbeat,
			RpcReqConfigHeartbeat,
			req_config_heartbeat,
			rpc__resp__config_heartbeat__init);

	RPC_RET_FAIL_IF(configure_heartbeat(req_payload->enable, req_payload->duration));

	return ESP_OK;
}


static void event_handler(void* arg, esp_event_base_t event_base,
		int32_t event_id, void* event_data)
{
	if (event_base == WIFI_EVENT) {
		if (event_id == WIFI_EVENT_AP_STACONNECTED) {
			wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *) event_data;
			ESP_LOGI(TAG, "station "MACSTR" join, AID=%d",
					MAC2STR(event->mac), event->aid);
			send_event_data_to_host(RPC_ID__Event_AP_StaConnected,
					event_data, sizeof(wifi_event_ap_staconnected_t));
		} else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
			wifi_event_ap_stadisconnected_t *event =
				(wifi_event_ap_stadisconnected_t *) event_data;
			ESP_LOGI(TAG, "station "MACSTR" leave, AID=%d",
					MAC2STR(event->mac), event->aid);
			send_event_data_to_host(RPC_ID__Event_AP_StaDisconnected,
					event_data, sizeof(wifi_event_ap_stadisconnected_t));
		} else if (event_id == WIFI_EVENT_SCAN_DONE) {
			ESP_LOGI(TAG, "Wi-Fi sta scan done");
			// rpc event receiver expects Scan Done to have this ID
			send_event_data_to_host(RPC_ID__Event_StaScanDone,
					event_data, sizeof(wifi_event_sta_scan_done_t));
		} else if (event_id == WIFI_EVENT_STA_CONNECTED) {
			ESP_LOGI(TAG, "Sta mode connected");
			send_event_data_to_host(RPC_ID__Event_StaConnected,
				event_data, sizeof(wifi_event_sta_connected_t));
			esp_wifi_internal_reg_rxcb(ESP_IF_WIFI_STA, (wifi_rxcb_t) wlan_sta_rx_callback);
			station_connected = true;
		} else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
			station_connected = false;
			esp_wifi_internal_reg_rxcb(ESP_IF_WIFI_STA, NULL);
			ESP_LOGI(TAG, "Sta mode disconnect");
			send_event_data_to_host(RPC_ID__Event_StaDisconnected,
				event_data, sizeof(wifi_event_sta_disconnected_t));
		} else {
			if (event_id == WIFI_EVENT_AP_START) {
				ESP_LOGI(TAG,"softap started");
				esp_wifi_internal_reg_rxcb(ESP_IF_WIFI_AP, (wifi_rxcb_t) wlan_ap_rx_callback);
				softap_started = 1;
			} else if (event_id == WIFI_EVENT_AP_STOP) {
				ESP_LOGI(TAG,"softap stopped");
				esp_wifi_internal_reg_rxcb(ESP_IF_WIFI_AP, NULL);
				softap_started = 0;
			}

			send_event_data_to_host(RPC_ID__Event_WifiEventNoArgs,
					&event_id, sizeof(event_id));
		}
	}
}

// macros to format output
#define PRINT_HEADER() ESP_LOGI(TAG, "     Wifi Init Param | Default |    Host |  Actual");
#define PRINT_FOOTER() ESP_LOGI(TAG, " End Wifi Init Param |");
#define PRINT_USE_HOST_VALUE(param_str, default, host, final)        \
	ESP_LOGD(TAG, "% 20s | % 7d | % 7d | % 7d", param_str, default, host, final);
#define PRINT_USE_DEFAULT_VALUE(param_str, default, host, final)     \
	ESP_LOGW(TAG, "% 20s | % 7d | % 7d | % 7d", param_str, default, host, final);
#define PRINT_HEX64_USE_HOST_VALUE(param_str, default, host, final)    \
	ESP_LOGD(TAG, "% 20s | 0x% 5"PRIx32" | 0x% 5"PRIx64" | 0x% 5"PRIx64, param_str, default, host, final);
#define PRINT_HEX64_USE_DEFAULT_VALUE(param_str, default, host, final) \
	ESP_LOGW(TAG, "% 20s | % 7"PRIx32" | % 7"PRIx64" | % 7"PRIx64, param_str, default, host, final);

// macros to copy host or default value
#define USE_HOST_VALUE(PARAM_STR, DEFAULT, PARAM) \
	do {                                          \
		dst_config->PARAM = src_config->PARAM;    \
		PRINT_USE_HOST_VALUE(PARAM_STR,           \
				DEFAULT,                          \
				src_config->PARAM,                \
				dst_config->PARAM);               \
	} while(0);

#define USE_DEFAULT_VALUE(PARAM_STR, DEFAULT, PARAM) \
	do {                                             \
		dst_config->PARAM = DEFAULT;                 \
		PRINT_USE_DEFAULT_VALUE(PARAM_STR,           \
				DEFAULT,                             \
				src_config->PARAM,                   \
				dst_config->PARAM);                  \
	} while(0);

/** Returns the merged wifi init config
 * Compares the src config from the host with our Wi-Fi defaults
 * and adjust dst_config as necessary.
 *
 * Also displays the changed configs.
 */
static wifi_init_config_t * get_merged_init_config(wifi_init_config_t *dst_config, WifiInitConfig *src_config)
{
	/* always use value from host, except for
	 * - cache_tx_buf_num
	 * - feature_caps
	 */
	PRINT_HEADER();
	USE_HOST_VALUE("static_rx_buf", CONFIG_ESP_WIFI_STATIC_RX_BUFFER_NUM, static_rx_buf_num);
	USE_HOST_VALUE("dynamic_rx_buf", CONFIG_ESP_WIFI_DYNAMIC_RX_BUFFER_NUM, dynamic_rx_buf_num);
	USE_HOST_VALUE("tx_buf_type", CONFIG_ESP_WIFI_TX_BUFFER_TYPE, tx_buf_type);
	USE_HOST_VALUE("static_tx_buf", WIFI_STATIC_TX_BUFFER_NUM, static_tx_buf_num);
	USE_HOST_VALUE("dynamic_tx_buf", WIFI_DYNAMIC_TX_BUFFER_NUM, dynamic_tx_buf_num);
	USE_HOST_VALUE("rx_mgmt_buf_type", CONFIG_ESP_WIFI_DYNAMIC_RX_MGMT_BUF, rx_mgmt_buf_type);
	USE_HOST_VALUE("rx_mgmt_buf", WIFI_RX_MGMT_BUF_NUM_DEF, rx_mgmt_buf_num);

	if (WIFI_ENABLE_CACHE_TX_BUFFER) {
		// use setting from host
		USE_HOST_VALUE("cache_tx_buf", WIFI_CACHE_TX_BUFFER_NUM, cache_tx_buf_num);
		dst_config->feature_caps = src_config->feature_caps;
		PRINT_HEX64_USE_HOST_VALUE("feature_caps", WIFI_FEATURE_CAPS,
				src_config->feature_caps,
				dst_config->feature_caps);
	} else {
		if (WIFI_FEATURE_CAPS != src_config->feature_caps) {
			// don't use host setting, which may have enabled CACHE_TX_BUFFER
			USE_DEFAULT_VALUE("cache_tx_buf", WIFI_CACHE_TX_BUFFER_NUM, cache_tx_buf_num);
			dst_config->feature_caps = WIFI_FEATURE_CAPS;
			PRINT_HEX64_USE_DEFAULT_VALUE("feature_caps", WIFI_FEATURE_CAPS,
					src_config->feature_caps,
					dst_config->feature_caps);
		} else {
			USE_HOST_VALUE("cache_tx_buf", WIFI_CACHE_TX_BUFFER_NUM, cache_tx_buf_num);
			dst_config->feature_caps = src_config->feature_caps;
			PRINT_HEX64_USE_HOST_VALUE("feature_caps", WIFI_FEATURE_CAPS,
					src_config->feature_caps,
					dst_config->feature_caps);
		}
	}

	USE_HOST_VALUE("csi_enable", WIFI_CSI_ENABLED, csi_enable);
	USE_HOST_VALUE("ampdu_rx_enable", WIFI_AMPDU_RX_ENABLED, ampdu_rx_enable);
	USE_HOST_VALUE("ampdu_tx_enable", WIFI_AMPDU_TX_ENABLED, ampdu_tx_enable);
	USE_HOST_VALUE("amsdu_tx_enable", WIFI_AMSDU_TX_ENABLED, amsdu_tx_enable);
	USE_HOST_VALUE("nvs_enable", WIFI_NVS_ENABLED, nvs_enable);
	USE_HOST_VALUE("nano_enable", WIFI_NANO_FORMAT_ENABLED, nano_enable);
	USE_HOST_VALUE("rx_ba_win", WIFI_DEFAULT_RX_BA_WIN, rx_ba_win);
	USE_HOST_VALUE("wifi_task_core", WIFI_TASK_CORE_ID, wifi_task_core_id);
	USE_HOST_VALUE("beacon_max_len", WIFI_SOFTAP_BEACON_MAX_LEN, beacon_max_len);
	USE_HOST_VALUE("mgmt_sbuf_num", WIFI_MGMT_SBUF_NUM, mgmt_sbuf_num);
	USE_HOST_VALUE("sta_disconnected_pm", WIFI_STA_DISCONNECTED_PM_ENABLED, sta_disconnected_pm);
	USE_HOST_VALUE("espnow_max_encrypt",CONFIG_ESP_WIFI_ESPNOW_MAX_ENCRYPT_NUM, espnow_max_encrypt_num);
	USE_HOST_VALUE("tx_hetb_queue", WIFI_TX_HETB_QUEUE_NUM, tx_hetb_queue_num);
	USE_HOST_VALUE("dump_hesigb_enable", WIFI_DUMP_HESIGB_ENABLED, dump_hesigb_enable);
	PRINT_FOOTER();

	dst_config->magic = src_config->magic;

	return dst_config;
}

static esp_err_t req_wifi_init(Rpc *req, Rpc *resp, void *priv_data)
{
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

	RPC_TEMPLATE(RpcRespWifiInit, resp_wifi_init,
			RpcReqWifiInit, req_wifi_init,
			rpc__resp__wifi_init__init);

	RPC_RET_FAIL_IF(!req_payload->cfg);
	RPC_RET_FAIL_IF(esp_wifi_init(get_merged_init_config(&cfg, req_payload->cfg)));

	ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
			ESP_EVENT_ANY_ID,
			&event_handler,
			NULL,
			&instance_any_id));

	return ESP_OK;
}

static esp_err_t req_wifi_deinit(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE_SIMPLE(RpcRespWifiDeinit, resp_wifi_deinit,
			RpcReqWifiDeinit, req_wifi_deinit,
			rpc__resp__wifi_deinit__init);

	RPC_RET_FAIL_IF(esp_wifi_deinit());

	return ESP_OK;
}


static esp_err_t req_wifi_start(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE_SIMPLE(RpcRespWifiStart, resp_wifi_start,
			RpcReqWifiStart, req_wifi_start,
			rpc__resp__wifi_start__init);

	RPC_RET_FAIL_IF(esp_wifi_start());
	return ESP_OK;
}

static esp_err_t req_wifi_stop(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE_SIMPLE(RpcRespWifiStop, resp_wifi_stop,
			RpcReqWifiStop, req_wifi_stop,
			rpc__resp__wifi_stop__init);

	RPC_RET_FAIL_IF(esp_wifi_stop());

	return ESP_OK;
}

static esp_err_t req_wifi_connect(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE_SIMPLE(RpcRespWifiConnect, resp_wifi_connect,
			RpcReqWifiConnect, req_wifi_connect,
			rpc__resp__wifi_connect__init);

	ESP_LOGI(TAG, "************ connect ****************");
	RPC_RET_FAIL_IF(esp_wifi_connect());

	return ESP_OK;
}

static esp_err_t req_wifi_disconnect(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE_SIMPLE(RpcRespWifiDisconnect, resp_wifi_disconnect,
			RpcReqWifiDisconnect, req_wifi_disconnect,
			rpc__resp__wifi_disconnect__init);

	RPC_RET_FAIL_IF(esp_wifi_disconnect());

	return ESP_OK;
}


static esp_err_t req_wifi_set_config(Rpc *req, Rpc *resp, void *priv_data)
{
	wifi_config_t cfg = {0};

	RPC_TEMPLATE(RpcRespWifiSetConfig, resp_wifi_set_config,
			RpcReqWifiSetConfig, req_wifi_set_config,
			rpc__resp__wifi_set_config__init);

	RPC_RET_FAIL_IF((req_payload->iface != WIFI_IF_STA) &&
			(req_payload->iface != WIFI_IF_AP));

	RPC_RET_FAIL_IF(!req_payload->cfg);
	if (req_payload->iface == WIFI_IF_STA) {
		wifi_sta_config_t * p_a_sta = &(cfg.sta);
		WifiStaConfig * p_c_sta = req_payload->cfg->sta;
		RPC_RET_FAIL_IF(!req_payload->cfg->sta);
		RPC_REQ_COPY_STR(p_a_sta->ssid, p_c_sta->ssid, SSID_LENGTH);
		if (strlen((char*)p_a_sta->ssid))
			ESP_LOGI(TAG, "STA set config: SSID:%s", p_a_sta->ssid);
		RPC_REQ_COPY_STR(p_a_sta->password, p_c_sta->password, PASSWORD_LENGTH);
		if (strlen((char*)p_a_sta->password))
			ESP_LOGD(TAG, "STA: password:%s", p_a_sta->password);
		p_a_sta->scan_method = p_c_sta->scan_method;
		p_a_sta->bssid_set = p_c_sta->bssid_set;

		if (p_a_sta->bssid_set)
			RPC_REQ_COPY_BYTES(p_a_sta->bssid, p_c_sta->bssid, BSSID_BYTES_SIZE);

		p_a_sta->channel = p_c_sta->channel;
		p_a_sta->listen_interval = p_c_sta->listen_interval;
		p_a_sta->sort_method = p_c_sta->sort_method;
		if (p_c_sta->threshold) {
			p_a_sta->threshold.rssi = p_c_sta->threshold->rssi;
			p_a_sta->threshold.authmode = p_c_sta->threshold->authmode;
#if H_PRESENT_IN_ESP_IDF_5_4_0
			p_a_sta->threshold.rssi_5g_adjustment = p_c_sta->threshold->rssi_5g_adjustment;
#endif
		}
		//p_a_sta->ssid_hidden = p_c_sta->ssid_hidden;
		//p_a_sta->max_connections = p_c_sta->max_connections;
		if (p_c_sta->pmf_cfg) {
			p_a_sta->pmf_cfg.capable = p_c_sta->pmf_cfg->capable;
			p_a_sta->pmf_cfg.required = p_c_sta->pmf_cfg->required;
		}
		p_a_sta->rm_enabled = H_GET_BIT(WIFI_STA_CONFIG_1_rm_enabled, p_c_sta->bitmask);
		p_a_sta->btm_enabled = H_GET_BIT(WIFI_STA_CONFIG_1_btm_enabled, p_c_sta->bitmask);
		p_a_sta->mbo_enabled = H_GET_BIT(WIFI_STA_CONFIG_1_mbo_enabled, p_c_sta->bitmask);
		p_a_sta->ft_enabled = H_GET_BIT(WIFI_STA_CONFIG_1_ft_enabled, p_c_sta->bitmask);
		p_a_sta->owe_enabled = H_GET_BIT(WIFI_STA_CONFIG_1_owe_enabled, p_c_sta->bitmask);
		p_a_sta->transition_disable = H_GET_BIT(WIFI_STA_CONFIG_1_transition_disable, p_c_sta->bitmask);
#if H_DECODE_WIFI_RESERVED_FIELD
#if H_WIFI_NEW_RESERVED_FIELD_NAMES
		p_a_sta->reserved1 = WIFI_STA_CONFIG_1_GET_RESERVED_VAL(p_c_sta->bitmask);
#else
		p_a_sta->reserved = WIFI_STA_CONFIG_1_GET_RESERVED_VAL(p_c_sta->bitmask);
#endif
#endif

		p_a_sta->sae_pwe_h2e = p_c_sta->sae_pwe_h2e;
		p_a_sta->sae_pk_mode = p_c_sta->sae_pk_mode;
		p_a_sta->failure_retry_cnt = p_c_sta->failure_retry_cnt;

		p_a_sta->he_dcm_set = H_GET_BIT(WIFI_STA_CONFIG_2_he_dcm_set_BIT, p_c_sta->he_bitmask);
		/* WIFI_STA_CONFIG_2_he_dcm_max_constellation_tx is two bits wide */
		p_a_sta->he_dcm_max_constellation_tx = (p_c_sta->he_bitmask >> WIFI_STA_CONFIG_2_he_dcm_max_constellation_tx_BITS) & 0x03;
		/* WIFI_STA_CONFIG_2_he_dcm_max_constellation_rx is two bits wide */
		p_a_sta->he_dcm_max_constellation_rx = (p_c_sta->he_bitmask >> WIFI_STA_CONFIG_2_he_dcm_max_constellation_rx_BITS) & 0x03;

		p_a_sta->he_mcs9_enabled = H_GET_BIT(WIFI_STA_CONFIG_2_he_mcs9_enabled_BIT, p_c_sta->he_bitmask);
		p_a_sta->he_su_beamformee_disabled = H_GET_BIT(WIFI_STA_CONFIG_2_he_su_beamformee_disabled_BIT, p_c_sta->he_bitmask);
		p_a_sta->he_trig_su_bmforming_feedback_disabled = H_GET_BIT(WIFI_STA_CONFIG_2_he_trig_su_bmforming_feedback_disabled_BIT, p_c_sta->he_bitmask);
		p_a_sta->he_trig_mu_bmforming_partial_feedback_disabled = H_GET_BIT(WIFI_STA_CONFIG_2_he_trig_mu_bmforming_partial_feedback_disabled_BIT, p_c_sta->he_bitmask);
		p_a_sta->he_trig_cqi_feedback_disabled = H_GET_BIT(WIFI_STA_CONFIG_2_he_trig_cqi_feedback_disabled_BIT, p_c_sta->he_bitmask);

#if H_PRESENT_IN_ESP_IDF_5_5_0
		p_a_sta->vht_su_beamformee_disabled = H_GET_BIT(WIFI_STA_CONFIG_2_vht_su_beamformee_disabled, p_c_sta->he_bitmask);
		p_a_sta->vht_mu_beamformee_disabled = H_GET_BIT(WIFI_STA_CONFIG_2_vht_mu_beamformee_disabled, p_c_sta->he_bitmask);
		p_a_sta->vht_mcs8_enabled = H_GET_BIT(WIFI_STA_CONFIG_2_vht_mcs8_enabled, p_c_sta->he_bitmask);
#endif

#if H_DECODE_WIFI_RESERVED_FIELD
#if H_WIFI_NEW_RESERVED_FIELD_NAMES
		p_a_sta->reserved2 = WIFI_STA_CONFIG_2_GET_RESERVED_VAL(p_c_sta->he_bitmask);
#else
		p_a_sta->he_reserved = WIFI_STA_CONFIG_2_GET_RESERVED_VAL(p_c_sta->he_bitmask);
#endif
#endif

		/* Avoid using fast scan, which leads to faster SSID selection,
		 * but faces data throughput issues when same SSID broadcasted by weaker AP
		 */
		p_a_sta->scan_method = WIFI_ALL_CHANNEL_SCAN;
		p_a_sta->sort_method = WIFI_CONNECT_AP_BY_SIGNAL;

		RPC_REQ_COPY_STR(p_a_sta->sae_h2e_identifier, p_c_sta->sae_h2e_identifier, SAE_H2E_IDENTIFIER_LEN);
	} else if (req_payload->iface == WIFI_IF_AP) {
		wifi_ap_config_t * p_a_ap = &(cfg.ap);
		WifiApConfig * p_c_ap = req_payload->cfg->ap;
		RPC_RET_FAIL_IF(!req_payload->cfg->ap);
		/* esp_wifi_types.h says SSID should be NULL terminated if ssid_len is 0 */
		RPC_REQ_COPY_STR(p_a_ap->ssid, p_c_ap->ssid, SSID_LENGTH);
		p_a_ap->ssid_len = p_c_ap->ssid_len;
		RPC_REQ_COPY_STR(p_a_ap->password, p_c_ap->password, PASSWORD_LENGTH);
		p_a_ap->channel = p_c_ap->channel;
		p_a_ap->authmode = p_c_ap->authmode;
		p_a_ap->ssid_hidden = p_c_ap->ssid_hidden;
		p_a_ap->max_connection = p_c_ap->max_connection;
		p_a_ap->beacon_interval = p_c_ap->beacon_interval;
		p_a_ap->csa_count = p_c_ap->csa_count;
		p_a_ap->dtim_period = p_c_ap->dtim_period;
		p_a_ap->pairwise_cipher = p_c_ap->pairwise_cipher;
		p_a_ap->ftm_responder = p_c_ap->ftm_responder;
		if (p_c_ap->pmf_cfg) {
			p_a_ap->pmf_cfg.capable = p_c_ap->pmf_cfg->capable;
			p_a_ap->pmf_cfg.required = p_c_ap->pmf_cfg->required;
		}
		p_a_ap->sae_pwe_h2e = p_c_ap->sae_pwe_h2e;
#if H_GOT_AP_CONFIG_PARAM_TRANSITION_DISABLE
		p_a_ap->transition_disable = p_c_ap->transition_disable;
#endif
#if H_PRESENT_IN_ESP_IDF_5_5_0
		p_a_ap->sae_ext = p_c_ap->sae_ext;
		if (p_c_ap->bss_max_idle_cfg) {
			p_a_ap->bss_max_idle_cfg.period = p_c_ap->bss_max_idle_cfg->period;
			p_a_ap->bss_max_idle_cfg.protected_keep_alive = p_c_ap->bss_max_idle_cfg->protected_keep_alive;
		}
		p_a_ap->gtk_rekey_interval = p_c_ap->gtk_rekey_interval;
#endif
	}

	RPC_RET_FAIL_IF(esp_wifi_set_config(req_payload->iface, &cfg));

	return ESP_OK;
}

static esp_err_t req_wifi_get_config(Rpc *req, Rpc *resp, void *priv_data)
{
	wifi_interface_t iface;
	wifi_config_t cfg = {0};

	RPC_TEMPLATE(RpcRespWifiGetConfig, resp_wifi_get_config,
			RpcReqWifiGetConfig, req_wifi_get_config,
			rpc__resp__wifi_get_config__init);

	iface = req_payload->iface;
	resp_payload->iface = iface;
	RPC_RET_FAIL_IF(iface > WIFI_IF_AP);
	RPC_RET_FAIL_IF(esp_wifi_get_config(iface, &cfg));

	RPC_ALLOC_ELEMENT(WifiConfig, resp_payload->cfg, wifi_config__init);
	switch (iface) {

	case WIFI_IF_STA: {
		wifi_sta_config_t * p_a_sta = &(cfg.sta);
		resp_payload->cfg->u_case = WIFI_CONFIG__U_STA;

		RPC_ALLOC_ELEMENT(WifiStaConfig, resp_payload->cfg->sta, wifi_sta_config__init);

		WifiStaConfig * p_c_sta = resp_payload->cfg->sta;
		RPC_RESP_COPY_STR(p_c_sta->ssid, p_a_sta->ssid, SSID_LENGTH);
		RPC_RESP_COPY_STR(p_c_sta->password, p_a_sta->password, PASSWORD_LENGTH);
		p_c_sta->scan_method = p_a_sta->scan_method;
		p_c_sta->bssid_set = p_a_sta->bssid_set;

		//TODO: Expected to break python for bssid
		if (p_c_sta->bssid_set)
			RPC_RESP_COPY_BYTES(p_c_sta->bssid, p_a_sta->bssid, BSSID_BYTES_SIZE);

		p_c_sta->channel = p_a_sta->channel;
		p_c_sta->listen_interval = p_a_sta->listen_interval;
		p_c_sta->sort_method = p_a_sta->sort_method;
		RPC_ALLOC_ELEMENT(WifiScanThreshold, p_c_sta->threshold, wifi_scan_threshold__init);
		p_c_sta->threshold->rssi = p_a_sta->threshold.rssi;
		p_c_sta->threshold->authmode = p_a_sta->threshold.authmode;
#if H_PRESENT_IN_ESP_IDF_5_4_0
		p_c_sta->threshold->rssi_5g_adjustment = p_a_sta->threshold.rssi_5g_adjustment;
#endif
		RPC_ALLOC_ELEMENT(WifiPmfConfig, p_c_sta->pmf_cfg, wifi_pmf_config__init);
		p_c_sta->pmf_cfg->capable = p_a_sta->pmf_cfg.capable;
		p_c_sta->pmf_cfg->required = p_a_sta->pmf_cfg.required;

		if (p_a_sta->rm_enabled)
			H_SET_BIT(WIFI_STA_CONFIG_1_rm_enabled, p_c_sta->bitmask);

		if (p_a_sta->btm_enabled)
			H_SET_BIT(WIFI_STA_CONFIG_1_btm_enabled, p_c_sta->bitmask);

		if (p_a_sta->mbo_enabled)
			H_SET_BIT(WIFI_STA_CONFIG_1_mbo_enabled, p_c_sta->bitmask);

		if (p_a_sta->ft_enabled)
			H_SET_BIT(WIFI_STA_CONFIG_1_ft_enabled, p_c_sta->bitmask);

		if (p_a_sta->owe_enabled)
			H_SET_BIT(WIFI_STA_CONFIG_1_owe_enabled, p_c_sta->bitmask);

		if (p_a_sta->transition_disable)
			H_SET_BIT(WIFI_STA_CONFIG_1_transition_disable, p_c_sta->bitmask);

#if H_DECODE_WIFI_RESERVED_FIELD
#if H_WIFI_NEW_RESERVED_FIELD_NAMES
		WIFI_STA_CONFIG_1_SET_RESERVED_VAL(p_a_sta->reserved1, p_c_sta->bitmask);
#else
		WIFI_STA_CONFIG_1_SET_RESERVED_VAL(p_a_sta->reserved, p_c_sta->bitmask);
#endif
#endif

		p_c_sta->sae_pwe_h2e = p_a_sta->sae_pwe_h2e;
		p_c_sta->sae_pk_mode = p_a_sta->sae_pk_mode;
		p_c_sta->failure_retry_cnt = p_a_sta->failure_retry_cnt;

		/* HE field handling */
		if (p_a_sta->he_dcm_set)
			H_SET_BIT(WIFI_STA_CONFIG_2_he_dcm_set_BIT, p_c_sta->he_bitmask);

		/* WIFI_STA_CONFIG_2_he_dcm_max_constellation_tx is two bits wide */
		if (p_a_sta->he_dcm_max_constellation_tx & 0x03) {
			p_c_sta->he_bitmask |= (p_a_sta->he_dcm_max_constellation_tx & 0x03) << WIFI_STA_CONFIG_2_he_dcm_max_constellation_tx_BITS;
		}
		/* WIFI_STA_CONFIG_2_he_dcm_max_constellation_rx is two bits wide */
		if (p_a_sta->he_dcm_max_constellation_rx & 0x03) {
			p_c_sta->he_bitmask |= (p_a_sta->he_dcm_max_constellation_rx & 0x03) << WIFI_STA_CONFIG_2_he_dcm_max_constellation_rx_BITS;
		}

		if (p_a_sta->he_mcs9_enabled)
			H_SET_BIT(WIFI_STA_CONFIG_2_he_mcs9_enabled_BIT, p_c_sta->he_bitmask);

		if (p_a_sta->he_su_beamformee_disabled)
			H_SET_BIT(WIFI_STA_CONFIG_2_he_su_beamformee_disabled_BIT, p_c_sta->he_bitmask);

		if (p_a_sta->he_trig_su_bmforming_feedback_disabled)
			H_SET_BIT(WIFI_STA_CONFIG_2_he_trig_su_bmforming_feedback_disabled_BIT, p_c_sta->he_bitmask);

		if (p_a_sta->he_trig_mu_bmforming_partial_feedback_disabled)
			H_SET_BIT(WIFI_STA_CONFIG_2_he_trig_mu_bmforming_partial_feedback_disabled_BIT, p_c_sta->he_bitmask);

		if (p_a_sta->he_trig_cqi_feedback_disabled)
			H_SET_BIT(WIFI_STA_CONFIG_2_he_trig_cqi_feedback_disabled_BIT, p_c_sta->he_bitmask);

#if H_PRESENT_IN_ESP_IDF_5_5_0
		if (p_a_sta->vht_su_beamformee_disabled)
			H_SET_BIT(WIFI_STA_CONFIG_2_vht_su_beamformee_disabled, p_c_sta->he_bitmask);

		if (p_a_sta->vht_mu_beamformee_disabled)
			H_SET_BIT(WIFI_STA_CONFIG_2_vht_mu_beamformee_disabled, p_c_sta->he_bitmask);

		if (p_a_sta->vht_mcs8_enabled)
			H_SET_BIT(WIFI_STA_CONFIG_2_vht_mcs8_enabled, p_c_sta->he_bitmask);
#endif

#if H_DECODE_WIFI_RESERVED_FIELD
#if H_WIFI_NEW_RESERVED_FIELD_NAMES
		WIFI_STA_CONFIG_2_SET_RESERVED_VAL(p_a_sta->reserved2, p_c_sta->he_bitmask);
#else
		WIFI_STA_CONFIG_2_SET_RESERVED_VAL(p_a_sta->he_reserved, p_c_sta->he_bitmask);
#endif
#endif

		break;
	}
	case WIFI_IF_AP: {
		wifi_ap_config_t * p_a_ap = &(cfg.ap);
		resp_payload->cfg->u_case = WIFI_CONFIG__U_AP;

		RPC_ALLOC_ELEMENT(WifiApConfig, resp_payload->cfg->ap, wifi_ap_config__init);
		WifiApConfig * p_c_ap = resp_payload->cfg->ap;
		RPC_RESP_COPY_STR(p_c_ap->password, p_a_ap->password, PASSWORD_LENGTH);
		p_c_ap->ssid_len = p_a_ap->ssid_len;
		if (p_c_ap->ssid_len)
			RPC_RESP_COPY_STR(p_c_ap->ssid, p_a_ap->ssid, SSID_LENGTH);
		p_c_ap->channel = p_a_ap->channel;
		p_c_ap->authmode = p_a_ap->authmode;
		p_c_ap->ssid_hidden = p_a_ap->ssid_hidden;
		p_c_ap->max_connection = p_a_ap->max_connection;
		p_c_ap->beacon_interval = p_a_ap->beacon_interval;
		p_c_ap->csa_count = p_a_ap->csa_count;
		p_c_ap->dtim_period = p_a_ap->dtim_period;
		p_c_ap->pairwise_cipher = p_a_ap->pairwise_cipher;
		p_c_ap->ftm_responder = p_a_ap->ftm_responder;
		RPC_ALLOC_ELEMENT(WifiPmfConfig, p_c_ap->pmf_cfg, wifi_pmf_config__init);
		p_c_ap->pmf_cfg->capable = p_a_ap->pmf_cfg.capable;
		p_c_ap->pmf_cfg->required = p_a_ap->pmf_cfg.required;
		p_c_ap->sae_pwe_h2e = p_a_ap->sae_pwe_h2e;
#if H_GOT_AP_CONFIG_PARAM_TRANSITION_DISABLE
		p_c_ap->transition_disable = p_a_ap->transition_disable;
#endif
#if H_PRESENT_IN_ESP_IDF_5_5_0
		p_c_ap->sae_ext = p_a_ap->sae_ext;
		RPC_ALLOC_ELEMENT(WifiBssMaxIdleConfig, p_c_ap->bss_max_idle_cfg, wifi_bss_max_idle_config__init);
		p_c_ap->bss_max_idle_cfg->period = p_a_ap->bss_max_idle_cfg.period;
		p_c_ap->bss_max_idle_cfg->protected_keep_alive = p_a_ap->bss_max_idle_cfg.protected_keep_alive;
		p_c_ap->gtk_rekey_interval = p_a_ap->gtk_rekey_interval;
#endif
		break;
	}
	default:
		ESP_LOGE(TAG, "Unsupported WiFi interface[%u]\n", iface);
	} //switch

err:
	return ESP_OK;
}

static esp_err_t req_wifi_scan_start(Rpc *req, Rpc *resp, void *priv_data)
{
	wifi_scan_config_t scan_conf = {0};
	WifiScanConfig *p_c = NULL;
	WifiScanTime *p_c_st = NULL;
	wifi_scan_config_t * p_a = &scan_conf;
	wifi_scan_time_t *p_a_st = &p_a->scan_time;

	RPC_TEMPLATE(RpcRespWifiScanStart, resp_wifi_scan_start,
			RpcReqWifiScanStart, req_wifi_scan_start,
			rpc__resp__wifi_scan_start__init);

	p_c = req_payload->config;

	if (!req_payload->config || !req_payload->config_set) {
		p_a = NULL;
	} else {
		//RPC_REQ_COPY_STR(p_a->ssid, p_c->ssid, SSID_LENGTH);
		//RPC_REQ_COPY_STR(p_a->bssid, p_c->ssid, MAC_SIZE_BYTES);

		/* Note these are only pointers, not allocating memory for that */
		if (p_c->ssid.len)
			p_a->ssid = p_c->ssid.data;
		if (p_c->bssid.len)
			p_a->bssid = p_c->bssid.data;

		p_a->channel = p_c->channel;
		p_a->show_hidden = p_c->show_hidden;
		p_a->scan_type = p_c->scan_type;

		p_c_st = p_c->scan_time;

		p_a_st->passive = p_c_st->passive;
		p_a_st->active.min = p_c_st->active->min ;
		p_a_st->active.max = p_c_st->active->max ;

		p_a->home_chan_dwell_time = p_c->home_chan_dwell_time;

		if (p_c->channel_bitmap) {
			p_a->channel_bitmap.ghz_2_channels = p_c->channel_bitmap->ghz_2_channels;
			p_a->channel_bitmap.ghz_5_channels = p_c->channel_bitmap->ghz_5_channels;
		}
	}

	RPC_RET_FAIL_IF(esp_wifi_scan_start(p_a, req_payload->block));

	return ESP_OK;
}



static esp_err_t req_wifi_set_protocol(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespWifiSetProtocol, resp_wifi_set_protocol,
			RpcReqWifiSetProtocol, req_wifi_set_protocol,
			rpc__resp__wifi_set_protocol__init);

	RPC_RET_FAIL_IF(esp_wifi_set_protocol(req_payload->ifx,
			req_payload->protocol_bitmap));
	return ESP_OK;
}

static esp_err_t req_wifi_get_protocol(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespWifiGetProtocol, resp_wifi_get_protocol,
			RpcReqWifiGetProtocol, req_wifi_get_protocol,
			rpc__resp__wifi_get_protocol__init);

	uint8_t protocol_bitmap = 0;
	RPC_RET_FAIL_IF(esp_wifi_get_protocol(req_payload->ifx, &protocol_bitmap));

	resp_payload->protocol_bitmap = protocol_bitmap;
	return ESP_OK;
}

static esp_err_t req_wifi_scan_stop(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE_SIMPLE(RpcRespWifiScanStop, resp_wifi_scan_stop,
			RpcReqWifiScanStop, req_wifi_scan_stop,
			rpc__resp__wifi_scan_stop__init);

	RPC_RET_FAIL_IF(esp_wifi_scan_stop());
	return ESP_OK;
}

static esp_err_t req_wifi_scan_get_ap_num(Rpc *req, Rpc *resp, void *priv_data)
{
	uint16_t number = 0;
	int ret = 0;

	RPC_TEMPLATE_SIMPLE(RpcRespWifiScanGetApNum, resp_wifi_scan_get_ap_num,
			RpcReqWifiScanGetApNum, req_wifi_scan_get_ap_num,
			rpc__resp__wifi_scan_get_ap_num__init);

	ret = esp_wifi_scan_get_ap_num(&number);
	RPC_RET_FAIL_IF(ret);

	resp_payload->number = number;

	return ESP_OK;
}

// function only copies data: any required memory in the rpc struct must be allocated already
static int copy_ap_record_to_rpc_struct(WifiApRecord *rpc, wifi_ap_record_t *scan)
{
	ESP_LOGD(TAG, "Ssid: %s, Bssid: " MACSTR, scan->ssid, MAC2STR(scan->bssid));
	ESP_LOGD(TAG, "Primary: %u Second: %u Rssi: %d Authmode: %u",
		scan->primary, scan->second,
		scan->rssi, scan->authmode
		);
	ESP_LOGD(TAG, "PairwiseCipher: %u Groupcipher: %u Ant: %u",
		scan->pairwise_cipher, scan->group_cipher,
		scan->ant
		);
	ESP_LOGD(TAG, "Bitmask: 11b:%u g:%u n:%u a:%u ac:%u ax:%u lr:%u wps:%u ftm_resp:%u ftm_ini:%u res: %u",
		scan->phy_11b, scan->phy_11g, scan->phy_11n,
		scan->phy_11a, scan->phy_11ac, scan->phy_11ax,
		scan->phy_lr,
		scan->wps, scan->ftm_responder,
		scan->ftm_initiator, scan->reserved
		);
	RPC_COPY_STR(rpc->ssid, scan->ssid, SSID_LENGTH);
	RPC_COPY_BYTES(rpc->bssid, scan->bssid, BSSID_BYTES_SIZE);
	rpc->primary         = scan->primary;
	rpc->second          = scan->second;
	rpc->rssi            = scan->rssi;
	rpc->authmode        = scan->authmode;
	rpc->pairwise_cipher = scan->pairwise_cipher;
	rpc->group_cipher    = scan->group_cipher;
	rpc->ant             = scan->ant;

	/*Bitmask*/
	if (scan->phy_11b)
		H_SET_BIT(WIFI_SCAN_AP_REC_phy_11b_BIT,rpc->bitmask);

	if (scan->phy_11g)
		H_SET_BIT(WIFI_SCAN_AP_REC_phy_11g_BIT,rpc->bitmask);

	if (scan->phy_11n)
		H_SET_BIT(WIFI_SCAN_AP_REC_phy_11n_BIT,rpc->bitmask);

	if (scan->phy_lr)
		H_SET_BIT(WIFI_SCAN_AP_REC_phy_lr_BIT,rpc->bitmask);

	if (scan->phy_11a)
		H_SET_BIT(WIFI_SCAN_AP_REC_phy_11a_BIT,rpc->bitmask);

	if (scan->phy_11ac)
		H_SET_BIT(WIFI_SCAN_AP_REC_phy_11ac_BIT,rpc->bitmask);

	if (scan->phy_11ax)
		H_SET_BIT(WIFI_SCAN_AP_REC_phy_11ax_BIT,rpc->bitmask);

	if (scan->wps)
		H_SET_BIT(WIFI_SCAN_AP_REC_wps_BIT,rpc->bitmask);

	if (scan->ftm_responder)
		H_SET_BIT(WIFI_SCAN_AP_REC_ftm_responder_BIT,rpc->bitmask);

	if (scan->ftm_initiator)
		H_SET_BIT(WIFI_SCAN_AP_REC_ftm_initiator_BIT,rpc->bitmask);

	WIFI_SCAN_AP_SET_RESERVED_VAL(scan->reserved, rpc->bitmask);

	/* country */
	RPC_COPY_BYTES(rpc->country->cc, scan->country.cc, sizeof(scan->country.cc));
	rpc->country->schan        = scan->country.schan;
	rpc->country->nchan        = scan->country.nchan;
	rpc->country->max_tx_power = scan->country.max_tx_power;
	rpc->country->policy       = scan->country.policy;

	ESP_LOGD(TAG, "Country cc:%c%c schan: %u nchan: %u max_tx_pow: %d policy: %u",
		scan->country.cc[0], scan->country.cc[1], scan->country.schan, scan->country.nchan,
		scan->country.max_tx_power, scan->country.policy);

	/* he_ap */
	WifiHeApInfo * p_c_he_ap = rpc->he_ap;
	wifi_he_ap_info_t * p_a_he_ap = &scan->he_ap;

	// bss_color uses six bits
	p_c_he_ap->bitmask = (p_a_he_ap->bss_color & WIFI_HE_AP_INFO_BSS_COLOR_BITS);

	if (p_a_he_ap->partial_bss_color)
		H_SET_BIT(WIFI_HE_AP_INFO_partial_bss_color_BIT,p_c_he_ap->bitmask);

	if (p_a_he_ap->bss_color_disabled)
		H_SET_BIT(WIFI_HE_AP_INFO_bss_color_disabled_BIT,p_c_he_ap->bitmask);

	p_c_he_ap->bssid_index = p_a_he_ap->bssid_index;

	ESP_LOGD(TAG, "HE_AP: bss_color %d, partial_bss_color %d, bss_color_disabled %d",
		p_a_he_ap->bss_color, p_a_he_ap->bss_color_disabled, p_a_he_ap->bss_color_disabled);

	rpc->bandwidth    = scan->bandwidth;
	rpc->vht_ch_freq1 = scan->vht_ch_freq1;
	rpc->vht_ch_freq2 = scan->vht_ch_freq2;

	return 0;
}

static esp_err_t req_wifi_scan_get_ap_record(Rpc *req, Rpc *resp, void *priv_data)
{
	int ret = 0;
	wifi_ap_record_t *p_a_ap = NULL;

	RPC_TEMPLATE_SIMPLE(RpcRespWifiScanGetApRecord, resp_wifi_scan_get_ap_record,
			RpcReqWifiScanGetApRecord, req_wifi_scan_get_ap_record,
			rpc__resp__wifi_scan_get_ap_record__init);

	p_a_ap = (wifi_ap_record_t *)calloc(1, sizeof(wifi_ap_record_t));
	RPC_RET_FAIL_IF(!p_a_ap);

	ret = esp_wifi_scan_get_ap_record(p_a_ap);
	if (ret) {
		ESP_LOGE(TAG,"failed to get ap record");
		resp_payload->resp = ret;
		goto err;
	}

	RPC_ALLOC_ELEMENT(WifiApRecord, resp_payload->ap_record, wifi_ap_record__init);
	RPC_ALLOC_ELEMENT(WifiCountry, resp_payload->ap_record->country, wifi_country__init);
	RPC_ALLOC_ELEMENT(WifiHeApInfo, resp_payload->ap_record->he_ap, wifi_he_ap_info__init);

	ret = copy_ap_record_to_rpc_struct(resp_payload->ap_record, p_a_ap);
	if (ret) {
		ESP_LOGE(TAG, "failed to copy ap record to rpc struct");
		resp_payload->resp = ret;
	}

err:
	mem_free(p_a_ap);
	return ESP_OK;
}

static esp_err_t req_wifi_scan_get_ap_records(Rpc *req, Rpc *resp, void *priv_data)
{
	uint16_t number = 0;
	uint16_t ap_count = 0;
	int ret = 0;
	uint16_t i;

	wifi_ap_record_t *p_a_ap_list = NULL;

	RPC_TEMPLATE_SIMPLE(RpcRespWifiScanGetApRecords, resp_wifi_scan_get_ap_records,
			RpcReqWifiScanGetApRecords, req_wifi_scan_get_ap_records,
			rpc__resp__wifi_scan_get_ap_records__init);

	number = req->req_wifi_scan_get_ap_records->number;
	ESP_LOGD(TAG,"n_elem_scan_list predicted: %u\n", number);

	p_a_ap_list = (wifi_ap_record_t *)calloc(number, sizeof(wifi_ap_record_t));
	RPC_RET_FAIL_IF(!p_a_ap_list);

	ret = esp_wifi_scan_get_ap_num(&ap_count);
	if (ret || !ap_count) {
		ESP_LOGE(TAG,"esp_wifi_scan_get_ap_num: ret: %d num_ap_scanned:%u", ret, number);
		goto err;
	}
	if (number < ap_count) {
		ESP_LOGI(TAG,"n_elem_scan_list wants to return: %u Limit to %u\n", ap_count, number);
	}

	ret = esp_wifi_scan_get_ap_records(&number, p_a_ap_list);
	if(ret) {
		ESP_LOGE(TAG,"Failed to scan ap records");
		goto err;
	}

	resp_payload->number = number;
	resp_payload->ap_records = (WifiApRecord**)calloc(number, sizeof(WifiApRecord *));
	if (!resp_payload->ap_records) {
		ESP_LOGE(TAG,"resp: malloc failed for resp_payload->ap_records");
		goto err;
	}

	for (i=0;i<number;i++) {
		ESP_LOGD(TAG, "ap_record[%u]:", i+1);
		RPC_ALLOC_ELEMENT(WifiApRecord, resp_payload->ap_records[i], wifi_ap_record__init);
		RPC_ALLOC_ELEMENT(WifiCountry, resp_payload->ap_records[i]->country, wifi_country__init);
		RPC_ALLOC_ELEMENT(WifiHeApInfo, resp_payload->ap_records[i]->he_ap, wifi_he_ap_info__init);

		ret = copy_ap_record_to_rpc_struct(resp_payload->ap_records[i], &p_a_ap_list[i]);
		if (ret) {
			ESP_LOGE(TAG, "failed to copy ap record to rpc struct");
			resp_payload->resp = ret;
			goto err;
		}

		/* increment num of records in rpc msg */
		resp_payload->n_ap_records++;
	}

err:
	mem_free(p_a_ap_list);
	return ESP_OK;
}

static esp_err_t req_wifi_clear_ap_list(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE_SIMPLE(RpcRespWifiClearApList, resp_wifi_clear_ap_list,
			RpcReqWifiClearApList, req_wifi_clear_ap_list,
			rpc__resp__wifi_clear_ap_list__init);

	RPC_RET_FAIL_IF(esp_wifi_clear_ap_list());
	return ESP_OK;
}

static esp_err_t req_wifi_restore(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE_SIMPLE(RpcRespWifiRestore, resp_wifi_restore,
			RpcReqWifiRestore, req_wifi_restore,
			rpc__resp__wifi_restore__init);

	RPC_RET_FAIL_IF(esp_wifi_restore());
	return ESP_OK;
}

static esp_err_t req_wifi_clear_fast_connect(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE_SIMPLE(RpcRespWifiClearFastConnect, resp_wifi_clear_fast_connect,
			RpcReqWifiClearFastConnect, req_wifi_clear_fast_connect,
			rpc__resp__wifi_clear_fast_connect__init);

	RPC_RET_FAIL_IF(esp_wifi_clear_fast_connect());
	return ESP_OK;
}

static esp_err_t req_wifi_sta_get_ap_info(Rpc *req, Rpc *resp, void *priv_data)
{
	int ret = 0;
	wifi_ap_record_t p_a_ap_info = {0};

	RPC_TEMPLATE_SIMPLE(RpcRespWifiStaGetApInfo, resp_wifi_sta_get_ap_info,
			RpcReqWifiStaGetApInfo, req_wifi_sta_get_ap_info,
			rpc__resp__wifi_sta_get_ap_info__init);


	RPC_RET_FAIL_IF(esp_wifi_sta_get_ap_info(&p_a_ap_info));
	RPC_ALLOC_ELEMENT(WifiApRecord, resp_payload->ap_record, wifi_ap_record__init);
	RPC_ALLOC_ELEMENT(WifiCountry, resp_payload->ap_record->country, wifi_country__init);
	RPC_ALLOC_ELEMENT(WifiHeApInfo, resp_payload->ap_record->he_ap, wifi_he_ap_info__init);

	ret = copy_ap_record_to_rpc_struct(resp_payload->ap_record, &p_a_ap_info);
	if (ret) {
		ESP_LOGE(TAG, "failed to copy ap info to rpc struct");
		resp_payload->resp = ret;
	}
err:
	return ESP_OK;
}


static esp_err_t req_wifi_deauth_sta(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespWifiDeauthSta, resp_wifi_deauth_sta,
			RpcReqWifiDeauthSta, req_wifi_deauth_sta,
			rpc__resp__wifi_deauth_sta__init);

	RPC_RET_FAIL_IF(esp_wifi_deauth_sta(req_payload->aid));
	return ESP_OK;
}

static esp_err_t req_wifi_set_storage(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespWifiSetStorage, resp_wifi_set_storage,
			RpcReqWifiSetStorage, req_wifi_set_storage,
			rpc__resp__wifi_set_storage__init);

	RPC_RET_FAIL_IF(esp_wifi_set_storage(req_payload->storage));
	return ESP_OK;
}

static esp_err_t req_wifi_set_bandwidth(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespWifiSetBandwidth, resp_wifi_set_bandwidth,
			RpcReqWifiSetBandwidth, req_wifi_set_bandwidth,
			rpc__resp__wifi_set_bandwidth__init);

	RPC_RET_FAIL_IF(esp_wifi_set_bandwidth(req_payload->ifx, req_payload->bw));
	return ESP_OK;
}

static esp_err_t req_wifi_get_bandwidth(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespWifiGetBandwidth, resp_wifi_get_bandwidth,
			RpcReqWifiGetBandwidth, req_wifi_get_bandwidth,
			rpc__resp__wifi_get_bandwidth__init);

	wifi_bandwidth_t bw = 0;
	RPC_RET_FAIL_IF(esp_wifi_get_bandwidth(req_payload->ifx, &bw));

	resp_payload->bw = bw;
	return ESP_OK;
}

static esp_err_t req_wifi_set_channel(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespWifiSetChannel, resp_wifi_set_channel,
			RpcReqWifiSetChannel, req_wifi_set_channel,
			rpc__resp__wifi_set_channel__init);

	RPC_RET_FAIL_IF(esp_wifi_set_channel(req_payload->primary, req_payload->second));
	return ESP_OK;
}

static esp_err_t req_wifi_get_channel(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE_SIMPLE(RpcRespWifiGetChannel, resp_wifi_get_channel,
			RpcReqWifiGetChannel, req_wifi_get_channel,
			rpc__resp__wifi_get_channel__init);

	uint8_t primary = 0;
	wifi_second_chan_t second = 0;
	RPC_RET_FAIL_IF(esp_wifi_get_channel(&primary, &second));

	resp_payload->primary = primary;
	resp_payload->second = second;
	return ESP_OK;
}

static esp_err_t req_wifi_set_country_code(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespWifiSetCountryCode, resp_wifi_set_country_code,
			RpcReqWifiSetCountryCode, req_wifi_set_country_code,
			rpc__resp__wifi_set_country_code__init);

	char cc[3] = {0}; // country code
	RPC_RET_FAIL_IF(!req_payload->country.data);
	RPC_REQ_COPY_STR(&cc[0], req_payload->country, 2); // only copy the first two chars

	RPC_RET_FAIL_IF(esp_wifi_set_country_code(&cc[0],
			req_payload->ieee80211d_enabled));

	return ESP_OK;
}

static esp_err_t req_wifi_get_country_code(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE_SIMPLE(RpcRespWifiGetCountryCode, resp_wifi_get_country_code,
			RpcReqWifiGetCountryCode, req_wifi_get_country_code,
			rpc__resp__wifi_get_country_code__init);

	char cc[3] = {0}; // country code
	RPC_RET_FAIL_IF(esp_wifi_get_country_code(&cc[0]));

	RPC_RESP_COPY_STR(resp_payload->country, &cc[0], sizeof(cc));

	return ESP_OK;
}

static esp_err_t req_wifi_set_country(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespWifiSetCountry, resp_wifi_set_country,
			RpcReqWifiSetCountry, req_wifi_set_country,
			rpc__resp__wifi_set_country__init);

	RPC_RET_FAIL_IF(!req_payload->country);

	wifi_country_t country = {0};
	WifiCountry * p_c_country = req_payload->country;
	RPC_REQ_COPY_BYTES(&country.cc[0], p_c_country->cc, sizeof(country.cc));
	country.schan        = p_c_country->schan;
	country.nchan        = p_c_country->nchan;
	country.max_tx_power = p_c_country->max_tx_power;
	country.policy       = p_c_country->policy;

	RPC_RET_FAIL_IF(esp_wifi_set_country(&country));

	return ESP_OK;
}

static esp_err_t req_wifi_get_country(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE_SIMPLE(RpcRespWifiGetCountry, resp_wifi_get_country,
			RpcReqWifiGetCountry, req_wifi_get_country,
			rpc__resp__wifi_get_country__init);

	wifi_country_t country = {0};
	RPC_RET_FAIL_IF(esp_wifi_get_country(&country));

	RPC_ALLOC_ELEMENT(WifiCountry, resp_payload->country, wifi_country__init);
	WifiCountry * p_c_country = resp_payload->country;
	RPC_RESP_COPY_BYTES(p_c_country->cc, &country.cc[0], sizeof(country.cc));
	p_c_country->schan        = country.schan;
	p_c_country->nchan        = country.nchan;
	p_c_country->max_tx_power = country.max_tx_power;
	p_c_country->policy       = country.policy;

err:
	return ESP_OK;
}

static esp_err_t req_wifi_ap_get_sta_list(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE_SIMPLE(RpcRespWifiApGetStaList, resp_wifi_ap_get_sta_list,
			RpcReqWifiApGetStaList, req_wifi_ap_get_sta_list,
			rpc__resp__wifi_ap_get_sta_list__init);

	wifi_sta_list_t sta;
	RPC_RET_FAIL_IF(esp_wifi_ap_get_sta_list(&sta));

	RPC_ALLOC_ELEMENT(WifiStaList, resp_payload->sta_list, wifi_sta_list__init);
	WifiStaList * p_c_sta_list = resp_payload->sta_list;

	resp_payload->sta_list->sta = (WifiStaInfo**)calloc(ESP_WIFI_MAX_CONN_NUM, sizeof(WifiStaInfo *));
	if (!resp_payload->sta_list->sta) {
		ESP_LOGE(TAG,"resp: malloc failed for resp_payload->sta_list->sta");
		goto err;
	}

	for (int i = 0; i < ESP_WIFI_MAX_CONN_NUM; i++) {
		RPC_ALLOC_ELEMENT(WifiStaInfo, p_c_sta_list->sta[i], wifi_sta_info__init);
		WifiStaInfo * p_c_sta_info = p_c_sta_list->sta[i];

		RPC_RESP_COPY_BYTES(p_c_sta_info->mac, &sta.sta[i].mac[0], sizeof(sta.sta[i].mac));
		p_c_sta_info->rssi = sta.sta[i].rssi;

		if (sta.sta[i].phy_11b)
			H_SET_BIT(WIFI_STA_INFO_phy_11b_BIT, p_c_sta_info->bitmask);

		if (sta.sta[i].phy_11g)
			H_SET_BIT(WIFI_STA_INFO_phy_11g_BIT, p_c_sta_info->bitmask);

		if (sta.sta[i].phy_11n)
			H_SET_BIT(WIFI_STA_INFO_phy_11n_BIT, p_c_sta_info->bitmask);

		if (sta.sta[i].phy_lr)
			H_SET_BIT(WIFI_STA_INFO_phy_lr_BIT, p_c_sta_info->bitmask);

		if (sta.sta[i].phy_11ax)
			H_SET_BIT(WIFI_STA_INFO_phy_11ax_BIT, p_c_sta_info->bitmask);

		if (sta.sta[i].is_mesh_child)
			H_SET_BIT(WIFI_STA_INFO_is_mesh_child_BIT, p_c_sta_info->bitmask);

		WIFI_STA_INFO_SET_RESERVED_VAL(sta.sta[i].reserved, p_c_sta_info->bitmask);
	}
	// number of sta records in the list
	resp_payload->sta_list->n_sta = ESP_WIFI_MAX_CONN_NUM;

	p_c_sta_list->num = sta.num;

err:
	return ESP_OK;
}

static esp_err_t req_wifi_ap_get_sta_aid(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespWifiApGetStaAid, resp_wifi_ap_get_sta_aid,
			RpcReqWifiApGetStaAid, req_wifi_ap_get_sta_aid,
			rpc__resp__wifi_ap_get_sta_aid__init);

	uint8_t mac[6];
	uint16_t aid;

	RPC_REQ_COPY_BYTES(mac, req_payload->mac, sizeof(mac));
	ESP_LOGI(TAG, "mac: " MACSTR, MAC2STR(mac));
	RPC_RET_FAIL_IF(esp_wifi_ap_get_sta_aid(mac, &aid));

	resp_payload->aid = aid;

	return ESP_OK;
}

static esp_err_t req_wifi_sta_get_rssi(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE_SIMPLE(RpcRespWifiStaGetRssi, resp_wifi_sta_get_rssi,
			RpcReqWifiStaGetRssi, req_wifi_sta_get_rssi,
			rpc__resp__wifi_sta_get_rssi__init);

	int rssi;
	RPC_RET_FAIL_IF(esp_wifi_sta_get_rssi(&rssi));

	resp_payload->rssi = rssi;

	return ESP_OK;
}

static esp_err_t req_wifi_sta_get_aid(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE_SIMPLE(RpcRespWifiStaGetAid, resp_wifi_sta_get_aid,
			RpcReqWifiStaGetAid, req_wifi_sta_get_aid,
			rpc__resp__wifi_sta_get_aid__init);

	uint16_t aid;
	RPC_RET_FAIL_IF(esp_wifi_sta_get_aid(&aid));

	resp_payload->aid = aid;

	return ESP_OK;
}

static esp_err_t req_wifi_sta_get_negotiated_phymode(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE_SIMPLE(RpcRespWifiStaGetNegotiatedPhymode, resp_wifi_sta_get_negotiated_phymode,
			RpcReqWifiStaGetNegotiatedPhymode, req_wifi_sta_get_netogitated_phymode,
			rpc__resp__wifi_sta_get_negotiated_phymode__init);

	wifi_phy_mode_t phymode;
	RPC_RET_FAIL_IF(esp_wifi_sta_get_negotiated_phymode(&phymode));

	resp_payload->phymode = phymode;

	return ESP_OK;
}

static esp_err_t req_wifi_set_protocols(Rpc *req, Rpc *resp, void *priv_data)
{
#if H_PRESENT_IN_ESP_IDF_5_4_0
	RPC_TEMPLATE(RpcRespWifiSetProtocols, resp_wifi_set_protocols,
			RpcReqWifiSetProtocols, req_wifi_set_protocols,
			rpc__resp__wifi_set_protocols__init);

	RPC_RET_FAIL_IF(!req_payload->protocols);

	wifi_interface_t ifx;
	ifx = req_payload->ifx;
	resp_payload->ifx = ifx;

	wifi_protocols_t protocols;
	protocols.ghz_2g = req_payload->protocols->ghz_2g;
	protocols.ghz_5g = req_payload->protocols->ghz_5g;

	ESP_LOGI(TAG, "set protocols: ghz_2g %d, ghz_5g %d", protocols.ghz_2g, protocols.ghz_5g);

	RPC_RET_FAIL_IF(esp_wifi_set_protocols(ifx, &protocols));

	return ESP_OK;
#else
	return ESP_FAIL;
#endif
}

static esp_err_t req_wifi_get_protocols(Rpc *req, Rpc *resp, void *priv_data)
{
#if H_PRESENT_IN_ESP_IDF_5_4_0
	RPC_TEMPLATE(RpcRespWifiGetProtocols, resp_wifi_get_protocols,
			RpcReqWifiGetProtocols, req_wifi_get_protocols,
			rpc__resp__wifi_get_protocols__init);

	wifi_interface_t ifx;
	ifx = req_payload->ifx;
	resp_payload->ifx = ifx;

	wifi_protocols_t protocols;

	RPC_RET_FAIL_IF(esp_wifi_get_protocols(ifx, &protocols));

	RPC_ALLOC_ELEMENT(WifiProtocols, resp_payload->protocols, wifi_protocols__init);
	resp_payload->protocols->ghz_2g = protocols.ghz_2g;
	resp_payload->protocols->ghz_5g = protocols.ghz_5g;

	ESP_LOGI(TAG, "get protocols: ghz_2g %d, ghz_5g %d", protocols.ghz_2g, protocols.ghz_5g);
err:
	return ESP_OK;
#else
	return ESP_FAIL;
#endif

}

static esp_err_t req_wifi_set_bandwidths(Rpc *req, Rpc *resp, void *priv_data)
{
#if H_PRESENT_IN_ESP_IDF_5_4_0
	RPC_TEMPLATE(RpcRespWifiSetBandwidths, resp_wifi_set_bandwidths,
			RpcReqWifiSetBandwidths, req_wifi_set_bandwidths,
			rpc__resp__wifi_set_bandwidths__init);

	RPC_RET_FAIL_IF(!req_payload->bandwidths);

	wifi_interface_t ifx;
	ifx = req_payload->ifx;
	resp_payload->ifx = ifx;

	wifi_bandwidths_t bw;

	bw.ghz_2g = req_payload->bandwidths->ghz_2g;
	bw.ghz_5g = req_payload->bandwidths->ghz_5g;

	ESP_LOGI(TAG, "set bandwidths: ghz_2g %d, ghz_5g %d", bw.ghz_2g, bw.ghz_5g);

	RPC_RET_FAIL_IF(esp_wifi_set_bandwidths(ifx, &bw));

	return ESP_OK;
#else
	return ESP_FAIL;
#endif
}

static esp_err_t req_wifi_get_bandwidths(Rpc *req, Rpc *resp, void *priv_data)
{
#if H_PRESENT_IN_ESP_IDF_5_4_0
	RPC_TEMPLATE(RpcRespWifiGetBandwidths, resp_wifi_get_bandwidths,
			RpcReqWifiGetBandwidths, req_wifi_get_bandwidths,
			rpc__resp__wifi_get_bandwidths__init);

	wifi_interface_t ifx;
	ifx = req_payload->ifx;
	resp_payload->ifx = ifx;

	wifi_bandwidths_t bw;

	RPC_RET_FAIL_IF(esp_wifi_get_bandwidths(ifx, &bw));

	RPC_ALLOC_ELEMENT(WifiBandwidths, resp_payload->bandwidths, wifi_bandwidths__init);

	resp_payload->bandwidths->ghz_2g = bw.ghz_2g;
	resp_payload->bandwidths->ghz_5g = bw.ghz_5g;

	ESP_LOGI(TAG, "get bandwidths: ghz_2g %d, ghz_5g %d", bw.ghz_2g, bw.ghz_5g);
err:
	return ESP_OK;
#else
	return ESP_FAIL;
#endif
}

static esp_err_t req_wifi_set_band(Rpc *req, Rpc *resp, void *priv_data)
{
#if H_PRESENT_IN_ESP_IDF_5_4_0
	RPC_TEMPLATE(RpcRespWifiSetBand, resp_wifi_set_band,
			RpcReqWifiSetBand, req_wifi_set_band,
			rpc__resp__wifi_set_band__init);

	wifi_band_t band;
	band = req_payload->band;

	ESP_LOGW(TAG, "set band: %d", band);

	RPC_RET_FAIL_IF(esp_wifi_set_band(band));

	return ESP_OK;
#else
	return ESP_FAIL;
#endif
}

static esp_err_t req_wifi_get_band(Rpc *req, Rpc *resp, void *priv_data)
{
#if H_PRESENT_IN_ESP_IDF_5_4_0
	RPC_TEMPLATE_SIMPLE(RpcRespWifiGetBand, resp_wifi_get_band,
			RpcReqWifiGetBand, req_wifi_get_band,
			rpc__resp__wifi_get_band__init);

	wifi_band_t band;
	RPC_RET_FAIL_IF(esp_wifi_get_band(&band));

	resp_payload->band = band;

	ESP_LOGW(TAG, "get band: %d", band);

	return ESP_OK;
#else
	return ESP_FAIL;
#endif
}

static esp_err_t req_wifi_set_band_mode(Rpc *req, Rpc *resp, void *priv_data)
{
#if H_PRESENT_IN_ESP_IDF_5_4_0
	RPC_TEMPLATE(RpcRespWifiSetBandMode, resp_wifi_set_bandmode,
			RpcReqWifiSetBandMode, req_wifi_set_bandmode,
			rpc__resp__wifi_set_band_mode__init);

	wifi_band_mode_t band_mode;
	band_mode = req_payload->bandmode;

	ESP_LOGW(TAG, "set band_mode: %d", band_mode);

	RPC_RET_FAIL_IF(esp_wifi_set_band_mode(band_mode));

	return ESP_OK;
#else
	return ESP_FAIL;
#endif
}

static esp_err_t req_wifi_get_band_mode(Rpc *req, Rpc *resp, void *priv_data)
{
#if H_PRESENT_IN_ESP_IDF_5_4_0
	RPC_TEMPLATE_SIMPLE(RpcRespWifiGetBandMode, resp_wifi_get_bandmode,
			RpcReqWifiGetBandMode, req_wifi_get_bandmode,
			rpc__resp__wifi_get_band_mode__init);

	wifi_band_mode_t band_mode;
	RPC_RET_FAIL_IF(esp_wifi_get_band_mode(&band_mode));

	resp_payload->bandmode = band_mode;

	ESP_LOGW(TAG, "get band_mode: %d", band_mode);

	return ESP_OK;
#else
	return ESP_FAIL;
#endif
}

static esp_err_t req_get_coprocessor_fw_version(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE_SIMPLE(RpcRespGetCoprocessorFwVersion, resp_get_coprocessor_fwversion,
			RpcReqGetCoprocessorFwVersion, req_get_coprocessor_fwversion,
			rpc__resp__get_coprocessor_fw_version__init);

	resp_payload->major1 = PROJECT_VERSION_MAJOR_1;
	resp_payload->minor1 = PROJECT_VERSION_MINOR_1;
	resp_payload->patch1 = PROJECT_VERSION_PATCH_1;
	resp_payload->resp = ESP_OK;

	return ESP_OK;
}

#if 0
static esp_err_t req_wifi_(Rpc *req, Rpc *resp, void *priv_data)
{
	RPC_TEMPLATE(RpcRespWifi, resp_wifi_,
			RpcReqWifi, req_wifi_,
			rpc__resp__wifi___init);

	RPC_RET_FAIL_IF(esp_wifi_(&cfg));

	return ESP_OK;
}
#endif

static esp_rpc_req_t req_table[] = {
	{
		.req_num = RPC_ID__Req_GetMACAddress ,
		.command_handler = req_wifi_get_mac
	},
	{
		.req_num = RPC_ID__Req_GetWifiMode,
		.command_handler = req_wifi_get_mode
	},
	{
		.req_num = RPC_ID__Req_SetWifiMode,
		.command_handler = req_wifi_set_mode
	},
	{
		.req_num = RPC_ID__Req_SetMacAddress,
		.command_handler = req_wifi_set_mac
	},
	{
		.req_num = RPC_ID__Req_WifiSetPs,
		.command_handler = req_wifi_set_ps
	},
	{
		.req_num = RPC_ID__Req_WifiGetPs,
		.command_handler = req_wifi_get_ps
	},
	{
		.req_num = RPC_ID__Req_OTABegin,
		.command_handler = req_ota_begin_handler
	},
	{
		.req_num = RPC_ID__Req_OTAWrite,
		.command_handler = req_ota_write_handler
	},
	{
		.req_num = RPC_ID__Req_OTAEnd,
		.command_handler = req_ota_end_handler
	},
	{
		.req_num = RPC_ID__Req_WifiSetMaxTxPower,
		.command_handler = req_wifi_set_max_tx_power
	},
	{
		.req_num = RPC_ID__Req_WifiGetMaxTxPower,
		.command_handler = req_wifi_get_max_tx_power
	},
	{
		.req_num = RPC_ID__Req_ConfigHeartbeat,
		.command_handler = req_config_heartbeat
	},
	{
		.req_num = RPC_ID__Req_WifiInit,
		.command_handler = req_wifi_init
	},
	{
		.req_num = RPC_ID__Req_WifiDeinit,
		.command_handler = req_wifi_deinit
	},
	{
		.req_num = RPC_ID__Req_WifiStart,
		.command_handler = req_wifi_start
	},
	{
		.req_num = RPC_ID__Req_WifiStop,
		.command_handler = req_wifi_stop
	},
	{
		.req_num = RPC_ID__Req_WifiConnect,
		.command_handler = req_wifi_connect
	},
	{
		.req_num = RPC_ID__Req_WifiDisconnect,
		.command_handler = req_wifi_disconnect
	},
	{
		.req_num = RPC_ID__Req_WifiSetConfig,
		.command_handler = req_wifi_set_config
	},
	{
		.req_num = RPC_ID__Req_WifiGetConfig,
		.command_handler = req_wifi_get_config
	},
	{
		.req_num = RPC_ID__Req_WifiScanStart,
		.command_handler = req_wifi_scan_start
	},
	{
		.req_num = RPC_ID__Req_WifiScanStop,
		.command_handler = req_wifi_scan_stop
	},
	{
		.req_num = RPC_ID__Req_WifiScanGetApNum,
		.command_handler = req_wifi_scan_get_ap_num
	},
	{
		.req_num = RPC_ID__Req_WifiScanGetApRecord,
		.command_handler = req_wifi_scan_get_ap_record
	},
	{
		.req_num = RPC_ID__Req_WifiScanGetApRecords,
		.command_handler = req_wifi_scan_get_ap_records
	},
	{
		.req_num = RPC_ID__Req_WifiClearApList,
		.command_handler = req_wifi_clear_ap_list
	},
	{
		.req_num = RPC_ID__Req_WifiRestore,
		.command_handler = req_wifi_restore
	},
	{
		.req_num = RPC_ID__Req_WifiClearFastConnect,
		.command_handler = req_wifi_clear_fast_connect
	},
	{
		.req_num = RPC_ID__Req_WifiStaGetApInfo,
		.command_handler = req_wifi_sta_get_ap_info
	},
	{
		.req_num = RPC_ID__Req_WifiDeauthSta,
		.command_handler = req_wifi_deauth_sta
	},
	{
		.req_num = RPC_ID__Req_WifiSetStorage,
		.command_handler = req_wifi_set_storage
	},
	{
		.req_num = RPC_ID__Req_WifiSetProtocol,
		.command_handler = req_wifi_set_protocol
	},
	{
		.req_num = RPC_ID__Req_WifiGetProtocol,
		.command_handler = req_wifi_get_protocol
	},
	{
		.req_num = RPC_ID__Req_WifiSetBandwidth,
		.command_handler = req_wifi_set_bandwidth
	},
	{
		.req_num = RPC_ID__Req_WifiGetBandwidth,
		.command_handler = req_wifi_get_bandwidth
	},
	{
		.req_num = RPC_ID__Req_WifiSetChannel,
		.command_handler = req_wifi_set_channel
	},
	{
		.req_num = RPC_ID__Req_WifiGetChannel,
		.command_handler = req_wifi_get_channel
	},
	{
		.req_num = RPC_ID__Req_WifiSetCountryCode,
		.command_handler = req_wifi_set_country_code
	},
	{
		.req_num = RPC_ID__Req_WifiGetCountryCode,
		.command_handler = req_wifi_get_country_code
	},
	{
		.req_num = RPC_ID__Req_WifiSetCountry,
		.command_handler = req_wifi_set_country
	},
	{
		.req_num = RPC_ID__Req_WifiGetCountry,
		.command_handler = req_wifi_get_country
	},
	{
		.req_num = RPC_ID__Req_WifiApGetStaList,
		.command_handler = req_wifi_ap_get_sta_list
	},
	{
		.req_num = RPC_ID__Req_WifiApGetStaAid,
		.command_handler = req_wifi_ap_get_sta_aid
	},
	{
		.req_num = RPC_ID__Req_WifiStaGetRssi,
		.command_handler = req_wifi_sta_get_rssi
	},
	{
		.req_num = RPC_ID__Req_WifiStaGetAid,
		.command_handler = req_wifi_sta_get_aid
	},
	{
		.req_num = RPC_ID__Req_WifiStaGetNegotiatedPhymode,
		.command_handler = req_wifi_sta_get_negotiated_phymode
	},
	{
		.req_num = RPC_ID__Req_WifiSetProtocols,
		.command_handler = req_wifi_set_protocols
	},
	{
		.req_num = RPC_ID__Req_WifiGetProtocols,
		.command_handler = req_wifi_get_protocols
	},
	{
		.req_num = RPC_ID__Req_WifiSetBandwidths,
		.command_handler = req_wifi_set_bandwidths
	},
	{
		.req_num = RPC_ID__Req_WifiGetBandwidths,
		.command_handler = req_wifi_get_bandwidths
	},
	{
		.req_num = RPC_ID__Req_WifiSetBand,
		.command_handler = req_wifi_set_band
	},
	{
		.req_num = RPC_ID__Req_WifiGetBand,
		.command_handler = req_wifi_get_band
	},
	{
		.req_num = RPC_ID__Req_WifiSetBandMode,
		.command_handler = req_wifi_set_band_mode
	},
	{
		.req_num = RPC_ID__Req_WifiGetBandMode,
		.command_handler = req_wifi_get_band_mode
	},
	{
		.req_num = RPC_ID__Req_GetCoprocessorFwVersion,
		.command_handler = req_get_coprocessor_fw_version
	},
};


static int lookup_req_handler(int req_id)
{
	for (int i = 0; i < sizeof(req_table)/sizeof(esp_rpc_req_t); i++) {
		if (req_table[i].req_num == req_id) {
			return i;
		}
	}
	return -1;
}

static esp_err_t esp_rpc_command_dispatcher(
		Rpc *req, Rpc *resp,
		void *priv_data)
{
	esp_err_t ret = ESP_OK;
	int req_index = 0;

	if (!req || !resp) {
		ESP_LOGE(TAG, "Invalid parameters in command");
		return ESP_FAIL;
	}

	if ((req->msg_id <= RPC_ID__Req_Base) ||
		(req->msg_id >= RPC_ID__Req_Max)) {
		ESP_LOGE(TAG, "Invalid command request lookup");
	}

	ESP_LOGI(TAG, "Received Req [0x%x]", req->msg_id);

	req_index = lookup_req_handler(req->msg_id);
	if (req_index < 0) {
		ESP_LOGE(TAG, "Invalid command handler lookup");
		return ESP_FAIL;
	}

	ret = req_table[req_index].command_handler(req, resp, priv_data);
	if (ret) {
		ESP_LOGE(TAG, "Error executing command handler");
		return ESP_FAIL;
	}

	return ESP_OK;
}

/* use rpc__free_unpacked to free memory
 * For RPC structure to be freed correctly with no memory leaks:
 * - n_xxx must be set to number of 'repeated xxx' structures in RPC msg
 * - xxx_case must be set for 'oneof xxx' structures in RPC msg
 * - xxx.len must be set for 'bytes xxx' or 'string xxx' in RPC msg
 */
static void esp_rpc_cleanup(Rpc *resp)
{
	if (resp) {
		rpc__free_unpacked(resp, NULL);
	}
}

esp_err_t data_transfer_handler(uint32_t session_id,const uint8_t *inbuf,
		ssize_t inlen, uint8_t **outbuf, ssize_t *outlen, void *priv_data)
{
	Rpc *req = NULL;
	esp_err_t ret = ESP_OK;

	Rpc *resp = (Rpc *)calloc(1, sizeof(Rpc)); // resp deallocated in esp_rpc_cleanup()
	if (!resp) {
		ESP_LOGE(TAG, "%s calloc failed", __func__);
		return ESP_FAIL;
	}

	if (!inbuf || !outbuf || !outlen) {
		ESP_LOGE(TAG,"Buffers are NULL");
		return ESP_FAIL;
	}

	req = rpc__unpack(NULL, inlen, inbuf);
	if (!req) {
		ESP_LOGE(TAG, "Unable to unpack config data");
		return ESP_FAIL;
	}

	rpc__init (resp);
	resp->msg_type = RPC_TYPE__Resp;
	resp->msg_id = req->msg_id - RPC_ID__Req_Base + RPC_ID__Resp_Base;
	resp->uid = req->uid;
	resp->payload_case = resp->msg_id;
	ESP_LOGI(TAG, "Resp_MSGId for req[0x%x] is [0x%x], uid %ld", req->msg_id, resp->msg_id, resp->uid);
	ret = esp_rpc_command_dispatcher(req,resp,NULL);
	if (ret) {
		ESP_LOGE(TAG, "Command dispatching not happening");
		goto err;
	}

	rpc__free_unpacked(req, NULL);

	*outlen = rpc__get_packed_size (resp);
	if (*outlen <= 0) {
		ESP_LOGE(TAG, "Invalid encoding for response");
		goto err;
	}

	*outbuf = (uint8_t *)calloc(1, *outlen);
	if (!*outbuf) {
		ESP_LOGE(TAG, "No memory allocated for outbuf");
		esp_rpc_cleanup(resp);
		return ESP_ERR_NO_MEM;
	}

	rpc__pack (resp, *outbuf);

	//printf("Resp outbuf:\n");
	//ESP_LOG_BUFFER_HEXDUMP("Resp outbuf", *outbuf, *outlen, ESP_LOG_INFO);

	esp_rpc_cleanup(resp);
	return ESP_OK;

err:
	esp_rpc_cleanup(resp);
	return ESP_FAIL;
}

/* Function ESPInit Notification */
static esp_err_t rpc_evt_ESPInit(Rpc *ntfy)
{
	RpcEventESPInit *ntfy_payload = NULL;

	ESP_LOGI(TAG,"event ESPInit");
	ntfy_payload = (RpcEventESPInit *)
		calloc(1,sizeof(RpcEventESPInit));
	if (!ntfy_payload) {
		ESP_LOGE(TAG,"Failed to allocate memory");
		return ESP_ERR_NO_MEM;
	}
	rpc__event__espinit__init(ntfy_payload);
	ntfy->payload_case = RPC__PAYLOAD_EVENT_ESP_INIT;
	ntfy->event_esp_init = ntfy_payload;

	return ESP_OK;
}

static esp_err_t rpc_evt_heartbeat(Rpc *ntfy)
{
	RpcEventHeartbeat *ntfy_payload = NULL;


	ntfy_payload = (RpcEventHeartbeat*)
		calloc(1,sizeof(RpcEventHeartbeat));
	if (!ntfy_payload) {
		ESP_LOGE(TAG,"Failed to allocate memory");
		return ESP_ERR_NO_MEM;
	}
	rpc__event__heartbeat__init(ntfy_payload);

	ntfy_payload->hb_num = hb_num;

	ntfy->payload_case = RPC__PAYLOAD_EVENT_HEARTBEAT;
	ntfy->event_heartbeat = ntfy_payload;

	return ESP_OK;

}

static esp_err_t rpc_evt_sta_scan_done(Rpc *ntfy,
		const uint8_t *data, ssize_t len, int event_id)
{
	WifiEventStaScanDone *p_c_scan = NULL;
	wifi_event_sta_scan_done_t * p_a = (wifi_event_sta_scan_done_t*)data;

	NTFY_TEMPLATE(RPC_ID__Event_StaScanDone,
			RpcEventStaScanDone, event_sta_scan_done,
			rpc__event__sta_scan_done__init);

	NTFY_ALLOC_ELEMENT(WifiEventStaScanDone, ntfy_payload->scan_done,
			wifi_event_sta_scan_done__init);
	p_c_scan = ntfy_payload->scan_done;

	p_c_scan->status = p_a->status;
	p_c_scan->number = p_a->number;
	p_c_scan->scan_id = p_a->scan_id;

err:
	return ESP_OK;
}

static esp_err_t rpc_evt_sta_connected(Rpc *ntfy,
		const uint8_t *data, ssize_t len, int event_id)
{
	WifiEventStaConnected *p_c = NULL;
	wifi_event_sta_connected_t * p_a = (wifi_event_sta_connected_t*)data;

	NTFY_TEMPLATE(RPC_ID__Event_StaConnected,
			RpcEventStaConnected, event_sta_connected,
			rpc__event__sta_connected__init);

	NTFY_ALLOC_ELEMENT(WifiEventStaConnected, ntfy_payload->sta_connected,
			wifi_event_sta_connected__init);

	p_c = ntfy_payload->sta_connected;

	NTFY_COPY_BYTES(p_c->ssid, p_a->ssid, sizeof(p_a->ssid));

	p_c->ssid_len = p_a->ssid_len;

	NTFY_COPY_BYTES(p_c->bssid, p_a->bssid, sizeof(p_a->bssid));

	p_c->channel = p_a->channel;
	p_c->authmode = p_a->authmode;
	p_c->aid = p_a->aid;

err:
	return ESP_OK;
}

static esp_err_t rpc_evt_sta_disconnected(Rpc *ntfy,
		const uint8_t *data, ssize_t len, int event_id)
{
	WifiEventStaDisconnected *p_c = NULL;
	wifi_event_sta_disconnected_t * p_a = (wifi_event_sta_disconnected_t*)data;

	NTFY_TEMPLATE(RPC_ID__Event_StaDisconnected,
			RpcEventStaDisconnected, event_sta_disconnected,
			rpc__event__sta_disconnected__init);

	NTFY_ALLOC_ELEMENT(WifiEventStaDisconnected, ntfy_payload->sta_disconnected,
			wifi_event_sta_disconnected__init);

	p_c = ntfy_payload->sta_disconnected;

	NTFY_COPY_BYTES(p_c->ssid, p_a->ssid, sizeof(p_a->ssid));

	p_c->ssid_len = p_a->ssid_len;

	NTFY_COPY_BYTES(p_c->bssid, p_a->bssid, sizeof(p_a->bssid));

	p_c->reason = p_a->reason;
	p_c->rssi = p_a->rssi;

err:
	return ESP_OK;
}

static esp_err_t rpc_evt_ap_staconn_conn_disconn(Rpc *ntfy,
		const uint8_t *data, ssize_t len, int event_id)
{
	ESP_LOGD(TAG, "%s event:%u",__func__,event_id);

	if (event_id == WIFI_EVENT_AP_STACONNECTED) {
		NTFY_TEMPLATE(RPC_ID__Event_AP_StaConnected,
				RpcEventAPStaConnected, event_ap_sta_connected,
				rpc__event__ap__sta_connected__init);

		wifi_event_ap_staconnected_t * p_a = (wifi_event_ap_staconnected_t *)data;

		NTFY_COPY_BYTES(ntfy_payload->mac, p_a->mac, sizeof(p_a->mac));

		ntfy_payload->aid = p_a->aid;
		ntfy_payload->is_mesh_child = p_a->is_mesh_child;

		return ESP_OK;

	} else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
		NTFY_TEMPLATE(RPC_ID__Event_AP_StaDisconnected,
				RpcEventAPStaDisconnected, event_ap_sta_disconnected,
				rpc__event__ap__sta_disconnected__init);

		wifi_event_ap_stadisconnected_t * p_a = (wifi_event_ap_stadisconnected_t *)data;

		NTFY_COPY_BYTES(ntfy_payload->mac, p_a->mac, sizeof(p_a->mac));

		ntfy_payload->aid = p_a->aid;
		ntfy_payload->is_mesh_child = p_a->is_mesh_child;
		ntfy_payload->reason = p_a->reason;

		return ESP_OK;
	}
	return ESP_FAIL;
}

static esp_err_t rpc_evt_Event_WifiEventNoArgs(Rpc *ntfy,
		const uint8_t *data, ssize_t len)
{
	NTFY_TEMPLATE(RPC_ID__Event_WifiEventNoArgs,
			RpcEventWifiEventNoArgs, event_wifi_event_no_args,
			rpc__event__wifi_event_no_args__init);

	int32_t event_id = (int32_t)*data;
	ESP_LOGI(TAG, "Sending Wi-Fi event [%ld]", event_id);

	ntfy_payload->event_id = event_id;

	return ESP_OK;
}

esp_err_t rpc_evt_handler(uint32_t session_id,const uint8_t *inbuf,
		ssize_t inlen, uint8_t **outbuf, ssize_t *outlen, void *priv_data)
{
	int ret = SUCCESS;

	Rpc *ntfy = (Rpc *)calloc(1, sizeof(Rpc)); // ntfy deallocated in esp_rpc_cleanup()
	if (!ntfy) {
		ESP_LOGE(TAG, "%s calloc failed", __func__);
		return ESP_FAIL;
	}

	if (!outbuf || !outlen) {
		ESP_LOGE(TAG,"Buffers are NULL");
		return ESP_FAIL;
	}

	rpc__init (ntfy);
	ntfy->msg_id = session_id;
	ntfy->msg_type = RPC_TYPE__Event;

	switch ((int)ntfy->msg_id) {
		case RPC_ID__Event_ESPInit : {
			ret = rpc_evt_ESPInit(ntfy);
			break;
		} case RPC_ID__Event_Heartbeat: {
			ret = rpc_evt_heartbeat(ntfy);
			break;
		} case RPC_ID__Event_AP_StaConnected: {
			ret = rpc_evt_ap_staconn_conn_disconn(ntfy, inbuf, inlen, WIFI_EVENT_AP_STACONNECTED);
			break;
		} case RPC_ID__Event_AP_StaDisconnected: {
			ret = rpc_evt_ap_staconn_conn_disconn(ntfy, inbuf, inlen, WIFI_EVENT_AP_STADISCONNECTED);
			break;
		} case RPC_ID__Event_StaScanDone: {
			ret = rpc_evt_sta_scan_done(ntfy, inbuf, inlen, WIFI_EVENT_SCAN_DONE);
			break;
		} case RPC_ID__Event_StaConnected: {
			ret = rpc_evt_sta_connected(ntfy, inbuf, inlen, WIFI_EVENT_STA_CONNECTED);
			break;
		} case RPC_ID__Event_StaDisconnected: {
			ret = rpc_evt_sta_disconnected(ntfy, inbuf, inlen, WIFI_EVENT_STA_DISCONNECTED);
			break;
		} case RPC_ID__Event_WifiEventNoArgs: {
			ret = rpc_evt_Event_WifiEventNoArgs(ntfy, inbuf, inlen);
			break;
		} default: {
			ESP_LOGE(TAG, "Incorrect/unsupported Ctrl Notification[%u]\n",ntfy->msg_id);
			goto err;
			break;
		}
	}

	if (ret) {
		ESP_LOGI(TAG, "notification[%u] not sent\n", ntfy->msg_id);
		goto err;
	}

	*outlen = rpc__get_packed_size (ntfy);
	if (*outlen <= 0) {
		ESP_LOGE(TAG, "Invalid encoding for notify");
		goto err;
	}

	*outbuf = (uint8_t *)calloc(1, *outlen);
	if (!*outbuf) {
		ESP_LOGE(TAG, "No memory allocated for outbuf");
		esp_rpc_cleanup(ntfy);
		return ESP_ERR_NO_MEM;
	}

	rpc__pack (ntfy, *outbuf);

	//printf("event outbuf:\n");
	//ESP_LOG_BUFFER_HEXDUMP("event outbuf", *outbuf, *outlen, ESP_LOG_INFO);

	esp_rpc_cleanup(ntfy);
	return ESP_OK;

err:
	if (!*outbuf) {
		free(*outbuf);
		*outbuf = NULL;
	}
	esp_rpc_cleanup(ntfy);
	return ESP_FAIL;
}
