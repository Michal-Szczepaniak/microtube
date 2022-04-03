#include "videohelper.h"
#include "../entities/videodefinition.h"
#include <QDebug>

VideoHelper::VideoHelper(QObject *parent) : QObject(parent)
{
    connect(&_jsProcessHelper, &JSProcessHelper::gotVideoInfo, this, &VideoHelper::gotFormats);
}

void VideoHelper::loadVideoUrl(QString videoId, int maxDefinition)
{
    _maxDefinition = maxDefinition;
    _videoUrl = "";
    _audioUrl = "";

    _jsProcessHelper.asyncGetVideoInfo(videoId);
}

QString VideoHelper::getVideoUrl() const
{
    return _videoUrl;
}

QString VideoHelper::getAudioUrl() const
{
    return _audioUrl;
}

QString VideoHelper::getDescription() const
{
    return _description;
}

Video *VideoHelper::getCurrentVideo() const
{
    return _currentVideo.get();
}

void VideoHelper::gotFormats(QHash<int, QString> formats)
{
    for (int definition : VideoDefinition::audioDefinitions) {
        if (formats.contains(definition)) {
            _audioUrl = formats[definition];
            qDebug() << "Selecting audio format: " << definition;
            break;
        }
    }

    for (QMap<quint32, QVector<int>>::const_iterator definitions = VideoDefinition::videoDefinitions.constEnd(); definitions-- != VideoDefinition::videoDefinitions.constBegin(); ) {
        if (definitions.key() > _maxDefinition || _videoUrl != "") continue;
        qDebug() << "Definition: " << definitions.key() << " max definition: " << _maxDefinition;
        for (int definition : definitions.value()) {
            if (formats.contains(definition)) {
                _videoUrl = formats[definition];
                qDebug() << "Selecting video format: " << definition;
                break;
            }
        }
    }

    _currentVideo = _jsProcessHelper.getVideoInfo();
    emit gotVideoInfo();
}


void VideoHelper::gotDescription(QString description)
{
    _description = description;

    emit gotVideoInfo();
}
