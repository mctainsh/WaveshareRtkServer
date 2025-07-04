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
	// XPowersPMU Get() { return _power; }

	///////////////////////////////////////////////////////////////////////////
	// Setup power management system
	void Setup()
	{
		bool result = _power.begin(Wire, AXP2101_SLAVE_ADDRESS, I2C_SDA, I2C_SCL);

		if (result == false)
		{
			Serial.println("power is not online...");
			return;
		}
		_isMounted = true;
		Serial.printf("getID:0x%x\n", _power.getChipID());

		// Set the minimum common working voltage of the PMU VBUS input,
		// below this value will turn off the PMU
		_power.setVbusVoltageLimit(XPOWERS_AXP2101_VBUS_VOL_LIM_4V36);

		// Set the maximum current of the PMU VBUS input,
		// higher than this value will turn off the PMU
		_power.setVbusCurrentLimit(XPOWERS_AXP2101_VBUS_CUR_LIM_1500MA);

		// Get the VSYS shutdown voltage
		uint16_t vol = _power.getSysPowerDownVoltage();
		Serial.println("SysPowerDownVoltage");
		Serial.printf("-> Before :%u\n", vol);

		// Set VSY off voltage as 2600mV , Adjustment range 2600mV ~ 3300mV
		_power.setSysPowerDownVoltage(2600);

		vol = _power.getSysPowerDownVoltage();
		Serial.printf("-> After  :%u\n", vol);

		// DC1 IMAX=2A
		// 1500~3400mV,100mV/step,20steps
		_power.setDC1Voltage(3300);
		Serial.printf("DC1  : %s   Voltage:%u mV \n", _power.isEnableDC1() ? "+" : "-", _power.getDC1Voltage());

		// DC2 IMAX=2A
		// 500~1200mV  10mV/step,71steps
		// 1220~1540mV 20mV/step,17steps
		_power.setDC2Voltage(1000);
		Serial.printf("DC2  : %s   Voltage:%u mV \n", _power.isEnableDC2() ? "+" : "-", _power.getDC2Voltage());

		// DC3 IMAX = 2A
		// 500~1200mV,10mV/step,71steps
		// 1220~1540mV,20mV/step,17steps
		// 1600~3400mV,100mV/step,19steps
		_power.setDC3Voltage(3300);
		Serial.printf("DC3  : %s   Voltage:%u mV \n", _power.isEnableDC3() ? "+" : "-", _power.getDC3Voltage());

		// DCDC4 IMAX=1.5A
		// 500~1200mV,10mV/step,71steps
		// 1220~1840mV,20mV/step,32steps
		_power.setDC4Voltage(1000);
		Serial.printf("DC4  : %s   Voltage:%u mV \n", _power.isEnableDC4() ? "+" : "-", _power.getDC4Voltage());

		// DC5 IMAX=2A
		// 1200mV
		// 1400~3700mV,100mV/step,24steps
		_power.setDC5Voltage(3300);
		Serial.printf("DC5  : %s   Voltage:%u mV \n", _power.isEnableDC5() ? "+" : "-", _power.getDC5Voltage());

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

		Serial.println("DCDC=======================================================================");
		Serial.printf("DC1  : %s   Voltage:%u mV \n", _power.isEnableDC1() ? "+" : "-", _power.getDC1Voltage());
		Serial.printf("DC2  : %s   Voltage:%u mV \n", _power.isEnableDC2() ? "+" : "-", _power.getDC2Voltage());
		Serial.printf("DC3  : %s   Voltage:%u mV \n", _power.isEnableDC3() ? "+" : "-", _power.getDC3Voltage());
		Serial.printf("DC4  : %s   Voltage:%u mV \n", _power.isEnableDC4() ? "+" : "-", _power.getDC4Voltage());
		Serial.printf("DC5  : %s   Voltage:%u mV \n", _power.isEnableDC5() ? "+" : "-", _power.getDC5Voltage());
		Serial.println("ALDO=======================================================================");
		Serial.printf("ALDO1: %s   Voltage:%u mV\n", _power.isEnableALDO1() ? "+" : "-", _power.getALDO1Voltage());
		Serial.printf("ALDO2: %s   Voltage:%u mV\n", _power.isEnableALDO2() ? "+" : "-", _power.getALDO2Voltage());
		Serial.printf("ALDO3: %s   Voltage:%u mV\n", _power.isEnableALDO3() ? "+" : "-", _power.getALDO3Voltage());
		Serial.printf("ALDO4: %s   Voltage:%u mV\n", _power.isEnableALDO4() ? "+" : "-", _power.getALDO4Voltage());
		Serial.println("BLDO=======================================================================");
		Serial.printf("BLDO1: %s   Voltage:%u mV\n", _power.isEnableBLDO1() ? "+" : "-", _power.getBLDO1Voltage());
		Serial.printf("BLDO2: %s   Voltage:%u mV\n", _power.isEnableBLDO2() ? "+" : "-", _power.getBLDO2Voltage());
		Serial.println("CPUSLDO====================================================================");
		Serial.printf("CPUSLDO: %s Voltage:%u mV\n", _power.isEnableCPUSLDO() ? "+" : "-", _power.getCPUSLDOVoltage());
		Serial.println("DLDO=======================================================================");
		Serial.printf("DLDO1: %s   Voltage:%u mV\n", _power.isEnableDLDO1() ? "+" : "-", _power.getDLDO1Voltage());
		Serial.printf("DLDO2: %s   Voltage:%u mV\n", _power.isEnableDLDO2() ? "+" : "-", _power.getDLDO2Voltage());
		Serial.println("===========================================================================");

		// Set the time of pressing the button to turn off
		_power.setPowerKeyPressOffTime(XPOWERS_POWEROFF_4S);
		uint8_t opt = _power.getPowerKeyPressOffTime();
		Serial.print("PowerKeyPressOffTime:");
		switch (opt)
		{
		case XPOWERS_POWEROFF_4S:
			Serial.println("4 Second");
			break;
		case XPOWERS_POWEROFF_6S:
			Serial.println("6 Second");
			break;
		case XPOWERS_POWEROFF_8S:
			Serial.println("8 Second");
			break;
		case XPOWERS_POWEROFF_10S:
			Serial.println("10 Second");
			break;
		default:
			break;
		}
		// Set the button power-on press time
		_power.setPowerKeyPressOnTime(XPOWERS_POWERON_128MS);
		opt = _power.getPowerKeyPressOnTime();
		Serial.print("PowerKeyPressOnTime:");
		switch (opt)
		{
		case XPOWERS_POWERON_128MS:
			Serial.println("128 Ms");
			break;
		case XPOWERS_POWERON_512MS:
			Serial.println("512 Ms");
			break;
		case XPOWERS_POWERON_1S:
			Serial.println("1 Second");
			break;
		case XPOWERS_POWERON_2S:
			Serial.println("2 Second");
			break;
		default:
			break;
		}

		Serial.println("===========================================================================");

		bool en;

		// DCDC 120%(130%) high voltage turn off PMIC function
		en = _power.getDCHighVoltagePowerDownEn();
		Serial.print("getDCHighVoltagePowerDownEn:");
		Serial.println(en ? "ENABLE" : "DISABLE");
		// DCDC1 85% low voltage turn off PMIC function
		en = _power.getDC1LowVoltagePowerDownEn();
		Serial.print("getDC1LowVoltagePowerDownEn:");
		Serial.println(en ? "ENABLE" : "DISABLE");
		// DCDC2 85% low voltage turn off PMIC function
		en = _power.getDC2LowVoltagePowerDownEn();
		Serial.print("getDC2LowVoltagePowerDownEn:");
		Serial.println(en ? "ENABLE" : "DISABLE");
		// DCDC3 85% low voltage turn off PMIC function
		en = _power.getDC3LowVoltagePowerDownEn();
		Serial.print("getDC3LowVoltagePowerDownEn:");
		Serial.println(en ? "ENABLE" : "DISABLE");
		// DCDC4 85% low voltage turn off PMIC function
		en = _power.getDC4LowVoltagePowerDownEn();
		Serial.print("getDC4LowVoltagePowerDownEn:");
		Serial.println(en ? "ENABLE" : "DISABLE");
		// DCDC5 85% low voltage turn off PMIC function
		en = _power.getDC5LowVoltagePowerDownEn();
		Serial.print("getDC5LowVoltagePowerDownEn:");
		Serial.println(en ? "ENABLE" : "DISABLE");

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
		_power.setWatchdogTimeout(XPOWERS_AXP2101_WDT_TIMEOUT_4S);
		_power.enableWatchdog();
		//_power.disableWatchdog();

		// Enable Button Battery charge
		_power.enableButtonBatteryCharge();

		// Set Button Battery charge voltage
		_power.setButtonBatteryChargeVoltage(3300);

		// Dump the current state
		PrintPMU();
	}

	///////////////////////////////////////////////////////////////////////////
	// Record the current state to log
	void PrintPMU()
	{
		Serial.print("isCharging:");
		Serial.println(_power.isCharging() ? "YES" : "NO");
		Serial.print("isDischarge:");
		Serial.println(_power.isDischarge() ? "YES" : "NO");
		Serial.print("isStandby:");
		Serial.println(_power.isStandby() ? "YES" : "NO");
		Serial.print("isVbusIn:");
		Serial.println(_power.isVbusIn() ? "YES" : "NO");
		Serial.print("isVbusGood:");
		Serial.println(_power.isVbusGood() ? "YES" : "NO");
		Serial.print("getChargerStatus:");
		uint8_t charge_status = _power.getChargerStatus();
		if (charge_status == XPOWERS_AXP2101_CHG_TRI_STATE)
		{
			Serial.println("tri_charge");
		}
		else if (charge_status == XPOWERS_AXP2101_CHG_PRE_STATE)
		{
			Serial.println("pre_charge");
		}
		else if (charge_status == XPOWERS_AXP2101_CHG_CC_STATE)
		{
			Serial.println("constant charge");
		}
		else if (charge_status == XPOWERS_AXP2101_CHG_CV_STATE)
		{
			Serial.println("constant voltage");
		}
		else if (charge_status == XPOWERS_AXP2101_CHG_DONE_STATE)
		{
			Serial.println("charge done");
		}
		else if (charge_status == XPOWERS_AXP2101_CHG_STOP_STATE)
		{
			Serial.println("not charge");
		}

		Serial.print("getBattVoltage:");
		Serial.print(_power.getBattVoltage());
		Serial.println("mV");
		Serial.print("getVbusVoltage:");
		Serial.print(_power.getVbusVoltage());
		Serial.println("mV");
		Serial.print("getSystemVoltage:");
		Serial.print(_power.getSystemVoltage());
		Serial.println("mV");

		// The battery percentage may be inaccurate at first use, the PMU will automatically
		// learn the battery curve and will automatically calibrate the battery percentage
		// after a charge and discharge cycle
		if (_power.isBatteryConnect())
		{
			Serial.print("getBatteryPercent:");
			Serial.print(_power.getBatteryPercent());
			Serial.println("%");
		}

		Serial.println();
	}

	// void EnterPmuSleep(void)
	// {
	// 	// Set the wake-up source to PWRKEY
	// 	_power.wakeupControl(XPOWERS_AXP2101_WAKEUP_IRQ_PIN_TO_LOW, true);

	// 	// Set sleep flag
	// 	_power.enableSleep();

	// 	_power.disableDC2();
	// 	_power.disableDC3();
	// 	_power.disableDC4();
	// 	_power.disableDC5();

	// 	_power.disableALDO1();
	// 	_power.disableALDO2();
	// 	_power.disableALDO3();
	// 	_power.disableALDO4();

	// 	_power.disableBLDO1();
	// 	_power.disableBLDO2();

	// 	_power.disableCPUSLDO();
	// 	_power.disableDLDO1();
	// 	_power.disableDLDO2();

	// 	// Finally, turn off the power of the control chip
	// 	_power.disableDC1();
	// }

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

		Serial.print("STATUS => HEX:");
		Serial.print(status, HEX);
		Serial.print(" BIN:");
		Serial.println(status, BIN);

		if (_power.isDropWarningLevel2Irq())
		{
			Serial.println("isDropWarningLevel2");
		}
		if (_power.isDropWarningLevel1Irq())
		{
			Serial.println("isDropWarningLevel1");
		}
		if (_power.isGaugeWdtTimeoutIrq())
		{
			Serial.println("isWdtTimeout");
		}
		if (_power.isBatChargerOverTemperatureIrq())
		{
			Serial.println("isBatChargeOverTemperature");
		}
		if (_power.isBatWorkOverTemperatureIrq())
		{
			Serial.println("isBatWorkOverTemperature");
		}
		if (_power.isBatWorkUnderTemperatureIrq())
		{
			Serial.println("isBatWorkUnderTemperature");
		}
		if (_power.isVbusInsertIrq())
		{
			Serial.println("isVbusInsert");
		}
		if (_power.isVbusRemoveIrq())
		{
			Serial.println("isVbusRemove");
		}
		if (_power.isBatInsertIrq())
		{
			Serial.println("isBatInsert");
		}
		if (_power.isBatRemoveIrq())
		{
			Serial.println("isBatRemove");
		}

		if (_power.isPekeyShortPressIrq())
		{
			Serial.println("isPekeyShortPress");
			// enterPmuSleep();

			Serial.print("Read pmu data buffer .");
			uint8_t data[4] = {0};
			_power.readDataBuffer(data, XPOWERS_AXP2101_DATA_BUFFER_SIZE);
			for (int i = 0; i < 4; ++i)
			{
				Serial.print(data[i]);
				Serial.print(",");
			}
			Serial.println();
		}

		if (_power.isPekeyLongPressIrq())
		{
			Serial.println("isPekeyLongPress");
			Serial.println("write pmu data buffer .");
			uint8_t data[4] = {1, 2, 3, 4};
			_power.writeDataBuffer(data, XPOWERS_AXP2101_DATA_BUFFER_SIZE);
		}

		if (_power.isPekeyNegativeIrq())
		{
			Serial.println("isPekeyNegative");
		}
		if (_power.isPekeyPositiveIrq())
		{
			Serial.println("isPekeyPositive");
		}
		if (_power.isWdtExpireIrq())
		{
			Serial.println("isWdtExpire");
			PrintPMU();
		}
		if (_power.isLdoOverCurrentIrq())
		{
			Serial.println("isLdoOverCurrentIrq");
		}
		if (_power.isBatfetOverCurrentIrq())
		{
			Serial.println("isBatfetOverCurrentIrq");
		}
		if (_power.isBatChargeDoneIrq())
		{
			Serial.println("isBatChargeDone");
		}
		if (_power.isBatChargeStartIrq())
		{
			Serial.println("isBatChargeStart");
		}
		if (_power.isBatDieOverTemperatureIrq())
		{
			Serial.println("isBatDieOverTemperature");
		}
		if (_power.isChargeOverTimeoutIrq())
		{
			Serial.println("isChargeOverTimeout");
		}
		if (_power.isBatOverVoltageIrq())
		{
			Serial.println("isBatOverVoltage");
		}

		// Clear PMU Interrupt Status Register
		_power.clearIrqStatus();
		Serial.printf("PowerLoop took %lu ms\n", millis() - _checkStartTime);
	}

	///////////////////////////////////////////////////////////////////////////
	/// Refresh the data in the table
	void RefreshData(SwipePageBase *pPage)
	{
		pPage->SetTableValue(1, _power.isCharging() ? "Yes" : "No");
		pPage->SetTableValue(2, _power.isDischarge() ? "Yes" : "No");
		pPage->SetTableValue(3, _power.isStandby() ? "Yes" : "No");

		uint8_t chargeStatusInt = _power.getChargerStatus();
		std::string chargeStatus;
		switch (chargeStatusInt)
		{
		case XPOWERS_AXP2101_CHG_TRI_STATE:
			chargeStatus = "Tri Charge";
			break;
		case XPOWERS_AXP2101_CHG_PRE_STATE:
			chargeStatus = "Pre Charge";
			break;
		case XPOWERS_AXP2101_CHG_CC_STATE:
			chargeStatus = "Constant Charge";
			break;
		case XPOWERS_AXP2101_CHG_CV_STATE:
			chargeStatus = "Constant Voltage";
			break;
		case XPOWERS_AXP2101_CHG_DONE_STATE:
			chargeStatus = "Charge Done";
			break;
		case XPOWERS_AXP2101_CHG_STOP_STATE:
			chargeStatus = "Not Charging";
			break;
		default:
			chargeStatus = std::to_string(chargeStatusInt);
			break;
		}
		pPage->SetTableString(4, chargeStatus);
		pPage->SetTableString(5, _power.isBatteryConnect() ? (std::to_string(_power.getBatteryPercent()) + "%") : "No Battery");

		pPage->SetTableValue(7, _power.isVbusIn() ? "Yes" : "No");
		pPage->SetTableValue(8, _power.isVbusGood() ? "Yes" : "No");


		pPage->SetTableString(10, std::to_string(_power.getVbusVoltage()) + " mV");
		pPage->SetTableString(11, std::to_string(_power.getBattVoltage()) + " mV");
		pPage->SetTableString(12, std::to_string(_power.getSystemVoltage()) + " mV");
	}
};
