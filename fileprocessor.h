#pragma once

#include <QObject>
#include <QTimer>
#include <QRegularExpression>
#include "Settings.h"

class FileProcessor : public QObject
{
    Q_OBJECT

public:
    explicit FileProcessor(const Settings &settings, QObject *parent = nullptr);

signals:
    void fileFound(const QString &path, qint64 size);
    void fileProgress(const QString &path, qint64 processed, qint64 total, qint64 bytesRead);
    void fileFinished(const QString &path, bool success);
    void allFinished();
    void finished();

public slots:
    void process();

private slots:
    void onTimer();

private:
    void processFiles();
    void processFile(const QString &inputPath);
    QStringList findFiles() const;
    QString uniqueOutputPath(const QString &fileName) const;

    Settings m_settings;
    QTimer *m_timer = nullptr;
};
