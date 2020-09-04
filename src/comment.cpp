#include "comment.h"

Comment::Comment()
{

}

QString Comment::getId() const
{
    return _id;
}

void Comment::setId(const QString &id)
{
    _id = id;
}

QString Comment::getText() const
{
    return _text;
}

void Comment::setText(const QString &text)
{
    _text = text;
}

QString Comment::getAuthor() const
{
    return _author;
}

void Comment::setAuthor(const QString &author)
{
    _author = author;
}

QString Comment::getAuthorProfileImage() const
{
    return _authorProfileImage;
}

void Comment::setAuthorProfileImage(const QString &authorProfileImage)
{
    _authorProfileImage = authorProfileImage;
}
