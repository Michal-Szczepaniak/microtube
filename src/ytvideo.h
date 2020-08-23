#ifndef YTVIDEO_H
#define YTVIDEO_H

#include <QtCore>

class VideoDefinition;

class YTVideo : public QObject {
    Q_OBJECT

public:
    YTVideo(const QString &videoId, QObject *parent);
    void loadStreamUrl();
    int getDefinitionCode() const { return definitionCode; }

signals:
    void gotDescription(const QString &description);
    void gotStreamUrl(const QString &videoUrl, const QString &audioUrl);
    void errorStreamUrl(const QString &message);

private slots:
    void gotVideoInfo(const QByteArray &bytes);
    void emitError(const QString &message);
    void scrapeWebPage(const QByteArray &bytes);
    void parseJsPlayer(const QByteArray &bytes);

private:
    void getVideoInfo();
    void parseFmtUrlMap(const QString &fmtUrlMap);
    void loadWebPage();
    void captureFunction(const QString &name, const QString &js);
    void captureObject(const QString &name, const QString &js);
    QString decryptSignature(const QString &s);
    void saveDefinitionForUrl(const QString &url, const VideoDefinition &definition);

    QString videoId;
    QUrl m_streamUrl;
    int definitionCode;
    bool loadingStreamUrl;
    int elIndex;
    bool ageGate;
    QString videoToken;
    QString fmtUrlMap;
    QString sigFuncName;
    QHash<QString, QString> sigFunctions;
    QHash<QString, QString> sigObjects;
    QString dashManifestUrl;
    QString jsPlayer;
    QMap<int, QString> urlMap;
    bool webPageLoaded = false;
};

#endif // YTVIDEO_H
