#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool existsFile(char accNo[]) {
    FILE *pfile = fopen("./database/index.txt", "r");
    char buffer[1024] = "";
    bool returnVal = false;

    while (fgets(buffer, sizeof(buffer), pfile) != NULL) {
        buffer[strcspn(buffer, "\n")] = 0;
        if (strcmp(buffer, accNo) == 0) {
            returnVal = true;
        }
    }

    fclose(pfile);
    return returnVal;
}
bool isStr(char inString[]) {
    int maxIndex = strlen(inString);
    char current;

    for (int i=0; i<maxIndex; i++) {
        current = inString[i];
        if (!('a' <= current && current <= 'z') && !('A' <= current && current <= 'Z') && current != ' ') {
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
