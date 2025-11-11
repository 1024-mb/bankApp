
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// allows efficient testing of individual deposit amount input
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


int main() {
    char name[1024] = "";

    do {
        printf("Enter Name: ");
        fgets(name, sizeof(name), stdin);
        name[strcspn(name, "\n")] = 0;

        if (!isStr(name)) printf("Enter a Valid String Name\n\n");
    } while (!isStr(name));
}
