#include "Waveform.h"

#include <stdint.h>
#include <stdbool.h>

//Phase boundaries 32, 96, 160, 224, 288, 352
//Phase midpoints  64, 128, 192, 256, 320, 0

//PhaseAB Angle 	A:32-96 	B:288-352 	C:160-224
//PhaseAC Angle 	A:96-160	B:352-32 	C:224-288
//PhaseBC Angle 	A:160-224	B:32-96 	C:288-352
//PhaseBA Angle 	A:224-288	B:160-224	C:352-32
//PhaseCA Angle 	A:288-352	B:224-288 	C:32-96
//PhaseCB Angle 	A:352-32	B:352-32 	C:96-160
const uint8_t SINUSOIDAL_WAVE_TABLE[SINE_ANGLE_SIZE] =
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

void Waveform_Init
(
	WAVEFORM_T * waveform,
	void (*setPWMVal)(uint16_t pwmA, uint16_t pwmB, uint16_t pwmC),
	uint16_t pwmMax,
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
	waveform->WaveformMode = WAVEFORM_MODE_UNIPOLAR1_T;
	waveform->SinusoidalModulation = false;

	waveform->SetPWMVal = setPWMVal;
	waveform->EnablePWM = enablePWM;
	waveform->InversePWMPolarity = invPWMPolarity;
	waveform->OnPhaseAB = onPhaseAB;
	waveform->OnPhaseAC = onPhaseAC;
	waveform->OnPhaseBC = onPhaseBC;
	waveform->OnPhaseBA = onPhaseBA;
	waveform->OnPhaseCA = onPhaseCA;
	waveform->OnPhaseCB = onPhaseCB;
	waveform->PWMHalfDuty = pwmMax/2;
}

void Waveform_InitSinusoidalModulation
(
	WAVEFORM_T * waveform,
	void (*setPWMVal)(uint16_t pwmA, uint16_t pwmB, uint16_t pwmC),
	uint16_t pwmMax,
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
		waveform,
		setPWMVal,
		pwmMax,
		enablePWM,
		invPWMPolarity,
		onPhaseAB,
		onPhaseAC,
		onPhaseBC,
		onPhaseBA,
		onPhaseCA,
		onPhaseCB
	);

	waveform->AngularSpeedTime = SINE_ANGLE_SIZE*hallTimerFreq/isrFreq;
	waveform->HallTimerDelta = hallTimerDelta;
}

void Waveform_EnableSinusoidalModulation(WAVEFORM_T * waveform)
{
	waveform->SinusoidalModulation = true;
	waveform->EnablePWM(1, 1, 1);
	waveform->SetPWMVal(0,0,0);
}

void Waveform_DisableSinusoidalModulation(WAVEFORM_T * waveform)
{
	waveform->SinusoidalModulation = false;
	waveform->EnablePWM(0, 0, 0);
	waveform->InversePWMPolarity(0,0,0);
}

void Waveform_SetMode(WAVEFORM_T * waveform, WAVEFORM_MODE_T waveformMode)
{
	waveform->WaveformMode = waveformMode;

	waveform->EnablePWM(0, 0, 0);
	waveform->InversePWMPolarity(0,0,0);

//	switch (WaveformMode)
//	{
//		case WAVEFORM_MODE_UNIPOLAR1_T:
//			break;
//
//		case WAVEFORM_MODE_UNIPOLAR2_T:
//			break;
//
//		case WAVEFORM_MODE_BIPOLAR_T:
//			break;
//	}
}

#define REFERENCE_ANGLE_PHASE_AB 0 // reference angle resets to this value at commutation
#define REFERENCE_ANGLE_PHASE_AC 64
#define REFERENCE_ANGLE_PHASE_BC 128
#define REFERENCE_ANGLE_PHASE_BA 192
#define REFERENCE_ANGLE_PHASE_CA 256
#define REFERENCE_ANGLE_PHASE_CB 320

