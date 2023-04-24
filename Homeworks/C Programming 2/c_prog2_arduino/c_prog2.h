#ifdef __cplusplus
 extern "C" {
#endif
#ifndef C_PROG2_H_INCLUDED
#define C_PROG2_H_INCLUDED

/*
 *      CSE/ECE 474 introductory C programming assignment  #2
 *      Spring 2023
 *
 *      Student Name:
 *      Student ID:
 *
 *      WRITE ANY NEW FUNCTION PROTOTYPES IN THIS FILE
 *      
 *      Note: You will need to modify the definition of Person as well.
 *
 */
#include <stdio.h>
#include <stdint.h>

/*************************************************************************
   Part 2.1:  Bitwise operations
*************************************************************************/
long mangle(long);

int  bit_check(int, int, int);

/*************************************************************************
   Part 2.2:  Basic Pointer declaration and usage
*************************************************************************/
char * pmatch(char);

/*************************************************************************
   Part 2.3:  Pointer Arithmetic
*************************************************************************/
char nlet(char*);

int ldif(char, char);

/*************************************************************************
   Part 2.4   struct and sizeof
*************************************************************************/
// Modify this struct definition according to the instructions
// HINT: How long do each of these fields need to be?
typedef struct Person {
    char FirstName[1];
    char LastName[1];
    char StreetAddr[1];
    char ZipCode[1];
    float Height;
    float Weight;
    long int DBirth;
} Person ;

int personSize(Person);

void float_to_ints(float, int*);

// Modify this definition 
// NOTE: This is used to store the output string of per_print.
// How long does this need to be?
char* personbuf;

char* per_print(Person*, char*);

#endif // C_PROG2_H_INCLUDED
#ifdef __cplusplus
}
#endif
