#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include "maincycle.h"
#include "global.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    MainCycle *mainCycle = nullptr;

    // public boolean to track running process
    bool isRunning = false;

public slots:
    void onValueChanged(int);
    void slotProcessFinished(bool);

private slots:

    void on_buttonStart_clicked();

    void on_buttonStop_clicked();

    void on_buttonReset_clicked();

    void updateStatusBar(QString message, QColor color);

    void on_buttonSave_clicked();

    void on_actionQuit_triggered();

    void on_actionAbout_triggered();

private:
    Ui::MainWindow *ui;
    void uiButtonAction(bool startButton, bool otherButtons);
    bool exitThePro;
    void initMainCycle();
};
#endif // MAINWINDOW_H
