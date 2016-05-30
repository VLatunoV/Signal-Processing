#pragma once
#include "complex.h"
#include "cyclic.h"

class DFT
{
	DFT(const DFT&);
	DFT& operator= (const DFT&);
protected:
	Complex<float>* coeff;
	size_t N;
public:
	DFT();
	~DFT();
	void Initialize(size_t);
	void Transform(const Cyclic<float>&, Complex<float>*) const;
};

class FFT : public DFT
{
	FFT(const FFT&);
	FFT& operator= (const FFT&);
protected:
	size_t p;
	Complex<float>* working;

	void _transform(const Cyclic<float>&, size_t, size_t, Complex<float>*, Complex<float>*);
public:
	FFT();
	~FFT();
	void Initialize(size_t);
	void Transform(const Cyclic<float>&, Complex<float>*);
};
