#ifndef _WAVEFORM_H_
#define _WAVEFORM_H_

#include <stdint.h>
#include <stdbool.h>

typedef enum
{
	WAVEFORM_MODE_UNIPOLAR1_T,
	WAVEFORM_MODE_UNIPOLAR2_T,
	WAVEFORM_MODE_BIPOLAR_T
} WAVEFORM_MODE_T;

typedef struct
{
	WAVEFORM_MODE_T WaveformMode;
	uint32_t PWMHalfDuty;

	bool SinusoidalModulation;
	volatile  uint16_t Angle;
	volatile  uint16_t AngleOffset;
//	volatile  uint32_t PWM;
	volatile  uint32_t * HallTimerDelta;
	volatile  uint32_t ISRCount;
	uint32_t AngularSpeedTime; // (384*HallBaseTimerFreq/ISRFreq)

	void (*SetPWMVal)(uint16_t pwmA, uint16_t pwmB, uint16_t pwmC); 	/*< User provides function to set complementary PWM values */
	void (*EnablePWM)(bool a, bool b, bool c);							/*< User provides function to enable each phase */
	void (*InversePWMPolarity)(bool a, bool b, bool c);					/*< 1 is inverse polarity, 0 is positive */

	void (*OnPhaseAB)(void); /*< User provides function e.g. measure current, start ADC*/
	void (*OnPhaseAC)(void);
	void (*OnPhaseBC)(void);
	void (*OnPhaseBA)(void);
	void (*OnPhaseCA)(void);
	void (*OnPhaseCB)(void);
} WAVEFORM_T;


//AngleOffset[degrees] = ISRCount * ISRTime [us] * AngularSpeed [degrees/us]
//AngleOffset[degrees] = ISRCount * ISRTime [us] * CYCLE_DOMAIN [degrees] * CycleFreq [cycles/us]
//AngleOffset[degrees] = ISRCount * ISRTime [us] * CYCLE_DOMAIN [degrees] / CycleTime [us]
static inline void Waveform_ModulateAngleISR(WAVEFORM_T * waveform, uint16_t pwm, bool direction)
{
	if (!waveform->SinusoidalModulation) return;

	AngleOffset = AngularSpeedTime * ISRCount / *HallTimerDelta; //AngleOffset = (384*HallBaseTimerFreq/ISRFreq) * ISRCount / *HallTimerDelta;
	//AngleOffset += AngularSpeedTime / *HallTimerDelta;
	ISRCount++;

	if (AngleOffset > 64) AngleOffset = 64; // angle offset range of 0-64, should not exceed before commutation

	uint8_t sineFactorA;
	uint8_t sineFactorB;
	uint8_t sineFactorC;

	bool invA, invB, invC;

	uint16_t angleA;
	uint16_t angleB;
	uint16_t angleC;

	switch (WaveformMode)
	{
		case WAVEFORM_MODE_UNIPOLAR1_T:
			//if ((32 + Angle + AngleOffset) > 383)  angle = (32 	+ Angle + AngleOffset) - 384
			sineFactorA = SINUSOIDAL_WAVE_TABLE[(PHASE_SHIFT_A + Angle + AngleOffset) % 384];
			sineFactorB = SINUSOIDAL_WAVE_TABLE[(PHASE_SHIFT_B + Angle + AngleOffset) % 384];
			sineFactorC = SINUSOIDAL_WAVE_TABLE[(PHASE_SHIFT_C + Angle + AngleOffset) % 384];

			SetPWMVal
			(
				(sineFactorA+1) * pwm >> SINE_BIT_SHIFT_DIVISOR,
				(sineFactorB+1) * pwm >> SINE_BIT_SHIFT_DIVISOR,
				(sineFactorC+1) * pwm >> SINE_BIT_SHIFT_DIVISOR
			);
			break;

		case WAVEFORM_MODE_UNIPOLAR2_T:
			sineFactorA = SINUSOIDAL_WAVE_TABLE[(PHASE_SHIFT_A + Angle + AngleOffset) % 384];
			sineFactorB = SINUSOIDAL_WAVE_TABLE[(PHASE_SHIFT_B + Angle + AngleOffset) % 384];
			sineFactorC = SINUSOIDAL_WAVE_TABLE[(PHASE_SHIFT_C + Angle + AngleOffset) % 384];

			SetPWMVal
			(
				// pwm factor = sin[-1,1](t) = sin[0,1](t)*2-1 = (sin[0,255](t)*2 - 255)/255
				PWMHalfDuty + ( ((int32_t)(sineFactorA+1)*2 - SINE_ANGLE_RANGE)*pwm >> (SINE_BIT_SHIFT_DIVISOR+1) ),	//PWMHalfDuty + (sineFactorA * 2 - 255) * (pwm/2) / 255,
				PWMHalfDuty + ( ((int32_t)(sineFactorB+1)*2 - SINE_ANGLE_RANGE)*pwm >> (SINE_BIT_SHIFT_DIVISOR+1) ),
				PWMHalfDuty + ( ((int32_t)(sineFactorC+1)*2 - SINE_ANGLE_RANGE)*pwm >> (SINE_BIT_SHIFT_DIVISOR+1) )
			);
			break;

		case WAVEFORM_MODE_BIPOLAR_T:

			angleA = (PHASE_SHIFT_A + Angle + AngleOffset) % 384;
			angleB = (PHASE_SHIFT_B + Angle + AngleOffset) % 384;
			angleC = (PHASE_SHIFT_C + Angle + AngleOffset) % 384;

			// angle[0,191] -> positive, [192,383] -> negative
			invA = (191 < angleA && angleA < 384) ? 0 : 1;
			invB = (191 < angleB && angleB < 384) ? 0 : 1;
			invC = (191 < angleC && angleC < 384) ? 0 : 1;

			sineFactorA = SINUSOIDAL_WAVE_TABLE[angleA % 192];
			sineFactorB = SINUSOIDAL_WAVE_TABLE[angleB % 192];
			sineFactorC = SINUSOIDAL_WAVE_TABLE[angleC % 192];

			InversePWMPolarity(invA, invB, invC); // phase between angle [352, 0, 32] or [160, 192, 224] will change polarity

			SetPWMVal
			(
				PWMHalfDuty + (sineFactorA*pwm >> (SINE_BIT_SHIFT_DIVISOR+1)), // PWMHalfDuty + sineFactorA * (pwm/2) / 255,
				PWMHalfDuty + (sineFactorB*pwm >> (SINE_BIT_SHIFT_DIVISOR+1)),
				PWMHalfDuty + (sineFactorC*pwm >> (SINE_BIT_SHIFT_DIVISOR+1))
			);
			break;
	}
}

void Waveform_CommutatePhaseAB(uint16_t pwm);
void Waveform_CommutatePhaseAC(uint16_t pwm);
void Waveform_CommutatePhaseBC(uint16_t pwm);
void Waveform_CommutatePhaseBA(uint16_t pwm);
void Waveform_CommutatePhaseCA(uint16_t pwm);
void Waveform_CommutatePhaseCB(uint16_t pwm);

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

#endif
