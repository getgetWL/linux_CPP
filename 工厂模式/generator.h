#pragma once
#include "shape.h"
#include <iostream>
using namespace std;
// 工厂抽象类
class ShapeGenerator
{
private:
    /*以抽象类作为私有成员 */
    Shape *pshape{nullptr};

protected:
    /*此处写为保护类型，为了防止类外调用 */
    virtual Shape *createShape() = 0;
    string shapeName;

public:
    virtual ~ShapeGenerator()
    {
        if (pshape)
        {
            delete pshape;
            cout << "析构~ShapeGenerator"<<endl;
            pshape = nullptr;
        }
    }
    /*统一接口*/
    float area()
    {
        if (pshape == nullptr)
        {
            pshape = createShape();
        }
        /*此处体现了多态特性*/
        return pshape->area();
    }

    float perimeter()
    {
        if (pshape == nullptr)
        {
            pshape = createShape();
        }
        return pshape->perimeter();
    }

    string getShapeName()
    {
        return shapeName;
    }
};

class RectangleGenerator : public ShapeGenerator
{
public:
    RectangleGenerator()
    {
        shapeName = "正方形";
    }

protected:
    virtual Shape *createShape()
    {
        float height;
        float width;
        cout << "请输入长方形的宽：";
        cin >> width;
        cout << "请输入长方形的高：";
        cin >> height;
        Shape *shape = new Rectangle(height, width);
        return shape;
    }
};

class CircleGenerator : public ShapeGenerator
{
public:
    CircleGenerator()
    {
        shapeName = "圆形";
    }

protected:
    virtual Shape *createShape()
    {
        float r;
        cout << "请输入圆的半径：";
        cin >> r;
        Shape *shape = new Circle(r);
        return shape;
    }
};
