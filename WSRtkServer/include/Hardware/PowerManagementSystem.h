#pragma once

// ! WARN:
// Please do not run the example without knowing the external load voltage of the PMU,
// it may burn your external load, please check the voltage setting before running the example,
// if there is any loss, please bear it by yourself

// #ifndef XPOWERS_NO_ERROR
// #error "Running this example is known to not damage the device! Please go and uncomment this!"
// #endif
// Defined using AXP2102

#define XPOWERS_CHIP_AXP2101

#include <Wire.h>
#include <Arduino.h>
#include "XPowersLib.h"
#include "History.h"

extern History _history;

///////////////////////////////////////////////////////////////////////////////
// Panel containing a label and a value
class PowerManagementSystem
{
private:
	bool _isMounted = false;
	bool pmu_flag = 0;
	XPowersPMU _power;
	unsigned long _checkStartTime;

public:
	
	///////////////////////////////////////////////////////////////////////////
	// Setup power management system
	void Setup()
	{
		bool result = _power.begin(Wire, AXP2101_SLAVE_ADDRESS, I2C_SDA, I2C_SCL);

		if (result == false)
		{
			Logln("power is not online...");
			return;
		}

		_isMounted = true;
		Logf("AXP2101 ID:0x%x", _power.getChipID());

		// Set the minimum common working voltage of the PMU VBUS input,
		// below this value will turn off the PMU
		_power.setVbusVoltageLimit(XPOWERS_AXP2101_VBUS_VOL_LIM_4V36);

		// Set the maximum current of the PMU VBUS input,
		// higher than this value will turn off the PMU
		_power.setVbusCurrentLimit(XPOWERS_AXP2101_VBUS_CUR_LIM_1500MA);

		// Get the VSYS shutdown voltage
		uint16_t vol = _power.getSysPowerDownVoltage();
		Logln("SysPowerDownVoltage");
		Logf("\t-> Before :%u", vol);

		// Set VSY off voltage as 2600mV , Adjustment range 2600mV ~ 3300mV
		_power.setSysPowerDownVoltage(2600);

		vol = _power.getSysPowerDownVoltage();
		Logf("\t-> After  :%u", vol);

		// DC1 IMAX=2A
		// 1500~3400mV,100mV/step,20steps
		_power.setDC1Voltage(3300);
		Logf("\tDC1  : %s   Voltage:%u mV ", _power.isEnableDC1() ? "+" : "-", _power.getDC1Voltage());

		// DC2 IMAX=2A
		// 500~1200mV  10mV/step,71steps
		// 1220~1540mV 20mV/step,17steps
		_power.setDC2Voltage(1000);
		Logf("\tDC2  : %s   Voltage:%u mV ", _power.isEnableDC2() ? "+" : "-", _power.getDC2Voltage());

		// DC3 IMAX = 2A
		// 500~1200mV,10mV/step,71steps
		// 1220~1540mV,20mV/step,17steps
		// 1600~3400mV,100mV/step,19steps
		_power.setDC3Voltage(3300);
		Logf("\tDC3  : %s   Voltage:%u mV ", _power.isEnableDC3() ? "+" : "-", _power.getDC3Voltage());

		// DCDC4 IMAX=1.5A
		// 500~1200mV,10mV/step,71steps
		// 1220~1840mV,20mV/step,32steps
		_power.setDC4Voltage(1000);
		Logf("\tDC4  : %s   Voltage:%u mV ", _power.isEnableDC4() ? "+" : "-", _power.getDC4Voltage());

		// DC5 IMAX=2A
		// 1200mV
		// 1400~3700mV,100mV/step,24steps
		_power.setDC5Voltage(3300);
		Logf("\tDC5  : %s   Voltage:%u mV ", _power.isEnableDC5() ? "+" : "-", _power.getDC5Voltage());

		// ALDO1 IMAX=300mA
		// 500~3500mV, 100mV/step,31steps
		_power.setALDO1Voltage(3300);

		// ALDO2 IMAX=300mA
		// 500~3500mV, 100mV/step,31steps
		_power.setALDO2Voltage(3300);

		// ALDO3 IMAX=300mA
		// 500~3500mV, 100mV/step,31steps
		_power.setALDO3Voltage(3300);

		// ALDO4 IMAX=300mA
		// 500~3500mV, 100mV/step,31steps
		_power.setALDO4Voltage(3300);

		// BLDO1 IMAX=300mA
		// 500~3500mV, 100mV/step,31steps
		_power.setBLDO1Voltage(3300);

		// BLDO2 IMAX=300mA
		// 500~3500mV, 100mV/step,31steps
		_power.setBLDO2Voltage(3300);

		// CPUSLDO IMAX=30mA
		// 500~1400mV,50mV/step,19steps
		_power.setCPUSLDOVoltage(1000);

		// DLDO1 IMAX=300mA
		// 500~3400mV, 100mV/step,29steps
		_power.setDLDO1Voltage(3300);

		// DLDO2 IMAX=300mA
		// 500~1400mV, 50mV/step,2steps
		_power.setDLDO2Voltage(3300);

		// _power.enableDC1();
		_power.enableDC2();
		_power.enableDC3();
		_power.enableDC4();
		_power.enableDC5();
		_power.enableALDO1();
		_power.enableALDO2();
		_power.enableALDO3();
		_power.enableALDO4();
		_power.enableBLDO1();
		_power.enableBLDO2();
		_power.enableCPUSLDO();
		_power.enableDLDO1();
		_power.enableDLDO2();

		Logln("DCDC=======================================================================");
		Logf("\tDC1  : %s   Voltage:%u mV ", _power.isEnableDC1() ? "+" : "-", _power.getDC1Voltage());
		Logf("\tDC2  : %s   Voltage:%u mV ", _power.isEnableDC2() ? "+" : "-", _power.getDC2Voltage());
		Logf("\tDC3  : %s   Voltage:%u mV ", _power.isEnableDC3() ? "+" : "-", _power.getDC3Voltage());
		Logf("\tDC4  : %s   Voltage:%u mV ", _power.isEnableDC4() ? "+" : "-", _power.getDC4Voltage());
		Logf("\tDC5  : %s   Voltage:%u mV ", _power.isEnableDC5() ? "+" : "-", _power.getDC5Voltage());
		Logln("ALDO=======================================================================");
		Logf("\tALDO1: %s   Voltage:%u mV", _power.isEnableALDO1() ? "+" : "-", _power.getALDO1Voltage());
		Logf("\tALDO2: %s   Voltage:%u mV", _power.isEnableALDO2() ? "+" : "-", _power.getALDO2Voltage());
		Logf("\tALDO3: %s   Voltage:%u mV", _power.isEnableALDO3() ? "+" : "-", _power.getALDO3Voltage());
		Logf("\tALDO4: %s   Voltage:%u mV", _power.isEnableALDO4() ? "+" : "-", _power.getALDO4Voltage());
		Logln("BLDO=======================================================================");
		Logf("\tBLDO1: %s   Voltage:%u mV", _power.isEnableBLDO1() ? "+" : "-", _power.getBLDO1Voltage());
		Logf("\tBLDO2: %s   Voltage:%u mV", _power.isEnableBLDO2() ? "+" : "-", _power.getBLDO2Voltage());
		Logln("CPUSLDO====================================================================");
		Logf("\tCPUSLDO: %s Voltage:%u mV", _power.isEnableCPUSLDO() ? "+" : "-", _power.getCPUSLDOVoltage());
		Logln("DLDO=======================================================================");
		Logf("\tDLDO1: %s   Voltage:%u mV", _power.isEnableDLDO1() ? "+" : "-", _power.getDLDO1Voltage());
		Logf("\tDLDO2: %s   Voltage:%u mV", _power.isEnableDLDO2() ? "+" : "-", _power.getDLDO2Voltage());
		Logln("===========================================================================");

		// Set the time of pressing the button to turn off
		_power.setPowerKeyPressOffTime(XPOWERS_POWEROFF_4S);
		uint8_t opt = _power.getPowerKeyPressOffTime();
		std::string optStr;
		switch (opt)
		{
		case XPOWERS_POWEROFF_4S:
			optStr = "4 Second";
			break;
		case XPOWERS_POWEROFF_6S:
			optStr = "6 Second";
			break;
		case XPOWERS_POWEROFF_8S:
			optStr = "8 Second";
			break;
		case XPOWERS_POWEROFF_10S:
			optStr = "10 Second";
			break;
		default:
			optStr = "Unknown " + std::to_string(opt);
			break;
		}
		Logf("\tPowerKeyPressOffTime:%s", optStr.c_str());

		// Set the button power-on press time
		_power.setPowerKeyPressOnTime(XPOWERS_POWERON_128MS);
		opt = _power.getPowerKeyPressOnTime();
		switch (opt)
		{
		case XPOWERS_POWERON_128MS:
			optStr = "128 Ms";
			break;
		case XPOWERS_POWERON_512MS:
			optStr = "512 Ms";
			break;
		case XPOWERS_POWERON_1S:
			optStr = "1 Second";
			break;
		case XPOWERS_POWERON_2S:
			optStr = "2 Second";
			break;
		default:
			optStr = "Unknown " + std::to_string(opt);
			break;
		}
		Logf("\tPowerKeyPressOnTime:%s", optStr.c_str());

		Logln("===========================================================================");

		bool en;

		// DCDC 120%(130%) high voltage turn off PMIC function
		Logf("\tDCHighVoltagePowerDownEn:%s ", _power.getDCHighVoltagePowerDownEn() ? "ENABLE" : "DISABLE");

		// DCDC1 85% low voltage turn off PMIC function
		Logf("\tDC1LowVoltagePowerDownEn:%s ", _power.getDC1LowVoltagePowerDownEn() ? "ENABLE" : "DISABLE");

		// DCDC2 85% low voltage turn off PMIC function
		Logf("\tDC2LowVoltagePowerDownEn:%s ", _power.getDC2LowVoltagePowerDownEn() ? "ENABLE" : "DISABLE");

		// DCDC3 85% low voltage turn off PMIC function
		Logf("\tDC3LowVoltagePowerDownEn:%s ", _power.getDC3LowVoltagePowerDownEn() ? "ENABLE" : "DISABLE");

		// DCDC4 85% low voltage turn off PMIC function
		Logf("\tDC4LowVoltagePowerDownEn:%s ", _power.getDC4LowVoltagePowerDownEn() ? "ENABLE" : "DISABLE");

		// DCDC5 85% low voltage turn off PMIC function
		Logf("\tDC5LowVoltagePowerDownEn:%s ", _power.getDC5LowVoltagePowerDownEn() ? "ENABLE" : "DISABLE");

		// _power.setDCHighVoltagePowerDown(true);
		// _power.setDC1LowVoltagePowerDown(true);
		// _power.setDC2LowVoltagePowerDown(true);
		// _power.setDC3LowVoltagePowerDown(true);
		// _power.setDC4LowVoltagePowerDown(true);
		// _power.setDC5LowVoltagePowerDown(true);

		// It is necessary to disable the detection function of the TS pin on the board
		// without the battery temperature detection function, otherwise it will cause abnormal charging
		_power.disableTSPinMeasure();

		// _power.enableTemperatureMeasure();

		// Enable internal ADC detection
		_power.enableBattDetection();
		_power.enableVbusVoltageMeasure();
		_power.enableBattVoltageMeasure();
		_power.enableSystemVoltageMeasure();

		//	The default setting is CHGLED is automatically controlled by the PMU.
		//  - XPOWERS_CHG_LED_OFF,
		//  - XPOWERS_CHG_LED_BLINK_1HZ,
		//  - XPOWERS_CHG_LED_BLINK_4HZ,
		//  - XPOWERS_CHG_LED_ON,
		//  - XPOWERS_CHG_LED_CTRL_CHG,
		_power.setChargingLedMode(XPOWERS_CHG_LED_CTRL_CHG);

		// Disable all interrupts
		_power.disableIRQ(XPOWERS_AXP2101_ALL_IRQ);

		// Clear all interrupt flags
		_power.clearIrqStatus();

		// Enable the required interrupt function
		_power.enableIRQ(
			XPOWERS_AXP2101_BAT_INSERT_IRQ | XPOWERS_AXP2101_BAT_REMOVE_IRQ |	 // BATTERY
			XPOWERS_AXP2101_VBUS_INSERT_IRQ | XPOWERS_AXP2101_VBUS_REMOVE_IRQ |	 // VBUS
			XPOWERS_AXP2101_PKEY_SHORT_IRQ | XPOWERS_AXP2101_PKEY_LONG_IRQ |	 // POWER KEY
			XPOWERS_AXP2101_BAT_CHG_DONE_IRQ | XPOWERS_AXP2101_BAT_CHG_START_IRQ // CHARGE
		);

		// Set the precharge charging current
		_power.setPrechargeCurr(XPOWERS_AXP2101_PRECHARGE_50MA);
		// Set constant current charge current limit
		_power.setChargerConstantCurr(XPOWERS_AXP2101_CHG_CUR_200MA);
		// Set stop charging termination current
		_power.setChargerTerminationCurr(XPOWERS_AXP2101_CHG_ITERM_25MA);

		// Set charge cut-off voltage
		_power.setChargeTargetVoltage(XPOWERS_AXP2101_CHG_VOL_4V1);

		// Set the watchdog trigger event type
		_power.setWatchdogConfig(XPOWERS_AXP2101_WDT_IRQ_TO_PIN);

		// Set watchdog timeout
		_power.setWatchdogTimeout(XPOWERS_AXP2101_WDT_TIMEOUT_32S);
		//_power.enableWatchdog();
		_power.disableWatchdog();

		// Enable Button Battery charge
		_power.enableButtonBatteryCharge();

		// Set Button Battery charge voltage
		_power.setButtonBatteryChargeVoltage(3300);

		// Dump the current state
		PrintPMU();
	}

