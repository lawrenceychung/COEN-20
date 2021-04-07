/*
    This code was written to support the book, "ARM Assembly for Embedded Applications",
    by Daniel W. Lewis. Permission is granted to freely share this software provided
    that this notice is not removed. This software is intended to be used with a run-time
    library adapted by the author from the STM Cube Library for the 32F429IDISCOVERY 
    board and available for download from http://www.engr.scu.edu/~dlewis/book3.
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "library.h"
#include "graphics.h"

#pragma GCC push_options
#pragma GCC optimize ("O0")

// NOTE: In a real application, divisors are usually signed.
// Since this lab simply demonstrates how to use Newton's
// method to compute the reciprocal, it only treats the 
// case for positive divisors. Any real application would
// need to modify this code to handle either sign.

typedef uint32_t        Q16 ;   // unsigned!!!

#define FIXED(x)        ((Q16) ((65536 * (x)) + 0.5))
#define FLOAT(x)        (((float) (x)) / 65536)

// These functions are NOT needed in your assembly code; they
// may be replaced by a short sequence of one or more instructions:
int                     LeadingZeros(Q16 x) ;               // replace by the CLZ instruction
Q16                     Q16Product(Q16 mpier, Q16 mcand) ;  // replace by a SMULL/LSR/ORR sequence

// These functions are to be replaced by assembly language:
Q16 __attribute__((weak)) Normalize(Q16 divisor, int zeros)
    {
    Q16 result ;

    if (zeros >= 16)    result = divisor << (zeros - 16) ;  // use logical shift
    else                result = divisor >> (16 - zeros) ;  // use logical shift

    return result ;
    }

Q16 __attribute__((weak)) Denormalize(Q16 estimate, int zeros)
    {
    Q16 result ;

    if (zeros >= 16)    result = estimate >> (zeros - 16) ;  // use logical shift
    else                result = estimate << (16 - zeros) ;  // use logical shift

    return result ;
    }

Q16 __attribute__((weak)) NormalizedEstimate(Q16 divisor)
    {
    // Replace FIXED macros by the equivalent constant in Q16 format
    // Replace Q16Product function call by SMULL/LSR/ORR sequence
    return FIXED(48.0/17.0) - Q16Product(divisor, FIXED(32.0/17.0)) ;
    }

Q16 __attribute__((weak)) InitialEstimate(Q16 divisor)
    {
    Q16 estimate ;
    int zeros ;

    // Replace LeadingZeros by CLZ instruction
    zeros    = LeadingZeros(divisor) ;
    divisor  = Normalize(divisor, zeros) ;
    estimate = NormalizedEstimate(divisor) ;
    return Denormalize(estimate, zeros) ;
    }

Q16 __attribute__((weak)) Reciprocal(Q16 divisor)
    {
    Q16 curr, diff ;

    curr = InitialEstimate(divisor) ;
    do  
        {
        Q16 prev, temp ;

        // Replace FIXED macros by equivalent constant in Q16 format.
        // Replace Q16Product function call by SMULL/LSR/ORR sequence.

        prev = curr ;
        temp = FIXED(2.0) - Q16Product(divisor, prev) ;
        curr = Q16Product(temp, prev) ;
        diff = curr - prev ;

        // The value of diff may be positive or negative, but since
        // our Q16 datatype is unsigned, a negative value will look 
        // like a very large unsigned magnitude. This allows the
        // testing -0.001 < diff < +0.001 with a single comparison:

        } while ((diff + FIXED(0.001)) > 2*FIXED(0.001)) ;

    return curr ;
    }

#pragma GCC pop_options

typedef struct
    {
    uint8_t *           table ;
    uint16_t            Width ;
    uint16_t            Height ;
    } sFONT ;

// These functions should NOT be re-written in assembly:
static void             DrawAxis(unsigned col0, unsigned row0) ;
static void             Error(char *function) ;
static void             PlotReciprocal(unsigned col0, unsigned row0) ;
static void             SanityCheck(void) ;
static void             SetFontSize(sFONT *Font) ;

extern sFONT            Font8, Font12, Font16, Font20, Font24 ;

#define FONT_LABEL      Font12

#define HDR_ROWS        48
#define FTR_ROWS        12
#define MAX_ROWS        (YPIXELS - HDR_ROWS - FTR_ROWS)

int main()
    {
    unsigned col0, row0 ;

    InitializeHardware(HEADER, "Lab 11D: Q16 Reciprocal") ;
    SanityCheck() ;

    col0 = XPIXELS / 2 ;
    row0 = HDR_ROWS + MAX_ROWS/2 ;
    DrawAxis(col0, row0) ;
    PlotReciprocal(col0, row0) ;

    return 0 ;
    }

static void SanityCheck(void)
    {
    if (LeadingZeros(0x0000FFFF)        != 0x00000010)  Error("LeadingZeros") ;
    if (Normalize(0x2710199A, 2)        != 0x00009C40)  Error("Normalize") ;
    if (Normalize(0x00000001, 31)       != 0x00008000)  Error("Normalize") ;
    if (Denormalize(0x0000FFFF,  8)     != 0x00FFFF00)  Error("Denormalize") ;
    if (Denormalize(0x0000FFFF, 24)     != 0x000000FF)  Error("Denormalize") ;
    if (NormalizedEstimate(0x0000C000)  != 0x1696A)     Error("NormalizedEstimate") ;
    }

static void Error(char *function)
    {
#   define MAX(x,y) ((x > y) ? (x) : (y))
    sFONT *font = &Font16 ;
    static char label[] = "Error in Function" ;
    int length = MAX(strlen(function), sizeof(label) - 1) ;
    int width  = length * font->Width + 10 ;
    int height = 2*font->Height + 10 ;
    int xlft   = (XPIXELS - width) / 2 ;
    int ytop   = 100 ;

    SetForeground(COLOR_RED) ;
    FillRect(xlft, ytop, width, height) ;
    SetForeground(COLOR_BLACK) ;
    DrawRect(xlft, ytop, width, height) ;
    SetFontSize(font) ;
    SetForeground(COLOR_WHITE) ;
    SetBackground(COLOR_RED) ;
    DisplayStringAt(xlft + 5, ytop + 5, label) ;
    DisplayStringAt(xlft + 5, ytop + font->Height + 5, function) ;
    exit(0) ;
    }

static void DrawAxis(unsigned col0, unsigned row0)
    {
    unsigned col, row ;

    SetFontSize(&FONT_LABEL) ;
    SetForeground(COLOR_BLACK) ;

    DrawHLine(col0 - 110, row0, 220) ; 
    for (col = col0 - 100; col <= col0 + 100; col += 10)
        {
        DrawVLine(col, row0 - 4, 8) ;
        }
    DisplayStringAt(col0 - 100 - (5*FONT_LABEL.Width)/2, row0 - FONT_LABEL.Height - 4, "-1.0") ;
    DisplayStringAt(col0 -  50 - (5*FONT_LABEL.Width)/2, row0 - FONT_LABEL.Height - 4, "-0.5") ;
    DisplayStringAt(col0 +  50 - (5*FONT_LABEL.Width)/2, row0 + 6, "+0.5") ;
    DisplayStringAt(col0 + 100 - (5*FONT_LABEL.Width)/2, row0 + 6, "+1.0") ;

    DrawVLine(col0, row0 - 110, 220) ; 
    for (row = row0 - 100; row <= row0 + 100; row += 10)
        {
        DrawHLine(col0 - 4, row, 8) ;
        }
    DisplayStringAt(col0 + 6, row0 + 100 - FONT_LABEL.Height/2 + 1, "-10") ;
    DisplayStringAt(col0 + 6, row0 +  50 - FONT_LABEL.Height/2 + 1, " -5") ;
    DisplayStringAt(col0 - 3*FONT_LABEL.Width - 6, row0 -  50 - FONT_LABEL.Height/2 + 1, " +5") ;
    DisplayStringAt(col0 - 3*FONT_LABEL.Width - 6, row0 - 100 - FONT_LABEL.Height/2 + 1, "+10") ;
    }

static void PlotReciprocal(unsigned col0, unsigned row0)
    {
    float x ;

    SetForeground(COLOR_RED) ;
    x = 0.1 ;
    while (x < 1.0)
        {
        Q16 divisor, inverse ;
        int col, row ;
        float dx, y ;

        divisor = FIXED(x) ;
        inverse = Reciprocal(divisor) ;
        y = FLOAT(inverse) ;

        col = 0.5 + 100 * (x/01.0) ;
        row = 0.5 + 100 * (y/10.0) ;
        DrawCircle(col0 + col, row0 - row, 1) ;
        DrawCircle(col0 - col, row0 + row, 1) ;
        dx = 0.01 * x ;
        if (dx > 0.1) dx = 0.1 ;
        x += dx ;
        }
    }

static void SetFontSize(sFONT *Font)
    {
    extern void BSP_LCD_SetFont(sFONT *) ;
    BSP_LCD_SetFont(Font) ;
    }

int __attribute__((weak)) LeadingZeros(Q16 x)
    {
    int n ;

    for (n = 0; n < 32; n++)
        {
        if ((int32_t) x < 0) break ;
        x <<= 1 ;
        }

    return n ;
    }

Q16 __attribute__((weak)) Q16Product(Q16 mpier, Q16 mcand)
    {
    uint64_t product64 = ((uint64_t) mpier) * ((uint64_t) mcand) ;
    return (Q16) (product64 >> 16) ;
    }

