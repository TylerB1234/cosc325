/* front.c - a lexical analyzer system for simple
             arithmetic expressions */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Global declarations */
/* Variables */
int charClass;
char lexeme[100];
char nextChar;
int lexLen;
int token;
int nextToken;
FILE *in_fp;
/* Function declarations */
void addChar();
void getChar();
void getNonBlank();
int lex();

/* Character classes */
#define LETTER 0
#define DIGIT 1
#define QUOTE 2
#define UNKNOWN 99

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
/******************************************************/
/* main driver */
int main()
{
    /* Open the input data file and process its contents */
    if ((in_fp = fopen("front.in", "r")) == NULL)
        printf("ERROR - cannot open front.in \n");
    else
    {
        getChar();
        do
        {
            lex();
        } while (nextToken != EOF);
    }
}

/*****************************************************/
/* lookup - a function to lookup operators and parentheses
            and return the token */

int lookup(char ch)
{
    switch (ch)
    {
    case '=':
        addChar();
        nextToken = EQUALS_OP;
        break;
        
    case '<':
        addChar();
        getChar();
        if (nextChar == '=') {
            addChar();
            nextToken = LE_OP;
        } else if (nextChar == '>') {
            addChar();
            nextToken = NE_OP;
        } else {
            nextToken = LT_OP;
            return nextToken;  // Don't consume next char
        }
        break;

    case '>':
        addChar();
        getChar();
        if (nextChar == '=') {
            addChar();
            nextToken = GE_OP;
        } else {
            nextToken = GT_OP;
            return nextToken;  // Don't consume next char
        }
        break;

    case '(':
        addChar();
        nextToken = LEFT_PAREN;
        break;

    case ')':
        addChar();
        nextToken = RIGHT_PAREN;
        break;

    case '+':
        addChar();
        nextToken = ADD_OP;
        break;

    case '-':
        addChar();
        nextToken = SUB_OP;
        break;

    case '*':
        addChar();
        nextToken = MULT_OP;
        break;

    case '/':
        addChar();
        nextToken = DIV_OP;
        break;
    
    case ',':
        addChar();
        nextToken = COMMA;
        break;
    
    case ':':
        addChar();
        nextToken = COLON;
        break;

    case ';':
        addChar();
        nextToken = SEMICOLON;
        break;

    case '?':
        addChar();
        nextToken = QMARK;
        break;

    default:
        addChar();
        printf("Unexpected symbol found: %c while working on the current lexeme: %s\n", nextChar, lexeme);
        exit(1);
        break;
    }
    return nextToken;
}

/*****************************************************/
/* addChar - a function to add nextChar to lexeme */
void addChar()
{
    if (lexLen <= 98)
    {
        lexeme[lexLen++] = nextChar;
        lexeme[lexLen] = 0;
    }
    else
        printf("Error - lexeme is too long \n");
}
/*****************************************************/
/* getChar - a function to get the next character of
             input and determine its character class */
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
            charClass = LETTER;
        else if (isdigit((unsigned char)nextChar))
            charClass = DIGIT;
        else if (nextChar == '"')
            charClass = QUOTE;
        else
            charClass = UNKNOWN;
    }
}

/*****************************************************/
/* getNonBlank - a function to call getChar until it
                 returns a non-whitespace character */
void getNonBlank()
{
    while (isspace(nextChar))
        getChar();
}

int keywordLookup() {
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
    else
        return IDENT;
}

/*****************************************************/
/* lex - a simple lexical analyzer for arithmetic
         expressions */
int lex() // get token
{
    lexLen = 0;
    getNonBlank();
    switch (charClass)
    {
        /* Parse identifiers */
    case LETTER:
        addChar();
        getChar();
        while (charClass == LETTER || charClass == DIGIT)
        {
            addChar();
            getChar();
        }
        nextToken = keywordLookup();
        break;

        /* Parse integer literals */
    case DIGIT:
        addChar();
        getChar();
        while (charClass == DIGIT)
        {
            addChar();
            getChar();
        }

        if (nextChar == '.'){
            addChar();
            getChar();
            while (charClass == DIGIT)
            {
                addChar();
                getChar();
            }
            nextToken = FLOAT_LIB;
        } else {
            nextToken = INT_LIT;
        }
        break;

    case QUOTE:
        addChar();
        getChar();
        
        while (charClass != QUOTE && charClass != EOF)
        {
            addChar();
            getChar();
        }
        if (charClass == QUOTE) {
            addChar();  // Add the closing quote
            getChar();
            nextToken = STR_LIT;
        } else {
            printf("Error - unterminated string literal\n");
            exit(1);
        }
        break;

        /* Parentheses and operators */
    case UNKNOWN:
        lookup(nextChar);
        getChar();
        break;

        /* EOF */
    case EOF:
        nextToken = EOF;
        lexeme[0] = 'E';
        lexeme[1] = 'O';
        lexeme[2] = 'F';
        lexeme[3] = 0;
        break;
    } /* End of switch */
    printf("Next token is: %d, Next lexeme is %s\n",
    nextToken, lexeme);
    return nextToken;
} /* End of function lex */