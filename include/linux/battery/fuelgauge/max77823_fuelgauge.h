/*
 * max77823_fuelgauge.h
 * Samsung MAX77823 Fuel Gauge Header
 *
 * Copyright (C) 2012 Samsung Electronics, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef __MAX17050_FUELGAUGE_H
#define __MAX17050_FUELGAUGE_H __FILE__

#if defined(ANDROID_ALARM_ACTIVATED)
#include <linux/android_alarm.h>
#endif

#include <linux/mfd/core.h>
#include <linux/mfd/max77823.h>
#include <linux/mfd/max77823-private.h>
#include <linux/regulator/machine.h>

/* Slave address should be shifted to the right 1bit.
 * R/W bit should NOT be included.
 */

#if defined(CONFIG_FUELGAUGE_MAX77823_VOLTAGE_TRACKING)
#define MAX77823_REG_STATUS		0x00
#define MAX77823_REG_VALRT_TH		0x01
#define MAX77823_REG_TALRT_TH		0x02
#define MAX77823_REG_SALRT_TH		0x03
#define MAX77823_REG_VCELL			0x09
#define MAX77823_REG_TEMPERATURE	0x08
#define MAX77823_REG_AVGVCELL		0x19
#define MAX77823_REG_CONFIG		0x1D
#define MAX77823_REG_VERSION		0x21
#define MAX77823_REG_LEARNCFG		0x28
#define MAX77823_REG_FILTERCFG	0x29
#define MAX77823_REG_MISCCFG		0x2B
#define MAX77823_REG_CGAIN		0x2E
#define MAX77823_REG_RCOMP		0x38
#define MAX77823_REG_VFOCV		0xFB
#define MAX77823_REG_SOC_VF		0xFF

struct battery_data_t {
	u8 *type_str;
};

struct sec_fg_info {
	bool dummy;
};

#endif

#if defined(CONFIG_FUELGAUGE_MAX77823_COULOMB_COUNTING)
#define PRINT_COUNT	10

#define LOW_BATT_COMP_RANGE_NUM	5
#define LOW_BATT_COMP_LEVEL_NUM	2
#define MAX_LOW_BATT_CHECK_CNT	10

enum {
	FG_LEVEL = 0,
	FG_TEMPERATURE,
	FG_VOLTAGE,
	FG_CURRENT,
	FG_CURRENT_AVG,
	FG_CHECK_STATUS,
	FG_RAW_SOC,
	FG_VF_SOC,
	FG_AV_SOC,
	FG_FULLCAP,
	FG_MIXCAP,
	FG_AVCAP,
	FG_REPCAP,
};

enum {
	POSITIVE = 0,
	NEGATIVE,
};

enum {
	RANGE = 0,
	SLOPE,
	OFFSET,
	TABLE_MAX
};

#define CURRENT_RANGE_MAX_NUM	5

struct battery_data_t {
	u32 Capacity;
	u32 low_battery_comp_voltage;
	s32 low_battery_table[CURRENT_RANGE_MAX_NUM][TABLE_MAX];
	u8	*type_str;
};

struct sec_fg_info {
	/* test print count */
	int pr_cnt;
	/* full charge comp */
	struct delayed_work	full_comp_work;
	u32 previous_fullcap;
	u32 previous_vffullcap;
	/* low battery comp */
	int low_batt_comp_cnt[LOW_BATT_COMP_RANGE_NUM][LOW_BATT_COMP_LEVEL_NUM];
	int low_batt_comp_flag;
	/* low battery boot */
	int low_batt_boot_flag;
	bool is_low_batt_alarm;

	/* battery info */
	u32 soc;

	/* miscellaneous */
	unsigned long fullcap_check_interval;
	int full_check_flag;
	bool is_first_check;
};

/* FullCap learning setting */
#define VFFULLCAP_CHECK_INTERVAL	300 /* sec */
/* soc should be 0.1% unit */
#define VFSOC_FOR_FULLCAP_LEARNING	950
#define LOW_CURRENT_FOR_FULLCAP_LEARNING	20
#define HIGH_CURRENT_FOR_FULLCAP_LEARNING	120
#define LOW_AVGCURRENT_FOR_FULLCAP_LEARNING	20
#define HIGH_AVGCURRENT_FOR_FULLCAP_LEARNING	100

/* power off margin */
/* soc should be 0.1% unit */
#define POWER_OFF_SOC_HIGH_MARGIN	20
#define POWER_OFF_VOLTAGE_HIGH_MARGIN	3500
#define POWER_OFF_VOLTAGE_LOW_MARGIN	3400

/* FG recovery handler */
/* soc should be 0.1% unit */
#define STABLE_LOW_BATTERY_DIFF	30
#define STABLE_LOW_BATTERY_DIFF_LOWBATT	10
#define LOW_BATTERY_SOC_REDUCE_UNIT	10
#endif

struct max77823_fuelgauge_data {
	struct device           *dev;
	struct i2c_client       *i2c;
	struct mutex            fuelgauge_mutex;
	struct max77823_platform_data *max77823_pdata;
	sec_battery_platform_data_t *pdata;
	struct power_supply		psy_fg;
	struct delayed_work isr_work;

	int cable_type;
	bool is_charging;

	/* HW-dedicated fuel guage info structure
	 * used in individual fuel gauge file only
	 * (ex. dummy_fuelgauge.c)
	 */
	struct sec_fg_info	info;
	struct battery_data_t        *battery_data;

	bool is_fuel_alerted;
	struct wake_lock fuel_alert_wake_lock;

	unsigned int capacity_old;	/* only for atomic calculation */
	unsigned int capacity_max;	/* only for dynamic calculation */

	bool initial_update_of_soc;
	struct mutex fg_lock;

	/* register programming */
	int reg_addr;
	u8 reg_data[2];

	int fg_irq;
};

#endif /* __MAX77823_FUELGAUGE_H */
