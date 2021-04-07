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
#include "library.h"
#include "graphics.h"
#include "touch.h"

#pragma GCC push_options
#pragma GCC optimize ("O0")

uint64_t __attribute__((weak)) TireDiam(uint32_t W, uint32_t A, uint32_t R)
	{
	uint64_t diameter ;

	((uint32_t *) &diameter)[1] = R + (A*W)/1270 ;
	((uint32_t *) &diameter)[0] = (A*W) % 1270 ;

	return diameter ;
	}

uint64_t __attribute__((weak)) TireCirc(uint32_t W, uint32_t A, uint32_t R)
	{
	uint64_t diameter, circumference ;
	uint32_t numerator ;

	diameter = TireDiam(W, A, R) ;

	numerator = (4987290*((uint32_t *) &diameter)[1] + 3927*((uint32_t *) &diameter)[0]) ;
	
	((uint32_t *) &circumference)[1] = numerator / 1587500 ;
	((uint32_t *) &circumference)[0] = numerator % 1587500 ;

	return circumference ;
	}

#pragma GCC pop_options

typedef enum {FALSE = 0, TRUE = 1} BOOL ;

typedef struct
	{
	char *		lbl ;
	int			xpos ;
	int			ypos ;
	int * 		pval ;
	char * 		(*text)(int) ;
	int			min ;
	int			max ;
	} ADJUST ;

// Public fonts defined in run-time library
typedef struct
	{
	const uint8_t *		table ;
	const uint16_t		Width ;
	const uint16_t		Height ;
	} sFONT ;

extern sFONT Font8, Font12, Font16, Font20, Font24 ;

#define	CPU_CLOCK_SPEED_MHZ	168

#define	FONT_INP			Font20
#define	FONT_OUT			Font16
#define	FONT_ADJ			Font16
#define	FONT_VAL			Font16

#define	VERT_INP_OFF		((FONT_INP.Height - FONT_ADJ.Height)/2)
#define	VERT_OUT_OFF		((FONT_OUT.Height - FONT_VAL.Height)/2)

#define	ADJOFF_XLABEL		0
#define	ADJOFF_XMINUS		(ADJOFF_XLABEL + 13*FONT_ADJ.Width + 3)
#define	ADJOFF_XVALUE		(ADJOFF_XMINUS + 1*FONT_INP.Width)
#define	ADJOFF_XPLUS		(ADJOFF_XVALUE + 3*FONT_INP.Width)

#define	ENTRIES(a)			(sizeof(a)/sizeof(a[0]))

#define	MS32BITS(a)			((uint32_t *) &a)[1]
#define	LS32BITS(a)			((uint32_t *) &a)[0]

#define	LEFT_MARGIN			10

#define	ADJUST_WXPOS		LEFT_MARGIN
#define	ADJUST_WYPOS		75

#define	ADJUST_AXPOS		LEFT_MARGIN
#define	ADJUST_AYPOS		(ADJUST_WYPOS + 30)

#define	ADJUST_RXPOS		LEFT_MARGIN
#define	ADJUST_RYPOS		(ADJUST_AYPOS + 30)

#define	DIAM_LBL_XPOS		LEFT_MARGIN
#define	DIAM_VAL_XPOS		(ADJOFF_XMINUS + 18)
#define	DIAM_YPOS			(ADJUST_RYPOS + 60)

#define	CIRC_LBL_XPOS		LEFT_MARGIN
#define	CIRC_VAL_XPOS		DIAM_VAL_XPOS
#define	CIRC_YPOS			(DIAM_YPOS + 30)

#define	RVPM_LBL_XPOS		LEFT_MARGIN
#define	RVPM_VAL_XPOS		CIRC_VAL_XPOS
#define	RVPM_YPOS			(CIRC_YPOS + 30)

#define	RSLT_XPOS			(DIAM_LBL_XPOS - 5)
#define	RSLT_YPOS			(DIAM_YPOS - 5)
#define	RSLT_HSIZ			(XPIXELS - 10)
#define	RSLT_VSIZ			(RVPM_YPOS - DIAM_YPOS + FONT_VAL.Height + 10)

#define	TS_XFUDGE			-4
#define	TS_YFUDGE			-4

static BOOL		Adjusted(void) ;
static BOOL		Between(uint32_t min, uint32_t val, uint32_t max) ;
static void		Delay(uint32_t msec) ;
static void		DisplayAdjusts(void) ;
static void		DisplayResults(void) ;
static char *	GetA(int Aindex) ;
static char *	GetR(int Rindex) ;
static uint32_t	GetTimeout(uint32_t msec) ;
static char *	GetW(int Windex) ;
static void		InitializeDisplay(void) ;
static void		InitializeTouchScreen(void) ;
static void		SetFontSize(sFONT *Font) ;


