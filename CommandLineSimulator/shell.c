#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// constants
#define RMAX 4   // max number of registers
#define MMAX 16   // max number of memory locations
#define SMAX 80   // max command line characters

// registers
int r[RMAX];

// memory locations
int m[MMAX];

// index
int i;

// cmd input string
char cmd_input[SMAX];

// cmd portion of input (1st word)
char cmd[SMAX];

// argument portion of input (2nd word)
char arg[SMAX];
int iarg;   // interger version of argument

// file pointers
FILE* fp = NULL; // file pointer for basic input/output
FILE* runfp = NULL; // file pointer for running command lines

// temporary integer
int temp;

// number words found in input
int wordsFound;

// display mode
char display_mode[4];

// program finished/unifinished flag
bool done = false;

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

int readIntFromFile(FILE* dest) {
  // read next int from file dest in decimal mode
  int result;
   
  fscanf(dest, "%d", &result);
  return(result);
}

void stringUpper(char string[]){
  // upper case a string
  int i = 0;

  while ((string[i] != '\0') && (i < SMAX)) {
    if (string[i] >= 'a' && string[i] <= 'z') {
      string[i] = string[i] - 32;
    }
    i++;
  }
}

void mode(char string[]) {
    if (strcmp(string, "DEC") == 0) {
        strcpy(display_mode, "DEC");
    } else if (strcmp(string, "HEX") == 0) {
        strcpy(display_mode, "HEX");
    } else {
        printf("INVALID MODE\n");
    }
}

void printRegister(int reg) {
    printf("[%s] ", display_mode);
    // prints value of r[reg] in correct display mode
    if (strcmp(display_mode, "DEC") == 0) {
        printf("r[%d]: ", reg);
        printf("%d\n", r[reg]);

    } else if (strcmp(display_mode, "HEX") == 0) {
        printf("r[0x%x]: ", reg);
        printf("0x%x\n", r[reg]);

    } else {
        printf("INVALID MODE\n");
    }
}

void printMemory(int mem) {
    printf("[%s] ", display_mode);
    // prints value of m[mem] in correct display mode
    if (strcmp(display_mode, "DEC") == 0) {
        printf("m[%d]: ", mem);
        printf("%d\n", m[mem]);

    } else if (strcmp(display_mode, "HEX") == 0) {
        printf("m[0x%x]: ", mem);
        printf("0x%x\n", m[mem]);

    } else {
        printf("INVALID MODE\n");
    }
}

void printRegisterToFile(int reg, FILE* dest) {
  // prints value of r[reg] in decimal mode
  fprintf(dest, "%d\n", r[reg]);
  return;
};

void printMemoryToFile(int mem, FILE* dest) {
  // prints value of m[mem] in decimal mode
  fprintf(dest, "%d\n", m[mem]);
  return;
};

void clearAllRegisters(){
    // clear all registers (reset to 0's)
    for (i = 0; i < RMAX; i++) {
      r[i] = 0;
    }
}

void clearAllMemory(){
    // clear all memory locations (reset to 0's)
    for (i = 0; i < MMAX; i++) {
      m[i] = 0;
    }
}

void add() {
    // add r[0] + r[1], puts answer into r[0]
    // moves 0->r[3]
    // moves r[3]->r[2]
    // moves r[2]->r[1]
      r[0] = r[0] + r[1];
      for (i = 1; i <= RMAX - 2; i++) {
          r[i] = r[i + 1];
      }
      r[RMAX - 1] = 0;
}

void subtract() {
    // subtracts r[1] - r[0], puts answer into r[0]
    // moves 0->r[3]
    // moves r[3]->r[2]
    // moves r[2]->r[1]
    r[0] = r[1] - r[0];
    for (i = 1; i <= RMAX - 2; i++){
        r[i] = r[i + 1];
    }
    r[RMAX - 1] = 0;
}

