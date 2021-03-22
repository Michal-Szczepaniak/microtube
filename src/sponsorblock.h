#ifndef SPONSORBLOCK_H
#define SPONSORBLOCK_H

#include <QObject>

class SponsorBlock : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString skipSegments READ getSkipSegments NOTIFY skipSegmentsChanged)
    Q_PROPERTY(QString videoId READ getVideoId WRITE setVideoId NOTIFY videoIdChanged)
public:
    static const QString API_URL;

    explicit SponsorBlock(QObject *parent = nullptr);

    QString getSkipSegments();
    QString getVideoId() { return _videoId; }
    void setVideoId(QString videoId);

public slots:
    void parseSkipSegments(QByteArray bytes);

signals:
    void skipSegmentsChanged();
    void videoIdChanged();

private:
    QString _videoId;
    QString _skipSegments;
};

#endif // SPONSORBLOCK_H
