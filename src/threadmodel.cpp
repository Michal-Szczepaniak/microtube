#include "threadmodel.h"
#include "yt3.h"
#include "http.h"
#include "httputils.h"

ThreadModel::ThreadModel() :
    QAbstractListModel()
{

}

int ThreadModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return _comments.count();
}

QVariant ThreadModel::data(const QModelIndex &index, int role) const
{
    if (rowCount() <= 0 || index.row() < 0 || index.row() >= rowCount()) return QVariant();

    auto &comment = _comments[index.row()];
    switch (role) {
    case DataRoles::IdRole:
        return comment.getId();
    case DataRoles::TextRole:
        return comment.getText();
    case DataRoles::AuthorRole:
        return comment.getAuthor();
    case DataRoles::AuthorProfileImageRole:
        return comment.getAuthorProfileImage();
    default:
        return QVariant();
    }
}

const Comment &ThreadModel::getTopLevelComment() const
{
    return _comment;
}

Comment &ThreadModel::getTopLevelCommentEditable()
{
    return _comment;
}

void ThreadModel::loadComments()
{
    QUrl url = YT3::instance().method("comments");
    QUrlQuery q(url);
    q.addQueryItem(QStringLiteral("parentId"), getTopLevelComment().getId());
    q.addQueryItem(QStringLiteral("part"), "snippet");
    url.setQuery(q);

    QObject *reply = HttpUtils::yt().get(url);
    connect(reply, SIGNAL(data(QByteArray)), SLOT(parseComments(QByteArray)));
}

void ThreadModel::parseComments(QByteArray bytes)
{
    QJsonDocument doc = QJsonDocument::fromJson(bytes);
    QJsonObject obj = doc.object();

    _comments.clear();

    QJsonValue items = obj["items"];
    if (items.isArray()) {
        const auto array = items.toArray();
        for (const QJsonValue &v : array) {
            if (!v.isObject()) continue;

            QJsonObject item = v.toObject();

            QString id = item["id"].toString();
            Comment comment;
            comment.setId(id);

            QString text = item["snippet"].toObject()["textDisplay"].toString();
            comment.setText(text);

            QString author = item["snippet"].toObject()["authorDisplayName"].toString();
            comment.setAuthor(author);

            QString authorProfileImage = item["snippet"].toObject()["authorProfileImageUrl"].toString();
            comment.setAuthorProfileImage(authorProfileImage);

            _comments.append(comment);
        }
    }

    emit dataChanged(createIndex(0, 0), createIndex(rowCount(), 0));
}

QHash<int, QByteArray> ThreadModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IdRole]  = "id";
    roles[TextRole] = "commentText";
    roles[AuthorRole] = "author";
    roles[AuthorProfileImageRole] = "authorProfileImageUrl";
    return roles;
}
