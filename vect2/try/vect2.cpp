#include "vect2.hpp"

//constructors
    vect2::vect2()
    {
        x = 0;
        y = 0;
    }
    vect2::vect2(int num1, int num2)
    {
        x = num1;
        y = num2;
    }
    
    //overloads
    vect2& vect2::operator=(const vect2& other)
    {
        if(this != &other)
        {
            this->x = other.x;
            this->y = other.y;
        }
            
        return(*this);
    }

    vect2::vect2(const vect2& copy)
    {
        *this = copy;
    }

    vect2& vect2::operator++()
    {
        ++x;
        ++y;
        return(*this);
    }

    vect2 vect2::operator++(int)
    {
        vect2 temp = *this;
        ++(*this);
        return(temp);
    }
    vect2& vect2::operator--()
    {
        --x;
        --y;
        return(*this);
    }

    vect2 vect2::operator--(int)
    {
        vect2 temp = *this;
        --(*this);
        return(temp);
    }
    
    vect2& vect2::operator+=(const vect2& other)
    {
        x = x + other.x; 
        y = y + other.y;
        
        return(*this);
    }

    vect2& vect2::operator-=(const vect2& other)
    {
        x = x - other.x; 
        y = y - other.y;
        
        return(*this);
    }

    vect2& vect2::operator*=(int num)
    {
        x = x * num; 
        y = y * num;
        
        return(*this);
    }
    vect2 vect2::operator*(int num) const
    {
        vect2 temp;
        temp.x = x * num; 
        temp.y = y * num;
        
        return(temp);
    }

    vect2 operator*(int num, const vect2& obj)
{
	vect2 temp(obj);
	temp *= num;
	return(temp);
}

    vect2 vect2::operator+(const vect2& other) const
    {
        vect2 temp = *this;
        temp += other;
        return(temp);
    }

    vect2 vect2::operator-(const vect2& other) const
    {
        vect2 temp = *this;
        temp -= other;
        return(temp);
    }

    vect2 vect2::operator-() const
    {
        vect2 temp = *this;
        temp.x *= -1;
        temp.y *= -1;
        return(temp);
    }


    bool vect2::operator==(const vect2& other) const
    {
        return (x == other.x && y == other.y);
    }

    bool vect2::operator!=(const vect2& other) const
    {
        return !(*this == other);
    }

    int vect2::operator[](int index) const
    {
        if(index == 0)
            return x;
        return y;
    }

    int& vect2::operator[](int index)
    {
        if(index == 0)
            return x;
        return y;
    }

    std::ostream& operator<<(std::ostream& os, const vect2& obj)
    {
        os << "{" << obj[0] << ", " << obj[1] << "}";
        return os;
    }

    vect2::~vect2(){}

    // stream

