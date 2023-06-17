#include "dpll.h"

#include <stdlib.h>

#include "cnf.h"
#include "err.h"
#include "list.h"
#include "util.h"
#include "variables.h"

typedef enum Reason { CHOSEN, IMPLIED } Reason;

/**
 * Struct to represent an entry in the assignment stack. Should only be created
 * and freed by pushAssignment and popAssignment.
 */
typedef struct Assignment {
    VarIndex var;
    Reason reason;
} Assignment;

/**
 * Adds a new assignment to the assignment stack.
 *
 * @param stack  an assignment stack
 * @param var    the variable to assign
 * @param r      the reason for the assignment
 */
void pushAssignment(List* stack, VarIndex var, Reason r) {
    Assignment* a = (Assignment*)malloc(sizeof(Assignment));
    a->var = var;
    a->reason = r;
    push(stack, a);
}

/**
 * Removes the head element of an assignment stack and frees it.
 *
 * @param stack  an assignment stack
 */
void popAssignment(List* stack) {
    Assignment* a = (Assignment*)peek(stack);
    free(a);
    pop(stack);
}

/**
 * Führt eine Iteration des DPLL Algorithmus aus.
 *
 * @param vt       die zugrunde liegende Variablentabelle
 * @param stack    der Zuweisungsstack
 * @param cnf      die zu prüfende Formel
 * @return         1 falls der Algorithmus mit SAT terminieren sollte,
 *                 0 falls der Algorithmus weiterlaufen sollte,
 *                 -1 falls der Algorithmus mit UNSAT terminieren sollte
 */
/**
 * Performs one iteration of the DPLL algorithm.
 *
 * @param vt       the underlying variable table
 * @param stack    an assignment stack
 * @param cnf      the formula to check
 * @return         1 if the algorithm should terminate with SAT,
 *                 0 if the algorithm should continue,
 *                -1 if the algorithm should terminate with UNSAT
 */
int iterate(VarTable* vt, List* stack, CNF* cnf) {
    Clause* cl;
    Literal x;
    int unitClauseFound = 0;
    ListIterator it = mkIterator(&cnf->clauses);
    while (isValid(&it)) {
        cl = getCurr(&it);
        x = getUnitLiteral(vt, cl);
        if (x != 0) {
            unitClauseFound = 1;
            updateVariableValue(vt, abs(x), TRUE);
            updateTruthValue(vt, cl);
            if (cl->val != TRUE) {
                updateVariableValue(vt, abs(x), FALSE);
                updateTruthValue(vt, cl);
                if (cl->val != TRUE) {
                    return 0;
                }
            }
            pushAssignment(stack, abs(x), IMPLIED);
        }
        next(&it);
    }
    if (!unitClauseFound) {
        VarIndex v = getNextUndefinedVariable(vt);
        if (v != 0) {
            updateVariableValue(vt, v, TRUE);
            pushAssignment(stack, v, CHOSEN);
            return 0;
        } else if (!isEmpty(stack)) {
            while (!isEmpty(stack)) {
                Assignment* ass = peek(stack);
                if (ass->reason == CHOSEN) {
                    updateVariableValue(vt, ass->var, FALSE);
                    popAssignment(stack);
                    pushAssignment(stack, ass->var, IMPLIED);
                    return 0;
                }
                popAssignment(stack);
            }
            return -1;
        } else {
            return -1;
        }
    }
    TruthValue res = evalCNF(cnf);
    if (res == TRUE)
        return 1;
    else
        return 0;
}

char isSatisfiable(VarTable* vt, CNF* cnf) {
    List stack = mkList();

    int res;
    do {
        res = iterate(vt, &stack, cnf);
    } while (res == 0);

    while (!isEmpty(&stack)) {
        popAssignment(&stack);
    }

    return (res < 0) ? 0 : 1;
}
