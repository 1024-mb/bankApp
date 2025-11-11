#include <stdbool.h>
#include <string.h>

bool isNum(char inString[]) {
    int maxIndex = strlen(inString);
    char current;

    for (int i=0; i<maxIndex; i++) {
        current = inString[i];
        if (!('0' <= current && current <= '9') && !(current == '.')) {
            return false;
        }
    }

    return true;
}