//static inline void SinusoidalPhaseAB(void) {Angle = 0;   ISRCount = 0;};
//static inline void SinusoidalPhaseAC(void) {Angle = 64;  ISRCount = 0;};
//static inline void SinusoidalPhaseBC(void) {Angle = 128; ISRCount = 0;};
//static inline void SinusoidalPhaseBA(void) {Angle = 192; ISRCount = 0;};
//static inline void SinusoidalPhaseCA(void) {Angle = 256; ISRCount = 0;};
//static inline void SinusoidalPhaseCB(void) {Angle = 320; ISRCount = 0;};

void Waveform_CommutatePhaseAB(WAVEFORM_T * waveform, uint16_t pwm)
{
	if (waveform->SinusoidalModulation)
	{
		waveform->Angle = REFERENCE_ANGLE_PHASE_AB;
		waveform->ISRCount = 0;
		//if (WaveformMode == WAVEFORM_MODE_BIPOLAR_T) EnablePWM(1, 1, 0);
	}
	else
	{
		switch (waveform->WaveformMode)
		{
			case WAVEFORM_MODE_UNIPOLAR1_T: waveform->SetPWMVal(pwm, 0, 0); 																							break;
			case WAVEFORM_MODE_UNIPOLAR2_T: waveform->SetPWMVal(waveform->PWMHalfDuty + pwm/2, waveform->PWMHalfDuty - pwm/2, 0);										break;
			case WAVEFORM_MODE_BIPOLAR_T: 	waveform->SetPWMVal(waveform->PWMHalfDuty + pwm/2, waveform->PWMHalfDuty + pwm/2, 0); waveform->InversePWMPolarity(0,1,0); 	break;
		}
		waveform->EnablePWM(1, 1, 0);
	}
	waveform->OnPhaseAB();
}

void Waveform_CommutatePhaseAC(WAVEFORM_T * waveform, uint16_t pwm)
{
	if (waveform->SinusoidalModulation)
	{
		waveform->Angle = REFERENCE_ANGLE_PHASE_AC;
		waveform->ISRCount = 0;
		//if (WaveformMode == WAVEFORM_MODE_BIPOLAR_T) EnablePWM(1, 0, 1);
	}
	else
	{
		switch (waveform->WaveformMode)
		{
			case WAVEFORM_MODE_UNIPOLAR1_T: waveform->SetPWMVal(pwm, 0, 0); 																							break;
			case WAVEFORM_MODE_UNIPOLAR2_T: waveform->SetPWMVal(waveform->PWMHalfDuty + pwm/2, 0, waveform->PWMHalfDuty - pwm/2);										break;
			case WAVEFORM_MODE_BIPOLAR_T: 	waveform->SetPWMVal(waveform->PWMHalfDuty + pwm/2, 0, waveform->PWMHalfDuty + pwm/2); waveform->InversePWMPolarity(0,0,1); 	break;
		}
		waveform->EnablePWM(1, 0, 1);
	}
	waveform->OnPhaseAC();
}

void Waveform_CommutatePhaseBC(WAVEFORM_T * waveform,uint16_t pwm)
{
	if (waveform->SinusoidalModulation)
	{
		waveform->Angle = REFERENCE_ANGLE_PHASE_BC;
		waveform->ISRCount = 0;
		//if (WaveformMode == WAVEFORM_MODE_BIPOLAR_T) EnablePWM(0, 1, 1);
	}
	else
	{
		switch (waveform->WaveformMode)
		{
			case WAVEFORM_MODE_UNIPOLAR1_T: waveform->SetPWMVal(0, pwm, 0); 																							break;
			case WAVEFORM_MODE_UNIPOLAR2_T: waveform->SetPWMVal(0, waveform->PWMHalfDuty + pwm/2, waveform->PWMHalfDuty - pwm/2);										break;
			case WAVEFORM_MODE_BIPOLAR_T: 	waveform->SetPWMVal(0, waveform->PWMHalfDuty + pwm/2, waveform->PWMHalfDuty + pwm/2); waveform->InversePWMPolarity(0,0,1); 	break;
		}
		waveform->EnablePWM(0, 1, 1);
	}
	waveform->OnPhaseBC();
}

