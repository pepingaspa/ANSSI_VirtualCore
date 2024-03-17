/*
 * GROUPE : Alan Dabrowski (dabrowskia@cy-tech.fr) - Gaspard Pepin (pepingaspa@cy-tech.fr)
 * FICHIER : COEUR.C
 * COMPILATION : gcc -o coeur coeur.c -lm
 * EXECUTION : ./coeur <fichier.bin> <State.txt> <verbose (0/1)>
 */

//INCLUDE
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <math.h> 
#include <stdint.h>
#include <inttypes.h> 
#include <limits.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

//GLOBAL VARIABLES
//Tab of flags
int flag[8] = {0,0,0,0,0,0,0,1}; /*0 : equals; 1 : different; 2 : lower or equal; 3 : lower; 4 : greater or equal; 5 : greater; 6 : carry+; 7 : carry-*/

//Tab of registers
uint64_t registres[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; 

//Boolean for verbose
bool verbose = false;

/*
 * PROCEDURE : PRINTREGISTER
 * PARAMETERS : char* titre
 * NO RETURN TYPE
 * GOAL : Prints the value of all registers
 */
void printRegister(char* titre){
    printf("----------\n");
    printf("%s", titre);
    printf("\n----------\n");
    for(int i=0; i<16; i++){
        printf("R%d = %"PRIx64"\n", i, registres[i]);
    }
    printf("----------\n");
}

/*
 * PROCEDURE : SETTINGREGISTER
 * PARAMETRES : [filename - char* - name of the state file to open]
 * NO RETURN TYPE
 * GOAL : Openning the state file - Setting up registers
 */
void settingRegister(char* filename){
    FILE *file; /* File descriptor for state file */
    char line[100];
    int index; 
    uint64_t value;
    file = fopen(filename, "r");
    /* Checks that opening the file is possible*/
    if (file == NULL) {
        printf("Error: could not open file %s\n", filename);
    }
    /* Reading file*/
    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "R%d=0x%llx", &index, &value);
        registres[index] = value;
    }
    printRegister("INIT STATE");
    fclose(file);

}

/*
 * FUNCTION : LECTURE
 * PARAMETERS : [f2r - char* - the file to be read] - [buffer - unsigned char** - buffer receiving the data]
 * RETURN TYPE : INT
 * GOAL : Opens the file - Stocks each line a buffer - Returns total size
 */
int lecture(char* f2r, unsigned char** buffer) {

    //File opening in readbyte
    FILE * file;
    file = fopen(f2r,"rb");

    if(NULL == file){
        printf("ERROR : \nBin file not found.\nPlease make shure you give the good name.");
        exit(-1);
    }

    //Size set
    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    fseek(file, 0, SEEK_SET);

    //File reading
    *buffer = malloc(size*sizeof(unsigned char));
    fread(*buffer, size*sizeof(unsigned char), 1, file);

    //File closing
    fclose(file);

    //Function return
    return(size);

}

/*
 * PROCEDURE : EXECUTE
 * PARAMETRES : [instruction - int* - the instruction to be executed along with the parameters]
 * NO RETURN TYPE
 * GOAL : Find the corresponding operation - Realise the instruction
 */
void execute(int* instruction) {

    //Set of variables for the opcode, operande1, operande2 and immediate value
    int OPcode = instruction[0];
    uint64_t ope1 = registres[instruction[1]];
    uint64_t ope2 = 0;  
    int IV = instruction[4];
    int IVflag = instruction[5];

    //dest <=> registres[instruction[3]]
    //ope2 is set to registres[ope2] if IVFlag is set to 0. Else IV
    if (IVflag==1) {ope2 = IV;}
    else {ope2 = registres[instruction[2]];}

    if(verbose){
        printf("execute : carry+ = %d, carry- = %d, ",flag[6],flag[7]);
        for(int i=0; i<16; i++){printf("R%d = %"PRIx64", ", i, registres[i]);}
        printf("flag= = %d, flag!= = %d, flag<= = %d, flag< = %d, flag>= = %d, flag> = %d\n\n",flag[0],flag[1],flag[2],flag[3],flag[4],flag[5],flag[6]);
    }

    //Matches operation
    switch (OPcode) {

        //AND
        case 0:
            registres[instruction[3]] = ope1 && ope2;
            break;

        //ORR
        case 1:
            registres[instruction[3]] = ope1 || ope2;
            break;

        //EOR
        case 2:
            registres[instruction[3]] = (ope1 || ope2) && !(ope1 && ope2);
            break;
        
        //ADD
        case 3:
            registres[instruction[3]] = ope1 + ope2;
            if(ope2 > ULLONG_MAX - ope1) {flag[6] = 1;}
            break;
        
        //ADC
        case 4:
            registres[instruction[3]] = ope1 + ope2 + flag[6];
            if((ope2 > ULLONG_MAX - ope1 - flag[6]) || (ope1 == ULLONG_MAX && flag[6] == 1)) {flag[6] = 1;} else {flag[6] = 0;}
            break;
        
        //CMP
        case 5:
            if (ope1 < ope2) {
                flag[1]++;flag[2]++;flag[3]++;
            } else if (ope1 > ope2) {
                flag[1]++;flag[4]++;flag[5]++;
            } else {
                flag[0]++;flag[2]++;flag[4]++;
            }
            break;
        
        //SUB
        case 6:
            registres[instruction[3]] = ope1 - ope2;
            if(ope1 < ope2) {flag[7] = 0;}
            break;
        
        //SBC
        case 7:
            registres[instruction[3]] = ope1 - ope2 + flag[7] - 1;
            if(ope1 + flag[7] < ope2 + 1) {flag[7] = 0;} else {flag[7] = 1;}
            break;
        
        //MOV
        case 8:
            registres[instruction[3]] = ope2;
            break;
        
        //LSH
        case 9:
            registres[instruction[3]] = ope1 << ope2;
            break;
        
        //RSH
        case 10:
            registres[instruction[3]] = ope1 >> ope2;
            break;
    }
}