	///////////////////////////////////////////////////////////////////////////
	// Get the current charge status
	// The battery percentage may be inaccurate at first use, the PMU will automatically
	// learn the battery curve and will automatically calibrate the battery percentage
	// after a charge and discharge cycle
	int GetBatteryPercent()
	{
		if (!_isMounted)
			return -1;
		if (!_power.isBatteryConnect())
		return -1;
		return _power.getBatteryPercent();
	}
	bool IsCharging()
	{
		if (!_isMounted)
			return false;
		return _power.isCharging();
	}

	///////////////////////////////////////////////////////////////////////////
	// Record the current state to log
	void PrintPMU()
	{
		if( !_isMounted )
		{
			Logln("PMU is not mounted, cannot print status.");
			return;
		}

		Logln("===========================================================================");
		Logf("\tisCharging:%s ", _power.isCharging() ? "YES" : "NO");
		Logf("\tisDischarge:%s ", _power.isDischarge() ? "YES" : "NO");
		Logf("\tisStandby:%s ", _power.isStandby() ? "YES" : "NO");
		Logf("\tisVbusIn:%s ", _power.isVbusIn() ? "YES" : "NO");
		Logf("\tisVbusGood:%s ", _power.isVbusGood() ? "YES" : "NO");
		Logf("\tChargerStatus:%s", GetChargeStatus().c_str());

		Logf("\tBattVoltage:%d mV ", _power.getBattVoltage());
		Logf("\tVbusVoltage:%d mV ", _power.getVbusVoltage());
		Logf("\tSystemVoltage:%d mV ", _power.getSystemVoltage());

		// The battery percentage may be inaccurate at first use, the PMU will automatically
		// learn the battery curve and will automatically calibrate the battery percentage
		// after a charge and discharge cycle
		if (_power.isBatteryConnect())
			Logf("\tBatteryPercent:%d%%", _power.getBatteryPercent());
	}

