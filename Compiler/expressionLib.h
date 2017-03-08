#include <stdlib.h>
#include <stdio.h>

Symbol *checkVariable(char *lookupVar, Symbol **currentTable, Symbol **globalTable, Stack_node *stackTable);
char *computeExpression(char *codeBuffer, char *expressionCode, Symbol **currentTable, Symbol **globalTable, Stack_node *stackTable);
int computeAssignment(char *codeBuffer, char *left, char *location, Symbol **currentTable, Symbol **globalTable, Stack_node *stackTable);
char *computeSelect(char *codeBuffer, int labelCounter, char *compCode, char *trueInstruction, char *comparison, int complex);
char *inverseComp(char *comp);
char *removeConstSymbol(char *str);

Symbol *checkVariable(char *lookupVar, Symbol **currentTable, Symbol **globalTable, Stack_node *stackTable)
{
  Stack_node *tempStackTable = initStack();
  Symbol *retSymbol = lookupSymbol(currentTable, lookupVar);

  // Loop through the scopes
  while (retSymbol == NULL)
  {

    // As long as we do not reach the end of the scopes, just go through up one more scope
    if (!isEmpty(stackTable))
    {
      pushTable(tempStackTable, currentTable);
      currentTable = popTable(stackTable);
      retSymbol = lookupSymbol(currentTable, lookupVar);
    }

    // Else, look into the global table
    else
    {
      retSymbol = lookupSymbol(currentTable, lookupVar);

      // If it is still NULL, then it is not present anywhere!
      if (retSymbol == NULL)
        return NULL;
    }
  }

  // Refill the stackTable to its original state
  while(!isEmpty(tempStackTable))
  {
    pushTable(stackTable, currentTable);
    currentTable = popTable(tempStackTable);
  }

  return retSymbol;
}

