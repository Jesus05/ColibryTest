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
    std::cout << " Run!" << std::endl;
}

