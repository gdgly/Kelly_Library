#if defined(SHELL_OPTION_USE_LIST) && defined(SHELL_OPTION_USE_ARRAY_TABLE)
#pragma GCC error "Cannot define both Shell modes"
#endif

#if !defined(SHELL_OPTION_USE_LIST) && !defined(SHELL_OPTION_USE_ARRAY_TABLE)
#define SHELL_OPTION_USE_LIST
#endif

#include "Shell.h"

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>

/******************************************************************************/
/*!
 * @name  	TxRxFunctions
 * @brief 	TxRx string functions must be supplied by the user application.
 */
/******************************************************************************/
/*! @{ */
extern void Shell_ReadChar(char * c);
extern void Shell_SendChar(char val);
extern void Shell_SendStr(char * str);
extern bool Shell_KeyPressed(void);
/*! @} */

/******************************************************************************/
/*!
 * @name  	ShellArrayTableMode
 * @brief 	Set up Shell to use Array tables defined by the user
 *
 * extern Table must be supplied by user app.
 */
/******************************************************************************/
/*! @{ */
#ifdef SHELL_OPTION_USE_ARRAY_TABLE
extern const CMDLINE_ENTRY_T CMD_TABLE[];
extern const RETURN_CODE_ENTRY_T RETURN_CODE_TABLE[];
#define CMDLINE_ENTRY_COUNT (sizeof(CMD_TABLE) / sizeof(CMDLINE_ENTRY_T))
#define RETURN_CODE_ENTRY_COUNT	(sizeof(RETURN_CODE_TABLE) / sizeof(RETURN_CODE_ENTRY_T))
//CMDLINE_ENTRY_T * CmdTable;
//RESULT_CODE_ENTRY_T * ReturnCodeTable;
//uint8_t CmdTableMax;
//uint8_t ReturnCodeTableMax;
#endif
/*! @} */

/******************************************************************************/
/*!
 * @name  	ShellListMode
 * @brief 	Set up Shell to use linked list defined by the user
 *
 * Add to the list using Shell_Register
 */
/******************************************************************************/
/*! @{ */
#ifdef SHELL_OPTION_USE_LIST
LIST_T CmdList;
LIST_T ReturnCodeList;
#endif
/*! @} */

/******************************************************************************/
/*!
 */
/******************************************************************************/
/*! @{ */
static const char CMD_PROMPT_STRING[] = "cmd> ";
//static const char CMD_PROMPT_STRING[] = "cmd> ";
//static const char * CmdPrompt = "cmd> ";
/*! @} */

/******************************************************************************/
/*!
 */
/******************************************************************************/
/*! @{ */
extern int Cmd_help(int argc, char * argv[]);
CMDLINE_ENTRY_T CmdEntry_help 		= { "help", 	"Display list of commands",  	Cmd_help };
CMDLINE_ENTRY_T CmdEntry_helph 	= { "h", 		"alias for help", 				Cmd_help };
CMDLINE_ENTRY_T CmdEntry_helpq 	= { "?", 		"alias for help", 				Cmd_help };
/*! @} */

/******************************************************************************/
/*!
 *  @name ShellState
 *  Variables containing Shell state
 */
/******************************************************************************/
/*! @{ */
#ifndef CMDLINE_MAX_ARGS
#define CMDLINE_MAX_ARGS	3
#endif
#ifndef CMDLINE_BUF_SIZE
#define CMDLINE_BUF_SIZE	32
#endif
static char CmdBuffer[CMDLINE_BUF_SIZE];			/*!<  */
static char * Argv[CMDLINE_MAX_ARGS + 1];			/*!<  */
static uint8_t Argc;								/*!<  */
static CMDLINE_FUNCTION_T CmdFunction;				/*!<  */
/*! @} */

// Allow user app to control shell options
static bool PrintReturnCode = 1;
static bool LoopMode = 0;
static uint8_t LoopControl = 0;
static uint32_t LoopFreqCalibrate = 0; //outter nonblock control freq
//SHELL_STATE_T ShellState; //pass through cmd function to allow user function to adjust shell options

/******************************************************************************/
/*!
 */
