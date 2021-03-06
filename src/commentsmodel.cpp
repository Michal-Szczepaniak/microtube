#include "commentsmodel.h"
#include <QQmlEngine>
#include "yt3.h"
#include "http.h"
#include "httputils.h"
#include "invidious.h"

CommentsModel::CommentsModel(QObject *parent) :
    QAbstractListModel(parent)
{
}

CommentsModel::~CommentsModel()
{
}

void CommentsModel::loadComments(QString videoId)
{
    QUrl url = Invidious::instance().method("comments/" + videoId);
    if (url.isEmpty()) {
        qWarning("No baseUrl");
        return;
    }

    QUrlQuery q(url);
    q.addQueryItem(QStringLiteral("maxResults"), "20");
    url.setQuery(q);

    _commentThreads.clear();
    _videoId = videoId;

    QObject *reply = HttpUtils::yt().get(url);
    connect(reply, SIGNAL(data(QByteArray)), SLOT(parseCommentThreads(QByteArray)));
}

void CommentsModel::loadMoreComments()
{
    QUrl url = YT3::instance().method("comments/" + _videoId);
    if (url.isEmpty()) {
        qWarning("No baseUrl");
        return;
    }

    QUrlQuery q(url);
    q.addQueryItem(QStringLiteral("continuation"), _continuation);
    url.setQuery(q);

    QObject *reply = HttpUtils::yt().get(url);
    connect(reply, SIGNAL(data(QByteArray)), SLOT(parseCommentThreads(QByteArray)));
}

int CommentsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return _commentThreads.count()+1;
}

QVariant CommentsModel::data(const QModelIndex &index, int role) const
{
    if (rowCount() <= 0 || index.row() < 0 || index.row() >= rowCount()) return QVariant();

    if (index.row() == rowCount()-1) return showMoreButton(role);

    ThreadModel* thread = _commentThreads[index.row()];
    switch (role) {
    case DataRoles::IdRole:
        return thread->getTopLevelComment().getId();
    case DataRoles::TextRole:
        return thread->getTopLevelComment().getText();
    case DataRoles::AuthorRole:
        return thread->getTopLevelComment().getAuthor();
    case DataRoles::AuthorProfileImageRole:
        return thread->getTopLevelComment().getAuthorProfileImage();
    case DataRoles::SubCommentsModelRole:
        return QVariant::fromValue(thread);
    default:
        return QVariant();
    }
}

void CommentsModel::parseCommentThreads(QByteArray bytes)
{
    QJsonDocument doc = QJsonDocument::fromJson(bytes);
    QJsonObject obj = doc.object();
    _continuation = obj["continuation"].toString();

    QJsonValue items = obj["comments"];
    if (items.isArray()) {
        const auto array = items.toArray();

        beginInsertRows(QModelIndex(), rowCount()-1, rowCount() + array.count() - 2);
        for (const QJsonValue &v : array) {
            if (!v.isObject()) continue;

            QJsonObject item = v.toObject();

            QString id = item["commentId"].toString();

            ThreadModel* thread = new ThreadModel;
            thread->getTopLevelCommentEditable().setId(id);

            QString text = item["content"].toString();
            thread->getTopLevelCommentEditable().setText(text);

            QString author = item["author"].toString();
            thread->getTopLevelCommentEditable().setAuthor(author);

            QString authorProfileImage = item["authorThumbnails"].toArray().last().toObject()["url"].toString();
            thread->getTopLevelCommentEditable().setAuthorProfileImage(authorProfileImage);

            thread->loadComments();

            QQmlEngine::setObjectOwnership(thread, QQmlEngine::CppOwnership);

            _commentThreads.append(thread);
        }
        endInsertRows();
    }
}

QHash<int, QByteArray> CommentsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IdRole]  = "id";
    roles[TextRole] = "commentText";
    roles[AuthorRole] = "author";
    roles[AuthorProfileImageRole] = "authorProfileImageUrl";
    roles[SubCommentsModelRole] = "subCommentsModel";
    return roles;
}

QVariant CommentsModel::showMoreButton(int role) const
{
    switch (role) {
    case DataRoles::IdRole:
        return -1;
    case DataRoles::TextRole:
        return tr("Show more");
    case DataRoles::AuthorRole:
        return "";
    case DataRoles::AuthorProfileImageRole:
        return "";
    case DataRoles::SubCommentsModelRole:
    default:
        return QVariant();
    }
}
