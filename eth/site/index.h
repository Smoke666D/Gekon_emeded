#ifndef INC_HTML_H_
#define INC_HTML_H_
/*----------------------- Includes -------------------------------------*/
#include	"stm32f2xx_hal.h"
/*------------------------ Define --------------------------------------*/
#define  HTML_LENGTH    582U    // 0 Kb
#define  HTML_ENCODING  1U
static const unsigned char data__index_html[HTML_LENGTH] = {
		0x1f,	0x8b,	0x8,	0x0,	0xd9,	0xb6,	0x58,	0x5f,	0x2,	0xff,	0x8d,	0x53,	0xe1,	0x6e,	0xd3,	0x30,	
		0x10,	0xfe,	0xcf,	0x53,	0x78,	0x99,	0x40,	0x20,	0xd5,	0x6b,	0x53,	0x75,	0x62,	0x24,	0x69,	0x25,	
		0x90,	0x78,	0x2,	0x84,	0xf8,	0xed,	0x26,	0x4e,	0x63,	0xe6,	0xd8,	0x95,	0xe3,	0xb6,	0x94,	0xaa,	
		0x52,	0x35,	0x40,	0xf0,	0x3,	0xd8,	0x13,	0x0,	0x4f,	0x80,	0xd4,	0xb1,	0x16,	0xca,	0x4a,	0xcb,	
		0x2b,	0xd8,	0x6f,	0xc4,	0x39,	0x29,	0x63,	0xe3,	0xd7,	0x94,	0xc8,	0xce,	0xdd,	0x7d,	0x77,	0xfe,	
		0xee,	0xfc,	0x25,	0xda,	0x4b,	0x64,	0xac,	0xc7,	0x7d,	0x8a,	0x32,	0x9d,	0xf3,	0x4e,	0xe4,	0x56,	
		0xc4,	0x89,	0xe8,	0xb5,	0x3d,	0x35,	0xf0,	0xc0,	0xa6,	0x24,	0xe9,	0x44,	0x9a,	0x69,	0x4e,	0x3b,	
		0xe6,	0xab,	0xd9,	0x98,	0xa5,	0x9d,	0x99,	0x73,	0x33,	0x37,	0x1b,	0x74,	0x67,	0xff,	0xa8,	0xe9,	
		0x37,	0x43,	0xf3,	0x9,	0xac,	0x85,	0x59,	0x9a,	0xef,	0x10,	0x9d,	0xdb,	0x53,	0x64,	0x3f,	0x5c,	
		0xc2,	0x96,	0xf6,	0xc4,	0xac,	0xcc,	0x85,	0x99,	0x47,	0xf5,	0xaa,	0x44,	0x94,	0x53,	0x4d,	0x50,	
		0x9c,	0x11,	0x55,	0x50,	0xdd,	0xf6,	0x6,	0x3a,	0xc5,	0x47,	0xde,	0xce,	0x2b,	0x48,	0x4e,	0xdb,	
		0xde,	0x90,	0xd1,	0x51,	0x5f,	0x2a,	0xed,	0xc5,	0x52,	0x68,	0x2a,	0x0,	0x34,	0x62,	0x89,	0xce,	
		0xda,	0x9,	0x1d,	0xb2,	0x98,	0xe2,	0xd2,	0xa8,	0x31,	0xc1,	0x34,	0x23,	0x1c,	0x17,	0x31,	0xe1,	
		0xb4,	0xed,	0xd7,	0x8a,	0x4c,	0x31,	0x71,	0x8c,	0xb5,	0xc4,	0x29,	0xd3,	0x6d,	0x21,	0x6b,	0x39,	
		0x20,	0x72,	0x0,	0xc,	0xd8,	0xf5,	0xea,	0xa4,	0xdf,	0xe7,	0x14,	0xe7,	0xb2,	0xcb,	0x60,	0x1b,	
		0xd1,	0x2e,	0x6,	0x7,	0x2e,	0xa9,	0xfd,	0x3b,	0xef,	0x7a,	0x9b,	0x37,	0xc8,	0x8f,	0x49,	0x9f,	
		0x74,	0xaf,	0x56,	0x18,	0xd3,	0x2,	0xd2,	0xa,	0x3d,	0x86,	0x8e,	0xdd,	0x40,	0x27,	0x19,	0x65,	
		0xbd,	0x4c,	0x7,	0x7e,	0xa3,	0x71,	0x3b,	0x4c,	0x1,	0x85,	0x53,	0x92,	0x33,	0x3e,	0xe,	0xa,	
		0x22,	0xa,	0x5c,	0x50,	0xc5,	0xd2,	0x90,	0x33,	0x41,	0xf1,	0x5f,	0xdc,	0x81,	0x7f,	0x18,	0xba,	
		0xf2,	0xc7,	0x4c,	0x63,	0x4d,	0x5f,	0x68,	0x5c,	0xb0,	0x97,	0x14,	0x93,	0xe4,	0xf9,	0xa0,	0xd8,	
		0x55,	0xb9,	0x8c,	0x92,	0x3e,	0xce,	0x20,	0x89,	0xbb,	0x44,	0x1c,	0x4b,	0x2e,	0x55,	0xa0,	0x7a,	
		0x5d,	0x72,	0xb7,	0x51,	0x43,	0xbb,	0xf7,	0x5e,	0x88,	0xf3,	0x2,	0xcb,	0x21,	0x55,	0x29,	0x97,	
		0x23,	0x5c,	0xd2,	0xa,	0x84,	0x14,	0x34,	0x2c,	0x62,	0x25,	0x39,	0xef,	0x12,	0x55,	0x8d,	0xb5,	
		0x72,	0x4e,	0xbb,	0x32,	0x19,	0x4f,	0x72,	0xa2,	0x7a,	0x4c,	0x4,	0x8d,	0xf0,	0x6,	0xd4,	0x4b,	
		0xb7,	0x63,	0x18,	0xf8,	0x8a,	0xe6,	0x61,	0x45,	0x62,	0x1f,	0xb4,	0x71,	0xd8,	0x7c,	0x10,	0x96,	
		0xf4,	0x9,	0xd0,	0x13,	0x41,	0xc,	0xc3,	0xa1,	0x2a,	0x9c,	0x66,	0x7e,	0x2d,	0x6b,	0x4d,	0x76,	
		0xb0,	0xfb,	0x4d,	0xf7,	0x84,	0xd3,	0xa8,	0x5e,	0xcd,	0x2b,	0xaa,	0x57,	0x92,	0x4b,	0xd8,	0x10,	
		0xd4,	0xe7,	0x77,	0xcc,	0x67,	0xfb,	0xe,	0x34,	0x74,	0xe6,	0x54,	0x84,	0xcc,	0xf,	0xb8,	0x92,	
		0x73,	0x3b,	0xb3,	0xaf,	0xe0,	0xeb,	0xc2,	0xac,	0x90,	0xf9,	0xd,	0xf7,	0xb4,	0x32,	0x6b,	0xb3,	
		0x5,	0xf9,	0x2d,	0xe1,	0xde,	0x56,	0xf6,	0x74,	0xf,	0x4a,	0xf8,	0x90,	0xdb,	0xea,	0x98,	0x2f,	
		0xff,	0x47,	0xcd,	0x12,	0x52,	0xc0,	0xfc,	0x6,	0x1,	0xa7,	0xd8,	0x45,	0xe9,	0xde,	0x98,	0xed,	
		0x1e,	0x2,	0x1d,	0x2f,	0x21,	0x72,	0x66,	0xdf,	0xc0,	0xba,	0x0,	0xe8,	0x2f,	0xb3,	0xad,	0xc0,	
		0xb,	0x38,	0x6a,	0x6d,	0x3f,	0xda,	0xb7,	0x50,	0xfd,	0xc4,	0xbe,	0x47,	0x60,	0x6e,	0xcd,	0x6,	
		0x3e,	0x67,	0xb0,	0xaf,	0xe1,	0x1,	0xb1,	0x94,	0x48,	0xf4,	0xf4,	0xc9,	0x23,	0xe4,	0x58,	0x5d,	
		0xa5,	0x59,	0xe5,	0x3c,	0x7e,	0xf6,	0x10,	0xd9,	0xd7,	0xe0,	0xfe,	0x69,	0xd6,	0x7,	0x40,	0xb0,	
		0x5,	0x8d,	0x96,	0x2d,	0xd6,	0xdd,	0xb8,	0x5d,	0xd7,	0xee,	0xf7,	0xbb,	0xf5,	0x7,	0x89,	0x2c,	
		0x42,	0x6f,	0x8f,	0x3,	0x0,	0x0,	};
#endif /* INC_INDEX_H_ */