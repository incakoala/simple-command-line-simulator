#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

// constants
#define rmax 4   // max number of registers
#define smax 80   // max command line characters

// registers
int r[rmax];

// index
int i;

// cmd input string
char cmd_input[smax];

// cmd portion of input (1st word)
char cmd[smax];

// argument portion of input (2nd word)
char arg[smax];
int iarg;   // interger version of argument

// number words found in input
int wordsFound;

// display mode
char display_mode[4];

// program finished/unifinished flag
bool done = false;


void printRegister(int reg) {
    // prints value of r[reg] in correct display mode
    if (strcmp(display_mode, "DEC") == 0) {
        printf("[%s] ", display_mode);
        printf("r[%d]: ", reg);
        printf("%d\n", r[reg]);

    } else if (strcmp(display_mode, "HEX") == 0) {
        printf("[%s] ", display_mode);
        printf("r[0x%x]: ", reg);
        printf("0x%x\n", r[reg]);

    } else {
        printf("INVALID MODE\n");
    }
}

int readIntFromString(char* source){
    // reads value of integer from source string in correct display mode
    int intsFound;
    int number;

    if (strcmp(display_mode, "DEC") == 0) {
        intsFound = sscanf(source, "%d", &number);
        if (intsFound != 1) {
            printf("INVALID NUMBER - used 0 instead\n");
            number = 0;
        }
        
    } else if (strcmp(display_mode, "HEX") == 0) {
        intsFound = sscanf(source, "%x", &number);
        if (intsFound != 1) {
            printf("INVALID NUMBER - used 0 instead\n");
            number = 0;
        }
        
    } else {
        printf("INVALID MODE - used 0 instead\n");
        number = 0;
    }
    
    return(number);
}
    
void stringUpper(char string[]){
  // upper case the argument string
  int i = 0;

  while ((string[i] != '\0') && (i < smax)) {
    if (string[i] >= 'a' && string[i] <= 'z') {
      string[i] = string[i] - 32;
    }
    i++;
  }
}

void clearAll(){
    // clear all registers (reset to 0's)
    for (i = 0; i < rmax; i++) {
      r[i] = 0;
    }
}

void add() {
    // save value of r[0] and r[1] to r[0] then scroll registers down
      r[0] = r[0] + r[1];
      for (i = 1; i <= rmax - 2; i++) {
          r[i] = r[i+1];
      }
      r[rmax - 1] = 0;
}

void subtract() {
    // subtract [r0] from r[1] and save to r[0] then scroll registers down
    r[0] = r[1] - r[0];
    for (i = 2; i <= rmax - 1; i++) {
        r[i-1] = r[i];
    }
    r[rmax - 1] = 0;
}

void multiply() {
    // multiply [r0] by r[1] and save to r[0] then scroll registers down
    r[0] = r[0] * r[1];
    for (i = 2; i <= rmax - 1; i++) {
        r[i-1] = r[i];
    }
    r[rmax - 1] = 0;
}

void mode(char string[]) {
    if (strcmp(string, "DEC") == 0) {
        strcpy(display_mode, "DEC");
    } else if (strcmp(string, "HEX") == 0) {
        strcpy(display_mode, "HEX");
    }
}

void help() {
    // display description of all valid commands
    printf("\nMODE --- switch display mode (DEC or HEX)\n"
           "\nEXIT --- exit from shell\n"
           "\nQUIT --- exit from shell\n"
           "\nLOADI --- load (immediate) arg into r[0]\n"
           "\nPRTRS --- print out the value of all the registers\n"
           "\nECHO --- echo arg\n"
           "\nCLRRS --- clear the value of all the registers\n"
           "\nADD --- save value of r[0] and r[1] to r[0] then scroll registers down\n"
           "\nSUB --- subtract [r0] from r[1] and save to r[0] then scroll registers down"
           "\nHELP --- description of all valid commands\n"
           "\nNOP --- do nothing\n");
}

// ============= MAIN PROGRAM ==============

int main () {
    // initialize display mode to decimal
    mode("DEC");

    // initialize registers to 0
    for (i = 0; i < rmax; i++) {
        r[i] = 0;
    }

    printf("\nWelcome to the command shell.\n");

    while (!done) {
        // GET INPUT
        // show current status and value of r[0]

        printRegister(0);

        //prompt for input and read input string
        printf("> ");
        fgets(cmd_input, smax, stdin);

        // break the input string into cmd arg and junk
        wordsFound = sscanf(cmd_input, "%s %s %*s", cmd, arg);

        // uppercase cmd and arg to standard format
        stringUpper(cmd);
        stringUpper(arg);

        // handle error inputs
        if (wordsFound <= 0) {
            // no command found, substitute NOP
            strcpy(cmd, "NOP");
            
        } else if (wordsFound <= 1) {
            // no argument found, substute empty string
            strcpy(arg, "");
        }

        // PROCESS INPUT
        if (strcmp(cmd, "MODE") == 0) {
            // switch display mode
            mode(arg);
            
        } else if ((strcmp(cmd, "EXIT") == 0) || (strcmp(cmd, "QUIT") == 0)) {
            // exit from shell
            printf("EXIT encountered.\n");
            done = true;

        } else if (strcmp(cmd, "NOP") == 0) {
            // do nothing
            printf("NOP\n");
          
        } else if (strcmp(cmd, "LOADI") == 0) {
            // load (immediate) arg into r[0]
            // convert arg to integer - make sure it is valid

            iarg = readIntFromString(arg);

            // push the other registers up
            for (i = rmax-1; i > 0; i--) {
              r[i] = r[i-1];
            }

            // load arg into r[0]
            r[0] = iarg;

        } else if (strcmp(cmd, "PRTRS") == 0) {
            // print out the value of all the registers

            printf("--------------------\n");

            for (i=rmax-1; i>=0; i--) {
                printRegister(i);
            }

            printf("--------------------\n");

        } else if (strcmp(cmd, "ECHO") == 0) {
            // echo arg
            printf("%s\n", arg);
            
        } else if (strcmp(cmd, "CLRRS") == 0) {
            // clear all registers
            clearAll();
          
        } else if (strcmp(cmd, "ADD") == 0) {
            // add r[0] and [r1] and save to r[0] then scroll registers down
            add();
            
        } else if (strcmp(cmd, "SUB") == 0) {
            // subtract [r0] from r[1] and save to r[0] then scroll registers down
            subtract();
            
        } else if (strcmp(cmd, "MUL") == 0) {
            // multiply [r0] by r[1] and save to r[0] then scroll registers down
            multiply();
                
        } else if (strcmp(cmd, "HELP") == 0) {
            // display description of all commands
            help();
          
        } else {
          // Error on Input
          printf(">> Unknown Command |%s|%s|\n", cmd, arg);
        }

    }
    // if done = true, end program
    printf("\nEnd Program.\n\n");
    return(0);
}
