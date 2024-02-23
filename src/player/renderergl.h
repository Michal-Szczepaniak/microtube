#ifndef RENDERERGL_H
#define RENDERERGL_H

#include "renderer.h"

#include <QMutex>
#include <QtOpenGL/qgl.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

class RendererGL : public Renderer
{
    Q_OBJECT

public:
    RendererGL(QObject *parent = nullptr);

    ~RendererGL();

    void paint(const QMatrix4x4& matrix, const QRectF& viewport) override;
    void resize(const QSizeF& size) override;
    void reset() override;
    GstElement *sinkElement() override;

    QRectF renderArea() override;
    QSizeF videoResolution() override;

signals:
    void updateRequested();
    void renderAreaChanged();
    void videoResolutionChanged();

private slots:
    void setVideoSize(const QSizeF& size);

private:
    inline static void reshapeCallback(GstVideoSink *, GLuint width, GLuint height, RendererGL *r);
    inline static void drawCallback(GstVideoSink *sink, GLuint texture, GLuint width, GLuint height, RendererGL *r);
    static void sink_notify(RendererGL *q, GObject *object, gboolean is_last_ref);
    static void sink_caps_changed(GObject *obj, GParamSpec *pspec, RendererGL *q);

    void calculateProjectionMatrix(const QRectF& rect);
    void createProgram();
    void paintFrame(const QMatrix4x4& matrix);
    void calculateVertexCoords();

    void cleanup();
    void updateCropInfo(const GstStructure *s, std::vector<GLfloat>& texCoords);

    GstElement *_sink;

    QMutex _frameMutex;
    QMutex _programMutex;
    gulong _reshapeId;
    gulong _drawId;
    gulong _notify;
    GLuint _texture;

    bool _needsInit;
    QGLShaderProgram *_program;
    QMatrix4x4 _projectionMatrix;
    std::vector<GLfloat> _vertexCoords;
    std::vector<GLfloat> _texCoords;

    QSizeF _size;
    QSizeF _videoSize;
    QRectF _renderArea;

    EGLDisplay _dpy;
    bool _displaySet;

    QMetaObject::Connection _sceneGraphInitializedSignal;
    QMetaObject::Connection _sceneGraphInvalidatedSignal;

};

#endif // RENDERERGL_H
