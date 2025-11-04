#include <ctype.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <regex.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>

bool existsFile(char accNo[]);
void createAccount();
void addFile(char accNo[], char name[], char ID[], char accType[], float balance, char PIN[]);
bool isStr(char inString[]);
bool isNum(char inString[]);
void deleteAccount();
void deposit();
void format_chars();
void withdraw();
void remit();
void delay(double number_of_seconds);
void setBalance(char filePath[], double balance);
double getBalance(char filePath[]);
bool checkPIN(char accountNumber[]);
void exitProgram();
void listAccounts();

int main() {
    // checks if database directory exists and makes it if not.
    struct stat st = {0};
    if (stat("./database", &st) == -1) {
        mkdir("./database", 0700);
    }

    char choice[1024];
    char options[6][100] = {"1. CREATE NEW BANK ACCOUNT", "2. DELETE BANK ACCOUNT",
        "3. DEPOSIT IN BANK", "4. WITHDRAW FROM BANK", "5. REMITTANCE", "Q. QUIT"};

    do {
        // Format and output welcome message
        delay(0.5);
        format_chars();
        printf("                        WELCOME TO SOFTBANK              ");
        printf("\n                            1. CREATE NEW BANK ACCOUNT");
        printf("\n                            2. DELETE BANK ACCOUNT");
        printf("\n                            3. DEPOSIT IN BANK");
        printf("\n                            4. WITHDRAW FROM BANK");
        printf("\n                            5. REMITTANCE\n");
        printf("\n                            Q. QUIT\n");
        format_chars();

        // Get choice from user
        printf("ENTER CHOICE: ");

        // get user input & remove newline character
        fgets(choice, sizeof(choice), stdin);
        choice[strcspn(choice, "\n")] = 0;

        if (strcmp(choice, "") == 0) continue;
        printf("________________________________________________________________________________\n\n");

        // convert to upper case for comparison
        for (int i=0; i<strlen(choice); i++) choice[i] = toupper(choice[i]);
        printf("\n");
        format_chars();

        int index=0;
        for (index=0; index < 6; index++) if (strstr(options[index], choice) != NULL) break;


        switch (index) {
            case 0:
                createAccount();
                break;

            case 1:
                deleteAccount();
                break;

	        case 2:
		        deposit();
		        break;

            case 3:
                withdraw();
                break;

            case 4:
                remit();
                break;

            case 5:
                printf("Exiting App...");
                break;

            default:
                printf("Invalid Choice\n");
        }
    } while (strcmp(choice, "Q") != 0);


    return 0;
}
void exitProgram() {
    printf("\nCorrupted File in databases. Please clear the folder and empty index.txt\n");
    exit(1);
}

// creates a delay.
void delay(double number_of_seconds){
    sleep(number_of_seconds);
}

// gets account type from file (S/C)
char getAccountType(char filePath[]) {
    char buffer[1024] = "";

    FILE *bfile = fopen(filePath, "r");
    fgets(buffer, sizeof(buffer), bfile);
    buffer[strcspn(buffer, "\n")] = 0;

    int typeIndex = (strstr(buffer, "TYPE:") == NULL) ? -1 : strstr(buffer, "TYPE:") - &buffer[0] + 5;
    if (typeIndex == -1) exitProgram();

    return buffer[typeIndex];
}

