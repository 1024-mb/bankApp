
#include <stdbool.h>
#include <stdio.h>
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

int main() {
    char IDNo[1024] = "";

    do {
        printf("Enter ID Number: ");
        fgets(IDNo, sizeof(IDNo), stdin);
        IDNo[strcspn(IDNo, "\n")] = 0;

        if (!isNum(IDNo) || strlen(IDNo) != 8) printf("Enter a Valid, 8 Digit Integer ID\n\n");
        
    } while (!isNum(IDNo) || strlen(IDNo) != 8);

    return 0;
}