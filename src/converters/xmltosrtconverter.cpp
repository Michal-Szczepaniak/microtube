#include "xmltosrtconverter.h"
#include <QNetworkRequest>
#include <QXmlStreamReader>
#include <QTime>
#include <QFile>
#include <QTextDocument>

XmlToSrtConverter::XmlToSrtConverter(QObject *parent) : QObject(parent), _manager(new QNetworkAccessManager(this))
{
    connect(_manager, &QNetworkAccessManager::finished, this, &XmlToSrtConverter::requestFinished);
}

void XmlToSrtConverter::convertFromUrl(QUrl url)
{
    _manager->get(QNetworkRequest(url));
}

void XmlToSrtConverter::requestFinished(QNetworkReply *reply)
{
    if (!reply->error()) parseXml(reply->readAll());
}

void XmlToSrtConverter::parseXml(QString xml)
{
    QXmlStreamReader reader(xml);
    QString srt;
    QTextStream stream(&srt);

    int counter = 1;
    while(!reader.atEnd() && !reader.hasError()) {
        if(reader.readNext() == QXmlStreamReader::StartElement && reader.name() == "text") {
            QTime start, end;
            start = QTime(0, 0).addMSecs(reader.attributes().at(0).value().toFloat()*1000);
            end = QTime(0, 0).addMSecs(reader.attributes().at(0).value().toFloat()*1000 + reader.attributes().at(1).value().toFloat()*1000);
            QTextDocument d;
            d.setHtml(reader.readElementText());
            stream << QString::number(counter) << "\n" << start.toString("hh:mm:ss,zzz") << " --> " << end.toString("hh:mm:ss,zzz") << "\n" << d.toPlainText() << "\n\n";
            counter++;
        }
    }

    emit gotSrt(srt);
}
