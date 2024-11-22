/*Name- Shefali Bishnoi
Roll number- 2301CS87
B.Tech. Second year, Computer Science and Engineering
Indian Institute of Technology Patna */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include<ctype.h>

//structure to store pairs of mnemonic names and their associated integer codes
struct opcode
{
    char opcode_name[50]; //Name of mnemonic
    int code;
};

//Mnemonic and associated opcode
struct opcode Opcodes[21]={
    {"ldc",0},
    {"adc",1},
    {"ldl",2},
    {"stl",3},
    {"ldnl",4},
    {"stnl",5},
    {"add",6},
    {"sub",7},
    {"shl",8},
    {"shr",9},
    {"adj",10},
    {"a2sp",11},
    {"sp2a",12},
    {"call",13},
    {"return",14},
    {"brz",15},
    {"brlz",16},
    {"br",17},
    {"HALT",18},
    {"data",19},
    {"SET",20}
};

//structure to store a single line of assembly code
struct instruction
{
    int pc; //program counter
    int line_number; //line number in the source code
    char label_name[50];
    int opcode; 
    char mnemonic[50];
    char operand[50];
    struct instruction* next; //Pointer to the next instruction structure in a linked list
};

//structure to store error
struct ErrorInfo
{
    int sourceline;
    char description[100];
    struct ErrorInfo* next; //Pointer to the next error in the list
};

struct ErrorInfo *error_head=NULL; //points to the head of linked list storing all errors
int error_count=0; //tracks the total number of errors detected
int label_count=0; //tracks the number of unique labels found

//add error to the linked list
void recorderror(int sourceline_cl, char* description_cl)
{
    //adding the first error
    if(error_head==NULL)
    {
        error_head=(struct ErrorInfo*)(malloc(sizeof(struct ErrorInfo)));
        error_head->sourceline=sourceline_cl;
        strcpy(error_head->description,description_cl);
        error_head->next=NULL;
        error_count++;
    }
    //adding subsequent errors
    else
    {
        struct ErrorInfo* temp=(struct ErrorInfo*)(malloc(sizeof(struct ErrorInfo)));
        temp->sourceline=sourceline_cl;
        strcpy(temp->description,description_cl);
        temp->next=NULL;
        error_count++;
        struct ErrorInfo* current=error_head; //pointer to traverse the linked list
        while(current->next != NULL) //traversing the linked list to attach temp at end
        {
            current=current->next;
        }
        current->next=temp;
        return;
    }
}

// Function to find and return the opcode for a given mnemonic 
int findOpcode(char *mnemonic) 
{
    for (int i = 0; i < 21; i++) 
    {
        if (strcmp(mnemonic, Opcodes[i].opcode_name) == 0) 
        {
            return Opcodes[i].code;  
        }
    }
    return -1;  //-1 indicates invalid opcode
}

//add valid code lines to the linked list
void addInstruction(struct instruction* head, int pc_cl, int line_num_cl, char* label_name_cl, char* mnemonic_cl, char* operand_cl)
{
    struct instruction* temp=(struct instruction*)malloc(sizeof(struct instruction));
    strcpy(temp->mnemonic, mnemonic_cl);
    strcpy(temp->operand, operand_cl);
    strcpy(temp->label_name, label_name_cl);
    temp->pc=pc_cl;
    temp->line_number=line_num_cl;
    temp->next=NULL;
    //assign opcode value based on mnemonic
    int opcode=findOpcode(mnemonic_cl);
    if(opcode!=-1)//valid opcode found
    {
        temp->opcode=opcode;
    }
    else //if no match found
    {
        recorderror(line_num_cl+1,"Mnemonic not found");
        temp->opcode=-1; //indicates invalid opcode
    }
    struct instruction* current=head; //pointer to iterate over the linked list
    while(current->next!=NULL)
    {
        current=current->next;
    }
    current->next=temp;
    return;
}

