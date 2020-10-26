#define DEFINE_COMMANDS(name, number, arg, coding) \
        COM_##name,

typedef enum commands_for_processor {
                                      #include "Commands.h"
                                    } commands;

#undef DEFINE_COMMANDS

typedef enum assembler_errors { ASM_OK,
                                ASM_WRONG_NUM,
                                ASM_WRONG_COMMAND,
                                ASM_FILE_ERROR,
                                ASM_MEMORY_ERROR
                              } assembl_er;
