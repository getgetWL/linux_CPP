#include "shape.h"
#include "generator.h"
#include <unistd.h>

int main()
{
    ShapeGenerator *shape = new RectangleGenerator();
    /*此处体现了多态特性*/
    float area =shape->area();
    cout<<"这是："<<shape->getShapeName()<<" 面积："<<area<<endl;;

    shape = new CircleGenerator();
    area =shape->area();
    cout<<"这是："<<shape->getShapeName()<<" 面积："<<area<<endl;
    delete shape;
    return 0;
}