//create a linked list representation of instructions
struct instruction* generateCodeRepresentation(char **mnemonics, char **operands, char **labels, int total_instructions)
{
    struct instruction* head=NULL;
    int pc=0;
    int start=0;
    //find the first valid line containing a mnemonic
    for(int i=0;i<total_instructions;i++)
    {
        if(strcmp(mnemonics[i],"")!=0)
        {
            printf("%s",labels[i]);
            head=(struct instruction*)(malloc(sizeof(struct instruction)));
            strcpy(head->mnemonic,mnemonics[i]);
            strcpy(head->operand,operands[i]);
            //strcpy(head->label_name,labels[i]);
            strncpy(head->label_name, labels[i], sizeof(head->label_name) - 1);
            head->label_name[sizeof(head->label_name) - 1] = '\0';
            printf("%s",head->label_name); 
            head->pc=0;
            head->line_number=i;
            head->next=NULL;
            //assign opcode value based on mnemonic
            int opcode=findOpcode(mnemonics[i]);
            if(opcode!=-1)//valid opcode found
            {
                head->opcode=opcode;
            }
            else //if no match found
            {
                recorderror(i+1,"Mnemonic not found");
                head->opcode=-1; //indicates invalid opcode
            }
            start=i;
            pc++;
            break;
        }
    }
    if(head != NULL) //valid head found, now process the remaining lines
    {
        for(int i=start+1;i<total_instructions;i++)
        {
            //add each valid line to the linked list
            if(strcmp(mnemonics[i],"")!=0)
            {
                addInstruction(head,pc, i, labels[i], mnemonics[i], operands[i]);
                pc++;
            }
        }
        return head;
    }
    else
    {
        printf("Failed to initialise head\n");
        return NULL;
    }
}

//print linked list of code instructions
void showInstructionList(struct instruction* head)
{
    struct instruction* current=head; //pointer to iterate over the linked list
    printf("\nCode List Representation\n");
    printf("PC | Label | Mnemonic | Opcode | Operand\n");
    printf("-----------------------------------------------------------\n");
    while (current != NULL)
    {
        printf("%d\t", current->pc);
        printf("%s\t", current->label_name);
        printf("%s\t", current->mnemonic);
        printf("%d\t", current->opcode);
        printf("%s\n", current->operand);
        current = current->next;
    }
}

//Function to check if a label exists and return its value or offset
int findLabel(char* label,struct instruction* head, int source_line,int current_pc, int mode)
{
    struct instruction* current=head;
    int found=0, offset=0;
    //traverse linked list until either the label is found or the end of the list is reached
    while(current!=NULL)
    {
        if(strcmp(label,current->label_name)==0)
        {
            found=1; //label found
            if(mode==1) //calculate relative offset
            {
                offset=current->pc-current_pc;
            }
            else if(mode==0)
            {
                if(current->opcode!=20) // opcode 20 is for SET instruction
                {
                    offset=current->pc;
                }
                else if(current->opcode==20) //SET instruction--interpret the label's operand as a constant
                {
                    char* end_ptr;
                    int constant;
                    constant=strtol(current->operand,&end_ptr,0);
                    offset=constant;
                }
            }
            break; //as label has been found
        }
        current = current->next;
    }
    if(found==0)
    {
        recorderror(source_line,"Label not found");
    }
    return offset;
}

//determine if the operand is a label or a number
long handleOperand(char* operand, struct instruction* head, int line_num, int pc, int mode)
{
    char* end_ptr;
    long constant;
    constant=strtol(operand, &end_ptr,0);
    if (strcmp(end_ptr, "") != 0)
    {
        if (strcmp(operand, end_ptr) == 0) //operand is a label
        {
            constant= findLabel(operand, head, line_num, pc, mode);
        }
        else 
        {
            recorderror(line_num, "Operand is not a valid number");
        }

    }
    return constant;
}

// function to check whether the given operand (operand_s) exists (i.e., is not an empty string).
int CheckOperandExistence(char* operand)
{
    if (strcmp(operand, "") == 0) 
    {
        return 0;  // Operand does not exist
    }
    return 1;  // Operand exists
}

