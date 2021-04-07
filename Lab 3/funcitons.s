    .syntax     unified
    .cpu        cortex-m4
    .text


    .global		Add
    .thumb_func
    .align
Add:    
    ADD         R0,R0,R1        //add R1 to R0 and save result to R0
    BX          LR              //return


    .global		Less1
    .thumb_func
    .align
Less1:  
    SUB         R0,R0,1         //subtract 1 from R0 and save result to R0
    BX          LR              //return


    .global		Square2x
    .thumb_func
    .align
Square2x:   
    MOV         R1,R0           //copy value from R0 to R1
    ADD         R0,R0,R1        //add R1 to R0 and save result in R0
    B           Square          //call square function


    .global		Last
    .thumb_func
    .align
Last:   
    PUSH        {R4,LR}     //preserve R4 and LR
    MOV         R4,R0       //copy R0 to R4
    BL          SquareRoot  //call SquareRoot function
    ADD         R0,R0,R4    //Add R4 to R0 and save result to R0
    POP         {R4,PC}     //return

    .end