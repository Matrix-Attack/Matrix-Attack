#ifndef defines_h
#define defines_h

#define SWAP(a,b)				{ (a)^=(b)^=(a)^=(b); }
#define ABS(a)					((a) > 0 ? (a) : -(a))
#define MIN(a, b)				(((a) < (b)) ? (a) : (b))
#define MAX(a, b)				(((a) > (b)) ? (a) : (b))
#define MIN3(a, b, c)			(MIN(MIN(a, b), c))
#define MAX3(a, b, c)			(MAX(MAX(a, b), c))
#define LIMIT(num, low, high)	(((num) < (low)) ? (low) : ((num) > (high)) ? (high) : (num))
#define INRANGE(num, low, high)	((num) >= (low) && (num) <= (high))



#endif