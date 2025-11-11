

#include "exitProgram.h"
// gets the balance of the account from account number
double getBalance(char filePath[]) {
    char buffer[1024] = "";
    char balance[100] = "";

    FILE *pfile = fopen(filePath, "r");
    fgets(buffer, sizeof(buffer), pfile);
    fclose(pfile);
    buffer[strcspn(buffer, "\n")] = 0;

    // gets balance.
    int balStart = (strstr(buffer, "BALANCE:") == NULL) ? -1 : strstr(buffer, "BALANCE:") - &buffer[0] + 8;
    int balEnd = (strstr(buffer, ",PIN:") == NULL) ? -1 : strstr(buffer, ",PIN:") - &buffer[0];
    if (balStart == -1 || balEnd == -1) exitProgram();

    int length = balEnd - balStart;

    for (int z=0; z<length; z++) {
        char now[2] = {buffer[balStart + z], '\0'};
        strcat(balance, &now[0]);
    }

    double finalBalance = 0;

    strcat(balance, "\0");
    finalBalance = atof(balance);

    return finalBalance;
}

char getAccountType(char filePath[]) {
    char buffer[1024] = "";

    FILE *bfile = fopen(filePath, "r");
    fgets(buffer, sizeof(buffer), bfile);
    buffer[strcspn(buffer, "\n")] = 0;

    int typeIndex = (strstr(buffer, "TYPE:") == NULL) ? -1 : strstr(buffer, "TYPE:") - &buffer[0] + 5;
    if (typeIndex == -1) exitProgram();

    return buffer[typeIndex];
}
