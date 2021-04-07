/*
    This code was written to support the book, "ARM Assembly for Embedded Applications",
    by Daniel W. Lewis. Permission is granted to freely share this software provided
    that this notice is not removed. This software is intended to be used with a run-time
    library adapted by the author from the STM Cube Library for the 32F429IDISCOVERY 
    board and available for download from http://www.engr.scu.edu/~dlewis/book3.
*/

#include <stdio.h>
#include <stdlib.h>
#include "library.h"
#include "graphics.h"

#pragma GCC push_options
#pragma GCC optimize ("O0")

int __attribute__((weak)) Sum1(int Ai, int Bi, int Cin)
    {
    return Ai ^ Bi ^ Cin ;
    }

int __attribute__((weak)) Sum2(int Ai, int Bi, int Cin)
    {
    return (Ai + Bi + Cin) & 1 ;
    }

int __attribute__((weak)) Sum3(int Ai, int Bi, int Cin)
    {
    int shift = (Ai << 2) | (Bi << 1) | Cin ;
    return (0b10010110 >> shift) & 1 ;
    }

int __attribute__((weak)) Sum4(int Ai, int Bi, int Cin)
    {
    static int8_t sum[] = {0,1,1,0,1,0,0,1} ;
    int index = (Ai << 2) | (Bi << 1) | Cin ;
    return sum[index] ;
    }

int __attribute__((weak)) Cout1(int Ai, int Bi, int Cin)
    {
    return (Ai&Bi) | (Ai&Cin) | (Bi&Cin) ;
    }

int __attribute__((weak)) Cout2(int Ai, int Bi, int Cin)
    {
    return (Ai + Bi + Cin) >> 1 ;
    }

int __attribute__((weak)) Cout3(int Ai, int Bi, int Cin)
    {
    int shift = (Ai << 2) | (Bi << 1) | Cin ;
    return (0b11101000 >> shift) & 1 ;
    }

int __attribute__((weak)) Cout4(int Ai, int Bi, int Cin)
    {
    static int8_t cout[] = {0,0,0,1,0,1,1,1} ;
    int index = (Ai << 2) | (Bi << 1) | Cin ;
    return cout[index] ;
    }

#pragma GCC pop_options

typedef enum {FALSE = 0, TRUE = 1} BOOL ;

typedef struct
    {
    uint8_t *               table ;
    uint16_t                Width ;
    uint16_t                Height ;
    } sFONT ;

typedef struct
    {
    char *                  slabel ;
    char *                  clabel ;
    int                     (*Sum)(int, int, int) ;
    int                     (*Cout)(int, int, int) ;
    int                     xpos ;
    int                     ypos ;
    int                     A ;     // 0 - 15
    int                     B ;     // 0 - 15
    int                     S ;     // 0 - 15
    int                     C ;     // 0 - 15
    BOOL                    fail ;
    } TEST ;

static void                 Add4Bits(TEST *test) ;
static char *               Bits(int value) ;
static void                 RunTest(TEST *test) ;
static int                  Sum0(int Ai, int Bi, int Cin) ;
static int                  Cout0(int Ai, int Bi, int Cin) ;
static void                 UpdateDisplay(TEST *test) ;
static void                 DelayMsec(int msec) ;
static unsigned             GetTimeout(int msec) ;
static void                 SetFontSize(sFONT *Font) ;

extern sFONT                Font8, Font12, Font16, Font20, Font24 ;

#define FONT                Font12

#define XPOS                10
#define YPOS                60

#define XINC                (16*FONT.Width)
#define YINC                ( 5*FONT.Height)

#define CPU_CLOCK_SPEED_MHZ 168
#define ENTRIES(a)          (sizeof(a)/sizeof(a[0]))

int main(void)
    {
    static TEST tests[] =
        {
        {" Sum #1",  "Carries", Sum1, Cout0, 0, 0},
        {" Sum #2",  "Carries", Sum2, Cout0, 0, 1},
        {" Sum #3",  "Carries", Sum3, Cout0, 0, 2},
        {" Sum #4",  "Carries", Sum4, Cout0, 0, 3},
        {"    Sum",  "Cout #1", Sum0, Cout1, 1, 0},
        {"    Sum",  "Cout #2", Sum0, Cout2, 1, 1},
        {"    Sum",  "Cout #3", Sum0, Cout3, 1, 2},
        {"    Sum",  "Cout #4", Sum0, Cout4, 1, 3}
        } ;
    TEST *test ;
    int t ;

    InitializeHardware(HEADER, "Lab 7I: Full Adder Simulation") ;

    test = &tests[0] ;
    for (t = 0; t < ENTRIES(tests); t++, test++)
        {
        test->xpos = XPOS + test->xpos * XINC ;
        test->ypos = YPOS + test->ypos * YINC ;
        UpdateDisplay(test) ;
        }

    for (;;)
        {
        test = &tests[0] ;
        for (t = 0; t < ENTRIES(tests); t++, test++)
            {
            RunTest(test) ;
            UpdateDisplay(test) ;
            }
        DelayMsec(500) ;
        }

    return 0 ;
    }

