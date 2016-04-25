#pragma once

static const double e  = 2.71828182846;
static const double pi = 3.14159265358;
static const double rad = 180.0 / pi;

struct Complex
{
	double Re, Im;
	Complex();
	Complex(const double&, const double&);

	Complex conjugate() const;
	Complex operator- () const;
	Complex operator* (const double&) const;
	Complex operator/ (const double&) const;
	Complex operator+ (const Complex&) const;
	Complex operator- (const Complex&) const;
	Complex operator* (const Complex&) const;
	Complex operator/ (const Complex&) const;
	Complex operator^ (const size_t&) const;
	Complex& operator+= (const Complex&);
	Complex& operator-= (const Complex&);
	Complex& operator*= (const Complex&);
	Complex& operator/= (const Complex&);
	Complex& operator^= (const size_t&);
	double mod() const;
	double mod_sqr() const;
	double arg() const;
	double arg_deg() const;
};

Complex operator* (const double&, const Complex&);
Complex operator/ (const double&, const Complex&);
Complex exp(const Complex&);
