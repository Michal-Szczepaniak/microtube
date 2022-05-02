#ifndef XMLTOSRTCONVERTER_H
#define XMLTOSRTCONVERTER_H

#include <QObject>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class XmlToSrtConverter : public QObject
{
    Q_OBJECT
public:
    explicit XmlToSrtConverter(QObject *parent = nullptr);

    void convertFromUrl(QUrl url);

signals:
    void gotSrt(QString text);

protected slots:
    void requestFinished(QNetworkReply *reply);

protected:
    void parseXml(QString xml);

private:
    QNetworkAccessManager *_manager;
};

#endif // XMLTOSRTCONVERTER_H