//function to convert a decimal number to its hexadecimal equivalent
void decimalToHex(long decimal, char *result) {
    int isNegative = 0;
    long negativeAdjustment = 16777216;

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
    char hexDigits[100];
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

// Function to print a label and its associated program counter (PC) 
void displayLabelList(FILE *flst, char *label, char *pcAddress) {
    int paddingLength = 8 - strlen(pcAddress);

    // Print leading zeros for the PC address to ensure it's 8 characters wide
    for (int i = 0; i < paddingLength; i++) 
    {
        fprintf(flst, "0");
    }
    fprintf(flst, "%s\t", pcAddress);
    // Print spaces to align the label properly
    for (int i = 0; i < 8; i++) {
        fprintf(flst, " ");
    }
    // Print the label 
    fprintf(flst, "\t%s:\n", label);
}

// Function to write a binary object (instruction) to a file
void writeBinaryObject(FILE *fbin,  char *instruction) 
{
    // Write the instruction to the binary file
    fwrite(instruction, strlen(instruction), 1, fbin);
}

//Function to print machine code into a listing file
void printMachineCode(FILE *fbin, FILE *flst, char *pc_hex, char *opcode_hex, char *operand_hex, char *mnemonic, char *operand)
{
    char pc[8];  //Stores the zero-padded PC in an 8-character format
    char instruction[17];  //Stores the complete zero-padded machine code instruction.
    
    // Write PC to the list file with leading zeros
    int pc_padding = 8 - strlen(pc_hex);
    int l=0;
    for(int i=0; i < pc_padding; i++) {
        fprintf(flst, "0");
        pc[l++] = '0';
    }
    fprintf(flst, "%s\t", pc_hex);
    for(int i=0; i < strlen(pc_hex); i++) {
        pc[l++] =pc_hex[i];
    }
    // Write operand to the list file with leading zeros
    l=0;
    int op_padding = 6 - strlen(operand_hex);
    for(int i=0; i < op_padding; i++) {
        fprintf(flst, "0");
        instruction[l++] = '0';
    }
    fprintf(flst, "%s", operand_hex);
    for(int i=0; i< strlen(operand_hex); i++) {
        instruction[l++] = operand_hex[i];
    }
    // Write opcode to the list file with leading zeros
    op_padding = 2 - strlen(opcode_hex);
    for(int i=0; i < op_padding; i++) {
        fprintf(flst, "0");
        instruction[l++] = '0';
    }
    for(int i=0; i < strlen(opcode_hex); i++) {
        instruction[l++] = opcode_hex[i];
    }
    fprintf(flst, "%s\t", opcode_hex);
    // Write mnemonic and operand to the list file
	fprintf(flst, "%s ", mnemonic);
	fprintf(flst, "%s\n", operand);
    // --- Add PC to the end of instruction ---
    // Padding PC with leading zeros if needed
    int pc_len = strlen(pc_hex);
    pc_padding = 8 - pc_len;
    for (int i = 0; i < pc_padding; i++) {
        instruction[l++] = '0';
    }
    for (int i = 0; i < pc_len; i++) {
        instruction[l++] = pc_hex[i];
    }
    instruction[16]='\0';
	writeBinaryObject(fbin,instruction);
}

/* Helper function to convert numbers to hex and write formatted output */
void convertAndWriteToFile(FILE *fptr, int pc, int opcode, long operand, char *pc_hex, char *opcode_hex, char *operand_hex) {
    int pc_pad, op_pad, k;
    // Convert PC to hex and add padding
    decimalToHex(pc, pc_hex);
    pc_pad = 8 - strlen(pc_hex);
    for (k = 0; k < pc_pad; k++) {
        fprintf(fptr, "0");
    }
    fprintf(fptr, "%s\t", pc_hex);
    // Convert opcode to hex and add padding
    decimalToHex(opcode, opcode_hex);
    op_pad = 2 - strlen(opcode_hex);
    for (k = 0; k < op_pad; k++) {
        fprintf(fptr, "0");
    }
    fprintf(fptr, "%s\t", opcode_hex);
    // Convert operand to hex and add padding
    decimalToHex(operand, operand_hex);
    op_pad = 6 - strlen(operand_hex);
    for (k = 0; k < op_pad; k++) {
        fprintf(fptr, "0");
    }
    fprintf(fptr, "%s\n", operand_hex);
}

// Main Function for generating the MACHINE CODE 
void secondPass(struct instruction* head, char *file_name) {
    char name_file[50]; // Preparing file names
    char base_name[50];
    strcpy(name_file, file_name);
    strcpy(base_name,file_name);
    strcat(file_name, ".lst");
    strcat(base_name,"_listfile.txt");
    FILE *fptr = fopen(base_name, "w"); // Open files for writing
    if (fptr == NULL) {
        printf("Error in opening list file!");
        exit(1);
    }
    FILE *flst = fopen(file_name, "w");
    if (flst == NULL) {
        printf("Error in opening list file!");
        exit(1);
    }
    // Opening the binary object file
    strcat(name_file, ".o");
    FILE *fbin = fopen(name_file, "wb");
    if (fbin == NULL) {
        printf("Error in opening binary file!");
        exit(1);
    }
    struct instruction* current = head;
    int ctr = 0;
    char pc_hex[32], opcode_hex[32], operand_hex[32];
    int pc_pad, op_pad;
    int type;
    while (current != NULL) {
        int opcode = current->opcode;
        int flag = 1;  // Initialize as 1 to assume operand exists unless stated otherwise
        // Check if opcode requires an operand
        if (!(opcode == 6 || opcode == 7|| opcode == 8|| opcode == 9|| opcode == 11|| opcode == 12|| opcode == 14|| opcode == 18 || opcode==20)) { // Skip operand check for opcodes 6 and 7
        flag = CheckOperandExistence(current->operand);
        if (flag == 0) {
            recorderror(current->line_number + 1, "Operand doesn't exist error");
            current = current->next;
            continue;  // Skip further processing for this instruction
        }
        }
        if(flag==1) {
            // Common setup for writing to the list and binary files
            long decoperand = 0;
            type = (opcode == 0 || opcode == 1 || opcode == 10 || opcode == 19 || opcode == 20) ? 0 : 1;
            if (opcode == 2 || opcode == 3 || opcode == 4 || opcode == 5 || opcode == 13 || opcode == 15 || opcode == 16 || opcode == 17) {
                decoperand = handleOperand(current->operand, head, current->line_number + 1, current->pc, type);
                decoperand -= 1;  // Subtract 1 for certain opcodes (e.g., jump-related)
            } else {
                decoperand = handleOperand(current->operand, head, current->line_number + 1, current->pc, type);
            }
            // Convert PC, opcode, and operand to hex and write to the list file
            convertAndWriteToFile(fptr, current->pc, opcode, decoperand, pc_hex, opcode_hex, operand_hex);
            // Print label information if present
            if (strcmp(current->label_name, "") != 0) {
                displayLabelList(flst, current->label_name, pc_hex);
            }
            // Print the machine code into binary object file
            printMachineCode(fbin, flst,  pc_hex, opcode_hex, operand_hex, current->mnemonic, current->operand);
        }
        // Move to the next instruction in the code list
        current = current->next;
        ctr++;
    }
    // Close files
    fclose(fptr);
    fclose(flst);
    fclose(fbin);
}


//Function to display the errors that occurred during the assembly process and write the error information to a log file
void displayErrorList(FILE *flog) {
    if (error_head == NULL) {
        return; // Return immediately if there are no errors
    }
    struct ErrorInfo* current = error_head;
    fprintf(flog, "\nData in the Error Linked List\n");
    // Loop through the error list and print each error
    while (current != NULL) {
        fprintf(flog, "Line %d: %s\n", current->sourceline, current->description);
        current = current->next;
    }
}

//Function to check if a given label has already been used
int checkLabelRepetition(char *mnemonic, char **labels) {
    // Loop through the list of labels to check if the current one is repeated
    for (int j = 0; j < label_count; j++) {
        if (strcmp(mnemonic, labels[j]) == 0) {
            return 1; // If label already exists, return 1 (error)
        }
    }
    return 0; // If no duplicate label is found, return 0
}

// Function for removing spaces from a string s
void remove_spaces(char* s) {
    const char* source = s;
    do {
        while (*source == ' ') {
            ++source;  // Skip over any spaces
        }
    } while (*s++ = *source++);  // Copy non-space characters
}

// Main Function for 1st Pass and initiating the 2nd Pass 
int main(int argc, char *argv[]) {
    FILE *fp; //File pointer for  input assembly file
    FILE *flog;  // File pointer for log file(used to record errors and messages during the assembly process.)
    char log_file_name[50];
    char *line = NULL;  // Pointer to each line in the file
    size_t len = 0;
    ssize_t read;  // Length of each line read
    const int NUM_LINES = 100, CHAR_LEN = 100;
    // Declaring arrays for each parsed element
    char *lines[NUM_LINES], *labels[NUM_LINES], *mnemonics[NUM_LINES];
    char *operands[NUM_LINES]; 
    char *valid_labels[NUM_LINES]; //to store validated labels
    int i;
    int line_count = 0; //track the number of processed lines
    // Memory allocation for each array
    for (i = 0; i < NUM_LINES; i++) {
        lines[i] = (char *)malloc(CHAR_LEN * sizeof(char));
        labels[i] = (char *)malloc(CHAR_LEN * sizeof(char));
        valid_labels[i] = (char *)malloc(CHAR_LEN * sizeof(char));
        valid_labels[i][0] = '\0';  // Initialize as empty
        mnemonics[i] = (char *)malloc(CHAR_LEN * sizeof(char));
        operands[i] = (char *)malloc(CHAR_LEN * sizeof(char));
    }
    // Check for the input file argument
    if (argc < 2) {
        fprintf(stderr, "Error: No input file specified.\n");
        return EXIT_FAILURE;
    }
    char *file_name = argv[1];
    fp = fopen(file_name, "r");
    if (fp == NULL) {
        perror("Error opening input file");
        return EXIT_FAILURE;
    }
    // Prepare and open the log file
    strcpy(log_file_name, file_name);
    // Find the last dot (.) in the file name to identify the extension
    char *dot_pos = strrchr(log_file_name, '.');
    if (dot_pos != NULL) {
        // Replace the extension with ".log"
        *dot_pos = '\0';  // Remove the current extension
    }
    strcat(log_file_name, ".log");
    flog = fopen(log_file_name, "w");
    if (flog == NULL) {
        perror("Error opening log file");
        fclose(fp);
        return EXIT_FAILURE;
    }

    // Read each line from the file
    while ((read = getline(&line, &len, fp)) != -1)
    {
        printf("Retrieved line of length %zu:\n", read); //length is stored in read
        printf("%s", line);
        // Process each line for labels and code
        int colon_pos = -1;
        char *temp_store = (char*)(malloc(CHAR_LEN*sizeof(char)));
        for (i = 0; i < strlen(line); i++) //iterate over each character in the line to separate labels from instructions
        {
            if (line[i] == '\n') //end of current line
                break;
            if (line[i] == ';') //comment check
                break;
            if (line[i] == ':') {
                colon_pos = i;
                break;
            }
            temp_store[i] = line[i];
        }

        // Processing lines based on colon position
        if (colon_pos == -1) //No colon found => instruction without label 
        {
            strcpy(lines[line_count], temp_store);
            strcpy(labels[line_count], "");
        } else //colon found=> instruction with label
        {
            char *temp_processed = (char*)(malloc(CHAR_LEN*sizeof(char)));  //to hold the instruction part following colon
            for (i = colon_pos + 1; i < strlen(line); i++) {
                if (line[i] == '\n') // Newline check
                    break;
                if (line[i] == ';') // Comment check 
                    break;
                if (line[i] == (int)'\t') //for consistent spacing convert tabs to spaces
                {
                    temp_processed[i - colon_pos - 1] += ' ';
                    continue;
                }
                temp_processed[i - colon_pos - 1] = line[i];
            }
            strcpy(lines[line_count], temp_processed); //store the instruction part
            strcpy(labels[line_count], temp_store); //store the label part
        }
        line_count++;
    }
    //extract mnemonic and operand from each line stored in lines array
    printf("\nPrinting the Lines:\n");
    for (i = 0; i < line_count; i++) {
        printf("%s\n", lines[i]);

        char temp_line[100];
        strcpy(temp_line, lines[i]);
        char *token = strtok(temp_line, " ");
        int token_count = 0;
        strcpy(mnemonics[i], "");
        strcpy(operands[i], "");

        // mnemonic and operand extraction
        while (token) {
            if (token_count == 0)  //Mnemonic extraction
            {
                char* c = token;
                while (isspace(*c)) //remove any leading spaces in the token
                    ++c;
                strcpy(mnemonics[i], c);
            } 
            else if (token_count == 1) //operand extraction
            {
                strcpy(operands[i], token);
            } 
            else if (token_count > 1) 
            {
                recorderror(i + 1, "More than one operand error");
            }
            printf("token: %s\n", token);
            token = strtok(NULL, " ");
            token_count++;
        }
    }

    // Process labels for errors and valid labels
    for (i = 0; i < line_count; i++) 
    {
        if (strcmp(valid_labels[i], "") != 0) //skipping non-empty valid labels
            continue;

        int is_valid_label = 1;
        for (int j = 0; j < strlen(labels[i]); j++) 
        {
            if (j == 0 && !isalpha(labels[i][j])) //first character check
            {
                recorderror(i + 1, "Invalid Label Name Error");
                is_valid_label = 0;
                break;
            }
            if (!isalnum(labels[i][j])) //remaining characters check
            {
                recorderror(i + 1, "Invalid Label Character Error");
                is_valid_label = 0;
                break;
            }
        }
        //further processing if label is valid
        if (is_valid_label && strcmp(labels[i], "") != 0) 
        {
            if (!checkLabelRepetition(labels[i], valid_labels)) //checking for duplicates, further processing if label is unique
            {
                if (strcmp(mnemonics[i], "") != 0) //label with a corresponding mnemonic
                {
                    strcpy(valid_labels[i], labels[i]);
                } 
                else //label without associated mnemonic
                {
                    int k = i + 1;
                    while (k < line_count && strcmp(mnemonics[k], "") == 0) //increment k till a line with a mnemonic is found
                        k++;
                    if (k < line_count) 
                    {
                        strcpy(valid_labels[k], labels[i]);
                    } 
                    else 
                    {
                        recorderror(i + 1, "Conflicting Labels Error");
                    }
                }
            } 
            else 
            {
                recorderror(i + 1, "Label Repeated Error");
            }
        }
        else
        {
            strcpy(valid_labels[i], ""); //skipping the invalid labels
        }
        label_count++;
    }
    fclose(fp);
    if (line) 
        free(line);

    struct instruction* head = generateCodeRepresentation(mnemonics, operands, valid_labels, line_count); //creating the intermediate code structure
    showInstructionList(head);

    printf("Please look at the log file: %s for compilation details.\n", log_file_name);
    if (error_count > 0) {
        fprintf(flog, "Errors detected in asm file. Compilation aborted.\n");
        displayErrorList(flog);
        fclose(flog);
        return EXIT_FAILURE; //terminate the program, skipping the second pass
    } 
    else //running the second pass if no errors found
    {
        char base_name[50];
        strcpy(base_name, strtok(file_name, "."));
        secondPass(head, base_name);
        //Post-Second pass error handling
        if (error_count == 0) {
            char* token = strtok(file_name, ".");
            token = strtok(token, "_");
            fprintf(flog, "Code Compiled Successfully. Generated %s.lst and %s.bin files\n", token, token);
        } else {
            fprintf(flog, "Errors detected during second pass. Compilation failed.\n");
            displayErrorList(flog);
            fclose(flog);
            return EXIT_FAILURE;
        }
    }
    fclose(flog);
    return EXIT_SUCCESS;
}
