#include <stdio.h>
//  also get math function
#include <math.h>
#include <string.h>    // for strlen (ONLY)
#include <stdlib.h>    // for random()

#include "c_prog2.h"

/*************************************************************************
* main() runS all demos and tests of the parts of assignment 2.
*
* Use these to test your code.  
* DO NOT MODIFY THIS FILE
* See sample_output.txt to see the expected output of the tests.
*************************************************************************/

char buf[80];

void setup() {
    long sid;   // student ID #
    Serial.begin(115200);

    // Part 1.1
    Serial.println( "Part 1.1: Enter your UW Student ID number...");
    while(Serial.available() == 0){
    }

    // Read user input
    sid = (long) trunc(Serial.parseFloat());
    Serial.print(" You entered "); Serial.println(sid);
    
    long msid =  mangle(sid);
    Serial.print(" Your mangled SID is "); Serial.println(msid);
    Serial.println();

    // Part 1.2
    Serial.println("Part 1.2: Check the bit_check() function\n");
    int onm1 = 3;
    int ofm1 = 4;
    int onm2 = 4;
    int ofm2 = 1; // same as example above
    unsigned int d = 0x14; // (1100)

    // The examples
    sprintf(buf, "d = %u Hex: %x, Octal: %o\n", d,d,d);
    Serial.print(buf);
    
    sprintf(buf, "bit_check(%d,%d,%d): %d\n", d, onm1, ofm1, bit_check(d,onm1,ofm1)); //Example 1
    Serial.print(buf);
    
    sprintf(buf, "bit_check(%d,%d,%d): %d\n", d, onm1, ofm2, bit_check(d,onm1,ofm2)); //Example 2
    Serial.print(buf);
    
    sprintf(buf, "bit_check(%d,%d,%d): %d\n", d, onm2, ofm2, bit_check(d,onm2,ofm2)); //Example 3
    Serial.print(buf);
    
    sprintf(buf, "bit_check(%d,%d,%d): %d\n", d, ofm1, ofm1, bit_check(d,ofm1,ofm1)); //Example 4
    Serial.print(buf);
    
    sprintf(buf, "bit_check(129753, 129753, ~129753): %d\n", bit_check(129753, 129753,
        ~129753)); // 1
    Serial.print(buf);

    d = 32768+4096+64;
    sprintf(buf, "d = %u Hex: %x, Octal: %o\n", d,d,d);
    Serial.print(buf);
    
    sprintf(buf, "bit_check(32768+4096+64, 4096+64, 16384): %d\n", 
        bit_check(32768+4096+64, 4096+64, 16384)); // 1
    Serial.print(buf); 
    
    sprintf(buf, "bit_check(32768+4096+64, 4096+64, 32768): %d\n", 
        bit_check(32768+4096+64, 4096+64, 32768)); // 0
    Serial.println(buf);

    // Part 2.1
    char ctest;
    Serial.println("Part 2.1: Enter a capital letter...");
    while(Serial.available() == 0){
    }
    
    // Read the input as a string and extract the first character
    String input = Serial.readString();
    ctest = input[0];
    
    char * ptest = pmatch(ctest);
    if (ptest != NULL){
        sprintf(buf, " You entered: %1c\n", *ptest);
        Serial.print(buf);
        int nl = nlet(ptest);
        if (nl != -1){
            sprintf(buf, "Part 2.2: The next letter after %1c is %c\n", *ptest,
                nlet(ptest));
            Serial.print(buf);
        }
    }
    else { 
        Serial.println(" You did NOT enter a capital letter!\n");
    }
    ctest = 'Z';
    ptest = &ctest;
    sprintf (buf, "Part 2.2: The next letter after %1c is %d\n\n", *ptest, 
        nlet(ptest));
    Serial.print(buf);
    
    sprintf (buf, "Part 2.3: M and Q are %d positions apart\n", ldif('M', 'Q'));
    Serial.print(buf);
    
    sprintf (buf, "Part 2.3: x and Q are %d positions apart\n\n", ldif('x', 'Q'));
    Serial.print(buf);

    // Part 3   Structs
    // Example of declaring and initializing values in a struct
    Person testp = {
        .FirstName={0},
        .LastName={0},
        .StreetAddr={0},
        .ZipCode={0},
        .Height=0.0,
        .Weight=0.0,
        .DBirth=0
    };

    int persize = personSize(testp);
    sprintf(buf, "Part 3.2:  Size of a Person is %d\n",persize);
    Serial.print(buf);

    // Check if person has been defined and add some data
    // The instructions and test cases are slightly ambiguous about whether a
    // "20 character string" includes the null terminator, so at least make
    // sure we can store 19 characters for FirstName, etc.
    Serial.print("Store some data about our person...\n");
    if(sizeof(testp.FirstName) > 19){
        strcpy(testp.FirstName, "Blake");
    } else{
        Serial.println("Person not defined correctly\n");
    }

    if(sizeof(testp.LastName) > 29){
        strcpy(testp.LastName, "Hannaford");
    } else{
       Serial.println("Person not defined correctly\n");
    }

    if(sizeof(testp.StreetAddr) > 78){
        strcpy(testp.StreetAddr, "124 N. Anystreet / Busytown, WA");
    } else{
        Serial.println("Person not defined correctly\n");
    }

    if(sizeof(testp.ZipCode) == 6){
        strcpy(testp.ZipCode, "99499"); 
    } else{
        Serial.println("Person not defined correctly\n");
    }

    testp.Height = 1.97;       // Height in m
    testp.Weight = 180.0/2.2;  // 180 lbs in kg
    // born 24-Mar-1995 (see  https://www.timeanddate.com/date/duration.html)
    testp.DBirth = 34780;
    
    // Check size again after adding data
    persize = personSize(testp);
    sprintf(buf, "Part 3.2:  Size of a Person is STILL %d\n",persize);
    Serial.print(buf);
    Serial.print("Part 3.3:  Our test person is:\n");
    Serial.print(per_print(&testp, personbuf));
    // Now we move to a place w/ really long address!
    if(sizeof(testp.StreetAddr) > 78){
        Serial.println("\nUpdate address to: \n 12903010542039840 NW Antidisetablishmentarianism Street, Apt.5/AnyTown, U.S.A.");
        strcpy(testp.StreetAddr,"12903010542039840 NW Antidisetablishmentarianism Street, Apt.5/AnyTown, U.S.A.");
        Serial.print("\nAddress stored in memory: \n ");
        Serial.println(testp.StreetAddr);
        Serial.print("\nThe new Address is 79 characters!\n");
        Serial.print("Output will be truncated to 60 characters for printing.\n\n");
    } else{
        Serial.print("Person not defined correctly\n");
    }
    Serial.print(per_print(&testp, personbuf));
}

void loop() {
  // put your main code here, to run repeatedly:

}
