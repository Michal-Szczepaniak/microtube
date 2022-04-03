#ifndef QUICKVIEWHELPER_H
#define QUICKVIEWHELPER_H

#include <QQuickView>

class QuickViewHelper {
public:
    QuickViewHelper() = delete;

    static QQuickView *getView() { return _view; }
    static void setView(QQuickView *view) { QuickViewHelper::_view = view; }
private:
    inline static QQuickView *_view;
};

#endif // QUICKVIEWHELPER_H
