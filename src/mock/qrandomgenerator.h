#ifndef QRANDOMGENERATOR_H
#define QRANDOMGENERATOR_H

#include <QObject>
#include <QDateTime>

class QRandomGenerator : public QObject
{
    Q_OBJECT
public:
    typedef quint32 result_type;
    static QRandomGenerator* global() {
        static QRandomGenerator instance;
        qsrand(QDateTime::currentMSecsSinceEpoch() / 1000);
        return &instance;
    }

    quint32 generate();
    quint32 min() { return 0; }
    quint32 max() { return std::numeric_limits<result_type>::max(); }

    quint32 operator()() { return generate(); };

private:
    QRandomGenerator()= default;
    ~QRandomGenerator()= default;
    QRandomGenerator(const QRandomGenerator&)= delete;
    QRandomGenerator& operator=(const QRandomGenerator&)= delete;
signals:

};

#endif // QRANDOMGENERATOR_H
