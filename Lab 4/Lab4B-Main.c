/*
	This code was written to support the book, "ARM Assembly for Embedded Applications",
	by Daniel W. Lewis. Permission is granted to freely share this software provided
	that this notice is not removed. This software is intended to be used with a run-time
    library adapted by the author from the STM Cube Library for the 32F429IDISCOVERY 
    board and available for download from http://www.engr.scu.edu/~dlewis/book3.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <memory.h>
#include "library.h"
#include "graphics.h"

#pragma GCC push_options
#pragma GCC optimize ("O0")

void __attribute__((weak)) OffBy0(void *dst, const void *src) 
	{
	int i ;

	// Unroll this loop using .rept and .endr
	for (i = 0; i < 1000/4; i++)
		{
		*((uint32_t *) dst) = *((uint32_t *) src) ;
		src = (uint32_t *) src + 1 ;
		dst = (uint32_t *) dst + 1 ;
		}
	}

void __attribute__((weak)) OffBy1(void *dst, const void *src)
	{
	int i ;

	// Unroll this loop using .rept and .endr
	for (i = 0; i < 3; i++)
		{
		*((uint8_t *) dst) = *((uint8_t *) src) ;
		src = (uint8_t *) src + 1 ;
		dst = (uint8_t *) dst + 1 ;
		}

	// Unroll this loop using .rept and .endr
	for (i = 0; i < 1000/4 - 1; i++)
		{
		*((uint32_t *) dst) = *((uint32_t *) src) ;
		src = (uint32_t *) src + 1 ;
		dst = (uint32_t *) dst + 1 ;
		}

	// Unroll this loop using .rept and .endr
	for (i = 0; i < 1; i++)
		{
		*((uint8_t *) dst) = *((uint8_t *) src) ;
		src = (uint8_t *) src + 1 ;
		dst = (uint8_t *) dst + 1 ;
		}
	}

void __attribute__((weak)) OffBy2(void *dst, const void *src)
	{
	int i ;

	// Unroll this loop using .rept and .endr
	for (i = 0; i < 2; i++)
		{
		*((uint8_t *) dst) = *((uint8_t *) src) ;
		src = (uint8_t *) src + 1 ;
		dst = (uint8_t *) dst + 1 ;
		}

	// Unroll this loop using .rept and .endr
	for (i = 0; i < 1000/4 - 1; i++)
		{
		*((uint32_t *) dst) = *((uint32_t *) src) ;
		src = (uint32_t *) src + 1 ;
		dst = (uint32_t *) dst + 1 ;
		}

	// Unroll this loop using .rept and .endr
	for (i = 0; i < 2; i++)
		{
		*((uint8_t *) dst) = *((uint8_t *) src) ;
		src = (uint8_t *) src + 1 ;
		dst = (uint8_t *) dst + 1 ;
		}
	}

void __attribute__((weak)) OffBy3(void *dst, const void *src)
	{
	int i ;

	// Unroll this loop using .rept and .endr
	for (i = 0; i < 1; i++)
		{
		*((uint8_t *) dst) = *((uint8_t *) src) ;
		src = (uint8_t *) src + 1 ;
		dst = (uint8_t *) dst + 1 ;
		}

	// Unroll this loop using .rept and .endr
	for (i = 0; i < 1000/4 - 1; i++)
		{
		*((uint32_t *) dst) = *((uint32_t *) src) ;
		src = (uint32_t *) src + 1 ;
		dst = (uint32_t *) dst + 1 ;
		}

	// Unroll this loop using .rept and .endr
	for (i = 0; i < 3; i++)
		{
		*((uint8_t *) dst) = *((uint8_t *) src) ;
		src = (uint8_t *) src + 1 ;
		dst = (uint8_t *) dst + 1 ;
		}
	}

#pragma GCC pop_options

typedef enum { FALSE=0, TRUE=1 } BOOL ;

typedef struct
	{
	char *				label ;
	void				(*name)(void *dst, const void * src) ;
	unsigned			cycles ;
	BOOL				copyOK ;
	} FUNCTION ;

typedef struct
	{
	FUNCTION			*func[2] ;
	unsigned			xpos ;
	} TEST ;

// Public fonts defined in run-time library
typedef struct
	{
	const uint8_t *		table ;
	const uint16_t		Width ;
	const uint16_t		Height ;
	} sFONT ;

static BOOL				CopyOK(uint8_t *pdst, uint8_t *psrc) ;
static unsigned			Cycles(void (*func)(), void *dst, void *src) ;
static void				InitializeSource(uint8_t *src) ;
static void				Plot1Bar(char *label, unsigned xpos, unsigned cycles, unsigned max, uint32_t color) ;
static void				PrintVertical(unsigned xpos, unsigned ypos, sFONT *font, char *string) ;
static void				SetFontSize(sFONT *Font) ;

static uint32_t			src[1000/4+3] ;
static uint32_t			dst[1000/4+3] ;

extern sFONT			Font8, Font12, Font16, Font20, Font24 ;

#define	FONT_FAULT		Font16
#define	FONT_CYCLES		Font12
#define	FONT_LABEL		Font16

#define	ENTRIES(a)		(sizeof(a)/sizeof(a[0]))

#define	HDR_HEIGHT		48
#define	FTR_HEIGHT		12

#define	MAX_HEIGHT		230
#define	BAR_WIDTH		30
#define	BAR_GAP			4
#define	BAR_SPACING		18
#define	BAR_BOTTOM		20

#define	REF_COLOR		COLOR_ORANGE
#define	NEW_COLOR		COLOR_GREEN

#define	OFFBY1_XPOS		(XPIXELS - 6*BAR_WIDTH - 3*BAR_GAP - 2*BAR_SPACING)/2
#define	OFFBY2_XPOS		(OFFBY1_XPOS + 2*BAR_WIDTH + BAR_GAP + BAR_SPACING)
#define	OFFBY3_XPOS		(OFFBY2_XPOS + 2*BAR_WIDTH + BAR_GAP + BAR_SPACING)

int main()
	{
	static FUNCTION functions[] =
		{
		{"OffBy0", OffBy0}, {"OffBy1", OffBy1},
		{"OffBy0", OffBy0}, {"OffBy2", OffBy2},
		{"OffBy0", OffBy0}, {"OffBy3", OffBy3}
		} ;
	static TEST tests[] =
		{
		{{&functions[0], &functions[1]}, OFFBY1_XPOS},
		{{&functions[2], &functions[3]}, OFFBY2_XPOS},
		{{&functions[4], &functions[5]}, OFFBY3_XPOS}
		} ;
	unsigned t, max, xpos ;
	uint8_t *psrc, *pdst ;
	TEST *test ;

	InitializeHardware(HEADER, "Lab 4B: Address Alignment") ;

	max = 0 ;
	psrc = (uint8_t *) &src[1] ;
	pdst = (uint8_t *) &dst[1] ;
	test = &tests[0] ;
	for (t = 0; t < ENTRIES(tests); t++, test++)
		{
		unsigned f ;
		pdst++ ; psrc++ ;
		for (f = 0; f < 2; f++)
			{
			FUNCTION *function = test->func[f] ;
			InitializeSource(psrc) ;
			memset(dst, 0, sizeof(dst)) ;
			function->cycles = Cycles(function->name, pdst, psrc) ;
			if (function->cycles > max) max = function->cycles ;
			function->copyOK = CopyOK(pdst, psrc) ;
			}
		}

	test = &tests[0] ;
	for (t = 0; t < ENTRIES(tests); t++, test++)
		{
		uint32_t color = COLOR_YELLOW ;
		unsigned f ;

		xpos = test->xpos ;
		for (f = 0; f < 2; f++)
			{
			FUNCTION *function = test->func[f] ;
			Plot1Bar(function->label, xpos, function->cycles, max, function->copyOK ? color : COLOR_RED) ;
			xpos += BAR_WIDTH + BAR_GAP ;
			color = COLOR_GREEN ;
			}
		}

	SetForeground(COLOR_BLACK) ;
	DrawHLine(0, YPIXELS - BAR_BOTTOM, XPIXELS) ;

	return 0 ;
	}

static BOOL CopyOK(uint8_t *pdst, uint8_t *psrc)
	{
	uint8_t *p, *end ;

	for (p = (uint8_t *) dst; p < pdst; p++)
		{
		if (*p != 0) return FALSE ;
		}

	if (memcmp(pdst, psrc, 1000) != 0) return FALSE ;

	end = (uint8_t *) dst + sizeof(dst) ;
	for (p = pdst + 1000; p < end; p++)
		{
		if (*p != 0) return FALSE ;
		}

	return TRUE ;
	}

static unsigned Cycles(void (*func)(), void *dst, void *src)
	{
	uint32_t strt, stop ;

	strt = GetClockCycleCount() ;
	(*func)(dst, src) ;
	stop = GetClockCycleCount() ;
	return (unsigned) (stop - strt) ;
	}

static void Plot1Bar(char *label, unsigned xlft, unsigned cycles, unsigned max, uint32_t color)
	{
	unsigned height, ytop, xlbl ;
	float percent ;
	char text[20] ;

	percent = (float) cycles / max ;
	height = (unsigned) 0.5 + MAX_HEIGHT * percent;
	ytop = YPIXELS - BAR_BOTTOM - height ;
	SetForeground(color) ;
	FillRect(xlft, ytop, BAR_WIDTH, height) ;
	SetForeground(COLOR_BLACK) ;
	DrawRect(xlft, ytop, BAR_WIDTH, height) ;
	SetFontSize(&FONT_LABEL) ;
	xlbl = xlft + BAR_WIDTH/2 - FONT_LABEL.Height/2 + 1 ;
	PrintVertical(xlbl, YPIXELS - BAR_BOTTOM - 4, &FONT_LABEL, label) ;
	SetFontSize(&FONT_CYCLES) ;
	sprintf(text, "%u", cycles) ;
	xlft += (BAR_WIDTH - strlen(text)*FONT_CYCLES.Width)/2 ;
	DisplayStringAt(xlft, ytop - FONT_CYCLES.Height, text) ;
	}

static void PrintVertical(unsigned xpos, unsigned ypos, sFONT *font, char *string)
	{
	unsigned rlen, size, offset ;
	char *p ;

	rlen = (font->Width + 7) / 8 ;	
	size = font->Height * rlen ;
	offset = 8 * rlen - font->Width ;
	for (p = string; *p != '\0'; p++)
		{
		unsigned bit = 1 << (font->Width + offset - 1) ;
		unsigned col ;

		for (col = 0; col < font->Width; col++, bit >>= 1)
			{
			const uint8_t *bits = &font->table[(*p - ' ') * size] ;
			unsigned row ;

			for (row = 0; row < font->Height; row++, bits += rlen)
				{
				unsigned line ;

				switch (rlen)
					{
					default:
					case 3: line =  (bits[0] << 16) | (bits[1] << 8) | bits[2] ; break ;
					case 2: line =  (bits[0] << 8) | bits[1] ; break ;
					case 1: line =  bits[0] ; break ;
					}

				if ((line & bit) != 0) DrawPixel(xpos + row, ypos - col, COLOR_BLACK) ;
				}
			}

		ypos -= font->Width ;
		}
	}

static void InitializeSource(uint8_t *src)
	{
	unsigned word ;

	for (word = 0; word < 1000/4; word++)
		{
		((uint32_t *) src)[word] = GetRandomNumber() ;
		}
	}

static void SetFontSize(sFONT *Font)
	{
	extern void BSP_LCD_SetFont(sFONT *) ;
	BSP_LCD_SetFont(Font) ;
	}

