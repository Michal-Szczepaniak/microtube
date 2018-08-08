#ifndef YT_H
#define YT_H

#include <QObject>
#include <QUrl>


class YT : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl streamUrl READ getStreamUrl WRITE setStreamUrl NOTIFY streamUrlChanged)

public:
    explicit YT(QObject *parent = nullptr);
    QUrl getStreamUrl() { return m_streamUrl; }
    void setStreamUrl(QUrl streamUrl) { m_streamUrl = streamUrl; }

signals:
    void streamUrlChanged(const QUrl &streamUrl);

public slots:
    void gotStreamUrl(const QUrl &streamUrl);

private:
    QUrl m_streamUrl;
};

#endif // YT_H
