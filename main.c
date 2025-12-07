#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "formatChars.h"
#include "isNum.h"
#include "existsFile.h"
#include "delay.h"
#include "getAccDetails.h"
#include "checkPIN.h"
#include "setBalance.h"
#include "isStr.h"
#include "addFile.h"
#include "displayInfo.h"


// MAIN Program:
//      Contains all 6 core functions  along with utilities that cannot be separated into header files

void createAccount();
void deleteAccount();
void deposit();
void withdraw();
void listAccounts();
void remit();

int main() {
    // initializes all required files for the program
    // checks if necessary files exist and makes them if not.
    struct stat st = {0};
    if (stat("./database", &st) == -1) {
        mkdir("./database", 0700);

        printf("\n Please Wait....\n");
        printf("\n Note: When Creating Account, Make Sure to Remember the Account Number and PIN. See the User Guide For More Details");
        // allows for delay in file creation
        delay(10);
    }
    FILE *checkIndex = fopen("./database/index.txt", "ab+");
    fclose(checkIndex);

    FILE *checkTransaction = fopen("./database/transaction.log", "ab+");
    fclose(checkTransaction);

    displayInfo();

    char choice[1024];
    char options[6][100] = {"1CREATENEWBANKACCOUNT", "2DELETEACCOUNT",
        "3DEPOSITINACCOUNT", "4WITHDRAWFROMACCOUNT", "5REMITTANCE", "QUIT"};

    do {
        // Format and output welcome message
        delay(0.5);
        formatChars();
        printf("                        WELCOME TO SOFTBANK              ");
        printf("\n                            1. CREATE NEW BANK ACCOUNT");
        printf("\n                            2. DELETE ACCOUNT");
        printf("\n                            3. DEPOSIT IN ACCOUNT");
        printf("\n                            4. WITHDRAW FROM ACCOUNT");
        printf("\n                            5. REMITTANCE\n");
        printf("\n                            Q. QUIT\n");
        formatChars();

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
        formatChars();

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
                exit(0);

            default:
                printf("Invalid Choice\n");
        }
    } while (strcmp(choice, "Q") != 0);


    return 0;
}


void createAccount() {
    printf("                              CREATE ACCOUNT\n");
    formatChars();

    char name[1024] = "", PIN[1024] = "", IDNo[1024] = "", accType[8] = "";
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
        else if (strlen(name) == 0) printf("Enter a Valid String Name\n\n");

    } while (!isStr(name) || (strlen(name) == 0));

    // gets + validates ID from user
    do {
        printf("Enter ID Number: ");
        fgets(IDNo, sizeof(IDNo), stdin);
        IDNo[strcspn(IDNo, "\n")] = 0;

        if (!isInt(IDNo) || strlen(IDNo) != 8) printf("Enter a Valid, 8 Digit Integer ID\n\n");

    } while (!isInt(IDNo) || strlen(IDNo) != 8);


    // gets + validates PIN from user
    do {
        printf("Enter Account PIN: ");
        fgets(PIN, sizeof(PIN), stdin);
        PIN[strcspn(PIN, "\n")] = 0;

        if ((strlen(PIN) != 4) || !(isInt(PIN))) printf("Enter a Valid 4-digit Integer PIN \n\n");

    } while ((strlen(PIN) != 4) || !(isInt(PIN)));

    // gets + validates account type
    do {
        printf("Enter Account Type (S for Savings, C for Current):  ");
        fgets(accType, sizeof(accType), stdin);
        accType[strcspn(accType, "\n")] = 0;
        if (strcasecmp(accType, "S") != 0 && strcasecmp(accType, "C") != 0) printf("Enter C or S \n\n");

    } while ((strcasecmp(accType, "S") != 0 && strcasecmp(accType, "C") != 0) || strlen(accType) == 0);

    printf("\n");
    formatChars();

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
    formatChars();
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

}

