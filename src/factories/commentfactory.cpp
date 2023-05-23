#include "commentfactory.h"

#include <QDateTime>
#include <QJsonArray>

Comment CommentFactory::fromJson(QJsonObject json)
{
    Comment comment{};

    QJsonObject commentJson = json["comment"].toObject();

    comment.text = commentJson["content"].toObject()["text"].toString();
    comment.author = AuthorFactory::fromCommentsJson(commentJson["author"].toObject());
    comment.likes = commentJson["vote_count"].toString();
    comment.numReplies = commentJson["reply_count"].toInt();
    comment.time = commentJson["published"].toObject()["text"].toString();
    comment.repliesContinuation = json["comment_replies_data"].toObject()["contents"].toArray().first().toObject();

    return comment;
}

Comment CommentFactory::fromReplyJson(QJsonObject json)
{
    Comment comment{};

    comment.text = json["content"].toObject()["text"].toString();
    comment.author = AuthorFactory::fromCommentsJson(json["author"].toObject());
    comment.likes = json["vote_count"].toString();
    comment.numReplies = json["reply_count"].toInt();
    comment.time = json["published"].toObject()["text"].toString();

    return comment;
}
