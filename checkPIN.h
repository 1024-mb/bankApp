// opens the account file and then checks if the PIN inside matches with the parameter.
bool checkPIN(char accountNumber[]) {
    char accDetails[1024] = "", PINUsr[1024] = "";
    bool match = false;

    char path[1024] = "./database/";
    strcat(path, accountNumber);
    strcat(path, "\0");

    FILE *PINFile = fopen(path, "r");
    fgets(accDetails, sizeof(accDetails), PINFile);

    // Gets the position of 'PIN:XXXX'
    int startPIN = (strstr(accDetails, "PIN:") == NULL) ? -1 : strstr(accDetails, "PIN:") - &accDetails[0];
    if (startPIN == -1) exitProgram();

    char pin[5] = {accDetails[startPIN+4], accDetails[startPIN+5], accDetails[startPIN+6], accDetails[startPIN+7], '\0'};

    // Gets and Validates PIN from user
    do {
        printf("Enter PIN: ");
        fgets(PINUsr, sizeof(PINUsr), stdin);
        PINUsr[strcspn(PINUsr, "\n")] = 0;

        if (strlen(PINUsr) != 4) {
            printf("PIN must be 4 Integer Digits. \n\n");
            continue;
        }
        if (strcmp(PINUsr, pin) != 0) {
            printf("Incorrect PIN. \n\n");
            continue;
        }
        if (strcmp(PINUsr, pin) == 0) {
            match = true;
        }

    } while (strcmp(PINUsr, pin) != 0);

    fclose(PINFile);

    return match;
}