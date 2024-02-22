#include "commentsmodel.h"

CommentsModel::CommentsModel(QObject *parent) : QAbstractListModel(parent), _canContinue(false)
{
    connect(&_jsProcessHelper, &JSProcessManager::gotComments, this, &CommentsModel::gotComments);
    connect(&_jsProcessHelper, &JSProcessManager::gotCommentReplies, this, &CommentsModel::gotCommentReplies);
}

void CommentsModel::loadCommentsForVideo(QString videoId)
{
    _jsProcessHelper.asyncGetComments(videoId);
}

void CommentsModel::loadRepliesForComment(QJsonObject continuation)
{
    _repliesContinuation = {};
    _jsProcessHelper.asyncGetCommentReplies(continuation);
}

int CommentsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return _comments.size();
}

QVariant CommentsModel::data(const QModelIndex &index, int role) const
{
    if (rowCount() <= 0 || index.row() < 0 || index.row() >= rowCount()) return QVariant();

    Comment comment = _comments[index.row()];
    switch (role) {
    case CommentsRoles::IdRole:
        return index.row();
    case CommentsRoles::NameRole:
        return comment.author.name;
    case CommentsRoles::PhotoRole:
        return comment.author.bestAvatar.url;
    case CommentsRoles::TextRole:
        return comment.text;
    case CommentsRoles::PublishedRole:
        return comment.time;
    case CommentsRoles::RepliesCountRole:
        return comment.numReplies;
    case CommentsRoles::RepliesContinuationRole:
        return comment.repliesContinuation;
    default:
        return QVariant();
    }
}

void CommentsModel::gotComments(bool canContinue, bool isContinuation)
{
    std::vector<Comment> comments = _jsProcessHelper.getComments();
    _canContinue = canContinue;

    if (isContinuation) {
        beginInsertRows(QModelIndex(), rowCount(), rowCount() + comments.size()-1);
        std::move(comments.begin(), comments.end(), std::back_inserter(_comments));
        endInsertRows();
    } else {
        beginResetModel();
        _comments = move(comments);
        endResetModel();
    }
}

void CommentsModel::gotCommentReplies(QJsonObject continuation)
{
    std::vector<Comment> comments = _jsProcessHelper.getCommentReplies();
    if (!_repliesContinuation.empty()) {
        beginInsertRows(QModelIndex(), rowCount(), rowCount() + comments.size()-1);
        std::move(comments.begin(), comments.end(), std::back_inserter(_comments));
        endInsertRows();
    } else {
        beginResetModel();
        _comments = move(comments);
        endResetModel();
    }

    _repliesContinuation = continuation;
}

QHash<int, QByteArray> CommentsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IdRole] = "id";
    roles[NameRole] = "author";
    roles[PhotoRole] = "photo";
    roles[TextRole] = "commentText";
    roles[PublishedRole] = "publishedAt";
    roles[RepliesCountRole] = "repliesCount";
    roles[RepliesContinuationRole] = "repliesContinuation";
    return roles;
}

bool CommentsModel::canFetchMore(const QModelIndex &parent) const
{
    return !_repliesContinuation.empty() || _canContinue;
}

void CommentsModel::fetchMore(const QModelIndex &parent)
{
    if (_canContinue) {
        _jsProcessHelper.asyncGetCommentsContinuation();
    } else if (!_repliesContinuation.empty()) {
        _jsProcessHelper.asyncGetCommentReplies(_repliesContinuation);
    }
}
