#include "maincycle.h"
#include <QDebug>
#include <QString>
#include <QTextStream>
#include <QThread>

#include <iostream>
#include <wiringPi.h>

MainCycle::MainCycle(QObject *parent, bool b):
    QThread(parent), Stop(b)
{
}

MainCycle::~MainCycle()
{
    exit();
}

void MainCycle::ReadWiteInTheFile(int *counterInd, QIODevice::OpenModeFlag qIODevice)
{
    QFile* file = b_InputData.mfile;
    file->open(QIODevice::ReadWrite);

    if(file == nullptr)
    {
        b_InputData.mfile->setFileName("counter.txt");
        b_InputData.mfile->open(QIODevice::ReadWrite);
    }

    if(qIODevice == QIODevice::WriteOnly)
    {
        file->resize(0);
        QTextStream out(file);
        QString text = QString::number(*counterInd);
        out << text;
    }
    else if(qIODevice == QIODevice::ReadOnly)
    {
        QTextStream in(file);
        QString text = in.readAll();
        if(!text.isNull())
        {
            *counterInd = text.toInt();
        }
        else
        {
            *counterInd = 0;
            QTextStream out(file);
            QString text = QString::number(*counterInd);
            out << text;
        }
    }

    cycleCounts = *counterInd;

    file->flush();
    file->close();
    file = nullptr;
}

void MainCycle::doActionOnPin(int pinNum)
{
    for(int n = 0; n < b_InputData.numOfSteps; n++)
    {
        //qDebug() << "highPIN";
        digitalWrite(pinNum, HIGH);
        //qDebug() << "100ms delay";
        QThread::usleep(b_InputData.stepDuration);
        //qDebug() << "lowPIN";
        digitalWrite(pinNum, LOW);
        //qDebug() << "100ms delay";
        QThread::msleep(b_InputData.timeBetweenSteps);
    }
}

// run() will be called when a thread is started in mainwindow.cpp
void MainCycle::run()
{
    /* Three GPIO PINS are connected to the stepper driver.
     * One pin on the stepper driver reads a puls signal to move the stepper motor in clockwise direction,
     * the second one reads a puls signal to move the stepper motor in counter clockwise direction,
     * and another pin reads a boolean so the driver knows if it needs to read the counter clockwise or the clockwise puls signal.*/

    wiringPiSetup(); // Setup the GPIO library
    pinMode(0, OUTPUT); // Configure GPIO0 as an output = engage pulsgenerator
    pinMode(1, OUTPUT); // Configure GPIO1 as an output = disengage pulsgenerator
    pinMode(2, OUTPUT); // Configure QPIO2 as an output = directionPIN, 0 = engage, 1 = disengage

    using namespace std::this_thread;
    using namespace std::chrono;

    // Initializeing the variable to store the pinnumber and read/write action for saving the counter value
    int thePin = 0;
    long readWriteAction = 0;

    // Starting the 'infinite' loop
    while(true) {

        /* Check if the operator pressed stop. Since we have no encoder on the stepper motor
         * it is not advised to break the loop while the stepper motor is NOT in home position.
         * If we would just stop the cycle when the motor is runnig, the next time the motor starts,
         * he will not correctly compress the seal. We could fix this by addind an encoder but for
         * this test stand we wanted to keep the hardware cost low. If we still want to stop as quick as possible,
         * close the running program without pressing stop first.*/
        if(this->Stop)
        {
            // Save i to the counter.txt file
            readWriteAction = QIODevice::WriteOnly;
            ReadWiteInTheFile(&cycleCounts, (QIODevice::OpenModeFlag) readWriteAction);
            emit SignalProcessFinished(true);
            break;
        }

        qDebug() << "Cycle running...";

        // Engage actuator
        // Set the boolean to indicate a cycle is running.
        // If we don't do this and we're quiting the application,
        // we forget the stepper motors acutal location and the
        // compression will be performed faulty when the users starts a new cycle
        b_InputData.cycleFinished = false;
        qDebug() << "engaging actuator";
        thePin = 0;
        doActionOnPin(thePin);

        // Timeout
        QThread::msleep(b_InputData.compressTime * 1000);

        // Disengage actuator
        qDebug() << "disengaging actuator";
        thePin = 1;
        doActionOnPin(thePin);

        b_InputData.cycleFinished = true;

        // Timeout
        QThread::msleep(5000);

        // Update the counter
        cycleCounts++;
        emit valueChanged(cycleCounts);
    }
}

void MainCycle::Init()
{
    b_InputData.numOfSteps = 20; // Amount of steps the motor should take in one movement forward or backwards
    b_InputData.stepDuration = 55; // How long the pulse should remain high in us
    b_InputData.timeBetweenSteps = 9; // Time between each pulse send to the stepper driver in ms
    b_InputData.compressTime = 10; // Time that the seal should be compressed
    b_InputData.cycleFinished = true; // Boolean to check wether a cycle is running or finished
    b_InputData.mfile = new QFile;
    b_InputData.mfile->setFileName("counter.txt");

    cycleCounts = 0;
}
