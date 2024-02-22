#ifndef BASEREPOSITORY_H
#define BASEREPOSITORY_H

#include <QMap>

template <typename T>
class BaseRepository
{
public:
    virtual T get(int id) = 0;
    virtual void put(T entity) = 0;
    virtual void update(int id) = 0;
    virtual void remove(int id) = 0;

protected:
    QMap<int, T> _trackedObjects;
};

#endif // BASEREPOSITORY_H
