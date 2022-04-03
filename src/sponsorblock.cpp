#include "sponsorblock.h"
#include <QUrl>
#include <QUrlQuery>
#include <QCryptographicHash>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QSettings>

const QString SponsorBlock::API_URL = "https://sponsor.ajay.app/api";

SponsorBlock::SponsorBlock(QObject *parent) : QObject(parent), _manager(new QNetworkAccessManager(this))
{
    connect(_manager, &QNetworkAccessManager::finished, this, &SponsorBlock::requestFinished);
}

SponsorBlock::~SponsorBlock()
{
    delete _manager;
}

QString SponsorBlock::getSkipSegments()
{
    if (!QSettings().value("sponsorBlockEnabled").toBool()) return "";
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

        _manager->get(QNetworkRequest(url));

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

void SponsorBlock::requestFinished(QNetworkReply *reply)
{
    if (!reply->error()) parseSkipSegments(reply->readAll());
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

QStringList SponsorBlock::getCategories() const
{
    return QSettings().value("sponsorBlockCategories").toStringList();
}

void SponsorBlock::setCategories(QStringList categories)
{
    QSettings settings;

    settings.setValue("sponsorBlockCategories", categories);

    emit categoriesChanged();
}

bool SponsorBlock::getEnabled() const
{
    return QSettings().value("sponsorBlockEnabled", false).toBool();
}

void SponsorBlock::setEnabled(bool enabled)
{
    QSettings().setValue("sponsorBlockEnabled", enabled);

    emit enabledChanged();
}
