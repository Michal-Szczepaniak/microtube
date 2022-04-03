#ifndef SPONSORBLOCK_H
#define SPONSORBLOCK_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class SponsorBlock : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ getEnabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(QString skipSegments READ getSkipSegments NOTIFY skipSegmentsChanged)
    Q_PROPERTY(QString videoId READ getVideoId WRITE setVideoId NOTIFY videoIdChanged)
    Q_PROPERTY(QStringList categories READ getCategories WRITE setCategories NOTIFY categoriesChanged)
public:
    static const QString API_URL;

    explicit SponsorBlock(QObject *parent = nullptr);
    ~SponsorBlock();

    QString getSkipSegments();
    QString getVideoId() { return _videoId; }
    void setVideoId(QString videoId);
    QStringList getCategories() const;
    void setCategories(QStringList categories);
    bool getEnabled() const;
    void setEnabled(bool enabled);

public slots:
    void parseSkipSegments(QByteArray bytes);

protected slots:
    void requestFinished(QNetworkReply *reply);

signals:
    void skipSegmentsChanged();
    void videoIdChanged();
    void categoriesChanged();
    void enabledChanged();

private:
    QString _videoId;
    QString _skipSegments;
    QNetworkAccessManager *_manager;
};

#endif // SPONSORBLOCK_H
