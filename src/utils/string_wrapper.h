//
// Created by cattenlinger on 19-11-22.
//

#ifndef LUA_TEST_STRING_WRAPPER_H
#define LUA_TEST_STRING_WRAPPER_H

#include <stdlib.h>

typedef struct String String;

String *string_concatArgs(int argc, char **argv);
String *string_emptyString();

String *string_appendChars(String* string,const char* chars);

char* string_value(String* string);
size_t string_length(String* string);

void string_free(String *string);

#endif //LUA_TEST_STRING_WRAPPER_H
