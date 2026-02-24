#ifndef COMPLEX_H
#define COMPLEX_H

#include <iostream>

class Complex {
public:
    Complex(double real = 0.0, double imag = 0.0);
    Complex(const Complex& other); // 复制构造函数
    Complex& operator=(const Complex& other); // 赋值运算符
    double getReal() const;
    double getImag() const;
    void setReal(double real);
    void setImag(double imag);
    Complex operator+(const Complex& other) const;
    Complex operator-(const Complex& other) const;
    Complex operator*(const Complex& other) const;
    Complex operator/(const Complex& other) const;

    // 声明友元函数
    friend std::ostream& operator<<(std::ostream& os, const Complex& c);    // 输出复数
    friend std::istream& operator>>(std::istream& is, Complex& c);          // 输入复数

private:
    double real;
    double imag;
};

#endif // COMPLEX_H