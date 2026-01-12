#ifndef VEC2_HPP
#define VEC2_HPP
#include <iostream>

class vect2 {
    private:
    int x;
    int y;

    public:
    //constructors
    vect2();
    vect2(int num1, int num2);
    vect2(const vect2 &copy);

    //overloads
    int operator[](int index) const;
    int& operator[](int index);
    vect2& operator=(const vect2& other);
    vect2& operator++();

    vect2 operator++(int);
    vect2& operator--();
    vect2 operator--(int);
    vect2& operator+=(const vect2& other);
    vect2& operator-=(const vect2& other);
    vect2& operator*=(int num);
    vect2 operator*(int num) const;
    vect2 operator+(const vect2& other) const;
    vect2 operator-(const vect2& other) const;
    vect2 operator-() const;
    bool operator==(const vect2& other) const;
    bool operator!=(const vect2& other) const;

    
    //destruct
    ~vect2();
};
std::ostream& operator<<(std::ostream& os, const vect2& obj);
vect2 operator*(int num, const vect2& obj);


#endif