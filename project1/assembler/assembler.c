/* Assembler code fragment for LC-2K */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAXLINELENGTH 1000

typedef struct _DICTIONARY{
	char* key;
	int value;
	struct _DICTIONARY* link;
}_dictionary;

typedef struct DICTIONARY{
	int count;
	struct _DICTIONARY* head;
}dictionary;

/*Function relative with dictionary*/
void DICTIONAY_SHOW(dictionary* dic);
int ShowOffset(dictionary* dic, char* key);
void DictionaryAdd(dictionary* head, char* key, int offset);
void DictionaryInit(dictionary* head);

/*Utility Function*/
int readAndParse(FILE *, char *, char *, char *, char *, char *);
int isNumber(char *);
char*** InstrAddress(FILE *,char ***, char *, char *, char *, char *, char *, dictionary* head);
void PrintIstr(char***);
void Delocate(char ***);

/*Formatting Function*/
void Change01(char ***, dictionary*);
int Rformat(char** );
int Iformat(char** , int, dictionary*);
int Jformat(char**);
int Oformat(char**);

/*Exception Handling Function*/
void UndefinedLabel();
void duplLabel();
void offsetLimit();
void UnregOp();
void NonRegister();
void RegisterLimit();

/*Global variable*/
int regist[8] = {0}; //register value
int int_instruction[MAXLINELENGTH];
int numOfinstr = 0;


int main(int argc, char *argv[]) 
{
	char *inFileString, *outFileString;
	FILE *inFilePtr, *outFilePtr;
	char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH], 
			 arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];
	int *mem[MAXLINELENGTH]; // Memory address
	

	if (argc != 3) {
		printf("error: usage: %s <assembly-code-file> <machine-code-file>\n",
				argv[0]);
		exit(1);
	}

	inFileString = argv[1];
	outFileString = argv[2];

	inFilePtr = fopen(inFileString, "r");
	if (inFilePtr == NULL) {
		printf("error in opening %s\n", inFileString);
		exit(1);
	}
	outFilePtr = fopen(outFileString, "w");
	if (outFilePtr == NULL) {
		printf("error in opening %s\n", outFileString);
		exit(1);
	}

	/* TODO: Phase-1 label calculation */
	// Define Variable
	// Instruction Memory in 3-dimension array
	char ***instr = malloc(MAXLINELENGTH * sizeof(char **)); 
    for (int i = 0; i < MAXLINELENGTH; i++) {
        instr[i] = malloc(MAXLINELENGTH * sizeof(char *));
		for (int j = 0; j < MAXLINELENGTH; j++) {
			instr[i][j] = malloc(MAXLINELENGTH * sizeof(char));}
    }

	// dictionary for finding offset in word
	dictionary* offset_dic = (dictionary*)malloc(sizeof(dictionary)); 
	DictionaryInit(offset_dic);

	instr = InstrAddress(inFilePtr, instr, label, opcode, arg0, arg1, arg2, offset_dic);
	// DICTIONAY_SHOW(offset_dic);
	// PrintIstr(instr);


	/* TODO: Phase-2 generate machine codes to outfile */
	Change01(instr, offset_dic);
	
	for(int i=0; i<numOfinstr; i++){
		printf("%d \n", int_instruction[i]);
	}

	//PrintIstr(instr);

	// Print in file 
	for (int i=0; i<numOfinstr; i++){
		fprintf(outFilePtr, "%d\n", int_instruction[i]);
	}

	if (inFilePtr) {
		fclose(inFilePtr);
	}
	if (outFilePtr) {
		fclose(outFilePtr);
	}

	free(offset_dic);
	Delocate(instr); // Delocate the instruction memory
	exit(0); // If finished program without error

	return(0);
}

/*
 * Read and parse a line of the assembly-language file.  Fields are returned
 * in label, opcode, arg0, arg1, arg2 (these strings must have memory already
 * allocated to them).
 *
 * Return values:
 *     0 if reached end of file
 *     1 if all went well
 *
 * exit(1) if line is too long.
 */
int readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0,
		char *arg1, char *arg2)
{
	char line[MAXLINELENGTH];
	char *ptr = line;

	/* delete prior values */
	label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';
	
	/* read the line from the assembly-language file */
	if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL) {
		/* reached end of file */
		return(0);
	}

	/* check for line too long (by looking for a \n) */
	if (strchr(line, '\n') == NULL) {
		/* line too long */
		printf("error: line too long\n");
		exit(1);
	}
	
	int i = 0;
	/* is there a label? */
	ptr = line;
	if (sscanf(ptr, "%[^\t\n\r ]", label)) {
		/* successfully read label; advance pointer over the label */
		ptr += strlen(label);
	}
	/*
	 * Parse the rest of the line.  Would be nice to have real regular
	 * expressions, but scanf will suffice.
	 */
	sscanf(ptr, "%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%"
			"[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]", opcode, arg0, arg1, arg2);

	return(1);
}

int isNumber(char *string)
{
	/* return 1 if string is a number */
	int i;
	return( (sscanf(string, "%d", &i)) == 1);
}

char*** InstrAddress(FILE *inFilePtr, char*** instr, char* label, char* opcode, char* arg0, char* arg1, char* arg2, dictionary* dic) {
	int i = -1; // index of memory
	while(readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)) { 
		i++;
		numOfinstr++;
		strcpy(instr[i][0], label);
		if (strcmp(label, "")) { // If there is Label in word
			DictionaryAdd(dic, instr[i][0], i); }
		strcpy(instr[i][1], opcode);
		strcpy(instr[i][2], arg0);
		strcpy(instr[i][3], arg1);
		strcpy(instr[i][4], arg2);
	}

	// Check for .fill
	for (int i=0; i<numOfinstr; i++){
		if(!strcmp(instr[i][1], ".fill")){
			if(isNumber(instr[i][2])) { int_instruction[i] = atoi(instr[i][2]); }
			else {int_instruction[i] = ShowOffset(dic, instr[i][2]);}
		}
	}
	return instr;
}

void PrintIstr(char*** instr){
	printf("Memory: \n");
	for(int i=0; i<numOfinstr; i++){
		printf("Memory[%d] -> ", i);
		for (int j=0; j<5; j++){
			printf("%s ", instr[i][j]); // Read the label of instructions
		}
		printf("\n");
	}
}

void Delocate(char*** arr){
	for (int i=0; i<MAXLINELENGTH; i++){
		for(int j=0; j<MAXLINELENGTH; j++){
			free(arr[i][j]);}
		free(arr[i]);}
	free(arr);
}

void DictionaryInit(dictionary* dic){
	dic->count = 0;
	dic->head = NULL;
}

void DICTIONAY_SHOW(dictionary* dic){
	_dictionary* temp = dic->head;
    int i = 0;

    printf("--- total count : %d\n", dic->count);
    while (temp != NULL) {
        printf("[%d] %s\n", temp->value, temp->key);
        i++; 
		temp = temp->link;
    }
}

int ShowOffset(dictionary* dic, char* _key){
	_dictionary* temp = dic->head;
    while (temp != NULL) {
		if (!strcmp(temp->key, _key)) {
			return temp->value;
		}
		temp = temp->link;
    }
	UndefinedLabel(); // If no key in dictionary
}

void DictionaryAdd(dictionary* dic, char* _key, int offset){
	_dictionary* tmp = dic->head;
	while(1){
		// No header
		if (dic->count == 0){
			tmp = (_dictionary*)malloc(sizeof(_dictionary));
			tmp->key = _key;
			tmp->value = offset;
			tmp->link = NULL;
			dic->head = tmp;
			break;
		}
		else if ( !strcmp(tmp->key, _key) ) { duplLabel(); }
		else if (tmp->link == NULL){
			tmp->link = (_dictionary*)malloc(sizeof(_dictionary));
			tmp->link->key = _key;
			tmp->link->value = offset;
			tmp->link->link = NULL;
			break;
		}
		else {
			tmp = tmp->link;
		}
	}
	dic->count++;
}

void Change01(char *** instr, dictionary* offset_dic){
	for(int i=0; i<numOfinstr; i++){
		/* after doing a readAndParse, you may want to do the following to test the opcode */
		char* opcode = instr[i][1];
		// R-Format
		if(!strcmp(opcode, "add")) { int_instruction[i] = Rformat(instr[i]);}
		if(!strcmp(opcode, "nor")) { int_instruction[i] = Rformat(instr[i]);}

		// I-Format 
		if(!strcmp(opcode, "lw")) {int_instruction[i] = Iformat(instr[i], i, offset_dic);}
		if(!strcmp(opcode, "sw")) {int_instruction[i] = Iformat(instr[i], i, offset_dic);}
		if(!strcmp(opcode, "beq")) {int_instruction[i] = Iformat(instr[i], i, offset_dic);}

		// J-Format
		if(!strcmp(opcode, "jalr")) {int_instruction[i] = Jformat(instr[i]);}

		// O-Format
		if(!strcmp(opcode, "halt")) {int_instruction[i] = Oformat(instr[i]);}
		if(!strcmp(opcode, "noop")) {int_instruction[i] = Oformat(instr[i]);}
	}
}

