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

    ui->setupUi(this);
    ui->buttonStart->setEnabled(true);
    ui->buttonStop->setDisabled(true);
    QString counter = "0";
    QFile file("counter.txt");
    if (!file.exists()) {
        ui->label->setText("Number of cycles: " + counter);
    } else {
        file.open(QIODevice::ReadOnly);
        QTextStream in(&file);
        counter = in.readAll();
        ui->label->setText("Number of cycles: " + counter);
    }
    file.close();

    // Configure the statusbar
    updateStatusBar("Press \"start\" to begin the test...", QColor(Qt::yellow));

    // create an instance of MainCycle
    mainCycle = new MainCycle(this);

    // connect signal/slot
    connect(mainCycle, SIGNAL(valueChanged(int)),
            this, SLOT(onValueChanged(int)));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onValueChanged(int count)
{
    ui->label->setText("Number of cycles: " + QString::number(count));
}

void MainWindow::on_buttonStart_clicked()
{
    qDebug() << "Cycle started...";

    ui->buttonStart->setDisabled(true);
    ui->buttonReset->setDisabled(true);
    ui->buttonStop->setEnabled(true);

    isRunning = true;

    // Configure the statusbar
    updateStatusBar("Running ...", QColor(Qt::green));

    // Start the thread
    mainCycle->Stop = false;
    mainCycle->start();

}

void MainWindow::on_buttonStop_clicked()
{
    qDebug() << "Cycle stopped...";

    ui->buttonStop->setDisabled(true);
    ui->buttonStart->setEnabled(true);
    ui->buttonReset->setEnabled(true);

    isRunning = false;

    // Configure the statusbar
    updateStatusBar("Stopped...", QColor(Qt::red));

    // Interrupt the thread
    mainCycle->Stop = true;

}


void MainWindow::on_buttonReset_clicked()
{
    qDebug() << "Reset button clicked...";

    QMessageBox::StandardButton reply = QMessageBox::question(this,
                                                              "Reset Counter", "Are you sure you want to reset the counter?",
                                                              QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        qDebug() << "Counter is resetted...";
        // Reset the counter file
        // Save i to the counter.txt file
        QFile file("counter.txt");
        file.open(QIODevice::WriteOnly);
        QTextStream out(&file);
        QString text = QString::number(0);
        out << text;
        file.flush();
        file.close();
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

