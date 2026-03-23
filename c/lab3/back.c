/* back.c - Tiny Basic Interpreter
   Executes Tiny Basic programs from front.in file */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

/* Token codes */
#define STR_LIT 8
#define INT_LIT 10
#define FLOAT_LIB 9
#define IDENT 11
#define EQUALS_OP 20
#define LT_OP 12
#define GT_OP 13
#define LE_OP 14
#define GE_OP 15
#define NE_OP 16
#define ADD_OP 21
#define SUB_OP 22
#define MULT_OP 23
#define DIV_OP 24
#define LEFT_PAREN 25
#define RIGHT_PAREN 26
#define COMMA 27
#define COLON 28
#define SEMICOLON 29
#define QMARK 52
#define PRINT 30
#define IF 31
#define THEN 32
#define INPUT 33
#define GOTO 34
#define LET 35
#define GOSUB 36
#define RETURN 37
#define CLEAR 38
#define LIST 39
#define RUN 40
#define END 41
#define REM 42

/* Symbol table for variables */
#define MAX_VARS 100
#define MAX_PROGRAMS 1000
#define MAX_LINE_LENGTH 256

typedef struct {
    char name[100];
    double value;
} Variable;

typedef struct {
    int lineNum;
    int token;
    char lexeme[256];
} Instruction;

/* Global data */
Variable vars[MAX_VARS];
int varCount = 0;
Instruction program[MAX_PROGRAMS];
int programSize = 0;
int currentLine = 0;
FILE *in_fp;
int charClass;
char lexeme[256];
char nextChar;
int lexLen;
int token;
int nextToken;

/* Function declarations */
void getChar();
void addChar();
void getNonBlank();
int lex();
void getNonBlank();
int keywordLookup();
int lookup(char ch);
void executeProgram();
void executeLine(int lineIdx);
double evaluateExpression();
double getTerm();
double getFactor();
void setVariable(char *name, double value);
double getVariable(char *name);
void printStatement();
void letStatement();
void ifStatement();
void gotoStatement();
void gosubStatement();
void returnStatement();
int findLineNumber(int lineNum);

/* Get next character from file */
void getChar()
{
    int c = getc(in_fp);
    if (c == EOF)
    {
        charClass = EOF;
        nextChar = 0;
    }
    else
    {
        nextChar = (char)c;
        if (isalpha((unsigned char)nextChar))
            charClass = 0; /* LETTER */
        else if (isdigit((unsigned char)nextChar))
            charClass = 1; /* DIGIT */
        else if (nextChar == '"')
            charClass = 2; /* QUOTE */
        else
            charClass = 99; /* UNKNOWN */
    }
}

/* Add character to lexeme */
void addChar()
{
    if (lexLen <= 254)
    {
        lexeme[lexLen++] = nextChar;
        lexeme[lexLen] = 0;
    }
}

/* Skip whitespace */
void getNonBlank()
{
    while (isspace(nextChar))
        getChar();
}

/* Lookup keyword */
int keywordLookup()
{
    if (strcmp(lexeme, "PRINT") == 0 || strcmp(lexeme, "PR") == 0)
        return PRINT;
    else if (strcmp(lexeme, "IF") == 0)
        return IF;
    else if (strcmp(lexeme, "THEN") == 0)
        return THEN;
    else if (strcmp(lexeme, "INPUT") == 0)
        return INPUT;
    else if (strcmp(lexeme, "GOTO") == 0)
        return GOTO;
    else if (strcmp(lexeme, "LET") == 0)
        return LET;
    else if (strcmp(lexeme, "GOSUB") == 0)
        return GOSUB;
    else if (strcmp(lexeme, "RETURN") == 0)
        return RETURN;
    else if (strcmp(lexeme, "CLEAR") == 0)
        return CLEAR;
    else if (strcmp(lexeme, "LIST") == 0)
        return LIST;
    else if (strcmp(lexeme, "RUN") == 0)
        return RUN;
    else if (strcmp(lexeme, "END") == 0)
        return END;
    else if (strcmp(lexeme, "REM") == 0)
        return REM;
    else
        return IDENT;
}

