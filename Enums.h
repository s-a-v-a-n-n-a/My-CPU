#ifndef ENUMS_H
#define ENUMS_H

#define DEFINE_COMMANDS(name, number, arg, coding, discoding) \
        COM_##name,

typedef enum commands_for_processor
{
    #include "Commands.h"
} commands;

#undef DEFINE_COMMANDS

#endif

