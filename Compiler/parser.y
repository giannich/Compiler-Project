%{
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "symboltab.h"
#include "tableStack.h"
#include "arithmeticStack.h"
#include "regStack.h"
#include "expressionLib.h"
#define SIZE 31
#define INT_SIZE 4

Symbol **globalTable;
Symbol **currentTable;
Symbol *lookupVar;
Stack_node *stackTable;

int stackPtr;
int maxPtrSize;
int retLabelCounter;
int ifLabelCounter;
int ifElseLabelCounter;
int whileLabelCounter;
int doLabelCounter;
int forLabelCounter;

  void yyerror(const char *str)
  {
    fprintf(stderr,"error: %s\n",str);
  }

  int yywrap()
  {
    return 1;
  }

  int main()
  {
    // This mallocs the arrays for the macros
    mallocArr();

    // This creates one global symbol table and a scope table
    globalTable = createTable();
    currentTable = (Symbol **) malloc(sizeof(Symbol) * SIZE);
    stackTable = initStack();

    lookupVar = (Symbol *) malloc(sizeof(Symbol));

    stackPtr = 0;
    maxPtrSize = 0;
    retLabelCounter = 0;
    ifLabelCounter = 0;
    ifElseLabelCounter = 0;
    whileLabelCounter = 0;
    doLabelCounter = 0;
    forLabelCounter = 0;

    yyparse();

    freeTable(globalTable);
    return 0;
  }

%}

%union 
{
  int integer;
  char *string;
}

%union
{
  struct
  {
    char *type;
    char *name;
    char *code;
    int argNum;
  } declaration;
}

%union
{
  struct
  {
    char *left;         // Important
    char *right;        // Useless?
    char *type;         // Useless?
    char *code;         // Important
    char *location;     // Important
    char *assType;      // Uselss
    char *comparison;   // Important
  } instruction;
}

%union
{
  struct
  {
    char *left;         // Useless
    char *right;        // Useless
    char *operation;    // Useless
    char *unary;        // Semi-useless
    int argNum;         // Important
    char *code;         // Important
  } expression;
}

%token IDENT
%token CONST_INT
%token CONST_STR

%token INT_TYPE
%token STR_TYPE
%token EXT_TYPE

%token INC_OP
%token DEC_OP
%token ADD_OP
%token SUB_OP
%token MUL_OP
%token DIV_OP
%token MOD_OP
%token ASS_OP
%token LSH_OP
%token RSH_OP

%token EQU_COMP
%token NEQ_COMP
%token LOE_COMP
%token MOE_COMP
%token LST_COMP
%token MRT_COMP

%token IF_CON
%token WHL_CON
%token DO_CON
%token FOR_CON
%token RET_CON
%token THN_CON

%token L_PAR
%token R_PAR
%token L_BRA
%token R_BRA
%token SEMIC
%token COMMA

%right "simple_if" ELS_CON

%type <string> program external_declaration type parameter_declaration block_start block_end comparison_operator  

%type <declaration> declarator declarator_list declaration function_definition declaration_list function_declarator parameter_list

%type <instruction> assignment condition cond_instruction instruction expression_instruction compound_instruction select_instruction iteration_instruction jump_instruction instruction_list

%type <expression> expression expression_additive expression_multiplicative unary_expression expression_postfixee primary_expression argument_expression_list

%type <string> IDENT CONST_INT CONST_STR INT_TYPE STR_TYPE EXT_TYPE INC_OP DEC_OP ADD_OP SUB_OP MUL_OP DIV_OP ASS_OP LSH_OP RSH_OP EQU_COMP NEQ_COMP LOE_COMP MOE_COMP LST_COMP MRT_COMP IF_CON WHL_CON DO_CON FOR_CON RET_CON THN_CON L_PAR R_PAR L_BRA R_BRA SEMIC COMMA

%%

/**********************************************
*                                             *
*   NOT IMPLEMENTED                           *
*       - STRING HANDLING                     *
*       - TYPE CHECKING                       *
*       - PRINT ASM IN FILE                   *
*                                             *
*   IMPLEMENTED                               *
*       - MODULO OPERATION                    *
*       - PARAMETER PASSING                   *
*       - FUNCTION CALLS                      *
*       - FUNCTION PARAMETERS CALLED          *
*       - FIX STACK POINTERS AT EVERY FUNC    *
*       - CONSTANT FOLDING OPTIMIZATION       *
*       - INTEGER DIVISION                    *
*       - EXPRESSION/ASSIGNMENT INSTRUCTION   *
*       - JUMP INSTRUCTION                    *
*       - SELECT INSTRUCTION                  *
*       - FUNCTION DEFIINITION                *
*       - ITERATOR INSTRUCTION                *
*                                             *
**********************************************/

