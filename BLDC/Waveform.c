#include <stdint.h>
#include <stdbool.h>

static void (*SetPWMVal)(uint16_t pwmA, uint16_t pwmB, uint16_t pwmC); 	/*< User provides function to set complementary PWM values */
static void (*EnablePWM)(bool a, bool b, bool c);						/*< User provides function to enable each phase */
static void (*InversePWMPolarity)(bool a, bool b, bool c);				/*< 1 is inverse polarity, 0 is positive */

static void (*OnPhaseAB)(void); /*< User provides function e.g. measure current, start ADC*/
static void (*OnPhaseAC)(void);
static void (*OnPhaseBC)(void);
static void (*OnPhaseBA)(void);
static void (*OnPhaseCA)(void);
static void (*OnPhaseCB)(void);

#define SINE_ANGLE_DOMAIN 	383
#define SINE_ANGLE_RANGE 	255
#define SINE_BIT_SHIFT_DIVISOR 8
#define PHASE_SHIFT_A 32
#define PHASE_SHIFT_B 288
#define PHASE_SHIFT_C 160

//Phase boundaries 32, 96, 160, 224, 288, 352
//Phase midpoints  64, 128, 192, 256, 320, 0

//PhaseAB Angle 	A:32-96 	B:288-352 	C:160-224
//PhaseAC Angle 	A:96-160	B:352-32 	C:224-288
//PhaseBC Angle 	A:160-224	B:32-96 	C:288-352
//PhaseBA Angle 	A:224-288	B:160-224	C:352-32
//PhaseCA Angle 	A:288-352	B:224-288 	C:32-96
//PhaseCB Angle 	A:352-32	B:352-32 	C:96-160

static const uint8_t SinusoidalWaveTable[384]=
{
	127,131,135,138,142,145,149,152,155,159,162,165,168,171,174,177,180,183,186,189,192,194,197,200,202,205,207,210,212,214,217,219,
	221,223,225,227,229,231,232,234,236,237,239,240,242,243,244,245,247,248,249,250,250,251,252,253,253,254,254,255,255,255,255,255,
	255,255,255,255,255,255,254,254,253,253,252,252,251,250,249,248,247,246,245,244,242,241,240,238,237,235,233,232,230,228,226,224,
	222,222,225,226,228,230,232,234,235,237,238,240,241,243,244,245,246,247,248,249,250,251,252,252,253,254,254,254,255,255,255,255,
	255,255,255,255,255,255,254,254,254,253,252,252,251,250,249,248,247,246,245,244,243,241,240,238,237,235,234,232,230,228,227,225,
	223,220,218,216,214,212,209,207,204,202,199,197,194,191,188,185,183,180,177,174,171,167,164,161,158,154,151,148,144,141,137,134,
	130,127,123,119,116,112,108,104,101, 97, 93, 89, 85, 81, 77, 73, 69, 65, 61, 57, 53, 49, 45, 41, 36, 32, 28, 24, 20, 16, 12,  7,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  2,  6, 10, 15, 19, 23, 27, 31, 35, 39, 44, 48, 52, 56, 60, 64, 68, 72, 76, 80, 84, 88, 92, 96, 99,103,107,111,115,118,121,124
};

typedef enum
{
	WAVEFORM_MODE_UNIPOLAR1_T,
	WAVEFORM_MODE_UNIPOLAR2_T,
	WAVEFORM_MODE_BIPOLAR_T
} WAVEFORM_MODE_T;

WAVEFORM_MODE_T WaveformMode;

static bool SinusoidalModulation;

volatile static uint32_t * HallTimerDelta;
static uint32_t AngularSpeedTime; // (384*TimerCounterFreq/ISRFreq)
static uint32_t PWMHalfDuty;

volatile static uint16_t Angle;
volatile static uint16_t AngleOffset;
volatile static uint32_t ISRCount;
//volatile static uint32_t PWM;

