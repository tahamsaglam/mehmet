/*
 *  linux/arch/arm/mach-exynos/mehmet-gpio.c
 *
 * Copyright (c) 2010 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com/
 *
 * EXYNOS - GPIO setting in set board
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/gpio.h>
#include <linux/serial_core.h>
#include <plat/devs.h>
#include <plat/gpio-cfg.h>
#include <plat/regs-serial.h>
#include <mach/gpio.h>
#include <plat/cpu.h>
#include <mach/pmu.h>
#include <mach/mehmet_version.h>
#include <mach/gpio-mehmet.h>
struct gpio_init_data {
	uint num;
	uint cfg;
	uint val;
	uint pud;
	uint drv;
};

extern int s3c_gpio_slp_cfgpin(unsigned int pin, unsigned int config);
extern int s3c_gpio_slp_setpull_updown(unsigned int pin, unsigned int config);
extern MEHMET_HW_VERSION mehmet_get_hw_version( void );

/* this is sample code for mehmet board */
static struct gpio_init_data mehmet_init_gpios[] = {
#if 0
	{EXYNOS4_GPD0(2), S3C_GPIO_INPUT, S3C_GPIO_SETPIN_NONE,
		S3C_GPIO_PULL_NONE, S5P_GPIO_DRVSTR_LV1}, /* CODEC_SDA_1.8V */
	{EXYNOS4_GPD0(3), S3C_GPIO_INPUT, S3C_GPIO_SETPIN_NONE,
		S3C_GPIO_PULL_NONE, S5P_GPIO_DRVSTR_LV1}, /* CODEC_SCL_1.8V */
	{EXYNOS4_GPD1(0), S3C_GPIO_INPUT, S3C_GPIO_SETPIN_NONE,
		S3C_GPIO_PULL_NONE, S5P_GPIO_DRVSTR_LV1}, /* NFC_SDA_1.8V */
	{EXYNOS4_GPD1(1), S3C_GPIO_INPUT, S3C_GPIO_SETPIN_NONE,
		S3C_GPIO_PULL_NONE, S5P_GPIO_DRVSTR_LV1}, /* NFC_SCL_1.8V */
	{EXYNOS4_GPD1(2), S3C_GPIO_INPUT, S3C_GPIO_SETPIN_NONE,
		S3C_GPIO_PULL_NONE, S5P_GPIO_DRVSTR_LV1}, /* GSENSE_SDA_1.8V */
	{EXYNOS4_GPD1(3), S3C_GPIO_INPUT, S3C_GPIO_SETPIN_NONE,
		S3C_GPIO_PULL_NONE, S5P_GPIO_DRVSTR_LV1}, /* GSENSE_SCL_1.8V */

	{EXYNOS4_GPX0(1), S3C_GPIO_INPUT, S3C_GPIO_SETPIN_NONE,
		S3C_GPIO_PULL_NONE, S5P_GPIO_DRVSTR_LV1}, /* EAR_DET */
	{EXYNOS4_GPX0(2), S3C_GPIO_INPUT, S3C_GPIO_SETPIN_NONE,
		S3C_GPIO_PULL_NONE, S5P_GPIO_DRVSTR_LV1}, /* PS_ALS_INT */
	{EXYNOS4_GPX0(7), S3C_GPIO_SFN(0xF), S3C_GPIO_SETPIN_NONE,
		S3C_GPIO_PULL_NONE, S5P_GPIO_DRVSTR_LV1}, /* AP_PMIC_IRQ */

	{EXYNOS4_GPX1(7), S3C_GPIO_INPUT, S3C_GPIO_SETPIN_NONE,
		S3C_GPIO_PULL_NONE, S5P_GPIO_DRVSTR_LV1}, /* NFC_IRQ */

