#ifndef	Blt2DopenGL_h
#define	Blt2DopenGL_h

void BltGLStyle(
	RECT	*prcSRC,
	RECT	*prcDST,
	float	ImageWidth,
	float	ImageHeight,
	long	ImageId);

void BltGLStyle(
	const int x,
	const int y,
	const RECT	*prcSRC,
	float	ImageWidth,
	float	ImageHeight,
	long	ImageId);


void BltGLStyle(
		int x,
		int y,
		int iScaleX,
		int iScaleY,
		const RECT	*prcDST,
		float	ImageWidth,
		float	ImageHeight,
		long	ImageId);

/*
void BltGLStyle(
		const float x,
		const float y,
		const float z,
		const RECT	*prcSRC,
		float	ImageWidth,
		float	ImageHeight,
		long	ImageId);
*/
#endif