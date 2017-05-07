#include "math.h"

double fabs(double x)
{
	return __builtin_fabs(x);
}

double sqrt(double x)
{
	return __builtin_sqrt(x);
}

double floor(double x)
{
	return __builtin_floor(x);
}

double cos(double x)
{
	return __builtin_cos(x);
}

int abs(int x)
{
	return x > 0 ? x : -x;
}

double atan2(double x, double y)
{
	return __builtin_atan2(x,y);
}

double exp(double x)
{
	return __builtin_exp(x);
}


double pow(double x, double y)
{
	return __builtin_pow(x, y);
}

double log(double x)
{
	return __builtin_log(x);
}

double log10(double x)
{
	return __builtin_log10(x);
}