/******************************************************************************/
inline static bool CmdLineBuild(void)
{
	static uint8_t idx = 0;
	char ch;

	Shell_ReadChar(&ch);

	if ((ch > 31 && ch < 127) && (idx < CMDLINE_BUF_SIZE - 1))
	{
		Shell_SendChar(ch);
		CmdBuffer[idx++] = ch;
	}
	else if (ch == '\b' || ch == 127) //ch == '^H' || ch == '^?'
	{
		if (idx > 0)
		{
			Shell_SendStr("\b \b");
			idx--;
		}
	}
	else if (ch == '\n' || ch == '\r')
	{
		Shell_SendStr("\r\n");
		CmdBuffer[idx] = '\0';
		idx = 0;
		return true;
	}
//			#ifdef SHELL_OPTION_ARROW_KEYS_ENABLE
//			else if (ch == '\e')
//			{
//				//Shell_SendChar(ch);
//				Shell_ReadChar(&ch);
//				if (ch == '[')
//				{
//					//Shell_SendChar(ch);
//					Shell_ReadChar(&ch);
//					//Shell_SendChar(ch);
//				}
//			}
//			#endif
//			else if (ch == '\0')
//			{
//
//			}
	else
	{
		Shell_SendStr("\a"); //beep
	}

	return false;
}

inline static int CmdLineParse(void)
{
    char * cmd = CmdBuffer;
    uint8_t argc = 0;
    bool argStart = true;
//    CMDLINE_ENTRY_HANDLE_T cmdEntry;
#ifdef SHELL_OPTION_USE_LIST
    LIST_NODE_HANDLE_T node;
#endif
#ifdef SHELL_OPTION_USE_ARRAY_TABLE
    uint8_t index = 0;
#endif

    while(*cmd != '\0')
    {
    	if(*cmd == ' ')
		{
			argStart = true;
			*cmd = 0;
		}
    	else if(argStart)
        {
            if(argc < CMDLINE_MAX_ARGS)
            {
                argStart = false;
                Argv[argc] = cmd;
                argc++;
            }
            else
            {
                return CMDLINE_TOO_MANY_ARGS;
            }
        }
        cmd++;
    }

    Argc = argc;

    if(argc)
    {
#ifdef SHELL_OPTION_USE_ARRAY_TABLE
        //cmdEntry = &CMD_TABLE[index];
        while(index < CMDLINE_ENTRY_COUNT)
        {
            if(!strcmp(Argv[0], CMD_TABLE[index]->CmdName))
            {
            	CmdFunction = CMD_TABLE[index]->CmdFunction;
            	return CMDLINE_OK;
            }
            index++;
        }
#endif

#ifdef SHELL_OPTION_USE_LIST
    	// ((CMDLINE_ENTRY_HANDLE_T)(listIterator->Data))->CmdName
        node = List_GetHead(&CmdList);
        while (node != NULL)
        {
        	if(!strcmp(Argv[0], GET_CONTAINER_STRUCT_POINTER(node, CMDLINE_ENTRY_T, Link)->CmdName))
        	{
            	CmdFunction = GET_CONTAINER_STRUCT_POINTER(node, CMDLINE_ENTRY_T, Link)->CmdFunction;
            	return CMDLINE_OK;
            }
            node = List_GetNext(node);
        }
#endif
    }

    return CMDLINE_BAD_CMD;
}

inline static void PrintReturnCodeParser(int returnCode)
{
	switch (returnCode)
	{
		case CMDLINE_OK:
			break;
		case CMDLINE_BAD_CMD:
			Shell_SendStr("Bad command!\r\n");
			break;
		case CMDLINE_TOO_MANY_ARGS:
			Shell_SendStr("Too many arguments for command processor!\r\n");
			break;
//		case CMDLINE_TOO_FEW_ARGS:
//			break;
//		case CMDLINE_INVALID_ARG:
//			break;
	}
}

inline static const char * StringFromReturnCode(int returnCode)
{
#ifdef SHELL_OPTION_USE_ARRAY_TABLE
	uint8_t idx;

	for(idx = 0; idx < RETURN_CODE_ENTRY_COUNT; idx++)
	{
		if (RETURN_CODE_TABLE[idx].ReturnCode == returnCode)
			return RETURN_CODE_TABLE[idx].ReturnCodeString;
	}
	// if table index has no gaps
	//	if (returnCode < RESULT_CODES_ENTRY_COUNT)
	//		return (RETURN_CODE_TABLE[returnCode].ReturnString);
#endif

#ifdef SHELL_OPTION_USE_LIST
	LIST_NODE_HANDLE_T node;
	node = List_GetHead(&ReturnCodeList);
	while (node != NULL)
	{
		if( GET_CONTAINER_STRUCT_POINTER(node, RETURN_CODE_ENTRY_T, Link)->ReturnCode == returnCode )
			return GET_CONTAINER_STRUCT_POINTER(node, RETURN_CODE_ENTRY_T, Link)->ReturnCodeString;
		node = List_GetNext(node);
	}
#endif

	return("UNKNOWN ERROR CODE");
}

