//
// Created by cattenlinger on 19-11-22.
//
#include <string.h>
#include "string_wrapper.h"

struct String {
    size_t size;
    char *value;
};

String *string_concatArgs(int argc, char **argv) {
    String *stringWrapper = (String *) malloc(sizeof(String));

    if (argc <= 0) return stringWrapper;
    if (argc <= 1) {
        stringWrapper->size = 0;
        stringWrapper->value = argv[0];
        return stringWrapper;
    }

    size_t length = strlen(argv[0]) + 1;
    char *newString = (char *) malloc(length);
    strcpy(newString, argv[0]);
    for (size_t i = 1; i < argc; i++) {
        length += 1 + strlen(argv[i]);
        newString = (char *) realloc(newString, length);
        strncat(strncat(newString, " ", length), argv[i], length);
    }

    stringWrapper->size = length;
    stringWrapper->value = newString;

    return stringWrapper;
}

String *string_emptyString() {
    String *string = (String *) malloc(sizeof(String));
    string->size = 0;
    string->value = NULL;
    return string;
}

String *string_appendChars(String *string, const char *chars) {

    if(string->size <= 0) {
        size_t length = strlen(chars) + 1;
        char* ref = (char*)malloc(length);
        strcpy(ref, chars);
        string->size = length;
        string->value = ref;
        return string;
    }

    size_t length = strlen(chars) + string->size;

    char* ref = (char *) realloc(string->value, length);

    strncat(ref, chars, length);

    string->value = ref;
    string->size = length;

    return string;
}

char *string_value(String *string) {
    return string->value;
}

size_t string_length(String *string) {
    return string->size - 1;
}

void string_free(String *string) {
    free(string->value);
    free(string);
}