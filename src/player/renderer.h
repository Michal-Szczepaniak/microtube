#ifndef RENDERER_H
#define RENDERER_H

#include <QObject>
#include <QRectF>
#include <gst/gst.h>
#include <gst/video/video.h>
#include <QtOpenGL/QGLShaderProgram>
#include <gst/interfaces/nemoeglimagememory.h>
#include <QOpenGLContext>
#include <QOpenGLExtensions>

class QGLShaderProgram;
class QOpenGLExtension_OES_EGL_image;

static const QString VERTEX_SHADER = ""
                                     "attribute vec4 inputVertex;"
                                     "attribute lowp vec2 textureCoord;"
                                     "uniform mat4 matrix;"
                                     "uniform mat4 matrixWorld;"
                                     "varying lowp vec2 fragTexCoord;"
                                     ""
                                     "void main() {"
                                     "    gl_Position = matrix * matrixWorld * inputVertex;"
                                     "    fragTexCoord = textureCoord;"
                                     "}"
                                     "";

class Renderer : public QObject
{
    Q_OBJECT
public:
    static Renderer *create(QObject *parent = 0, bool droid = true);
    virtual ~Renderer();

    enum Projection {
        Flat,
        s180,
        s360
    };
    Q_ENUM(Projection)

    virtual void paint(const QMatrix4x4& matrix, const QRectF& viewport) = 0;
    virtual void resize(const QSizeF& size) = 0;
    virtual void reset() = 0;
    virtual GstElement *sinkElement() = 0;

    virtual QRectF renderArea() = 0;
    virtual QSizeF videoResolution() = 0;
    float getProjectionX() const;
    void setProjectionX(float projectionX);
    float getProjectionY() const;
    void setProjectionY(float projectionY);

    bool needsNativePainting() { return true; };
    void setProjection(Projection projection);

protected:
    Renderer(QObject *parent = nullptr);

signals:
    void updateRequested();
    void renderAreaChanged();
    void videoResolutionChanged();

protected:
    Projection _projection;
    float _projectionX;
    float _projectionY;
};

#endif // RENDERER_H
