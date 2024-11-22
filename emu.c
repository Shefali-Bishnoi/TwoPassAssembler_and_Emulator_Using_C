/*Name- Shefali Bishnoi
Roll number- 2301CS87
B.Tech. Second year, Computer Science and Engineering
Indian Institute of Technology Patna */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_MEMORY_SIZE 16777216

struct instruction {
    char pc[8];
    char ins[17]; //instruction in hexadecimal format
    struct instruction *next; //pointer to the next instruction in the list
};

struct data //represents data loaded into memory
{
    int pc;
    int value;
    int address;
    struct data *next;
};

struct instruction *instruction_head = NULL;
struct data *data_head = NULL;
int memory[MAX_MEMORY_SIZE]; //array representing memory
int traceMode = 0;     // Enable instruction trace
int beforeDump = 0;    // Show memory dump before execution
int afterDump = 0;     // Show memory dump after execution
int isaMode = 0;       // Display ISA

int regA=0, regB=0; /* Two registers A and B*/

void entryMsg(){
    // Entry message
    printf("Welcome to Emulator\n");
    printf("Options:\n");
    printf("1. -trace\n");
    printf("1. -before\n");
    printf("1. -after\n");
    printf("1. -isa\n");
}
//function to convert a decimal number to its hexadecimal equivalent
void decimalToHex(long decimal, char *result) {
    int isNegative = 0;
    long negativeAdjustment = 16777216;//2^24

    if (decimal == 0) 
    {
        result[0] = '0';
        result[1] = '\0';
        return;
    } 
    else if (decimal < 0) 
    {
        decimal = negativeAdjustment + decimal;
        isNegative = 1;
    }
    long quotient = decimal;
    int index = 0;
    char hexDigits[7];//6 hex digits + null terminator for 24 bits
    while (quotient != 0) 
    {
        int remainder = quotient % 16;
        if (remainder < 10)
            hexDigits[index++] = '0' + remainder;
        else
            hexDigits[index++] = 'A' + (remainder - 10);
        quotient /= 16;
    }
    int resultIndex = 0;
    for (int i = index - 1; i >= 0; i--) 
    {
        result[resultIndex++] = hexDigits[i];
    }
    result[resultIndex] = '\0';
}

void copyString(char *str1, const char *str2) {
    int i;
    for (i = 0; str2[i] != '\0'; i++) {
        str1[i] = str2[i];
    }
    str1[i] = '\0';
}

