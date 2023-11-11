#include "jsdiagnostics.h"
#include <QDebug>
#include "repositories/videorepository.h"

JsDiagnostics::JsDiagnostics(QObject *parent) : QObject(parent), _worker(this)
{
    connect(&_worker, &JsDiagnosticsWorker::statusChanged, this, &JsDiagnostics::onStatusChanged);
    connect(&_worker, &JsDiagnosticsWorker::step, this, &JsDiagnostics::onStep);
}

void JsDiagnostics::runDiagnostics()
{
    _step = 0;
    _status = 0;
    emit statusTextChanged();
    emit stepTextChanged();

    _worker.start();
}

void JsDiagnostics::clearVideoDatabase()
{
    VideoRepository repository;
    repository.deleteAll();
}

QString JsDiagnostics::statusText() const
{
    switch (_status) {
    case 1:
        return tr("Success");
    case -1:
        return tr("Failed");
    default:
        return tr("Not running");
    }
}

QString JsDiagnostics::stepText() const
{
    switch (_step) {
    case 1:
        return tr("Checking if js directory exists");
    case 2:
        return tr("Checking if js directory is directory");
    case 3:
        return tr("Checking if js directory is readable");
    case 4:
        return tr("Checking if js directory is writable");
    case 5:
        return tr("Checking if js directory is executable");
    case 6:
        return tr("Checking if node is executable and correct version");
    case 7:
        return tr("No problems found");
    case 0:
    default:
        return tr("Not running");
    }
}

void JsDiagnostics::onStatusChanged(int status)
{
    if (status == _status) return;

    _status = status;

    emit statusTextChanged();
}

void JsDiagnostics::onStep(int step)
{
    if (step == _step) return;

    _step = step;

    emit stepTextChanged();
}
