#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "maincycle.h"
#include <QMessageBox>
#include <QDebug>
#include <QPalette>
#include <QColor>
#include <QFile>
#include <QTextStream>
#include <QString>

#include <wiringPi.h>
#include <iostream>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    // Set the UI
    ui->setupUi(this);
    uiButtonAction(false, false, false);

    // Configure the statusbar
    updateStatusBar("Press \"start\" to begin the test...", QColor(Qt::yellow));

    // create an instance of MainCycle
    initMainCycle();
}

MainWindow::~MainWindow()
{
    delete ui;

    if (mainCycle != nullptr)
    {
        delete mainCycle;
        mainCycle = nullptr;
    }
}

void MainWindow::initMainCycle()
{
    int index = 0;

    mainCycle = new MainCycle(this);

    if(mainCycle != nullptr)
    {
        mainCycle->Init();

        if (!mainCycle->b_InputData.mfile->exists()) {
            ui->label->setText("Number of cycles: " + QString::number(index));
        } else {
            long readWriteAction = QIODevice::ReadOnly;
            mainCycle->ReadWiteInTheFile(&index, (QIODevice::OpenModeFlag) readWriteAction);
            ui->label->setText("Number of cycles: " + QString::number(index));
        }

        // Connect signal/slot
        connect(mainCycle, SIGNAL(valueChanged(int)), this, SLOT(onValueChanged(int)));

        connect(mainCycle, SIGNAL(SignalProcessFinished(bool)), this, SLOT(SlotprocessFinished(bool)), Qt::ConnectionType::DirectConnection);
    }
}

void MainWindow::onValueChanged(int count)
{
    ui->label->setText("Number of cycles: " + QString::number(count));
}

void MainWindow::SlotprocessFinished(bool processFinished)
{
    exitThePro = processFinished;
    if(exitThePro)
    {
        // Update the UI
        uiButtonAction(false, false, false);
        // Configure the statusbar
        updateStatusBar("Stopped...", QColor(Qt::red));
    }
}

void MainWindow::uiButtonAction(bool startButton, bool otherButtons, bool intermediateProcess)
{
    qDebug() << "uiButtonAction Called";
    if(startButton)
    {
        ui->buttonStart->setDisabled(true);
        ui->buttonStop->setDisabled(false);
    }
    else
    {
        if(intermediateProcess)
        {
            ui->buttonStart->setDisabled(true);
            ui->buttonStop->setDisabled(true);
        } else
        {
            ui->buttonStart->setDisabled(false);
            ui->buttonStop->setDisabled(true);
        }
    }

    ui->buttonReset->setDisabled(otherButtons);
    ui->buttonSave->setDisabled(otherButtons);
    ui->lineEditSteps->setDisabled(otherButtons);
    ui->lineEditStepWidth->setDisabled(otherButtons);
    ui->lineEditPeriod->setDisabled(otherButtons);
}

void MainWindow::on_buttonStart_clicked()
{
    qDebug() << "Cycle started...";

    // Update the UI
    uiButtonAction(true, true, false);

    // Update the global variable
    //isRunning = true;

    // Configure the statusbar
    updateStatusBar("Running ...", QColor(Qt::green));

    if(mainCycle == nullptr)
    {
        initMainCycle();
    }

    if(mainCycle != nullptr)
    {
        // Start the thread
        mainCycle->Stop = false;
        exitThePro = false;
        mainCycle->start();
    }
}

void MainWindow::on_buttonStop_clicked()
{
    qDebug() << "Cycle stopped...";

    // Update the UI
    uiButtonAction(false, true, true);

    // Update the global variable
    //isRunning = false;

    // Configure the statusbar
    updateStatusBar("Stopping, wait for finish...", QColor(Qt::yellow));

    // Interrupt the thread
    mainCycle->Stop = true;

    //while(true) {
        //if(exitThePro)
        //{
            //qDebug() << "Click on Start...";
            //break;
        //}
    //}

}


void MainWindow::on_buttonReset_clicked()
{
    qDebug() << "Reset button clicked...";

    // Verify is the user really wants to reset the counter
    QMessageBox::StandardButton reply = QMessageBox::question(this,
                                                              "Reset Counter", "Are you sure you want to reset the counter?",
                                                              QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        qDebug() << "Counter is resetted...";
        // Reset the counter file
        // Save i to the counter.txt file
        long readWriteAction = QIODevice::WriteOnly;
        int i = 0;
        mainCycle->ReadWiteInTheFile(&i, (QIODevice::OpenModeFlag) readWriteAction);
        ui->label->setText("Number of cycles: 0");
    } else {
        qDebug() << "Reset abort...";
    }
}

void MainWindow::updateStatusBar(QString message, QColor color) {

    ui->statusbar->showMessage(message);
    ui->statusbar->setAutoFillBackground(true);
    QPalette p = statusBar()->palette();
    p.setColor(statusBar()->backgroundRole(), color);
    statusBar()->setPalette(p);

}


void MainWindow::on_buttonSave_clicked()
{
   // Save the parameters
   mainCycle->b_InputData.numOfSteps = ui->lineEditSteps->text().toInt();
   mainCycle->b_InputData.stepDuration = ui->lineEditStepWidth->text().toInt();
   mainCycle->b_InputData.timeBetweenSteps = ui->lineEditPeriod->text().toInt();
   mainCycle->b_InputData.compressTime = ui->lineEditCompressTime->text().toInt();
}

void MainWindow::on_actionQuit_triggered()
{
    QString info_text = "Completing last cycle before closing...";
    QMessageBox::information(this, "Quitting appliction", info_text);
    while (!mainCycle->b_InputData.cycleFinished) {
        // Do nothing
    }
    QCoreApplication::quit();
}

void MainWindow::on_actionAbout_triggered()
{
    QString about_text;
        about_text  = "Author:  Thpir\n";
        about_text += "Date:    05-01-2023\n";
        about_text += "(C) Seal Compressor V1.0.1 (R)";
        QMessageBox::about(this, "About My Notepad", about_text);
}
