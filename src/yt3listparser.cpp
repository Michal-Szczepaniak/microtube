/* $BEGIN_LICENSE

This file is part of Minitube.
Copyright 2009, Flavio Tordini <flavio.tordini@gmail.com>
Copyright 2018, Michał Szczepaniak <m.szczepaniak.000@gmail.com>

Minitube is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Minitube is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Minitube.  If not, see <http://www.gnu.org/licenses/>.

$END_LICENSE */

#include "yt3listparser.h"
#include "datautils.h"
#include "video.h"

YT3ListParser::YT3ListParser(const QByteArray &bytes) {
    QJsonDocument doc = QJsonDocument::fromJson(bytes);
    QJsonObject obj = doc.object();

    nextPageToken = obj[QLatin1String("nextPageToken")].toString();

    const QJsonArray items = obj[QLatin1String("items")].toArray();
    videos.reserve(items.size());
    for (const QJsonValue &v : items) {
        QJsonObject item = v.toObject();
        parseItem(item);
    }
}

void YT3ListParser::parseItem(const QJsonObject &item) {
    Video *video = new Video();

    QJsonValue id = item[QLatin1String("id")];
    if (id.isString())
        video->setId(id.toString());
    else {
        QString videoId = id.toObject()[QLatin1String("videoId")].toString();
        video->setId(videoId);
    }

    QJsonObject snippet = item[QLatin1String("snippet")].toObject();

    bool isLiveBroadcastContent =
            snippet[QLatin1String("liveBroadcastContent")].toString() != QLatin1String("none");
    if (isLiveBroadcastContent) {
        delete video;
        return;
    }

    QString publishedAt = snippet[QLatin1String("publishedAt")].toString();
    QDateTime publishedDateTime = QDateTime::fromString(publishedAt, Qt::ISODate);
    video->setPublished(publishedDateTime);

    video->setChannelId(snippet[QLatin1String("channelId")].toString());

    video->setTitle(snippet[QLatin1String("title")].toString());
    video->setDescription(snippet[QLatin1String("description")].toString());

    QJsonObject thumbnails = snippet[QLatin1String("thumbnails")].toObject();
    QLatin1String url("url");
    video->setThumbnailUrl(thumbnails[QLatin1String("medium")].toObject()[url].toString());
    video->setMediumThumbnailUrl(thumbnails[QLatin1String("high")].toObject()[url].toString());
    video->setLargeThumbnailUrl(thumbnails[QLatin1String("standard")].toObject()[url].toString());

    video->setChannelTitle(snippet[QLatin1String("channelTitle")].toString());

    // These are only for "videos" requests

    QJsonValue contentDetails = item[QLatin1String("contentDetails")];
    if (contentDetails.isObject()) {
        QString isoPeriod = contentDetails.toObject()[QLatin1String("duration")].toString();
        int duration = DataUtils::parseIsoPeriod(isoPeriod);
        video->setDuration(duration);
    }

    QJsonValue statistics = item[QLatin1String("statistics")];
    if (statistics.isObject()) {
        int viewCount = statistics.toObject()[QLatin1String("viewCount")].toString().toInt();
        video->setViewCount(viewCount);
    }

    videos.append(video);
}