	///////////////////////////////////////////////////////////////////////////
	// Turn off the PMU
	void EnterPmuSleep(void)
	{
		// Set the wake-up source to PWRKEY
		_power.wakeupControl(XPOWERS_AXP2101_WAKEUP_IRQ_PIN_TO_LOW, true);

		// Set sleep flag
		_power.enableSleep();

		_power.disableDC2();
		_power.disableDC3();
		_power.disableDC4();
		_power.disableDC5();

		_power.disableALDO1();
		_power.disableALDO2();
		_power.disableALDO3();
		_power.disableALDO4();

		_power.disableBLDO1();
		_power.disableBLDO2();

		_power.disableCPUSLDO();
		_power.disableDLDO1();
		_power.disableDLDO2();

		// Finally, turn off the power of the control chip
		_power.disableDC1();
	}

	///////////////////////////////////////////////////////////////////////////
	// Loop to check PMU status and handle interrupts
	// This function should be called periodically in the main loop
	void PowerLoop()
	{
		_checkStartTime = millis();
		// Only run if the PMU is mounted
		// This prevents unnecessary checks if the PMU is not initialized
		if (!_isMounted)
			return;

		// Get PMU Interrupt Status Register
		uint32_t status = _power.getIrqStatus();

		// No interrupt, return early
		if (status == 0)
			return;

		Logf("STATUS => HEX:%04x ", status);

		if (_power.isDropWarningLevel2Irq())
		{
			Logln("isDropWarningLevel2");
		}
		if (_power.isDropWarningLevel1Irq())
		{
			Logln("isDropWarningLevel1");
		}
		if (_power.isGaugeWdtTimeoutIrq())
		{
			Logln("isWdtTimeout");
		}
		if (_power.isBatChargerOverTemperatureIrq())
		{
			Logln("isBatChargeOverTemperature");
		}
		if (_power.isBatWorkOverTemperatureIrq())
		{
			Logln("isBatWorkOverTemperature");
		}
		if (_power.isBatWorkUnderTemperatureIrq())
		{
			Logln("isBatWorkUnderTemperature");
		}
		if (_power.isVbusInsertIrq())
		{
			Logln("isVbusInsert");
		}
		if (_power.isVbusRemoveIrq())
		{
			Logln("isVbusRemove");
		}
		if (_power.isBatInsertIrq())
		{
			Logln("isBatInsert");
		}
		if (_power.isBatRemoveIrq())
		{
			Logln("isBatRemove");
		}

		if (_power.isPekeyShortPressIrq())
		{
			Logln("isPekeyShortPress");
			// enterPmuSleep();
			std::string buff;
			uint8_t data[4] = {0};
			_power.readDataBuffer(data, XPOWERS_AXP2101_DATA_BUFFER_SIZE);
			for (int i = 0; i < 4; ++i)
			{
				buff += std::to_string(data[i]);
				buff += ",";
			}
			Logf("Read pmu data buffer .%s", buff.c_str());
		}

		if (_power.isPekeyLongPressIrq())
		{
			Logln("isPekeyLongPress");
			Logln("write pmu data buffer .");
			uint8_t data[4] = {1, 2, 3, 4};
			_power.writeDataBuffer(data, XPOWERS_AXP2101_DATA_BUFFER_SIZE);
		}

		if (_power.isPekeyNegativeIrq())
		{
			Logln("isPekeyNegative");
		}
		if (_power.isPekeyPositiveIrq())
		{
			Logln("isPekeyPositive");
		}
		if (_power.isWdtExpireIrq())
		{
			Logln("isWdtExpire");
			PrintPMU();
		}
		if (_power.isLdoOverCurrentIrq())
		{
			Logln("isLdoOverCurrentIrq");
		}
		if (_power.isBatfetOverCurrentIrq())
		{
			Logln("isBatfetOverCurrentIrq");
		}
		if (_power.isBatChargeDoneIrq())
		{
			Logln("isBatChargeDone");
		}
		if (_power.isBatChargeStartIrq())
		{
			Logln("isBatChargeStart");
		}
		if (_power.isBatDieOverTemperatureIrq())
		{
			Logln("isBatDieOverTemperature");
		}
		if (_power.isChargeOverTimeoutIrq())
		{
			Logln("isChargeOverTimeout");
		}
		if (_power.isBatOverVoltageIrq())
		{
			Logln("isBatOverVoltage");
		}

		// Clear PMU Interrupt Status Register
		_power.clearIrqStatus();
		Logf("PowerLoop took %lu ms", millis() - _checkStartTime);
	}

