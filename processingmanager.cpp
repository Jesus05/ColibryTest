#include "processingmanager.h"

#include "FileProcessor.h"

ProcessingManager::ProcessingManager(QObject *parent)
    : QObject(parent)
    , m_thread(new QThread(this))
{
}

void ProcessingManager::start(const Settings &settings)
{
    if (m_thread->isRunning())
        return;

    m_settings = settings;
    auto *processor = new FileProcessor(m_settings);
    processor->moveToThread(m_thread);

    connect(processor, &FileProcessor::fileFound,
            this, &ProcessingManager::fileFound);
    connect(processor, &FileProcessor::fileProgress,
            this, &ProcessingManager::fileProgress);
    connect(processor, &FileProcessor::fileFinished,
            this, &ProcessingManager::fileFinished);
    connect(processor, &FileProcessor::allFinished,
            this, &ProcessingManager::allFinished);

    connect(m_thread, &QThread::started, processor, &FileProcessor::process);
    connect(processor, &FileProcessor::finished, m_thread, &QThread::quit);
    connect(processor, &FileProcessor::finished, processor, &QObject::deleteLater);

    m_thread->start();
}
