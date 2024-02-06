#ifndef VIDEOHELPER_H
#define VIDEOHELPER_H

#include <QObject>
#include <memory>
#include "src/repositories/videorepository.h"
#include "src/entities/video.h"
#include "jsprocesshelper.h"

Q_DECLARE_METATYPE(QList<Caption>);
class VideoHelper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString videoUrl READ getVideoUrl NOTIFY gotVideoInfo)
    Q_PROPERTY(QString audioUrl READ getAudioUrl NOTIFY gotVideoInfo)
    Q_PROPERTY(int projection READ getProjection NOTIFY gotVideoInfo)
    Q_PROPERTY(QString subtitle READ getSubtitle NOTIFY subtitleChanged)
    Q_PROPERTY(QStringList subtitlesLabels READ getSubtitlesLabels NOTIFY subtitlesChanged)
    Q_PROPERTY(Video* currentVideo READ getCurrentVideo NOTIFY gotVideoInfo)
public:
    explicit VideoHelper(QObject *parent = nullptr);

    Q_INVOKABLE void loadVideoUrl(QString videoId, int maxDefinition, bool combined = false);
    Q_INVOKABLE void loadSubtitle(int index);
    Q_INVOKABLE void markAsWatched();
    QString getVideoUrl() const;
    QString getAudioUrl() const;
    QString getSubtitle() const;
    Video* getCurrentVideo() const;
    QStringList getSubtitlesLabels() const;
    int getProjection() const;

signals:
    void gotVideoInfo();
    void subtitleChanged();
    void subtitlesChanged();

public slots:
    void gotFormats(QHash<int, QString> formats);
    void gotSubtitles(QString srt);

private:
    JSProcessHelper _jsProcessHelper;
    XmlToSrtConverter _converter;
    QString _videoUrl;
    QString _audioUrl;
    quint32 _maxDefinition;
    bool _combined;
    std::unique_ptr<Video> _currentVideo{};
    QString _currentSubtitle;
    VideoRepository _videoRepository;
};

#endif // VIDEOHELPER_H
