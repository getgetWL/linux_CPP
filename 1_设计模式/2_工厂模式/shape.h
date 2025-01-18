#pragma once
#define PI 3.14

//多边形抽象类
class Shape
{
public:
    virtual float area() = 0;
    virtual float perimeter() = 0;
    virtual ~Shape() {};
};

//长方形
class Rectangle : public Shape
{
private:
    float height;
    float width;

public:
    Rectangle(float _height,float _width):height(_height),width(_width){};
    //重写虚函数
    virtual float area()
    {
        return height * width;
    }
    virtual float perimeter()
    {
        return 2 * (height + width);
    }
};

class Circle : public Shape
{
private:
    float m_r;

public:
    Circle(float r):m_r(r){};
    virtual float area()
    {
        return PI * m_r * m_r;
    }
    virtual float perimeter()
    {
        return 2 * PI * m_r;
    }
};