/* Lookup operator */
int lookup(char ch)
{
    switch (ch)
    {
    case '=':
        addChar();
        return EQUALS_OP;
    case '<':
        addChar();
        return LT_OP;
    case '>':
        addChar();
        return GT_OP;
    case '(':
        addChar();
        return LEFT_PAREN;
    case ')':
        addChar();
        return RIGHT_PAREN;
    case '+':
        addChar();
        return ADD_OP;
    case '-':
        addChar();
        return SUB_OP;
    case '*':
        addChar();
        return MULT_OP;
    case '/':
        addChar();
        return DIV_OP;
    case ',':
        addChar();
        return COMMA;
    case ':':
        addChar();
        return COLON;
    case ';':
        addChar();
        return SEMICOLON;
    case '?':
        addChar();
        return QMARK;
    default:
        return -1;
    }
}

/* Lexical analyzer */
int lex()
{
    lexLen = 0;
    getNonBlank();
    
    switch (charClass)
    {
    case 0: /* LETTER */
        addChar();
        getChar();
        while (charClass == 0 || charClass == 1)
        {
            addChar();
            getChar();
        }
        nextToken = keywordLookup();
        break;

    case 1: /* DIGIT */
        addChar();
        getChar();
        while (charClass == 1)
        {
            addChar();
            getChar();
        }
        if (nextChar == '.')
        {
            addChar();
            getChar();
            while (charClass == 1)
            {
                addChar();
                getChar();
            }
            nextToken = FLOAT_LIB;
        }
        else
            nextToken = INT_LIT;
        break;

    case 2: /* QUOTE */
        addChar();
        getChar();
        while (charClass != 2 && charClass != EOF)
        {
            addChar();
            getChar();
        }
        if (charClass == 2)
        {
            addChar();
            getChar();
        }
        nextToken = STR_LIT;
        break;

    case 99: /* UNKNOWN */
        nextToken = lookup(nextChar);
        getChar();
        break;

    case EOF:
        nextToken = EOF;
        break;
    }
    return nextToken;
}

/* Set variable value */
void setVariable(char *name, double value)
{
    for (int i = 0; i < varCount; i++)
    {
        if (strcmp(vars[i].name, name) == 0)
        {
            vars[i].value = value;
            return;
        }
    }
    if (varCount < MAX_VARS)
    {
        strcpy(vars[varCount].name, name);
        vars[varCount].value = value;
        varCount++;
    }
}

/* Get variable value */
double getVariable(char *name)
{
    for (int i = 0; i < varCount; i++)
    {
        if (strcmp(vars[i].name, name) == 0)
            return vars[i].value;
    }
    return 0;
}

/* Find line number in program */
int findLineNumber(int lineNum)
{
    for (int i = 0; i < programSize; i++)
    {
        if (program[i].lineNum == lineNum)
            return i;
    }
    return -1;
}

/* Evaluate factor (number, variable, or parenthesized expression) */
double getFactor()
{
    if (nextToken == INT_LIT)
    {
        double val = atof(lexeme);
        lex();
        return val;
    }
    else if (nextToken == FLOAT_LIB)
    {
        double val = atof(lexeme);
        lex();
        return val;
    }
    else if (nextToken == IDENT)
    {
        char varName[100];
        strcpy(varName, lexeme);
        lex();
        return getVariable(varName);
    }
    else if (nextToken == LEFT_PAREN)
    {
        lex();
        double val = evaluateExpression();
        lex(); /* skip RIGHT_PAREN */
        return val;
    }
    else if (nextToken == SUB_OP)
    {
        lex();
        return -getFactor();
    }
    return 0;
}

/* Evaluate term (handles * and /) */
double getTerm()
{
    double result = getFactor();
    
    while (nextToken == MULT_OP || nextToken == DIV_OP)
    {
        int op = nextToken;
        lex();
        double right = getFactor();
        if (op == MULT_OP)
            result *= right;
        else if (op == DIV_OP && right != 0)
            result /= right;
    }
    return result;
}

