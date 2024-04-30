//font.h
#ifndef font_h
#define font_h

typedef struct tagTextObj	TEXTOBJ, *P_TEXTOBJ;

//Gets, and sets our OpenGLModes (we need this to draw in 2D)
void SetOpenGLModes(U32 state);

//****FONT TO DO LIST:
//1.)  Can't think of anything other than new methods (or ways of using font flags)
//
//************

typedef class tagFont
{
	private:
		float	XPos, YPos;		// Current 2D Cursor Position
		float	fgColor[4];		// RGBA foreground color,	default white
		float	gdColor[4];		// RGBA gradient color,		default gray
		//float	bgColor[4];		// RGBA background color,	default gray
		float	Size;			// Size of text,			default 12
		float	Italic;			// Italic Amount (CAN BE NEGATIVE)
		
		//Booleans
		U32		bGradient;		// boolean gradient?		default 0
		U32		bShadow;		// boolean shadow text?		default 0
		U32		bBold;			// boolean bold text?		default 0

		//Vars that don't change
		float	tIncX;			// used for texture coords, x axis amount to move
		float	tIncY;			// used for texture coords, y axis amount to move
		U32		blockRow;		// characters per row
		U32		blockCol;		// characters per col	
		float	tPoints[257][2];// Character MAP coords (it has 2 floats, one for x and one for y)
		int		ViewPort[4];	// Used to deturmine if our text is out of bounds

		void WalkString(char *s);
		void DrawChar(char c);
	
	public:	
		int	 texID;			// texture id

		int  Initialize(int glTexID);	//init with default settings
		int  Initialize(char *fileName, U32 row, U32 col, U32 glTexID);//custom settings
		
		void DrawStaticString(float Xpos, float Ypos, char *s);
		void DrawString(float Xpos, float Ypos, char *s, ...);

		void Reset();								//Resets everything to default
		void SetGradientMode(U32 g){bGradient = g;} //Sets the Gradient mode
		void SetSize(float s){Size = s;}			//Sets the font size
		void SetItalics(float i){Italic = i;}		//Sets the Italics
		void SetColor(float r, float g, float b){fgColor[0]=r;fgColor[1]=g;fgColor[2]=b;}
//		void SetShadowColor(float r, float g, float b){bgColor[0]=r;bgColor[1]=g;bgColor[2]=b;}
		void SetGradientColor(float r, float g, float b){gdColor[0]=r;gdColor[1]=g;gdColor[2]=b;}
		void DrawStringFromTextObj(P_TEXTOBJ pTextObj);


}FONT, *P_FONT;

typedef struct tagTextObj
{
	char	String[64];		// Character String
	float	fgColor[4];		// rgba foreground color,	default white
	float	gdColor[4];		// rgba gradient color,		default gray
	
	U32		TypeFlags;		// Text Object TYPE flags (ULTRAAAA COOOOMMMBOOOO!!!!!)
	float	XPos, YPos;		// Current Position	
	float	Size;			// Current Size
	float	Italic;			// Current Italic

	float	XVel, YVel;		// X and Y Velocity	
	float	SizeVel;		// Size Velocity
	float	ItalicVel;		// Italic Velocity

	U32		Health;			//When Health == 0 the TextObject will die
//	U32		TimeLeft;		//Used to slow down updating (if need be)
	
	//Pointers
	U32 (*pUpdate)(struct tagTextObj *t);

	struct tagTextObj *pNext;
	struct tagTextObj *pPrev;

}TEXTOBJ, *P_TEXTOBJ;

//********************************TEXT FLAG DEFINES ***************************************
//**** FONT DEFINES *******
#define FONT_BLOCK_COL              16
#define FONT_BLOCK_ROW              16
#define FONT_GET_MODES              1 
#define FONT_RESTORE_MODES          2 
#define FONT_MAX_LEN                1024	// maximum chars to draw to the screen, used for buffers also 
#define FONT_TAB_SPACE              4		// spaces to draw for a tab, make option? 
#define FONT_ITOF                   (float) pow (255, -1)
#define FONT_ITALIC_AMOUNT          8		// italic amount in pixels
#define FONT_DEFAULT_SPEED			3		//in pixels
#define	FONT_DEFAULT_ITALIC			0		//in pixels
#define	FONT_DEFAULT_HEALTH			4000	//in milliseconds
#define	FONT_DEFAULT_TIMELEFT		15000	//in milliseconds
#define FONT_DEFAULT_FALPHA			1.0f	//Forground Alpha Value
#define FONT_DEFAULT_GALPHA			1.0f	//Gradient Alpha Value

