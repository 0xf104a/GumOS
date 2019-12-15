#include "object.hpp"

Object::Object(void (*_display)(void))
{
   this->display=_display;
}
void Object::draw(void){
    this->display();
}
Object::~Object()
{
}