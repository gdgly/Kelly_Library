#ifndef _WAVEFORM_H_
#define _WAVEFORM_H_

#include <stdint.h>
#include <stdbool.h>

#define SINE_ANGLE_MAX 	383
#define SINE_ANGLE_SIZE	384
#define SINE_RANGE_MAX 	255
#define SINE_RANGE_SIZE	256
#define SINE_BIT_SHIFT_DIVISOR 8 // divide by 256
#define PHASE_SHIFT_A 32
#define PHASE_SHIFT_B 288
#define PHASE_SHIFT_C 160

#define REFERENCE_ANGLE_OFFSET_MAX 64 // delta angle within 1 phase, should not be exceeded before commutation

#define REFERENCE_ANGLE_PHASE_AB 0 // reference angle resets to this value at commutation
#define REFERENCE_ANGLE_PHASE_AC 64
#define REFERENCE_ANGLE_PHASE_BC 128
#define REFERENCE_ANGLE_PHASE_BA 192
#define REFERENCE_ANGLE_PHASE_CA 256
#define REFERENCE_ANGLE_PHASE_CB 320

//static inline uint32_t IntegerBitShiftDividePowerOf2(uint32_t dividend, uint32_t divisor)
//{
//	(dividend + ((dividend >> 31) & ((1 << divisor) + ~0))) >> divisor;
//}

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
	uint32_t AngularSpeedTime; // (384*HallBaseTimerFreq/ISRFreq)

	volatile  uint16_t Angle;
	volatile  uint16_t AngleOffset;
//	volatile  uint32_t PWM;
	volatile  uint32_t * HallTimerDelta;
	volatile  uint32_t ISRCount;


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

extern const uint8_t SINUSOIDAL_WAVE_TABLE[SINE_ANGLE_SIZE];

