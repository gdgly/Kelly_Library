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

	DIV = R2RATIO/(R1RATIO + R2RATIO)
	VDIV = VIN*DIV
	VDIV_PER_ADC = VREF/ADCMAX
	ADC = VDIV/VDIV_PER_ADC

	ADC = VIN*(R2RATIO/(R1RATIO + R2RATIO))/(VREF/ADCMAX)
	ADC = VIN*(ADCMAX*R2RATIO)/((R1RATIO + R2RATIO)*VREF)

	VIN_PER_ADC == VIN/ADC:
	VIN*DIV = ADC*VDIV_PER_ADC
	VIN/ADC = VDIV_PER_ADC/DIV
	VIN_PER_ADC = VDIV_PER_ADC/DIV
	VIN_PER_ADC = VREF/ADCMAX/(R2RATIO/(R1RAIO + R2RATIO))
	VIN_PER_ADC = VREF*(R1RAIO + R2RATIO)/(ADCMAX*R2RATIO)
*/
/******************************************************************************/
#include "VoltageDivider.h"

/******************************************************************************/
/*!
 * @brief Initialize Voltage Divider struct.
 * Struct contains numerator and denominator of ADC to Vin conversion factor.
 *
 * @param[in] div - Struct containing calculated intermediate values
 * @param[in] r1Ratio - R1 value expressed as a whole number
 * @param[in] r2Ratio - R2 value expressed as a whole number
 * @param[in] vRef - reference voltage
 * @param[in] adcMax - ADC range maximum
 */
/******************************************************************************/
void VoltageDivider_Init(VOLTAGE_DIVIDER_T * div, uint32_t r1Ratio, uint32_t r2Ratio, uint8_t vRef, uint16_t adcMax)
{
	div->VPerADCTop = vRef*(r1Ratio+r2Ratio);
	div->VPerADCBottom = adcMax*r2Ratio;
}

/******************************************************************************/
/*!
 * @brief Calculate voltage from given ADC value
 *
 * @param[in] div - Struct containing calculated intermediate values
 * @param[in] adcRaw - ADC value
 * @return Calculated voltage
 */
/******************************************************************************/
uint16_t VoltageDivider_GetVoltage(VOLTAGE_DIVIDER_T * div, uint16_t adcRaw)
{
	return (adcRaw*div->VPerADCTop + (div->VPerADCBottom/2) )/div->VPerADCBottom; // (adcRaw*VREF*(R1_RATIO+R2_RATIO))/(R2_RATIO*ADC_RES); // add (div->VPerADCBottom/2) to round up .5
}

/******************************************************************************/
/*!
 * @brief Calculate voltage from given ADC value
 *
 * @param[in] div - Struct containing calculated intermediate values
 * @param[in] adcRaw - ADC value
 * @param[in] precision - number of decimal digits
 * @return Calculated voltage
 */
/******************************************************************************/
uint16_t VoltageDivider_GetVoltage10(VOLTAGE_DIVIDER_T * div, uint16_t adcRaw, uint8_t precision)
{
	return (adcRaw*div->VPerADCTop*(10^precision))/div->VPerADCBottom;
}

/******************************************************************************/
/*!
 * @brief Calculate ADC value from given voltage
 *
 * @param[in] div - Struct containing calculated intermediate values
 * @param[in] voltage - voltage
 * @return Calculated ADC value
 */
/******************************************************************************/
uint16_t VoltageDivider_GetADCRaw(VOLTAGE_DIVIDER_T * div, uint16_t voltage)
{
	return (voltage*div->VPerADCBottom + (div->VPerADCTop/2) )/div->VPerADCTop;
}

void VoltageDivider_InitPercentage(VOLTAGE_DIVIDER_T * div, uint32_t r1Ratio, uint32_t r2Ratio, uint8_t vRef, uint16_t adcMax, uint16_t v100Percent, uint16_t v0Percent)
{
//	div->V100Percent;
//	div->V0Percent;
//	div->VPercentPerADCU;
//	div->V0PercentADCU;
}