/*********************
* DECLARATIONS BLOCK *
*********************/

program :  
external_declaration
| program external_declaration 
;

external_declaration:  
declaration                                 /* GLOBAL VARIABLES AND FUNCTIONS HERE */
{

  //stackPtr -= INT_SIZE;
  //maxPtrSize += INT_SIZE;                                             

  if (!strcmp($1.type, "function, int"))
    insertSymbol(globalTable, $1.name, "function, int", "global", stackPtr);
  else if (!strcmp($1.type, "function, string"))
    insertSymbol(globalTable, $1.name, "function, string", "global", stackPtr);
  else if (!strcmp($1.type, "int"))
    insertSymbol(globalTable, $1.name, "int", "global", stackPtr);
  else if (!strcmp($1.type, "string"))
    insertSymbol(globalTable, $1.name, "string", "global", stackPtr);
  else
  {
    //stackPtr += INT_SIZE;
    //maxPtrSize -= INT_SIZE;   
    printf("Error: type does not match in external_declaration->declaration.\n");
  }

}
| EXT_TYPE declaration                /* EXTERN VARIABLES AND FUNCTIONS HERE */
{                                

  //stackPtr -= INT_SIZE;
  //maxPtrSize += INT_SIZE;      

  if (!strcmp($2.type, "function, int"))
    insertSymbol(globalTable, $2.name, "function, int", "extern", stackPtr);
  else if (!strcmp($2.type, "function, string"))
    insertSymbol(globalTable, $2.name, "function, string", "extern", stackPtr);
  else if (!strcmp($2.type, "variable, int"))
    insertSymbol(globalTable, $2.name, "int", "extern", stackPtr);
  else if (!strcmp($2.type, "variable, string"))
    insertSymbol(globalTable, $2.name, "string", "extern", stackPtr);
  else
  {
    //stackPtr += INT_SIZE;
    //maxPtrSize -= INT_SIZE;
    printf("Error: type does not match in external_declaration->EXT_TYPE declaration.\n");
  }

}                               
| function_definition                       /* GLOBAL FUNCTIONS HERE */
{

  //stackPtr -= INT_SIZE;
  //maxPtrSize += INT_SIZE;      

  if (!strcmp($1.type, "int"))
    insertSymbol(globalTable, $1.name, "function, int", "global", stackPtr);
  else if (!strcmp($1.type, "string"))
    insertSymbol(globalTable, $1.name, "function, string", "global", stackPtr);
  else
  {
    //stackPtr += INT_SIZE;
    //maxPtrSize -= INT_SIZE;
    printf("Error: type does not match in external_declaration->function_declaration.\n");
  }

}
;

/***************************
* FUNCTION DEFINITION HERE *
***************************/

function_definition :                                       
type function_declarator compound_instruction
{          
  int i;
  char *argReg;
  int argNum = $2.argNum;

  $$.name = $2.name;
  $$.type = $1;

  // This segment is to make sure that we are allocating a multiple of 16 bits of code
  int rem = maxPtrSize % 16;
  if (rem != 0)
  {
    stackPtr -= 16 - rem;
    maxPtrSize += 16 - rem;
  }

  // Set global variable and create function label
  printf("\t.globl\t_%s\n", $2.name);
  printf("_%s:\n", $2.name);

  // Set save current rbp into stack, copy rsp into rpb, and set new rsp
  printf("\tpushq\t%%rbp\n");
  printf("\tmovq\t%%rsp, %%rbp\n");
  printf("\tsubq\t$%d, %%rsp\n", maxPtrSize);

  // Move the arguments to the stack
  for (i = 0; i < argNum; i++)
  {
    argReg = popArgumentRegister(i);
    printf("\tmovl\t%s, -%d(%%rbp)\n", argReg, (i + 1) * 4);
  }

  // Propagate instruction code
  printf("%s\n", $3.code);

  // Need to reset the ptrs...
  stackPtr = 0;
  maxPtrSize = 0;
}
;

// Important Note: apparently, the grammar allows the declaration and assignment on the same line, but recognizes it as an assignment only

declaration :  
type declarator_list SEMIC {

  if (!strcmp($2.type, "function"))
  {
    if (!strcmp($1, "int"))
      {
        $$.name = $2.name;
        $$.type = "function, int";
      }
    else if (!strcmp($1, "string"))
      {
        $$.name = $2.name;
        $$.type = "function, string";
      }
  }

  else if (!strcmp($2.type, "variable"))
  {
    if (!strcmp($1, "int"))
      {
        $$.name = $2.name;
        $$.type = "int";
      }
    else if (!strcmp($1, "string"))
      {
        $$.name = $2.name;
        $$.type = "string";
      }
  }

}
;

