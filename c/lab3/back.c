/* back.c - Tiny Basic Interpreter
   Reads and executes Tiny Basic programs from front.in */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_VARS 100
#define MAX_LINES 1000
#define MAX_LINE_LEN 256

typedef struct {
    char name[50];
    double value;
} Variable;

typedef struct {
    int lineNum;
    char command[50];
    char args[200];
} Line;

Variable vars[MAX_VARS];
Line lines[MAX_LINES];
int varCount = 0;
int lineCount = 0;

void setVar(char *name, double value) {
    for (int i = 0; i < varCount; i++) {
        if (strcmp(vars[i].name, name) == 0) {
            vars[i].value = value;
            return;
        }
    }
    if (varCount < MAX_VARS) {
        strcpy(vars[varCount].name, name);
        vars[varCount].value = value;
        varCount++;
    }
}

double getVar(char *name) {
    for (int i = 0; i < varCount; i++) {
        if (strcmp(vars[i].name, name) == 0)
            return vars[i].value;
    }
    return 0;
}

double evalExpr(char *expr) {
    char *p = expr;
    double result = 0;
    int op = '+';
    char numStr[50];
    int j = 0;
    
    while (*p) {
        if (isdigit(*p) || *p == '.') {
            numStr[j++] = *p;
            numStr[j] = 0;
            p++;
        } else if (isalpha(*p)) {
            char varName[50];
            int i = 0;
            while (isalnum(*p)) {
                varName[i++] = *p;
                p++;
            }
            varName[i] = 0;
            double val = getVar(varName);
            
            if (op == '+') result += val;
            else if (op == '-') result -= val;
            else if (op == '*') result *= val;
            else if (op == '/') result /= val;
            
            j = 0;
            numStr[0] = 0;
        } else if (*p == '+' || *p == '-' || *p == '*' || *p == '/') {
            if (j > 0) {
                double val = atof(numStr);
                if (op == '+') result += val;
                else if (op == '-') result -= val;
                else if (op == '*') result *= val;
                else if (op == '/') result /= val;
                j = 0;
                numStr[0] = 0;
            }
            op = *p;
            p++;
        } else if (*p == '(') {
            p++;
            char subExpr[100];
            int i = 0, depth = 1;
            while (depth > 0) {
                if (*p == '(') depth++;
                else if (*p == ')') depth--;
                if (depth > 0) subExpr[i++] = *p;
                p++;
            }
            subExpr[i] = 0;
            double val = evalExpr(subExpr);
            
            if (op == '+') result += val;
            else if (op == '-') result -= val;
            else if (op == '*') result *= val;
            else if (op == '/') result /= val;
        } else {
            p++;
        }
    }
    
    if (j > 0) {
        double val = atof(numStr);
        if (op == '+') result += val;
        else if (op == '-') result -= val;
        else if (op == '*') result *= val;
        else if (op == '/') result /= val;
    }
    
    return result;
}

int compareExpr(char *left, char *op, char *right) {
    double lval = evalExpr(left);
    double rval = evalExpr(right);
    
    if (strcmp(op, "<") == 0) return lval < rval;
    if (strcmp(op, ">") == 0) return lval > rval;
    if (strcmp(op, "=") == 0) return lval == rval;
    if (strcmp(op, "<=") == 0) return lval <= rval;
    if (strcmp(op, ">=") == 0) return lval >= rval;
    if (strcmp(op, "<>") == 0) return lval != rval;
    
    return 0;
}

int findLine(int lineNum) {
    for (int i = 0; i < lineCount; i++) {
        if (lines[i].lineNum == lineNum)
            return i;
    }
    return -1;
}

void executePrint(char *args) {
    char *p = args;
    char part[200];
    int i = 0;
    
    while (*p) {
        if (*p == ',') {
            part[i] = 0;
            i = 0;
            
            char *start = part;
            while (*start && isspace(*start)) start++;
            char *end = start + strlen(start) - 1;
            while (end > start && isspace(*end)) end--;
            end[1] = 0;
            
            if (*start == '"') {
                for (int j = 1; j < strlen(start) - 1; j++)
                    printf("%c", start[j]);
            } else {
                printf("%g", evalExpr(start));
            }
            printf("\t");
            p++;
        } else {
            part[i++] = *p;
            p++;
        }
    }
    
    if (i > 0) {
        part[i] = 0;
        char *start = part;
        while (*start && isspace(*start)) start++;
        char *end = start + strlen(start) - 1;
        while (end > start && isspace(*end)) end--;
        end[1] = 0;
        
        if (*start == '"') {
            for (int j = 1; j < strlen(start) - 1; j++)
                printf("%c", start[j]);
        } else {
            printf("%g", evalExpr(start));
        }
    }
    printf("\n");
}

