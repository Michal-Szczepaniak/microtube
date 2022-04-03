#ifndef VIDEOHELPER_H
#define VIDEOHELPER_H

#include <QObject>
#include <memory>
#include "../entities/video.h"
#include "jsprocesshelper.h"

class VideoHelper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString description READ getDescription NOTIFY gotVideoInfo)
    Q_PROPERTY(QString videoUrl READ getVideoUrl NOTIFY gotVideoInfo)
    Q_PROPERTY(QString audioUrl READ getAudioUrl NOTIFY gotVideoInfo)
    Q_PROPERTY(Video* currentVideo READ getCurrentVideo NOTIFY gotVideoInfo)
public:
    explicit VideoHelper(QObject *parent = nullptr);

    Q_INVOKABLE void loadVideoUrl(QString videoId, int maxDefinition);
    QString getVideoUrl() const;
    QString getAudioUrl() const;
    QString getDescription() const;
    Video* getCurrentVideo() const;

signals:
    void gotVideoInfo();

public slots:
    void gotFormats(QHash<int, QString> formats);
    void gotDescription(QString description);

private:
    JSProcessHelper _jsProcessHelper;
    QString _videoUrl;
    QString _audioUrl;
    QString _description;
    int _maxDefinition;
    std::unique_ptr<Video> _currentVideo{};
};

#endif // VIDEOHELPER_H
