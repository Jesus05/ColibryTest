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

    connect(&manager, &ProcessingManager::fileFound, this, &MainWindow::onFileFound);
    connect(&manager, &ProcessingManager::fileProgress, this, &MainWindow::onFileProgress);
    connect(&manager, &ProcessingManager::fileFinished, this, &MainWindow::onFileFinished);
    connect(&manager, &ProcessingManager::allFinished, this, &MainWindow::onAllFinished);
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
    s.pollIntervalMs = ui->teTimer->time().msecsSinceStartOfDay();
    s.xorValue = (QString("0x") + ui->leXor->text()).toULongLong(nullptr, 16);

    std::cout << s.pollIntervalMs << std::endl;

    totalFiles = 0;
    processedFiles = 0;
    totalBytes = 0;
    processedBytes = 0;

    ui->pbAll->setValue(0);
    ui->pbAll->setMaximum(0);

    ui->pbMbytes->setValue(0);
    ui->pbMbytes->setMaximum(0);

    manager.start(s);
}

void MainWindow::onFileFound(const QString &, qint64 size) {
    totalFiles++;
    ui->pbAll->setMaximum(totalFiles);

    totalBytes += size;
    int totalMB = static_cast<int>(totalBytes / (1024 * 1024));
    ui->pbMbytes->setMaximum(totalMB);
}

void MainWindow::onFileProgress(const QString &path, qint64 processed, qint64 total, qint64 bytesRead)
{
    processedBytes += bytesRead;
    int processedMB = static_cast<int>(processedBytes / (1024 * 1024));
    ui->pbMbytes->setValue(processedMB);
}

void MainWindow::onFileFinished(const QString &, bool) {
    processedFiles++;
    ui->pbAll->setValue(processedFiles);
}

void MainWindow::onAllFinished() {
    ui->pbAll->setValue(ui->pbAll->maximum());
    totalFiles = 0;
    processedFiles = 0;
    ui->pbMbytes->setMaximum(std::max(1, ui->pbMbytes->maximum()));
    ui->pbMbytes->setValue(ui->pbMbytes->maximum());
}