inline static void PrintReturnCodeCmdFunction(int returnCode)
{
	Shell_SendStr("Command returned error code ");
	Shell_SendStr(StringFromReturnCode(returnCode));
	Shell_SendStr("\r\n");
}

int Shell_ProcessBlocking(void)
{
	int8_t status;
	Shell_SendStr(CMD_PROMPT_STRING);

	while(1) if (CmdLineBuild()) break;

	status = CmdLineParse();
	PrintReturnCodeParser(status);
	if (status != CMDLINE_OK) return RESERVED_SHELL_RETURN_CODE_PARSER_FAIL;

	status = CmdFunction(Argc, Argv);
	if (status != RESERVED_SHELL_RETURN_CODE_SUCCESS) PrintReturnCodeCmdFunction(status);
	return status;
}

typedef enum
{
	SHELL_MODE_PROMPT,
	SHELL_MODE_USER_INPUT,
	SHELL_MODE_PARSE_INPUT,
	SHELL_MODE_CMD_PROCESS,
	SHELL_MODE_CMD_PROCESS_LOOP,
} SHELL_MODE_T;

int Shell_ProcessNonBlocking(void)
{
	static SHELL_MODE_T mode = SHELL_MODE_PROMPT;
	int8_t status;

	static uint8_t count;
	char ch;

	switch(mode)
	{
		case SHELL_MODE_PROMPT:
			Shell_SendStr(CMD_PROMPT_STRING);
			mode = SHELL_MODE_USER_INPUT;
			return RESERVED_SHELL_RETURN_CODE_PROCESSING;

		case SHELL_MODE_USER_INPUT:
			if (Shell_KeyPressed())
				if (CmdLineBuild())
					mode = SHELL_MODE_PARSE_INPUT;
			return RESERVED_SHELL_RETURN_CODE_PROCESSING;

		case SHELL_MODE_PARSE_INPUT:
			status = CmdLineParse();
			PrintReturnCodeParser(status);
			if (status == CMDLINE_OK)	mode = SHELL_MODE_CMD_PROCESS;
			else						mode = SHELL_MODE_PROMPT;
			return status;

		case SHELL_MODE_CMD_PROCESS:
			status = CmdFunction(Argc, Argv);
			if (PrintReturnCode)
			{
				if (status != RESERVED_SHELL_RETURN_CODE_SUCCESS)
					PrintReturnCodeCmdFunction(status);
			}
			if (LoopMode) 	mode = SHELL_MODE_CMD_PROCESS_LOOP;
			else 			mode = SHELL_MODE_PROMPT;
			return status;

		case SHELL_MODE_CMD_PROCESS_LOOP:	//how should user app communicate go to this mode?? reserved return code? public shell function?
			if (Shell_KeyPressed())
			{
				Shell_ReadChar(&ch);

				if (ch == '^C' || ch == 27) //esc key
				{
					mode = SHELL_MODE_PROMPT;
					LoopMode = 0;
					return RESERVED_SHELL_RETURN_CODE_PROCESSING;
				}
			}
			else
			{
				count++;
				if(count > LoopControl)
				{
					count = 0;
					status = CmdFunction(Argc, Argv);
					return status;
				}
				return RESERVED_SHELL_RETURN_CODE_PROCESSING;
			}
	}

	return RESERVED_SHELL_RETURN_CODE_PROCESSING;
}

void Shell_SetProcessModeLoop(uint16_t cmdLoopFreq)
{
	LoopMode = 1;
	LoopControl = LoopFreqCalibrate/cmdLoopFreq;
}

void Shell_SetPrintReturnCode(bool print)
{
	PrintReturnCode = print;
}

#ifdef SHELL_OPTION_USE_LIST
void Shell_RegisterReturnCodeEntry(RETURN_CODE_ENTRY_T * entry)
{
	List_AddTail(&ReturnCodeList, &entry->Link);
}

//void Shell_RegisterReturnCodeEntry(RETURN_CODE_ENTRY_T * entry, uint8_t partition)
//{
//	entry->ReturnCode = partition * 100 + 1 + entry->ReturnCode; //+1, 0 is reserved
//	List_AddTail(&ReturnCodeList, &entry->Link);
//}

