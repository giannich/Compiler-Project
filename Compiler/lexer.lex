%{
#include "y.tab.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/* Constants are here for the length of names and max size of macros */
#define MAX_CHAR_PER_NAME 20
#define MAX_MACROS 10

/* These two arrays hold the macro names and their respective values
 * The count here is just to keep track of the number of macros
 */ 

char **nameArr;
char **subArr;

int macroCount = 0;

void mallocArr()
{
  nameArr = (char **) malloc(sizeof(char)*MAX_CHAR_PER_NAME*MAX_MACROS);
  subArr = (char **) malloc(sizeof(char)*MAX_CHAR_PER_NAME*MAX_MACROS);
}

%}

/* Definitions */

/* Primary Expressions Definition Block */
IDENT           [a-zA-Z][a-zA-Z0-9]*
CONST_INT       [0-9]+
CONST_FLT       [0-9]+(\.)[0-9]+         
CONST_STR       (\")[a-zA-Z0-9 ]+(\")

/* Types Definition Block */
INT_TYPE        int
STR_TYPE        string
EXT_TYPE        extern

/* Operation Definition Block */
ADD_OP          (\+)
SUB_OP          (\-)
MUL_OP          (\*)
DIV_OP          (\/)
MOD_OP          (\%)
ASS_OP          (\=)
LSH_OP          (<<)
RSH_OP          (>>)
INC_OP          (\+\+)
DEC_OP          (\-\-)

/* Comparison Operation Definition Block */
EQU_COMP        (==)
NEQ_COMP        (!=)
LOE_COMP        (<=)
MOE_COMP        (>=)
LST_COMP        (<)
MRT_COMP        (>)

/* Control Definitions Block */
IF_CON          if
ELS_CON         else
WHL_CON         while
DO_CON          do
FOR_CON         for
RET_CON         return
THN_CON         then

/* Comment Definition Block */
C89_COMM        (\/\*).*(\*\/)
C99_COMM        (\/\/).*\n

/* Whitespace Definition Block */
SPC_WTSP        [ ]
TAB_WTSP        (\t)
NEW_WTSP        (\n)

/* Extra Definitions Block */
L_PAR           (\()
R_PAR           (\))
L_BRA           (\{)
R_BRA           (\})
SEMIC           (;)
COMMA           (,)

/* Macro Definitions Block */
MACRO           [A-Z]+
DEFINE          #define[ \t]{MACRO}[ \t]{CONST_INT}
NAME            #{MACRO}[ \t]{CONST_INT}

%%

  /* Rules */

  /* When the define keyword is encountered, its macro name is added to nameArr and its value to subArr */

{DEFINE}          {

  strtok(yytext, " \t");
  nameArr[macroCount] = strtok(NULL, " \t");
  subArr[macroCount] = strtok(NULL, " ");

  macroCount++;
}

  /* When a definition change is encountered, the macro name is assigned to a 
   * temporary subName array while its value is assigned to a temporary subVal array 
   * It then loops through the nameArr array and looks for a matching name
   * If there is a match, the values are swapped, otherwise it prints an error
   */

{NAME}            {

  char *subName;
  char *subVal;
  int i = 0, arrLen;

  subName = (char *) malloc(sizeof(char) * MAX_CHAR_PER_NAME);
  subVal = (char *) malloc(sizeof(char) * MAX_CHAR_PER_NAME);

  subName = strtok(yytext, " ");
  subVal = strtok(NULL, " ");

  /* Here it only reads the macro name without the # sign */

  memmove(subName, subName+1, strlen(subName));

  while (1)
    {
      if (!strcmp(subName, nameArr[i]))
  {
    subArr[i] = subVal;
    break;
  }
      i++;
      if (i >= macroCount)
  {
    printf("Error: Undefined re-definition #%s %s does not match any of the previously defined macros.\n", subName, subVal);
    break;
  }
    }
}

  /* If a macro is found, the scanner will output its respective value otherwise an error message */

{MACRO}           {

  int i = 0;

  while (1)
    {
      if (!strcmp(yytext, nameArr[i]))
  {
    /* The string is tokenized here in order to just get the value and not the rest of the crap */
    // printf("%s: integer\n", strtok(subArr[i], " \n"));
    yylval.string = strdup(strtok(subArr[i], " \n")); 
    return CONST_INT;
    break;
  }
      i++;
      if (i >= macroCount)
  {
    printf("Error: Undefined macro %s does not match any of the previously defined macros.\n", yytext);
    break;
  }
    }
}

{C89_COMM}        ;
{C99_COMM}        ;

{SPC_WTSP}        ;
{TAB_WTSP}        ;
{NEW_WTSP}        ;

{INT_TYPE}        yylval.string = strdup(yytext); return INT_TYPE;
{STR_TYPE}        yylval.string = strdup(yytext); return STR_TYPE;
{EXT_TYPE}        return EXT_TYPE;

{INC_OP}          return INC_OP;
{DEC_OP}          return DEC_OP;
{ADD_OP}          return ADD_OP;
{SUB_OP}          return SUB_OP;
{MUL_OP}          return MUL_OP;
{MOD_OP}          return MOD_OP;
{DIV_OP}          return DIV_OP;
{ASS_OP}          return ASS_OP;
{LSH_OP}          return LSH_OP;
{RSH_OP}          return RSH_OP;

{EQU_COMP}        yylval.string = strdup(yytext); return EQU_COMP;
{NEQ_COMP}        yylval.string = strdup(yytext); return NEQ_COMP;
{LOE_COMP}        yylval.string = strdup(yytext); return LOE_COMP;
{MOE_COMP}        yylval.string = strdup(yytext); return MOE_COMP;
{LST_COMP}        yylval.string = strdup(yytext); return LST_COMP;
{MRT_COMP}        yylval.string = strdup(yytext); return MRT_COMP;

{IF_CON}          return IF_CON;
{ELS_CON}         return ELS_CON;
{WHL_CON}         return WHL_CON;
{DO_CON}          return DO_CON;
{FOR_CON}         return FOR_CON;
{RET_CON}         return RET_CON;
{THN_CON}         return THN_CON;

{IDENT}           yylval.string = strdup(yytext); return IDENT;
{CONST_INT}       yylval.string = strdup(yytext); return CONST_INT;
{CONST_FLT}       yylval.string = strdup(yytext); return CONST_INT;
{CONST_STR}       yylval.string = strdup(yytext); return CONST_STR;

{L_PAR}           return L_PAR;
{R_PAR}           return R_PAR;
{L_BRA}           return L_BRA;
{R_BRA}           return R_BRA;
{SEMIC}           return SEMIC;
{COMMA}           return COMMA;

.                 printf("Unmatched Parser Error With %s\n", yytext);

%%