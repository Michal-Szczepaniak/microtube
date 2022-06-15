#include "commentsmodel.h"

CommentsModel::CommentsModel(QObject *parent) : QAbstractListModel(parent), _canContinue(false)
{
    connect(&_jsProcessHelper, &JSProcessHelper::gotComments, this, &CommentsModel::gotComments);
    connect(&_jsProcessHelper, &JSProcessHelper::gotCommentReplies, this, &CommentsModel::gotCommentReplies);
}

void CommentsModel::loadCommentsForVideo(QString videoId)
{
    _jsProcessHelper.asyncGetComments(videoId);
}

void CommentsModel::loadRepliesForComment(QString videoId, QString replyToken)
{
    _repliesContinuation = "";
    _jsProcessHelper.asyncGetCommentReplies(videoId, replyToken);
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
    case CommentsRoles::RepliesCountRole:
        return comment.numReplies;
    case CommentsRoles::ReplyTokenRole:
        return comment.replyToken;
    default:
        return QVariant();
    }
}

void CommentsModel::gotComments(bool canContinue, bool isContinuation)
{
    std::vector<Comment> comments = _jsProcessHelper.getComments();
    if (isContinuation) {
        beginInsertRows(QModelIndex(), rowCount(), rowCount() + comments.size()-1);
        std::move(comments.begin(), comments.end(), std::back_inserter(_comments));
        endInsertRows();
    } else {
        beginResetModel();
        _comments = move(comments);
        endResetModel();
    }

    _canContinue = canContinue;
}

void CommentsModel::gotCommentReplies(QString continuation)
{
    std::vector<Comment> comments = _jsProcessHelper.getCommentReplies();
    if (_repliesContinuation != "") {
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
    roles[RepliesCountRole] = "repliesCount";
    roles[ReplyTokenRole] = "replyToken";
    return roles;
}

bool CommentsModel::canFetchMore(const QModelIndex &parent) const
{
    return _repliesContinuation != "" || _canContinue;
}

void CommentsModel::fetchMore(const QModelIndex &parent)
{
    if (_canContinue) {
        _jsProcessHelper.asyncGetCommentsContinuation();
    } else if (_repliesContinuation != "") {
        _jsProcessHelper.asyncGetCommentReplies(_videoId, _repliesContinuation);
    }
}