	///////////////////////////////////////////////////////////////////////////
	/// Refresh the data in the table
	void RefreshData(SwipePageBase *pPage)
	{
		// CPU Temperature
		pPage->SetTableString(14, ToFloat(_history.CheckTemperatureLoop()) + " °C");

		if( !_isMounted )
		{
			pPage->SetTableValue(0, "PMU not mounted");
			return;
		}

		pPage->SetTableValue(1, _power.isCharging() ? "Yes" : "No");
		pPage->SetTableValue(2, _power.isDischarge() ? "Yes" : "No");
		pPage->SetTableValue(3, _power.isStandby() ? "Yes" : "No");

		pPage->SetTableString(4, GetChargeStatus());
		pPage->SetTableString(5, _power.isBatteryConnect() ? (std::to_string(_power.getBatteryPercent()) + "%") : "No Battery");

		pPage->SetTableValue(7, _power.isVbusIn() ? "Yes" : "No");
		pPage->SetTableValue(8, _power.isVbusGood() ? "Yes" : "No");

		pPage->SetTableString(10, std::to_string(_power.getVbusVoltage()) + " mV");
		pPage->SetTableString(11, std::to_string(_power.getBattVoltage()) + " mV");
		pPage->SetTableString(12, std::to_string(_power.getSystemVoltage()) + " mV");
	}

	std::string GetChargeStatus()
	{
		uint8_t cs = _power.getChargerStatus();
		switch (cs)
		{
		case XPOWERS_AXP2101_CHG_TRI_STATE:
			return "Tri Charge";
		case XPOWERS_AXP2101_CHG_PRE_STATE:
			return "Pre Charge";
		case XPOWERS_AXP2101_CHG_CC_STATE:
			return "Constant Charge";
		case XPOWERS_AXP2101_CHG_CV_STATE:
			return "Constant Voltage";
		case XPOWERS_AXP2101_CHG_DONE_STATE:
			return "Charge Done";
		case XPOWERS_AXP2101_CHG_STOP_STATE:
			return "Not Charging";
		default:
			return std::to_string(cs);
		}
	}
};
