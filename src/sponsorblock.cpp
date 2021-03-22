#include "sponsorblock.h"
#include <QUrl>
#include <QUrlQuery>
#include "httputils.h"
#include "http.h"
#include <QCryptographicHash>

const QString SponsorBlock::API_URL = "https://sponsor.ajay.app/api";

SponsorBlock::SponsorBlock(QObject *parent) : QObject(parent)
{

}

QString SponsorBlock::getSkipSegments()
{
    if (!QSettings().value("sponsorBlockEnabled", false).toBool()) return "";
    if (_skipSegments != "") {
        return _skipSegments;
    } else {
        qDebug() << _videoId;
        qDebug() << QCryptographicHash::hash(_videoId.toUtf8(), QCryptographicHash::Sha256).toHex();

        QUrl url(API_URL + "/skipSegments/" + QCryptographicHash::hash(_videoId.toUtf8(), QCryptographicHash::Sha256).toHex().left(4));


        QUrlQuery q(url);

        QJsonArray categories = QJsonArray::fromStringList(QSettings().value("sponsorBlockCategories").toStringList());

        q.addQueryItem(QStringLiteral("categories"), QJsonDocument(categories).toJson(QJsonDocument::JsonFormat::Compact));

        url.setQuery(q);
        QObject *reply = HttpUtils::cached().get(url);
        connect(reply, SIGNAL(data(QByteArray)), SLOT(parseSkipSegments(QByteArray)));
        return "";
    }
}

void SponsorBlock::parseSkipSegments(QByteArray bytes)
{
    QJsonDocument doc = QJsonDocument::fromJson(bytes);
    QJsonArray array = doc.array();
    for (const QJsonValue &v : array) {
        if (v.toObject()["videoID"].toString() == _videoId) {
            _skipSegments = QJsonDocument(v.toObject()["segments"].toArray()).toJson(QJsonDocument::JsonFormat::Compact);
            emit skipSegmentsChanged();
        }
    }
}

void SponsorBlock::setVideoId(QString videoId)
{
    bool idChanged = false;
    if (videoId != _videoId) {
        idChanged = true;
    }

    _videoId = videoId;
    emit videoIdChanged();

    if (idChanged) {
        _skipSegments = "";
        emit skipSegmentsChanged();
    }
}