	{EXYNOS4_GPX2(0), S3C_GPIO_SFN(2), S3C_GPIO_SETPIN_NONE,
		S3C_GPIO_PULL_NONE, S5P_GPIO_DRVSTR_LV1}, /* VOL_UP */
	{EXYNOS4_GPX2(1), S3C_GPIO_SFN(2), S3C_GPIO_SETPIN_NONE,
		S3C_GPIO_PULL_NONE, S5P_GPIO_DRVSTR_LV1}, /* VOL_DOWN */
	{EXYNOS4_GPX2(3), S3C_GPIO_INPUT, S3C_GPIO_SETPIN_NONE,
		S3C_GPIO_PULL_NONE, S5P_GPIO_DRVSTR_LV1}, /* FUEL_ALERT */
	{EXYNOS4_GPX2(4), S3C_GPIO_INPUT, S3C_GPIO_SETPIN_NONE,
		S3C_GPIO_PULL_DOWN, S5P_GPIO_DRVSTR_LV1}, /* V_BUS_INT */
	{EXYNOS4_GPX2(5), S3C_GPIO_INPUT, S3C_GPIO_SETPIN_NONE,
		S3C_GPIO_PULL_NONE, S5P_GPIO_DRVSTR_LV1}, /* WLAN_HOST_WAKEUP */
	{EXYNOS4_GPX2(6), S3C_GPIO_INPUT, S3C_GPIO_SETPIN_NONE,
		S3C_GPIO_PULL_NONE, S5P_GPIO_DRVSTR_LV1}, /* BT_HOST_WAKEUP */

	{EXYNOS4_GPX3(0), S3C_GPIO_INPUT, S3C_GPIO_SETPIN_NONE,
		S3C_GPIO_PULL_NONE, S5P_GPIO_DRVSTR_LV1}, /* WPC_INT */
	{EXYNOS4_GPX3(1), S3C_GPIO_INPUT, S3C_GPIO_SETPIN_NONE,
		S3C_GPIO_PULL_NONE, S5P_GPIO_DRVSTR_LV1}, /* BT_WAKE */
	{EXYNOS4212_GPJ0(0), S3C_GPIO_INPUT, S3C_GPIO_SETPIN_ZERO,
		S3C_GPIO_PULL_NONE, S5P_GPIO_DRVSTR_LV4}, /* WLAN_EN */
	{EXYNOS4_GPK3(1), S3C_GPIO_INPUT, S3C_GPIO_SETPIN_NONE,
		S3C_GPIO_PULL_NONE, S5P_GPIO_DRVSTR_LV1}, /* WLAN_SDIO_CMD */
	{EXYNOS4_GPK3(3), S3C_GPIO_INPUT, S3C_GPIO_SETPIN_NONE,
		S3C_GPIO_PULL_NONE, S5P_GPIO_DRVSTR_LV1}, /* WLAN_SDIO_D(0) */
	{EXYNOS4_GPK3(4), S3C_GPIO_INPUT, S3C_GPIO_SETPIN_NONE,
		S3C_GPIO_PULL_NONE, S5P_GPIO_DRVSTR_LV1}, /* WLAN_SDIO_D(1) */
	{EXYNOS4_GPK3(5), S3C_GPIO_INPUT, S3C_GPIO_SETPIN_NONE,
		S3C_GPIO_PULL_NONE, S5P_GPIO_DRVSTR_LV1}, /* WLAN_SDIO_D(2) */
	{EXYNOS4_GPK3(6), S3C_GPIO_INPUT, S3C_GPIO_SETPIN_NONE,
		S3C_GPIO_PULL_NONE, S5P_GPIO_DRVSTR_LV1}, /* WLAN_SDIO_D(3) */
#endif
};

