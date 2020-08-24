#include "CANController/CANController.h"

#include <stdint.h>
#include <stdbool.h>

//Inputs
uint16_t PacketThrottle; 	//0-1000
uint16_t PacketBrake;		//0-1000

typedef struct
{
	uint8_t B0 			: 1;
	uint8_t B1 			: 1;
	uint8_t B2 			: 1;
	uint8_t B3 			: 1;
	uint8_t B4 			: 1;
	uint8_t Brake 		: 1;
	uint8_t Direction 	: 2; // 0 neutral , 1 forward, 2 back , 3 reserved
} ControlFlags_t;

ControlFlags_t ControlFlags;

//Outputs
uint16_t PacketSpeed_RPMOffset10000;	//0-20000 -> -10000 to 10000
uint16_t PacketMotorCurent_TenthAmp; 	//0-4000

struct
{
	uint8_t Err8			: 1;
	uint8_t Reset			: 1;
	uint8_t HallPedal		: 1;
	uint8_t AngleSensor		: 1;
	uint8_t Err12 			: 1;
	uint8_t CAN 			: 1;
	uint8_t MotorOverTemp	: 1;
	uint8_t CurrentSensor 	: 1;
} ErrorFlagHigh;

struct
{
	uint8_t AngleSensor 				: 1;
	uint8_t BatteryOverVoltage 			: 1;
	uint8_t BatteryUnderVoltage 		: 1;
	uint8_t Err3						: 1;
	uint8_t Stall 						: 1;
	uint8_t Voltage 					: 1;
	uint8_t ControllerOverTemp 			: 1;
	uint8_t Pedal	 					: 1;
} ErrorFlagLow;


void ParseRxPacket(CAN_Frame_t * p_rxFrame)
{
	PacketThrottle 		= ((p_rxFrame->DSR[1] << 8) + p_rxFrame->DSR[0]);
	PacketBrake 		= ((p_rxFrame->DSR[4] << 8) + p_rxFrame->DSR[3]);
	ControlFlags 		= p_rxFrame->DSR[2];
}

void BuildTxPacket(CAN_Frame_t * p_txFrame)
{
	p_txFrame->DSR[0] = (uint8_t)PacketSpeed_RPMOffset10000;
	p_txFrame->DSR[1] = (uint8_t)(PacketSpeed_RPMOffset10000 << 8);
	p_txFrame->DSR[2] = (uint8_t)PacketMotorCurent_TenthAmp;
	p_txFrame->DSR[3] = (uint8_t)(PacketMotorCurent_TenthAmp << 8);
	p_txFrame->DSR[4] = ErrorFlagLow;
	p_txFrame->DSR[5] = ErrorFlagHigh;
}

//
uint16_t GetControllerThrottle(void) //convert external code to Kelly controller value.
{
	uint8_t x;

	return PacketThrottle*x;
}

uint16_t GetControllerBrake(void) //convert external code to Kelly controller value.
{
	uint8_t x;

	return PacketBrake*x;
}

void SetPacketSpeed(uint16_t controllerRPM) //convert Kelly controller value to external code.
{
	PacketSpeed_RPMOffset10000 = controllerRPM + 10000;
}

void SetPacketCurrent(uint16_t controllerCurrent) //convert Kelly controller value to external code.
{
	PacketMotorCurent_TenthAmp = controllerCurrent;
}

bool SetPacketErrorPowerOnReset(bool isOn)
{
	ErrorFlagHigh.Reset = isOn;
}
