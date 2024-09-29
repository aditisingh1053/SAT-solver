# SAT Solver

This SAT solver implements the **Davis–Putnam–Logemann–Loveland (DPLL)** backtracking algorithm to determine whether a propositional logic formula in **Conjunctive Normal Form (CNF)** is satisfiable.

## **Overview of the Algorithm**

The **DPLL algorithm** is a complete algorithm for solving the propositional satisfiability problem (SAT). It works by:
1. **Unit Propagation**: Automatically assigning truth values to variables that must be true to satisfy unit clauses.
2. **Decision Making**: Guessing the value of a variable when no unit clause can be used.
3. **Backtracking**: Reverting decisions when conflicts arise.
4. **Conflict Propagation**: Detecting when an assignment leads to a conflict and adjusting accordingly.

### **Key Functions**

#### 1. `parseInput()`
This function reads and parses the input CNF formula from standard input. It extracts the number of variables and clauses, and constructs a representation of the clauses. Positive and negative literals are stored separately for efficient propagation later.

#### 2. `currentValueForLiteral(int literal)`
Returns the current truth value of a literal (TRUE, FALSE, or UNDEFINED). This helps in evaluating whether a clause is satisfied by the current partial assignment of variables.

#### 3. `setLiteralToTrue(int literal)`
This function assigns the truth value `TRUE` to a literal and stores the assignment in the model stack, which tracks variable assignments.

#### 4. `propagateGivesConflict()`
Handles **unit propagation** by checking whether clauses can be satisfied with the current assignment. If a clause cannot be satisfied and there are no unassigned literals, it detects a **conflict**. If a unit clause is found, the appropriate literal is assigned a truth value. If a conflict occurs during propagation, the function returns `true`, signaling the need for backtracking.

#### 5. `backtrack()`
Performs **backtracking** when a conflict is detected. It undoes previous assignments by resetting variables to **UNDEFINED**, reverting to an earlier decision level, and assigning the opposite value to the most recent decision literal.

#### 6. `getNextDecisionLiteral()`
Selects the next literal for decision-making based on **activity heuristics**, prioritizing literals that are more likely to influence the satisfiability of the formula.

#### 7. `checkmodel()`
After a solution is found, this function verifies that all clauses in the formula are satisfied by the current variable assignments. If any clause is not satisfied, it reports an error.

#### 8. `exitWithSatisfiability(bool satisfiable)`
Handles solver termination. If the problem is satisfiable, it verifies the model and prints "SATISFIABLE" with some statistics. If unsatisfiable, it prints "UNSATISFIABLE".

#### 9. `executeDPLL()`
Implements the main **DPLL algorithm** loop. It repeatedly propagates assignments, handles conflicts with backtracking, and continues until either a solution is found or the formula is deemed unsatisfiable.

#### 10. `checkUnitClauses()`
This function checks for **unit clauses** (clauses with only one literal) in the input and immediately assigns their corresponding literals truth values to simplify the problem.

## **Execution Flow**

1. **Input Parsing**: The SAT problem is parsed using `parseInput()`.
2. **Initial Unit Propagation**: Any unit clauses are processed with `checkUnitClauses()`.
3. **Main Loop**:
   - `executeDPLL()` repeatedly attempts to assign truth values to variables.
   - If conflicts arise, it backtracks using `backtrack()`.
   - If no conflicts arise and no unassigned variables remain, the solver exits with "SATISFIABLE".
   - If a conflict is found at the top decision level, the solver exits with "UNSATISFIABLE".

The solver leverages unit propagation and activity-based heuristics to explore the search space efficiently, minimizing unnecessary conflicts and backtracking.