/* this table only for mehmet board */
static unsigned int exynos4_sleep_gpio_table_common[][3] = {
	{EXYNOS4_GPA0(0),  S3C_GPIO_SLP_PREV, S3C_GPIO_PULL_NONE},		// BT_RXD, keep previous state
	{EXYNOS4_GPA0(1),  S3C_GPIO_SLP_PREV, S3C_GPIO_PULL_NONE},		// BT_TXD, keep previous state
	{EXYNOS4_GPA0(2),  S3C_GPIO_SLP_PREV, S3C_GPIO_PULL_NONE},		// BT_CTS, keep previous state
	{EXYNOS4_GPA0(3),  S3C_GPIO_SLP_PREV, S3C_GPIO_PULL_NONE},		// BT_RTS, keep previous state
	{EXYNOS4_GPA0(4),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_UP},		// GPS_RXD
	{EXYNOS4_GPA0(5),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},		// GPS_TXD
	{EXYNOS4_GPA0(6),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_UP},		// GSENSOR_I2C_SDA
	{EXYNOS4_GPA0(7),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_UP},		// GSENSOR_I2C_SCL

	{EXYNOS4_GPA1(0),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_UP},		// DEBUG_UART_RXD
	{EXYNOS4_GPA1(1),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},		// DEBUG_UART_TXD
	{EXYNOS4_GPA1(2),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_UP},		// FUEL_I2C_SDA
	{EXYNOS4_GPA1(3),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_UP},		// FUEL_I2C_SCL
	{EXYNOS4_GPA1(4),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_UP},		// TEST_UART_RXD
	{EXYNOS4_GPA1(5),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},		// TEST_UART_TXD

	{EXYNOS4_GPB(0),   S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_UP},		// COMPASS_I2C_SDA
	{EXYNOS4_GPB(1),   S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_UP},		// COMPASS_I2C_SCL
	{EXYNOS4_GPB(2),   S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_UP},		// TSP_I2C_SDA
	{EXYNOS4_GPB(3),   S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_UP},		// TSP_I2C_SCL
	{EXYNOS4_GPB(4),   S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_UP},		// TSP_RESET
	{EXYNOS4_GPB(5),   S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},		// TSP_WAKE_UP
	{EXYNOS4_GPB(6),   S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_UP},		// HAPTIC_I2C_SDA
	{EXYNOS4_GPB(7),   S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_UP},		// HAPTIC_I2C_SCL

	{EXYNOS4_GPC0(0),  S3C_GPIO_SLP_OUT0, S3C_GPIO_PULL_NONE},		// BT_PCM_CLK
	{EXYNOS4_GPC0(1),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN}, 	/* NC */
	{EXYNOS4_GPC0(2),  S3C_GPIO_SLP_OUT0, S3C_GPIO_PULL_NONE},		// BT_PCM_SYNC
	{EXYNOS4_GPC0(3),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_NONE},		// BT_PCM_IN
	{EXYNOS4_GPC0(4),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_NONE},		// BT_PCM_OUT

	{EXYNOS4_GPC1(0),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},		/* NC */
	{EXYNOS4_GPC1(1),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN}, 	/* NC */
	{EXYNOS4_GPC1(2),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},		/* NC */
	{EXYNOS4_GPC1(3),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_UP},		// GYRO_I2C_SDA
	{EXYNOS4_GPC1(4),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_UP},		// GYRO_I2C_SCL

	{EXYNOS4_GPD0(0),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_DOWN}, 	// MOT_PWM
	{EXYNOS4_GPD0(1),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_DOWN},		// LCD_BL_PWM
	{EXYNOS4_GPD0(2),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_UP},		// LSENSOR_SDA
	{EXYNOS4_GPD0(3),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_UP},		// LSENSOR_SCL

	{EXYNOS4_GPD1(0),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_UP},		// PM_I2C_SDA
	{EXYNOS4_GPD1(1),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_UP},		// PM_I2C_SCL
	{EXYNOS4_GPD1(2),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_UP},		// CODEC_I2C_SDA
	{EXYNOS4_GPD1(3),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_UP},		// CODEC_I2C_SCL

	{EXYNOS4_GPF0(0),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_NONE},		// LCD
	{EXYNOS4_GPF0(1),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_NONE},
	{EXYNOS4_GPF0(2),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_NONE},
	{EXYNOS4_GPF0(2),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_NONE},
	{EXYNOS4_GPF0(3),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_NONE},
	{EXYNOS4_GPF0(4),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_NONE},
	{EXYNOS4_GPF0(5),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_NONE},
	{EXYNOS4_GPF0(6),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_NONE},
	{EXYNOS4_GPF0(7),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_NONE},

	{EXYNOS4_GPF1(0),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_NONE},
	{EXYNOS4_GPF1(1),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_NONE},
	{EXYNOS4_GPF1(2),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_NONE},
	{EXYNOS4_GPF1(3),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_NONE},
	{EXYNOS4_GPF1(4),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_NONE},
	{EXYNOS4_GPF1(5),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_NONE},
	{EXYNOS4_GPF1(6),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_NONE},
	{EXYNOS4_GPF1(6),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_NONE},
	{EXYNOS4_GPF1(7),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_NONE},

	{EXYNOS4_GPF2(0),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_NONE},
	{EXYNOS4_GPF2(1),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_NONE},
	{EXYNOS4_GPF2(2),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_NONE},
	{EXYNOS4_GPF2(3),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_NONE},
	{EXYNOS4_GPF2(4),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_NONE},
	{EXYNOS4_GPF2(5),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_NONE},
	{EXYNOS4_GPF2(5),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_NONE},
	{EXYNOS4_GPF2(6),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_NONE},
	{EXYNOS4_GPF2(7),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_NONE},

	{EXYNOS4_GPF3(0),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_NONE},
	{EXYNOS4_GPF3(1),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_NONE},
	{EXYNOS4_GPF3(2),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_NONE},
	{EXYNOS4_GPF3(3),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_NONE},
	{EXYNOS4_GPF3(4),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_NONE},
	{EXYNOS4_GPF3(5),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_NONE},

	{EXYNOS4_GPK0(0),  S3C_GPIO_SLP_OUT0, S3C_GPIO_PULL_DOWN},		// MOVI
	{EXYNOS4_GPK0(1),  S3C_GPIO_SLP_OUT0, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPK0(2),  S3C_GPIO_SLP_OUT0, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPK0(3),  S3C_GPIO_SLP_OUT0, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPK0(4),  S3C_GPIO_SLP_OUT0, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPK0(5),  S3C_GPIO_SLP_OUT0, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPK0(6),  S3C_GPIO_SLP_OUT0, S3C_GPIO_PULL_DOWN},

	{EXYNOS4_GPK1(0),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},		/* NC */
	{EXYNOS4_GPK1(1),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},		/* NC */
	{EXYNOS4_GPK1(2),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},		/* NC */
	{EXYNOS4_GPK1(3),  S3C_GPIO_SLP_OUT0, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPK1(4),  S3C_GPIO_SLP_OUT0, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPK1(5),  S3C_GPIO_SLP_OUT0, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPK1(6),  S3C_GPIO_SLP_OUT0, S3C_GPIO_PULL_DOWN},

	{EXYNOS4_GPK2(0),  S3C_GPIO_SLP_OUT0, S3C_GPIO_PULL_DOWN},		// SD
	{EXYNOS4_GPK2(1),  S3C_GPIO_SLP_OUT0, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPK2(2),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPK2(3),  S3C_GPIO_SLP_OUT0, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPK2(4),  S3C_GPIO_SLP_OUT0, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPK2(5),  S3C_GPIO_SLP_OUT0, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPK2(6),  S3C_GPIO_SLP_OUT0, S3C_GPIO_PULL_DOWN},

	{EXYNOS4_GPK3(0),  S3C_GPIO_SLP_PREV, S3C_GPIO_PULL_DOWN},      // WIFI, keep previous state
	{EXYNOS4_GPK3(1),  S3C_GPIO_SLP_PREV, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPK3(2),  S3C_GPIO_SLP_PREV, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPK3(3),  S3C_GPIO_SLP_PREV, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPK3(4),  S3C_GPIO_SLP_PREV, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPK3(5),  S3C_GPIO_SLP_PREV, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPK3(6),  S3C_GPIO_SLP_PREV, S3C_GPIO_PULL_DOWN},

	{EXYNOS4_GPL0(0),  S3C_GPIO_SLP_PREV,  S3C_GPIO_PULL_NONE},		// PMIC_SET2
	{EXYNOS4_GPL0(1),  S3C_GPIO_SLP_PREV,  S3C_GPIO_PULL_NONE},		// PMIC_SET3
	{EXYNOS4_GPL0(2),  S3C_GPIO_SLP_PREV,  S3C_GPIO_PULL_NONE},		// PMIC_SET4
	{EXYNOS4_GPL0(3),  S3C_GPIO_SLP_PREV,  S3C_GPIO_PULL_NONE},		// PM_DVS1
	{EXYNOS4_GPL0(4),  S3C_GPIO_SLP_PREV,  S3C_GPIO_PULL_NONE}, 	// PM_DVS2
	{EXYNOS4_GPL0(5),  S3C_GPIO_SLP_PREV,  S3C_GPIO_PULL_DOWN},		/* NC */
	{EXYNOS4_GPL0(6),  S3C_GPIO_SLP_PREV,  S3C_GPIO_PULL_NONE},		// PM_DVS3

	{EXYNOS4_GPL1(0),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},		/* NC */
	{EXYNOS4_GPL1(1),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},		/* NC */

	{EXYNOS4_GPL2(3),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},		/* NC */
	{EXYNOS4_GPL2(4),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN}, 	/* NC */
	{EXYNOS4_GPL2(5),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},		/* NC */
	{EXYNOS4_GPL2(6),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},		/* NC */
	{EXYNOS4_GPL2(7),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},		/* NC */

	{EXYNOS4_GPY0(0),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},		// EBI NC
	{EXYNOS4_GPY0(1),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPY0(2),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPY0(3),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPY0(4),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPY0(5),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},

	{EXYNOS4_GPY1(0),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPY1(1),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPY1(2),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPY1(3),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPY2(0),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPY2(1),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPY2(2),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPY2(3),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPY2(4),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPY2(5),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},

	{EXYNOS4_GPY3(0),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPY3(1),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPY3(2),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPY3(3),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPY3(4),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPY3(5),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPY3(6),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPY3(7),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},

	{EXYNOS4_GPY4(0),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPY4(1),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPY4(2),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPY4(3),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPY4(4),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPY4(5),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPY4(6),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPY4(7),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},

	{EXYNOS4_GPY5(0),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPY5(1),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPY5(2),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPY5(3),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPY5(4),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPY5(5),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPY5(6),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPY5(7),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},

	{EXYNOS4_GPY6(0),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPY6(1),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPY6(2),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPY6(3),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPY6(4),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPY6(5),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPY6(6),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPY6(7),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},

	{EXYNOS4_GPZ(0),   S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_DOWN},		// I2S
	{EXYNOS4_GPZ(1),   S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPZ(2),   S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPZ(3),   S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPZ(4),   S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_DOWN},
	{EXYNOS4_GPZ(5),   S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN}, 	/* NC */
	{EXYNOS4_GPZ(6),   S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN}, 	/* NC */
};

