# 🏭 Uncapacitated Facility Location Problem (UFLP) Solver

This repository contains C code implementations for solving the **Uncapacitated Facility Location Problem (UFLP)** using three approaches:

- 🧮 **MIP model with CPLEX**
- 🧠 **Heuristic approach** (constructive + local search)
- 📉 **Lagrangian relaxation** using the subgradient method

---

## 📂 File Structure

### `/data`
- `cap72.txt`: Example input instance with facility data, customer demands, and transportation costs.

### `/src`
- `def.h`: Function declarations and constants.
- `main.c` *(originally `Heuristic.c`)*: Executes all solving methods and prints results.
- `ReadData.c`: Reads the input data and allocates/deallocates memory.
- `solve_MIP_model.c`: Builds and solves the MIP formulation using IBM CPLEX.
- `Heuristic.c`: Heuristic procedure for constructing a feasible solution and refining it via local search.
- `Lagrange_relaxationv2.c`: Lagrangian relaxation with subgradient optimization.

---

## 🧠 Problem Description

Given:
- A set of potential facilities \( N \)
- A set of customers \( M \)
- Fixed cost \( f_i \) to open facility \( i \)
- Transportation cost \( c_{ij} \) from facility \( i \) to customer \( j \)
- Demand \( d_j \) for each customer

### Objective:
Minimize the sum of facility opening costs and customer servicing costs by:
- Deciding which facilities to open
- Assigning each customer to one open facility

---

## 🛠️ Compilation

You can compile with:

```bash
gcc -o uflp_solver src/*.c -lcplex -lm