/* Evaluate expression (handles + and -) */
double evaluateExpression()
{
    double result = getTerm();
    
    while (nextToken == ADD_OP || nextToken == SUB_OP)
    {
        int op = nextToken;
        lex();
        double right = getTerm();
        if (op == ADD_OP)
            result += right;
        else
            result -= right;
    }
    return result;
}

/* Execute PRINT statement */
void printStatement()
{
    lex(); /* skip PRINT token */
    
    while (nextToken != EOF && nextToken != COLON)
    {
        if (nextToken == STR_LIT)
        {
            /* Print string, removing quotes */
            for (int i = 1; i < strlen(lexeme) - 1; i++)
                printf("%c", lexeme[i]);
            lex();
        }
        else if (nextToken == COMMA)
        {
            printf("\t");
            lex();
        }
        else
        {
            /* Print expression result */
            double val = evaluateExpression();
            if (val == (int)val)
                printf("%d", (int)val);
            else
                printf("%g", val);
        }
        
        if (nextToken == COMMA)
            printf("\t");
    }
    printf("\n");
}

/* Execute LET statement */
void letStatement()
{
    lex(); /* skip LET */
    char varName[100];
    strcpy(varName, lexeme);
    lex(); /* skip variable name */
    lex(); /* skip = */
    double val = evaluateExpression();
    setVariable(varName, val);
}

/* Execute IF statement */
void ifStatement()
{
    lex(); /* skip IF */
    double left = evaluateExpression();
    int op = nextToken;
    lex();
    double right = evaluateExpression();
    
    int condition = 0;
    if (op == LT_OP && left < right) condition = 1;
    else if (op == GT_OP && left > right) condition = 1;
    else if (op == LE_OP && left <= right) condition = 1;
    else if (op == GE_OP && left >= right) condition = 1;
    else if (op == EQUALS_OP && left == right) condition = 1;
    else if (op == NE_OP && left != right) condition = 1;
    
    lex(); /* skip THEN */
    
    if (condition)
    {
        if (nextToken == GOTO)
        {
            lex();
            int lineNum = atoi(lexeme);
            int idx = findLineNumber(lineNum);
            if (idx >= 0)
                currentLine = idx;
            lex();
        }
        else if (nextToken == LET)
        {
            letStatement();
        }
    }
}

/* Execute GOTO statement */
void gotoStatement()
{
    lex(); /* skip GOTO */
    int lineNum = atoi(lexeme);
    int idx = findLineNumber(lineNum);
    if (idx >= 0)
        currentLine = idx;
    lex();
}

/* Main program execution */
void executeProgram()
{
    currentLine = 0;
    
    while (currentLine < programSize && program[currentLine].token != END)
    {
        nextToken = program[currentLine].token;
        strcpy(lexeme, program[currentLine].lexeme);
        
        switch (nextToken)
        {
        case PRINT:
            printStatement();
            break;
        case LET:
            letStatement();
            break;
        case IF:
            ifStatement();
            break;
        case GOTO:
            gotoStatement();
            currentLine--;
            break;
        case REM:
            break;
        case END:
            return;
        }
        currentLine++;
    }
}

/* Parse entire program file */
void parseProgram()
{
    in_fp = fopen("front.in", "r");
    if (!in_fp)
    {
        printf("ERROR - cannot open front.in\n");
        return;
    }
    
    getChar();
    int lineNum = 0;
    
    while (charClass != EOF && programSize < MAX_PROGRAMS)
    {
        lexLen = 0;
        
        /* Get line number */
        while (charClass != EOF && !isdigit(nextChar))
            getChar();
        
        if (charClass == EOF)
            break;
        
        lineNum = 0;
        while (isdigit(nextChar))
        {
            lineNum = lineNum * 10 + (nextChar - '0');
            getChar();
        }
        
        program[programSize].lineNum = lineNum;
        
        /* Get first token of line */
        lex();
        program[programSize].token = nextToken;
        strcpy(program[programSize].lexeme, lexeme);
        
        programSize++;
        
        /* Skip to end of line */
        while (charClass != EOF && nextChar != '\n')
            getChar();
        if (nextChar == '\n')
            getChar();
    }
    
    fclose(in_fp);
}

/* Main */
int main()
{
    parseProgram();
    executeProgram();
    return 0;
}