/*
 * PROCEDURE : DECODE
 * PARAMETRES : [instruction - unsigned char* - the instruction to be execute with its parameters] - [res - int* - pointer to stock the decoded instruction]
 * NO RETURN TYPE
 * GOAL : Decode given instruction - Stock each decoded part in res tab with format : [OPcode,ope1,ope2,dest,IV,IVflag]
 */
void decode(unsigned char* instruction,int* res) {
    res[0] = instruction[1]/16;     //OPcode
    res[1] = instruction[1]%16;     //First operande
    res[2] = instruction[2]/16;     //Second operande
    res[3] = instruction[2]%16;     //Destination
    res[4] = instruction[3];        //Immediate value
    res[5] = instruction[0];        //Flag of immediate value
    if(verbose){printf("decode : OPcode = %x, ope1 = %x, ope2 = %x, dest = %x, IV = %x, IVflag = %d\n",res[0],res[1],res[2],res[3],res[4],res[5]);}
}

/*
 * FUNCTION : FETCH
 * PARAMETERS : [PC - int - actual program counter] - [instructions - unsigned char* - operation given that must be realised]
 * RETURN TYPE : INT
 * GOAL : Read instruction (do branch or decode and execute other instruction) - return updated program counter PC
 */
int fetch(int PC, unsigned char* instructions) {

    //Tab of value for branch condition code BCC
    int BCCond[15] = {0,0,0,0,0,0,0,0,0,0,1,2,4,3,5};
    //BRANCH CONDITION CODE
    if (instructions[PC*4]>1) {
        //Set of variables
        int BCC = instructions[PC*4]/16;
        int negative = (instructions[PC*4]-BCC*16)/8;
        int offset = (instructions[PC*4]-BCC*16-negative*8)*pow(2,24)+ instructions[PC*4+1]*pow(2,16)+instructions[PC*4+2]*pow(2,8)+instructions[PC*4+3];
        
        //Conditionnal jump - In case the condition is not verified - Resets the flags
        if (BCC != 8) {
            if (flag[(BCCond[BCC])] != 1) {
                for(int i=0;i<6;i++) {flag[i] = 0;}
                if(verbose){printf("Fetch : BCC = %x, offset = %d, PC = %x\n",BCC,offset,PC+1);}
                return(PC+1);
            }
        }

        //Resets the flags
        for(int i=0;i<5;i++) {flag[i] = 0;}

        if(verbose){printf("fetch : BCC = %x, offset = %d, PC = %x\n",BCC,offset,PC + pow(-1,negative)*offset);}
        //Return new value of PC
        return(PC + pow(-1,negative)*offset);
    }

    //ALL OTHER INSTRUCTIONS (that need to be decoded and executed)
    else {
        //Set of variables
        unsigned char instruction[4] = {instructions[PC*4],instructions[PC*4+1],instructions[PC*4+2],instructions[PC*4+3]};
        if(verbose){printf("fetch : instruction = %"PRIx64" %"PRIx64" %"PRIx64" %"PRIx64", PC = %x\n",instruction[0],instruction[1],instruction[2],instruction[3],PC+1);}
        int* decoded;
        decoded = malloc(6*sizeof(int));

        //Decode the new instruction into decoded
        decode(instruction,decoded);

        //Execution of decoded instruction
        execute(decoded);

        //Return next value of PC
        return(PC+1);
    }
}

/*
 * FUNCTION : MAIN
 * PARAMETERS : [argv - int - number of arguments given] - [argc[] - char* - tab of arguments]
 * RETURN TYPE : INT
 * GOAL : Main function of program - Read given file - Parse all instructions - Fetch each one - Print registers values - Manage program counter
 */
int main(int argc, char* argv[]) {

    //Parameters verification
    if(argc < 3 || argc > 4){
        printf("ERROR : \nWrong number of arguments. Please execute as follow : \n");
        printf("./coeur <file.bin> <initState.txt> <verbose (Null, 0 for inactive / 1 for active)>");
        return(-1);
    }
    
    //argv[1] - bin name
    //argv[2] - state name
    //argv[3] - verbose

    //Verbose mode
    printf("----------\n");
    if(argc == 4){
        int var = strtol(argv[3], NULL, 10);
        if(var == 1){
            printf("Verbose mode activated.\n");
            verbose = true;
        }else{
            printf("Verbose mode not activated.\n");
        }
    }else{
        printf("Verbose mode not activated.\n");
    }
    printf("----------\n");

    //Set of values
    unsigned char* instructions;
    int size;
    int PC = 0;
    int count = 0;
    char* filename = argv[1];

    settingRegister(argv[2]);

    //File reading - instructions are in a tab of bytes
    size = lecture(filename, &instructions);


    //While loop depending on the program counter - Analyse each instruction indicated by the program counter
    while(PC*4<size) {
        PC = fetch(PC,instructions);
        count ++;
        //If PC negative, gets off the loop
        if (PC<0) {PC = size;} 
    }

    //If jump is out of file, returns an error
    if (PC*4>size) {
        printf("ERROR : \nJump was too far. Ending program with error code (-1).\n");
        free(instructions);
        return(-1);
    }
    

    //Display of registers 
    printRegister("FINAL STATE");

    //Empty instructions asm
    free(instructions);

    //Return a good end
    return(0);
}