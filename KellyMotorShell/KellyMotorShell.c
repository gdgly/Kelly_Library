#include "Shell/Terminal.h"
#include "Shell/Shell.h"

#include "BLDC/BLDC.h" //includes components

/******************************************************************************/
/*!
 *  @name	ShellSection
 *  Shell functions
 */
/******************************************************************************/
/*! @{ */
extern BLDC_CONTROLLER_T Motor1;

#define SHELL_OUTTER_LOOP_FREQ 100

typedef enum
{
	RETURN_CODE_OK = 0,
	RETURN_CODE_ERROR_1 = 1,
} RETURN_CODE_T;

//extern int Cmd_pwm 		(int argc, char ** argv);
//extern int Cmd_rpm 		(int argc, char ** argv);
//extern int Cmd_jog 		(int argc, char ** argv);
//extern int Cmd_run 		(int argc, char ** argv);
//extern int Cmd_stop		(int argc, char ** argv);
//extern int Cmd_release 	(int argc, char ** argv);
//extern int Cmd_vbat		(int argc, char ** argv);
//extern int Cmd_v		(int argc, char ** argv);
//extern int Cmd_phase	(int argc, char ** argv);
//extern int Cmd_hold		(int argc, char ** argv);
//extern int Cmd_print	(int argc, char ** argv);


int Cmd_pwm(int argc, char ** argv)
{
    if(argc == 2) BLDC_SetPWM(&Motor1, strtoul(argv[1], 0, 10));
    return RESERVED_SHELL_RETURN_CODE_SUCCESS;
}

int Cmd_v(int argc, char ** argv)
{
    if(argc == 2) BLDC_SetVoltage(&Motor1, strtoul(argv[1], 0, 10));
    return RESERVED_SHELL_RETURN_CODE_SUCCESS;
}

int Cmd_rpm(int argc, char ** argv)
{
	(void)argv;
    if(argc == 1)
    {
    	Terminal_SendStr("\r\nRPM = ");
    	Terminal_SendNum(Speed_GetRPM(&Motor1Speed));
    	Terminal_SendStr("\r\n");
    }
    return RESERVED_SHELL_RETURN_CODE_SUCCESS;
}

int Cmd_jog(int argc, char ** argv)
{
	if(argc == 1)
	{
		BLDC_SetJogSteps(&Motor1, 1);

	}
	else if(argc == 2) BLDC_SetJogSteps(&Motor1, strtoul(argv[1], 0, 10));
    return RESERVED_SHELL_RETURN_CODE_SUCCESS;
}

int Cmd_run(int argc, char ** argv)
{
	(void)argv;
    if(argc == 1) BLDC_Start(&Motor1);
    return RESERVED_SHELL_RETURN_CODE_SUCCESS;
}

int Cmd_stop(int argc, char ** argv)
{
	(void)argv;
    if(argc == 1) BLDC_Stop(&Motor1);
    return RESERVED_SHELL_RETURN_CODE_SUCCESS;
}

int Cmd_hold(int argc, char ** argv)
{
	(void)argv;
    if(argc == 1) BLDC_Hold(&Motor1);
    return RESERVED_SHELL_RETURN_CODE_SUCCESS;
}

int Cmd_release(int argc, char ** argv)
{
	(void)argv;
    if(argc == 1) BLDC_Coast(&Motor1);
    return RESERVED_SHELL_RETURN_CODE_SUCCESS;
}

int Cmd_vbat(int argc, char ** argv)
{
	(void)argv;
    if(argc == 1)
    {
    	Terminal_SendStr("\r\nVBat = ");
    	Terminal_SendNum(VoltageDivider_GetVoltage(&DividerCommon, *Motor1.VBat_ADCU));
    	Terminal_SendStr("\r\n");
    }
    return RESERVED_SHELL_RETURN_CODE_SUCCESS;
}


int Cmd_phase(int argc, char ** argv)
{
    if(argc == 2)
    {
    	if 		(argv[1][0] == 'a' && argv[1][1] == 'b') CommutateMotor1PhaseAB(Motor1.PWM);
    	else if (argv[1][0] == 'a' && argv[1][1] == 'c') CommutateMotor1PhaseAC(Motor1.PWM);
    	else if (argv[1][0] == 'b' && argv[1][1] == 'a') CommutateMotor1PhaseBA(Motor1.PWM);
    	else if (argv[1][0] == 'b' && argv[1][1] == 'c') CommutateMotor1PhaseBC(Motor1.PWM);
    	else if (argv[1][0] == 'c' && argv[1][1] == 'a') CommutateMotor1PhaseCA(Motor1.PWM);
    	else if (argv[1][0] == 'c' && argv[1][1] == 'b') CommutateMotor1PhaseCB(Motor1.PWM);

//
//    	switch(argv[1][0])
//    	{
//
//    	case 'a':
//    	case 'A':
//    		switch(argv[1][1])
//    		{
//
//        	case 'b':
//        	case 'B':
//
//        		break;
//
//    		case 'c':
//    		case 'C':
//
//    			break;
//    		}
//    		break;
//
//		case 'b':
//		case 'B':
//			switch(argv[1][1])
//			{
//
//			}
//			break;
//
//		case 'c':
//		case 'C':
//			switch(argv[1][1])
//			{
//
//			}
//			break;
//
//    	}


    }

    return RESERVED_SHELL_RETURN_CODE_SUCCESS;
}





