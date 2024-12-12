#include "shape.h"
#include "generator.h"
#include <unistd.h>
#include <memory>

int main()
{
    // ShapeGenerator *shape = new RectangleGenerator();
    shared_ptr<ShapeGenerator> RectangleShape = make_shared<RectangleGenerator>();
    /*此处体现了多态特性*/
    float area =RectangleShape->area();
    cout<<"这是："<<RectangleShape->getShapeName()<<" 面积："<<area<<endl;;

    shared_ptr<ShapeGenerator> CircleShape = make_shared<CircleGenerator>();
    area =CircleShape->area();
    cout<<"这是："<<CircleShape->getShapeName()<<" 面积："<<area<<endl;
    return 0;
}