static int W[] = {165, 175, 185, 195, 205, 215, 225, 235, 245, 255, 265, 275, 285, 295, 305, 315, 325, 335} ;
static int A[] = {25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85} ;
static int R[] = {13, 14, 15, 16, 17, 18} ;

static int Windex = 0 ;
static int Aindex = 8 ;
static int Rindex = 2 ;

static ADJUST adjusts[] =
	{
	{"  Tire Width:", ADJUST_WXPOS, ADJUST_WYPOS, &Windex, GetW, 0, ENTRIES(W)-1},
	{"Aspect Ratio:", ADJUST_AXPOS, ADJUST_AYPOS, &Aindex, GetA, 0, ENTRIES(A)-1},
	{"Rim Diameter:", ADJUST_RXPOS, ADJUST_RYPOS, &Rindex, GetR, 0, ENTRIES(R)-1}
	} ;

int main(void)
	{
	uint32_t delay1, delay2 ;

	InitializeHardware(NULL, "Lab 5D: Automobile Tires") ;
	InitializeTouchScreen() ;
	InitializeDisplay() ;

	delay1 = delay2 = 0 ;
	while (1)
		{
		DisplayResults() ;
		Delay(delay1) ;
		delay1 = delay2 ;
		while (1)
			{
			if (TS_Touched())
				{
				if (Adjusted())
					{
					delay2 = 30 ;
					break ;
					}
				}
			else delay1 = 500 ;
			}
		}

	return 0 ;
	}

static void DisplayResults(void)
	{
	uint64_t tire_diam, tire_circ ;
	float diam, circ, rev_per_mi ;
	char text[20] ;

	SetFontSize(&FONT_OUT) ;
	SetForeground(COLOR_BLACK) ;
	SetBackground(COLOR_YELLOW) ;

	tire_diam = TireDiam(W[Windex], A[Aindex], R[Rindex]) ;
	diam = (float) MS32BITS(tire_diam) + (float) LS32BITS(tire_diam) / 1270.0 ;
	sprintf(text, "%6.2f", diam) ;
	DisplayStringAt(DIAM_VAL_XPOS, DIAM_YPOS, text) ;

	tire_circ = TireCirc(W[Windex], A[Aindex], R[Rindex]) ;
	circ = (float) MS32BITS(tire_circ) + (float) LS32BITS(tire_circ) / 1587500.0 ;
	sprintf(text, "%6.2f", circ) ;
	DisplayStringAt(CIRC_VAL_XPOS, CIRC_YPOS, text) ;

	rev_per_mi = 5280 * 12 / circ ;
	sprintf(text, "%6.2f", rev_per_mi) ;
	DisplayStringAt(RVPM_VAL_XPOS, RVPM_YPOS, text) ;
	}

static BOOL Adjusted(void)
	{
	ADJUST *adj ;
	int which, x, y ;

	x = TS_GetX() + TS_XFUDGE ;
	y = TS_GetY() + TS_YFUDGE ;

	adj = adjusts ;
	for (which = 0; which < ENTRIES(adjusts); which++, adj++)
		{
		if (Between(adj->ypos, y, adj->ypos + FONT_INP.Height - 1))
			{
			if (Between(adj->xpos + ADJOFF_XMINUS, x, adj->xpos + ADJOFF_XMINUS + FONT_INP.Width - 1))
				{
				if (*adj->pval > adj->min) --*adj->pval ;
				break ;
				}

			if (Between(adj->xpos + ADJOFF_XPLUS, x, adj->xpos + ADJOFF_XPLUS + FONT_INP.Width - 1))
				{
				if (*adj->pval < adj->max) ++*adj->pval ;
				break ;
				}
			}
		}

	if (which == ENTRIES(adjusts)) return FALSE ;

	DisplayAdjusts() ;
	return TRUE ;
	}

static char *GetW(int Windex)
	{
	static char text[4] ;
	sprintf(text, "%3d", W[Windex]) ;
	return text ;
	}

static char *GetA(int Aindex)
	{
	static char text[4] ;
	sprintf(text, "%3d", A[Aindex]) ;
	return text ;
	}

static char *GetR(int Rindex)
	{
	static char text[4] ;
	sprintf(text, "%3d", R[Rindex]) ;
	return text ;
	}

