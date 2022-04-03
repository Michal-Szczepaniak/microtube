#ifndef RENDERER_H
#define RENDERER_H

#include <QObject>
#include <gst/gst.h>
#include <QRectF>

class QMetaObject;
class QMatrix4x4;
class QSizeF;

class QtCamViewfinderRenderer : public QObject {
  Q_OBJECT

public:
    static QtCamViewfinderRenderer *create(QObject *parent = 0);
    virtual ~QtCamViewfinderRenderer();

    virtual void paint(const QMatrix4x4& matrix, const QRectF& viewport) = 0;
    virtual void resize(const QSizeF& size) = 0;
    virtual void reset() = 0;
    virtual GstElement *sinkElement() = 0;

    virtual QRectF renderArea() = 0;
    virtual QSizeF videoResolution() = 0;

    virtual bool needsNativePainting() = 0;

protected:
    QtCamViewfinderRenderer(QObject *parent = 0);

signals:
    void updateRequested();
    void renderAreaChanged();
    void videoResolutionChanged();
};

#endif /* RENDERER_H */
