#include "complex.h"
#include "math.h"

inline double abs(const double& in)
{
	return in >= 0 ? in : -in;
}

Complex::Complex(): Re(0), Im(0){}
Complex::Complex(const double& x, const double& y): Re(x), Im(y){}
Complex Complex::conjugate() const
{
	return Complex(Re, -Im);
}
Complex Complex::operator- () const
{
	return Complex(-Re, -Im);
}
Complex Complex::operator* (const double& scale) const
{
	return Complex(Re * scale, Im * scale);
}
Complex Complex::operator/ (const double& scale) const
{
	return Complex(Re / scale, Im / scale);
}
Complex& Complex::operator*= (const double& scale)
{
	return *this = Complex(Re * scale, Im * scale);
}
Complex& Complex::operator/= (const double& scale)
{
	return *this = Complex(Re / scale, Im / scale);
}
Complex Complex::operator+ (const Complex& other) const
{
	return Complex(Re + other.Re, Im + other.Im);
}
Complex Complex::operator- (const Complex& other) const
{
	return Complex(Re - other.Re, Im - other.Im);
}
Complex Complex::operator* (const Complex& other) const
{
	return Complex(Re*other.Re - Im*other.Im, Re*other.Im + Im*other.Re);
}
Complex Complex::operator/ (const Complex& other) const
{
	return *this * other.conjugate() / other.mod_sqr();
}
Complex Complex::operator^ (const size_t& power) const
{
	if (power == 0) return Complex(1, 0);
	if (power == 1) return *this;
	Complex p = this->operator^(power / 2);
	if (power & 0x1)
		return p * p * *this;
	else
		return p * p;
}
Complex& Complex::operator+= (const Complex& other)
{
	return *this = Complex(Re + other.Re, Im + other.Im);
}
Complex& Complex::operator-= (const Complex& other)
{
	return *this = Complex(Re - other.Re, Im - other.Im);
}
Complex& Complex::operator*= (const Complex& other)
{
	return *this = Complex(Re*other.Re - Im*other.Im, Re*other.Im + Im*other.Re);
}
Complex& Complex::operator/= (const Complex& other)
{
	return *this = *this * other.conjugate() / other.mod_sqr();
}
Complex& Complex::operator^= (const size_t& power)
{
	if (power == 0) return *this = Complex(1, 0);
	if (power == 1) return *this;
	Complex p = this->operator^(power / 2);
	if (power & 0x1)
		return *this = p * p * *this;
	else
		return *this = p * p;
}
double Complex::mod() const
{
	return sqrt(Re * Re + Im * Im);
}
double Complex::mod_sqr() const
{
	return Re * Re + Im * Im;
}
double Complex::arg() const
{
	if(Re == 0){
		if(Im == 0) return 0;
		return Im > 0 ? pi / 2 : -pi / 2;
	}
	return Re > 0 ? (atan(Im / Re)) : (Im > 0 ? (pi + atan(Im / Re)) : (-pi + atan(Im / Re)));
}
double Complex::arg_deg() const
{
	return arg() * rad;
}
Complex operator* (const double& scale, const Complex& num)
{
	return num * scale;
}
Complex operator/ (const double& scale, const Complex& num)
{
	return num / scale;
}
Complex exp(const Complex& in)
{
	return Complex(cos(in.Im), sin(in.Im)) * exp(in.Re);
}
