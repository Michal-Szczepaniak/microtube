#ifndef AUTHORREPOSITORY_H
#define AUTHORREPOSITORY_H

#include "baserepository.h"
#include <src/entities/author.h>

class AuthorRepository : public BaseRepository<Author>
{
public:
    Author *get(int id);
    void put(Author *entity);
    void update(int id);
    void remove(int id);

    Author *getOneByChannelId(QString channelId);

    static void initTable();
};

#endif // AUTHORREPOSITORY_H
