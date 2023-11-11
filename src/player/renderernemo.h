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

class QtCamViewfinderRendererNemo : public QObject {
    Q_OBJECT

public:
    QtCamViewfinderRendererNemo(QObject *parent = 0);

    ~QtCamViewfinderRendererNemo();

    virtual void paint(const QMatrix4x4& matrix, const QRectF& viewport);
    virtual void resize(const QSizeF& size);
    virtual void reset();
    virtual GstElement *sinkElement();

    QRectF renderArea();
    QSizeF videoResolution();

    bool needsNativePainting();

signals:
    void updateRequested();
    void renderAreaChanged();
    void videoResolutionChanged();

private slots:
    void setVideoSize(const QSizeF& size);

private:
    inline static void show_frame(GstVideoSink *, GstBuffer *buffer, QtCamViewfinderRendererNemo *r);
    inline static void buffers_invalidated(GstVideoSink *sink, QtCamViewfinderRendererNemo *r);
    static void sink_notify(QtCamViewfinderRendererNemo *q, GObject *object, gboolean is_last_ref);
    static void sink_caps_changed(GObject *obj, GParamSpec *pspec, QtCamViewfinderRendererNemo *q);

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

    GstElement *m_sink;
    GstBuffer *m_queuedBuffer;
    GstBuffer *m_currentBuffer;
    QMutex m_frameMutex;
    gulong m_showFrameId;
    gulong m_buffersInvalidatedId;
    gulong m_notify;
    bool m_needsInit;
    QGLShaderProgram *m_program;
    QMatrix4x4 m_projectionMatrix;
    std::vector<GLfloat> m_vertexCoords;
    std::vector<GLfloat> m_texCoords;
    std::vector<CachedTexture> m_textures;
    QSizeF m_size;
    QSizeF m_videoSize;
    QRectF m_renderArea;
    EGLDisplay m_dpy;
    bool m_displaySet;
    bool m_buffersInvalidated;
    bool m_bufferChanged;
    QOpenGLExtension_OES_EGL_image *m_img;
    float _move = 0.0f;
};

#endif /* RENDERER_MEEGO_H */
