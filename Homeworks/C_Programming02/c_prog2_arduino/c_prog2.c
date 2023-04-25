/*
 *
 *      CSE/ECE 474 introductory C programming assignment  #2
 *      Spring 2023
 *
 *      Student Name: Mason Wheeler
 *      Student ID: 2032634
 *
 *      WRITE YOUR CODE IN THIS FILE
 *
 */

/*  Objectives: Gain experience and proficiency with C programming.


Format of assignment:
  1)  enter your name and ID# into the space above
  2)  Read through the comments below and edit code into this file and 
      c_prog2.h to solve the problems.
  3)  Test your code by running c_prog2_arduino.ino and comparing the output
      to the posted solution in sample_output.txt

#define constants are NOT required for this assignment but MAY be used.

*/
#include "c_prog2.h"

/*******************************************************************************
   Part 1:  Bitwise operations
*******************************************************************************/
/*
 * Some terminology: SETTING a bit means that if K-th bit is 0, then set it to 1
 * and if it is 1 then leave it unchanged (in other words, a bit is SET if it is
 * 1)
 *
 * CLEARING a bit means that if K-th bit is 1, then clear it to 0 and if it is 0
 * then leave it unchanged (a bit is CLEAR if it is 0)
 *
 * COMPLEMENTING or TOGGLING a bit means that if K-th bit is 1, then change it
 * to 0 and if it is 0 then change it to 1.
*/


/* Part 1.1 Write the function long mangle(long SID) which will take your sudent
 * ID and alter it in ways that depend on the binary representation of it as an
 * integer.
 *
 * Specifications: 
 *     1) right shift the ID by 2 positions 
 *     2) clear bit 6 (counting from the LSB=0) 
 *     3) complement bit 3 (If bit 3 is one, make it 0, If bit 3 is 0, make it 
 *        1)
 *
 *   OUTPUT EXAMPLE:
 *   Part 2.1: Enter your UW Student ID number: 
 *    You entered 51218 
 *    Your mangled SID is 12812
 */

/* Part 1.1: Write the function long mangle(long SID) */
long mangle(long SID) {
    SID >>= 2;               // 1) Right shift the ID by 2 positions
    SID &= ~(1 << 6);         // 2) Clear bit 6 (counting from the LSB=0)
    SID ^= (1 << 3);          // 3) Complement bit 3
    return SID;
}

/* Part 1.2  More bit manipulation. The function bit_check(int data, int bits_on,
 * int bits_off), will check an int to see if a specific mask of  bits is set
 * AND that another bit mask is clear.  Returns 1 if matches and 0 if not.
 * Return -1 for the degenerate case of testing for BOTH off and on for any bit
 *
 * Pseudocode examples of some masks and what they are checking  (Using 4-bit
 * #'s for clarity)
 *
 * bits_on = binary(0011) - this mask means that we are checking if the last
 *                          two LSBs (e.g. 00XX, the X bits) of the data are
 *                          SET (1). bits_off= binary(0100) - this mask means
 *                          that we are checking if the third LSB (e.g. 0X00,
 *                          the X bit) of the data is CLEAR (0).
 *
 * Example of returning -1: bits_on = binary(0110) bits_off= binary(0011) here
 * the second LSB (00X0, the X bit) has to be both SET and CLEAR to pass
 * according to the masks, which is impossible - in this case we return a -1
 * for invalid mask combinations.
 *
 * Again in pseudocode, some examples of actual checks/values that the test
 * code would do: (Using 4-bit #'s for clarity)
 *
 *  int d = binary(1100)  // pseudocode 
 *  int onmask1 =  binary(0011)  
 *  int offmask1 = binary(0100)  
 *  int onmask2 =  binary(0100)
 *  int offmask2 = binary(0001)  
 *
 *  bit_check(d, onmask1, offmask1) --> 0 
 *  bit_check(d, onmask1, offmask2) --> -1 // contradictory
 *  bit_check(d, onmask2, offmask2) --> 1 
 *  bit_check(d, offmask1, offmask1) --> -1 // contradictory
 */

/* Part 1.2: More bit manipulation */
int bit_check(int data, int bits_on, int bits_off) {
    if ((bits_on & bits_off) != 0) {
        return -1; // Contradictory mask combinations
    }

    int on_result = (data & bits_on) == bits_on;
    int off_result = (data & bits_off) == 0;

    return on_result && off_result;
}

/*******************************************************************************
   Part 2:  Pointer declaration and usage
*******************************************************************************/
char a_array[] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O',
                  'P','Q','R','S','T','U','V','W','X','Y','Z'};

/* Part 2.1: Write the function char* pmatch(char c)
 * This function will return a pointer to the element in a pre-defined
 * array which matches it's character arg. (accepts capital letters only).
 * If the character cannot be found, return the NULL pointer.
 *
 * Example: pmatch('A') should return a pointer to 'A' in a_array
 * Dereferencing that pointer should then print 'A'
 */

/* Part 2.1: Write the function char* pmatch(char c) */
char* pmatch(char c){
    if (c >= 'A' && c <= 'Z') {
        return &a_array[c - 'A'];
    }
    return 0;
}

/* Part 2.2: Write the function char nlet(char* ptr) where ptr is a pointer
 * returned by pmatch. This function will return the next letter of the alphabet
 * (Not a pointer to the next letter of the alphabet) unless the pointer points
 * to 'Z'. In that case it will return -1. If the argument does not point to a
 * capital letter A-Y, return -1.
 */

/* Part 2.2: Write the function char nlet(char* ptr) */
char nlet(char *ptr){
    if (ptr >= a_array && ptr < a_array + 25) {
        return *(ptr + 1);
    }
    return -1;
}

/* Part 2.3: Write the function int ldif(char c1, char c2) to find the alphabet
 * distance between two letters using pointers. 
 * Example:     
 *     Ldif('A','E') -> 4
 *
 * If either character is not a capital letter, return a negative number < -26
 */

/* Part 2.3: Write the function int ldif(char c1, char c2) */
int ldif(char c1, char c2) {
    // Ensure both characters are uppercase letters
    if ((c1 >= 'A' && c1 <= 'Z') && (c2 >= 'A' && c2 <= 'Z')) {

        // Calculate the difference in positions, including c2 but excluding c1
        int diff = c2 - c1;

        // Return the signed difference in positions
        return diff;
    } else {
        // If any of the input characters is not an uppercase letter, return -999
        return -999;
    }
}
