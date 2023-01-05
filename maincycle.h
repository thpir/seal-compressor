#ifndef MAINCYCLE_H
#define MAINCYCLE_H

#include <QThread>
#include <stdio.h>
#include <QFile>

class MainCycle : public QThread
{
    Q_OBJECT
public:
    explicit MainCycle(QObject *parent = 0, bool b = false);
    ~MainCycle();

    void run();
    void Init();
    void ReadWiteInTheFile(int* i, QIODevice::OpenModeFlag qIODevice);

    typedef struct
    {
        int numOfSteps; // Amount of steps the motor should take in one movement forward or backwards
        int stepDuration; // How long the pulse should remain high in us
        int timeBetweenSteps; // Time between each pulse send to the stepper driver in ms
        int compressTime; // Time that the seal should be compressed
        bool cycleFinished; // Boolean to check wether a cycle is running or finished
        QFile* mfile;        //the file for writing the cycle can be located here
    } stInput;

    stInput b_InputData;

    // if Stop = true, the thread will break out of the loop, and will be disposed
    bool Stop;

signals:
    // To communicate with Gui Thread we need to emit a signal
    void valueChanged(int);
    void SignalProcessFinished(bool);

public slots:

private:
    void doActionOnPin(int pinNum);
    int cycleCounts;
};

#endif // MAINCYCLE_H