// performs remittance
void remit() {
    char accOriginNo[100] = "";
    char accDestNo[100] = "";

    // Gets + validates the origin account number from the user
    do {
        printf("Enter Origin Account Number: ");
        fgets(accOriginNo, sizeof(accOriginNo), stdin);
        accOriginNo[strcspn(accOriginNo, "\n")] = 0;
        strcat(accOriginNo, ".txt");

        if (!existsFile(accOriginNo)) printf("Enter an Existing Account No. \n\n");
    } while (!existsFile(accOriginNo));


    // Gets + validates the destination account number from the user
    do {
        printf("Enter Destination Account Number: ");
        fgets(accDestNo, sizeof(accDestNo), stdin);
        accDestNo[strcspn(accDestNo, "\n")] = 0;
        strcat(accDestNo, ".txt");

        if (!existsFile(accOriginNo)) printf("Account No. not Found \n\n");
        if (strcmp(accDestNo, accOriginNo)==0) printf("Cannot Send Money to the Same Bank Account \n\n");

    } while (!existsFile(accDestNo) || strcmp(accDestNo, accOriginNo)==0);

    //sets up the path for reading in the balances of both and modifying them.
    char pathBeneficiary[100] = "./database/";
    char pathRecipient[100] = "./database/";

    strcat(pathBeneficiary, accOriginNo);
    strcat(pathRecipient, accDestNo);

    pathBeneficiary[strlen(pathBeneficiary)] = 0;
    pathRecipient[strlen(pathRecipient)] = 0;

    double amountAdd = 0;
    double senderBalance = getBalance(pathBeneficiary);
    double receiverBalance = getBalance(pathRecipient);

    char amount[100] = "";
    char *endPtr;

    checkPIN(accOriginNo);

    // "Savings to Current account will incur a 2% remittance fee"
    // "(e.g. Transfer amount: RM100, remittance fee: RM2)."

    // "Current to Savings account will incur a 3% remittance fee"
    // "(e.g. Transfer amount: RM100, remittance fee: RM3)."

    // NO CHARGES WILL BE INCURRED FOR TRANSFERS BETWEEN THE SAME ACCOUNT TYPE
    double percentOnTop = 0;
    if ((getAccountType(pathBeneficiary)== 'C') && (getAccountType(pathRecipient)== 'S')) {
        percentOnTop = 0.03;
    }
    if ((getAccountType(pathBeneficiary)== 'S') && (getAccountType(pathRecipient)== 'C')) {
        percentOnTop = 0.02;
    }

    while (amountAdd <= 0 || senderBalance < amountAdd*(1+percentOnTop)) {
        printf("Enter Amount to Transfer: ");
        fgets(amount,sizeof(amount),stdin);
        amount[strcspn(amount, "\n")] = 0;

        amountAdd = strtod(amount, &endPtr);

        if (!isNum(amount)) continue;
        if (endPtr == amount || *endPtr != '\0') printf("Invalid input. Please enter a numeric value.\n\n");
        if (senderBalance < amountAdd) printf("Error - Insufficient Balance.\n\n");
        if (amountAdd == 0) printf("Error - Add Amount Must Be Greater than 0.\n\n");
    }

    double senderDeduct = (1+percentOnTop) * amountAdd;

    // "The remittance fee shall be deducted from the current balance of the sender"
    setBalance(pathRecipient, receiverBalance + amountAdd);
    setBalance(pathBeneficiary, senderBalance - senderDeduct);

    char displayAccountOriginNo[20] = "";
    char displayAccountDestNo[20] = "";

    for (int i=0; i<strlen(accDestNo); i++) {
        if (accDestNo[i] == '.') break;
        displayAccountDestNo[i] = accDestNo[i];
    }
    for (int i=0; i<strlen(accOriginNo); i++) {
        if (accOriginNo[i] == '.') break;
        displayAccountOriginNo[i] = accOriginNo[i];
    }

    printf("\n");
    format_chars();
    printf("                     SUCCESSFUL REMITTANCE RECEIPT\n");
    printf("                         %s ⟶ ⟶ ⟶ ⟶ ⟶ ⟶ ⟶ ⟶ ⟶ ⟶ ⟶ ⟶ ⟶ ⟶  %s\n", displayAccountOriginNo, displayAccountDestNo);
    printf("                         AMOUNT:                               RM %.2lf\n", amountAdd);
    printf("                         CHARGES INCURRED:                     RM %.2lf\n", percentOnTop*amountAdd);
    printf("                         DEDUCTED:  RM %.2lf + RM %.2lf  =  RM %.2lf\n", amountAdd, percentOnTop*amountAdd, senderDeduct);
    printf("                         %s: %.2lf - %.2lf  =  RM %.2lf\n", displayAccountOriginNo, senderBalance, senderDeduct, senderBalance - senderDeduct);
    format_chars();
    printf("\n");

}