int Cmd_print(int argc, char ** argv)
{
//	//printf("Loop Time is %d \n", LoopDelta);
//	//printf("PWM %d \n", Motor1.PWM);
//	printf("BackEMF A ADCU %d \n", *Motor1.BackEMFPhaseA_ADCU);
//	printf("BackEMF B ADCU %d \n", *Motor1.BackEMFPhaseB_ADCU);
//	//printf("Battery Voltage %d \n", VoltageDivider_GetVoltage(&DividerCommon, *Motor1.VBat_ADCU));
//	printf("BackEMF A %d \n", VoltageDivider_GetVoltage(&DividerCommon, *Motor1.BackEMFPhaseA_ADCU));
//	printf("BackEMF B %d \n", VoltageDivider_GetVoltage(&DividerCommon, *Motor1.BackEMFPhaseB_ADCU));
//	printf("BackEMF Select %d \n", VoltageDivider_GetVoltage(&DividerCommon, *Motor1.BackEMFSelect_ADCU));
//	printf("Hall Delta %d \n", Speed_GetDeltaTicks(&Motor1Speed));
//	//printf("RPM %d \n", Speed_GetRPM(&Motor1Speed));
//	printf("I ADCU %d \n", *Motor1.I_ADCU);
//
////	printf("Motor Desired %d \n", *Motor1PID.SetPoint);
////	printf("Motor Input %d \n", *Motor1PID.Input);
////	printf("Motor Error (Measured - Input) %d \n", *Motor1PID.SetPoint - *Motor1PID.Input);

	(void)argv;
    if(argc == 1)
    {
    	Terminal_SendStr("\r\nBackEMF A  = ");
		Terminal_SendNum(VoltageDivider_GetVoltage(&DividerCommon, *Motor1.BackEMFPhaseA_ADCU));
		Terminal_SendStr("\r\n");

    	Terminal_SendStr("\r\nBackEMF B  = ");
		Terminal_SendNum(VoltageDivider_GetVoltage(&DividerCommon, *Motor1.BackEMFPhaseB_ADCU));
		Terminal_SendStr("\r\n");

    	Terminal_SendStr("\r\nBackEMFSelect = ");
    	Terminal_SendNum(VoltageDivider_GetVoltage(&DividerCommon, *Motor1.BackEMFSelect_ADCU));
    	Terminal_SendStr("\r\n");

    	Terminal_SendStr("\r\nDelta Ticks = ");
		Terminal_SendNum(Speed_GetDeltaTicks(&Motor1Speed));
		Terminal_SendStr("\r\n");

    	Terminal_SendStr("\r\nI ADCU = ");
    	Terminal_SendNum(*Motor1.I_ADCU);
    	Terminal_SendStr("\r\n");

    }
    return RESERVED_SHELL_RETURN_CODE_SUCCESS;
}

CMDLINE_ENTRY_T CmdEntry_pwm 		=	{ "pwm", 		"Sets pwm value", 			Cmd_pwm		};
CMDLINE_ENTRY_T CmdEntry_v	 		=	{ "v", 			"Sets applied voltage", 	Cmd_v		};
CMDLINE_ENTRY_T CmdEntry_rpm 		=	{ "rpm", 		"Display rpm", 				Cmd_rpm		};
CMDLINE_ENTRY_T CmdEntry_jog		=	{ "jog", 		"Jog motor", 				Cmd_jog		};
CMDLINE_ENTRY_T CmdEntry_run 		=	{ "run", 		"Set motor to run mode", 	Cmd_run		};
CMDLINE_ENTRY_T CmdEntry_stop 		=	{ "stop", 		"Set motor to idle mode", 	Cmd_stop	};
CMDLINE_ENTRY_T CmdEntry_release 	=	{ "release", 	"Release hold", 			Cmd_release	};
CMDLINE_ENTRY_T CmdEntry_vbat 		=	{ "vbat", 		"Display battery voltage", 	Cmd_vbat	};
CMDLINE_ENTRY_T CmdEntry_phase		=	{ "phase", 		"Set motor phase", 			Cmd_phase	};
CMDLINE_ENTRY_T CmdEntry_hold		=	{ "hold", 		"hold position", 			Cmd_hold	};
CMDLINE_ENTRY_T CmdEntry_print		=	{ "print", 		"print debug info",			Cmd_print	};

void KellyMotorShell_RegisterShellCmds()
{
	Shell_RegisterCmdLineEntry(&CmdEntry_pwm);
	Shell_RegisterCmdLineEntry(&CmdEntry_rpm);
	Shell_RegisterCmdLineEntry(&CmdEntry_jog);
	Shell_RegisterCmdLineEntry(&CmdEntry_run);
	Shell_RegisterCmdLineEntry(&CmdEntry_stop);
	Shell_RegisterCmdLineEntry(&CmdEntry_release);
	Shell_RegisterCmdLineEntry(&CmdEntry_vbat);
	Shell_RegisterCmdLineEntry(&CmdEntry_v);
	Shell_RegisterCmdLineEntry(&CmdEntry_phase);
	Shell_RegisterCmdLineEntry(&CmdEntry_hold);
	Shell_RegisterCmdLineEntry(&CmdEntry_print);
}
/*! @} */
