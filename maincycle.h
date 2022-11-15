#ifndef MAINCYCLE_H
#define MAINCYCLE_H

#include <QThread>

class MainCycle : public QThread
{
    Q_OBJECT
public:
    explicit MainCycle(QObject *parent = 0, bool b = false);
    void run();

    // if Stop = true, the thread will break out of the loop, and will be disposed
    bool Stop;

signals:
    // To communicate with Gui Thread we need to emit a signal
    void valueChanged(int);

public slots:

};

#endif // MAINCYCLE_H
