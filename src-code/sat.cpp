#include <iostream>
#include <vector>
#include <stdlib.h>
using namespace std;

#define UNDEFINED -1
#define TRUE 1
#define FALSE 0
#define DECISION_MARK 0
#define ACTIVITY_INCREMENT 1.0
#define ACT_INC_UPDATE_RATE 1000

bool silentOutput = false;
uint numVariables, numClauses;
vector<vector<int>> clauses;
vector<vector<vector<int>*>> positiveClauses, negativeClauses;
vector<int> model, modelStack;
uint indexOfNextLiteralToPropagate, decisionLevel, conflicts, propagations, decisions;
vector<double> positiveLiteralActivity, negativeLiteralActivity;

inline uint var(int literal) {
    return abs(literal);
}

void parseInput() {
    char c = cin.get();
    while (c == 'c') {
        while (c != '\n') c = cin.get();
        c = cin.get();
    }
    string aux;
    cin >> aux >> numVariables >> numClauses;
    clauses.resize(numClauses);
    positiveClauses.resize(numVariables + 1);
    negativeClauses.resize(numVariables + 1);
    for (uint clause = 0; clause < numClauses; ++clause) {
        int literal;
        while (cin >> literal && literal != 0) {
            clauses[clause].push_back(literal);
            if (literal > 0) positiveClauses[var(literal)].push_back((vector<int>*)&clauses[clause]);
            else negativeClauses[var(literal)].push_back((vector<int>*)&clauses[clause]);
        }
    }
    model.resize(numVariables + 1, UNDEFINED);
    indexOfNextLiteralToPropagate = 0;
    decisionLevel = 0;
    positiveLiteralActivity.resize(numVariables + 1, 0.0);
    negativeLiteralActivity.resize(numVariables + 1, 0.0);
    conflicts = propagations = decisions = 0;
}

int currentValueForLiteral(int literal) {
    if (literal >= 0) return model[literal];
    if (model[-literal] == UNDEFINED) return UNDEFINED;
    return 1 - model[-literal];
}

void setLiteralToTrue(int literal) {
    modelStack.push_back(literal);
    if (literal > 0) model[literal] = TRUE;
    else model[-literal] = FALSE;
}

void updateActivityForLiteral(int literal) {
    uint index = var(literal);
    if (literal > 0) positiveLiteralActivity[index] += ACTIVITY_INCREMENT;
    else negativeLiteralActivity[index] += ACTIVITY_INCREMENT;
}

void updateActivityForConflictingClause(const vector<int>& clause) {
    ++conflicts;
    if ((conflicts % ACT_INC_UPDATE_RATE) == 0) {
        for (uint i = 1; i <= numVariables; ++i) {
            positiveLiteralActivity[i] /= 2.0;
            negativeLiteralActivity[i] /= 2.0;
        }
    }
    for (uint i = 0; i < clause.size(); ++i) updateActivityForLiteral(clause[i]);
}

bool propagateGivesConflict() {
    while (indexOfNextLiteralToPropagate < modelStack.size()) {
        int literalToPropagate = modelStack[indexOfNextLiteralToPropagate++];
        ++propagations;
        vector<vector<int>*> clausesToPropagate = literalToPropagate > 0 ?
            negativeClauses[var(literalToPropagate)] : positiveClauses[var(literalToPropagate)];
        for (uint i = 0; i < clausesToPropagate.size(); ++i) {
            vector<int> clause = *clausesToPropagate[i];
            bool isSomeLiteralTrue = false;
            int undefinedLiterals = 0, lastUndefinedLiteral = 0;
            for (uint k = 0; !isSomeLiteralTrue && k < clause.size(); ++k) {
                int value = currentValueForLiteral(clause[k]);
                if (value == TRUE) isSomeLiteralTrue = true;
                else if (value == UNDEFINED) {
                    ++undefinedLiterals;
                    lastUndefinedLiteral = clause[k];
                }
            }
            if (!isSomeLiteralTrue && undefinedLiterals == 0) {
                updateActivityForConflictingClause(clause);
                return true;
            } else if (!isSomeLiteralTrue && undefinedLiterals == 1) setLiteralToTrue(lastUndefinedLiteral);
        }
    }
    return false;
}

void backtrack() {
    uint i = modelStack.size() - 1;
    int literal = 0;
    while (modelStack[i] != DECISION_MARK) {
        literal = modelStack[i];
        model[var(literal)] = UNDEFINED;
        modelStack.pop_back();
        --i;
    }
    modelStack.pop_back();
    --decisionLevel;
    indexOfNextLiteralToPropagate = modelStack.size();
    setLiteralToTrue(-literal);
}

int getNextDecisionLiteral() {
    ++decisions;
    double maximumActivity = 0.0;
    int mostActiveVariable = 0;
    for (uint i = 1; i <= numVariables; ++i) {
        if (model[i] == UNDEFINED) {
            if (positiveLiteralActivity[i] >= maximumActivity) {
                maximumActivity = positiveLiteralActivity[i];
                mostActiveVariable = i;
            }
            if (negativeLiteralActivity[i] >= maximumActivity) {
                maximumActivity = negativeLiteralActivity[i];
                mostActiveVariable = -i;
            }
        }
    }
    return mostActiveVariable;
}

void checkmodel() {
    for (uint i = 0; i < numClauses; ++i) {
        bool someTrue = false;
        for (uint j = 0; !someTrue && j < clauses[i].size(); ++j)
            someTrue = (currentValueForLiteral(clauses[i][j]) == TRUE);
        if (!someTrue) {
            cout << "Error in model, clause is not satisfied:";
            for (uint j = 0; j < clauses[i].size(); ++j) cout << clauses[i][j] << " ";
            cout << endl;
            exit(1);
        }
    }
}

void exitWithSatisfiability(bool satisfiable) {
    if (satisfiable) {
        checkmodel();
        cout << "SATISFIABLE," << decisions << "," << propagations << endl;
        exit(20);
    } else {
        cout << "UNSATISFIABLE," << decisions << "," << propagations << endl;
        exit(10);
    }
}
void executeDPLL() {
    while (true) {
        while (propagateGivesConflict()) {
            if (decisionLevel == 0) {
                exitWithSatisfiability(false);
            }
            backtrack();
        }
        int decisionLit = getNextDecisionLiteral();
        if (decisionLit == 0) {
            exitWithSatisfiability(true);
        }
        modelStack.push_back(DECISION_MARK);
        ++indexOfNextLiteralToPropagate;
        ++decisionLevel;
        setLiteralToTrue(decisionLit);
    }
}

void checkUnitClauses() {
    for (uint i = 0; i < numClauses; ++i) {
        if (clauses[i].size() == 1) {
            int literal = clauses[i][0];
            int value = currentValueForLiteral(literal);
            if (value == FALSE) {
                exitWithSatisfiability(false);
            } else if (value == UNDEFINED) {
                setLiteralToTrue(literal);
            }
        }
    }
}

int main(int argc, char* argv[]) {
    parseInput();
    checkUnitClauses();
    executeDPLL();
}

