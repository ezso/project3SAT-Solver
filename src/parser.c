#include "parser.h"

#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "err.h"
#include "lexer.h"
#include "list.h"
#include "propformula.h"
#include "util.h"
#include "variables.h"

/**
 * Assigns symbols to strings.
 *
 * Aborts the program with an error message if an invalid input is detected.
 *
 * @param str  a string to translate
 * @return     the resulting symbol
 */
FormulaKind toKind(const char* str) {
    if (strcmp("||", str) == 0) {
        return OR;
    } else if (strcmp("&&", str) == 0) {
        return AND;
    } else if (strcmp("!", str) == 0) {
        return NOT;
    } else if (strcmp("=>", str) == 0) {
        return IMPLIES;
    } else if (strcmp("<=>", str) == 0) {
        return EQUIV;
    } else {
        regex_t varpattern;
        int res = regcomp(&varpattern, "^[a-zA-Z][a-zA-Z0-9]*$", 0);
        if (res != 0) {
            err("compilation failed");
        }
        res = regexec(&varpattern, str, 0, NULL, 0);
        regfree(&varpattern);
        if (res == 0) {
            return VAR;
        } else {
            err(str);
            return -1;
        }
    }
}

PropFormula* parseFormula(FILE* input, VarTable* vt) {
    if (input != NULL) {
        PropFormula* pf;
        List ls = mkList();
        char* str;
        str = nextToken(input);
        if (str == NULL) {
            err("the file is empty");
            return NULL;
        }
        while (str) {
            switch (toKind(str)) {
                case VAR:
                    pf = mkVarFormula(vt, str);
                    push(&ls, pf);
                    break;
                case NOT:
                    pf = peek(&ls);
                    if (pf == NULL) {
                        clearList(&ls);
                        err("invalid input formula");
                        return NULL;
                    }
                    pop(&ls);
                    pf = mkUnaryFormula(NOT, pf);
                    push(&ls, pf);
                    break;
                case OR:
                    pf = peek(&ls);
                    if (pf == NULL) {
                        clearList(&ls);
                        err("invalid input formula");
                        return NULL;
                    }
                    pop(&ls);
                    if (peek(&ls) == NULL) {
                        clearList(&ls);
                        err("invalid input formula");
                        return NULL;
                    }
                    pf = mkBinaryFormula(OR, peek(&ls), pf);
                    pop(&ls);
                    push(&ls, pf);
                    break;
                case AND:
                    pf = peek(&ls);
                    if (pf == NULL) {
                        clearList(&ls);
                        err("invalid input formula");
                        return NULL;
                    }
                    pop(&ls);
                    if (peek(&ls) == NULL) {
                        clearList(&ls);
                        err("invalid input formula");
                        return NULL;
                    }
                    pf = mkBinaryFormula(AND, peek(&ls), pf);
                    pop(&ls);
                    push(&ls, pf);
                    break;
                case IMPLIES:
                    pf = peek(&ls);
                    if (pf == NULL) {
                        clearList(&ls);
                        err("invalid input formula");
                        return NULL;
                    }
                    pop(&ls);
                    if (peek(&ls) == NULL) {
                        clearList(&ls);
                        err("invalid input formula");
                        return NULL;
                    }
                    pf = mkBinaryFormula(IMPLIES, peek(&ls), pf);
                    pop(&ls);
                    push(&ls, pf);
                    break;
                case EQUIV:
                    pf = peek(&ls);
                    if (pf == NULL) {
                        clearList(&ls);
                        err("invalid input formula");
                        return NULL;
                    }
                    pop(&ls);
                    if (peek(&ls) == NULL) {
                        clearList(&ls);
                        err("invalid input formula");
                        return NULL;
                    }
                    pf = mkBinaryFormula(EQUIV, peek(&ls), pf);
                    pop(&ls);
                    push(&ls, pf);
                    break;
                default:
                    clearList(&ls);
                    err("invalid input formula");
                    return NULL;
            }
            str = nextToken(input);
        }
        pf = peek(&ls);
        pop(&ls);
        if (isEmpty(&ls)) {
            return pf;
        } else {
            clearList(&ls);
            err("formula is not correct");
            return NULL;
        }
    } else {
        err("the file is not found");
        return NULL;
    }
}
