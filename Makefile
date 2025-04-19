# Makefile for UFLP Solver in C

CC = gcc
CFLAGS = -O2 -Wall
CPLEXLIB = -lcplex -lm

SRC = src/main.c src/ReadData.c src/solve_MIP_model.c src/Lagrange_relaxationv2.c src/Heuristic.c
HEADERS = src/def.h
TARGET = uflp_solver

$(TARGET): $(SRC) $(HEADERS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(CPLEXLIB)

clean:
	rm -f $(TARGET) *.o
