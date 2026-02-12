#include "fileprocessor.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <QDebug>

FileProcessor::FileProcessor(const Settings &settings, QObject *parent)
    : QObject(parent), m_settings(settings)
{
}

void FileProcessor::process()
{
    if (m_settings.timerMode == Settings::SingleRun) {
        processFiles();
        emit allFinished();
        emit finished();
    } else {
        if (!m_timer) {
            m_timer = new QTimer(this);
            connect(m_timer, &QTimer::timeout, this, &FileProcessor::onTimer);
            m_timer->start(m_settings.pollIntervalMs);
        }
        onTimer(); // сразу проверить
    }
}

void FileProcessor::onTimer()
{
    processFiles();
}

void FileProcessor::processFiles()
{
    QStringList files = findFiles();

    for (const QString &file : files) {
        QFileInfo info(file);
        emit fileFound(file, info.size());
    }

    for (const QString &file : files)
        processFile(file);
}

void FileProcessor::processFile(const QString &inputPath)
{
    QFile inputFile(inputPath);
    if (!inputFile.open(QIODevice::ReadOnly)) {
        emit fileFinished(inputPath, false);
        return;
    }

    QString outPath = uniqueOutputPath(QFileInfo(inputPath).fileName());
    QFile outputFile(outPath);
    if (!outputFile.open(QIODevice::WriteOnly)) {
        emit fileFinished(inputPath, false);
        return;
    }

    const qint64 fileSize = inputFile.size();
    qint64 totalRead = 0;
    const qint64 bufferSize = 1024 * 1024; // 1 MB
    QByteArray buffer;
    buffer.resize(bufferSize);

    // XOR ключ
    QByteArray xorKey(8, 0);
    for (int i = 0; i < 8; ++i)
        xorKey[i] = static_cast<char>((m_settings.xorValue >> (8 * i)) & 0xFF);

    while (!inputFile.atEnd()) {
        qint64 bytesRead = inputFile.read(buffer.data(), bufferSize);
        if (bytesRead <= 0) break;

        // XOR
        for (qint64 i = 0; i < bytesRead; ++i)
            buffer[i] = buffer[i] ^ xorKey[i % 8];

        outputFile.write(buffer.constData(), bytesRead);
        totalRead += bytesRead;

        emit fileProgress(inputPath, totalRead, fileSize, bytesRead);
    }

    inputFile.close();
    outputFile.close();

    if (m_settings.deleteInputFiles)
        QFile::remove(inputPath);

    emit fileFinished(inputPath, true);
}

QStringList FileProcessor::findFiles() const
{
    QDir dir("."); // входные файлы в текущей папке
    QStringList nameFilters;
    nameFilters << m_settings.fileMask;
    QStringList entries = dir.entryList(nameFilters, QDir::Files);

    QRegularExpression maskRegex(QRegularExpression::wildcardToRegularExpression(m_settings.fileMask));
    QStringList result;
    for (const QString &entry : entries) {
        if (maskRegex.match(entry).hasMatch()) {
            result << dir.absoluteFilePath(entry);
        }
    }
    return result;
}

QString FileProcessor::uniqueOutputPath(const QString &fileName) const
{
    QDir outDir(m_settings.outputPath);
    if (!outDir.exists())
        outDir.mkpath(".");

    QString baseName = fileName;
    QString stem = QFileInfo(baseName).completeBaseName();
    QString suffix = QFileInfo(baseName).suffix();
    if (!suffix.isEmpty())
        suffix = "." + suffix;

    QString candidate = outDir.absoluteFilePath(baseName);
    if (m_settings.duplicateAction == Settings::Overwrite || !QFile::exists(candidate))
        return candidate;

    int counter = 1;
    do {
        candidate = outDir.absoluteFilePath(stem + "_" + QString::number(counter) + suffix);
        ++counter;
    } while (QFile::exists(candidate));
    return candidate;
}
