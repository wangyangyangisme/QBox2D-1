#ifndef EXAMPLEWORLD_H
#define EXAMPLEWORLD_H

#include "world.h"

class ExampleWorld : public QBox2DWorld {
public:
    ExampleWorld();
    void populate();
};


#endif // EXAMPLEWORLD_H
