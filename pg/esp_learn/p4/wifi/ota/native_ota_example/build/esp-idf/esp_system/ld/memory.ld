/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 *                    ESP32-P4 Linker Script Memory Layout
 * This file describes the memory layout (memory blocks) by virtual memory addresses.
 * This linker script is passed through the C preprocessor to include configuration options.
 * Please use preprocessor features sparingly!
 * Restrict to simple macros with numeric values, and/or #if/#endif blocks.
 */
/*
 * Automatically generated file. DO NOT EDIT.
 * Espressif IoT Development Framework (ESP-IDF) 6.0.0 Configuration Header
 */
       
/* List of deprecated options */
/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* CPU instruction prefetch padding size for flash mmap scenario */
/*
 * PMP region granularity size
 * Software may determine the PMP granularity by writing zero to pmp0cfg, then writing all ones
 * to pmpaddr0, then reading back pmpaddr0. If G is the index of the least-significant bit set,
 * the PMP granularity is 2^G+2 bytes.
 */
/* CPU instruction prefetch padding size for memory protection scenario */
/* Memory alignment size for PMS */
    /* rtc timer data (s_rtc_timer_retain_mem, see esp_clk.c files). For rtc_timer_data_in_rtc_mem section. */
/* If the cache size is less than 512KB, then there is a region of RAM
 * above the ROM-reserved region and below the start of the cache.
 */
MEMORY
{
  /**
   *  All these values assume the flash cache is on, and have the blocks this uses subtracted from the length
   *  of the various regions. The 'data access port' dram/drom regions map to the same iram/irom regions but
   *  are connected to the data port of the CPU and eg allow byte-wise access.
   */
  /* TCM */
  tcm_idram_seg (RX) : org = 0x30100000, len = 0x2000
  /* Flash mapped instruction data */
  irom_seg (RX) : org = 0x40000020, len = (0x10000 << 10) - 0x20
  /**
   * (0x20 offset above is a convenience for the app binary image generation.
   * Flash cache has 64KB pages. The .bin file which is flashed to the chip
   * has a 0x18 byte file header, and each segment has a 0x08 byte segment
   * header. Setting this offset makes it simple to meet the flash cache MMU's
   * constraint that (paddr % 64KB == vaddr % 64KB).)
   */
  /**
   * Shared data RAM, excluding memory reserved for ROM bss/data/stack.
   * Enabling Bluetooth & Trace Memory features in menuconfig will decrease the amount of RAM available.
   */
  sram_low (RWX) : org = 0x4FF00000, len = 0x4FF2CBD0 - 0x4FF00000
  sram_high (RW) : org = 0x4FF40000, len = 0x80000 - 0x20000
  /* Flash mapped constant data */
  drom_seg (R) : org = 0x40000020, len = (0x10000 << 10) - 0x20
  /* (See irom_seg for meaning of 0x20 offset in the above.) */
  /**
   * lp ram memory (RWX). Persists over deep sleep. // TODO: IDF-5667
   */
  lp_ram_seg(RW) : org = 0x50108000 + (0 + (24)), len = 0x8000 - (0 + (24))
  /* We reduced the size of lp_ram_seg by RESERVE_RTC_MEM value.
     It reserves the amount of LP memory that we use for this memory segment.
     This segment is intended for keeping:
       - (lower addr) rtc timer data (s_rtc_timer_retain_mem, see esp_clk.c files).
       - (higher addr) bootloader rtc data (s_bootloader_retain_mem, when a Kconfig option is on).
     The aim of this is to keep data that will not be moved around and have a fixed address.
     This segment is placed at the beginning of LP RAM, as the end of LP RAM is occupied by LP ROM stack/data
  */
  lp_reserved_seg(RW) : org = 0x50108000, len = (0 + (24))
  /* PSRAM seg */
  extern_ram_seg(RWX) : org = 0x48000000, len = (0x10000 << 10)
}
/* Heap ends at top of dram0_0_seg */
_heap_end = 0x50000000;
_data_seg_org = ORIGIN(rtc_data_seg);
/**
 *  The lines below define location alias for .rtc.data section
 *  P4 has no distinguished LP(RTC) fast and slow memory sections, instead, there is a unified LP_RAM section
 *  Thus, the following region segments are not configurable like on other targets
 */
REGION_ALIAS("rtc_iram_seg", lp_ram_seg );
REGION_ALIAS("rtc_data_seg", rtc_iram_seg );
REGION_ALIAS("rtc_slow_seg", rtc_iram_seg );
REGION_ALIAS("rtc_data_location", rtc_iram_seg );
REGION_ALIAS("rtc_reserved_seg", lp_reserved_seg );
  REGION_ALIAS("text_seg_low", irom_seg);
  REGION_ALIAS("rodata_seg_low", drom_seg);
  REGION_ALIAS("ext_ram_seg", extern_ram_seg);
/**
 *  If rodata default segment is placed in `drom_seg`, then flash's first rodata section must
 *  also be first in the segment.
 */
  ASSERT(_flash_rodata_dummy_start == ORIGIN(rodata_seg_low),
         ".flash_rodata_dummy section must be placed at the beginning of the rodata segment.")
