#include "global.h"

namespace global
{
    int numOfSteps = 20; // Amount of steps the motor should take in a movement forward or backwards
    int stepDuration = 50; // How long the pulse should remain high in us
    int timeBetweenSteps = 7; // Time between each pulse send to the stepper driver in ms
    int compressTime = 5; // Time that the seal should be compressed
    bool cycleFinished = true; // Boolean to check wether a cycle is running or finished
}
