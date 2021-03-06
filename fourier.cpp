#include "fourier.h"
#include "math.h"

DFT::DFT() : coeff(nullptr), N(0) {}
DFT::~DFT()
{
	delete[] coeff;
}
void DFT::Initialize(size_t size)
{
	N = size;
	delete[] coeff;
	coeff = new Complex<float>[N];
	double Im = -2 * pi / N;
	for (size_t i = 0; i < N; ++i)
	{
		coeff[i] = exp(Complex<float>(0, Im * i));
	}
}
void DFT::Transform(const Cyclic<float>& arr, Complex<float>* result) const
{
	for (size_t i = 0; i < N; ++i)
	{
		for (size_t j = 0; j < N; ++j)
		{
			result[i] += Complex<float>(arr[j], 0) * coeff[(i * j) % N];
		}
		result[i] /= N;
	}
}
FFT::FFT(): DFT(), p(0), working(nullptr){}
FFT::~FFT()
{
	delete[] working;
}
void FFT::Initialize(size_t p)
{
	DFT::Initialize((size_t)1 << p);
	this->p = p;
	delete[] working;
	working = new Complex<float>[N];
}
void FFT::Transform(const Cyclic<float>& arr, Complex<float>* result)
{
	if (N == 1)
	{
		result[0] = Complex<float>(arr[0], 0);
		return;
	}
	_transform(arr, N, 1, result, working);
	for (size_t i = 0; i < N; ++i)
	{
		result[i] /= N;
	}
}
void FFT::_transform(const Cyclic<float>& arr, size_t size, size_t stride, Complex<float>* result, Complex<float>* working)
{
	if (size == 2)
	{
		result[0].Re = (arr[0] + arr[stride]);
		result[1].Re = (arr[0] - arr[stride]);
		result[0].Im = result[1].Im = 0.0;
		return;
	}
	size_t half = size / 2;
	_transform(arr, half, stride * 2, result, working);
	_transform(arr + stride, half, stride * 2, result + half, working + half);

	for (size_t i = 0; i < size; ++i)
	{
		working[i] = result[i];
	}
	for (size_t i = 0; i < half; ++i)
	{
		result[i] = (working[i] + coeff[stride * i] * working[half + i]);
		result[i + half] = (working[i] - coeff[stride * i] * working[half + i]);
	}
}