static int Sum0(int Ai, int Bi, int Cin)
    {
    return Ai ^ Bi ^ Cin ;
    }

static int Cout0(int Ai, int Bi, int Cin)
    {
    return (Ai&Bi) | (Ai&Cin) | (Bi&Cin) ;
    }

static void RunTest(TEST *test)
    {
    if (test->fail) return ;

    test->B = (test->B + 1) & 0xF ;
    if (test->B == 0) test->A = (test->A + 1) & 0xF ;
    Add4Bits(test) ;
    test->fail = (test->S != ((test->A + test->B) & 0xF)) ;
    }

static void UpdateDisplay(TEST *test)
    {
    static int foreground[2][2] = 
        {
        {COLOR_BLACK, COLOR_BLACK}, // Normal:Good/Fail
        {COLOR_BLACK, COLOR_WHITE}  // Tested:Good/Fail
        } ;
    static int background[2][2] =
        {
        {COLOR_WHITE, COLOR_WHITE}, // Normal:Good/Fail
        {COLOR_GREEN, COLOR_RED}    // Tested:Good/Fail
        } ;
    char text[100] ;

    SetFontSize(&FONT) ;
    SetBackground(COLOR_WHITE) ;

    SetForeground(COLOR_BLACK) ;
    sprintf(text, "%7s: ", test->clabel) ;
    DisplayStringAt(test->xpos +  0*FONT.Width, test->ypos + 0*FONT.Height, text) ;
    SetForeground(foreground[test->Cout != Cout0][test->fail]) ;
    SetBackground(background[test->Cout != Cout0][test->fail]) ;
    DisplayStringAt(test->xpos +  9*FONT.Width, test->ypos + 0*FONT.Height, Bits(test->C)) ;
    SetForeground(COLOR_BLACK) ;
    SetBackground(COLOR_WHITE) ;
    DisplayStringAt(test->xpos + 13*FONT.Width, test->ypos + 0*FONT.Height, "0") ;

    SetForeground(COLOR_BLACK) ;
    sprintf(text, "%7s:  %4s", "A", Bits(test->A)) ;
    DisplayStringAt(test->xpos +  0*FONT.Width, test->ypos + 1*FONT.Height, text) ;
    sprintf(text, "%7s:  %4s", "+B", Bits(test->B)) ;
    DisplayStringAt(test->xpos +  0*FONT.Width, test->ypos + 2*FONT.Height, text) ;

    FillRect(test->xpos + 10*FONT.Width, test->ypos + 3*FONT.Height, 4*FONT.Width, 2) ;

    SetForeground(COLOR_BLACK) ;
    sprintf(text, "%7s:  ", test->slabel) ;
    DisplayStringAt(test->xpos +  0*FONT.Width, test->ypos + 3*FONT.Height + 3, text) ;
    SetForeground(foreground[test->Sum != Sum0][test->fail]) ;
    SetBackground(background[test->Sum != Sum0][test->fail]) ;
    DisplayStringAt(test->xpos + 10*FONT.Width, test->ypos + 3*FONT.Height + 3, Bits(test->S)) ;
    }

static char *Bits(int value)
    {
    static char bits[5] ;
    int bit ;

    for (bit = 0; bit <= 3; bit++)
        {
        bits[3 - bit] = '0' + ((value >> bit) & 1) ;
        }
    bits[4] = '\0';
    return bits ;
    }

static unsigned GetTimeout(int msec)
    {
    unsigned cycles = 1000 * msec * CPU_CLOCK_SPEED_MHZ ;
    return GetClockCycleCount() + cycles ;
    }

static void DelayMsec(int msec)
    {
    unsigned timeout = GetTimeout(msec) ;
    while ((int) (timeout - GetClockCycleCount()) > 0) ;
    }

static void Add4Bits(TEST *test)
    {
    int bit, Ai, Bi, Si, Ci ;

    test->S = test->C = 0 ;
    Ci = 0 ;
    for (bit = 0; bit <= 3; bit++)
        {
        Ai = (test->A >> bit) & 1 ;
        Bi = (test->B >> bit) & 1 ;
        Si = (*test->Sum)(Ai, Bi, Ci) ;
        Ci = (*test->Cout)(Ai, Bi, Ci) ;
        test->S |= (Si << bit) ;
        test->C |= (Ci << bit) ;
        }
    }

static void SetFontSize(sFONT *Font)
    {
    extern void BSP_LCD_SetFont(sFONT *) ;
    BSP_LCD_SetFont(Font) ;
    }

