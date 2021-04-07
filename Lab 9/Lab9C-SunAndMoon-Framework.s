/*
    This code was written to support the book, "ARM Assembly for Embedded Applications",
    by Daniel W. Lewis. Permission is granted to freely share this software provided
    that this notice is not removed. This software is intended to be used with a run-time
    library adapted by the author from the STM Cube Library for the 32F429IDISCOVERY 
    board and available for download from http://www.engr.scu.edu/~dlewis/book3.
*/
        .syntax         unified
        .cpu            cortex-m4
        .text

// float EccentricAnomaly(float e, float M)

        .global         EccentricAnomaly
        .thumb_func
        .align
EccentricAnomaly:    // S0 = e, S1 = M
        PUSH            {LR}                    //preserves LR
        VPUSH           {S16,S17,S18}           //preserves S16,S17,S18
        VMOV            S16,S0                  //moves copy of e to S16 to be preserved
        VMOV            S17,S1                  //moves copy of M to S17 to be preserved
        VMOV            S0,S1                   //moves M to S0
        BL              cosDeg                  //calls cosDeg function
        VMUL.F32        S3,S16,S0               //multiply S16(e) by S0 (results from cosDeg(M)), into S3
        VLDR            S4,one                 //load S4 with value of R0
        VADD.F32        S18,S3,S4               //add S4(1) to S3(e * results from cosDeg(M)), save into S17
        VMOV            S0,S17                  //moves copy of M to S0
        BL              sinDeg                  //calls sinDeg function
        VMUL.F32        S0,S0,S18               //multiply S0(sinDeg(M)) by S18(1 + e * results from cosDeg(M)), save into S0
        VMUL.F32        S0,S0,S16               //multiply S0(sinDeg(M) * (1 + e * results from cosDeg(M)) by S16(e)
        BL              Rad2Deg                 //calls Rad2Deg function
        VADD.F32        S0,S0,S17               //adds S17(M) to R0(Rad2Deg(e * sinDeg(M) * (1.0 + e * cosDeg(M))) 
        VPOP            {S16,S17,S18}           //restores S16,S17,S18
        POP             {PC}                    //return 

one:                                           
        .float          1.0                      //creating float 1.0

        

// float Kepler(float m, float ecc)

        .global            Kepler
        .thumb_func
        .align
Kepler:    // S0 = m, S1 = ecc
        PUSH            {LR}
        VPUSH           {S16,S17,S18,S19}       //preserves S16,S17,S18,S19
        VMOV            S16,S0                  //moves copy of m to S16 to be preserved
        VMOV            S17,S1                  //moves copy of ecc to S17 to be preserved
        BL              Deg2Rad                 //calls Deg2Rad function
        VMOV            S16,S0                  //moves copy of Rad2Deg(m) to S16 = m
        VMOV            S18,S0                  //moves copy of Rad2Deg(m) to S18 = e
F1:
        VMOV            S0,S18                  //moves copy of S18(e) to S0     
        BL              sinf                    //calls sinf function
        VMUL.F32        S0,S0,S17               //multipy S0(sinf(e)) by S17(ecc)
        VSUB.F32        S0,S18,S0               //subtracts S18(e) by (S0(sinf(e)) * S17(ecc))
        VSUB.F32        S19,S0,S16              //subtracts S16(m) by (S18(e) - S0(sinf(e)) * S17(ecc)) to S19 = delta

        VMOV            S0,S18                  //moves copy of S18(e) to S0
        BL              cosf                    //calls cosf function
        VMUL.F32        S0,S0,S17               //multiply S0(cosf(e)) by S17(ecc)
        VLDR            S1,one                  //loads S1 with 1
        VSUB.F32        S0,S1,S0                //subtracts S1(1) by S0(cosf(e)) * S17(ecc))
        VDIV.F32        S0,S19,S0               //divides S19(delta) by S0(S1(1) - S0(cosf(e)) * S17(ecc))
        VSUB.F32        S18,S18,S0              //subtracts S18(e) - (S19(delta) / S0(S1(1) - S0(cosf(e)) * S17(ecc))

        VABS.F32        S0,S19                  //absolute value of S19(delta) saved into S0
        VLDR            S1,epsilon              //load S1 with value of epsilon
        VCMP.F32        S0,S1                   //compares S0(abs(delta)) to S1(1E-6)
        VMRS            APSR_nzcv,FPSCR         //moves flags from FPU FPSCR to core APSR
        BGT             F1                      //if greater than, then branch to F1

        VMOV            S0,S18                  //otherwise move S18(e) to S0
        VPOP            {S16,S17,S18,S19}       //restores S16,S17,S18,S19
        POP             {PC}                    //return

        .align
epsilon:
        .float          1E-6

        .end