void listAccounts() {
    FILE *pfile = fopen("./database/index.txt", "r");
    char bufferIndex[1024] = "", fileName[100] = "", out[200] = "";
    int typeIndex = 0;

    printf("ALL ACCOUNTS: \n\n");
    printf("  NUMBER     |    NAME  \n");
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

        printf("%11s  |  %s", displayName, current);

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
    formatChars();

    // lists all accounts
    listAccounts();

    // gets + validates account number from user
    do {
        printf("Enter Account Number: ");
        fgets(accNo, sizeof(accNo), stdin);
        accNo[strcspn(accNo, "\n")] = 0;
        strcat(accNo, ".txt");

        if (!existsFile(accNo)) {
            printf("Account Not Found\n\n");
            formatChars();
            continue;
        }

        printf("Repeat Account Number: ");
        fgets(accConfirm, sizeof(accConfirm), stdin);
        accConfirm[strcspn(accConfirm, "\n")] = 0;
        strcat(accConfirm, ".txt");

        if (strcmp(accConfirm, accNo) != 0) {printf("Account Details Do Not Match.\n");}

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

        formatChars();
        printf("ACCOUNT DELETED SUCCESSFULLY\n");
        formatChars();

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

void remit() {
    char accOriginNo[100] = "";
    char accDestNo[100] = "";

    // Gets + validates the origin account number from the user
    do {
        printf("Enter Origin Account Number: ");
        fgets(accOriginNo, sizeof(accOriginNo), stdin);
        accOriginNo[strcspn(accOriginNo, "\n")] = 0;
        strcat(accOriginNo, ".txt");

        if (!existsFile(accOriginNo)) {printf("Account Not Found\n\n");}
    } while (!existsFile(accOriginNo));


    // Gets + validates the destination account number from the user
    do {
        printf("Enter Destination Account Number: ");
        fgets(accDestNo, sizeof(accDestNo), stdin);
        accDestNo[strcspn(accDestNo, "\n")] = 0;
        strcat(accDestNo, ".txt");

        if (!existsFile(accOriginNo)) {printf("Account Not Found\n\n");}
        else if (strcmp(accDestNo, accOriginNo)==0) printf("Cannot Send Money to the Same Bank Account \n\n");

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

    while (amountAdd <= 0 || senderBalance < amountAdd*(1+percentOnTop) || !isNum(amount)) {
        amount[0] = '\0';

        printf("Enter Amount to Transfer: RM ");
        fgets(amount,sizeof(amount),stdin);
        amount[strcspn(amount, "\n")] = 0;

        amountAdd = strtod(amount, &endPtr);

        if (!isNum(amount)) printf("Invalid input. Please enter a numeric value.\n\n");
        else if (endPtr == amount || *endPtr != '\0') printf("Invalid input. Please enter a numeric value.\n\n");
        else if (senderBalance < amountAdd || senderBalance < amountAdd*(1+percentOnTop)) printf("Error - Insufficient Balance.\n\n");
        else if (amountAdd == 0) printf("Error - Add Amount Must Be Greater than 0.\n\n");
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
    formatChars();
    printf("                     SUCCESSFUL REMITTANCE RECEIPT\n");
    printf("                         %s ⟶ ⟶ ⟶ ⟶ ⟶ ⟶ ⟶ ⟶ ⟶ ⟶ ⟶ ⟶ ⟶ ⟶  %s\n", displayAccountOriginNo, displayAccountDestNo);
    printf("                         AMOUNT:                               RM %.2lf\n", amountAdd);
    printf("                         CHARGES INCURRED:                     RM %.2lf\n", percentOnTop*amountAdd);
    printf("                         DEDUCTED:  RM %.2lf + RM %.2lf        RM %.2lf\n", amountAdd, percentOnTop*amountAdd, senderDeduct);
    printf("                         %s: %.2lf - %.2lf         RM %.2lf\n", displayAccountOriginNo, senderBalance, senderDeduct, senderBalance - senderDeduct);
    formatChars();
    printf("\n");

}

// deposit function
void deposit() {
    char accountNumber[1024] = "";
    // gets account number + validates it

    do {
        printf("                       DEPOSIT INTO ACCOUNT\n");
        formatChars();

        printf("Enter Account Number: ");
        fgets(accountNumber, sizeof(accountNumber), stdin);
	    accountNumber[strcspn(accountNumber, "\n")] = 0;
        strcat(accountNumber, ".txt");

        if (!existsFile(accountNumber)) {printf("Account Not Found\n\n");}
        formatChars();

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
            amount[0] = '\0';

            printf("Enter Amount to deposit: RM ");
            fgets(amount,sizeof(amount),stdin);
            amount[strcspn(amount, "\n")] = 0;

            amountAdd = strtod(amount, &endPtr);

            if (!isNum(amount)) {printf("Invalid input. Please enter a numeric value.\n\n");}
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
        formatChars();
        printf("RM %.2f ⟶ ⟶ ⟶ ⟶ ⟶ ⟶ ⟶ ⟶ ⟶ ⟶ ⟶ ⟶ ⟶ ACCOUNT NO. %s \n"
               "CURRENT BALANCE IN %s:         RM %.2f\n", amountAdd, displayAccount, displayAccount, finalBalance);
        formatChars();
        printf("\n");

    }
}
void withdraw() {
    char accountNumber[1024] = "";
    printf("                       WITHDRAW FROM ACCOUNT\n");

    //gets account number
    do {
        formatChars();

        printf("Enter Account Number: ");
        fgets(accountNumber, sizeof(accountNumber), stdin);
	    accountNumber[strcspn(accountNumber, "\n")] = 0;
        strcat(accountNumber, ".txt");

        if (!existsFile(accountNumber)) {printf("Account Not Found\n");}
        else {
            char path[100] = "./database/";
            strcat(path, accountNumber);
            path[strlen(path)] = 0;

            double finalBalance = getBalance(path);

            if (finalBalance == 0) printf("Error - Insufficient Balance.\n\n");

        }

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
        while (amountWithdraw <= 0.0f || finalBalance-amountWithdraw < 0.0f || !isNum(amount)) {
            printf("Enter Amount to Withdraw: RM ");
            fgets(amount,sizeof(amount),stdin);
            amount[strcspn(amount, "\n")] = 0;

            amountWithdraw = strtod(amount, &endPtr);

            if (!isNum(amount)) {printf("Invalid input. Please enter a numeric value.\n\n");}
            else if (endPtr == amount || *endPtr != '\0') printf("Invalid input. Please enter a numeric value.\n\n");
            else if (finalBalance-amountWithdraw < 0.0f) printf("Withdrawal amount is greater than the balance.\n\n");
            else if (amountWithdraw <= 0.0f) printf("Withdrawal amount must be greater than 0\n\n");
        }

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
        formatChars();
        printf("RM %.2f ← ← ← ← ← ← ← ← ← NO. %s. \n"
               "CURRENT BALANCE:          RM %.2f\n", amountWithdraw, displayAccount, finalBalance);
        formatChars();
        printf("\n");

    }
}

