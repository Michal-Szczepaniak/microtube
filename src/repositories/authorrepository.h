#ifndef AUTHORREPOSITORY_H
#define AUTHORREPOSITORY_H

#include "baserepository.h"
#include <src/entities/author.h>

class AuthorRepository
{
public:
    Author get(int id);
    void put(Author &entity);
    void update(Author entity);
    void remove(Author entity);

    Author getOneByChannelId(QString channelId);
    QVector<Author> getSubscriptions();
    QVector<Author> getSubscriptionsWithUnwatchedCount();

    static void initTable();
};

#endif // AUTHORREPOSITORY_H
