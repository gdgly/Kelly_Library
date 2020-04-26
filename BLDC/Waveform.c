///*
// * Waveform.c
// *
// *  Created on: Apr 25, 2020
// *      Author: FireSourcery
// */
//
//#include <stdint.h>
//
//static uint32_t PWMMax;
//
//static void (*SetPWMVal)(uint16_t pwmA, uint16_t pwmB, uint16_t pwm);
////static void (*EnablePWM)(bool a, bool b, bool c);
////static void (*SetPolarity)(bool a, bool b, bool c);
//
//static void (*OnPhaseAB)(void);
//static void (*OnPhaseAC)(void);
//static void (*OnPhaseBC)(void);
//static void (*OnPhaseBA)(void);
//static void (*OnPhaseCA)(void);
//static void (*OnPhaseCB)(void);
//
//void Unipolar1PhaseAB(uint16_t pwm)
//{
//	SetPWMVal(pwm, 0, 0);
//	//PWM_1A_PIN_SET_CV(pwm);
//	//PWM_1B_PIN_SET_CV(0);
//
//	EN_1CBA_PIN_SET_OUT(0b011);
//	/*SE_1CBA_PIN_SET_OUT(0b010);*/
//	TriggerADCMotor1PhaseC();
//}
//
//void Unipolar2PhaseAB(uint16_t pwm)
//{
//	SetPWMVal(PWM_MAX/2 + pwm/2, PWM_MAX/2 - pwm/2, 0);
////	PWM_1A_PIN_SET_CV(PWM_MAX/2 + pwm/2);
////	PWM_1B_PIN_SET_CV(PWM_MAX/2 - pwm/2);
//
//	EN_1CBA_PIN_SET_OUT(0b011);
//	/*SE_1CBA_PIN_SET_OUT(0b010);*/
//	TriggerADCMotor1PhaseC();
//}
//
//
//void BipolarPhaseAB(uint16_t pwm)
//{
//	SetPWMVal(PWMMax/2 + pwm/2, PWMMax/2 + pwm/2, 0);
////	PWM_1A_PIN_SET_CV(PWM_MAX/2 + pwm/2);
////	PWM_1B_PIN_SET_CV(PWM_MAX/2 + pwm/2);
//
//	OnPhaseAB();
////	PWM_1ABC_PIN_SET_POL(0b010);
////	EN_1CBA_PIN_SET_OUT(0b011);
////	SE_1CBA_PIN_SET_OUT(0b010);
////	TriggerADCMotor1PhaseC();
//}
//
//
//
//// From FastLED
//
//const uint8_t b_m16_interleave[] = { 0, 49, 49, 41, 90, 27, 117, 10 };
//
///// Fast 8-bit approximation of sin(x). This approximation never varies more than
///// 2% from the floating point value you'd get by doing
/////
/////     float s = (sin(x) * 128.0) + 128;
/////
///// @param theta input angle from 0-255
///// @returns sin of theta, value between 0 and 255
//static inline uint8_t sin8_C( uint8_t theta)
//{
//    uint8_t offset = theta;
//    if( theta & 0x40 ) {
//        offset = (uint8_t)255 - offset;
//    }
//    offset &= 0x3F; // 0..63
//
//    uint8_t secoffset  = offset & 0x0F; // 0..15
//    if( theta & 0x40) secoffset++;
//
//    uint8_t section = offset >> 4; // 0..3
//    uint8_t s2 = section * 2;
//    const uint8_t* p = b_m16_interleave;
//    p += s2;
//    uint8_t b   =  *p;
//    p++;
//    uint8_t m16 =  *p;
//
//    uint8_t mx = (m16 * secoffset) >> 4;
//
//    int8_t y = mx + b;
//    if( theta & 0x80 ) y = -y;
//
//    y += 128;
//
//    return y;
//}
//
//
//
////const uint8_t SIN8[360]={
////  0,   0,   0,   0,   0,   1,   1,   2,
////  2,   3,   4,   5,   6,   7,   8,   9,
//// 11,  12,  13,  15,  17,  18,  20,  22,
//// 24,  26,  28,  30,  32,  35,  37,  39,
//// 42,  44,  47,  49,  52,  55,  58,  60,
//// 63,  66,  69,  72,  75,  78,  81,  85,
//// 88,  91,  94,  97, 101, 104, 107, 111,
////114, 117, 121, 124, 127, 131, 134, 137,
////141, 144, 147, 150, 154, 157, 160, 163,
////167, 170, 173, 176, 179, 182, 185, 188,
////191, 194, 197, 200, 202, 205, 208, 210,
////213, 215, 217, 220, 222, 224, 226, 229,
////231, 232, 234, 236, 238, 239, 241, 242,
////244, 245, 246, 248, 249, 250, 251, 251,
////252, 253, 253, 254, 254, 255, 255, 255,
////255, 255, 255, 255, 254, 254, 253, 253,
////252, 251, 251, 250, 249, 248, 246, 245,
////244, 242, 241, 239, 238, 236, 234, 232,
////231, 229, 226, 224, 222, 220, 217, 215,
////213, 210, 208, 205, 202, 200, 197, 194,
////191, 188, 185, 182, 179, 176, 173, 170,
////167, 163, 160, 157, 154, 150, 147, 144,
////141, 137, 134, 131, 127, 124, 121, 117,
////114, 111, 107, 104, 101,  97,  94,  91,
//// 88,  85,  81,  78,  75,  72,  69,  66,
//// 63,  60,  58,  55,  52,  49,  47,  44,
//// 42,  39,  37,  35,  32,  30,  28,  26,
//// 24,  22,  20,  18,  17,  15,  13,  12,
//// 11,   9,   8,   7,   6,   5,   4,   3,
////  2,   2,   1,   1,   0,   0,   0,   0,
////  0,   0,   0,   0,   0,   0,   0,   0,
////  0,   0,   0,   0,   0,   0,   0,   0,
////  0,   0,   0,   0,   0,   0,   0,   0,
////  0,   0,   0,   0,   0,   0,   0,   0,
////  0,   0,   0,   0,   0,   0,   0,   0,
////  0,   0,   0,   0,   0,   0,   0,   0,
////  0,   0,   0,   0,   0,   0,   0,   0,
////  0,   0,   0,   0,   0,   0,   0,   0,
////  0,   0,   0,   0,   0,   0,   0,   0,
////  0,   0,   0,   0,   0,   0,   0,   0,
////  0,   0,   0,   0,   0,   0,   0,   0,
////  0,   0,   0,   0,   0,   0,   0,   0,
////  0,   0,   0,   0,   0,   0,   0,   0,
////  0,   0,   0,   0,   0,   0,   0,   0,
////  0,   0,   0,   0,   0,   0,   0,   0};
//
//static unsigned char SinusoidalWaveTable[384]=
//{
//	127,131,135,138,142,145,149,152,155,159,162,165,168,171,174,177,180,183,186,189,192,194,197,200,
//	202,205,207,210,212,214,217,219,221,223,225,227,229,231,232,234,236,237,239,240,242,243,244,245,
//	247,248,249,250,250,251,252,253,253,254,254,255,255,255,255,255,255,255,255,255,255,255,254,254,
//	253,253,252,252,251,250,249,248,247,246,245,244,242,241,240,238,237,235,233,232,230,228,226,224,
//	222,222,225,226,228,230,232,234,235,237,238,240,241,243,244,245,246,247,248,249,250,251,252,252,
//	253,254,254,254,255,255,255,255,255,255,255,255,255,255,254,254,254,253,252,252,251,250,249,248,
//	247,246,245,244,243,241,240,238,237,235,234,232,230,228,227,225,223,220,218,216,214,212,209,207,
//	204,202,199,197,194,191,188,185,183,180,177,174,171,167,164,161,158,154,151,148,144,141,137,134,
//	130,127,123,119,116,112,108,104,101, 97, 93, 89, 85, 81, 77, 73, 69, 65, 61, 57, 53, 49, 45, 41,
//	36, 32, 28, 24, 20, 16, 12, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 6, 10, 15, 19, 23, 27, 31,
//	35, 39, 44, 48, 52, 56, 60, 64, 68, 72, 76, 80, 84, 88, 92, 96, 99,103,107,111,115,118,121,124
//};
//
//void SinusoidalAvgPhaseAB(uint16_t pwm)
//{
//	SetPWMVal
//	(
//		SinusoidalWaveTable[64] *(PWM_MAX*PWM_MAX/255)*pwm,
//		SinusoidalWaveTable[320]*(PWM_MAX*PWM_MAX/255)*pwm,
//		SinusoidalWaveTable[192]*(PWM_MAX*PWM_MAX/255)*pwm
//	);
//	PWM_1A_PIN_SET_CV(SinusoidalWaveTable[64] *2*pwm/PWM_MAX);
//	PWM_1B_PIN_SET_CV(SinusoidalWaveTable[320]*2*pwm/PWM_MAX);
//	PWM_1C_PIN_SET_CV(SinusoidalWaveTable[192]*2*pwm/PWM_MAX);
//	EN_1CBA_PIN_SET_OUT(0b111);
//	/*SE_1CBA_PIN_SET_OUT(0b010);*/
//	TriggerADCMotor1PhaseC();
//}
//
//
//
//void ModulateSinusoidalAngle(void)
//{
//	AngleOffset = (384*FTM1_FREQ/FTM2_ISR_FREQ) * Count51us / Speed_GetDelta(&Motor1Speed); // angle offset range of 0-64
//
//	if (AngleOffset > 64) AngleOffset = 64;
//	Count51us++;
//
//	SetPWMVal
//	(
//		SinusoidalWaveTable[(Angle + 32  + AngleOffset) % 384]*2 * PWMRatio/PWM_MAX,
//		SinusoidalWaveTable[(Angle + 288 + AngleOffset) % 384]*2 * PWMRatio/PWM_MAX,
//		SinusoidalWaveTable[(Angle + 160 + AngleOffset) % 384]*2 * PWMRatio/PWM_MAX
//	);
//
//	//if (Angle > 383)  angle = angle mod 384
//	//PWM_1A_PIN_SET_CV(SinusoidalWaveTable[(Angle + 32  + AngleOffset) % 384]*2 * PWMRatio/PWM_MAX);
//	//PWM_1B_PIN_SET_CV(SinusoidalWaveTable[(Angle + 288 + AngleOffset) % 384]*2 * PWMRatio/PWM_MAX);
//	//PWM_1C_PIN_SET_CV(SinusoidalWaveTable[(Angle + 160 + AngleOffset) % 384]*2 * PWMRatio/PWM_MAX);
//}
//
//void SinusoidalPhaseAB(uint16_t pwm)
//{
//	Count51us = 0;
//	Angle = 0;
//	PWMRatio = pwm;
//	EN_1CBA_PIN_SET_OUT(0b111);
//	SE_1CBA_PIN_SET_OUT(0b010);
//	TriggerADCMotor1PhaseC();
//}
