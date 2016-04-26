#pragma once
#include "complex.h"

class DFT
{
	DFT(const DFT&);
	DFT& operator= (const DFT&);
protected:
	Complex* coeff;
	size_t N;
public:
	DFT();
	~DFT();
	void Initialize(size_t);
	void Transform(const double*, Complex*) const;
};

class FFT : public DFT
{
	FFT(const FFT&);
	FFT& operator= (const FFT&);
protected:
	size_t p;
	Complex* working;

	void _transform(const double*, size_t, size_t, Complex*, Complex*);
public:
	FFT();
	~FFT();
	void Initialize(size_t);
	void Transform(const double*, Complex*);
};