//AngleOffset[degrees] = ISRCount * ISRTime [us] * AngularSpeed [degrees/us]
//AngleOffset[degrees] = ISRCount * ISRTime [us] * CYCLE_DOMAIN [degrees] * CycleFreq [cycles/us]
//AngleOffset[degrees] = ISRCount * ISRTime [us] * CYCLE_DOMAIN [degrees] / CycleTime [us]
static inline void Waveform_ModulateAngleISR(WAVEFORM_T * waveform, uint16_t pwm, bool direction)
{
	if (!waveform->SinusoidalModulation) return;

	//AngleOffset += AngularSpeedTime / *HallTimerDelta;
	waveform->AngleOffset = waveform->AngularSpeedTime * waveform->ISRCount / *(waveform->HallTimerDelta); //AngleOffset = (384*HallBaseTimerFreq/ISRFreq) * ISRCount / *HallTimerDelta;
	waveform->ISRCount++;

	if (waveform->AngleOffset > REFERENCE_ANGLE_OFFSET_MAX) waveform->AngleOffset = REFERENCE_ANGLE_OFFSET_MAX; // angle offset range of 0-64, should not exceed 64 before commutation

	uint8_t sineOfA;
	uint8_t sineOfB;
	uint8_t sineOfC;

	bool invA, invB, invC;

	uint16_t angleA;
	uint16_t angleB;
	uint16_t angleC;

	angleA = PHASE_SHIFT_A + waveform->Angle + waveform->AngleOffset;
	if (angleA > SINE_ANGLE_MAX) angleA = angleA - SINE_ANGLE_SIZE; // angle should not exceed 2*SINE_ANGLE_DOMAIN_SIZE

	angleB = PHASE_SHIFT_B + waveform->Angle + waveform->AngleOffset;
	if (angleB > SINE_ANGLE_MAX) angleB = angleB - SINE_ANGLE_SIZE;

	angleC = PHASE_SHIFT_C + waveform->Angle + waveform->AngleOffset;
	if (angleC > SINE_ANGLE_MAX) angleC = angleC - SINE_ANGLE_SIZE;

	switch (waveform->WaveformMode)
	{
		case WAVEFORM_MODE_UNIPOLAR1_T:
//			sineFactorA = SINUSOIDAL_WAVE_TABLE[ (PHASE_SHIFT_A + waveform->Angle + waveform->AngleOffset) % 384 ];
//			sineFactorB = SINUSOIDAL_WAVE_TABLE[ (PHASE_SHIFT_B + waveform->Angle + waveform->AngleOffset) % 384 ];
//			sineFactorC = SINUSOIDAL_WAVE_TABLE[ (PHASE_SHIFT_C + waveform->Angle + waveform->AngleOffset) % 384 ];
			sineOfA = SINUSOIDAL_WAVE_TABLE[angleA];
			sineOfB = SINUSOIDAL_WAVE_TABLE[angleB];
			sineOfC = SINUSOIDAL_WAVE_TABLE[angleC];

			waveform->SetPWMVal
			(
				(sineOfA * pwm) / SINE_RANGE_MAX,
				(sineOfB * pwm) / SINE_RANGE_MAX,
				(sineOfC * pwm) / SINE_RANGE_MAX
			);
			break;

		case WAVEFORM_MODE_UNIPOLAR2_T:
//			sineFactorA = SINUSOIDAL_WAVE_TABLE[ (PHASE_SHIFT_A + waveform->Angle + waveform->AngleOffset) % 384 ];
//			sineFactorB = SINUSOIDAL_WAVE_TABLE[ (PHASE_SHIFT_B + waveform->Angle + waveform->AngleOffset) % 384 ];
//			sineFactorC = SINUSOIDAL_WAVE_TABLE[ (PHASE_SHIFT_C + waveform->Angle + waveform->AngleOffset) % 384 ];

			sineOfA = SINUSOIDAL_WAVE_TABLE[angleA];
			sineOfB = SINUSOIDAL_WAVE_TABLE[angleB];
			sineOfC = SINUSOIDAL_WAVE_TABLE[angleC];

			waveform->SetPWMVal
			(
				// pwm factor = sin[-1,1](t) = sin[0,1](t)*2-1 = (sin[0,255](t)*2 - 255)/255
				waveform->PWMHalfDuty + ( (((int32_t)sineOfA - SINE_RANGE_MAX / 2) * pwm) / SINE_RANGE_MAX ),	//PWMHalfDuty + (sineFactorA * 2 - 255) * (pwm/2) / 255,
				waveform->PWMHalfDuty + ( (((int32_t)sineOfB - SINE_RANGE_MAX / 2) * pwm) / SINE_RANGE_MAX ),
				waveform->PWMHalfDuty + ( (((int32_t)sineOfC - SINE_RANGE_MAX / 2) * pwm) / SINE_RANGE_MAX )
			);
			break;

		case WAVEFORM_MODE_BIPOLAR_T:
//			angleA = (PHASE_SHIFT_A + waveform->Angle + waveform->AngleOffset) % SINE_ANGLE_DOMAIN_MAX;
//			angleB = (PHASE_SHIFT_B + waveform->Angle + waveform->AngleOffset) % SINE_ANGLE_DOMAIN_MAX;
//			angleC = (PHASE_SHIFT_C + waveform->Angle + waveform->AngleOffset) % SINE_ANGLE_DOMAIN_MAX;

			// angle[0,191] -> positive, [192,383] -> negative
//
//			invB = (angleB > 191 /*&& angleB < 384*/) ? 1 : 0;
//			invC = (angleC > 191 /*&& angleC < 384*/) ? 1 : 0;

//			sineOfA = SINUSOIDAL_WAVE_TABLE[angleA % 192];
//			sineOfB = SINUSOIDAL_WAVE_TABLE[angleB % 192];
//			sineOfC = SINUSOIDAL_WAVE_TABLE[angleC % 192];

			if (angleA > SINE_ANGLE_MAX/2)
			{
				invA = true;
				sineOfA = SINUSOIDAL_WAVE_TABLE[angleA - SINE_ANGLE_MAX/2];
			}
			else
			{
				invA = false;
				sineOfA = SINUSOIDAL_WAVE_TABLE[angleA];
			}

			if (angleB > SINE_ANGLE_MAX/2)
			{
				invB = true;
				sineOfB = SINUSOIDAL_WAVE_TABLE[angleB - SINE_ANGLE_MAX/2];
			}
			else
			{
				invB = false;
				sineOfB = SINUSOIDAL_WAVE_TABLE[angleB];
			}

			if (angleC > SINE_ANGLE_MAX/2)
			{
				invC = true;
				sineOfC = SINUSOIDAL_WAVE_TABLE[angleC - SINE_ANGLE_MAX/2];
			}
			else
			{
				invC = false;
				sineOfC = SINUSOIDAL_WAVE_TABLE[angleC];
			}

			waveform->InversePWMPolarity(invA, invB, invC); // phase between angle [352, 0, 32] or [160, 192, 224] will change polarity

			waveform->SetPWMVal
			(
				waveform->PWMHalfDuty + (sineOfA * pwm / (SINE_RANGE_MAX * 2)), // PWMHalfDuty + sineFactorA * (pwm/2) / 255,
				waveform->PWMHalfDuty + (sineOfB * pwm / (SINE_RANGE_MAX * 2)),
				waveform->PWMHalfDuty + (sineOfC * pwm / (SINE_RANGE_MAX * 2))
			);
			break;
	}
}

void Waveform_CommutatePhaseAB(WAVEFORM_T * waveform, uint16_t pwm);
void Waveform_CommutatePhaseAC(WAVEFORM_T * waveform, uint16_t pwm);
void Waveform_CommutatePhaseBC(WAVEFORM_T * waveform, uint16_t pwm);
void Waveform_CommutatePhaseBA(WAVEFORM_T * waveform, uint16_t pwm);
void Waveform_CommutatePhaseCA(WAVEFORM_T * waveform, uint16_t pwm);
void Waveform_CommutatePhaseCB(WAVEFORM_T * waveform, uint16_t pwm);

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
