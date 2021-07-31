#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
// Getenv gets a "property" from the system environment variables. User home would be:
// *Whatever drive windows is on, lets say C drive* C:\*USER*\ 
// I made my own concatenate function because strcat appends an existing string, but I needed to store it into a new string entirely
#define USERHOME getenv("USERPROFILE")
#define PATH concatenate(USERHOME, "\\Documents") // PATH should be the path to, but not including, the file (if you change the path, keep this in mind)
#define DIARY concatenate(PATH, "\\Diary.txt") // File's path. This is where you can rename the file.
#define DIARY_T concatenate(PATH, "\\Diary_T.txt") // This is a temporary file that is used in some functions, but is never permanently on your pc. It doesn't have to be in the same directory, but it is perferred that it is
// All the functions, you know the drill
int createPassword(const char* fName, const char* pass);
char* getPass(const char* fName);
int addEntry();
int viewEntry(const char* fName, int entry);
int delEntry(const char* fName, int entry);
int editPass(const char* fName, const char* pass);
int fileExists(const char* fName);
int getNextRecNum(const char* fName);
char* concatenate(const char* str1, const char* str2);
// Just a boolean that checks if the program has been opened
short int a = 0;

int main(int argc, char* argv[]) {
    // REMOVE THIS IF STATEMENT IF YOU CHANGE THE PATH (or edit it depending on what you do)
    // (default) If the USERHOME isn't set or isn't found then the program can't continue
    if (USERHOME == NULL) {
        printf("User home not found, program cannot continue.");
        exit(EXIT_FAILURE);
    }
    // Checks for first time running 
    short int choice;
    char pass[21];
    // If it's the first time running the program, the program prompts the user to create a password
    // That password is used for every run after that
    // Remember that silly boolean above the main function? This is where it comes in
    // It stops the password prompt from running every time you go back to the main menu
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
    // Main menu
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
    // Do while loop to catch invalid inputs by the user
    // The input stream is flushed constantly because the newline character would get used as input each loop
    // I used switch cases for the main menu because it felt better than an if statement for this application
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
    // Frees each of the paths because the concatenate function mallocates a string and returns it
    // IF YOU CHANGE THE PATHS AND USE THE CONCATENATE FUNCTION, MAKE SURE TO FREE THEM HERE (if you changed the names)
    free(PATH);
    free(DIARY);
    free(DIARY_T);
    // Success!
    return 0;
}
// Creates the password, hence the name "createPassword"
// In each of the functions it checks if the files had problems opening or not and either returns 1 or exits
// (returns 1 if the function is not critical to the program's runability)
// (exits if the function was required for the program to continue)
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
// Gets the password from the file
char* getPass(const char* fName) {
    FILE* temp = fopen(fName, "r");
    if (temp==NULL) {
        printf("An error ocurred checking password.\n");
        exit(EXIT_FAILURE);
    }
    // fgetc was used here to crop off the newline character
    // Using fgets is much faster and easier, but carries the newline character, messing up comparison
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
    // A buffer of 501 (500 chars max per line, last is reserved for \0) is used to fputs each line into the txt.
    // Record number goes first, then time/date, then the actual entry, then "END" is put for the program to tell where the end is
    // Looks like:
    /*
    *   @1\n
    *   DATETIME (ex: Fri 1/1/2000 12:00:00) (or something like that)\n\n
    *   
    *   *user typed stuff*
    *   Secret word is ".--. .--."\n\n
    *   Epstein didn't kill himself
    *   *more user typed stuff*
    *   
    *   END\n\n\n
    */
    char buffer[501];
    fprintf(d, "@%i", getNextRecNum(DIARY));
    fputc('\n', d);
    fputs(asctime(timeinfo), d);
    printf("Type your new record (MAX 500 CHARACTERS PER LINE (end line by pressing enter)). Type \"stop\" when done.\n");
    for (;;) {
        gets(buffer);
        // Strncmp was the fasted way to compare the strings without comparing the newline character at the end
        // SYNTAX: strncmp(string, string, number_of_letters_to_compare)
        // strncmp(buffer, "stop", 4) compares buffer with the first 4 letters of stop (or vice versa)
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
// Function to view an entry/record... I shouldn't have to explain, this it's in the name
int viewEntry(const char* fName, int entry) {
    FILE* temp = fopen(fName, "r");
    if (temp == NULL) {
        printf("Error opening file.\n");
        return 1;
    }
    char line[501];
    char rec[3];
    sprintf(rec, "@%i\0", entry);
    // Boolean on whether it should print to the console or not (becomes 1 when it's time to print, aka when it reaches the record number)
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
// Function to... delete an entry... why are you reading this
int delEntry(const char* fName, int entry) {
    FILE* temp = fopen(fName, "r");
    FILE* temp2 = fopen(DIARY_T, "w");
    if (temp == NULL || temp2 == NULL) {
        printf("Error opening file.\n");
        return 1;
    }
    // A lot of this may be unnecessary, but it was the only way i figured out
    // It could be a lot more efficient
    char line[501];
    char rec[3];
    // Same functionality as the read boolean in the previous function
    short int read = 1;
    // Boolean for if the program is reading after the record
    short int after = 0;
    // This is just used for cleaning up the file a bit
    // Without it, there would be some unnecessary newlines in the .txt, which was ugly
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
                // I didn't come up with this myself, but it's really clever
                // To convert a character integer to just an integer (ex: '1' to 1)
                // You can subtract '0' from the character and cast it to an int (it implicitly gets casted but just for clarification)
                // Ex: '0' = 48 (ASCII) Trying to convert '1' (49) to 1
                // (int) ('1' (49) - '0' (48)) = 1
                // This works because the compiler subtracts the ASCII equivalent of the characters and the result is an int
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
    // If the record was never found, after is never set to 1, meaning it doesn't exist
    if (after == 0) {
        printf("Record was not found.\n");
        return 1;
    }
    fclose(temp);
    fclose(temp2);
    // This writes over the file
    // The way this works is that the program creates a temporary file (DIARY_T) and basically copys everything in the original file to the temporary one.
    // When it gets to the record that wants to be deleted, however, it just stops copying, but still reads and continues through the file
    remove(fName);
    rename(DIARY_T, DIARY);
    printf("Record successfully deleted.\n");
    return 0;
}
// I'm not even going to say anything
int editPass(const char* fName, const char* pass) {
    FILE* temp = fopen(fName, "r");
    FILE* temp2 = fopen(DIARY_T, "w");
    if (temp == NULL || temp2 == NULL) {
        printf("Error opening files.\n");
        return 1;
    }
    // This uses the same method as the delete function and writes the new password where the old one should've been
    // (I couldn't figure out how to delete a single line another way lmao, backspace (8 in ASCII) wouldn't work)
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
    // Same thing as the delete function
    remove(fName);
    rename(DIARY_T, DIARY);
    printf("Successfully edited password.\n");
    return 0;
}
// Hmmm idk what this is
// There's probably a function in one of the libraries but this is what I chose for no reason
int fileExists(const char* fName) {
    FILE* tester;
    if ((tester = fopen(fName, "r"))) {
        fclose(tester);
        return 0;
    }
    return 1;
}
// Gets what record number should be next
// (if the previous one was 1, then this would return 2)
int getNextRecNum(const char* fName) {
    FILE* temp = fopen(fName, "r");
    if (temp==NULL) {
        printf("Error opening file.\n");
        exit(EXIT_FAILURE);
    }
    char line[501];
    int current = 0;
    while (fgets(line, sizeof(line), temp)) if (line[0] == '@') current = (int)(line[1]-'0');
    fclose(temp);
    return current+1;
}
// HMMMMMMMMMMMM
// This was really simple to make... with the help of strcat
char* concatenate(const char* str1, const char* str2) {
    // Mallocating the length of str1 + str1 * the size of a char bytes into a string
    char* temp = (char*)malloc((strlen(str1)+strlen(str2))*sizeof(char));
    if (temp == NULL) {
        printf("Error allocating memory.\n");
        exit(EXIT_FAILURE);
    }
    // The first index is set to \0 due to the way strcat works (basically looks for the \0 first)
    // Without this, the string would have a trail of clutter (random ASCII characters) before it
    *temp = '\0';
    strcat(temp, str1);
    strcat(temp, str2);
    // Making sure the last index is \0, don't want any memory leaks here
    *(temp+strlen(str1)+strlen(str2)) = '\0';
    return temp;
}
// As you can see, everything here is very sloppy and could be refactored a bunch
// But, I'm lazy
// Let's see if you read all the comments, what was the secret word ;)
