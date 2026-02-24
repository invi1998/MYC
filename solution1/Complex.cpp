#include "Complex.h"

Complex::Complex(double real, double imag) : real(real), imag(imag) {}
Complex::Complex(const Complex& other) : real(other.real), imag(other.imag) {}
Complex& Complex::operator=(const Complex& other) {
    if (this != &other) {
        real = other.real;
        imag = other.imag;
    }
    return *this;
}

double Complex::getReal() const {
    return real;
}

double Complex::getImag() const {
    return imag;
}

void Complex::setReal(double real) {
    this->real = real;
}

void Complex::setImag(double imag) {
    this->imag = imag;
}

Complex Complex::operator+(const Complex& other) const {
    return Complex(real + other.real, imag + other.imag);
}

Complex Complex::operator-(const Complex& other) const {
    return Complex(real - other.real, imag - other.imag);
}

Complex Complex::operator*(const Complex& other) const {
    // (a + bi)(c + di) = (ac - bd) + (ad + bc)i
    return Complex(real * other.real - imag * other.imag,
                   real * other.imag + imag * other.real);
}

Complex Complex::operator/(const Complex& other) const {
    // (a + bi) / (c + di) = [(a + bi)(c - di)] / (c^2 + d^2)
    double denominator = other.real * other.real + other.imag * other.imag;
    return Complex((real * other.real + imag * other.imag) / denominator,
                   (imag * other.real - real * other.imag) / denominator);
}

std::ostream& operator<<(std::ostream& os, const Complex& c)
{
    os << "(" << c.real << " + " << c.imag << "i)";
    return os;
}

std::istream& operator>>(std::istream& is, Complex& c)
{
    std::cout << "请输入实部: ";
    is >> c.real;
    std::cout << "请输入虚部: ";
    is >> c.imag;
    return is;
}
