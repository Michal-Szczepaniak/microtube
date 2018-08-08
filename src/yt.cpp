#include "yt.h"
#include <QDebug>

YT::YT(QObject *parent) : QObject(parent)
{

}

void YT::gotStreamUrl(const QUrl &streamUrl) {
    qDebug()<<streamUrl;
    this->setStreamUrl(streamUrl);
    emit streamUrlChanged(streamUrl);
}
