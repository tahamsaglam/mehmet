#ifndef __MEHMET_VERSION_H__
#define __MEHMET_VERSION_H__

	/* Read GPL2[0],[1],[2],[3] Data Register
	 * -----------------------------------------------------------------
	 * VER | ADC3_HW_VER0 | ADC2_HW_VER0 | ADC1_HW_VER0 | ADC0_HW_VER0 |
	 * -----------------------------------------------------------------
	 * DVT |       0      |       0      |       0      |       0      | //0
	 * -----------------------------------------------------------------
	 * MVT |       0      |       0      |       0      |       1      | //1
	 * -----------------------------------------------------------------
	 * PP  |       0      |       0      |       1      |       0      | //2
	 * -----------------------------------------------------------------
	 * MP  |       0      |       0      |       1      |       1      | //3
	 * -----------------------------------------------------------------
	 * MP2 |       0      |       1      |       0      |       0      | //4
 	 * -----------------------------------------------------------------
	 * MP3 |       0      |       1      |       0      |       1      | //5
 	 * -----------------------------------------------------------------
	 * MP4 |       0      |       1      |       1      |       0      | //6
	 * -----------------------------------------------------------------
	 * VE1 |       0      |       1      |       1      |       1      | //7
	 * -----------------------------------------------------------------
	 * VE3 |       1      |       1      |       0      |       1      | //13
	 * -----------------------------------------------------------------
	 * VE4 |       1      |       1      |       1      |       0      | //14
	 * -----------------------------------------------------------------
	 * VE2 |       1      |       1      |       1      |       1      | //15
	 * -----------------------------------------------------------------
	 */

typedef enum {
	MEHMET_HW_DVT = 0,
	MEHMET_HW_MVT = 1,
	MEHMET_HW_PP = 2,
	MEHMET_HW_MP = 3,
	MEHMET_HW_MP2 = 4,
	MEHMET_HW_MP3 = 5,
	MEHMET_HW_MP4 = 6,
	MEHMET_HW_VE1 = 7,
	MEHMET_HW_RESERVED0 = 8,
	MEHMET_HW_RESERVED1 = 9,
	MEHMET_HW_RESERVED2 = 10,
	MEHMET_HW_RESERVED3 = 11,
	MEHMET_HW_RESERVED4 = 12,
	MEHMET_HW_VE4 = 13,
	MEHMET_HW_VE3 = 14,
	MEHMET_HW_VE2 = 15,
	MEHMET_HW_UNKNOWN = 16,
} MEHMET_HW_VERSION;

extern MEHMET_HW_VERSION g_mehmet_hw_version;

#endif /*__MEHMET_VERSION_H__*/