void Waveform_CommutatePhaseBA(WAVEFORM_T * waveform , uint16_t pwm)
{
	if (waveform->SinusoidalModulation)
	{
		waveform->Angle = REFERENCE_ANGLE_PHASE_BA;
		waveform->ISRCount = 0;
		//if (WaveformMode == WAVEFORM_MODE_BIPOLAR_T) EnablePWM(1, 1, 0);
	}
	else
	{
		switch (waveform->WaveformMode)
		{
			case WAVEFORM_MODE_UNIPOLAR1_T: waveform->SetPWMVal(0, pwm, 0); 																							break;
			case WAVEFORM_MODE_UNIPOLAR2_T: waveform->SetPWMVal(waveform->PWMHalfDuty - pwm/2, waveform->PWMHalfDuty + pwm/2, 0);										break;
			case WAVEFORM_MODE_BIPOLAR_T: 	waveform->SetPWMVal(waveform->PWMHalfDuty + pwm/2, waveform->PWMHalfDuty + pwm/2, 0); waveform->InversePWMPolarity(1,0,0); 	break;
		}
		waveform->EnablePWM(1, 1, 0);
	}
	waveform->OnPhaseBC();
}

void Waveform_CommutatePhaseCA(WAVEFORM_T * waveform , uint16_t pwm)
{
	if (waveform->SinusoidalModulation)
	{
		waveform->Angle = REFERENCE_ANGLE_PHASE_CA;
		waveform->ISRCount = 0;
		//if (WaveformMode == WAVEFORM_MODE_BIPOLAR_T) EnablePWM(1, 0, 1);
	}
	else
	{
		switch (waveform->WaveformMode)
		{
			case WAVEFORM_MODE_UNIPOLAR1_T: waveform->SetPWMVal(0, 0, pwm); 																							break;
			case WAVEFORM_MODE_UNIPOLAR2_T: waveform->SetPWMVal(waveform->PWMHalfDuty - pwm/2, 0, waveform->PWMHalfDuty + pwm/2);										break;
			case WAVEFORM_MODE_BIPOLAR_T: 	waveform->SetPWMVal(waveform->PWMHalfDuty + pwm/2, 0, waveform->PWMHalfDuty + pwm/2); waveform->InversePWMPolarity(1,0,0); 	break;
		}
		waveform->EnablePWM(1, 0, 1);
	}
	waveform->OnPhaseCA();
}

void Waveform_CommutatePhaseCB(WAVEFORM_T * waveform, uint16_t pwm)
{
	if (waveform->SinusoidalModulation)
	{
		waveform->Angle = REFERENCE_ANGLE_PHASE_CB;
		waveform->ISRCount = 0;
		//if (WaveformMode == WAVEFORM_MODE_BIPOLAR_T) EnablePWM(0, 1, 1);
	}
	else
	{
		switch (waveform->WaveformMode)
		{
			case WAVEFORM_MODE_UNIPOLAR1_T: waveform->SetPWMVal(0, 0, pwm); 																							break;
			case WAVEFORM_MODE_UNIPOLAR2_T: waveform->SetPWMVal(0, waveform->PWMHalfDuty - pwm/2, waveform->PWMHalfDuty + pwm/2);										break;
			case WAVEFORM_MODE_BIPOLAR_T: 	waveform->SetPWMVal(0, waveform->PWMHalfDuty + pwm/2, waveform->PWMHalfDuty + pwm/2); waveform->InversePWMPolarity(0,1,0); 	break;
		}
		waveform->EnablePWM(0, 1, 1);
	}
	waveform->OnPhaseCB();
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





