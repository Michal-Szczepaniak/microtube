#include "commentfactory.h"

Comment CommentFactory::fromJson(QJsonObject json)
{
    Comment comment{};

    comment.text = json["text"].toString();
    comment.author = AuthorFactory::fromCommentsJson(json);
    comment.likes = json["likes"].toString();
    comment.numReplies = json["numReplies"].toInt();
    comment.time = json["time"].toString();

    if (!json["replyToken"].isNull()) {
        comment.replyToken = json["replyToken"].toString();
    }

    return comment;
}
