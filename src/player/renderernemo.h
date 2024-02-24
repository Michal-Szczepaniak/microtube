#ifndef RENDERER_MEEGO_H
#define RENDERER_MEEGO_H

#include <QImage>
#include <QMutex>
#include <QMatrix4x4>
#include <QtOpenGL/qgl.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <gst/video/gstvideometa.h>

class QGLShaderProgram;
class QOpenGLExtension_OES_EGL_image;

class RendererNemo : public QObject {
    Q_OBJECT

public:
    RendererNemo(QObject *parent = 0);

    ~RendererNemo();

    enum Projection {
        Flat,
        s180,
        s360
    };
    Q_ENUM(Projection)

    virtual void paint(const QMatrix4x4& matrix, const QRectF& viewport);
    virtual void resize(const QSizeF& size);
    virtual void reset();
    virtual GstElement *sinkElement();

    QRectF renderArea();
    QSizeF videoResolution();
    float getProjectionX() const;
    void setProjectionX(float projectionX);
    float getProjectionY() const;
    void setProjectionY(float projectionY);

    bool needsNativePainting();
    void setProjection(Projection projection);

signals:
    void updateRequested();
    void renderAreaChanged();
    void videoResolutionChanged();

private slots:
    void setVideoSize(const QSizeF& size);

private:
    inline static void show_frame(GstVideoSink *, GstBuffer *buffer, RendererNemo *r);
    inline static void buffers_invalidated(GstVideoSink *sink, RendererNemo *r);
    static void sink_notify(RendererNemo *q, GObject *object, gboolean is_last_ref);
    static void sink_caps_changed(GObject *obj, GParamSpec *pspec, RendererNemo *q);

    void calculateProjectionMatrix(const QRectF& rect);
    void createProgram();
    void paintFrame(const QMatrix4x4& matrix);
    void calculateVertexCoords();

    void cleanup();
    void destroyCachedTextures();
    void updateCropInfo(const GstStructure *s, std::vector<GLfloat>& texCoords);

    struct CachedTexture
    {
        GstMemory *memory;
        EGLImageKHR image;
        GLuint textureId;
    };

    GstElement *_sink;
    GstBuffer *_queuedBuffer;
    GstBuffer *_currentBuffer;
    QMutex _frameMutex;
    QMutex _programMutex;
    gulong _showFrameId;
    gulong _buffersInvalidatedId;
    gulong _notify;
    bool _needsInit;
    QGLShaderProgram *_program;
    QMatrix4x4 _projectionMatrix;
    std::vector<GLfloat> _vertexCoords;
    std::vector<GLfloat> _texCoords;
    QVector<CachedTexture> _textures;
    QSizeF _size;
    QSizeF _videoSize;
    QRectF _renderArea;
    EGLDisplay _dpy;
    bool _displaySet;
    bool _buffersInvalidated;
    bool _bufferChanged;
    QOpenGLExtension_OES_EGL_image *_img;
    Projection _projection;
    float _projectionX;
    float _projectionY;
    QMetaObject::Connection _sceneGraphInitializedSignal;
};

#endif /* RENDERER_MEEGO_H */
