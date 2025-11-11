#include <stdbool.h>
#include <stdio.h>
#include <string.h>


//checks if the text file with the account number exists,
//i.e. if the file is in the index
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