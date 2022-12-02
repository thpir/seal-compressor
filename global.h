#ifndef GLOBAL_H
#define GLOBAL_H


namespace global
{
    extern int numOfSteps; // Amount of steps the motor should take in a movement forward or backwards
    extern int stepDuration; // How long the pulse should remain high in us
    extern int timeBetweenSteps; // Time between each pulse send to the stepper driver in ms
}

#endif // GLOBAL_H