void executeLet(char *args) {
    char *eq = strchr(args, '=');
    if (!eq) return;
    
    char varName[50];
    strncpy(varName, args, eq - args);
    varName[eq - args] = 0;
    
    char *start = varName;
    while (*start && isspace(*start)) start++;
    char *end = start + strlen(start) - 1;
    while (end > start && isspace(*end)) end--;
    end[1] = 0;
    strcpy(varName, start);
    
    double val = evalExpr(eq + 1);
    setVar(varName, val);
}

int executeIf(char *args, int *nextLine) {
    char argsCopy[200];
    strcpy(argsCopy, args);
    char *then = strstr(argsCopy, "THEN");
    if (!then) return 1;
    
    *then = 0;
    
    char left[100], op[10], right[100];
    char *p = argsCopy;
    int i = 0;
    
    while (p < then && *p && *p != '<' && *p != '>' && *p != '=') {
        if (!isspace(*p)) left[i++] = *p;
        p++;
    }
    left[i] = 0;
    
    i = 0;
    if (*p == '<' || *p == '>' || *p == '=') {
        if (p[1] == '>' || p[1] == '=') {
            op[i++] = *p;
            op[i++] = p[1];
            p += 2;
        } else {
            op[i++] = *p;
            p++;
        }
    }
    op[i] = 0;
    
    i = 0;
    while (p < then && *p) {
        if (!isspace(*p)) right[i++] = *p;
        p++;
    }
    right[i] = 0;
    
    if (compareExpr(left, op, right)) {
        then += 4;
        while (*then && isspace(*then)) then++;
        
        if (strncmp(then, "GOTO", 4) == 0) {
            int lineNum = atoi(then + 4);
            *nextLine = findLine(lineNum);
            return 0;
        } else if (strncmp(then, "LET", 3) == 0) {
            executeLet(then + 3);
        }
    }
    
    return 1;
}

void loadProgram(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("ERROR: Cannot open %s\n", filename);
        exit(1);
    }
    
    char line[MAX_LINE_LEN];
    while (fgets(line, sizeof(line), fp)) {
        if (strlen(line) < 2) continue;
        if (strncmp(line, "REM", 3) == 0) continue;
        
        int lineNum = atoi(line);
        if (lineNum == 0) continue;
        
        char *cmd = line;
        while (*cmd && isdigit(*cmd)) cmd++;
        while (*cmd && isspace(*cmd)) cmd++;
        
        char command[50];
        int i = 0;
        while (*cmd && !isspace(*cmd) && i < 49) {
            command[i++] = *cmd;
            cmd++;
        }
        command[i] = 0;
        
        while (*cmd && isspace(*cmd)) cmd++;
        
        lines[lineCount].lineNum = lineNum;
        strcpy(lines[lineCount].command, command);
        strcpy(lines[lineCount].args, cmd);
        
        char *nl = strchr(lines[lineCount].args, '\n');
        if (nl) *nl = 0;
        
        lineCount++;
    }
    fclose(fp);
}

void run() {
    int pc = 0;
    
    while (pc < lineCount) {
        char *cmd = lines[pc].command;
        char *args = lines[pc].args;
        
        if (strcmp(cmd, "PRINT") == 0 || strcmp(cmd, "PR") == 0) {
            executePrint(args);
        } else if (strcmp(cmd, "LET") == 0) {
            executeLet(args);
        } else if (strcmp(cmd, "IF") == 0) {
            int nextLine = pc + 1;
            if (!executeIf(args, &nextLine)) {
                pc = nextLine;
                continue;
            }
        } else if (strcmp(cmd, "GOTO") == 0) {
            int lineNum = atoi(args);
            pc = findLine(lineNum);
            if (pc < 0) break;
            continue;
        } else if (strcmp(cmd, "END") == 0) {
            break;
        }
        
        pc++;
    }
}

int main() {
    loadProgram("front.in");
    run();
    return 0;
}