static void InitializeDisplay(void)
	{
	int which ;
	ADJUST *adj ;

	adj = adjusts ;
	for (which = 0; which < ENTRIES(adjusts); which++, adj++)
		{
		SetForeground(COLOR_BLACK) ;
		SetBackground(COLOR_WHITE) ;
		SetFontSize(&FONT_ADJ) ;
		DisplayStringAt(adj->xpos + ADJOFF_XLABEL, adj->ypos + VERT_INP_OFF, adj->lbl) ;
		SetFontSize(&FONT_INP) ;

		SetForeground(COLOR_WHITE) ;
		SetBackground(COLOR_BLACK) ;
		DisplayChar(adj->xpos + ADJOFF_XMINUS, adj->ypos, '-') ;

		SetForeground(COLOR_BLACK) ;
		SetBackground(COLOR_WHITE) ;
		DrawRect(adj->xpos + ADJOFF_XMINUS - 1, adj->ypos - 1, 5*FONT_INP.Width + 1, FONT_INP.Height + 1) ;

		SetForeground(COLOR_WHITE) ;
		SetBackground(COLOR_BLACK) ;
		DisplayChar(adj->xpos + ADJOFF_XPLUS, adj->ypos, '+') ;
		}

	SetForeground(COLOR_YELLOW) ;
	FillRect(RSLT_XPOS, RSLT_YPOS, RSLT_HSIZ, RSLT_VSIZ) ;
	SetForeground(COLOR_BLACK) ;
	DrawRect(RSLT_XPOS, RSLT_YPOS, RSLT_HSIZ, RSLT_VSIZ) ;

	SetFontSize(&FONT_VAL) ;
	SetForeground(COLOR_BLACK) ;
	SetBackground(COLOR_YELLOW) ;
	DisplayStringAt(DIAM_LBL_XPOS, DIAM_YPOS + VERT_OUT_OFF, "Tire Diameter:") ;
	DisplayStringAt(CIRC_LBL_XPOS, CIRC_YPOS + VERT_OUT_OFF, "Circumference:") ;
	DisplayStringAt(RVPM_LBL_XPOS, RVPM_YPOS + VERT_OUT_OFF, "Revs Per Mile:") ;

	DisplayAdjusts() ;
	}

static void DisplayAdjusts(void)
	{
	ADJUST *adj ;
	int which ;

	SetFontSize(&FONT_INP) ;
	adj = adjusts ;
	for (which = 0; which < ENTRIES(adjusts); which++, adj++)
		{
		SetForeground(COLOR_WHITE) ;
		SetBackground(*adj->pval < adj->max ? COLOR_DARKGREEN : COLOR_RED) ;
		DisplayChar(adj->xpos + ADJOFF_XPLUS, adj->ypos, '+') ;
		SetBackground(*adj->pval > adj->min ? COLOR_DARKGREEN : COLOR_RED) ;
		DisplayChar(adj->xpos + ADJOFF_XMINUS, adj->ypos, '-') ;

		SetForeground(COLOR_BLACK) ;
		SetBackground(COLOR_WHITE) ;
		DisplayStringAt(adj->xpos + ADJOFF_XVALUE, adj->ypos, (*adj->text)(*adj->pval)) ;
		}
	}

static void InitializeTouchScreen(void)
	{
	static char *message[] =
		{
		"If this message remains on",
		"the screen, there is an",
		"initialization problem with",
		"the touch screen. This does",
		"NOT mean that there is a",
		"problem with your code.",
		" ",
		"To correct the problem,",
		"disconnect and reconnect",
		"the power.",
		NULL
		} ;
	char **pp ;
	unsigned x, y ;

	x = 25 ;
	y = 100 ;
	for (pp = message; *pp != NULL; pp++)
		{
		DisplayStringAt(x, y, *pp) ;
		y += 12 ;
		}
	TS_Init() ;
	ClearDisplay() ;
	}

static void SetFontSize(sFONT *Font)
	{
	extern void BSP_LCD_SetFont(sFONT *) ;
	BSP_LCD_SetFont(Font) ;
	}

static uint32_t GetTimeout(uint32_t msec)
	{
	uint32_t cycles = 1000 * msec * CPU_CLOCK_SPEED_MHZ ;
	return GetClockCycleCount() + cycles ;
	}

static void Delay(uint32_t msec)
	{
	uint32_t timeout = GetTimeout(msec) ;
	while ((int) (timeout - GetClockCycleCount()) > 0) ;
	}

static BOOL Between(uint32_t min, uint32_t val, uint32_t max)
	{
	return (min <= val && val <= max) ;
	}

