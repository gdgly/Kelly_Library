/******************************************************************************/
/*!
    @file     VoltageDivider.c
    @author   FireSourcery / Kelly Controls Inc

    @section LICENSE

    Copyright (C) 2020 Kelly Controls Inc

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

    @brief 	Voltage to ADC values conversion using voltage divider.
    		Calculates and holds ADCU voltage conversion ratios

	VDIV = VIN*(R2/(R1+R2))
	DIV = (R2/(R1+R2))
	VADC_RES = VREF/ADC_MAX

	ADC = VIN*DIV/VADC_RES = VIN*(R2*ADC_MAX)/((R1+R2)*VREF)
	VIN = ADC*VADC_RES/DIV = ADC*(VREF*(R1+R2))/(ADC_MAX*R2)
	VIN/ADC = VADC_RES/DIV = VREF*(R1 + R2)/(ADC_MAX*R2)
*/
/******************************************************************************/
#include "VoltageDivider/VoltageDivider.h"

/******************************************************************************/
/*!
 * @brief Initialize Voltage Divider struct.
 * Struct contains numerator and denominator of ADC to Vin conversion factor.
 *
 * @param[in] div - Struct containing calculated intermediate values
 * @param[in] r1 - R1 value expressed as a whole number
 * @param[in] r2 - R2 value expressed as a whole number
 * @param[in] vRef - reference voltage
 * @param[in] adcMax - ADC range maximum
 */
/******************************************************************************/
#ifndef VOLTAGE_DIVIDER_CONFIG_FAST
void VoltageDivider_Init(VOLTAGE_DIVIDER_T * div, uint32_t r1, uint32_t r2, uint8_t vRef, uint16_t adcBits)
{
	div->VPerADCFactor = vRef * (r1 + r2);
	div->VPerADCDivisor = (2^adcBits - 1) * r2;
}
#else
void VoltageDivider_Init(VOLTAGE_DIVIDER_T * div, uint32_t r1, uint32_t r2, uint8_t vRef, uint8_t adcBits)
{
	div->VPerADCFactor = ((vRef * (r1 + r2)) << (16 - adcBits)) / r2; 	// (VREF*(R1 + R2) << 16)/(ADC_MAX*R2)
	div->VPerADCDivisor = ((r2 << adcBits) / (vRef * (r1 + r2)));		// ((ADC_MAX*R2) << 16)/(VREF*(R1 + R2))
	div->ADCBits = adcBits;
}
#endif

/******************************************************************************/
/*!
 * @brief Calculate voltage from given ADC value
 *
 * @param[in] div - struct containing calculated intermediate values
 * @param[in] adcRaw - ADC value
 * @return Calculated voltage
 */
/******************************************************************************/
#ifndef VOLTAGE_DIVIDER_CONFIG_FAST
uint16_t VoltageDivider_GetVoltage(VOLTAGE_DIVIDER_T * div, uint16_t adcRaw)
{
	return ((adcRaw * div->VPerADCFactor + (div->VPerADCDivisor / 2)) / div->VPerADCDivisor); // (adcRaw*VREF*(R1+R2))/(ADC_RES*R2); // add (div->VPerADCDivisor/2) to round up .5
}
#else
uint16_t VoltageDivider_GetVoltage(VOLTAGE_DIVIDER_T * div, uint16_t adcRaw)
{
	return ((adcRaw * div->VPerADCFactor) >> 16);
}
#endif

/******************************************************************************/
/*!
 * @brief Calculate voltage from given ADC value, include extra decimal digits
 *
 * @param[in] div - struct containing calculated intermediate values
 * @param[in] adcRaw - ADC value
 * @param[in] digits - number of decimal digits
 * @return Calculated voltage
 */
/******************************************************************************/
#ifndef VOLTAGE_DIVIDER_CONFIG_FAST
uint16_t VoltageDivider_GetVoltageDigits(VOLTAGE_DIVIDER_T * div, uint16_t adcRaw, uint8_t digits)
{
	return ((adcRaw*div->VPerADCFactor * (10^digits)) / div->VPerADCDivisor);
}
#else
uint16_t VoltageDivider_GetVoltageDigits(VOLTAGE_DIVIDER_T * div, uint16_t adcRaw, uint8_t digits)
{
	return (((adcRaw * div->VPerADCFactor) * (10^digits)) >> 16);
}
#endif

/******************************************************************************/
/*!
 * @brief Calculate ADC value from given voltage
 *
 * @param[in] div - struct containing calculated intermediate values
 * @param[in] voltage - voltage
 * @return Calculated ADC value
 */
/******************************************************************************/
#ifndef VOLTAGE_DIVIDER_CONFIG_FAST
uint16_t VoltageDivider_GetADCRaw(VOLTAGE_DIVIDER_T * div, uint16_t voltage)
{
	return ((voltage * div->VPerADCDivisor + (div->VPerADCFactor / 2)) / div->VPerADCFactor); // voltage*(R2*ADC_MAX)/((R1+R2)*VREF) // add (div->VPerADCFactor/2) to round up .5
}
#else
uint16_t VoltageDivider_GetADCRaw(VOLTAGE_DIVIDER_T * div, uint16_t voltage)
{
	return ((voltage * div->VPerADCDivisor) >> (16 - div->ADCBits));
}
#endif

void VoltageDivider_SetDivider(VOLTAGE_DIVIDER_T * div, uint32_t r1, uint32_t r2)
{

}

void VoltageDivider_InitPercentage(VOLTAGE_DIVIDER_T * div, uint32_t r1, uint32_t r2, uint8_t vRef, uint16_t adcBits, uint16_t v100Percent, uint16_t v0Percent)
{
//	div->V100Percent;
//	div->V0Percent;
//	div->VPercentPerADCU;
//	div->V0PercentADCU;
}
