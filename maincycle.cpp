#include "maincycle.h"
#include "global.h"
#include <QDebug>
#include <QFile>
#include <QString>
#include <QTextStream>
#include <QThread>

#include <iostream>
#include <wiringPi.h>

MainCycle::MainCycle(QObject *parent, bool b):
    QThread(parent), Stop(b)
{
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

   // setting the counter to the last stored value
    int i = 0;
    QFile file("counter.txt");
    if(!file.exists()) {
        // The file doesn't exist
        qDebug() << "File doesn't exist yet";
        file.close();

        // So we create the file
        qDebug() << "Creating file...";
        QFile file("counter.txt");
        file.open(QIODevice::WriteOnly);

        // Set i to 0 and store to the file
        i = 0;
        QTextStream out(&file);
        QString text = QString::number(i);
        out << text;
        file.flush();
        file.close();

    } else {
        // The file does exist!
        qDebug() << "File does exist!";
        if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            QString text = in.readAll();
            try {
                i = text.toInt();
            } catch(...) {
               i = 0;
               QTextStream out(&file);
               QString text = QString::number(i);
               out << text;
               file.flush();
            }
        }
        file.close();
    }

    // Starting the 'infinite' loop
    while(true) {

        /* Check if the operator pressed stop. Since we have no encoder on the stepper motor
         * it is not advised to break the loop while the stepper motor is NOT in home position.
         * If we would just stop the cycle when the motor is runnig, the next time the motor starts,
         * he will not correctly compress the seal. We could fix this by addind an encoder but for
         * this test stand we wanted to keep the hardware cost low. If we still want to stop as quick as possible,
         * close the running program without pressing stop first.*/
        if(this->Stop) break;
        qDebug() << "Cycle running...";

        // Engage actuator
        // Set the boolean to indicate a cycle is running.
        // If we don't do this and we're quiting the application,
        // we forget the stepper motors acutal location and the
        // compression will be performed faulty when the users starts a new cycle
        global::cycleFinished = false;
        qDebug() << "engaging actuator";
        digitalWrite(2, LOW);
        for(int n = 0; n < global::numOfSteps; n++) {
            qDebug() << "highPIN";
            digitalWrite(0, HIGH);
            qDebug() << "100ms delay";
            QThread::usleep(global::stepDuration);
            //sleep_for(milliseconds(100));
            qDebug() << "lowPIN";
            digitalWrite(0, LOW);
            qDebug() << "100ms delay";
            QThread::msleep(global::timeBetweenSteps);
            //sleep_for(milliseconds(100));
        }

        // Timeout
        QThread::msleep(global::compressTime * 1000);

        // Disengage actuator
        qDebug() << "disengaging actuator";
        digitalWrite(2, HIGH);
        for(int n = 0; n < global::numOfSteps; n++) {
            qDebug() << "highPIN";
            digitalWrite(1, HIGH);
            qDebug() << "100ms delay";
            QThread::usleep(global::stepDuration);
            //sleep_for(milliseconds(100));
            qDebug() << "lowPIN";
            digitalWrite(1, LOW);
            qDebug() << "100ms delay";
            QThread::msleep(global::timeBetweenSteps);
            //sleep_for(milliseconds(100));
        }

        global::cycleFinished = true;

        // Timeout
        QThread::msleep(5000);

        // Update the counter
        i++;
        emit valueChanged(i);

        // Save i to the counter.txt file
        QFile file("counter.txt");
        file.open(QIODevice::WriteOnly);
        QTextStream out(&file);
        QString text = QString::number(i);
        out << text;
        file.flush();
        file.close();

    }
}