char *computeExpression(char *codeBuffer, char *expressionCode, Symbol **currentTable, Symbol **globalTable, Stack_node *stackTable)
{
  Arithmetic *opStack = initArithmeticStack();
  initRegisterStack();

  strcpy(codeBuffer, "");

  char *token = strtok(expressionCode, " ");

  while ( token != NULL )
  {
      // If it is an operation -> addition, subtraction, multiplication, division
      if ( !strcmp(token, "add") || !strcmp(token, "sub") || !strcmp(token, "imul") || !strcmp(token, "idiv") || !strcmp(token, "imod") )
      {
        //printf("Operation token [%s] found\n", token);
        Arithmetic tok2 = popArithmetic(opStack);
        Arithmetic tok1 = popArithmetic(opStack);

        // 2 Register Operation -> Left Register, Right Register
        if ( strcmp(tok1.type, "register") == 0 && strcmp(tok2.type, "register") == 0 )
        {
          //printf("2 register operation\n");

          // If it is an addition, multiplication, or a subtraction
          if ( strcmp(token, "add") == 0 || strcmp(token, "imul") == 0 || strcmp(token, "sub") == 0)
          {
            strcat(codeBuffer, "\t");
            strcat(codeBuffer, token);
            strcat(codeBuffer, "l\t");
            strcat(codeBuffer, tok2.id);
            strcat(codeBuffer, ", ");
            strcat(codeBuffer, tok1.id);
            strcat(codeBuffer, "\n");
            freeRegister(tok2.id);

            pushArithmetic(opStack, tok1.id, "register");
          }

          // Else if it is a division
          else if (strcmp(token, "idiv") == 0)
          {
            char *tempReg = nextRegister();

            // Move eax into tempReg
            strcat(codeBuffer, "\tmovl\t%eax, ");
            strcat(codeBuffer, tempReg);
            strcat(codeBuffer, "\n");

            // Move token 1 (register) into eax
            strcat(codeBuffer, "\tmovl\t");
            strcat(codeBuffer, tok1.id);
            strcat(codeBuffer, ", %eax\n");

            // Divide eax by token 2 (register)
            strcat(codeBuffer, "\tidivl\t");
            strcat(codeBuffer, tok2.id);
            strcat(codeBuffer, "\n");

            // Move eax into token 1 (register)
            strcat(codeBuffer, "\tmovl\t%eax, ");
            strcat(codeBuffer, tok1.id);
            strcat(codeBuffer, "\n");

            // Move tempReg back into eax
            strcat(codeBuffer, "\tmovl\t");
            strcat(codeBuffer, tempReg);
            strcat(codeBuffer, ", %eax\n");

            // Free the temporary register and token 2 (register)
            freeRegister(tempReg);
            freeRegister(tok2.id);

            // Finall push tok1's register back into the stack
            pushArithmetic(opStack, tok1.id, "register");
          }

          // Else if it is a modulo division
          else if (strcmp(token, "imod") == 0)
          {
            int eaxBlocked = 0;
            int edxBlocked = 0;

            // Reserves eax and edx
            if (isRegisterFree("%eax"))
            {
              blockRegister("%eax");
              eaxBlocked = 1;
            }
            if (isRegisterFree("%edx"))
            {
              blockRegister("%edx");
              edxBlocked = 1;
            }

            char *tempRegEax = nextRegister();
            char *tempRegEdx = nextRegister();

            // Move eax into tempRegEax
            strcat(codeBuffer, "\tmovl\t%eax, ");
            strcat(codeBuffer, tempRegEax);
            strcat(codeBuffer, "\n");

            // Move edx into tempRegEdx
            strcat(codeBuffer, "\tmovl\t%edx, ");
            strcat(codeBuffer, tempRegEdx);
            strcat(codeBuffer, "\n");

            // Move token 1 (register) into eax
            strcat(codeBuffer, "\tmovl\t");
            strcat(codeBuffer, tok1.id);
            strcat(codeBuffer, ", %eax\n");

            // Divide eax by token 2 (register)
            strcat(codeBuffer, "\tidivl\t");
            strcat(codeBuffer, tok2.id);
            strcat(codeBuffer, "\n");

            // Move edx into token 1 (register)
            strcat(codeBuffer, "\tmovl\t%edx, ");
            strcat(codeBuffer, tok1.id);
            strcat(codeBuffer, "\n");

            // Move tempRegEax back into eax
            strcat(codeBuffer, "\tmovl\t");
            strcat(codeBuffer, tempRegEax);
            strcat(codeBuffer, ", %eax\n");

            // Move tempRegEdx back into edx
            strcat(codeBuffer, "\tmovl\t");
            strcat(codeBuffer, tempRegEdx);
            strcat(codeBuffer, ", %edx\n");

            // Free the temporary register and token 2 (register)
            freeRegister(tempRegEax);
            freeRegister(tempRegEdx);
            freeRegister(tok2.id);

            // Free eax and edx if they were blocked
            if (eaxBlocked)
              freeRegister("%eax");

            if (edxBlocked)
              freeRegister("%edx");

            // Finall push tok1's register back into the stack
            pushArithmetic(opStack, tok1.id, "register");
          }
        }

        // 1 Register Operation -> Left Register, Right Constant/Variable
        else if ( strcmp(tok1.type, "register") == 0 && strcmp(tok2.type, "register") != 0 )
        {
          //printf("1 left register operation\n");

          // If it is an addition, multiplication, or a subtraction
          if ( strcmp(token, "add") == 0 || strcmp(token, "imul") == 0 || strcmp(token, "sub") == 0)
          {
            strcat(codeBuffer, "\t");
            strcat(codeBuffer, token);
            strcat(codeBuffer, "l\t");
            strcat(codeBuffer, tok2.id);
            strcat(codeBuffer, ", ");
            strcat(codeBuffer, tok1.id);
            strcat(codeBuffer, "\n");

            pushArithmetic(opStack, tok1.id, "register");
          }

          // Else if it is a division
          else if (strcmp(token, "idiv") == 0)
          {
            char *tempReg = nextRegister();
            char *trashReg = nextRegister();

            // Move eax into tempReg
            strcat(codeBuffer, "\tmovl\t%eax, ");
            strcat(codeBuffer, tempReg);
            strcat(codeBuffer, "\n");

            // Move token 1 (register) into eax
            strcat(codeBuffer, "\tmovl\t");
            strcat(codeBuffer, tok1.id);
            strcat(codeBuffer, ", %eax\n");

            // Move token 2 (constant/variable) into trashReg
            strcat(codeBuffer, "\tmovl\t");
            strcat(codeBuffer, tok2.id);
            strcat(codeBuffer, ", ");
            strcat(codeBuffer, trashReg);
            strcat(codeBuffer, "\n");

            // Divide eax by trashReg
            strcat(codeBuffer, "\tidivl\t");
            strcat(codeBuffer, trashReg);
            strcat(codeBuffer, "\n");

            // Move eax into token 1 (register)
            strcat(codeBuffer, "\tmovl\t%eax, ");
            strcat(codeBuffer, tok1.id);
            strcat(codeBuffer, "\n");

            // Move tempReg back into eax
            strcat(codeBuffer, "\tmovl\t");
            strcat(codeBuffer, tempReg);
            strcat(codeBuffer, ", %eax\n");

            // Make tempReg and trashReg available again
            freeRegister(tempReg);
            freeRegister(trashReg);

            // Finall push tok1's register back into the stack
            pushArithmetic(opStack, tok1.id, "register");
          }

          // Else if it is a modulo division
          else if (strcmp(token, "imod") == 0)
          {
            int eaxBlocked = 0;
            int edxBlocked = 0;

            // Reserves eax and edx
            if (isRegisterFree("%eax"))
            {
              blockRegister("%eax");
              eaxBlocked = 1;
            }
            if (isRegisterFree("%edx"))
            {
              blockRegister("%edx");
              edxBlocked = 1;
            }

            char *tempRegEax = nextRegister();
            char *tempRegEdx = nextRegister();
            char *trashReg = nextRegister();

            // Move eax into tempRegEax
            strcat(codeBuffer, "\tmovl\t%eax, ");
            strcat(codeBuffer, tempRegEax);
            strcat(codeBuffer, "\n");

            // Move edx into tempRegEdx
            strcat(codeBuffer, "\tmovl\t%edx, ");
            strcat(codeBuffer, tempRegEdx);
            strcat(codeBuffer, "\n");

            // Move token 1 (register) into eax
            strcat(codeBuffer, "\tmovl\t");
            strcat(codeBuffer, tok1.id);
            strcat(codeBuffer, ", %eax\n");

            // Move token 2 (constant/variable) into trashReg
            strcat(codeBuffer, "\tmovl\t");
            strcat(codeBuffer, tok2.id);
            strcat(codeBuffer, ", ");
            strcat(codeBuffer, trashReg);
            strcat(codeBuffer, "\n");

            // Divide eax by trashReg
            strcat(codeBuffer, "\tidivl\t");
            strcat(codeBuffer, trashReg);
            strcat(codeBuffer, "\n");

            // Move edx into token 1 (register)
            strcat(codeBuffer, "\tmovl\t%edx, ");
            strcat(codeBuffer, tok1.id);
            strcat(codeBuffer, "\n");

            // Move tempRegEax back into eax
            strcat(codeBuffer, "\tmovl\t");
            strcat(codeBuffer, tempRegEax);
            strcat(codeBuffer, ", %eax\n");

            // Move tempRegEdx back into edx
            strcat(codeBuffer, "\tmovl\t");
            strcat(codeBuffer, tempRegEdx);
            strcat(codeBuffer, ", %edx\n");

            // Free the temporary register and token 2 (register)
            freeRegister(tempRegEax);
            freeRegister(tempRegEdx);
            freeRegister(trashReg);
            freeRegister(tok2.id);

            // Free eax and edx if they were blocked
            if (eaxBlocked)
              freeRegister("%eax");

            if (edxBlocked)
              freeRegister("%edx");

            // Finall push tok1's register back into the stack
            pushArithmetic(opStack, tok1.id, "register");
          }
        }

        // 1 Register Operation -> Left Constant/Variable, Right Register
        else if ( strcmp(tok1.type, "register") != 0 && strcmp(tok2.type, "register") == 0 )
        {
          //printf("1 right register operation\n");

          // If it's addition or multiplication, then it's ok
          if ( strcmp(token, "add") == 0 || !strcmp(token, "imul") == 0 )
          {
            strcat(codeBuffer, "\t");
            strcat(codeBuffer, token);
            strcat(codeBuffer, "l\t");
            strcat(codeBuffer, tok1.id);
            strcat(codeBuffer, ", ");
            strcat(codeBuffer, tok2.id);
            strcat(codeBuffer, "\n");

            pushArithmetic(opStack, tok2.id, "register");
          }

          // If it's a subtraction, we can subtract the other way, not the result, and add by 1 since it's 2's complement
          else if ( strcmp(token, "sub") == 0 )
          {
            // Subtracts tok2 by tok1, so it's y-x instead of x-y
            strcat(codeBuffer, "\tsubl\t");
            strcat(codeBuffer, tok1.id);
            strcat(codeBuffer, ", ");
            strcat(codeBuffer, tok2.id);
            strcat(codeBuffer, "\n");

            // Reverses the bits for the number
            strcat(codeBuffer, "\tnotl\t");
            strcat(codeBuffer, tok2.id);
            strcat(codeBuffer, "\n");

            // And adds 1 to the result
            strcat(codeBuffer, "\taddl\t$1, ");
            strcat(codeBuffer, tok2.id);
            strcat(codeBuffer, "\n");

            // Finally pushes the value into the arithmetic stack
            pushArithmetic(opStack, tok2.id, "register");
          }

          // Division Case
          else if ( strcmp(token, "idiv") == 0 )
          {
            char *tempReg = nextRegister();

            // Move eax into tempReg
            strcat(codeBuffer, "\tmovl\t%eax, ");
            strcat(codeBuffer, tempReg);
            strcat(codeBuffer, "\n");

            // Move token 1 (constant/variable) into eax
            strcat(codeBuffer, "\tmovl\t");
            strcat(codeBuffer, tok1.id);
            strcat(codeBuffer, ", %eax\n");

            // Divide eax by token 2 (register)
            strcat(codeBuffer, "\tidivl\t");
            strcat(codeBuffer, tok2.id);
            strcat(codeBuffer, "\n");

            // Move eax into token 2 (register)
            strcat(codeBuffer, "\tmovl\t%eax, ");
            strcat(codeBuffer, tok2.id);
            strcat(codeBuffer, "\n");

            // Move tempReg back into eax
            strcat(codeBuffer, "\tmovl\t");
            strcat(codeBuffer, tempReg);
            strcat(codeBuffer, ", %eax\n");

            // Free the temporary register
            freeRegister(tempReg);

            // Finally push tok2's register back into the stack
            pushArithmetic(opStack, tok2.id, "register");
          }

          // Modulo Case
          else if ( strcmp(token, "imod") == 0 )
          {
            int eaxBlocked = 0;
            int edxBlocked = 0;

            // Reserves eax and edx
            if (isRegisterFree("%eax"))
            {
              blockRegister("%eax");
              eaxBlocked = 1;
            }
            if (isRegisterFree("%edx"))
            {
              blockRegister("%edx");
              edxBlocked = 1;
            }

            char *tempRegEax = nextRegister();
            char *tempRegEdx = nextRegister();

            // Move eax into tempRegEax
            strcat(codeBuffer, "\tmovl\t%eax, ");
            strcat(codeBuffer, tempRegEax);
            strcat(codeBuffer, "\n");

            // Move edx into tempRegEdx
            strcat(codeBuffer, "\tmovl\t%edx, ");
            strcat(codeBuffer, tempRegEdx);
            strcat(codeBuffer, "\n");

            // Move token 1 (constant/variable) into eax
            strcat(codeBuffer, "\tmovl\t");
            strcat(codeBuffer, tok1.id);
            strcat(codeBuffer, ", %eax\n");

            // Divide eax by token 2 (register)
            strcat(codeBuffer, "\tidivl\t");
            strcat(codeBuffer, tok2.id);
            strcat(codeBuffer, "\n");

            // Move edx into token 2 (register)
            strcat(codeBuffer, "\tmovl\t%edx, ");
            strcat(codeBuffer, tok2.id);
            strcat(codeBuffer, "\n");

            // Move tempRegEax back into eax
            strcat(codeBuffer, "\tmovl\t");
            strcat(codeBuffer, tempRegEax);
            strcat(codeBuffer, ", %eax\n");

            // Move tempRegEdx back into edx
            strcat(codeBuffer, "\tmovl\t");
            strcat(codeBuffer, tempRegEdx);
            strcat(codeBuffer, ", %edx\n");

            // Free the temporary register and token 2 (register)
            freeRegister(tempRegEax);
            freeRegister(tempRegEdx);

            // Free eax and edx if they were blocked
            if (eaxBlocked)
              freeRegister("%eax");

            if (edxBlocked)
              freeRegister("%edx");

            // Finally push tok2's register back into the stack
            pushArithmetic(opStack, tok2.id, "register");
          }
        }

        // 0 Register Operation -> Left Constant/Variable, Right Constant/Variable
        else
        {
          //printf("0 register operation\n");

          // If both of the tokens are constants, just do normal calculation (ASCII for $ is 36)
          if (tok1.id[0] == 36 && tok2.id[0] == 36)
          {
            // Char pointers to store the constant strings
            char *constStr1 = (char *) malloc(strlen(tok1.id));
            char *constStr2 = (char *) malloc(strlen(tok2.id));

            // Integers used to do operations
            int const1;
            int const2;
            int constResult;

            // Flushes the $ symbol and puts the rest of the string into constStr1 and constStr2
            constStr1 = removeConstSymbol(tok1.id);
            constStr2 = removeConstSymbol(tok2.id);

            // Transforms the strings into ints
            const1 = atoi(constStr1);
            const2 = atoi(constStr2);

            // Frees the string pointers
            free(constStr1);
            free(constStr2);

            // If it is an addition
            if (strcmp(token, "add") == 0)
              constResult = const1 + const2;
            // Else if it is a subtraction
            else if (strcmp(token, "sub") == 0)
              constResult = const1 - const2;
            // Else if it is a multiplication
            else if (strcmp(token, "imul") == 0)
              constResult = const1 * const2;
            // Else if it is a division
            else if (strcmp(token, "idiv") == 0)
              constResult = const1 / const2;
            else if (strcmp(token, "imod") == 0)
              constResult = const1 % const2;

            // Finally mallocs enough space, moves constResult into constToken, and pushes it into the arithmetic stack
            char *constToken = (char *) malloc(12);
            sprintf(constToken, "$%d", constResult);

            pushArithmetic(opStack, constToken, "constant");
          }

          // Addition, multiplication, and subtraction cases
          else if (strcmp(token, "add") == 0 || strcmp(token, "imul") == 0 || strcmp(token, "sub") == 0)
          {
            char *reg = nextRegister();

            strcat(codeBuffer, "\tmovl\t");
            strcat(codeBuffer, tok1.id);
            strcat(codeBuffer, ", ");
            strcat(codeBuffer, reg);
            strcat(codeBuffer, "\n\t");
            strcat(codeBuffer, token);
            strcat(codeBuffer, "l\t");
            strcat(codeBuffer, tok2.id);
            strcat(codeBuffer, ", ");
            strcat(codeBuffer, reg);
            strcat(codeBuffer, "\n");

            pushArithmetic(opStack, reg, "register");
          }

          // Division Case
          else if (strcmp(token, "idiv") == 0)
          {
            char *reg = nextRegister();
            char *tempReg = nextRegister();
            char *trashReg = nextRegister();

            // Move eax into tempReg
            strcat(codeBuffer, "\tmovl\t%eax, ");
            strcat(codeBuffer, tempReg);
            strcat(codeBuffer, "\n");

            // Move token 1 into eax
            strcat(codeBuffer, "\tmovl\t");
            strcat(codeBuffer, tok1.id);
            strcat(codeBuffer, ", %eax\n");

            // Move token 2 into trashReg
            strcat(codeBuffer, "\tmovl\t");
            strcat(codeBuffer, tok2.id);
            strcat(codeBuffer, ", ");
            strcat(codeBuffer, trashReg);
            strcat(codeBuffer, "\n");

            // Divide eax by trashReg
            strcat(codeBuffer, "\tidivl\t");
            strcat(codeBuffer, trashReg);
            strcat(codeBuffer, "\n");

            // Move eax into the next register
            strcat(codeBuffer, "\tmovl\t%eax, ");
            strcat(codeBuffer, reg);
            strcat(codeBuffer, "\n");

            // Move tempReg back into eax
            strcat(codeBuffer, "\tmovl\t");
            strcat(codeBuffer, tempReg);
            strcat(codeBuffer, ", %eax\n");

            // Make tempReg and trashReg available again
            freeRegister(tempReg);
            freeRegister(trashReg);

            // Push the final register back into the stack
            pushArithmetic(opStack, reg, "register");
          }

          // Modulo Case
          else if (strcmp(token, "imod") == 0)
          {
            int eaxBlocked = 0;
            int edxBlocked = 0;

            // Reserves eax and edx
            if (isRegisterFree("%eax"))
            {
              blockRegister("%eax");
              eaxBlocked = 1;
            }
            if (isRegisterFree("%edx"))
            {
              blockRegister("%edx");
              edxBlocked = 1;
            }

            char *tempRegEax = nextRegister();
            char *tempRegEdx = nextRegister();
            char *trashReg = nextRegister();

            // Move eax into tempRegEax
            strcat(codeBuffer, "\tmovl\t%eax, ");
            strcat(codeBuffer, tempRegEax);
            strcat(codeBuffer, "\n");

            // Move edx into tempRegEdx
            strcat(codeBuffer, "\tmovl\t%edx, ");
            strcat(codeBuffer, tempRegEdx);
            strcat(codeBuffer, "\n");

            // Move token 1 into eax
            strcat(codeBuffer, "\tmovl\t");
            strcat(codeBuffer, tok1.id);
            strcat(codeBuffer, ", %eax\n");

            // Move token 2 into trashReg
            strcat(codeBuffer, "\tmovl\t");
            strcat(codeBuffer, tok2.id);
            strcat(codeBuffer, ", ");
            strcat(codeBuffer, trashReg);
            strcat(codeBuffer, "\n");

            // Divide eax by trashReg
            strcat(codeBuffer, "\tidivl\t");
            strcat(codeBuffer, trashReg);
            strcat(codeBuffer, "\n");

            // Move tempRegEax back into eax
            strcat(codeBuffer, "\tmovl\t");
            strcat(codeBuffer, tempRegEax);
            strcat(codeBuffer, ", %eax\n");

            // Move edx into tempRegEax
            strcat(codeBuffer, "\tmovl\t%edx, ");
            strcat(codeBuffer, tempRegEax);
            strcat(codeBuffer, "\n");

            // Move tempRegEdx back into edx
            strcat(codeBuffer, "\tmovl\t");
            strcat(codeBuffer, tempRegEdx);
            strcat(codeBuffer, ", %edx\n");

            // Make tempRegEdx and trashReg available again
            freeRegister(tempRegEdx);
            freeRegister(trashReg);

            // Free eax and edx if they were blocked
            if (eaxBlocked)
              freeRegister("%eax");

            if (edxBlocked)
              freeRegister("%edx");

            // Finally push tempRegEax register back into the stack
            pushArithmetic(opStack, tempRegEax, "register");
          }
        }
      }

      // If it is a function
      else if ( strcmp(token, "func") == 0 )
      {
        // Assumption -> function name is no longer than 20 chars
        char *funcName = (char *) malloc(20);
        int argNum;
        int i;
        int eaxIsFree;

        // These 4 pointers are for the arguments block
        char *tempExpLoc = (char *) malloc(4);
        char *tempExpCode = (char *) malloc(500);
        char *tempCodeBuffer = (char *) malloc(2000);
        char *argumentReg;

        // Grab the function name from the token stream
        funcName = strtok(NULL, " ");

        // Grab the number of arguments from the token stream
        argNum = atoi(strtok(NULL, " "));

        // Flushes the next token, which is just $
        strtok(NULL, " ");

        // Stores the rest of the stuff in the strtokStorage as subsequent strtok calls will mess up some stuff
        char *strtokStorage = strtok(NULL, "");

        char *tempReg;
        char *location;

        if (isRegisterFree("%eax"))
          eaxIsFree = 1;
        else
          eaxIsFree = 0;

        location = nextRegister();

        // If eax is not available before function call
        if (eaxIsFree == 0)
        {
          tempReg = nextRegister();

          // Move eax into tempReg
          strcat(codeBuffer, "\tmovl\t%eax, ");
          strcat(codeBuffer, tempReg);
          strcat(codeBuffer, "\n");
        }

        // Loop through the arguments by evaluating their expressions and putting them into argument registers
        for (i = 0; i < argNum; i++)
        {
          // Gets a register to pass the argument
          argumentReg = popArgumentRegister(i);

          // Grabs the next expression
          tempExpCode = strtok(strtokStorage, "$");
          strtokStorage = strtok(NULL, "");

          // Grabs the location of the resulting expression and fills tempcodeBuffer
          tempExpLoc = computeExpression(tempCodeBuffer, tempExpCode, currentTable, globalTable, stackTable);

          // Concatenates the expression operation in the argument
          strcat(codeBuffer, tempCodeBuffer);

          // Moves the resting place of the expression into an argument register
          strcat(codeBuffer, "\tmovl\t");
          strcat(codeBuffer, tempExpLoc);
          strcat(codeBuffer, ", ");
          strcat(codeBuffer, argumentReg);
          strcat(codeBuffer, "\n");

          // If the result of the expression was stored in a register, free up that register
          if (isRegister(tempExpLoc))
            freeRegister(tempExpLoc);
        }

        // Call the function
        strcat(codeBuffer, "\tcallq\t_");
        strcat(codeBuffer, funcName);
        strcat(codeBuffer, "\n");

        // If eax was not available before function call
        if (eaxIsFree == 0)
        {
          // Move the result from eax into location
          strcat(codeBuffer, "\tmovl\t%eax, ");
          strcat(codeBuffer, location);
          strcat(codeBuffer, "\n");

          // Move tempReg back into eax
          strcat(codeBuffer, "\tmovl\t");
          strcat(codeBuffer, tempReg);
          strcat(codeBuffer, ", %eax\n");

          // Make tempReg available for use again
          freeRegister(tempReg);
        }

        // Push the result of the function into the arithmetic stack
        pushArithmetic(opStack, location, "register");

        // Because of how messy strotok is, we will now restore whatever was after the function to expressionCode
        expressionCode = strtokStorage;

        // Lastly, free those pointers
        free(tempExpLoc);
        free(tempCodeBuffer);
      }

      // If the first character of the token is a number, it returns 0
      else if ( !isalpha(token[0]) )
      {
        //printf("Constant token [%s] found\n", token);

        char *constToken = malloc(sizeof(token) + 1);
        sprintf(constToken, "%s", "$");
        strcat(constToken, token);

        pushArithmetic(opStack, constToken, "constant");
      }

      // Else it's a variable
      else
      {
        //printf("Variable token [%s] found\n", token);

        Symbol *variable = checkVariable(token, currentTable, globalTable, stackTable);

        // If the variable is present in scope or global
        if (variable == NULL)
          printf("Warning: variable [%s] is not defined in local or global scope, but it is going to be added anyways\n", token);

        char *location = malloc(sizeof(char) * floor(variable->offset * -1 / 10) + 1 + 7);
        sprintf(location, "%d", variable->offset);
        strcat(location, "(%rbp)");

        pushArithmetic(opStack, location, "variable");
      }

      token = strtok(NULL, " ");
  }

  Arithmetic finalTok = popArithmetic(opStack);

  free(opStack);
  return finalTok.id;
}

