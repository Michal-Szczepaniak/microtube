#include "ytjschannel.h"

#include "js.h"

YTJSChannel::YTJSChannel(const QString &id, QObject *parent) : QObject(parent) {
    load(id);
}

void YTJSChannel::load(const QString &channelId) {
    QProcess* process = JS::instance().executeNodeScript("channelInfo", {channelId});
    QObject::connect(process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), [this, process](int exitCode, QProcess::ExitStatus exitStatus) {
        Q_UNUSED(exitCode)
        Q_UNUSED(exitStatus)

        qDebug() << "got channelInfo";
        auto doc = QJsonDocument::fromJson(process->readAll());
        auto obj = doc.object();

        displayName = obj["author"].toString();
        description = obj["description"].toString();
        subscriberCount = QString::number(obj["subscriberCount"].toInt());

        const auto thumbs = obj["authorThumbnails"].toArray();
        int maxFoundWidth = 0;
        for (const auto &thumbObj : thumbs) {
            QString url = thumbObj.toObject()["url"].toString();
            int width = thumbObj.toObject()["width"].toInt();
            if (width > maxFoundWidth) {
                maxFoundWidth = width;
                thumbnailUrl = url;
            }
        }

        emit loaded();
        emit process->deleteLater();
    });
}