int Rformat(char** instr){
	int mc = 0;
	char* regA = instr[2]; // 3bit
	char* regB = instr[3]; // 3bit
	char* regDest = instr[4]; // 3bit

	// Check if the register is Integer
	NonRegister(regA);
	NonRegister(regB);
	NonRegister(regDest);

	// Check if the register is in range
	RegisterLimit(regA);
	RegisterLimit(regB);
	RegisterLimit(regDest);

	// Opcode
	char* opcode = instr[1];
	if (!strcmp(opcode, "add")) {
		mc = (0 << 22);
	}
	else if (!strcmp(opcode, "nor")){
		mc = (1 << 22);
	}
	else{ UnregOp();}

	mc |= (atoi(regA) << 19);
	mc |= (atoi(regB) << 16);
	mc |= (atoi(regDest) << 0);

	return mc;
}

int Iformat(char** instr, int pc, dictionary* offset_dic){
	int mc = 0;
	int address = 0;
	char* regA = instr[2]; // 3bit
	char* regB = instr[3]; // 3bit
	char* offset = instr[4]; // 16bit

	// Check if the register is Integer
	NonRegister(regA);
	NonRegister(regB);

	// Check if the register is in range
	RegisterLimit(regA);
	RegisterLimit(regB);

	// Opcode
	char* opcode = instr[1];
	if (!strcmp(opcode, "lw")) {
		mc |= (2 << 22);
	}
	else if (!strcmp(opcode, "sw")){
		mc |= (3 << 22);
	}
	else if (!strcmp(opcode, "beq")){
		mc |= (4 << 22);
	}
	else{ UnregOp();}

	mc |= (atoi(regA) << 19);
	mc |= (atoi(regB) << 16);

	// For calculating offset
	if (isNumber(offset)){ // if offset is in number
		address = atoi(offset);
		offsetLimit(address);
	}
	else { //if offset is not in number
		address = ShowOffset(offset_dic, offset);
		offsetLimit(address);
	}
	// When address is dependent on PC
	if(!strcmp(opcode, "beq") && !isNumber(offset)){
		int dest = address;
		address = dest - pc - 1;
	}

	address &= 0xFFFF;
	mc |= address;

	return mc;
}

int Jformat(char** instr){
	int mc = 0;
	char* regA = instr[2];
	char* regB = instr[3];

	// Check if the register is Integer
	NonRegister(regA);
	NonRegister(regB);

	// Check if the register is in range
	RegisterLimit(regA);
	RegisterLimit(regB);

	// Opcode
	char* opcode = instr[1];
	if (!strcmp(opcode, "jalr")) {
		mc |= (5 << 22);
	}
	else{ UnregOp();}

	// Register
	mc |= (atoi(regA) << 19);
	mc |= (atoi(regB) << 16);
	
	return mc;
}

int Oformat(char** instr){
	int mc = 0;
	// Opcode
	char* opcode = instr[1];
	if (!strcmp(opcode, "halt")) {
		mc |= (6 << 22);
	}
	else if (!strcmp(opcode, "noop")) {
		mc |= (7 << 22);
	}
	else{ UnregOp();}
	return mc;
}

/*Exception Handling Function*/
void UndefinedLabel(){
	printf("Use of undefined labels\n");
	exit(1);
}
void duplLabel(){
	printf("Duplicate definition of labels\n");
	exit(1);
}
void offsetLimit(int address){
	if (address > 32767 || address <-32768){
		printf("offset Fields that don't fit in 16 bits");
		exit(1);}	
}
void UnregOp(){
	printf("Unrecognized opcodes");
	exit(1);
}
void NonRegister(char* regis){
	if(!isNumber(regis)){
		printf("Non-integer register arguments");
		exit(1);
	}
}
void RegisterLimit(char* regis){
	int address = atoi(regis);
	if (address > 8 || address <0){
		printf("Register outside the range [0,7]");
		exit(1);
	}
}