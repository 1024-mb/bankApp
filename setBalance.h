#include <stdio.h>
#include <string.h>

// Updates the balance stored in the file for a particular account.
void setBalance(char path[], double finalBalance) {
    char finalPrint[200] = "";
    char finalBalanceString[200] = "";
    char buffer[1024] = "";

    FILE *pfile = fopen(path, "r");
    fgets(buffer, sizeof(buffer), pfile);
    fclose(pfile);
    buffer[strcspn(buffer, "\n")] = 0;

    // finds the location of PIN:XXXX and retrieves the value
    int balStart = (strstr(buffer, "BALANCE:") == NULL) ? -1 : strstr(buffer, "BALANCE:") - &buffer[0] + 8;
    int balEnd = (strstr(buffer, ",PIN:") == NULL) ? -1 : strstr(buffer, ",PIN:") - &buffer[0];

    if (balStart == -1 || balEnd == -1) exitProgram();

    snprintf(finalBalanceString, sizeof(finalBalanceString), "%.2f", finalBalance);
    strcat(finalBalanceString, "\0");

    // puts the final balance within an output string and writes it to the file for the account number
    char cache[2] = {'s', '\0'};
    for (int z=0; z<balStart; z++) {
        cache[0] = buffer[z];
        strcat(finalPrint, cache);
    }

    strcat(finalPrint, finalBalanceString);

    for (int z=balEnd; z<strlen(buffer); z++) {
        cache[0] = buffer[z];
        strcat(finalPrint, cache);
    }

    strcat(finalPrint, "\0");

    FILE *wfile = fopen(path, "w");
    fprintf(wfile, "%s\n", finalPrint);
    fclose(wfile);
}