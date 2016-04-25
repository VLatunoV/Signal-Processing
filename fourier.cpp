#include "fourier.h"
#include "math.h"

DFT::~DFT()
{
	delete[] coeff;
}
void DFT::Initialize(size_t size)
{
	N = size;
	coeff = new Complex[N];
	double Im = -2 * pi / N;
	for (size_t i = 0; i < N; ++i)
	{
		coeff[i] = exp(Complex(0, Im * i));
	}
}
Complex* DFT::Transform(double* arr)
{
	if(N == 0)
		return NULL;
	Complex* result = new Complex[N];
	for (size_t i = 0; i < N; ++i)
	{
		for (size_t j = 0; j < N; ++j)
		{
			result[i] += Complex(arr[j], 0) * coeff[(i * j) % N];
		}
	}
	return result;
}
