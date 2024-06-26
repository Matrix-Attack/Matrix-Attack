#ifndef text_h
#define text_h

#define TF_FONT_SMALL			0x01
#define TF_FONT_LARGE			0x02
#define TF_LENGTH_CHAT_OUTPUT	0x04
#define TF_LENGTH_CHAT_INPUT	0x08
#define TF_LENGTH_IP_INPUT		0x10
#define TF_LENGTH_64			0x20
#define TF_LENGTH_STATS			0x40
#define TF_COLOR_WHITE			0x80
#define TF_COLOR_RED			0x100
#define TF_COLOR_GREEN			0x200
#define TF_COLOR_BLUE			0x400
#define TF_COLOR_YELLOW			0x800
#define TF_COLOR_LIGHT_BLUE		0x1000
#define TF_SPACING_0			0x2000
#define TF_SPACING_1			0x4000
#define TF_SPACING_2			0x8000
#define TF_SPACING_3			0x10000

#define TF_COLOR	(TF_COLOR_WHITE | TF_COLOR_RED | TF_COLOR_GREEN | TF_COLOR_BLUE | TF_COLOR_YELLOW | TF_COLOR_LIGHT_BLUE)
#define TF_FONT		(TF_FONT_SMALL | TF_FONT_LARGE )
#define TF_LENGTH	(TF_LENGTH_CHAT_OUTPUT | TF_LENGTH_CHAT_INPUT | TF_LENGTH_STATS)
#define TF_SPACING  (TF_SPACING_0 | TF_SPACING_1 | TF_SPACING_2)

#endif