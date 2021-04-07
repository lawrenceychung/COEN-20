        .syntax     unified
        .cpu        cortex-m4
        .text

// int Between(int min, int value, int max) ;

        .global     Between
        .align
        .thumb_func
Between:    
        SUB         R1,R1,R0            //computes (value - min) and stores in R1
        SUB         R0,R2,R0            //computes (max - min) and stores in R0
        CMP         R0,R1               //compares (value - min) to (max - min)
        ITE         HS                  //if comparasion is Higher Than or Same
        LDRHS       R0,=1               //loads R0 with 1
        LDRLO       R0,=0               //if comparasion is Lower Than, then load R0 with 0
        BX          LR                  //Returns


// int Count(int cells[], int numb, int value) ;

        .global     Count
        .align
        .thumb_func
Count:    
        LDR         R12,=0              //initializes a counter at R12 to 0
        ADD         R1,R0,R1,LSL 2      //Adds beginning of array + length of array = end of array

_start:
        CMP         R0,R1               //compares where R0(where array is at) to R1 (end of array)
        BEQ         done                //if where the array is at == end of array, break out of loop

        LDR         R3,[R0],#1          //gets value at array auto incrementing 
        CMP         R3,R2               //compare "arr" to "value"
        IT          EQ                  //if comparasion is equal   
        ADDEQ       R12,R12,1           //increment counter by 1
        B           _start              //loop back to "_start"

done:   MOV         R0,R12              //copy counter value to R0 for return
        BX          LR                  //Returns

        