void multiply() {
    // multiplies r[1] * r[0], puts answer into r[0]
    // moves 0->r[3]
    // moves r[3]->r[2]
    // moves r[2]->r[1]
    r[0] = r[0] * r[1];
    for (i = 1; i <= RMAX - 2; i++){
        r[i] = r[i + 1];
    }
    r[RMAX - 1] = 0;
}

void divide() {
    // divides r[1] / r[0], puts answer into r[0]
    // moves 0->r[3]
    // moves r[3]->r[2]
    // moves r[2]->r[1]
    if (r[0] == 0) {
        printf("ERROR: Divide by 0\n");
    } else
        r[0] = r[1] / r[0];
        for (i = 1; i <= RMAX - 2; i++){
            r[i] = r[i + 1];
        }
        r[RMAX - 1] = 0;
}

void findremainder() {
    // computes the remainder of r[1] % r[0], puts answer into r[0]
    // moves 0->r[3]
    // moves r[3]->r[2]
    // moves r[2]->r[1]
    if (r[0] == 0) {
        printf("ERROR: Divide by 0\n");
    } else
        r[0] = r[1] % r[0];
        for (i = 1; i <= RMAX - 2; i++){
            r[i] = r[i + 1];
        }
        r[RMAX - 1] = 0;
}

// ============= MAIN PROGRAM ==============
int main () {
    // boolean variable too flag input from file
    bool fromFile = false;
    
    // initialize display mode to decimal
    mode("DEC");

    // initialize registers to 0
    clearAllRegisters();
    
    // initialize memory locations to 0
    clearAllMemory();
    
    printf("\nWelcome to the command shell.\n");

    while (!done) {
        // show current status and value of r[0]
        printRegister(0);
        
        // GET INPUT based on input type
        if (fromFile == true) {
            if ((fgets(cmd_input, SMAX, runfp)) != NULL) {
                printf("RUN> %s", cmd_input);
            } else {
                printf("RUN> EOF\n");
                printf("=====================\n");
                printf("RUN - end reading from file\n");
                printf("=====================\n");
                printf("NOP\n");
                printRegister(0);
                fclose(runfp);
                fromFile = false;
            }
        }
        
        if (fromFile == false) {
            // prompt for input and read input string
            printf("> ");
            fgets(cmd_input, SMAX, stdin);
        }
        
        // break the input string into cmd arg and junk
        wordsFound = sscanf(cmd_input, "%s %s %*s", cmd, arg);

        // uppercase cmd and arg to standard format
        stringUpper(cmd);

        // handle error inputs
        if (wordsFound <= 0) {
            // no command found, substitute NOP
            strcpy(cmd, "NOP");

        } else if (wordsFound <= 1) {
            // no argument found, substute empty string
            strcpy(arg, "");
        }
        
        // PROCESS INPUT
        if ((strcmp(cmd, "EXIT") == 0) || (strcmp(cmd, "QUIT") == 0)) {
            // exit program if currently reading from stdin
            if (fromFile == false) {
                // exit from shell
                printf("EXIT encountered.\n");
                done = true;
            } else {
                // close file if currently reading from runfp
                printf("EXIT encountered.\n");
                printf("=====================\n");
                printf("RUN - end reading from file\n");
                printf("=====================\n");
                printf("NOP\n");
                fclose(runfp);
                fromFile = false;
            }
        
        } else if (strcmp(cmd, "NOP") == 0) {
            // do nothing
            printf("NOP\n");
            
        } else if (strcmp(cmd, "ECHO") == 0) {
            // echo arg
            printf("%s\n", arg);
            
        } else if (strcmp(cmd, "MODE") == 0) {
            // switch display mode
            mode(arg);
        
        } else if (strcmp(cmd, "RUN") == 0) {
            // Open file fn, read multiple lines containing commands, then execute them line by line

            // execute file if currently reading from stdin
            if (fromFile == false) {
                runfp = fopen(arg, "r");

                if (runfp == NULL) {
                    printf("Error while opening file.\n");
                } else {
                    printf("=====================\n");
                    printf("RUN - starting to read from file: %s\n", arg);
                    printf("=====================\n");
                    fromFile = true; // make fromFile true to start reading lines
                }
            } else {
                // do nothing if currently reading from runfp
                printf("NOP\n");
            }
            
        } else if (strcmp(cmd, "HELP") == 0) {
            // display description of all valid commands
            printf("\nEXIT (or QUIT) --- exit from shell.\n"
                   "\nNOP --- do nothing.\n"
                   "\nECHO --- echo arg.\n"
                   "\nMODE --- switch display mode (DEC or HEX).\n"
                   "\nRUN fn --- open file fn, read multiple lines containing commands, then execute them line by line\n"
                   "\nHELP --- description of all valid commands.\n"
                   "\nLOADI arg --- load (immediate) arg into r[0].\n"
                   "\nLOADM arg --- copy m[arg] to r[0], scroll other registers up (0 <= arg <= MMAX - 1).\n"
                   "\nSTORM arg --- move r[0] to m[arg], scroll other registers down (0 <= arg <= MMAX - 1).\n"
                   "\nFSTOR fn --- write out values of registers and memory to file fn (e.g. FSTOR test.txt).\n"
                   "\nFLOAD fn --- read in values of registers and memory from file fn and save to registers and memory (e.g. FSTOR test.txt).\n"
                   "\nPRTRS --- print out the value of all the registers.\n"
                   "\nPRTMS --- print out the value of all memory locations.\n"
                   "\nCLRRS --- clear the value of all the registers.\n"
                   "\nCLRMS --- clear the value of all memory locations.\n"
                   "\nADD --- add [r0] + r[1] and save to r[0] then scroll registers down.\n"
                   "\nSUB --- subtract r[1] - r[0] and save to r[0] then scroll registers down.\n"
                   "\nMUL --- multiply [r0] * r[1] and save to r[0] then scroll registers down.\n"
                   "\nDIV --- divide [r1] / r[0] and save to r[0] then scroll registers down.\n"
                   "\nREM --- compute the remainder of [r1] %% r[0] and save to r[0] then scroll registers down.\n"
                   "\nINC --- adds one to r[0], puts answer into r[0]\n"
                   "\nDEC --- subtracts one from r[0], puts answer into r[0]\n"
                   "\nNEG --- multiplies r[0] by -1, puts answer into r[0]\n");

        } else if (strcmp(cmd, "LOADI") == 0) {
            // load (immediate) arg into r[0]
            // convert arg to integer - make sure it is valid
            iarg = readIntFromString(arg);

            // push the other registers up
            for (i = RMAX - 1; i > 0; i--) {
              r[i] = r[i - 1];
            }

            // load arg into r[0]
            r[0] = iarg;
            
        } else if (strcmp(cmd, "LOADM") == 0) {
            // copy m[arg] to r[0], scroll other registers up (0 <= arg <= MMAX - 1)
            
            // convert arg to integer - make sure it is valid
            iarg = readIntFromString(arg);

            if ((iarg > MMAX - 1) || (iarg < 0)) printf("Error: memory location does not exist.\n");
            
            else {
                // push the other memory locations UP
                for (i = RMAX - 1; i > 0; i--) {
                  r[i] = r[i - 1];
                }
                
                // load m[arg] into r[0]
                r[0] = m[iarg];
            }

        } else if (strcmp(cmd, "STORM") == 0) {
            // move r[0] to m[arg], scroll other registers down (0 <= arg <= MMAX - 1)
            
            // convert arg to integer - make sure it is valid
            iarg = readIntFromString(arg);
            
            if ((iarg > MMAX - 1) || (iarg < 0)) printf("Error: memory location does not exist.\n");
            
            else {
                //  move r[0] to m[arg]
                m[iarg] = r[0];
                
                // push the other memory locations DOWN
                for (i = 0; i <= RMAX - 2; i++) {
                  r[i] = r[i + 1];
                }
                
                // load 0 into r[RMAX - 1]
                r[RMAX - 1] = 0;
            }
        
        } else if (strcmp(cmd, "FSTOR") == 0) {
            // write out values of registers and memory to file fn
            fp = fopen(arg, "w");
            
            if (fp == NULL) {
                printf("Error while opening file.\n");
            } else {
                for (i = 0; i < RMAX; i++) {
                    printRegisterToFile(i, fp);
                }
                for (i = 0; i < MMAX; i++) {
                    printMemoryToFile(i, fp);
                }
                fclose(fp);
                printf("Data successfully stored to file: %s\n",arg);
            }
            
        } else if (strcmp(cmd, "FLOAD") == 0) {
            // read in values of registers and memory from file fn and save to registers and memory
            fp = fopen(arg, "r");

            if (fp == NULL) {
                printf("Error while opening file.\n");
            } else {
                for (i = 0; i < RMAX; i++) {
                    temp = readIntFromFile(fp); // read next integer from file
                    if (feof(fp) == 0) {
                        r[i] = temp;  // assign only if not eof
                    } else {
                        printf("End of File encountered at register %d\n", i);
                    }
                }

                for (i = 0; i < MMAX; i++) {
                    temp = readIntFromFile(fp); // read next integer from file
                    if (feof(fp) == 0) {
                        m[i] = temp;  // assign only if not eof
                    } else {
                        printf("End of File encountered at memory %d\n", i);
                    }
                }
                fclose(fp);
                printf("Data successfully read from file: %s\n", arg);
            }

        } else if (strcmp(cmd, "PRTRS") == 0) {
            // print out the value of all the registers

            printf("--------------------\n");

            for (i = RMAX - 1; i >= 0; i--) {
                printRegister(i);
            }

            printf("--------------------\n");
        
        } else if (strcmp(cmd, "PRTMS") == 0) {
            // print out the value of all memory locations

            printf("--------------------\n");

            for (i = MMAX - 1; i >= 0; i--) {
                printMemory(i);
            }

            printf("--------------------\n");

        } else if (strcmp(cmd, "CLRRS") == 0) {
            // clear all registers
            clearAllRegisters();
            
        } else if (strcmp(cmd, "CLRMS") == 0) {
            // clear all memory locations
            clearAllMemory();
        
        } else if (strcmp(cmd, "ADD") == 0) {
            // add r[0] and [r1] and save to r[0] then scroll registers down
            add();

        } else if (strcmp(cmd, "SUB") == 0) {
            // subtract r[1] - r[0] and save to r[0] then scroll registers down
            subtract();

        } else if (strcmp(cmd, "MUL") == 0) {
            // multiply [r0] * r[1] and save to r[0] then scroll registers down
            multiply();

        } else if (strcmp(cmd, "DIV") == 0) {
            // divide [r1] / r[0] and save to r[0] then scroll registers down
            divide();
            
        } else if (strcmp(cmd, "REM") == 0) {
            // compute the remainder of [r1] % r[0] and save to r[0] then scroll registers down
            findremainder();
        
        } else if (strcmp(cmd, "INC") == 0) {
            // adds one to r[0], puts answer into r[0] - other registers are unchanged
            r[0] += 1;
            
        } else if (strcmp(cmd, "DEC") == 0) {
            // subtracts one from r[0], puts answer into r[0] - other registers are unchanged
            r[0] -= 1;
        
        } else if (strcmp(cmd, "NEG") == 0) {
            // multiplies r[0] by -1, puts answer into r[0] - other registers are unchanged
            r[0] *= -1;

        } else {
          // Error on Input
          printf(">> Unknown Command |%s|%s|\n", cmd, arg);
        }

    }
    // if done = true, end program
    printf("\nEnd Program.\n\n");
    return(0);
}
