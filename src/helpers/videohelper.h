#ifndef VIDEOHELPER_H
#define VIDEOHELPER_H

#include <QObject>
#include <memory>
#include "../entities/video.h"
#include "jsprocesshelper.h"

Q_DECLARE_METATYPE(QList<Caption>);
class VideoHelper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString description READ getDescription NOTIFY gotVideoInfo)
    Q_PROPERTY(QString videoUrl READ getVideoUrl NOTIFY gotVideoInfo)
    Q_PROPERTY(QString audioUrl READ getAudioUrl NOTIFY gotVideoInfo)
    Q_PROPERTY(QString subtitle READ getSubtitle NOTIFY subtitleChanged)
    Q_PROPERTY(QStringList subtitlesLabels READ getSubtitlesLabels NOTIFY subtitlesChanged)
    Q_PROPERTY(Video* currentVideo READ getCurrentVideo NOTIFY gotVideoInfo)
public:
    explicit VideoHelper(QObject *parent = nullptr);

    Q_INVOKABLE void loadVideoUrl(QString videoId, int maxDefinition);
    Q_INVOKABLE void loadSubtitle(int index);
    QString getVideoUrl() const;
    QString getAudioUrl() const;
    QString getDescription() const;
    QString getSubtitle() const;
    Video* getCurrentVideo() const;
    QStringList getSubtitlesLabels() const;

signals:
    void gotVideoInfo();
    void subtitleChanged();
    void subtitlesChanged();

public slots:
    void gotFormats(QHash<int, QString> formats);
    void gotDescription(QString description);
    void gotSubtitles(QString srt);

private:
    JSProcessHelper _jsProcessHelper;
    XmlToSrtConverter _converter;
    QString _videoUrl;
    QString _audioUrl;
    QString _description;
    int _maxDefinition;
    std::unique_ptr<Video> _currentVideo{};
    QString _currentSubtitle;
};

#endif // VIDEOHELPER_H
