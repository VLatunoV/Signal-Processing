#pragma once
#include "complex.h"
#include "cyclic.h"

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
	void Transform(const Cyclic<double>&, Complex*) const;
};

class FFT : public DFT
{
	FFT(const FFT&);
	FFT& operator= (const FFT&);
protected:
	size_t p;
	Complex* working;

	void _transform(const Cyclic<double>&, size_t, size_t, Complex*, Complex*);
public:
	FFT();
	~FFT();
	void Initialize(size_t);
	void Transform(const Cyclic<double>&, Complex*);
};
