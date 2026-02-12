#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QString>
#include <QFileDialog>

#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->leMask->setText("*");
    ui->lePath->setText(".");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pbPath_clicked()
{
    QString folder = QFileDialog::getExistingDirectory(this, "Выберите папку для выходных файлов", ui->lePath->text());
    ui->lePath->setText(folder);
}


void MainWindow::on_pbStart_clicked()
{
    Settings s;
    s.fileMask = ui->leMask->text();
    s.deleteInputFiles = ui->cbDelete->isChecked();
    s.outputPath = ui->lePath->text();
    s.duplicateAction = ui->rbRewrite->isChecked() ? Settings::Overwrite : Settings::AddCounter;
    s.timerMode = ui->cbTimer->isChecked() ? Settings::Periodic : Settings::SingleRun;
    s.pollIntervalMs = ui->teTimer->time().msec();
    s.xorValue = (QString("0x") + ui->leXor->text()).toULongLong(nullptr, 16);

    manager.start(s);
}

