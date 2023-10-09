#ifndef _TYPES_H
#define _TYPES_H

#include <stdlib.h>
#include <stdint.h>

typedef enum
{
	FALSE,
	TRUE
}BOOL;

#define LOCAL static
#define PRIVATE static
#define PUBLIC
#define TSMCALL
#define SPECIAL
#define MACRO
#define NUL (VOID *)0
#define EMPTY ~0
#define ASIZE(a) (sizeof(a)/sizeof((a)[0]))
#define FMUL32(a) ( ( a ) << 5 )

typedef uint8_t BYTE;
typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef void VOID;
typedef char CHAR;
typedef int16_t SHORT;
typedef uint16_t USHORT;
typedef int32_t	INT;
typedef uint32_t UINT;

#define random(x) rap_random(x)
extern int rap_random(int x);

#endif