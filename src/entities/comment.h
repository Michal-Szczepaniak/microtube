#ifndef COMMENT_H
#define COMMENT_H

#include <QJsonObject>
#include <QObject>
#include <QVector>
#include "thumbnail.h"
#include "author.h"

struct Comment {
    Q_GADGET
    Q_PROPERTY(QString commentId MEMBER commentId)
    Q_PROPERTY(bool edited MEMBER edited)
    Q_PROPERTY(QString text MEMBER text)
    Q_PROPERTY(QString likes MEMBER likes)
    Q_PROPERTY(QString time MEMBER time)
    Q_PROPERTY(int numReplies MEMBER numReplies)
    Q_PROPERTY(bool isOwner MEMBER isOwner)
    Q_PROPERTY(bool isHearted MEMBER isHearted)
    Q_PROPERTY(bool isPinned MEMBER isPinned)
    Q_PROPERTY(bool isVerified MEMBER isVerified)
    Q_PROPERTY(bool isOfficialArtist MEMBER isOfficialArtist)
    Q_PROPERTY(bool hasOwnerReplied MEMBER hasOwnerReplied)
    Q_PROPERTY(bool isMember MEMBER isMember)
    Q_PROPERTY(QString memberIconUrl MEMBER memberIconUrl)
    Q_PROPERTY(QJsonObject repliesContinuation MEMBER repliesContinuation)

public:
    QString commentId;
    Author author;
    bool edited = false;
    QString text;
    QString likes;
    QString time;
    int numReplies;
    bool isOwner = false;
    bool isHearted = false;
    bool isPinned = false;
    bool isVerified = false;
    bool isOfficialArtist = false;
    bool hasOwnerReplied = false;
    bool isMember = false;
    QString memberIconUrl;
    QJsonObject repliesContinuation;
};

#endif // COMMENT_H
