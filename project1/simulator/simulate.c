/* LC-2K Instruction-level simulator */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000 
typedef struct stateStruct {
    int pc;
    int mem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory;
} stateType;

/*Utility Function*/
void printState(stateType *);
int convertNum(int);
void InitRegis(stateType *);

/*Check for error*/
void invalidRegis(int);
void invalidOffset();
void invalidOpcode();
void invalidPC();


/*Compute instructions*/
void readAndParse(stateType*, int*, int*, int*, int*);
int DecisionType(int*);
void Rformat(stateType*, int, int, int, int);
void Iformat(stateType*, int, int, int, int);
void Jformat(stateType*, int, int, int, int);

int main(int argc, char *argv[])
{   
    // Variables
    char line[MAXLINELENGTH];
    stateType state;
    FILE *filePtr;
    int opcode, arg0, arg1, arg2; 
    int type;
    int exCnt = 0; // count for instruction executed

    if (argc != 2) {
        printf("error: usage: %s <machine-code file>\n", argv[0]);
        exit(1);
    }

    filePtr = fopen(argv[1], "r");
    if (filePtr == NULL) {
        printf("error: can't open file %s", argv[1]);
        perror("fopen");
        exit(1);
    }

    /* read in the entire machine-code file into memory */
    for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL;
            state.numMemory++) {

        if (sscanf(line, "%d", state.mem+state.numMemory) != 1) {
            printf("error in reading address %d\n", state.numMemory);
            exit(1);
        }
        printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
    }

    // initiate register
    InitRegis(&state); 
    // Print state
    printState(&state);

	while(1){
        // Read machine code and parse it
        readAndParse(&state, &opcode, &arg0, &arg1, &arg2);
        type = DecisionType(&opcode);

        state.pc++;
        exCnt++;

        // Check for PC range
        if (state.pc < 0 || state.pc >= NUMMEMORY) {
            invalidPC();
        }

        // R-format
        if (type == 1) { Rformat(&state, opcode, arg0, arg1, arg2);}
        
        // I-format
        else if (type == 2) { Iformat(&state, opcode, arg0, arg1, arg2);}

        // J-format
        else if (type == 3 ) {Jformat(&state, opcode, arg0, arg1, arg2);}

        // O-format (halt)
        else if (type == 4) { break; }

        // O-format (noop)
        else if (type == 5) { continue; }

        // Print state after each stage
        printState(&state);
    }

    printf("%s\n", "machine halted");
    printf("%s %d %s\n", "total of", exCnt, "instructions executed");
    printf("%s\n", "final state of machine:");

    // Print state after each stage
    printState(&state);
    

    return(0);
}

void printState(stateType *statePtr)
{
    int i;
    printf("\n@@@\nstate:\n");
    printf("\tpc %d\n", statePtr->pc);
    printf("\tmemory:\n");
    for (i = 0; i < statePtr->numMemory; i++) {
        printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);
    }
    printf("\tregisters:\n");
    for (i = 0; i < NUMREGS; i++) {
        printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
    }
    printf("end state\n");
}

int convertNum(int num)
{
	/* convert a 16-bit number into a 32-bit Linux integer */
	if (num & (1 << 15)) {
		num -= (1 << 16);
	}
	return (num);
}

void InitRegis(stateType *statePtr){
    for (int i = 0; i < NUMREGS; i++) {
            statePtr->reg[i]=0;
        }
}

void readAndParse(stateType* state, int* opcode, int* arg0, int* arg1, int* arg2){
    // Get instruction from pc
    int instr = state->mem[state->pc];

    *opcode = (instr >> 22) & 0b111; // 3 bit
    *arg0 = (instr >> 19) & 0b111; // 3 bit
    *arg1 = (instr >> 16) & 0b111; // 3 bit
    *arg2 = (instr) & 0xFFFF; // 16 bit
}

int DecisionType(int* opcode){
    switch(*opcode){
    // R-format = return 1
    case 0: 
    case 1:
        return 1;
        break;
    // I-format = return 2
    case 2:
    case 3:
    case 4:
        return 2;
        break;
    // J-format = return 3
    case 5:
        return 3;
        break;
    // O-format (halt) = return 4
    case 6:
        return 4;
        break;
    // O-format (noop) = return 5
    case 7:
        return 5;
        break;
    // Else = unvalid opcode
    default:
        invalidOpcode();
        break;   
    }
}

// Interprete instruction
// R-format
void Rformat(stateType* state, int opcode, int arg0, int arg1, int arg2) {
    // Check if the register is valid
    invalidRegis(arg0);
    invalidRegis(arg1);
    invalidRegis(arg2);

    if (opcode == 0) { // If instruction is add
        state->reg[arg2] = state->reg[arg0] + state->reg[arg1];
    }
    else if (opcode ==1) { // If instruction is nor
        state->reg[arg2] = ~(state->reg[arg0] | state->reg[arg1]);
    }
    else {
        invalidOpcode();
    }
}
// I-format
void Iformat(stateType* state, int opcode, int arg0, int arg1, int arg2){
    // Check if the register is valid
    invalidRegis(arg0);
    invalidRegis(arg1);

    arg2 = convertNum(arg2);
    // Check if the register is valid
    invalidOffset(arg2);

    if (opcode == 2) { // If instruction is lw
        int offset = state->reg[arg0] + arg2;
        state->reg[arg1] = state->mem[offset];
    }
    else if (opcode == 3) { // If instruction is sw
        int offset = state->reg[arg0] + arg2;
        state->mem[offset] = state->reg[arg1];
    }
    else if (opcode == 4) { // If instruction is beq
        if (state->reg[arg0] == state->reg[arg1]){
            state->pc += arg2;
        }
    }
    else {
        invalidOpcode();
    }
}
void Jformat(stateType* state, int opcode, int arg0, int arg1, int arg2){
    // Check if the register is valid
    invalidRegis(arg0);
    invalidRegis(arg1);

    if (opcode == 5) { // If instruction is jalr
        state->reg[arg1] = state->pc; // Store PC+1 in reg[arg1]
        state->pc = state->reg[arg0];
    }
    else {
        invalidOpcode();
    }
}

/*Check for error*/
void invalidRegis(int reg) {
    if((reg < 0 )||(reg > NUMREGS)){
        printf("Register is not valid");
        exit(1);
    }
}
void invalidOffset(int offset){
    if((offset > 32767) || (offset < -32768)){
        printf("Offset is not valid");
        exit(1);
    }
}
void invalidOpcode(){
    printf("Unvalid Opcode");
    exit(1);
}
void invalidPC(){
    printf("out of range");
    exit(1);
}