#ifndef GUI_OBJECT_H
#define GUI_OBJECT_H
/*
Defines genric GUI object.
For example button/progressbar/etc.
*/
class Object
{
private:
    /* data */
    void (*display)(void);
public:
    Object(void (*display)(void));
    ~Object();
    void on_press(void);
    void on_activate(void);
    void draw(void);
};


#endif