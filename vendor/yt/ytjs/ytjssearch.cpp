#include "ytjssearch.h"

#include "mainwindow.h"
#include "searchparams.h"
#include "video.h"
#include "ytsearch.h"

#include "js.h"

namespace {

int parseDuration(const QString &s) {
    const auto parts = s.splitRef(':');
    int secs = 0;
    int p = 0;
    for (auto i = parts.crbegin(); i != parts.crend(); ++i) {
        if (p == 0) {
            secs = i->toInt();
        } else if (p == 1) {
            secs += i->toInt() * 60;
        } else if (p == 2) {
            secs += i->toInt() * 60 * 60;
        }
        p++;
    }
    return secs;
}

QString parseChannelId(const QString &channelUrl) {
    int pos = channelUrl.lastIndexOf('/');
    if (pos >= 0) return channelUrl.mid(pos + 1);
    return QString();
}

QDateTime parsePublishedText(const QString &s) {
    int num = 0;
    const auto parts = s.splitRef(' ');
    for (const auto &part : parts) {
        num = part.toInt();
        if (num > 0) break;
    }
    if (num == 0) return QDateTime();

    auto now = QDateTime::currentDateTimeUtc();
    if (s.contains("hour")) {
        return now.addSecs(-num * 3600);
    } else if (s.contains("day")) {
        return now.addDays(-num);
    } else if (s.contains("week")) {
        return now.addDays(-num * 7);
    } else if (s.contains("month")) {
        return now.addMonths(-num);
    } else if (s.contains("year")) {
        return now.addDays(-num * 365);
    }
    return QDateTime();
}

} // namespace

YTJSSearch::YTJSSearch(SearchParams *searchParams, QObject *parent)
    : VideoSource(parent), searchParams(searchParams) {}

void YTJSSearch::loadVideos(int max, int startIndex) {
    aborted = false;

    QString q;
    if (!searchParams->keywords().isEmpty()) {
        if (searchParams->keywords().startsWith("http://") ||
            searchParams->keywords().startsWith("https://")) {
            q = YTSearch::videoIdFromUrl(searchParams->keywords());
        } else
            q = searchParams->keywords();
    }

    // Options
    QJsonObject optionsJSON;

    if (startIndex > 1) {
        if (!nextpageRef.isEmpty()) {
            optionsJSON.insert("nextpageRef", QJsonValue(nextpageRef));
        } else {
            // non-first page was requested but we have no continuation token
            emit error("No pagination token");
            return;
        }
    }

    optionsJSON.insert("limit", QJsonValue(max));

    switch (searchParams->safeSearch()) {
    case SearchParams::None:
        optionsJSON.insert("safeSearch", QJsonValue(false));
        break;
    case SearchParams::Strict:
        optionsJSON.insert("safeSearch", QJsonValue(true));
        break;
    }

    // Filters
    QJsonObject jsonFilterMap;

    jsonFilterMap.insert("Type", QJsonValue("Video"));

    switch (searchParams->sortBy()) {
    case SearchParams::SortByNewest:
        jsonFilterMap.insert("Sort by", QJsonValue("Upload date"));
        break;
    case SearchParams::SortByViewCount:
        jsonFilterMap.insert("Sort by", QJsonValue("View count"));
        break;
    case SearchParams::SortByRating:
        jsonFilterMap.insert("Sort by", QJsonValue("Rating"));
        break;
    }

    switch (searchParams->duration()) {
    case SearchParams::DurationShort:
        jsonFilterMap.insert("Duration", QJsonValue("Short"));
        break;
    case SearchParams::DurationMedium:
    case SearchParams::DurationLong:
        jsonFilterMap.insert("Duration", QJsonValue("Long"));
        break;
    }

    switch (searchParams->time()) {
    case SearchParams::TimeToday:
        jsonFilterMap.insert("Upload date", QJsonValue("Today"));
        break;
    case SearchParams::TimeWeek:
        jsonFilterMap.insert("Upload date", QJsonValue("This week"));
        break;
    case SearchParams::TimeMonth:
        jsonFilterMap.insert("Upload date", QJsonValue("This month"));
        break;
    }

    switch (searchParams->quality()) {
    case SearchParams::QualityHD:
        jsonFilterMap.insert("Features", QJsonValue("HD"));
        break;
    case SearchParams::Quality4K:
        jsonFilterMap.insert("Features", QJsonValue("4K"));
        break;
    case SearchParams::QualityHDR:
        jsonFilterMap.insert("Features", QJsonValue("HDR"));
        break;
    }

    QProcess* process = JS::instance().executeNodeScript("search", {q, QJsonDocument(optionsJSON).toJson(QJsonDocument::JsonFormat::Compact), QJsonDocument(jsonFilterMap).toJson(QJsonDocument::JsonFormat::Compact)});
    QObject::connect(process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), [this, process](int exitCode, QProcess::ExitStatus exitStatus) {
        Q_UNUSED(exitCode)
        Q_UNUSED(exitStatus)

        qDebug() << "got search";
        auto doc = QJsonDocument::fromJson(process->readAll());

        auto obj = doc.object();

        nextpageRef = QJsonDocument(obj["nextpageRef"].toArray()).toJson(QJsonDocument::JsonFormat::Compact);

        const auto items = obj["items"].toArray();
        QVector<Video *> videos;
        videos.reserve(items.size());

        for (const auto &j : items) {
            const auto i = j.toObject();
            QString type = i["type"].toString();
            if (type != "video") continue;

            Video *video = new Video();

            QString id = YTSearch::videoIdFromUrl(i["link"].toString());
            video->setId(id);

            QString title = i["title"].toString();
            video->setTitle(title);

            QString desc = i["description"].toString();
            video->setDescription(desc);

            QString thumb = i["thumbnail"].toString();
            video->setThumbnailUrl(thumb);

            int views = i["views"].toInt();
            video->setViewCount(views);

            int duration = parseDuration(i["duration"].toString());
            video->setDuration(duration);

            auto published = parsePublishedText(i["uploaded_at"].toString());
            if (published.isValid()) video->setPublished(published);

            auto authorObj = i["author"];
            QString channelName = authorObj.toObject()["name"].toString();
            video->setChannelTitle(channelName);
            QString channelId = parseChannelId(authorObj.toObject()["ref"].toString());
            video->setChannelId(channelId);

            videos << video;
        }

        if (videos.isEmpty()) {
            emit error("No results");
        } else {
            emit gotVideos(videos);
            emit finished(videos.size());
        }
        emit process->deleteLater();
    });
}

QString YTJSSearch::getName() {
    if (!name.isEmpty()) return name;
    if (!searchParams->keywords().isEmpty()) return searchParams->keywords();
    return QString();
}

const QList<QAction *> &YTJSSearch::getActions() {
    static const QList<QAction *> channelActions = {
            MainWindow::instance()->getAction("subscribeChannel")};
    if (searchParams->channelId().isEmpty()) {
        static const QList<QAction *> noActions;
        return noActions;
    }
    return channelActions;
}