// opens the account file and then checks if the PIN inside matches with the parameter.
bool checkPIN(char accountNumber[]) {
    char accDetails[1024] = "", PINUsr[100] = "";
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

void createAccount() {
    printf("                              CREATE ACCOUNT\n");
    format_chars();

    char name[1024] = "", PIN[1024] = "", IDNo[1024] = "", accType[2] = "";
    srand(time(NULL));

    int generatedNo;
    char strVersion[1024] = "";

    // generates a random account number in the valid range.
    // checks if the account number is already taken
    // if it's taken, the number is regenerated
    do {
        generatedNo = 1000000 + rand() % 999000000;
        snprintf(strVersion, sizeof(strVersion),"%d", generatedNo);
        strcat(strVersion, ".txt");

    } while (existsFile(strVersion));

    // adds creation to logs
    FILE *pfile = fopen("./database/transaction.log", "a");

    // gets + validates name from user
    do {
        printf("Enter Name: ");
        fgets(name, sizeof(name), stdin);
        name[strcspn(name, "\n")] = 0;

        if (!isStr(name)) printf("Enter a Valid String Name\n\n");
    } while (!isStr(name));

    // gets + validates ID from user
    do {
        printf("Enter ID Number: ");
        fgets(IDNo, sizeof(IDNo), stdin);
        IDNo[strcspn(IDNo, "\n")] = 0;

        if (!isNum(IDNo) || strlen(IDNo) != 8) printf("Enter a Valid, 8 Digit Integer ID\n\n");
    } while (!isNum(IDNo) || strlen(IDNo) != 8);


    // gets + validates PIN from user
    do {
        printf("Enter Account PIN: ");
        fgets(PIN, sizeof(PIN), stdin);
        PIN[strcspn(PIN, "\n")] = 0;

        if ((strlen(PIN) != 4) || !(isNum(PIN))) printf("Enter a Valid 4-digit Integer PIN \n\n");

    } while ((strlen(PIN) != 4) || !(isNum(PIN)));

    // gets + validates account type
    do {
        printf("Enter Account Type (S for Savings, C for Current):  ");
        scanf("%s", accType);

        if (strcasecmp(accType, "S") != 0 && strcasecmp(accType, "C") != 0) printf("Enter C or S \n\n");

    } while ((strcasecmp(accType, "S") != 0 && strcasecmp(accType, "C") != 0) || strlen(accType) == 0);

    printf("\n");
    format_chars();

    switch (toupper(accType[0])) {
        case 'S':
            printf("Created Account For %s. Account Number: %d. Account Type: Savings", name, generatedNo);
            break;

        case 'C':
            printf("Created Account For %s. Account Number: %d. Account Type: Current", name, generatedNo);
            break;
    }

    printf("\nBalance: RM 0");
    printf("\nChanges May Take a Few Moments to Take Place\n");
    format_chars();
    printf("\n");


    time_t currentTime;
    time(&currentTime);
    char logs[2048];
    accType[0] = toupper(accType[0]);

    // adds account creation to logs
    snprintf(logs,2048, "<createAccount>%d.txt,<type>%s,<time>%s",
        generatedNo, accType, ctime(&currentTime));

    fprintf(pfile, "%s", logs);
    fclose(pfile);
    addFile(strVersion, name, IDNo, accType, 0, PIN );

    while (getchar() != '\n');
}

void listAccounts() {
    FILE *pfile = fopen("./database/index.txt", "r");
    char bufferIndex[1024] = "", fileName[100] = "", out[200] = "";
    int typeIndex = 0;

    printf("ALL ACCOUNTS: \n\n");
    printf("  NUMBER   |    NAME  \n");
    while (fgets(bufferIndex, sizeof(bufferIndex), pfile) != NULL) {
        // Finds index of string to print until
        typeIndex = (strstr(bufferIndex, ".") == NULL) ? -1 : strstr(bufferIndex, ".") - &bufferIndex[0];
        if (typeIndex == -1) exitProgram();

        char displayName[100] = "";
        for (int i=0; i < typeIndex; i++) {
            displayName[i] = bufferIndex[i];
        }
        strcat(fileName, bufferIndex);

        // eliminates special characters from the string
        fileName[strcspn(fileName, "\a")] =0;
        fileName[strcspn(fileName, "\n")] =0;

        // formats the output to resemble a table
        strcat(out, displayName);
        strcat(out, "  |  ");

        char path[200] = "./database/" ;
        strcat(path, fileName);

        char accDetails[1024] = "";

        FILE *nameFile = fopen(path, "r");
        fgets(accDetails, sizeof(accDetails), nameFile);

        int endIndex = strstr(accDetails, ",") - &accDetails[0];

        char current[100] = "";
        for (int i=5; i<endIndex; i++) {
            current[i-5] = accDetails[i];
        }
        strcat(out, current);

        printf("%s", out);

        fclose(nameFile);

        printf("\n");

        // clears all the string so that the data doest get carried over from the previous iteration.
        strcpy(bufferIndex, "");
        strcpy(fileName, "");
        strcpy(out, "");

    }
    printf("\n");
}

void deleteAccount() {
    char accNo[1024] = "", accDetails[1024] = "", accConfirm[1024] = "";

    printf("                         DELETE ACCOUNT\n");
    format_chars();

    // lists all accounts
    listAccounts();

    // gets + validates account number from user
    do {
        printf("Enter Account Number: ");
        fgets(accNo, sizeof(accNo), stdin);
        accNo[strcspn(accNo, "\n")] = 0;
        strcat(accNo, ".txt");

        if (!existsFile(accNo)) {
            printf("Enter an Existing Account No. \n");
            format_chars();
            continue;
        }

        printf("Repeat Account Number: ");
        fgets(accConfirm, sizeof(accConfirm), stdin);
        accConfirm[strcspn(accConfirm, "\n")] = 0;
        strcat(accConfirm, ".txt");

        if (strcmp(accConfirm, accNo) != 0) {printf("\nAccount Details Do Not Match.");}

    } while ((!existsFile(accNo)) || (strcmp(accConfirm, accNo) != 0));

    char path[1024] = "./database/";
    strcat(path, accNo);
    strcat(path, "\0");

    FILE *delFIle = fopen(path, "r");
    fgets(accDetails, sizeof(accDetails), delFIle);

    // extracts the ID from the file
    int startID = (strstr(accDetails, "ID:") == NULL) ? -1 : strstr(accDetails, "ID:") - &accDetails[0] + 3;
    int IDlen = (strstr(accDetails, ",NUMBER:") == NULL) ? -1 : strstr(accDetails, ",NUMBER:") - &accDetails[startID];

    if (startID == -1 || IDlen == -1) exitProgram();

    char ID[50] = "", usrID[50] = "";
    for (int i=0; i < IDlen; i++) ID[i] = accDetails[startID+i];

    // gets + verifies the identity
    do {
        printf("Enter ID Number - last four characters: ");
        fgets(usrID, sizeof(usrID), stdin);
        usrID[strcspn(usrID, "\n")] = 0;

        if (!(usrID[0]==ID[strlen(ID)-4] &&
        usrID[1]==ID[strlen(ID)-3] &&
        usrID[2]==ID[strlen(ID)-2] &&
        usrID[3]==ID[strlen(ID)-1])) printf("Incorrect Detail. \n\n");


    } while (!(usrID[0]==ID[strlen(ID)-4] &&
        usrID[1]==ID[strlen(ID)-3] &&
        usrID[2]==ID[strlen(ID)-2] &&
        usrID[3]==ID[strlen(ID)-1]));

    checkPIN(accNo);

    // checks if the account exists, then deletes it
    if (existsFile(accNo)) {
        char contents[4096*2] = "", buffer[1024] = "";
        remove(path);

        FILE *modIndex = fopen("./database/index.txt", "r");
        while (fgets(buffer, sizeof(buffer), modIndex) != NULL) {
            buffer[strcspn(buffer, "\n")] = 0;
            if (!(strcmp(buffer, accNo) == 0)) {
                strcat(contents, buffer);
                strcat(contents, "\n");
            }
        }
        fclose(modIndex);

        FILE *writeFile = fopen("./database/index.txt", "w");
        fprintf(writeFile, "%s", contents);
        fclose(writeFile);

        format_chars();
        printf("ACCOUNT DELETED SUCCESSFULLY\n");
        format_chars();

        FILE *logFile = fopen("./database/transaction.log", "a");

        time_t currentTime;
        time(&currentTime);
        char logs[2048];

        // adds account deletion to logs
        snprintf(logs,sizeof(logs), "<deleteAccount>%s<time>%s",accNo, ctime(&currentTime));

        fprintf(logFile, "%s", logs);
        fclose(logFile);
    }
}

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

// writes the data to the file corresponding to the account number
// used for updating the balance during deposit/withdrawal
void addFile(char accNo[], char name[], char ID[], char accType[], float balance, char PIN[]) {
    char finalFile[1024] = "./database/";
    strcat(finalFile, accNo);
    FILE *pfile = fopen(finalFile, "w");

    if (!(pfile == NULL)) {
        char logs[2048];
        time_t currentTime;
        time(&currentTime);


        snprintf(logs,sizeof(logs), "NAME:%s,ID:%s,NUMBER:%s,TYPE:%s,BALANCE:%.2f,PIN:%s",
            name, ID, accNo, accType, balance, PIN);

        fprintf(pfile, "%s" ,logs);
        fclose(pfile);

        FILE *index = fopen("./database/index.txt", "a");
        fprintf(index, "%s\n", accNo);
        fclose(index);
    }
}

// function for verifying if a string is a number
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

// Checks if the string is entirely alphabetic
bool isStr(char inString[]) {
    const char* pattern = "[a-zA-Z]";
    regex_t re;
    int returnVal;

    if (regcomp(&re, pattern, REG_EXTENDED)) {
        perror("Failed to compile regular expression");
        exit(EXIT_FAILURE);
    }

    for (int i=0; i<strlen(inString); i++) {
        returnVal = regexec(&re, &inString[i], 0, NULL, 0);

        if (returnVal != 0) return false;
    }

    return true;
}

// deposit function
void deposit() {
    char accountNumber[1024] = "";
    // gets account number + validates it

    do {
        printf("                       DEPOSIT INTO ACCOUNT\n");
        format_chars();

        printf("Enter Account Number: ");
        fgets(accountNumber, sizeof(accountNumber), stdin);
	    accountNumber[strcspn(accountNumber, "\n")] = 0;
        strcat(accountNumber, ".txt");

        if (!existsFile(accountNumber)) printf("Enter an Existing Account No.\n");
        format_chars();

        errno = 0;
    } while(!existsFile(accountNumber));


    if (existsFile(accountNumber)) {

        char path[100] = "./database/";
        strcat(path, accountNumber);
        path[strlen(path)] = 0;

        // gets PIN from user and validates it
        checkPIN(accountNumber);

        double finalBalance = getBalance(path);

        double amountAdd = 0;
        char amount[100] = "";
        char *endPtr;

        // gets + validates deposit amount
        while (amountAdd <= 0 || amountAdd > 50000 || !isNum(amount)) {
            printf("Enter Amount to deposit: ");
            fgets(amount,sizeof(amount),stdin);
            amount[strcspn(amount, "\n")] = 0;

            amountAdd = strtod(amount, &endPtr);

            if (!isNum(amount)) printf("Invalid input. Please enter a numeric value.\n\n");
            else if (amountAdd > 50000) printf("Deposit Amount Cannot Be Greater than RM 50,000\n\n");
            else if (amountAdd<=0) printf("Please Enter a Value Above 0.\n\n");

        }

        // updates the balance and writes the balance to the file.
        finalBalance += amountAdd;
        setBalance(path, finalBalance);

        FILE *logFile = fopen("./database/transaction.log", "a");

        time_t currentTime;
        time(&currentTime);
        char logs[2048];

        // adds deposition to logs
        snprintf(logs,sizeof(logs), "<depositAccount>%s<amount>%.2f<time>%s",accountNumber, amountAdd, ctime(&currentTime));
        fprintf(logFile, "%s", logs);
        fclose(logFile);

        // prints the account number and the logs in a presentable manner, i.e no '.txt' at the end
        char displayAccount[20] = "";

        for (int i=0; i<strlen(accountNumber); i++) {
            if (accountNumber[i] == '.') break;
            displayAccount[i] = accountNumber[i];
        }

        printf("\n");
        format_chars();
        printf("DEPOSIT OF RM %.2f WAS SUCCESSFULLY TRANSFERRED TO ACCOUNT NO. %s \n"
               "CURRENT BALANCE IN %s: RM %.2f\n", amountAdd, displayAccount, displayAccount, finalBalance);
        format_chars();
        printf("\n");

    }
}
void withdraw() {
    char accountNumber[1024] = "";
    printf("                       WITHDRAW FROM ACCOUNT\n");

    //gets account number
    do {
        format_chars();

        printf("Enter Account Number: ");
        fgets(accountNumber, sizeof(accountNumber), stdin);
	    accountNumber[strcspn(accountNumber, "\n")] = 0;
        strcat(accountNumber, ".txt");

    } while(!existsFile(accountNumber));

    if (existsFile(accountNumber)) {

        checkPIN(accountNumber);

        char path[100] = "./database/";
        strcat(path, accountNumber);
        path[strlen(path)] = 0;



        double amountWithdraw = 0;
        double finalBalance = getBalance(path);
        char amount[100] = "";
        char *endPtr;

        printf("\nCurrent Balance:  RM %.2lf\n", finalBalance);

        // gets + validates withdrawal amount from user
        do {
            printf("Enter Amount to Withdraw: ");
            fgets(amount,sizeof(amount),stdin);
            amount[strcspn(amount, "\n")] = 0;

            amountWithdraw = strtod(amount, &endPtr);

            if (endPtr == amount || *endPtr != '\0') printf("Invalid input. Please enter a numeric value.\n\n");
            if (finalBalance-amountWithdraw < 0.0f) printf("Withdrawal amount is greater than the balance.\n\n");
            if (amountWithdraw <= 0.0f) printf("Withdrawal amount must be greater than 0\n\n");

        } while (amountWithdraw <= 0.0f || finalBalance-amountWithdraw < 0.0f);
        finalBalance -= amountWithdraw;

        setBalance(path, finalBalance);

        char finalPrint[90] = "";

        FILE *logFile = fopen("./database/transaction.log", "a");
        strcat(finalPrint, "\0");

        time_t currentTime;
        time(&currentTime);
        char logs[2048];

        // adds withdrawal to logs
        snprintf(logs,sizeof(logs), "<withdrawAccount>%s<amount>%.2f<time>%s",accountNumber, amountWithdraw, ctime(&currentTime));

        fprintf(logFile, "%s", logs);
        fclose(logFile);

        char displayAccount[20] = "";

        for (int i=0; i<strlen(accountNumber); i++) {
            if (accountNumber[i] == '.') break;
            displayAccount[i] = accountNumber[i];
        }

        printf("\n");
        format_chars();
        printf("RM %.2f WAS SUCCESSFULLY WITHDRAWN FROM ACCOUNT NO. %s. \n"
               "CURRENT BALANCE: RM %.2f\n", amountWithdraw, displayAccount, finalBalance);
        format_chars();
        printf("\n");

    }
}

// adds spacing and asterisks to make beautify the interface
void format_chars() {
    for (int i=0; i<80; i++)printf("_");
    printf("\n");
}