type :  
INT_TYPE {$$ = "int";}                                          // set INT
| STR_TYPE {$$ = "string";}                                     // set String
;

declarator_list :  
declarator {                                                    // Propagate code

  $$.name = $1.name;
  $$.type = $1.type;
  // Initialize ASM code here
  // But how do we know if it is going to local or global/external?
  // Bubble up the name and type then!

}                                    
| declarator_list COMMA declarator
{
  $$.name = $3.name;
  $$.type = $3.type;
  // Need to add to the name and type using strcat??
} 
;

declaration_list :  
declaration                            /* SINGLE LOCAL VARIABLES HERE */
{          

  stackPtr -= INT_SIZE;
  maxPtrSize += INT_SIZE;      

  if (!strcmp($1.type, "int"))
  {
    insertSymbol(currentTable, $1.name, $1.type, "local", stackPtr);
    // Print ASM code here for local variables

    // DOES NOT PRINT ANYTHING, IT JUST ACKNOWLEDGES ITS EXISTANCE 

    // First test here:
    //printf("\tmovq \t$%s, %d(%%rbp)\n", $1.name, stackPtr);
  }
  else if (!strcmp($1.type, "string"))
  {
    insertSymbol(currentTable, $1.name, $1.type, "local", stackPtr);
    // Print ASM code here for local variables
  }
  else
  {
    stackPtr += INT_SIZE;
    maxPtrSize -= INT_SIZE;
    printf("Error: type does not match in declaration_list->declaration.\n");
  }

}
| declaration_list declaration          /* LOCAL VARIABLES LIST HERE */
{

  stackPtr -= INT_SIZE;
  maxPtrSize += INT_SIZE;      

  if (!strcmp($2.type, "int"))
  {
    insertSymbol(currentTable, $2.name, $2.type, "local", stackPtr);
    // Print ASM code here for local variables
  }
  else if (!strcmp($2.type, "string"))
  {
    insertSymbol(currentTable, $2.name, $2.type, "local", stackPtr);
    // Print ASM code here for local variables
  }
  else
  {
    stackPtr += INT_SIZE;
    maxPtrSize -= INT_SIZE;
    printf("Error: type does not match in declaration_list->declaration_list declaration.\n");
  }

}
;

declarator :  
IDENT {                                                       // Create Variable

  $$.name = $1;
  $$.type = "variable";

}
| function_declarator {                                       // Create Function
  
  $$.name = $1.name;
  $$.type = "function";

}                                   
;

/****************************
* FUNCTION PARAMETERS: DONE *
****************************/

function_declarator :  
IDENT L_PAR R_PAR
{
  $$.name = $1;
  $$.argNum = 0;
}
| IDENT L_PAR parameter_list R_PAR
{
  $$.name = $1;
  $$.argNum = $3.argNum;
}
;

parameter_list :  
parameter_declaration
{
  $$.argNum = 1;
} 
| parameter_list COMMA parameter_declaration
{
  $$.argNum = $1.argNum + 1;
}
;

parameter_declaration :  
type IDENT                                                    /* FUNCTION PARAMETERS HERE */
{
  stackPtr -= INT_SIZE;
  maxPtrSize += INT_SIZE;    

  insertSymbol(currentTable, $2, $1, "function parameter", stackPtr);

}
;

/*********************
* INSTRUCTIONS BLOCK *
*********************/

instruction :  
SEMIC
{
  // empty instruction
  $$.code = "";
}  
| compound_instruction 
{
  $$.code = $1.code;
}
| expression_instruction  
{
  $$.code = $1.code;
}
| iteration_instruction  
{
  $$.code = $1.code;
}
| select_instruction  
{
  $$.code = $1.code;
}
| jump_instruction 
{
  $$.code = $1.code;
}
;

/***********************************************
* EXPRESSION INSTRUCTION: PARTIALLY DONE       *
* NEED TYPE CHECKING                           *
* NEED GLOBAL POINTING                         *
***********************************************/

expression_instruction :              
expression SEMIC 
{
  // Only void functions are evaluated here
  $$.code = $1.code;
}
| assignment SEMIC {

  // Most of the code is done in assignment
  $$.code = $1.code;
}                            
;

/***********************************
* ASSIGNMENT: PARTIALLY DONE       *
* NEED TO CHECK FOR TYPES          *
***********************************/

