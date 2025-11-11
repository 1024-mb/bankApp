#include <stdbool.h>
#include <string.h>

bool isNum(char inString[]) {
    int maxIndex = strlen(inString);
    char current;
    int countDecimal=0;

    for (int i=0; i<maxIndex; i++) {
        current = inString[i];
        if ((!('0' <= current && current <= '9') && !(current == '.')) || countDecimal > 1) {
            return false;
        }
        if (current == '.') {countDecimal++;}
    }

    return true;
}

bool isInt(char inString[]) {
    int maxIndex = strlen(inString);
    char current;

    for (int i=0; i<maxIndex; i++) {
        current = inString[i];
        if (!('0' <= current && current <= '9')) return false;
    }

    return true;
}
