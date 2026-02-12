#pragma once

#include <QObject>
#include <QThread>
#include "Settings.h"

class ProcessingManager : public QObject
{
    Q_OBJECT

public:
    explicit ProcessingManager(QObject *parent = nullptr);
    void start(const Settings &settings);

signals:
    void fileFound(const QString &path);
    void fileProgress(const QString &path, qint64 processed, qint64 total);
    void fileFinished(const QString &path, bool success);
    void allFinished();

private:
    QThread *m_thread;
    Settings m_settings;
};
