#ifndef GLOBAL_H
#define GLOBAL_H


namespace global
{
    extern int numOfSteps; // Amount of steps the motor should take in one movement forward or backwards
    extern int stepDuration; // How long the pulse should remain high in us
    extern int timeBetweenSteps; // Time between each pulse send to the stepper driver in ms
    extern int compressTime; // Time that the seal should be compressed
    extern bool cycleFinished; // Boolean to check wether a cycle is running or finished
}

#endif // GLOBAL_H
