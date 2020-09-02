#ifndef COMMENT_H
#define COMMENT_H

#include <QString>

class Comment
{
public:
    Comment();

    QString getId() const;
    void setId(const QString &id);
    QString getText() const;
    void setText(const QString &text);
    QString getAuthor() const;
    void setAuthor(const QString &author);
    QString getAuthorProfileImage() const;
    void setAuthorProfileImage(const QString &authorProfileImage);

private:
    QString _id;
    QString _text;
    QString _author;
    QString _authorProfileImage;
};

#endif // COMMENT_H
