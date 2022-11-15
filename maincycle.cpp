#include "maincycle.h"
#include <QDebug>
#include <QMutex>
#include <QFile>
#include <QString>
#include <QTextStream>

MainCycle::MainCycle(QObject *parent, bool b):
    QThread(parent), Stop(b)
{
}

// run() will be called when a thread starts
void MainCycle::run()
{

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

    // Starting the loop
    while(true) {
        QMutex mutex;
        // prevent other threads from changing the "stop" value
        mutex.lock();
        if(this->Stop) break;
        mutex.unlock();

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

        qDebug() << "Cycle running...";
        this->msleep(500);
    }
}
