#pragma once
#include "math.h"

static const double e  = 2.71828182846;
static const double pi = 3.14159265358;
static const double rad = 180.0 / pi;

template<typename T> struct Complex
{
	T Re, Im;
	Complex() : Re(0), Im(0) {}
	Complex(const T& x, const T& y) : Re(x), Im(y) {}

	Complex conjugate() const
	{
		return Complex(Re, -Im);
	}
	Complex operator- () const
	{
		return Complex(-Re, -Im);
	}
	Complex operator* (const T& scale) const
	{
		return Complex(Re * scale, Im * scale);
	}
	Complex operator/ (const T& scale) const
	{
		return Complex(Re / scale, Im / scale);
	}
	Complex& operator*= (const T& scale)
	{
		return *this = Complex(Re * scale, Im * scale);
	}
	Complex& operator/= (const T& scale)
	{
		return *this = Complex(Re / scale, Im / scale);
	}
	Complex operator+ (const Complex& other) const
	{
		return Complex(Re + other.Re, Im + other.Im);
	}
	Complex operator- (const Complex& other) const
	{
		return Complex(Re - other.Re, Im - other.Im);
	}
	Complex operator* (const Complex& other) const
	{
		return Complex(Re*other.Re - Im*other.Im, Re*other.Im + Im*other.Re);
	}
	Complex operator/ (const Complex& other) const
	{
		return *this * other.conjugate() / other.mod_sqr();
	}
	Complex operator^ (const size_t& power) const
	{
		if (power == 0) return Complex(1, 0);
		if (power == 1) return *this;
		Complex p = this->operator^(power / 2);
		if (power & 0x1)
			return p * p * *this;
		else
			return p * p;
	}
	Complex& operator+= (const Complex& other)
	{
		return *this = Complex(Re + other.Re, Im + other.Im);
	}
	Complex& operator-= (const Complex& other)
	{
		return *this = Complex(Re - other.Re, Im - other.Im);
	}
	Complex& operator*= (const Complex& other)
	{
		return *this = Complex(Re*other.Re - Im*other.Im, Re*other.Im + Im*other.Re);
	}
	Complex& operator/= (const Complex& other)
	{
		return *this = *this * other.conjugate() / other.mod_sqr();
	}
	Complex& operator^= (const size_t& power)
	{
		if (power == 0) return *this = Complex(1, 0);
		if (power == 1) return *this;
		Complex p = this->operator^(power / 2);
		if (power & 0x1)
			return *this = p * p * *this;
		else
			return *this = p * p;
	}
	T mod() const
	{
		return sqrt(Re * Re + Im * Im);
	}
	T mod_sqr() const
	{
		return Re * Re + Im * Im;
	}
	T arg() const
	{
		if (Re == 0) {
			if (Im == 0) return 0;
			return Im > 0 ? pi / 2 : -pi / 2;
		}
		return Re > 0 ? (atan(Im / Re)) : (Im > 0 ? (pi + atan(Im / Re)) : (-pi + atan(Im / Re)));
	}
	T arg_deg() const
	{
		return arg() * rad;
	}
};

template<typename T> Complex<T> operator* (const T& scale, const Complex<T>& num)
{
	return num * scale;
}
template<typename T> Complex<T> operator/ (const T& scale, const Complex<T>& num)
{
	return num / scale;
}
template<typename T> Complex<T> exp(const Complex<T>& in)
{
	return Complex<T>(cos(in.Im), sin(in.Im)) * exp(in.Re);
}
