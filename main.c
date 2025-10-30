#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <regex.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

bool existsFile(char accNo[]);
void createAccount();
void addFile(char accNo[], char name[], char ID[], char accType[], float balance, char PIN[]);
bool isStr(char inString[]);
bool isNum(char inString[]);
void deleteAccount();
void deposit();
void format_chars();
void withdraw();

int main() {
    char choice[1024];
    char options[6][100] = {"1. CREATE NEW BANK ACCOUNT", "2. DELETE BANK ACCOUNT",
        "3. DEPOSIT IN BANK", "4. WITHDRAW FROM BANK", "5. REMITTANCE", "Q. QUIT"};

    do {
        // Format and output welcome message
        format_chars();
        printf("             WELCOME TO SOFTBANK              ");
        printf("\n                 1. CREATE NEW BANK ACCOUNT");
        printf("\n                 2. DELETE BANK ACCOUNT");
        printf("\n                 3. DEPOSIT IN BANK");
        printf("\n                 4. WITHDRAW FROM BANK");
        printf("\n                 5. REMITTANCE\n");
        printf("\n                 Q. QUIT");
        format_chars();

        // Get choice from user
        printf("ENTER CHOICE: ");

        // get user input & remove newline character
        fgets(choice, sizeof(choice), stdin);
        choice[strcspn(choice, "\n")] = 0;

        if (strcmp(choice, "") == 0) continue;

        // convert to upper case for comparison
        for (int i=0; i<strlen(choice); i++) choice[i] = toupper(choice[i]);

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

            case 5:
                printf("\nExiting App...");
                break;

            default:
                printf("\nInvalid Choice\n");
        }
    } while (strcmp(choice, "Q") != 0);


    return 0;
}
void createAccount() {
    printf("                       CREATE ACCOUNT");
    format_chars();

    char name[1024] = "", PIN[1024] = "", IDNo[1024] = "", accType[2] = "";
    srand(time(NULL));

    int generatedNo;
    char strVersion[1024] = "";

    //generates a random account number in the valid range.
    // checks if the account number is already taken
    // if it's taken, the number is regenerated
    do {
        generatedNo = 1000000 + rand() % 999000000;
        snprintf(strVersion, sizeof(strVersion),"%d", generatedNo);
        strcat(strVersion, ".txt");

    } while (existsFile(strVersion));

    FILE *pfile = fopen("/home/moiz/CLionProjects/Assignment/database/transaction.log", "a");

    do {
        printf("Enter Name: ");
        fgets(name, sizeof(name), stdin);
        name[strcspn(name, "\n")] = 0;

        if (!isStr(name)) printf("Enter a Valid String Name\n\n");
    } while (!isStr(name));

    do {
        printf("Enter ID Number: ");
        fgets(IDNo, sizeof(IDNo), stdin);
        IDNo[strcspn(IDNo, "\n")] = 0;

        if (!isStr(name)) printf("Enter a Valid Integer ID\n\n");
    } while (!isNum(IDNo));


    // gets PIN from user
    do {
        printf("Enter Account PIN: ");
        fgets(PIN, sizeof(PIN), stdin);
        PIN[strcspn(PIN, "\n")] = 0;

        if ((strlen(PIN) != 4) || !(isNum(PIN))) printf("Enter a Valid 4-digit Integer PIN \n\n");

    } while ((strlen(PIN) != 4) || !(isNum(PIN)));

    // asks user to input account type until it is either C or S
    do {
        printf("Enter Account Type (S for Savings, C for Current):  ");
        scanf("%s", accType);

        if (strcasecmp(accType, "S") != 0 && strcasecmp(accType, "C") != 0) printf("Enter C or S \n\n");

    } while (strcasecmp(accType, "S") != 0 && strcasecmp(accType, "C") != 0);

    printf("\nCreated Account For %s. Account Number: %d. Account Type: %s", name, generatedNo, accType);
    printf("\nBalance: RM 0");

    time_t currentTime;
    time(&currentTime);
    char logs[1024];
    accType[0] = toupper(accType[0]);


    // adds account creation to logs
    snprintf(logs,1024, "<createAccount>%d.txt,<type>%s,<time>%s",
        generatedNo, accType, ctime(&currentTime));

    fprintf(pfile, "%s", logs);
    fclose(pfile);
    addFile(strVersion, name, IDNo, accType, 0, PIN );

    while (getchar() != '\n');
}
void deleteAccount() {
    FILE *pfile = fopen("/home/moiz/CLionProjects/Assignment/database/index.txt", "r");
    char buffer[1024] = "", accNo[1024] = "", accDetails[1024] = "";
    int typeIndex = 0;

    printf("\n                         DELETE ACCOUNT");
    format_chars();

    printf("ALL ACCOUNTS: \n");
    while (fgets(buffer, sizeof(buffer), pfile) != NULL) {
        typeIndex = strstr(buffer, ".") - &buffer[0];
        for (int i=0; i<typeIndex; i++) printf("%c", buffer[i]);
        printf("\n");
    }

    do {
        printf("Enter Account Number: ");
        fgets(accNo, sizeof(accNo), stdin);
        accNo[strcspn(accNo, "\n")] = 0;
        strcat(accNo, ".txt");
        if (!existsFile(accNo)) printf("Enter an Existing Account No. \n\n");

    } while (!existsFile(accNo));

    char path[1024] = "/home/moiz/CLionProjects/Assignment/database/";
    strcat(path, accNo);
    strcat(path, "\0");

    FILE *delFIle = fopen(path, "r");
    fgets(accDetails, sizeof(accDetails), delFIle);

    int startPIN = strstr(accDetails, "PIN:") - &accDetails[0];
    int startID = strstr(accDetails, "ID:") - &accDetails[0] + 3;
    int IDlen = strstr(accDetails, ",NUMBER:") - &accDetails[startID];

    char ID[50] = "", usrID[50] = "", PINUsr[6] = "";
    for (int i=0; i < IDlen; i++) ID[i] = accDetails[startID+i];

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

    char pin[5] = {accDetails[startPIN+4], accDetails[startPIN+5], accDetails[startPIN+6], accDetails[startPIN+7], '\0'};

    do {
        printf("Enter PIN: ");
        fgets(PINUsr, sizeof(PINUsr), stdin);
        PINUsr[strcspn(PINUsr, "\n")] = 0;

        if (strcmp(PINUsr, pin) != 0) printf("Incorrect PIN. \n\n");

    } while (strcmp(PINUsr, pin) != 0);

    if (existsFile(accNo)) {
        char contents[4096*2] = "", buffer[1024] = "";

        remove(path);

        FILE *modIndex = fopen("/home/moiz/CLionProjects/Assignment/database/index.txt", "r");
        while (fgets(buffer, sizeof(buffer), modIndex) != NULL) {
            buffer[strcspn(buffer, "\n")] = 0;
            if (!(strcmp(buffer, accNo) == 0)) {
                strcat(contents, buffer);
                strcat(contents, "\n");
            }
        }
        fclose(modIndex);

        FILE *writeFile = fopen("/home/moiz/CLionProjects/Assignment/database/index.txt", "w");
        fprintf(writeFile, "%s", contents);
        fclose(writeFile);

        format_chars();
        printf("\nACCOUNT DELETED SUCCESSFULLY\n");
        format_chars();

        FILE *logFile = fopen("/home/moiz/CLionProjects/Assignment/database/transaction.log", "a");


        time_t currentTime;
        time(&currentTime);
        char logs[1024];

        // adds account deletion to logs
        snprintf(logs,1024, "<depositAccount>%s,<time>%s",accNo, ctime(&currentTime));

        fprintf(logFile, "%s", logs);
        fclose(logFile);
    }
}
bool existsFile(char accNo[]) {
    FILE *pfile = fopen("/home/moiz/CLionProjects/Assignment/database/index.txt", "r");
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
void addFile(char accNo[], char name[], char ID[], char accType[], float balance, char PIN[]) {
    char finalFile[1024] = "/home/moiz/CLionProjects/Assignment/database/";
    strcat(finalFile, accNo);
    FILE *pfile = fopen(finalFile, "w");

    if (!(pfile == NULL)) {
        char logs[1024];
        time_t currentTime;
        time(&currentTime);

        // adds account creation to logs
        snprintf(logs,1024, "NAME:%s,ID:%s,NUMBER:%s,TYPE:%s,BALANCE:%.2f,PIN:%s",
            name, ID, accNo, accType, balance, PIN);

        fprintf(pfile, logs);
        fclose(pfile);

        FILE *index = fopen("/home/moiz/CLionProjects/Assignment/database/index.txt", "a");
        fprintf(index, "%s\n", accNo);
        fclose(index);
    }
}
bool isNum(char inString[]) {
    const char* pattern = "[0-9]";
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
void deposit() {
    char accountNumber[1024] = "";
    do {
        printf("                       DEPOSIT INTO ACCOUNT");
        format_chars();

        printf("Enter Account Number: ");
        fgets(accountNumber, sizeof(accountNumber), stdin);
	    accountNumber[strcspn(accountNumber, "\n")] = 0;
        strcat(accountNumber, ".txt");

        if (!existsFile(accountNumber)) printf("Enter an Existing Account No. \n\n");

        errno = 0;
    } while(!existsFile(accountNumber));

    if (existsFile(accountNumber)) {
        char buffer[1024] = "";
        char balance[10] = "";

        char path[100] = "/home/moiz/CLionProjects/Assignment/database/";
        strcat(path, accountNumber);
        path[strlen(path)] = 0;

        FILE *pfile = fopen(path, "r");
        fgets(buffer, sizeof(buffer), pfile);
        fclose(pfile);
        buffer[strcspn(buffer, "\n")] = 0;

        int balStart = strstr(buffer, "BALANCE:") - &buffer[0] + 8;
        int balEnd = strstr(buffer, ",PIN:") - &buffer[0];
        int length = balEnd - balStart;

        for (int z=0; z<length; z++) {
            char now[2] = {buffer[balStart + z], '\0'};
   		    strcat(balance, &now[0]);
        }

        double finalBalance = 0;

        strcat(balance, "\0");
        finalBalance = atof(balance);

        double amountAdd = 0;
        char amount[100] = "";
        char *endPtr;

        while (amountAdd <= 0) {
            printf("Enter Amount to deposit: ");
            fgets(amount,sizeof(amount),stdin);
            amount[strcspn(amount, "\n")] = 0;

            amountAdd = strtod(amount, &endPtr);

            if (endPtr == amount || *endPtr != '\0') {
                printf("Invalid input. Please enter a numeric value.\n\n");
            }
        }

        finalBalance += amountAdd;

        char finalPrint[90] = "";
        char finalBalanceString[100] = "";
        snprintf(finalBalanceString, sizeof(finalBalanceString), "%.2f", finalBalance);
        strcat(finalBalanceString, "\0");

        char cache[2] = {'s', '\0'};
        for (int z=0; z<balStart; z++) {
            cache[0] = buffer[z];
            strcat(finalPrint, cache);
        }

        strcat(finalPrint, finalBalanceString);
        strcat(finalPrint, ",PIN:3342");
        strcat(finalPrint, "\0");

        FILE *wfile = fopen(path, "w");
        fprintf(wfile, "%s\n", finalPrint);
        fclose(wfile);

        FILE *logFile = fopen("/home/moiz/CLionProjects/Assignment/database/transaction.log", "a");

        time_t currentTime;
        time(&currentTime);
        char logs[1024];

        // adds deposition to logs
        snprintf(logs,1024, "<depositAccount>%s,<amount>%.2f,<time>%s",accountNumber, amountAdd, ctime(&currentTime));
        fprintf(logFile, "%s", logs);
        fclose(logFile);

        char displayAccount[20] = "";

        for (int i=0; i<strlen(accountNumber); i++) {
            if (accountNumber[i] == '.') break;

            displayAccount[i] = accountNumber[i];
        }

        format_chars();
        printf("DEPOSIT OF RM %.2f WAS SUCCESSFULLY TRANSFERRED TO ACCOUNT NO. %s. \n"
               "CURRENT BALANCE: RM %.2f", amountAdd, displayAccount, finalBalance);
        format_chars();

    }
}
void withdraw() {
    char accountNumber[1024] = "";
    printf("                       WITHDRAW FROM ACCOUNT");
    do {
        format_chars();

        printf("Enter Account Number: ");
        fgets(accountNumber, sizeof(accountNumber), stdin);
	    accountNumber[strcspn(accountNumber, "\n")] = 0;
        strcat(accountNumber, ".txt");

        errno = 0;

    } while(!existsFile(accountNumber));

    if (existsFile(accountNumber)) {
        char buffer[1024] = "";
        char balance[100] = "";


        char path[100] = "/home/moiz/CLionProjects/Assignment/database/";
        strcat(path, accountNumber);
        path[strlen(path)] = 0;

        FILE *pfile = fopen(path, "r");
        fgets(buffer, sizeof(buffer), pfile);
        buffer[strcspn(buffer, "\n")] = 0;

        int balStart = strstr(buffer, "BALANCE:") - &buffer[0] + 8;
        int balEnd = strstr(buffer, ",PIN:") - &buffer[0];
        int length = balEnd - balStart;

        for (int z=0; z<length; z++) {
            char now[2] = {buffer[balStart + z], '\0'};
            strcat(balance, &now[0]);
        }

        double amountWithdraw = 0;
        double finalBalance = atof(balance);
        char amount[100] = "";
        char *endPtr;

        do {
            printf("Enter Amount to Withdraw: ");
            fgets(amount,sizeof(amount),stdin);
            amount[strcspn(amount, "\n")] = 0;

            amountWithdraw = strtod(amount, &endPtr);

            if (endPtr == amount || *endPtr != '\0') {
                printf("Invalid input. Please enter a numeric value.\n\n");
            }
            finalBalance -= amountWithdraw;

        } while (amountWithdraw <= 0.0f || finalBalance < 0.0f);


        char finalPrint[90] = "";
        char finalBalanceString[100] = "";
        snprintf(finalBalanceString, sizeof(finalBalanceString), "%.2f", finalBalance);

        char cache[2] = {'s', '\0'};
        for (int z=0; z<balStart; z++) {
            cache[0] = buffer[z];
            strcat(finalPrint, cache);
        }

        strcat(finalPrint, finalBalanceString);
        strcat(finalPrint, ",PIN:3342");

        FILE *wfile = fopen(path, "w");
        fprintf(wfile, "%s\n", finalPrint);
        fclose(wfile);

        FILE *logFile = fopen("/home/moiz/CLionProjects/Assignment/database/transaction.log", "a");

        strcat(finalPrint, "\0");

        time_t currentTime;
        time(&currentTime);
        char logs[1024];

        // adds withdrawal to logs
        snprintf(logs,sizeof(logs), "<withdrawAccount>%s,<amount>%.2f,<time>%s",accountNumber, amountWithdraw, ctime(&currentTime));

        fprintf(logFile, "%s", logs);
        fclose(logFile);

        char displayAccount[20] = "";

        for (int i=0; i<strlen(accountNumber); i++) {
            if (accountNumber[i] == '.') break;
            displayAccount[i] = accountNumber[i];
        }

        // CHECK THE POINTER CONCATENATION AND FIX AMOUNT CHANGE BUG

        format_chars();
        printf("RM %.2f WAS SUCCESSFULLY WITHDRAWN FROM ACCOUNT NO. %s. \n"
               "CURRENT BALANCE: RM %.2f", amountWithdraw, displayAccount, finalBalance);
        format_chars();

    }


}
void format_chars() {
    printf("\n");
    for (int i=0; i<70; i++)printf("*");
    printf("\n");
}
