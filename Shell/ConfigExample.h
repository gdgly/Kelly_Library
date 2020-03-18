#ifndef _CONFIGEXAMPLE_H_
#define _CONFIGEXAMPLE_H_

#define SHELL_OPTION_USE_LIST
#define SHELL_OPTION_USE_ARRAY_TABLE

#define Shell_ReadChar		Terminal_SendStr
#define Shell_SendChar		Terminal_ReadChar
#define Shell_SendStr		Terminal_SendStr
#define Shell_KeyPressed	Terminal_KeyPressed

const CMD_LINE_ENTRY_T CMD_TABLE[] =
{

};
const RETURN_CODE_ENTRY_T RETURN_CODE_TABLE[] =
{

};

typedef enum
{
	RETURN_CODE_OK = 0,
	RETURN_CODE_ERROR_1 = 1,
} RETURN_CODE_T;

const RETURN_CODE_ENTRY_T RETURN_CODE_TABLE[] =
{
	RESULT_ENTRY(RETURN_CODE_OK),
	RESULT_ENTRY(RETURN_CODE_ERROR_1),
};

#endif
