#ifndef __SHELL_H__
#define __SHELL_H__

#ifdef SHELL_OPTION_USE_LIST
#include "List.h"
#endif

//#define CMDLINE_OK				(0)
//#define CMDLINE_BAD_CMD			(-1)
//#define CMDLINE_TOO_MANY_ARGS	(-2)
//#define CMDLINE_TOO_FEW_ARGS	(-3)
//#define CMDLINE_INVALID_ARG		(-4)
//#define CMDLINE_LOOP_CMD		(-5)

typedef enum
{
	USER_RETURN_CODE_OK = 0,
	RESERVED_SHELL_RETURN_CODE_SUCCESS = 0,
	RESERVED_SHELL_RETURN_CODE_PARSER_FAIL = -1,
	RESERVED_SHELL_RETURN_CODE_PROCESSING = -2,
} RESERVED_SHELL_RETURN_CODE_T;

typedef enum
{
	CMDLINE_OK,
	CMDLINE_BAD_CMD,
	CMDLINE_TOO_MANY_ARGS,
	CMDLINE_TOO_FEW_ARGS,
	CMDLINE_INVALID_ARG,
} CMDLINE_PARSE_STATUS_T;

typedef int (*CMDLINE_FUNCTION_T)(int argc, char *argv[]);

typedef struct
{
    const char * CmdName;
    const char * CmdHelp;
	CMDLINE_FUNCTION_T CmdFunction;
    //ExpectedArgCount;
#ifdef SHELL_OPTION_USE_LIST
    LIST_NODE_T Link;
#endif
}
CMDLINE_ENTRY_T, *CMDLINE_ENTRY_HANDLE_T;

typedef struct
{
	int ReturnCode;
    char * ReturnCodeString;
    //errorhandlingfunction?
#ifdef SHELL_OPTION_USE_LIST
    LIST_NODE_T Link;
#endif
}
RETURN_CODE_ENTRY_T, *RETURN_CODE_ENTRY_HANDLE_T;

#ifdef SHELL_OPTION_USE_ARRAY_TABLE
#define CMD_ENTRY(CmdName, CmdHelpString, CmdFunction) { (#CmdName), (CmdHelpString), (CmdFunction) }
#define RETURN_ENTRY(ReturnCode) { (ReturnCode), (#ReturnCode) }
extern CMDLINE_ENTRY_T CmdEntry_help;
extern CMDLINE_ENTRY_T CmdEntry_helph;
extern CMDLINE_ENTRY_T CmdEntry_helpq;
#endif

#ifdef SHELL_OPTION_USE_LIST
#define DEFINE_CMD_ENTRY(CmdName, CmdHelpString, CmdFunction) CMDLINE_ENTRY_T CmdEntry##CmdName = { (#CmdName), (CmdHelpString), (CmdFunction), {0} }
#define DEFINE_RETURN_ENTRY(ReturnCode)	RESULT_ENTRY_T ReturnEntry##ReturnCode = { (ReturnCode), (#ReturnCode) }
void Shell_RegisterReturnCodeEntry(RETURN_CODE_ENTRY_T * result);
void Shell_RegisterCmdLineEntry(CMDLINE_ENTRY_T * cmd);
#endif

int Shell_ProcessBlocking(void);
int Shell_ProcessNonBlocking(void);

#endif
