#include "ytjssinglevideosource.h"

#include "js.h"
#include "video.h"

YTJSSingleVideoSource::YTJSSingleVideoSource(QObject *parent)
    : VideoSource(parent), video(nullptr) {}

void YTJSSingleVideoSource::loadVideos(int max, int startIndex) {
    aborted = false;

    if (startIndex == 1) {
        if (video) {
            if (name.isEmpty()) {
                name = video->getTitle();
                qDebug() << "Emitting name changed" << name;
                emit nameChanged(name);
            }
            emit gotVideos({video->clone()});
        }
    }

    QProcess* process = JS::instance().executeNodeScript("videoInfo", {videoId});
    QObject::connect(process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), [this, process](int exitCode, QProcess::ExitStatus exitStatus) {
        Q_UNUSED(exitCode)
        Q_UNUSED(exitStatus)

        qDebug() << "got videoInfo";
        auto doc = QJsonDocument::fromJson(process->readAll());
        if (aborted) return;

        auto obj = doc.object();
        // qDebug() << doc.toJson();

        auto parseVideoObject = [](QJsonObject i) {
            Video *video = new Video();

            QString id = i["id"].toString();
            video->setId(id);

            QString title = i["title"].toString();
            video->setTitle(title);

            QString desc = i["description"].toString();
            if (desc.isEmpty()) desc = i["desc"].toString();
            video->setDescription(desc);

            const auto thumbs = i["thumbnails"].toArray();
            for (const auto &thumb : thumbs) {
                QString url = thumb.toObject()["url"].toString();
                int width = thumb.toObject()["width"].toInt();
                if (width >= 336)
                    video->setLargeThumbnailUrl(url);
                else if (width >= 246)
                    video->setMediumThumbnailUrl(url);
                else if (width >= 168)
                    video->setThumbnailUrl(url);
            }

            int views = i["view_count"].toString().toInt();
            video->setViewCount(views);

            int duration = i["length_seconds"].toInt();
            video->setDuration(duration);

            QJsonObject author = i["author"].toObject();
            video->setChannelId(author["id"].toString());
            video->setChannelTitle(author["name"].toString());

            return video;
        };

        QVector<Video *> videos;

        if (!video) {
            auto i = obj["videoDetails"].toObject();
            Video *video = new Video();

            QString id = i["videoId"].toString();
            video->setId(id);

            QString title = i["title"].toString();
            video->setTitle(title);

            QString desc = i["description"].toString();
            if (desc.isEmpty()) desc = i["desc"].toString();
            video->setDescription(desc);

            const auto thumbs = i["thumbnails"].toArray();
            for (const auto &thumb : thumbs) {
                QString url = thumb.toObject()["url"].toString();
                int width = thumb.toObject()["width"].toInt();
                if (width >= 336)
                    video->setLargeThumbnailUrl(url);
                else if (width >= 246)
                    video->setMediumThumbnailUrl(url);
                else if (width >= 168)
                    video->setThumbnailUrl(url);
            }

            int views = i["viewCount"].toString().toInt();
            video->setViewCount(views);

            int duration = i["lengthSeconds"].toString().toInt();
            video->setDuration(duration);

            QJsonObject author = i["author"].toObject();
            video->setChannelId(author["id"].toString());
            video->setChannelTitle(author["name"].toString());

            videos << video;
        }

        const auto items = obj["related_videos"].toArray();
        videos.reserve(items.size());

        for (const auto &i : items) {
            videos << parseVideoObject(i.toObject());
        }

        if (videos.isEmpty()) {
            emit error("No results");
        } else {
            emit gotVideos(videos);
            emit finished(videos.size());
        }

        // fake more videos by loading videos related to the last one
        video = nullptr;
        if (!videos.isEmpty()) videoId = videos.last()->getId();
    });
}

void YTJSSingleVideoSource::setVideo(Video *video) {
    this->video = video;
    videoId = video->getId();
}

void YTJSSingleVideoSource::abort() {
    aborted = true;
}

QString YTJSSingleVideoSource::getName() {
    return name;
}