// Modified the assignment semantics to include 'int i = 0' instead of just 'i = 0'
assignment :  
IDENT ASS_OP expression
{         
  $$.left = $1;
  $$.right = $3.unary;
  $$.assType = "simple";

  /********************************
  * Assumptions:                  *
  *   Worst Case: 0 Reg Division  *
  *   67 chars for base           *
  *   20 chars for registers      *
  *   20 chars for variables      *
  *   8 more chars for padding    *
  *   115 total char size         *
  *   2300 total chars for 20 Ops *
  ********************************/

  // Mallocs the codeBuffer
  char *codeBuffer = (char *) malloc(sizeof(char) * 2300);

  // Calculates the location of the final expression and fills codeBuffer
  char *location = computeExpression(codeBuffer, $3.code, currentTable, globalTable, stackTable);

  /*****************************
  * Assumptions:               *
  *   25 chars for base        *
  *   4 chars for offset       *
  *   20 chars for location    *
  *   8 chars for reg          *
  *   8 more chars for padding *
  *   65 total char size       *
  *****************************/

  // Extra code buffer malloc
  char* extraCodeBuffer = (char *) malloc(strlen(codeBuffer) + 65);

  strcpy(extraCodeBuffer, codeBuffer);
  computeAssignment(extraCodeBuffer, $1, location, currentTable, globalTable, stackTable);

  $$.code = extraCodeBuffer;
  flushRegisters();
  free(codeBuffer);

}
| type IDENT ASS_OP expression
{
  $$.left = $2; 
  $$.right = $3;
  $$.assType = "declaration";

  stackPtr -= INT_SIZE;
  maxPtrSize += INT_SIZE;   

  // If the symbol is not in local and global tables
  if ( insertSymbol(currentTable, $2, $1, "local", stackPtr) == 0 && lookupSymbol(globalTable, $2) == NULL )
  {
    // Mallocs the codeBuffer
    char *codeBuffer = (char *) malloc(sizeof(char) * 2300);

    // Calculates the location of the final expression and fills codeBuffer
    char *location = computeExpression(codeBuffer, $4.code, currentTable, globalTable, stackTable);

    // Mallocs the extraCodeBuffer
    char *extraCodeBuffer = (char *) malloc(strlen(codeBuffer) + 65);

    strcpy(extraCodeBuffer, codeBuffer);
    computeAssignment(extraCodeBuffer, $2, location, currentTable, globalTable, stackTable);

    $$.code = extraCodeBuffer;
    flushRegisters();
    free(codeBuffer);
  }
  else
  {
    stackPtr += INT_SIZE;
    maxPtrSize -= INT_SIZE;
    printf("Error: cannot assign to [%s] as it is already declared in local or global scope\n", $2);
    $$.code = $4.code;
  }
}
;

/*******
* DONE *
*******/

compound_instruction :  
block_start declaration_list instruction_list block_end 
{
  // No need to add declaration_list's code since it doesn't print any ASM instructions
  $$.code = $3.code;
}
| block_start declaration_list block_end
{
  // No need to add declaration_list code
  $$.code = "";
} 
| block_start instruction_list block_end 
{
  $$.code = $2.code;
}
| block_start block_end 
{
  // Nothing to add here...
  $$.code = "";
}
;

block_start :  
L_BRA
{
  // Pushes the currentTable into the stack if it is not NULL, then creates a new local symbol table
  if (currentTable != NULL)
    pushTable(stackTable, currentTable);
  currentTable = createTable();
}
;

block_end :  
R_BRA
{
  free(currentTable);
  currentTable = popTable(stackTable);
}
;

/*******
* DONE *
*******/

instruction_list :  
instruction  
{
  // Since it is the first in the grammar, just sets the code here
  $$.code = $1.code;
}
| instruction_list instruction 
{
  // Since it is a continuation of the code, it appends the stuff here
  char *code = malloc(strlen($1.code) + strlen($2.code));
  strcpy(code, $1.code);
  strcat(code, $2.code);

  //printf("INSTRUCTION 1 HERE\n%sINSTRUCTION 1 DONE\n", $1.code);
  //printf("INSTRUCTION 2 HERE\n%sINSTRUCTION 2 DONE\n", $2.code);
  //printf("INSTRUCTION 3 HERE\n%sINSTRUCTION 3 DONE\n", code);

  $$.code = code;
}
;

/***************************
* SELECT INSTRUCTION: DONE *
***************************/

