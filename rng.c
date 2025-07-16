/**************************************************************
***************************************************************
*** 
***   rng.c
***
***   Jason Olejarz
***   2010-09-14
***
***   This file contains the random number generator functions for ising3d.
***
***************************************************************
**************************************************************/

#include "includes.h"

const unsigned long int a=2862933555777941757, b=1013904243;

unsigned long int x;

static int i=0;

//	algorithm:	x_(i+1) = x(i)*a + b

void ising3d_rng_seed (unsigned long int seed)
{
	x=seed;
	return;
}

unsigned long int ising3d_rng ()
{
	x=x*a+b;
	return x;
}

unsigned long int ising3d_rng_state ()
{
	return x;
}