static unsigned int exynos4212_sleep_gpio_table[][3] = {
	{EXYNOS4212_GPJ0(0),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},		// CAM
	{EXYNOS4212_GPJ0(1),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4212_GPJ0(2),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4212_GPJ0(3),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4212_GPJ0(4),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4212_GPJ0(5),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4212_GPJ0(6),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4212_GPJ0(7),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},

	{EXYNOS4212_GPJ1(0),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4212_GPJ1(1),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4212_GPJ1(2),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4212_GPJ1(3),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4212_GPJ1(4),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},		/* NC */

	{EXYNOS4212_GPM0(1),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN}, 		// JIG_LED_ON
	{EXYNOS4212_GPM0(2),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},		// nCHARGE_EN
	{EXYNOS4212_GPM0(3),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_UP}, 		// LCD_OFF_CHG
	{EXYNOS4212_GPM0(4),  S3C_GPIO_SLP_PREV,  S3C_GPIO_PULL_NONE}, 		// WL_REG_ON, keep previous state
	{EXYNOS4212_GPM0(5),  S3C_GPIO_SLP_PREV,  S3C_GPIO_PULL_NONE}, 		// BT_REG_ON, keep previous state
	{EXYNOS4212_GPM0(6),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_NONE}, 		// EXT_PWM_REQ
	{EXYNOS4212_GPM0(7),  S3C_GPIO_SLP_PREV,  S3C_GPIO_PULL_NONE}, 		// BT_WAKE, keep previous state

	{EXYNOS4212_GPM1(0),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_DOWN}, 		// nLVDS_PDN
	{EXYNOS4212_GPM1(1),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN}, 		/* NC */
	{EXYNOS4212_GPM1(2),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_DOWN}, 		// SPK_AMP_OFF
	{EXYNOS4212_GPM1(3),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_DOWN}, 		// POP_DISABLE
	{EXYNOS4212_GPM1(4),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN}, 		// CAM_RESET
	{EXYNOS4212_GPM1(5),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_DOWN}, 		// CAM_STANDBY
	{EXYNOS4212_GPM1(6),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_DOWN},		// nUSB_DET_EN

	{EXYNOS4212_GPM3(0),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN}, 		/* NC */
	{EXYNOS4212_GPM3(1),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN}, 		/* NC */
	{EXYNOS4212_GPM3(3),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN}, 		/* NC */
	{EXYNOS4212_GPM3(4),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN}, 		/* NC */
	{EXYNOS4212_GPM3(5),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN}, 		/* NC */
	{EXYNOS4212_GPM3(6),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN}, 		/* NC */
	{EXYNOS4212_GPM3(7),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN}, 		/* NC */

	{EXYNOS4212_GPM4(0),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_UP}, 		// CAM_I2C_SCL
	{EXYNOS4212_GPM4(1),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_UP}, 		// CAM_I2C_SDA
	{EXYNOS4212_GPM4(2),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN}, 		/* NC */
	{EXYNOS4212_GPM4(3),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN}, 		/* NC */
	{EXYNOS4212_GPM4(4),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN}, 		/* NC */
	{EXYNOS4212_GPM4(5),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN}, 		/* NC */
	{EXYNOS4212_GPM4(6),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN}, 		/* NC */
	{EXYNOS4212_GPM4(7),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN}, 		/* NC */

	{EXYNOS4212_GPV0(0),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},		// C2C NC
	{EXYNOS4212_GPV0(1),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4212_GPV0(2),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4212_GPV0(3),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4212_GPV0(4),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4212_GPV0(5),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4212_GPV0(6),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4212_GPV0(7),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},

	{EXYNOS4212_GPV1(0),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4212_GPV1(1),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4212_GPV1(2),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4212_GPV1(3),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4212_GPV1(4),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4212_GPV1(5),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4212_GPV1(6),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4212_GPV1(7),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},

	{EXYNOS4212_GPV2(0),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4212_GPV2(1),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4212_GPV2(2),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4212_GPV2(3),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4212_GPV2(4),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4212_GPV2(5),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4212_GPV2(6),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4212_GPV2(7),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},

	{EXYNOS4212_GPV3(0),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4212_GPV3(1),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4212_GPV3(2),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4212_GPV3(3),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4212_GPV3(4),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4212_GPV3(5),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4212_GPV3(6),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4212_GPV3(7),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},

	{EXYNOS4212_GPV4(0),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
	{EXYNOS4212_GPV4(1),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},
};

static unsigned int mehmet_dvt_sleep_gpio_table[][3] = {
	{EXYNOS4_GPL2(0),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},		// HW_VER2 (0)
	{EXYNOS4_GPL2(1),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN}, 	// HW_VER1 (0)
	{EXYNOS4_GPL2(2),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN}, 	// HW_VER0 (0)

	{EXYNOS4212_GPM0(0),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN}, 		/* NC */

	{EXYNOS4212_GPM2(0),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_DOWN}, 		// KEY_LED_EN
	{EXYNOS4212_GPM2(1),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_NONE}, 		// WL_DEV_WAKE
	{EXYNOS4212_GPM2(2),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},		/* NC */
	{EXYNOS4212_GPM2(3),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_DOWN}, 		// TEST_PWM_DRV
	{EXYNOS4212_GPM2(4),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN}, 		/* NC */

	{EXYNOS4212_GPM3(2),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN}, 		/* NC */
	{EXYNOS4212_GPM3(3),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},		/* NC */
};

static unsigned int mehmet_mvt_sleep_gpio_table[][3] = {
	{EXYNOS4_GPL2(0),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},		// HW_VER2 (0)
	{EXYNOS4_GPL2(1),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN}, 	// HW_VER1 (0)
	{EXYNOS4_GPL2(2),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_UP}, 		// HW_VER0 (1)

	{EXYNOS4212_GPM0(0),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_DOWN}, 		//VDD_5V0_EN

	{EXYNOS4212_GPM2(0),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN}, 		/* NC */
	{EXYNOS4212_GPM2(1),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_NONE}, 		// WL_DEV_WAKE
	{EXYNOS4212_GPM2(2),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_DOWN},		// CAM_MCLK
	{EXYNOS4212_GPM2(3),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_DOWN}, 		// HUB_CONNECT
	{EXYNOS4212_GPM2(4),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_DOWN}, 		// HUB_RESET(N)

	{EXYNOS4212_GPM3(2),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_DOWN}, 		// HDMI_PWR_EN
	{EXYNOS4212_GPM3(3),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_DOWN},		// TSP_BOOST_EN
};

static unsigned int mehmet_pp_sleep_gpio_table[][3] = {
	{EXYNOS4_GPL2(0),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},		// HW_VER2 (0)
	{EXYNOS4_GPL2(1),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_UP}, 		// HW_VER1 (1)
	{EXYNOS4_GPL2(2),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN}, 	// HW_VER0 (0)

	{EXYNOS4212_GPM0(0),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_DOWN}, 		//VDD_5V0_EN

	{EXYNOS4212_GPM2(0),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN}, 		/* NC */
	{EXYNOS4212_GPM2(1),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_NONE}, 		// WL_DEV_WAKE
	{EXYNOS4212_GPM2(2),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_DOWN},		// CAM_MCLK
	{EXYNOS4212_GPM2(3),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_DOWN}, 		// HUB_CONNECT
	{EXYNOS4212_GPM2(4),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_DOWN}, 		// HUB_RESET(N)

	{EXYNOS4212_GPM3(2),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_DOWN}, 		// HDMI_PWR_EN
	{EXYNOS4212_GPM3(3),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_DOWN},		// TSP_BOOST_EN
};

static unsigned int mehmet_mp_sleep_gpio_table[][3] = {
	{EXYNOS4_GPL2(0),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN},		// HW_VER2 (0)
	{EXYNOS4_GPL2(1),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_UP}, 		// HW_VER1 (1)
	{EXYNOS4_GPL2(2),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_UP}, 	// HW_VER0 (1)

	{EXYNOS4212_GPM0(0),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_DOWN}, 		//VDD_5V0_EN

	{EXYNOS4212_GPM2(0),  S3C_GPIO_SLP_INPUT, S3C_GPIO_PULL_DOWN}, 		/* NC */
	{EXYNOS4212_GPM2(1),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_NONE}, 		// WL_DEV_WAKE
	{EXYNOS4212_GPM2(2),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_DOWN},		// CAM_MCLK
	{EXYNOS4212_GPM2(3),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_DOWN}, 		// HUB_CONNECT
	{EXYNOS4212_GPM2(4),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_DOWN}, 		// HUB_RESET(N)

	{EXYNOS4212_GPM3(2),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_DOWN}, 		// HDMI_PWR_EN
	{EXYNOS4212_GPM3(3),  S3C_GPIO_SLP_OUT0,  S3C_GPIO_PULL_DOWN},		// TSP_BOOST_EN
};


static void config_sleep_gpio_table(int array_size,
				    unsigned int (*gpio_table)[3])
{
	u32 i, gpio;

	for (i = 0; i < array_size; i++) {
		gpio = gpio_table[i][0];
		s3c_gpio_slp_cfgpin(gpio, gpio_table[i][1]);
		s3c_gpio_slp_setpull_updown(gpio, gpio_table[i][2]);
	}
}

/* To save power consumption, gpio pin set before enterling sleep */
void mehmet_config_sleep_gpio_table(void)
{
	config_sleep_gpio_table(ARRAY_SIZE(exynos4_sleep_gpio_table_common),
			exynos4_sleep_gpio_table_common);

	config_sleep_gpio_table(ARRAY_SIZE(exynos4212_sleep_gpio_table),
			exynos4212_sleep_gpio_table);

	switch(mehmet_get_hw_version()){
	case MEHMET_HW_DVT:
		config_sleep_gpio_table(ARRAY_SIZE(mehmet_dvt_sleep_gpio_table),
				mehmet_dvt_sleep_gpio_table);
		break;
	case MEHMET_HW_MVT:
		config_sleep_gpio_table(ARRAY_SIZE(mehmet_mvt_sleep_gpio_table),
				mehmet_mvt_sleep_gpio_table);
		break;
	case MEHMET_HW_PP:
		config_sleep_gpio_table(ARRAY_SIZE(mehmet_pp_sleep_gpio_table),
				mehmet_pp_sleep_gpio_table);
		break;
	case MEHMET_HW_MP:
	case MEHMET_HW_MP2:
	default:
		config_sleep_gpio_table(ARRAY_SIZE(mehmet_mp_sleep_gpio_table),
				mehmet_mp_sleep_gpio_table);
		break;
	}
}

/* Intialize gpio set in mehmet board */
void mehmet_config_gpio_table(void)
{
	u32 i, gpio;

	printk(KERN_DEBUG "%s\n", __func__);

	for (i = 0; i < ARRAY_SIZE(mehmet_init_gpios); i++) {
		gpio = mehmet_init_gpios[i].num;

		s3c_gpio_cfgpin(gpio, mehmet_init_gpios[i].cfg);
		s3c_gpio_setpull(gpio, mehmet_init_gpios[i].pud);

		if (mehmet_init_gpios[i].val != S3C_GPIO_SETPIN_NONE)
			gpio_set_value(gpio, mehmet_init_gpios[i].val);

		s5p_gpio_set_drvstr(gpio, mehmet_init_gpios[i].drv);
	}
	mehmet_config_sleep_gpio_table();
}