select_instruction :
cond_instruction instruction
{    
  /******************************
  * Assumptions:                *
  *   13 chars for base         *
  *   3 chars for comparison op *  
  *   56 chars for labels       *
  *   8 more chars for padding  *
  *   80 total charsize         *
  ******************************/

  // Code Buffer Malloc
  char *codeBuffer = (char *) malloc(strlen($1.code) + strlen($2.code) + 80);

  computeSelect(codeBuffer, ifLabelCounter, $1.code, $2.code, $1.comparison, 0);

  $$.code = codeBuffer;

  ifLabelCounter++;

} %prec "simple_if"
| cond_instruction instruction ELS_CON instruction
{  
  /******************************
  * Assumptions:                *
  *   22 chars for base         *
  *   3 chars for comparison op *  
  *   88 chars for labels       *
  *   7 more chars for padding  *
  *   120 total charsize        *
  ******************************/

  // Code Buffer Malloc
  char *codeBuffer = (char *) malloc(strlen($1.code) + strlen($2.code) + strlen($4.code) + 120);

  char *labelCont = (char *) malloc(sizeof(char) * 14);
  labelCont = computeSelect(codeBuffer, ifElseLabelCounter, $1.code, $2.code, $1.comparison, 1);

  strcat(codeBuffer, $4.code);
  strcat(codeBuffer, labelCont);
  strcat(codeBuffer, ":\n");

  $$.code = codeBuffer;

  ifElseLabelCounter++;
  free(labelCont);
}
;

/*************************
* COND INSTRUCTION: DONE *
*************************/

cond_instruction :  
IF_CON L_PAR condition R_PAR 
{ 
  $$.code = $3.code;
  $$.comparison = $3.comparison;
} 
;

/********************************
* ITERATION INSTRUCTION: DONE   *
* NEED TO TIDY UP THE FUNCTIONS *
********************************/

iteration_instruction :
WHL_CON L_PAR condition R_PAR instruction
{

  /***********************************************
  * WHILE LOOP STRUCTURE:                        *
  *    While Loop Label                          *
  *    -> Condition code                         *
  *    Inverse Compararer Jump to Continue Label *
  *    -> Instruction code                       *
  *    Unconditional Jump to While Loop Label    *
  *    Continue Label                            *
  ***********************************************/

  /******************************
  * Assumptions:                *
  *   13 chars for base         *
  *   3 chars for comparison op *  
  *   72 chars for labels       *
  *   12 more chars for padding *
  *   100 total charsize        *
  ******************************/

  // Code Buffer Malloc
  char *codeBuffer = (char *) malloc(strlen($3.code) + strlen($5.code) + 100);

  // Base 15 + 3 chars for counter
  char *whileLabel = (char *) malloc(18);
  char *contLabel = (char *) malloc(18);

  sprintf(whileLabel, "whileLoopLabel_%d", whileLabelCounter);
  sprintf(contLabel, "whileContLabel_%d", whileLabelCounter);

  // While Loop Label
  strcpy(codeBuffer, whileLabel);
  strcat(codeBuffer, ":\n");
  
  // Condition Code
  strcat(codeBuffer, $3.code);

  // Inverse Comparer, jump to ContLabel
  strcat(codeBuffer, "\t");
  strcat(codeBuffer, inverseComp($3.comparison));
  strcat(codeBuffer, "\t");
  strcat(codeBuffer, contLabel);
  strcat(codeBuffer, "\n");

  // Instruction Code
  strcat(codeBuffer, $5.code);

  // Unconditional Jump to While Loop Label
  strcat(codeBuffer, "\tjmp\t");
  strcat(codeBuffer, whileLabel);
  strcat(codeBuffer, "\n");

  // Continue Label
  strcat(codeBuffer, contLabel);
  strcat(codeBuffer, ":\n");

  $$.code = codeBuffer;

  free(whileLabel);
  free(contLabel);

  // Increment whileLabelCounter
  whileLabelCounter++;

}
| DO_CON instruction WHL_CON L_PAR condition R_PAR
{

  /**************************************
  * DO LOOP STRUCTURE:                  *
  *    Do Loop Label                    *
  *    -> Instruction code              *
  *    -> Condition code                *
  *    Comparison Jump to Do Loop Label *
  **************************************/

  /******************************
  * Assumptions:                *
  *   5 chars for base          *
  *   3 chars for comparison op *  
  *   30 chars for labels       *
  *   7 more chars for padding  *
  *   45 total charsize         *
  ******************************/

  // Code Buffer Malloc
  char *codeBuffer = (char *) malloc(strlen($2.code) + strlen($5.code) + 45);

  // Base 12 + 3 chars for counter
  char *doLabel = (char *) malloc(15);

  sprintf(doLabel, "doLoopLabel_%d", doLabelCounter);

  // Do Loop Label
  strcpy(codeBuffer, doLabel);
  strcat(codeBuffer, ":\n");

  // Instruction Code
  strcat(codeBuffer, $2.code);
  
  // Condition Code
  strcat(codeBuffer, $5.code);

  // Comparison, jump to Do Loop Label
  strcat(codeBuffer, "\t");
  strcat(codeBuffer, $5.comparison);
  strcat(codeBuffer, "\t");
  strcat(codeBuffer, doLabel);
  strcat(codeBuffer, "\n");

  $$.code = codeBuffer;
  free(doLabel);

  // Increment doLabelCounter
  doLabelCounter++;
} 

