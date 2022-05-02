#ifndef CAPTION_H
#define CAPTION_H

#include <QObject>

struct Caption {
    Q_GADGET
    Q_PROPERTY(QString label MEMBER label)
    Q_PROPERTY(QString code MEMBER code)
    Q_PROPERTY(QString id MEMBER id)
    Q_PROPERTY(bool translatable MEMBER translatable)
    Q_PROPERTY(QString baseUrl MEMBER baseUrl)
public:
    QString label;
    QString code;
    QString id;
    bool translatable;
    QString baseUrl;
};

#endif // CAPTION_H