int computeAssignment(char *codeBuffer, char *left, char *location, Symbol **currentTable, Symbol **globalTable, Stack_node *stackTable)
{
  char *reg = nextRegister();

  // If the left part is memory and right part of the expression is also memory, put it in a register first and modify the location
  if (isRegister(location) == 0 && location[0] != 36)
  {
    strcat(codeBuffer, "\tmovl\t");
    strcat(codeBuffer, location);
    strcat(codeBuffer, ", ");
    strcat(codeBuffer, reg);
    strcat(codeBuffer, "\n");

    location = reg;
  }

  freeRegister(reg);

  // In case the assigned is a register
  if (isRegister(left))
  {
    strcat(codeBuffer, "\tmovl\t");
    strcat(codeBuffer, location);
    strcat(codeBuffer, ", ");
    strcat(codeBuffer, left);
    strcat(codeBuffer, "\n");
    //printf("Register assigned\n");
    return 1;
  }

  // Looks up the variable in the symbol tables otherwise
  Symbol *lookupVar = checkVariable(left, currentTable, globalTable, stackTable);

  // LookupVar present in scope
  if (lookupVar != NULL)
  {
    char *offset = malloc(sizeof(char) * floor(lookupVar->offset * -1 / 10) + 1 + 7);
    sprintf(offset, "%d", lookupVar->offset);

    strcat(codeBuffer, "\tmovl\t");
    strcat(codeBuffer, location);
    strcat(codeBuffer, ", ");
    strcat(codeBuffer, offset);
    strcat(codeBuffer, "(%rbp)\n");
    //printf("Scope or global variable assigned\n");
    return 1;
  }
  // LookupVar is not present in scope
  else
  {
    printf("Error: cannot assign to [%s] as it is not in local or global scope\n", left);
    return 0;
  }

}

