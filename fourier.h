#pragma once
#include "complex.h"

class DFT
{
	Complex* coeff;
	size_t N;
	DFT(const DFT&);
	DFT& operator= (const DFT&);
public:
	DFT() : coeff(nullptr), N(0){}
	~DFT();
	void Initialize(size_t);
	Complex* Transform(double*);
};