void addInstruction(const char *pcValue, const char *insValue) {
    struct instruction *newInstruction = malloc(sizeof(struct instruction));
    if (!newInstruction) 
    {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    copyString(newInstruction->pc, pcValue);
	copyString(newInstruction->ins, insValue);
    newInstruction->next = NULL;

    if (instruction_head==NULL) //first instruction
    {
        instruction_head = newInstruction;
    } 
    else {
        struct instruction *current = instruction_head;
        while (current->next !=NULL) //iterate through the linked list
        {
            current = current->next;
        }
        current->next = newInstruction;
    }
    return;
}

void displayInstructionList() 
{
    if (instruction_head == NULL) {
        printf("The instruction list is empty.\n");
        return;
    }
    struct instruction *current = instruction_head;
    printf("\nInstructions :\n");
    printf("PC \tInstruction\n");
    while (current != NULL)
    {
        printf("%s\t\t%s\n", current->pc, current->ins);
        current = current->next;
    }
}

void addData(int pcValue, int dataValue, int addressValue) {
    struct data *newData = malloc(sizeof(struct data));
    if (!newData) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    newData->pc = pcValue;
    newData->value = dataValue;
    newData->address = addressValue;
    newData->next = NULL;

    if (data_head==NULL) //first data entry
    {
        data_head = newData;
    } else {
        struct data *current = data_head;
        while (current->next) //iterate through the linked list
        {
            current = current->next;
        }
        current->next = newData;
    }
    return;
}

void displayDataList() 
{
    if (data_head == NULL) {
        printf("The data list is empty.\n");
        return;
    }
    struct data *current = data_head;
    printf("\nData in the Memory Data Linked List:\n");
    printf("Program Counter\tValue\tAddress\n");
    while (current != NULL) {
        printf("%d\t\t%d\t%d\n", current->pc, current->value, current->address);
        current = current->next;
    }
}


//locate the memory address associated with a pc value
int getAddressByPC(int pcValue) {
    struct data *current = data_head;
    while (current !=NULL) {
        if (current->pc == pcValue) {
            return current->address;
        }
        current = current->next;
    }
    return -1; //no address was found
}

//function to convert a hexadecimal string operand into a signed integer value
int processHexOperand(const char *hexOperand) {
    // Convert the hex string to an unsigned long integer
    char *end;
    unsigned long result = strtol(hexOperand, &end, 16);
    
    // Check if the value is in the range for negative signed 24-bit integers
    if (result > 0x7FFFFF) {
        // Convert it to the signed value 
        result -= 0x1000000;
    }

    return (int)result; // Return the signed integer
}

// Function to initialize the last 32 locations of memory with 0
void initialiseMemory() {
    int start = MAX_MEMORY_SIZE - 32; // Starting point for the last 32 locations
    for (int i = start; i < MAX_MEMORY_SIZE; i++) {
        memory[i] = 0;
    }
    printf("The last 32 memory locations have been initialized to 0.\n");
}

//function to displays the content of the memory (32 locations)
void displayMemory(int sp) {
    printf("Memory Contents:\n");
    printf("Address\tValue\n");
    for (int i = MAX_MEMORY_SIZE-32; i < MAX_MEMORY_SIZE; i++) {
        printf("%06X\t%d\n", i, memory[i]);
    }
}

//Function to display ISA
void displayISA(){
    printf("OpMachineCode Mnemonic Operand\n");
    printf("0      ldc      value \n");
    printf("1      adc      value \n");
    printf("2      ldl      value \n");
    printf("3      stl      value \n");
    printf("4      ldnl     value \n");
    printf("5      stnl     value \n");
    printf("6      add            \n");
    printf("7      sub            \n");
    printf("8      shl            \n");
    printf("9      shr            \n");
    printf("10     adj      value \n");
    printf("11     a2sp           \n");
    printf("12     sp2a           \n");
    printf("13     call     offset\n");
    printf("14     return         \n");
    printf("15     brz      offset\n");
    printf("16     brlz     offset\n");
    printf("17     br       offset\n");
    printf("18     HALT           \n");
}

/*Main Function for EMULATION of the OBJECT FILE*/
void emulate(char **operandArray, char **opcodeArray, int instructionCount)
{
	int pc;
	//int regA=0, regB=0; /* Two registers A and B*/
    int flag=0; 
	int sp = MAX_MEMORY_SIZE - 1;//Stack pointer starting from top of memory
	char hexPC[32];//For displaying PC in hexadecimal format 
	int executionCount = 0; //Counter for executed instructions 

    // Show memory dump before execution if beforeDump is enabled
    if (beforeDump) {
        printf("\nMemory Dump Before Execution:\n");
        displayMemory(sp);
    }

	/* first pass - assign all data locations from below,First Pass - Loading Data into Memory*/
	for(pc = 0; pc < instructionCount; pc++)
	{
		if (strcmp(opcodeArray[pc], "13") == 0) //data directive-->loads the associated operand into memory
		{
			memory[sp] = processHexOperand(operandArray[pc]);
			addData(pc, memory[sp], sp);
			sp--;
		}
	}
	displayDataList();
    // Reset pc for the main emulation loop
    pc = 0;
    //Second pass--iterate over each instruction, executing it based on its opcode
	for(pc = 0; pc < instructionCount;)
	{   flag=0;
        if (traceMode) {
            printf("Executing PC: %d | Opcode: %s | Operand: %s\n", pc, opcodeArray[pc], operandArray[pc]);
            // Print registers in hexadecimal format
            printf("Registers - A: %08X, B: %08X, SP: %X\n", regA, regB, sp);
        }

		if (strcmp(opcodeArray[pc], "00") == 0) //00 (Load): Loads an operand directly into register a
		{
			/* convert hex to decimal*/
			int operand = processHexOperand(operandArray[pc]);
			regB = regA;
			regA = operand;
		}
		else if(strcmp(opcodeArray[pc], "01") == 0) //01 (Add): Adds an operand to the value in a
		{
			int operand = processHexOperand(operandArray[pc]);
			regA = regA + operand;
		}
		else if(strcmp(opcodeArray[pc], "02") == 0) //02 (Load with Offset): Loads a value from memory at an offset from the stack pointer
		{
			int offset = processHexOperand(operandArray[pc]);
			regB = regA;
			regA = memory[sp + offset + 1];
		}
		else if(strcmp(opcodeArray[pc], "03") == 0) //03 (Store with Offset): Stores the value in a at an offset from the stack pointer
		{
			int offset = processHexOperand(operandArray[pc]);
			memory[sp + offset + 1] = regA;
			regA = regB;
		}
		else if(strcmp(opcodeArray[pc], "04") == 0) //04 (Load from Address): Loads a value from memory at the address in a with an offset
		{
			int offset = processHexOperand(operandArray[pc]);
			regA = memory[regA + offset + 1];
		}
		else if(strcmp(opcodeArray[pc], "05") == 0) //05 (Store to Address): Stores the value in b at the address in a with an offset
		{
			int offset = processHexOperand(operandArray[pc]);
			memory[regA + offset + 1] = regB;
		}
		else if(strcmp(opcodeArray[pc], "06") == 0) //06 (Add Registers): Adds the values in a and b and stores the result in a
		{
			regA = regB + regA;
		}
		else if(strcmp(opcodeArray[pc], "07") == 0) //07 (Subtract): Subtracts a from b and stores the result in a
		{
			regA = regB - regA;
		}
		else if(strcmp(opcodeArray[pc], "08") == 0) //08 (Shift Left): Shifts b left by a bits and stores in a
		{
			regA = regB << regA;
		}
		else if(strcmp(opcodeArray[pc], "09") == 0) //09 (Shift Right): Shifts b right by a bits and stores in a
		{
			regA = regB >> regA;
		}
		else if(strcmp(opcodeArray[pc], "0A") == 0) //0A (Adjust Stack): Adjusts the stack pointer by the operand
		{
			int operand = processHexOperand(operandArray[pc]);
			sp = sp + operand;
		}
		else if(strcmp(opcodeArray[pc], "0B") == 0) //0B (Set Stack Pointer): Sets the stack pointer to a if a is a valid address
		{
			if (regA >= MAX_MEMORY_SIZE)
			{
				pc++;
				flag = 1;
			}
			sp = regA;
			regA = regB;
		}
		else if(strcmp(opcodeArray[pc], "0C") == 0) //0C (Load SP): Loads the stack pointer value into a
		{
			regB = regA;
			regA = sp;
		}
		else if(strcmp(opcodeArray[pc], "0D") == 0) //0D (Call): Saves the current pc in a and jumps to a new location
		{
			/* For CALL instruction*/
			int operand = processHexOperand(operandArray[pc]);
			regB = regA;
			regA = pc;
			pc = pc + operand + 1;
			flag = 1;		
		}
		else if(strcmp(opcodeArray[pc], "0E") == 0) //0E (Return): Sets pc to a (used to return from a call)
		{
			/* For return instruction*/
			pc = regA;
			regA = regB;
		}
		else if(strcmp(opcodeArray[pc], "0F") == 0) //0F (Jump if Zero): Jumps by the operand if a is zero
		{
			int operand = processHexOperand(operandArray[pc]);
			if (regA == 0)
			{
				pc = pc + operand + 1;
				flag = 1;	
			}		
		}
		else if(strcmp(opcodeArray[pc], "10") == 0) //10 (Jump if Negative): Jumps by the operand if a is negative
		{
			int operand = processHexOperand(operandArray[pc]);
			if (regA < 0)
			{
				pc = pc + operand + 1;
				flag = 1;	
			}		
		}
		else if(strcmp(opcodeArray[pc], "11") == 0) //11 (Unconditional Jump): Jumps by the operand unconditionally
		{
			int operand = processHexOperand(operandArray[pc]);
			pc = pc + operand;
			flag = 1;			
		}
		else if(strcmp(opcodeArray[pc], "12") == 0) //12 (Halt): Stops execution by exiting the loop
		{
			/* Condition for HALT*/
			break;
		}
        
		/* Incrementing in normal case*/
        printf("After PC: %d - Registers: A = %08X, B = %08X\n\n", pc, regA, regB);
		if (flag == 0)
			pc++;
		executionCount++;
	}
    // Show memory dump after execution if afterDump is enabled
    if (afterDump) {
        printf("\nMemory Dump After Execution:\n");
        displayMemory(sp);
    }
	//displayMemory(sp);
	//printf("Total number of lines executed are %d\n", (executionCount + 1));
    //contents of registers after emulstion
    //printf("Registers - A: %08X, B: %08X, SP: %X\n", regA, regB, sp);
}

int main(int argc, char *argv[]) {
    if (argc < 2) //when no file was provided
    {
        fprintf(stderr, "Usage: %s <binary file>\n", argv[0]);
        return EXIT_FAILURE;
    }
    //memset(memory, 0, sizeof(memory));  // Initialize memory to 0 at the start
    initialiseMemory();
    entryMsg();
    // Read user input for options
    char input[100];
    fgets(input, sizeof(input), stdin);

    // Parse user input and set corresponding flags
    char *token = strtok(input, " \n");
    while (token != NULL) {
        if (strcmp(token, "trace") == 0) traceMode = 1;
        else if (strcmp(token, "before") == 0) beforeDump = 1;
        else if (strcmp(token, "after") == 0) afterDump = 1;
        else if (strcmp(token, "isa") == 0) isaMode = 1;
        else printf("Unknown option: %s\n", token);
        token = strtok(NULL, " \n");
    }

    // Display ISA if isaMode is enabled
    if (isaMode) {
        displayISA();
        return 0;
    }
    FILE *filePtr = fopen(argv[1], "rb");
    if (!filePtr) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    // Determine file size
    fseek(filePtr, 0, SEEK_END); /* seek to end of file*/
    int fileSize = ftell(filePtr); /* get current file pointer--retrieves the file size in bytes*/
    fseek(filePtr, 0, SEEK_SET); /* seek back to beginning of file*/
    printf("Size of the binary file is %d\n", fileSize);

    // Read file contents into buffer
    char buffer[fileSize];
    fread(buffer, sizeof(char), fileSize, filePtr);
    fclose(filePtr); //closes the file after reading

    int instructionCount = fileSize / 16; // assuming each instruction is 16 bytes
    char *pcArray[instructionCount], *instructionArray[instructionCount];
    char *opcodeArray[instructionCount], *operandArray[instructionCount];

    // Copy each instruction from buffer
    for (int i = 0; i < instructionCount; i++) {
        // Allocate 16 bytes for each instruction
        instructionArray[i] = (char *)malloc(16 * sizeof(char));
        for (int j = 0; j < 16; j++) {
            instructionArray[i][j] = buffer[(i * 16) + j];
        }

        // Separate each instruction into operand, opcode, and PC
        operandArray[i] = (char *)malloc(7 * sizeof(char));  // Allocate 7 bytes for operand (6 + null terminator)
        opcodeArray[i] = (char *)malloc(3 * sizeof(char));    // Allocate 3 bytes for opcode (2 + null terminator)
        pcArray[i] = (char *)malloc(9 * sizeof(char));        // Allocate 9 bytes for PC (8 + null terminator)

        // Copy the first six characters as operand
        for (int k = 0; k < 6; k++) {
            operandArray[i][k] = instructionArray[i][k];
        }
        operandArray[i][6] = '\0'; // Null terminator for operand

        // Copy the next two characters as opcode
        for (int k = 0; k < 2; k++) {
            opcodeArray[i][k] = instructionArray[i][6 + k];
        }
        opcodeArray[i][2] = '\0'; // Null terminator for opcode

        // Copy the last eight characters as PC
        for (int k = 0; k < 8; k++) {
            pcArray[i][k] = instructionArray[i][8 + k];
        }
        pcArray[i][8] = '\0'; // Null terminator for PC

        // Print operand, opcode, and PC for debugging
        printf("Operand: %s\tOpcode: %s\tPC: %s\n", operandArray[i], opcodeArray[i], pcArray[i]);
    }

    // Call emulate function with operand and opcode arrays
    emulate(operandArray, opcodeArray, instructionCount);// Emulate with options enabled

    // Free allocated memory
    for (int i = 0; i < instructionCount; i++) {
        free(instructionArray[i]);
        free(opcodeArray[i]);
        free(operandArray[i]);
    }
    //printf("Registers - A: %08X, B: %08X\n", regA, regB);
    return 0;
}
