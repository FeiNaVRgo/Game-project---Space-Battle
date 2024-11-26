#include "Application.h"

int main() {
   //TODO -- namespace for components
   //TODO -- texture manager class - will work on strings
   //TODO -- more components should be tied to weapons 
    //i think i will rewrite a lot of code here - like logic for collision(allowing more than just fixed rectangles) + i want inputs to be processed by event manager + learn callbacks and more
    //events are complicated
    
    Application::instance().run();
    return 0;
}
