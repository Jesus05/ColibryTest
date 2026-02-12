#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "processingmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pbPath_clicked();
    void on_pbStart_clicked();
    void onFileFound(const QString &path, qint64 size);
    void onFileProgress(const QString &path, qint64 processed, qint64 total, qint64 bytesRead);
    void onFileFinished(const QString &path, bool success);
    void onAllFinished();

private:
    Ui::MainWindow *ui;
    ProcessingManager manager;
    int totalFiles = 0;
    int processedFiles = 0;
    qint64 totalBytes = 0;
    qint64 processedBytes = 0;
};
#endif // MAINWINDOW_H