/*******************************
* NOTE: UNARY OPS ARE DISABLED *
*******************************/

| FOR_CON L_PAR assignment SEMIC condition SEMIC assignment R_PAR instruction
{
  /***********************************************
  * FOR LOOP STRUCTURE:                          *
  *    -> Assignment code (assignment)           *
  *    For Loop Label                            *
  *    -> Condition code                         *
  *    Inverse Compararer Jump to Continue Label *
  *    -> Instruction code                       *
  *    -> Assignment code (increment)            *
  *    Unconditional Jump to For Loop Label      *
  *    Continue Label                            *
  ***********************************************/

  /******************************
  * Assumptions:                *
  *   13 chars for base         *
  *   3 chars for comparison op *  
  *   64 chars for labels       *
  *   5 more chars for padding  *
  *   85 total charsize         *
  ******************************/

  // Code Buffer Malloc
  char *codeBuffer = (char *) malloc(strlen($3.code) + strlen($5.code) + strlen($7.code) + strlen($9.code) + 85);

  // Base 13 + 3 chars for counter
  char *forLabel = (char *) malloc(16);
  char *contLabel = (char *) malloc(16);

  sprintf(forLabel, "forLoopLabel_%d", forLabelCounter);
  sprintf(contLabel, "forContLabel_%d", forLabelCounter);

  // Assignment Code (assignment)
  strcpy(codeBuffer, $3.code);

  // For Loop Label
  strcat(codeBuffer, forLabel);
  strcat(codeBuffer, ":\n");
  
  // Condition Code
  strcat(codeBuffer, $5.code);

  // Inverse Comparer, jump to ContLabel
  strcat(codeBuffer, "\t");
  strcat(codeBuffer, inverseComp($5.comparison));
  strcat(codeBuffer, "\t");
  strcat(codeBuffer, contLabel);
  strcat(codeBuffer, "\n");

  // Instruction Code
  strcat(codeBuffer, $9.code);

  // Assignment Code (increment)
  strcat(codeBuffer, $7.code);

  // Unconditional Jump to For Loop Label
  strcat(codeBuffer, "\tjmp\t");
  strcat(codeBuffer, forLabel);
  strcat(codeBuffer, "\n");

  // Continue Label
  strcat(codeBuffer, contLabel);
  strcat(codeBuffer, ":\n");

  $$.code = codeBuffer;

  free(forLabel);
  free(contLabel);

  // Increment label counter
  forLabelCounter++;
} 
;

/*************************
* JUMP INSTRUCTION: DONE *
*************************/

jump_instruction:  
RET_CON expression SEMIC
{
  // WARNING: ASSUMING ONLY ONE RETURN STATEMENT PER FUNCTION

  // Modifies the maxPtrSize so that it is always a multiple of 16
  int rem = maxPtrSize % 16;
  if (rem != 0)
  {
    stackPtr -= 16 - rem;
    maxPtrSize += 16 - rem;
  }

  // Mallocs the size needed for the maxPtrSize number
  char *maxPtrString = (char *) malloc(sizeof(char) * floor(maxPtrSize/10));
  sprintf(maxPtrString, "%d", maxPtrSize);

  // Produces the code from the expression if there is any
  char* codeBuffer = (char *) malloc(sizeof(char) * 2300);
  char *location = computeExpression(codeBuffer, $2.code, currentTable, globalTable, stackTable);

  // Finally mallocs the space needed for the code...
  char *code = (char *) malloc(strlen(codeBuffer) + strlen(location) + strlen(maxPtrString) + 58);

  // Label gets produced here
  // Assumption: max number of return labels is 999
  char *retNum = (char *) malloc(sizeof(char) * 3);
  sprintf(retNum, "%d", retLabelCounter);
  retLabelCounter++;

  strcpy(code, "retlabel_");
  strcat(code, retNum);
  strcat(code, ":\n");

  // The return value is computed here
  strcat(code, codeBuffer);

  // If the final location is not in eax...
  if (strcmp(location, "%eax") != 0)
  {
    strcat(code, "\tmovl\t");
    strcat(code, location);
    strcat(code, ", %eax\n");
  }

  // Rest of the code is concatenated...
  strcat(code, "\taddq\t$");
  strcat(code, maxPtrString);
  strcat(code, ", %rsp\n");
  strcat(code, "\tpopq\t%rbp\n");
  strcat(code, "\tretq\n");

  $$.code = code;

  // Liberate eax
  freeRegister("%eax");
} 
;