//void Shell_RegisterReturnCodeList(RETURN_CODE_ENTRY_T * entry, uint8_t partition)
//{
//    LIST_NODE_HANDLE_T node = &entry->Link;
//
//	while (node != NULL)
//	{
//		GET_CONTAINER_STRUCT_POINTER(node, RETURN_CODE_ENTRY_T, Link)->ReturnCode = partition * 100 + GET_CONTAINER_STRUCT_POINTER(node, RETURN_CODE_ENTRY_T, Link)->ReturnCode;
//		node = List_GetNext(node);
//	}
//
//    List_AddTail(&ReturnCodeList, &entry->Link);
//}

void Shell_RegisterCmdLineEntry(CMDLINE_ENTRY_T * cmd)
{
	List_AddTail(&CmdList, &cmd->Link);
}

//void Shell_RegisterCmdLineList(CMDLINE_ENTRY_T * cmd)
//{
//	List_AddTail(&CmdList, &cmd->Link);
//}

//void Shell_InitRegisterReturnCodeEntry(RETURN_CODE_ENTRY_T * entry, int resultCode, char * resultString)
//{
//	entry->ReturnCode = resultCode;
//	entry->ReturnString = resultString;
//	List_AddTail(&ReturnCodeList, &entry->Link);
//}
//
//void Shell_InitRegisterCmdLineEntry(CMDLINE_ENTRY_T * cmd, char * cmdName, char * cmdHelp, CMDLINE_FUNCTION_T cmdFunction)
//{
//	cmd->CmdName = cmdName;
//	cmd->CmdHelp = cmdHelp;
//	cmd->CmdFunction = cmdFunction;
//	List_AddTail(&CmdList, &cmd->Link);
//}
#endif

void Shell_Init()
{
	Terminal_Init();

#ifdef SHELL_OPTION_USE_LIST
	Shell_RegisterCmdLineEntry(&CmdEntry_help);
	Shell_RegisterCmdLineEntry(&CmdEntry_helph);
	Shell_RegisterCmdLineEntry(&CmdEntry_helpq);
#endif
#ifdef SHELL_OPTION_USE_ARRAY_TABLE

#endif
}

void Shell_InitNonBlocking(uint16_t cmdLoopFreq, uint16_t outterLoopFreq)
{
	LoopFreqCalibrate = outterLoopFreq;
	LoopControl = outterLoopFreq/cmdLoopFreq;

	Terminal_Init();

#ifdef SHELL_OPTION_USE_LIST
	Shell_RegisterCmdLineEntry(&CmdEntry_help);
	Shell_RegisterCmdLineEntry(&CmdEntry_helph);
	Shell_RegisterCmdLineEntry(&CmdEntry_helpq);
#endif

#ifdef SHELL_OPTION_USE_ARRAY_TABLE

#endif
}


/******************************************************************************/
/*!
 *  @name Cmds
 *  Default commands
 */
/******************************************************************************/
/*! @{ */
int Cmd_help(int argc, char * argv[])
{
#ifdef SHELL_OPTION_USE_ARRAY_TABLE
	uint8_t idx = 0;
#endif

#ifdef SHELL_OPTION_USE_LIST
	LIST_NODE_HANDLE_T node;
#endif

    (void)argc;
    (void)argv;

    Shell_SendStr("\r\nAvailable commands\r\n------------------\r\n\r\n");
#ifdef SHELL_OPTION_USE_ARRAY_TABLE
    while(CMD_TABLE[idx].CmdName)
    {
        Shell_SendStr(CMD_TABLE[idx].CmdName);
        Shell_SendStr("	");
        Shell_SendStr(CMD_TABLE[idx].CmdHelp);
        Shell_SendStr("\r\n");
        idx++;
    }
#endif
#ifdef SHELL_OPTION_USE_LIST
    node = List_GetHead(&CmdList);
    while(node != NULL)
    {
        Shell_SendStr(GET_CONTAINER_STRUCT_POINTER(node, CMDLINE_ENTRY_T, Link)->CmdName);
        Shell_SendStr("	");
        Shell_SendStr(GET_CONTAINER_STRUCT_POINTER(node, CMDLINE_ENTRY_T, Link)->CmdHelp);
        Shell_SendStr("\r\n");
        node = List_GetNext(node);
    }
#endif

    Shell_SendStr("\r\n");

    return RESERVED_SHELL_RETURN_CODE_SUCCESS;
}

int Cmd_echo(int argc, char ** argv)
{
	if(argc == 2)
	{
	    Shell_SendStr("\r\n");
	    Shell_SendStr(argv[1]);
	    Shell_SendStr("\r\n");
	}

	return RESERVED_SHELL_RETURN_CODE_SUCCESS;
}
/*! @} */
