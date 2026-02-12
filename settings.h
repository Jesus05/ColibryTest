#pragma once
#include <QString>
#include <QMetaType>

class Settings {
public:
    enum DuplicateAction { Overwrite, AddCounter };
    enum TimerMode { SingleRun, Periodic };

    QString fileMask;               // маска входных файлов
    bool deleteInputFiles = false; // удалять исходные файлы
    QString outputPath;            // путь для сохранения
    DuplicateAction duplicateAction = AddCounter; // политика дубликатов
    TimerMode timerMode = SingleRun; // разовый / по таймеру
    int pollIntervalMs = 1000;     // интервал опроса (мс)
    quint64 xorValue = 0;          // 8-байтное значение для XOR
};

Q_DECLARE_METATYPE(Settings)
