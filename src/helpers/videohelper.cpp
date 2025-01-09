#include "videohelper.h"
#include "../entities/videodefinition.h"
#include <QDebug>
#include <QSettings>

VideoHelper::VideoHelper(QObject *parent) : QObject(parent)
{
    connect(&_jsProcessHelper, &JSProcessManager::gotVideoInfo, this, &VideoHelper::gotFormats);
    connect(&_converter, &XmlToSrtConverter::gotSrt, this, &VideoHelper::gotSubtitles);
}

void VideoHelper::loadVideoUrl(QString videoId, int maxDefinition, bool combined)
{
    _maxDefinition = maxDefinition;
    _videoUrl = "";
    _audioUrl = "";
    _combined = combined;

    _jsProcessHelper.asyncGetVideoInfo(videoId);
}

void VideoHelper::loadSubtitle(int index)
{
    if (!_currentVideo || _currentVideo->subtitles.empty()) return;

    if (index == -1) {
        _currentSubtitle = "";
        emit subtitleChanged();
        return;
    }

    _converter.convertFromUrl(_currentVideo->subtitles.at(index).baseUrl);
}

void VideoHelper::markAsWatched()
{
    if (!_currentVideo) return;

    Video* v = _videoRepository.getOneByVideoId(_currentVideo->videoId);
    if (v == nullptr) {
        _videoRepository.put(_currentVideo.get());
        v = _videoRepository.get(_currentVideo->id);
    }

    v->watched = true;
    _videoRepository.update(v->id);
}

QString VideoHelper::getVideoUrl() const
{
    return _videoUrl;
}

QString VideoHelper::getAudioUrl() const
{
    return _audioUrl;
}

QString VideoHelper::getSubtitle() const
{
    return _currentSubtitle;
}

Video *VideoHelper::getCurrentVideo() const
{
    return _currentVideo.get();
}

QStringList VideoHelper::getSubtitlesLabels() const
{
    QStringList list;
    list.append(tr("Off"));

    if (_currentVideo) {
        for (Caption c : _currentVideo->subtitles)
            list.append(c.label);
    }

    return list;
}

int VideoHelper::getProjection() const
{
    if (!_currentVideo) return Projection::Flat;

    return _currentVideo->projection;
}

bool VideoHelper::getUseAVC() const
{
    return QSettings().value("avc", false).toBool();
}

void VideoHelper::setUseAVC(bool useAVC)
{
    QSettings().setValue("avc", useAVC);
}

void VideoHelper::gotFormats(QHash<int, QString> formats)
{
    _currentVideo = _jsProcessHelper.getVideoInfo();

    qDebug() << "Available formats: " << formats.keys();

    if (_currentVideo->isLive) {
        for (int definition : VideoDefinition::audioDefinitions) {
            if (formats.contains(definition)) {
                _audioUrl = formats[definition];
                qDebug() << "Selecting audio format: " << definition;
                break;
            }
        }

        for (QMap<quint32, QVector<int>>::const_iterator definitions = VideoDefinition::liveVideoDefinitions.constEnd(); definitions-- != VideoDefinition::liveVideoDefinitions.constBegin(); ) {
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
    } else if (_combined) {
        for (QMap<quint32, QVector<int>>::const_iterator definitions = VideoDefinition::videoDefinitionsCombined.constEnd(); definitions-- != VideoDefinition::videoDefinitionsCombined.constBegin(); ) {
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

        if (_maxDefinition == 0) {
            for (int definition : VideoDefinition::audioDefinitions) {
                if (formats.contains(definition)) {
                    _audioUrl = formats[definition];
                    qDebug() << "Selecting audio format: " << definition;
                    break;
                }
            }
        }
    } else if (getUseAVC()) {
        for (int definition : VideoDefinition::audioDefinitions) {
            if (formats.contains(definition)) {
                _audioUrl = formats[definition];
                qDebug() << "Selecting audio format: " << definition;
                break;
            }
        }

        for (QMap<quint32, QVector<int>>::const_iterator definitions = VideoDefinition::videoDefinitionsAvc1.constEnd(); definitions-- != VideoDefinition::videoDefinitionsAvc1.constBegin(); ) {
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
    } else {
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

                    if (definition == 22 || definition == 18) {
                        _audioUrl = "";
                    }

                    break;
                }
            }
        }
    }

    emit gotVideoInfo();
    emit subtitlesChanged();
}

void VideoHelper::gotSubtitles(QString srt)
{
    _currentSubtitle = srt;

    emit subtitleChanged();
}