/**********************
* CONDITIONS BLOCK    *
* NEED: TYPE CHECKING *
**********************/

condition :  
expression comparison_operator expression 
{
  $$.left = $1.unary;
  $$.right = $3.unary;
  $$.type = $2;

  // Mallocs the codeBuffers
  char *codeBufferExp1 = (char *) malloc(sizeof(char) * 2300);
  char *codeBufferExp2 = (char *) malloc(sizeof(char) * 2300);

  // Calculates the location of the final expression and fills codeBuffer
  char *locationExp1 = computeExpression(codeBufferExp1, $1.code, currentTable, globalTable, stackTable);
  char *locationExp2 = computeExpression(codeBufferExp2, $3.code, currentTable, globalTable, stackTable);

  // Puts the first expression in the register %edi no matter whether it is a constant, variable, or in a register
  char *loc1 = (char *) malloc(sizeof(char) * 4);
  loc1 = nextRegister();

  char *extraCodeBufferExp1 = (char *) malloc(strlen(codeBufferExp1) + 65);
  strcpy(extraCodeBufferExp1, codeBufferExp1);
  computeAssignment(extraCodeBufferExp1, loc1, locationExp1, currentTable, globalTable, stackTable);

  // Declare the extraCodeBufferExp2 and loc2
  char *extraCodeBufferExp2;
  char *loc2;

  // If expression 2 is in a register, do same as above and put it in register %esi
  if (isRegister(locationExp2))
  {
    loc2 = (char *) malloc(sizeof(char) * 4);
    loc2 = nextRegister();

    extraCodeBufferExp2 = (char *) malloc(strlen(codeBufferExp2) + 65);
    strcpy(extraCodeBufferExp2, codeBufferExp2);
    computeAssignment(extraCodeBufferExp2, loc2, locationExp2, currentTable, globalTable, stackTable);
  }
  // If expression 2 is a variable or a constant
  else
  {
    // The expression is empty for that since no operations are performed
    extraCodeBufferExp2 = "";

    // The location is already spat out from computeExpression, and checked if it is a valid variable
    loc2 = locationExp2;
  }

  /**********************************
  * Assumptions:                    *
  *   8 chars for base              *
  *   4 chars for register in loc 1 *  
  *   3 more chars for padding      *
  *   15 total charsize             *
  **********************************/

  // Final code buffer malloc
  char *finalCodeBuffer = (char *) malloc(strlen(extraCodeBufferExp1) + strlen(extraCodeBufferExp2) + strlen(loc2) + 15);

  strcpy(finalCodeBuffer, extraCodeBufferExp1);
  strcat(finalCodeBuffer, extraCodeBufferExp2);

  strcat(finalCodeBuffer, "\tcmp\t");
  strcat(finalCodeBuffer, loc2);
  strcat(finalCodeBuffer, ", ");
  strcat(finalCodeBuffer, loc1);
  strcat(finalCodeBuffer, "\n");

  $$.comparison = $2;
  $$.code = finalCodeBuffer;

  freeRegister(loc1);
  if (isRegister(loc2))
  {
    freeRegister(loc2);
    free(extraCodeBufferExp2);
  }

  // Free the remaining pointers
  free(codeBufferExp1);
  free(codeBufferExp2);
  free(locationExp1);
  free(locationExp2);
  free(extraCodeBufferExp1);
}
;

comparison_operator :  
EQU_COMP {$$ = "je";}
| NEQ_COMP {$$ = "jne";}
| LOE_COMP {$$ = "jle";}
| MOE_COMP {$$ = "jge";}
| LST_COMP {$$ = "jl";}
| MRT_COMP {$$ = "jg";}
;

/**************************
* EXPRESSIONS BLOCK: DONE *
**************************/

expression :  
expression_additive
{
  $$.code = $1.code;
}
| expression LSH_OP expression_additive
{
  char *code = malloc(strlen($1.code) + strlen($3.code) + 6);

  strcpy(code, $1.code);
  strcat(code, " ");
  strcat(code, $3.code);
  strcat(code, " lsh ");

  $$.code = code;  
}
| expression RSH_OP expression_additive
{
  char *code = malloc(strlen($1.code) + strlen($3.code) + 6);

  strcpy(code, $1.code);
  strcat(code, " ");
  strcat(code, $3.code);
  strcat(code, " rsh ");

  $$.code = code;  
}
;