static inline void SinusoidalPhaseAB(void) {Angle = 0;   ISRCount = 0;};
static inline void SinusoidalPhaseAC(void) {Angle = 64;  ISRCount = 0;};
static inline void SinusoidalPhaseBC(void) {Angle = 128; ISRCount = 0;};
static inline void SinusoidalPhaseBA(void) {Angle = 192; ISRCount = 0;};
static inline void SinusoidalPhaseCA(void) {Angle = 256; ISRCount = 0;};
static inline void SinusoidalPhaseCB(void) {Angle = 320; ISRCount = 0;};


//AngleOffset[degrees] = ISRCount * ISRTime [us] * AngularSpeed [degrees/us]
//AngleOffset[degrees] = ISRCount * ISRTime [us] * CYCLE_DOMAIN [degrees] * CycleFreq [cycles/us]
//AngleOffset[degrees] = ISRCount * ISRTime [us] * CYCLE_DOMAIN [degrees] / CycleTime [us]
void Waveform_ModulateAngleISR(uint16_t pwm, bool direction)
{
	if (!SinusoidalModulation) return;

	AngleOffset = AngularSpeedTime * ISRCount / *HallTimerDelta; //AngleOffset = (384*TimerCounterFreq/ISRFreq) * ISRCount / Speed_GetDelta(&Motor1Speed);
	//AngleOffset += AngularSpeedTime / *HallTimerDelta;
	ISRCount++;

	if (AngleOffset > 64) AngleOffset = 64; // angle offset range of 0-64, should not exceed before commutation


	uint8_t sineFactorA;
	uint8_t sineFactorB;
	uint8_t sineFactorC;

	bool invA, invB, invC;

	switch (WaveformMode)
	{
		case WAVEFORM_MODE_UNIPOLAR1_T:
			//if ((32 + Angle + AngleOffset) > 383)  angle = (32 	+ Angle + AngleOffset) - 384
			sineFactorA = SinusoidalWaveTable[(PHASE_SHIFT_A + Angle + AngleOffset) % 384];
			sineFactorB = SinusoidalWaveTable[(PHASE_SHIFT_B + Angle + AngleOffset) % 384];
			sineFactorC = SinusoidalWaveTable[(PHASE_SHIFT_C + Angle + AngleOffset) % 384];

			SetPWMVal
			(
				(sineFactorA+1) * pwm >> SINE_BIT_SHIFT_DIVISOR,
				(sineFactorB+1) * pwm >> SINE_BIT_SHIFT_DIVISOR,
				(sineFactorC+1) * pwm >> SINE_BIT_SHIFT_DIVISOR
			);
			break;

		case WAVEFORM_MODE_UNIPOLAR2_T:
			sineFactorA = SinusoidalWaveTable[(PHASE_SHIFT_A + Angle + AngleOffset) % 384];
			sineFactorB = SinusoidalWaveTable[(PHASE_SHIFT_B + Angle + AngleOffset) % 384];
			sineFactorC = SinusoidalWaveTable[(PHASE_SHIFT_C + Angle + AngleOffset) % 384];

			SetPWMVal
			(
				// pwm factor = sin[-1,1](t) = sin[0,1](t)*2-1 = (sin[0,255](t)*2 - 255)/255
				PWMHalfDuty + ( ((int32_t)(sineFactorA+1)*2 - SINE_ANGLE_RANGE)*pwm >> (SINE_BIT_SHIFT_DIVISOR+1) ),	//PWMHalfDuty + (sineFactorA * 2 - 255) * (pwm/2) / 255,
				PWMHalfDuty + ( ((int32_t)(sineFactorB+1)*2 - SINE_ANGLE_RANGE)*pwm >> (SINE_BIT_SHIFT_DIVISOR+1) ),
				PWMHalfDuty + ( ((int32_t)(sineFactorC+1)*2 - SINE_ANGLE_RANGE)*pwm >> (SINE_BIT_SHIFT_DIVISOR+1) )
			);
			break;

		case WAVEFORM_MODE_BIPOLAR_T:

			// angle[0,191] -> positive, [192,383] -> negative
			invA = (191 < PHASE_SHIFT_A + Angle + AngleOffset && PHASE_SHIFT_A + Angle + AngleOffset < 384) ? 0 : 1;
			invB = (191 < PHASE_SHIFT_B + Angle + AngleOffset && PHASE_SHIFT_B + Angle + AngleOffset < 384) ? 0 : 1;
			invC = (191 < PHASE_SHIFT_C + Angle + AngleOffset && PHASE_SHIFT_C + Angle + AngleOffset < 384) ? 0 : 1;

			sineFactorA = SinusoidalWaveTable[(PHASE_SHIFT_A + Angle + AngleOffset) % 192];
			sineFactorB = SinusoidalWaveTable[(PHASE_SHIFT_B + Angle + AngleOffset) % 192];
			sineFactorC = SinusoidalWaveTable[(PHASE_SHIFT_C + Angle + AngleOffset) % 192];

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

void Waveform_Init
(
	void (*setPWMVal)(uint16_t pwmA, uint16_t pwmB, uint16_t pwmC),
	void (*enablePWM)(bool a, bool b, bool c),
	void (*invPWMPolarity)(bool a, bool b, bool c),

	void (*onPhaseAB)(void),
	void (*onPhaseAC)(void),
	void (*onPhaseBC)(void),
	void (*onPhaseBA)(void),
	void (*onPhaseCA)(void),
	void (*onPhaseCB)(void)
)
{
	WaveformMode = WAVEFORM_MODE_UNIPOLAR1_T;

	SetPWMVal = setPWMVal;
	EnablePWM = enablePWM;
	InversePWMPolarity = invPWMPolarity;
	OnPhaseAB = onPhaseAB;
	OnPhaseAC = onPhaseAC;
	OnPhaseBC = onPhaseBC;
	OnPhaseBA = onPhaseBA;
	OnPhaseCA = onPhaseCA;
	OnPhaseCB = onPhaseCB;
}

void Waveform_InitSinusoidalModulation
(
	void (*setPWMVal)(uint16_t pwmA, uint16_t pwmB, uint16_t pwmC),
	void (*enablePWM)(bool a, bool b, bool c),
	void (*invPWMPolarity)(bool a, bool b, bool c),

	void (*onPhaseAB)(void),
	void (*onPhaseAC)(void),
	void (*onPhaseBC)(void),
	void (*onPhaseBA)(void),
	void (*onPhaseCA)(void),
	void (*onPhaseCB)(void),

	volatile uint32_t * hallTimerDelta,
	uint32_t hallTimerFreq,
	uint32_t isrFreq
)
{
	Waveform_Init
	(
		setPWMVal,
		enablePWM,
		invPWMPolarity,
		onPhaseAB,
		onPhaseAC,
		onPhaseBC,
		onPhaseBA,
		onPhaseCA,
		onPhaseCB
	);

	AngularSpeedTime = 384*hallTimerFreq/isrFreq;
	HallTimerDelta = hallTimerDelta;
}

void Waveform_EnableSinusoidalModulation(void)
{
	SinusoidalModulation = true;
	EnablePWM(1, 1, 1);
}

void Waveform_DisableSinusoidalModulation(void)
{
	SinusoidalModulation = false;
	EnablePWM(0, 0, 0);
	InversePWMPolarity(0,0,0);
}

void Waveform_SetMode(WAVEFORM_MODE_T waveformMode)
{
	WaveformMode = waveformMode;
	EnablePWM(0, 0, 0);

	switch (WaveformMode)
	{
		case WAVEFORM_MODE_UNIPOLAR1_T:
			InversePWMPolarity(0,0,0);
			break;

		case WAVEFORM_MODE_UNIPOLAR2_T:
			InversePWMPolarity(0,0,0);
			break;

		case WAVEFORM_MODE_BIPOLAR_T:
			break;
	}
}


void Waveform_CommutatePhaseAB(uint16_t pwm)
{
	if (SinusoidalModulation)
	{
		SinusoidalPhaseAB();
	}
	else
	{
		switch (WaveformMode)
		{
			case WAVEFORM_MODE_UNIPOLAR1_T: SetPWMVal(pwm, 0, 0); 																break;
			case WAVEFORM_MODE_UNIPOLAR2_T: SetPWMVal(PWMHalfDuty + pwm/2, PWMHalfDuty - pwm/2, 0);								break;
			case WAVEFORM_MODE_BIPOLAR_T: 	SetPWMVal(PWMHalfDuty + pwm/2, PWMHalfDuty + pwm/2, 0); InversePWMPolarity(0,1,0); 	break;
		}
		EnablePWM(1, 1, 0);
	}
	OnPhaseAB();
}

void Waveform_CommutatePhaseAC(uint16_t pwm)
{
	if (SinusoidalModulation)
	{
		SinusoidalPhaseAC();
	}
	else
	{
		switch (WaveformMode)
		{
			case WAVEFORM_MODE_UNIPOLAR1_T: SetPWMVal(pwm, 0, 0); 																break;
			case WAVEFORM_MODE_UNIPOLAR2_T: SetPWMVal(PWMHalfDuty + pwm/2, 0, PWMHalfDuty - pwm/2);								break;
			case WAVEFORM_MODE_BIPOLAR_T: 	SetPWMVal(PWMHalfDuty + pwm/2, 0, PWMHalfDuty + pwm/2); InversePWMPolarity(0,0,1); 	break;
		}
		EnablePWM(1, 0, 1);
	}
	OnPhaseAC();
}

void Waveform_CommutatePhaseBC(uint16_t pwm)
{
	if (SinusoidalModulation)
	{
		Angle = 128;
		ISRCount = 0;
	}
	else
	{
		switch (WaveformMode)
		{
			case WAVEFORM_MODE_UNIPOLAR1_T: SetPWMVal(0, pwm, 0); 																break;
			case WAVEFORM_MODE_UNIPOLAR2_T: SetPWMVal(0, PWMHalfDuty + pwm/2, PWMHalfDuty - pwm/2);								break;
			case WAVEFORM_MODE_BIPOLAR_T: 	SetPWMVal(0, PWMHalfDuty + pwm/2, PWMHalfDuty + pwm/2); InversePWMPolarity(0,0,1); 	break;
		}
		EnablePWM(0, 1, 1);
	}
	OnPhaseBC();
}

void Waveform_CommutatePhaseBA(uint16_t pwm)
{
	if (SinusoidalModulation)
	{
		Angle = 192;
		ISRCount = 0;
	}
	else
	{
		switch (WaveformMode)
		{
			case WAVEFORM_MODE_UNIPOLAR1_T: SetPWMVal(0, pwm, 0); 																break;
			case WAVEFORM_MODE_UNIPOLAR2_T: SetPWMVal(PWMHalfDuty - pwm/2, PWMHalfDuty + pwm/2, 0);								break;
			case WAVEFORM_MODE_BIPOLAR_T: 	SetPWMVal(PWMHalfDuty + pwm/2, PWMHalfDuty + pwm/2, 0); InversePWMPolarity(1,0,0); 	break;
		}
		EnablePWM(1, 1, 0);
	}
	OnPhaseBA();
}

void Waveform_CommutatePhaseCA(uint16_t pwm)
{
	if (SinusoidalModulation)
	{
		Angle = 256;
		ISRCount = 0;
	}
	else
	{
		switch (WaveformMode)
		{
			case WAVEFORM_MODE_UNIPOLAR1_T: SetPWMVal(0, 0, pwm); 																break;
			case WAVEFORM_MODE_UNIPOLAR2_T: SetPWMVal(PWMHalfDuty - pwm/2, 0, PWMHalfDuty + pwm/2);								break;
			case WAVEFORM_MODE_BIPOLAR_T: 	SetPWMVal(PWMHalfDuty + pwm/2, 0, PWMHalfDuty + pwm/2); InversePWMPolarity(1,0,0); 	break;
		}
		EnablePWM(1, 0, 1);
	}
	OnPhaseCA();
}

void Waveform_CommutatePhaseCB(uint16_t pwm)
{
	if (SinusoidalModulation)
	{
		Angle = 320;
		ISRCount = 0;
	}
	else
	{
		switch (WaveformMode)
		{
			case WAVEFORM_MODE_UNIPOLAR1_T: SetPWMVal(0, 0, pwm); 																break;
			case WAVEFORM_MODE_UNIPOLAR2_T: SetPWMVal(0, PWMHalfDuty - pwm/2, PWMHalfDuty + pwm/2);								break;
			case WAVEFORM_MODE_BIPOLAR_T: 	SetPWMVal(0, PWMHalfDuty + pwm/2, PWMHalfDuty + pwm/2); InversePWMPolarity(0,1,0); 	break;
		}
		EnablePWM(0, 1, 1);
	}
	OnPhaseCB();
}




//// On commutation
//void Unipolar1PhaseAB(uint16_t pwm)
//{
//	SetPWMVal(pwm, 0, 0);
//	//PWM_1A_PIN_SET_CV(pwm);
//	//PWM_1B_PIN_SET_CV(0);
//
//	OnPhaseAB();
//	EN_1CBA_PIN_SET_OUT(0b011);
//	SE_1CBA_PIN_SET_OUT(0b010);
//	TriggerADCMotor1PhaseC();
//}
//
//void Unipolar2PhaseAB(uint16_t pwm)
//{
//	SetPWMVal(PWM_MAX/2 + pwm/2, PWM_MAX/2 - pwm/2, 0);
////	PWM_1A_PIN_SET_CV(PWM_MAX/2 + pwm/2);
////	PWM_1B_PIN_SET_CV(PWM_MAX/2 - pwm/2);
//
//	OnPhaseAB();
//	EN_1CBA_PIN_SET_OUT(0b011);
//	SE_1CBA_PIN_SET_OUT(0b010);
//	TriggerADCMotor1PhaseC();
//}
//

//void SinusoidalAvgPhaseAB(uint16_t pwm)
//{
//	SetPWMVal
//	(
//		SinusoidalWaveTable[64] *pwm/255,
//		SinusoidalWaveTable[320]*pwm/255,
//		SinusoidalWaveTable[192]*pwm/255
//	);
//	PWM_1A_PIN_SET_CV(SinusoidalWaveTable[64] *2*pwm/PWM_MAX);
//	PWM_1B_PIN_SET_CV(SinusoidalWaveTable[320]*2*pwm/PWM_MAX);
//	PWM_1C_PIN_SET_CV(SinusoidalWaveTable[192]*2*pwm/PWM_MAX);
//	EN_1CBA_PIN_SET_OUT(0b111);
//	/*SE_1CBA_PIN_SET_OUT(0b010);*/
//	TriggerADCMotor1PhaseC();
//}

////void CommutateBipolarMotor1PhaseAB(uint16_t pwm) {PWM_1ABC_PIN_SET_POL(0b010); PWM_1A_PIN_SET_CV(PWM_MAX/2 + pwm/2); PWM_1B_PIN_SET_CV(PWM_MAX/2 + pwm/2); EN_1CBA_PIN_SET_OUT(0b011); SE_1CBA_PIN_SET_OUT(0b010); TriggerADCMotor1PhaseC();}
//void BipolarPhaseAB(uint16_t pwm)
//{
//	if (SinusoidalModulation)
//	{
//		SinusoidalPhaseAB();
//	}
//	else
//	{
//		SetPWMVal(PWMHalfDuty + pwm/2, PWMHalfDuty + pwm/2, 0);
//		EnablePWM(1, 1, 0);
//		SetPolarity(1, 0, 1); //C is dont care for non sinusoidal
//	}
//
//	OnPhaseAB();
//}
//
////void CommutateBipolarMotor1PhaseAC(uint16_t pwm) {PWM_1ABC_PIN_SET_POL(0b001); PWM_1A_PIN_SET_CV(PWM_MAX/2 + pwm/2); PWM_1C_PIN_SET_CV(PWM_MAX/2 + pwm/2); EN_1CBA_PIN_SET_OUT(0b101); SE_1CBA_PIN_SET_OUT(0b100); TriggerADCMotor1PhaseB();}
//void BipolarPhaseAC(uint16_t pwm)
//{
//	if (SinusoidalModulation)
//	{
//		SinusoidalPhaseAC();
//	}
//	else
//	{
//		SetPWMVal(PWMHalfDuty + pwm/2, 0, PWMHalfDuty + pwm/2);
//		EnablePWM(1, 0, 1);
//		SetPolarity(1, 1, 0); //B is dont care for non sinusoidal
//	}
//
//	OnPhaseAC();
//}
//
////void CommutateBipolarMotor1PhaseBC(uint16_t pwm) {PWM_1ABC_PIN_SET_POL(0b001); PWM_1B_PIN_SET_CV(PWM_MAX/2 + pwm/2); PWM_1C_PIN_SET_CV(PWM_MAX/2 + pwm/2); EN_1CBA_PIN_SET_OUT(0b110); SE_1CBA_PIN_SET_OUT(0b100); TriggerADCMotor1PhaseA();}
//void BipolarPhaseBC(uint16_t pwm)
//{
//	if (SinusoidalModulation)
//	{
//		SinusoidalPhaseBC();
//	}
//	else
//	{
//		SetPWMVal(0, PWMHalfDuty + pwm/2, PWMHalfDuty + pwm/2);
//		EnablePWM(0, 1, 1);
//		SetPolarity(1, 1, 0); //A is dont care for non sinusoidal
//	}
//
//	OnPhaseBC();
//}
//
////void CommutateBipolarMotor1PhaseBA(uint16_t pwm) {PWM_1ABC_PIN_SET_POL(0b100); PWM_1B_PIN_SET_CV(PWM_MAX/2 + pwm/2); PWM_1A_PIN_SET_CV(PWM_MAX/2 + pwm/2); EN_1CBA_PIN_SET_OUT(0b011); SE_1CBA_PIN_SET_OUT(0b001); TriggerADCMotor1PhaseC();}
//void BipolarPhaseBA(uint16_t pwm)
//{
//	if (SinusoidalModulation)
//	{
//		SinusoidalPhaseBA();
//	}
//	else
//	{
//		SetPWMVal(PWMHalfDuty + pwm/2, PWMHalfDuty + pwm/2, 0);
//		EnablePWM(1, 1, 0);
//		SetPolarity(0, 1, 1); //C is dont care for non sinusoidal
//	}
//
//	OnPhaseBA();
//}
//
////void CommutateBipolarMotor1PhaseCA(uint16_t pwm) {PWM_1ABC_PIN_SET_POL(0b100); PWM_1C_PIN_SET_CV(PWM_MAX/2 + pwm/2); PWM_1A_PIN_SET_CV(PWM_MAX/2 + pwm/2); EN_1CBA_PIN_SET_OUT(0b101); SE_1CBA_PIN_SET_OUT(0b001); TriggerADCMotor1PhaseB();}
//void BipolarPhaseCA(uint16_t pwm)
//{
//	if (SinusoidalModulation)
//	{
//		SinusoidalPhaseCA();
//	}
//	else
//	{
//		SetPWMVal(PWMHalfDuty + pwm/2, 0, PWMHalfDuty + pwm/2);
//		EnablePWM(1, 0, 1);
//		SetPolarity(0, 1, 1); //B is dont care for non sinusoidal
//	}
//
//	OnPhaseCA();
//}
//
////void CommutateBipolarMotor1PhaseCB(uint16_t pwm) {PWM_1ABC_PIN_SET_POL(0b010); PWM_1C_PIN_SET_CV(PWM_MAX/2 + pwm/2); PWM_1B_PIN_SET_CV(PWM_MAX/2 + pwm/2); EN_1CBA_PIN_SET_OUT(0b110); SE_1CBA_PIN_SET_OUT(0b010); TriggerADCMotor1PhaseA();}
//void BipolarPhaseCB(uint16_t pwm)
//{
//	if (SinusoidalModulation)
//	{
//		SinusoidalPhaseCB();
//	}
//	else
//	{
//		SetPWMVal(0, PWMHalfDuty + pwm/2, PWMHalfDuty + pwm/2);
//		EnablePWM(0, 1, 1);
//		SetPolarity(1, 0, 1); //A is dont care for non sinusoidal,
//	}
//
//	OnPhaseCB();
//}