//****** DEFINED TYPE FLAGS ***
#define TF_FAST				0x1		//Speed Flags
#define TF_SLOW				0x2
#define TF_SMALL			0x4		//Size Flags
#define TF_LARGE			0x8	
#define TF_SLIDING_UP		0x10	//Movement Flags
#define TF_SLIDING_DOWN		0x20
#define TF_SLIDING_LEFT		0x40
#define TF_SLIDING_RIGHT	0x80
#define TF_GROWING			0x100	//Scaling Flags
#define TF_SHRINKING		0x200
#define TF_WARBLING			0x400
#define TF_ITALICIZED_LEFT	0x800	//Italics Flags
#define TF_ITALICIZED_RIGHT	0x1000
#define TF_ITALIC_WARBLING  0x2000
#define TF_FADING			0x4000	//Fading Flags
#define TF_FADING_FAST		0x8000
#define TF_GRADIENT			0x10000	//Gradient Flags

//*Special* Type Flags (This flag is set to true if we are using a gradient)
// Note: These special defines are used to test if we have a group of flags set or not
#define TF_USING_ITALICS    (TF_ITALICIZED_LEFT|TF_ITALICIZED_RIGHT)
#define TF_USING_SCALING	(TF_GROWING | TF_SHRINKING | TF_WARBLING)
#define TF_USING_FADING		(TF_FADING  | TF_FADING_FAST)

//***Pre defined Move Methods ***
#define TF_COMBO (TF_MEDIUM | TF_SLIDING_RIGHT | TF_WARBLE | TF_ITALICZED_LEFT) 


//*** Defined COLOR FLAGS ***
#define CF_BLACK				0x1		//Solid Colors,( TOP Color for gradient )
#define CF_RED					0x2		
#define CF_GREEN				0x4
#define CF_BLUE					0x8
#define CF_HALF_RED				0x10	//Solid Half Colors, ( TOP Color for gradient )
#define CF_HALF_GREEN			0x20			
#define CF_HALF_BLUE			0x40
#define CF_WHITE				(CF_RED		| CF_GREEN		| CF_BLUE)
#define CF_LIGHT_RED			(CF_RED		| CF_HALF_GREEN | CF_HALF_BLUE)
#define CF_LIGHT_BLUE			(CF_BLUE	| CF_HALF_GREEN | CF_HALF_RED)
#define CF_LIGHT_GREEN			(CF_GREEN	| CF_HALF_BLUE  | CF_HALF_RED)
#define CF_GREY					(CF_HALF_RED| CF_HALF_GREEN | CF_HALF_BLUE)
#define CF_VIOLET				(CF_RED		| CF_BLUE)
#define CF_YELLOW				(CF_RED		| CF_GREEN)
#define CF_CYAN					(CF_BLUE	| CF_GREEN)
#define CF_ORANGE				(CF_RED		| CF_HALF_GREEN)

#define CF_BOT_BLACK			0x100	//Bottom Color, only used if bGradient = 1
#define CF_BOT_RED				0x200
#define CF_BOT_GREEN			0x400
#define CF_BOT_BLUE				0x800
#define CF_BOT_HALF_RED			0x1000	//Bottom Half Color, only used if bGradient = 1
#define CF_BOT_HALF_GREEN		0x2000
#define CF_BOT_HALF_BLUE		0x4000
#define CF_BOT_GREY				(CF_BOT_HALF_RED | CF_BOT_HALF_GREEN | CF_BOT_HALF_BLUE)
#define CF_BOT_WHITE			(CF_BOT_RED  | CF_BOT_GREEN | CF_BOT_BLUE )
#define CF_BOT_VIOLET			(CF_BOT_RED  | CF_BOT_BLUE )
#define CF_BOT_YELLOW			(CF_BOT_RED  | CF_BOT_GREEN)
#define CF_BOT_CYAN				(CF_BOT_BLUE | CF_BOT_GREEN)
#define CF_BOT_ORANGE			(CF_BOT_RED	 | CF_BOT_HALF_GREEN)

//#define CF_CYCLING_RAINBOW		0x10000	//These Color Methods overide all others if ORed
//#define CF_CYCLING_RED_OARNGE	0x20000

//*Special* Color Flags
// Note: These special defines are used to test if we have a group of flags set or not
#define CF_USING_BOT_COLOR (CF_BOT_BLACK|CF_BOT_RED|CF_BOT_GREEN|CF_BOT_BLUE|CF_BOT_GREY|CF_BOT_HALF_RED|CF_BOT_HALF_GREEN|CF_BOT_HALF_BLUE)
#define CF_USING_CYCLING   (CF_CYCLING_RAINBOW|CF_CYCLING_RED_OARNGE)

#endif	