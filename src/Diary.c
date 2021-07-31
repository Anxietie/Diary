#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define USERHOME getenv("USERPROFILE")
#define PATH concatenate(USERHOME, "\\Documents")
#define DIARY concatenate(PATH, "\\Diary.txt")
#define DIARY_T concatenate(PATH, "\\Diary_T.txt")


int createPassword(const char* fName, const char* pass);
char* getPass(const char* fName);
int addEntry();
int viewEntry(const char* fName, int entry);
int delEntry(const char* fName, int entry);
int editPass(const char* fName, const char* pass);
int fileExists(const char* fName);
int getNextRecNum(const char* fName);
char* concatenate(const char* str1, const char* str2);

short int a = 0;

int main(int argc, char* argv[]) {
    if (USERHOME == NULL) {
        printf("User home not found, program cannot continue.");
        exit(EXIT_FAILURE);
    }
    short int choice;
    char pass[21];
    if (a==0) {
        if (fileExists(DIARY)==1) {
            printf("You do not have a password yet. What would you like your password to be (max 20 characters, no spaces)? ");
            scanf("%20s", pass);
            createPassword(DIARY, pass);
            a = 1;
        }
        else {
            printf("What is the password? ");
            scanf("%20s", pass);
            if (strcmp(pass, getPass(DIARY))==1){
                printf("Access denied.");
                exit(0);
            }
            a = 1;
        }
    }
    printf("***********************\n"
        "*WELCOME TO YOUR DIARY*\n"
        "***********************"
        "\n\n\t"
        "MAIN MENU:"
        "\n\n"
        "ADD RECORD: [1]\n"
        "VIEW RECORD: [2]\n"
        "DELETE RECORD: [3]\n"
        "EDIT PASSWORD: [4]\n"
        "EXIT: [5]\n\n"
        "What would you like to do? ");
    do {
        int rec;
        char pass[21];
        char again;
        scanf("%i", &choice);
        switch (choice) {
            case 1:
                addEntry();
                for(;;) {
                    printf("would you like to add another entry (y/n)? ");
                    scanf("%c", &again);
                    fflush(stdin);
                    switch (again) {
                        case 'y':
                            addEntry();
                            fflush(stdin);
                            break;
                        case 'n':
                            main(argc, argv);
                            break;
                        default:
                            printf("Please type a valid input!\n");
                            break;
                    }
                }
                break;
            case 2:
                printf("What record would you like to view? ");
                scanf("%i", &rec);
                fflush(stdin);
                viewEntry(DIARY, rec);
                for(;;) {
                    printf("would you like to view another record (y/n)? ");
                    scanf("%c", &again);
                    fflush(stdin);
                    switch (again) {
                        case 'y':
                            printf("What record would you like to view? ");
                            scanf("%i", &rec);
                            fflush(stdin);
                            viewEntry(DIARY, rec);
                            break;
                        case 'n':
                            main(argc, argv);
                            break;
                        default:
                            printf("Please type a valid input!\n");
                            break;
                    }
                }
                break;
            case 3:
                printf("What record would you like to delete? ");
                scanf("%i", &rec);
                fflush(stdin);
                delEntry(DIARY, rec);
                for(;;) {
                    printf("would you like to delete another record (y/n)? ");
                    scanf("%c", &again);
                    fflush(stdin);
                    switch (again) {
                        case 'y':
                            printf("What record would you like to delete? ");
                            scanf("%i", &rec);
                            fflush(stdin);
                            delEntry(DIARY, rec);
                            break;
                        case 'n':
                            main(argc, argv);
                            break;
                        default:
                            printf("Please type a valid input!\n");
                            break;
                    }
                }
                break;
            case 4:
                printf("What is the current password? ");
                scanf("%20s", pass);
                fflush(stdin);
                if (strcmp(pass, getPass(DIARY))==0) {
                    printf("What would you like your new password to be (max 20 characters, no spaces)? ");
                    scanf("%20s", pass);
                    fflush(stdin);
                    editPass(DIARY, pass);
                    main(argc, argv);
                }
                else {
                    printf("Incorrect password.");
                    main(argc, argv);
                }
                break;
            case 5:
                printf("Come back when you have a new record!");
                exit(0);
                break;
            default:
                printf("Please input a valid choice! ");
        }
    } while (choice > 5 || choice < 1);

    free(PATH);
    free(DIARY);
    free(DIARY_T);

    return 0;
}

int createPassword(const char* fName, const char* pass) {
    FILE* temp = fopen(fName, "w");
    if (temp==NULL) {
        printf("There was an error creating the file.\n");
        exit(EXIT_FAILURE);
    }
    fprintf(temp, "%s\n\n\n", pass);
    fclose(temp);
    return 0;
}

char* getPass(const char* fName) {
    FILE* temp = fopen(fName, "r");
    if (temp==NULL) {
        printf("An error ocurred checking password.\n");
        exit(EXIT_FAILURE);
    }
    char* pass = (char*)malloc(21*sizeof(char));
    *pass = '\0';
    char ch;
    int c;
    for (c = 0; c < 21; c++) {
        ch = fgetc(temp);
        if (ch == '\n') break;
        pass[c] = ch;
    }
    fclose(temp);
    pass[c] = '\0';
    return pass;
}

