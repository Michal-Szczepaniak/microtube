/*
 * gst-droid
 *
 * Copyright (C) 2014 Mohammed Sameer <msameer@foolab.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.    See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "renderernemo.h"
#include <QDebug>
#include <gst/video/video.h>
#include <QtOpenGL/QGLShaderProgram>
#include <gst/interfaces/nemoeglimagememory.h>
#include <QOpenGLContext>
#include <QOpenGLExtensions>
#include <QtMath>
#include "../helpers/quickviewhelper.h"

typedef void *EGLSyncKHR;
#define EGL_SYNC_FENCE_KHR                                             0x30F9

typedef EGLSyncKHR(EGLAPIENTRYP PFNEGLCREATESYNCKHRPROC)(EGLDisplay dpy, EGLenum type,
                                                            const EGLint *attrib_list);

PFNEGLCREATESYNCKHRPROC eglCreateSyncKHR = 0;

static const QString FRAGMENT_SHADER = ""
        "#extension GL_OES_EGL_image_external: enable\n"
        "uniform samplerExternalOES texture0;"
        "varying lowp vec2 fragTexCoord;"
        "void main() {"
        "    gl_FragColor = texture2D(texture0, fragTexCoord);"
        "}"
    "";

static const QString FRAGMENT_SHADER_180 = ""
    "#extension GL_OES_EGL_image_external: enable\n"
    "\n"
    "varying lowp vec2 fragTexCoord;\n"
    "\n"
    "//Uniforms\n"
    "uniform vec2 cameraRotation;\n"
    "uniform float invAspectRatio; //height / width\n"
    "uniform float zoom;\n"
    "uniform bool onlyHalfOfTheScreen;\n"
    "\n"
    "//Texture\n"
    "uniform samplerExternalOES texture0;\n"
    "\n"
    "const vec2 inverseAtan = vec2(0.1591, 0.3183);\n"
    "\n"
    "vec2 sampleSphericalMap(vec3 localPosition) {\n"
    "    vec2 uv = vec2(atan(localPosition.z, localPosition.x), asin(localPosition.y));\n"
    "    uv *= inverseAtan;\n"
    "    uv += 0.5;\n"
    "\n"
    "    return uv;\n"
    "}\n"
    "\n"
    "const vec3 up = vec3(0.0, 1.0, 0.0);\n"
    "const float fov = 50.0; //Field of view Y\n"
    "\n"
    "vec3 getDirection(in vec2 cameraRotation, in vec2 positionInView) {\n"
    "    vec3 viewDirection  = normalize(vec3(sin(radians(cameraRotation.y)), sin(radians(-cameraRotation.x)), cos(radians(cameraRotation.y))));\n"
    "    vec3 viewCrossUp    = normalize(cross(viewDirection, up));\n"
    "    vec3 viewCrossRight = normalize(cross(viewCrossUp,   viewDirection));\n"
    "    mat3 viewMatrix = mat3(viewCrossUp, viewCrossRight, viewDirection);\n"
    "\n"
    "    return viewMatrix * normalize(vec3(positionInView * tan(radians(fov)), 1.0));\n"
    "}\n"
    "\n"
    "void main() {\n"
    "    vec2 position = -(fragTexCoord * 2.0 - 1.0);\n"
    "    vec2 positionInView = vec2(position.x, position.y * invAspectRatio) * zoom;\n"
    "    vec3 localPosition = getDirection(cameraRotation, positionInView);\n"
    "    vec2 uv = sampleSphericalMap(localPosition);\n"
    "    uv.y = 1.0 - uv.y;"
    "    if (onlyHalfOfTheScreen)\n"
    "        uv.x = 2.0 - uv.x * 2.0;\n"
    "    gl_FragColor = vec4(texture2D(texture0, uv).rgb, 1.0);\n"
    "}\n"
    "";

static const QString FRAGMENT_SHADER_360 = ""
    "#extension GL_OES_EGL_image_external: enable\n"
    "\n"
    "varying lowp vec2 fragTexCoord;\n"
    "\n"
    "//Uniforms\n"
    "uniform vec2 cameraRotation;\n"
    "uniform float invAspectRatio;\n"
    "uniform float zoom;\n"
    "\n"
    "//Texture\n"
    "uniform samplerExternalOES texture0;\n"
    "\n"
    "const vec3 up = vec3(0.0, 1.0, 0.0);\n"
    "const float fov = 50.0;\n"
    "\n"
    "vec3 getDirection(in vec2 cameraRotation, in vec2 positionInView) {\n"
    "    vec3 viewDirection  = normalize(vec3(sin(radians(cameraRotation.y)), sin(radians(-cameraRotation.x)), cos(radians(cameraRotation.y))));\n"
    "    vec3 viewCrossUp    = normalize(cross(viewDirection, up));\n"
    "    vec3 viewCrossRight = normalize(cross(viewCrossUp,   viewDirection));\n"
    "    mat3 viewMatrix = mat3(viewCrossUp, viewCrossRight, viewDirection);\n"
    "\n"
    "    return viewMatrix * normalize(vec3(positionInView * tan(radians(fov)), 1.0));\n"
    "}\n"
    "\n"
    "int max3(vec3 v) {\n"
    "      int index = 0;\n"
    "    if (v[1] > v[0])\n"
    "        index = 1;\n"
    "    if (v[2] > v[index])\n"
    "        index = 2;\n"
    "\n"
    "    return index;\n"
    "}\n"
    "\n"
    "const float PI = 3.14159265359;\n"
    "\n"
    "void main() {\n"
    "vec2 position = fragTexCoord * 2.0 - 1.0;\n"
    "    vec2 positionInView = vec2(position.x, position.y * invAspectRatio) * zoom;\n"
    "    vec3 localPosition = normalize(getDirection(cameraRotation, positionInView));\n"
    "    int maxComponent = max3(abs(localPosition));\n"
    "\n"
    "    float x = localPosition.x, y = -localPosition.y, z = localPosition.z;\n"
    "\n"
    "    vec2 face;\n"
    "    vec2 uv;\n"
    "    float scale;\n"
    "    if (maxComponent == 0) {\n"
    "        scale = 1.0 / abs(x);\n"
    "        if (x >= 0.0) { //Right\n"
    "            face = vec2(1.0, 3.0);\n"
    "            uv.x = 4.0 * atan(z * scale) / PI;\n"
    "            uv.y = 4.0 * atan(y * scale) / PI;\n"
    "        } else { //Left\n"
    "            face = vec2(5.0, 3.0);\n"
    "            uv.x = -4.0 * atan(z * scale) / PI;\n"
    "            uv.y = 4.0 * atan(y * scale) / PI;\n"
    "        }\n"
    "    } else if (maxComponent == 1) {\n"
    "        scale = 1.0 / abs(y);\n"
    "        if (y >= 0.0) { //Top\n"
    "            face = vec2(5.0, 1.0);\n"
    "            uv.x = 4.0 * atan(z * scale) / PI;\n"
    "            uv.y = -4.0 * atan(x * scale) / PI;\n"
    "        } else { //Down\n"
    "            face = vec2(1.0, 1.0);\n"
    "            uv.x = -4.0 * atan(z * scale) / PI;\n"
    "            uv.y = -4.0 * atan(x * scale) / PI;\n"
    "        }\n"
    "    } else {\n"
    "        scale = 1.0 / abs(z);\n"
    "        if (z >= 0.0) { //Front\n"
    "            face = vec2(3.0, 3.0);\n"
    "            uv.x = -4.0 * atan(x * scale) / PI;\n"
    "            uv.y = 4.0 * atan(y * scale) / PI;\n"
    "        } else { //Back\n"
    "            face = vec2(3.0, 1.0);\n"
    "            uv.x = 4.0 * atan(y * scale) / PI;\n"
    "            uv.y = -4.0 * atan(x * scale) / PI;\n"
    "        }\n"
    "    }\n"
    "\n"
    "    uv = clamp(uv, -0.995, 0.995);\n"
    "    uv = vec2(face.x + uv.x, face.y + uv.y) / vec2(6.0, 4.0);\n"
    "    uv.y = 1.0 - uv.y;\n"
    "\n"
    "    gl_FragColor = vec4(texture2D(texture0, uv).rgb, 1.0);\n"
    "}\n"
    "";

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

RendererNemo::RendererNemo(QObject *parent) :
    QObject(parent),
    _sink(0),
    _queuedBuffer(nullptr),
    _currentBuffer(nullptr),
    _showFrameId(0),
    _buffersInvalidatedId(0),
    _notify(0),
    _needsInit(true),
    _program(0),
    _displaySet(false),
    _buffersInvalidated(false),
    _bufferChanged(false),
    _img(0),
    _projection(Projection::Flat),
    _projectionX(0.0f),
    _projectionY(0.0f) {

    _texCoords.resize(8);
    _vertexCoords.resize(8);

    _texCoords[0] = 0;             _texCoords[1] = 0;
    _texCoords[2] = 1;             _texCoords[3] = 0;
    _texCoords[4] = 1;             _texCoords[5] = 1;
    _texCoords[6] = 0;             _texCoords[7] = 1;

    for (int x = 0; x < 8; x++) {
        _vertexCoords[x] = 0;
    }

    _sceneGraphInitializedSignal = connect(QuickViewHelper::getView(), &QQuickView::sceneGraphInitialized, [&](){ createProgram(); });
}

RendererNemo::~RendererNemo() {
    disconnect(_sceneGraphInitializedSignal);
    disconnect(_sceneGraphInvalidatedSignal);

    cleanup();

    if (_queuedBuffer) {
        gst_buffer_unref(_queuedBuffer);
    }

    if (_currentBuffer) {
        gst_buffer_unref(_currentBuffer);
    }

    if (_program) {
        delete _program;
        _program = nullptr;
    }

    if (_img) {
        delete _img;
        _img = 0;
    }
}

bool RendererNemo::needsNativePainting() {
    return true;
}

void RendererNemo::setProjection(Projection projection)
{
    _projection = projection;
}

void RendererNemo::paint(const QMatrix4x4& matrix, const QRectF& viewport) {
    if (!_img) {
        QOpenGLContext *ctx = QOpenGLContext::currentContext();
        if (!ctx) {
            qCritical() << "No current OpenGL context";
            return;
        }

        if (!ctx->hasExtension("GL_OES_EGL_image")) {
            qCritical() << "GL_OES_EGL_image not supported";
            return;
        }

        _img = new QOpenGLExtension_OES_EGL_image;

        if (!_img->initializeOpenGLFunctions()) {
            qCritical() << "Failed to initialize GL_OES_EGL_image";
            delete _img;
            _img = 0;
            return;
        }
    }

    if (_dpy == EGL_NO_DISPLAY) {
        _dpy = eglGetCurrentDisplay();
    }

    if (_dpy == EGL_NO_DISPLAY) {
        qCritical() << "Failed to obtain EGL Display";
    }

    if (_sink && _dpy != EGL_NO_DISPLAY && !_displaySet) {
        g_object_set(G_OBJECT(_sink), "egl-display", _dpy, NULL);
        _displaySet = true;
    }

    QMutexLocker locker(&_frameMutex);
    if (!_queuedBuffer) {
        GstBuffer *currentBuffer = _currentBuffer;
        _currentBuffer = nullptr;

        locker.unlock();

        if (currentBuffer) {
            gst_buffer_unref(currentBuffer);
        }

        qDebug() << "No queued buffer";
        return;
    }

    if (_needsInit) {
        calculateProjectionMatrix(viewport);

        _needsInit = false;
    }

    if (!_program) {
        createProgram();
        return;
    }

    paintFrame(matrix);
}

void RendererNemo::resize(const QSizeF& size) {
    if (size == _size) {
        return;
    }

    _size = size;

    _renderArea = QRectF();

    calculateVertexCoords();

    _needsInit = true;

    emit renderAreaChanged();
}

void RendererNemo::reset() {
    QMutexLocker locker(&_frameMutex);

    destroyCachedTextures();
}

GstElement *RendererNemo::sinkElement() {
    if (!_sink) {
        _sink = gst_element_factory_make("droideglsink",
                                            "NemoSink");
        if (!_sink) {
            qCritical() << "Failed to create droideglsink";
            return 0;
        }

        g_object_add_toggle_ref(G_OBJECT(_sink), (GToggleNotify)sink_notify, this);
        _displaySet = false;
    }

    _dpy = eglGetCurrentDisplay();
    if (_dpy == EGL_NO_DISPLAY) {
        qCritical() << "Failed to obtain EGL Display";
    } else {
        g_object_set(G_OBJECT(_sink), "egl-display", _dpy, NULL);
        _displaySet = true;
    }

    _showFrameId = g_signal_connect(G_OBJECT(_sink), "show-frame", G_CALLBACK(show_frame), this);
    _buffersInvalidatedId = g_signal_connect(
            G_OBJECT(_sink), "buffers-invalidated", G_CALLBACK(buffers_invalidated), this);

    GstPad *pad = gst_element_get_static_pad(_sink, "sink");
    _notify = g_signal_connect(G_OBJECT(pad), "notify::caps",
                                    G_CALLBACK(sink_caps_changed), this);
    gst_object_unref(pad);

    return _sink;
}

void RendererNemo::sink_notify(RendererNemo *q, GObject *object, gboolean is_last_ref) {
    Q_UNUSED(object);

    if (is_last_ref) {
        q->cleanup();
    }
}

void RendererNemo::sink_caps_changed(GObject *obj, GParamSpec *pspec, RendererNemo *q) {
    Q_UNUSED(pspec);

    if (!obj) {
        return;
    }

    if (!GST_IS_PAD (obj)) {
        return;
    }

    GstPad *pad = GST_PAD (obj);
    GstCaps *caps = gst_pad_get_current_caps (pad);
    if (!caps) {
        return;
    }

    if (gst_caps_get_size (caps) < 1) {
        gst_caps_unref (caps);
        return;
    }

    GstVideoInfo info;
    if (!gst_video_info_from_caps (&info, caps)) {
        qWarning() << "failed to get video info";
        gst_caps_unref (caps);
        return;
    }

    QMetaObject::invokeMethod(q, "setVideoSize", Qt::QueuedConnection,
                                Q_ARG(QSizeF, QSizeF(info.width, info.height)));

    gst_caps_unref (caps);
}

void RendererNemo::calculateProjectionMatrix(const QRectF& rect) {
    _projectionMatrix = QMatrix4x4();
    _projectionMatrix.ortho(rect);
}

void RendererNemo::createProgram() {
    QMutexLocker locker(&_programMutex);
    if (_program) {
        delete _program;
    }

    _program = new QGLShaderProgram;

    if (!_program->addShaderFromSourceCode(QGLShader::Vertex, VERTEX_SHADER)) {
        qCritical() << "Failed to add vertex shader";
        return;
    }

    QString shader;

    switch (_projection) {
    case Flat:
        shader = FRAGMENT_SHADER;
        break;
    case s180:
        shader = FRAGMENT_SHADER_180;
        break;
    case s360:
        shader = FRAGMENT_SHADER_360;
        break;
    }

    if (!_program->addShaderFromSourceCode(QGLShader::Fragment, shader)) {
        qCritical() << "Failed to add fragment shader";
        return;
    }

    if (!_program->addShaderFromSourceCode(QGLShader::Fragment, shader)) {
        qCritical() << "Failed to add fragment shader";
        return;
    }

    _program->bindAttributeLocation("inputVertex", 0);
    _program->bindAttributeLocation("textureCoord", 1);

    if (!_program->bind()) {
        qCritical() << "Failed to bind program";
        return;
    }

    _program->setUniformValue("texture0", 0);
    _program->setUniformValue("cameraRotation", QVector2D(0.f, 0.f));
    _program->setUniformValue("invAspectRatio", 1.0f);
    _program->setUniformValue("zoom", 1.0f);
    _program->setUniformValue("onlyHalfOfTheScreen", false);
    _program->release();
}

void RendererNemo::paintFrame(const QMatrix4x4& matrix) {
    if (_buffersInvalidated) {
        _buffersInvalidated = false;
        destroyCachedTextures();
    }


    GstBuffer *bufferToRelease = nullptr;
    if (_currentBuffer != _queuedBuffer && _bufferChanged) {
        bufferToRelease = _currentBuffer;

        _currentBuffer = gst_buffer_ref(_queuedBuffer);
    }

    _bufferChanged = false;

    if (!_currentBuffer || gst_buffer_n_memory(_currentBuffer) == 0) {
        return;
    }

    std::vector<GLfloat> texCoords(_texCoords);

    GLuint texture = 0;

    GstMemory *memory = gst_buffer_peek_memory(_currentBuffer, 0);

    for (CachedTexture &cachedTexture : _textures) {
        if (cachedTexture.memory == memory) {
            texture = cachedTexture.textureId;
            glBindTexture(GL_TEXTURE_EXTERNAL_OES, texture);
            _img->glEGLImageTargetTexture2DOES(GL_TEXTURE_EXTERNAL_OES, cachedTexture.image);
        }
    }

    if (texture == 0) {
        if (EGLImageKHR img = nemo_gst_egl_image_memory_create_image(memory, _dpy, nullptr)) {
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_EXTERNAL_OES, texture);
            glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glActiveTexture(GL_TEXTURE0);

            _img->glEGLImageTargetTexture2DOES (GL_TEXTURE_EXTERNAL_OES, (GLeglImageOES)img);

            CachedTexture cachedTexture = { gst_memory_ref(memory), img, texture };
            _textures.push_back(cachedTexture);
        }
    }

    _program->bind();

    _program->setUniformValue("matrix", _projectionMatrix);
    _program->setUniformValue("matrixWorld", matrix);
    _program->setUniformValue("cameraRotation", QVector2D(_projectionY*90.f, _projectionX*(_projection == Projection::s360 ? 180.f : 90.f)));
    _program->setUniformValue("invAspectRatio", (float)_videoSize.height()/(float)_videoSize.width());
    _program->setUniformValue("zoom", 1.0f);
    _program->setUniformValue("onlyHalfOfTheScreen", true);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, &_vertexCoords[0]);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, &texCoords[0]);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

    _program->release();

    glBindTexture(GL_TEXTURE_EXTERNAL_OES, 0);

    if (bufferToRelease) {
        gst_buffer_unref(bufferToRelease);
    }
}

void RendererNemo::calculateVertexCoords() {
    if (!_size.isValid() || !_videoSize.isValid()) {
        return;
    }

    QRectF area = renderArea();

    qreal leftMargin = area.x();
    qreal topMargin = area.y();
    QSizeF renderSize = area.size();

    _vertexCoords[0] = leftMargin;
    _vertexCoords[1] = topMargin + renderSize.height();

    _vertexCoords[2] = renderSize.width() + leftMargin;
    _vertexCoords[3] = topMargin + renderSize.height();

    _vertexCoords[4] = renderSize.width() + leftMargin;
    _vertexCoords[5] = topMargin;

    _vertexCoords[6] = leftMargin;
    _vertexCoords[7] = topMargin;
}

QRectF RendererNemo::renderArea() {
    if (!_renderArea.isNull()) {
        return _renderArea;
    }

    QSizeF renderSize = _videoSize;
    renderSize.scale(_size, Qt::KeepAspectRatio);

    qreal leftMargin = (_size.width() - renderSize.width())/2.0;
    qreal topMargin = (_size.height() - renderSize.height())/2.0;

    _renderArea = QRectF(QPointF(leftMargin, topMargin), renderSize);

    return _renderArea;
}

QSizeF RendererNemo::videoResolution() {
    return _videoSize;
}

float RendererNemo::getProjectionX() const
{
    return _projectionX;
}

void RendererNemo::setProjectionX(float projectionX)
{
    _projectionX = std::clamp(projectionX, -1.0f, 1.0f);

    if (_projectionX == 1.0f) _projectionX = -1.0f;
    else if (_projectionX == -1.0f) _projectionX = 1.0f;

    QMetaObject::invokeMethod(this, "updateRequested", Qt::QueuedConnection);
}

float RendererNemo::getProjectionY() const
{
    return _projectionY;
}

void RendererNemo::setProjectionY(float projectionY)
{
    _projectionY = std::clamp(projectionY, -1.0f, 1.0f);

    QMetaObject::invokeMethod(this, "updateRequested", Qt::QueuedConnection);
}

void RendererNemo::setVideoSize(const QSizeF& size) {
    if (size == _videoSize) {
        return;
    }

    _videoSize = size;

    _renderArea = QRectF();

    calculateVertexCoords();

    _needsInit = true;

    emit renderAreaChanged();
    emit videoResolutionChanged();
}

void RendererNemo::show_frame(GstVideoSink *, GstBuffer *buffer, RendererNemo *r)
{
    QMutexLocker locker(&r->_frameMutex);

    GstBuffer * const bufferToRelease = r->_queuedBuffer;
    r->_queuedBuffer = buffer ? gst_buffer_ref(buffer) : nullptr;
    r->_bufferChanged = true;

    locker.unlock();

    if (bufferToRelease) {
        gst_buffer_unref(bufferToRelease);
    }

    QMetaObject::invokeMethod(r, "updateRequested", Qt::QueuedConnection);
}

void RendererNemo::buffers_invalidated(GstVideoSink *, RendererNemo *r)
{
    {
        QMutexLocker locker(&r->_frameMutex);
        r->_buffersInvalidated = true;
    }
    QMetaObject::invokeMethod(r, "updateRequested", Qt::QueuedConnection);
}

void RendererNemo::cleanup() {
    if (!_sink) {
        return;
    }

    destroyCachedTextures();

    if (_showFrameId) {
        g_signal_handler_disconnect(_sink, _showFrameId);
        _showFrameId = 0;
    }

    if (_buffersInvalidatedId) {
        g_signal_handler_disconnect(_sink, _buffersInvalidatedId);
        _buffersInvalidatedId = 0;
    }


    if (_notify) {
        g_signal_handler_disconnect(_sink, _notify);
        _notify = 0;
    }

    g_object_remove_toggle_ref(G_OBJECT(_sink), (GToggleNotify)sink_notify, this);
    _sink = 0;
}

void RendererNemo::destroyCachedTextures()
{
    static const PFNEGLDESTROYIMAGEKHRPROC eglDestroyImageKHR
        = reinterpret_cast<PFNEGLDESTROYIMAGEKHRPROC>(eglGetProcAddress("eglDestroyImageKHR"));

    for (CachedTexture &texture : _textures) {
        glDeleteTextures(1, &texture.textureId);

        eglDestroyImageKHR(_dpy, texture.image);

        gst_memory_unref(texture.memory);
    }

    _textures.clear();
}

void RendererNemo::updateCropInfo(const GstStructure *s,
                                                 std::vector<GLfloat>& texCoords) {
    int right = 0, bottom = 0, top = 0, left = 0;

    if (!gst_structure_get_int(s, "top", &top) ||
            !gst_structure_get_int(s, "left", &left) ||
            !gst_structure_get_int(s, "bottom", &bottom) ||
            !gst_structure_get_int(s, "right", &right)) {
        qWarning() << "incomplete crop info";
        return;
    }

    if ((right - left) <= 0 || (bottom - top) <= 0) {
        return;
    }

    int width = right - left;
    int height = bottom - top;
    qreal tx = 0.0f, ty = 0.0f, sx = 1.0f, sy = 1.0f;
    int bufferWidth = _videoSize.width();
    int bufferHeight = _videoSize.height();
    if (width < bufferWidth) {
        tx = (qreal)left / (qreal)bufferWidth;
        sx = (qreal)right / (qreal)bufferWidth;
    }

    if (height < bufferHeight) {
        ty = (qreal)top / (qreal)bufferHeight;
        sy = (qreal)bottom / (qreal)bufferHeight;
    }

    texCoords[0] = tx;             texCoords[1] = ty;
    texCoords[2] = sx;             texCoords[3] = ty;
    texCoords[4] = sx;             texCoords[5] = sy;
    texCoords[6] = tx;             texCoords[7] = sy;
}
