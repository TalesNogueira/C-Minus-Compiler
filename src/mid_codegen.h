/*-------------------------------------------------------------------------------------------------/
 *  Intermediate "Mid" Code Generator interface for a C- Compiler
 *  File: mid_codegen.h
 *---------------------------------*/

#include "globals.h"

#ifndef MIDGEN_H
#define MIDGEN_H

/*--------------------------------------------/
 *  Quadruples List structure
 *---------------------------------*/

/*  Operation → Defines all supported intermediate code operations  */
typedef enum {
    Add, Sub, Mul, Div, SGT, SLT, SGET, SLET, SET, SDT,
    AllocVAR, AllocARRAY, StoreVAR, StoreARRAY, LoadVAR, LoadARRAY, Move,
    IFfalse, Label, Jump, 
    FunBGN, FunEND, Param, Call, Return,
    Push, Pop, Halt
} Operation;

/*  AddrType → Defines the type of an address (void, constant, or string)  */
typedef enum { addrVoid, addrConst, addrString } AddrType;

/*  Address → Represents an operand (address) in a quadruple  */
typedef struct {
	AddrType type;

	union{
		int value;
		char *name;
	} content;
} Address;

/*  QuadList → Node structure for a quadruple in/and the Quadruples List  */
typedef struct Quadruple {
    Operation op;
    Address src, tgt, dst;
  	struct Quadruple *next;
} QuadList;

#endif