char *computeSelect(char *codeBuffer, int labelCounter, char *compCode, char *trueInstruction, char *comparison, int complex)
{
  // Label Malloc (11 base + 3 for labelCounterStr)
  char *labelTrue = (char *) malloc(sizeof(char) * 14);
  char *labelFalse = (char *) malloc(sizeof(char) * 14);
  char *labelCont = (char *) malloc(sizeof(char) * 16);

  // Assuming that there are no more than 999 if conditions
  char *labelCounterStr = (char *) malloc(sizeof(char) * 3);
  sprintf(labelCounterStr, "%d", labelCounter);

  // For an if/else condition
  if (complex)
  {
    // True Label Creation
    strcpy(labelTrue, "ifLabel_");
    strcat(labelTrue, labelCounterStr);

    // False Label Creation
    strcpy(labelFalse, "elseLabel_");
    strcat(labelFalse, labelCounterStr);

    // Continue Label Creation
    strcpy(labelCont, "contIfLabel_");
    strcat(labelCont, labelCounterStr);
  }
  // For a simple if condition
  else
  {
    // True Label Creation
    strcpy(labelTrue, "trueLabel_");
    strcat(labelTrue, labelCounterStr);

    // False Label Creation
    strcpy(labelFalse, "falseLabel_");
    strcat(labelFalse, labelCounterStr);
  }

  // Condition Comparison
  strcpy(codeBuffer, compCode);
  strcat(codeBuffer, "\t");

  // Jump if True
  strcat(codeBuffer, comparison);
  strcat(codeBuffer, "\t");
  strcat(codeBuffer, labelTrue);

  // Jump if False
  strcat(codeBuffer, "\n\tjmp\t");
  strcat(codeBuffer, labelFalse);
  strcat(codeBuffer, "\n");

  // True Label and Instruction
  strcat(codeBuffer, labelTrue);
  strcat(codeBuffer, ":\n");
  strcat(codeBuffer, trueInstruction);

  if (complex)
  {
    // Jump to continue if it is an if/else statement
    strcat(codeBuffer, "\tjmp\t");
    strcat(codeBuffer, labelCont);
    strcat(codeBuffer, "\n");
  }

  // False Label
  strcat(codeBuffer, labelFalse);
  strcat(codeBuffer, ":\n");

  free(labelTrue);
  free(labelFalse);

  if (complex)
    return labelCont;

  else
    return NULL;
}

char *inverseComp(char *comp)
{
  if (!strcmp(comp, "je"))
    return "jne";
  else if (!strcmp(comp, "jne"))
    return "je";
  else if (!strcmp(comp, "jle"))
    return "jg";
  else if (!strcmp(comp, "jge"))
    return "jl";
  else if (!strcmp(comp, "jl"))
    return "jge";
  else if (!strcmp(comp, "jg"))
    return "jle";
  else
    printf("Error: wrong input at inverseComp\n");
  return NULL;
}

char *removeConstSymbol(char *str)
{
  int length = strlen(str);
  int i;
  char *retStr = (char *) malloc(length - 1);

  for (i = 0; i < length - 1; i++)
    retStr[i] = str[i+1];

  return retStr;
}