int addEntry() {
    // Default location for diary.txt is %USERPROFILE%\Documents\Diary.txt
    // Creates the file if it doesn't exist, and opens it for appending
    FILE* d = fopen(DIARY, "a");
    // Checks if there was an error in opening the file
    if (d == NULL) {
        printf("Error opening file.\n");
        return 1;
    }
    // This is used to tell the date and time of when the record was created
    time_t rawtime;
    struct tm * timeinfo;
    time (&rawtime);
    timeinfo = localtime(&rawtime);
    // This is where the user actually types the entry
    // A buffer of 500 (499 chars max per line, last is reserved for \0) is used to fputs each line into the txt.
    // Date/time go first, then record number, then the actual entry, then "END" is put for the program to tell where the end is
    char buffer[501];
    fprintf(d, "@%i", getNextRecNum(DIARY));
    fputc('\n', d);
    fputs(asctime(timeinfo), d);
    printf("Type your new record (MAX 500 CHARACTERS PER LINE (end line by pressing enter)). Type \"stop\" when done.\n");
    for (;;) {
        gets(buffer);
        if (strncmp(buffer, "stop", 4)==0) {
            fputs("\nEND\n\n\n", d);
            printf("Record successfully added.\n");
            break;
        }
        fputs(buffer, d);
        fputc('\n', d);
    }
    // Successfully executed function
    fclose(d);
    return 0;
}

int viewEntry(const char* fName, int entry) {
    FILE* temp = fopen(fName, "r");
    if (temp == NULL) {
        printf("Error opening file.\n");
        return 1;
    }
    char line[501];
    char rec[3];
    sprintf(rec, "@%i\0", entry);
    short int print = 0;
    while(fgets(line, sizeof(line), temp)) {
        if (strncmp(line, "END", 3)==0) break;
        if (strncmp(line, rec, 2)==0) {
            print = 1;
            putchar('\n');
            continue;
        }
        if (print == 1) printf("%s", line);
    }
    if (print==0) {
        printf("Record was not found.\n");
        return 1;
    }
    fclose(temp);
    return 0;
}

int delEntry(const char* fName, int entry) {
    FILE* temp = fopen(fName, "r");
    FILE* temp2 = fopen(DIARY_T, "w");
    if (temp == NULL || temp2 == NULL) {
        printf("Error opening file.\n");
        return 1;
    }
    char line[501];
    char rec[3];
    short int read = 1;
    short int after = 0;
    short int count = 0;
    sprintf(rec, "@%i\0", entry);
    while(fgets(line, sizeof(line), temp)) {
        if (strncmp(line, rec, 2)==0) {
            read = 0;
            after = 1;
        }
        else if (strncmp(line, "END", 3)==0 && after == 1 && read == 0) {
            read = 1;
            count++;
            continue;
        }
        if (read == 1) {
            if (line[0] == '@' && after == 1) {
                int i = ((int)(line[1]-'0'))-1;
                sprintf(line, "@%i\n", i);
            }
            if (count > 0 && count < 3) {
                count++;
                continue;
            }
            fputs(line, temp2);
        }
    }
    if (after == 0) {
        printf("Record was not found.\n");
        return 1;
    }
    fclose(temp);
    fclose(temp2);
    remove(fName);
    rename(DIARY_T, DIARY);
    printf("Record successfully deleted.\n");
    return 0;
}

int editPass(const char* fName, const char* pass) {
    FILE* temp = fopen(fName, "r");
    FILE* temp2 = fopen(DIARY_T, "w");
    if (temp == NULL || temp2 == NULL) {
        printf("Error opening files.\n");
        return 1;
    }

    char line[501];
    short int count = 0;
    fputs(pass, temp2);
    fputs("\n\n\n", temp2);
    while (fgets(line, sizeof(line), temp)) {
        if (count < 3) {
            count++;
            continue;
        }
        fputs(line, temp2);
    }
    fclose(temp);
    fclose(temp2);
    remove(fName);
    rename(DIARY_T, DIARY);
    printf("Successfully edited password.\n");
    return 0;
}

int fileExists(const char* fName) {
    FILE* tester;
    if ((tester = fopen(fName, "r"))) {
        fclose(tester);
        return 0;
    }
    return 1;
}

int getNextRecNum(const char* fName) {
    FILE* temp = fopen(fName, "r");
    if (temp==NULL) {
        printf("Error opening file.\n");
        exit(EXIT_FAILURE);
    }
    char line[500];
    int current = 0;
    while (fgets(line, sizeof(line), temp)) {
        if (line[0] == '@') {
            char c = line[1];
            current = (int)(c-'0');
        }
    }
    fclose(temp);
    return current+1;
}

char* concatenate(const char* str1, const char* str2) {
    char* temp = (char*)malloc((strlen(str1)+strlen(str2))*sizeof(char));
    if (temp == NULL) {
        printf("Error allocating memory.\n");
        exit(EXIT_FAILURE);
    }
    *temp = '\0';
    strcat(temp, str1);
    strcat(temp, str2);
    *(temp+strlen(str1)+strlen(str2)) = '\0';
    return temp;
}