expression_additive :  
expression_multiplicative 
{
  $$.code = $1.code;
  $$.unary = $1.unary;
}
| expression_additive ADD_OP expression_multiplicative
{
  char *code = malloc(strlen($1.code) + strlen($3.code) + 6);

  strcpy(code, $1.code);
  strcat(code, " ");
  strcat(code, $3.code);
  strcat(code, " add");

  $$.left = $1.unary;
  $$.right = $3.unary;
  $$.operation = "add";
  $$.code = code;

}
| expression_additive SUB_OP expression_multiplicative
{
  char *code = malloc(strlen($1.code) + strlen($3.code) + 6);

  strcpy(code, $1.code);
  strcat(code, " ");
  strcat(code, $3.code);
  strcat(code, " sub");

  $$.left = $1.unary;
  $$.right = $3.unary;
  $$.operation = "sub";
  $$.code = code;
}
;

expression_multiplicative :  
unary_expression 
{
  $$.code = $1.code;
  $$.unary = $1.unary;
}
| expression_multiplicative MUL_OP unary_expression
{
  char *code = malloc(strlen($1.code) + strlen($3.code) + 7);

  strcpy(code, $1.code);
  strcat(code, " ");
  strcat(code, $3.code);
  strcat(code, " imul");

  $$.left = $1.unary;
  $$.right = $3.unary;
  $$.operation = "imul";
  $$.code = code;
}
| expression_multiplicative DIV_OP unary_expression
{
  char *code = malloc(strlen($1.code) + strlen($3.code) + 6);

  strcpy(code, $1.code);
  strcat(code, " ");
  strcat(code, $3.code);
  strcat(code, " idiv");

  $$.left = $1.unary;
  $$.right = $3.unary;
  $$.operation = "idiv";
  $$.code = code;
}
| expression_multiplicative MOD_OP unary_expression
{
  char *code = malloc(strlen($1.code) + strlen($3.code) + 6);

  strcpy(code, $1.code);
  strcat(code, " ");
  strcat(code, $3.code);
  strcat(code, " imod");

  $$.left = $1.unary;
  $$.right = $3.unary;
  $$.operation = "imod";
  $$.code = code;
}
;

unary_expression:                                     
expression_postfixee 
{
  $$.code = $1.code;
  $$.unary = $1.unary;
}
| SUB_OP unary_expression      
{
  $$.code = $2.code;
  $$.unary = $2.unary;
}                                                
;

/**************************
* FUNCTION CALL: DONE     *
* NOTE: FUNC IS A KEYWORD *
**************************/

expression_postfixee :  
primary_expression 
{
  $$.code = $1.code;
  $$.unary = $1.unary;
}
| IDENT L_PAR argument_expression_list R_PAR
{
  // Assuming that the number of arguments does not exceed like 4?
  char *codeBuffer = (char *) malloc(strlen($1) + strlen($3.code) + 12);
  sprintf(codeBuffer, "func %s %d $ %s $ ", $1, $3.argNum, $3.code);
  $$.code = codeBuffer;
}
| IDENT L_PAR R_PAR 
{
  char *codeBuffer = (char *) malloc(strlen($1) + 10);
  sprintf(codeBuffer, "func %s 0 $ ", $1);
  $$.code = codeBuffer;
}
;

argument_expression_list:  
expression 
{
  // Bubbles up the code from expression
  $$.code = $1.code;

  // Sets the number of arguments to 1
  $$.argNum = 1;
}
| argument_expression_list COMMA expression 
{
  if ($$.argNum < 4)
  {
    // Mallocs enough space for the code
    char *code = malloc(strlen($1.code) + strlen($3.code) + 3);

    // Bubbles up the code from the expression
    strcpy(code, $1.code);
    strcat(code, " $ ");
    strcat(code, $3.code);

    $$.code = code;
  
    // Adds 1 to the number of arguments
    $$.argNum = $1.argNum + 1;
  }

  else
  {
    printf("Warning, this compiler so far only supports up to 4 arguments per function. It will stop taking any argument beyond the 4th one.\n");
    $$.code = $1.code;
    $$.argNum = $1.argNum;
  }

}
;

/*************************
* NEED TO HANDLE STRINGS *
*************************/

primary_expression :  
IDENT 
{
  $$.code = $1;
  $$.unary = $1;
}
| CONST_INT   
{
  $$.code = $1;
  $$.unary = $1;
}
| CONST_STR 
{
  $$.code = $1;
  $$.unary = $1;
}
| L_PAR expression R_PAR
{
  $$.unary = $2.unary;
  $$.code = $2.code;
}
;

%%