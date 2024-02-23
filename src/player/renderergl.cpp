#include "renderergl.h"

static const QString FRAGMENT_SHADER = ""
                                       "uniform sampler2D texture0;"
                                       "varying lowp vec2 fragTexCoord;"
                                       "void main() {"
                                       "    gl_FragColor = texture2D(texture0, fragTexCoord);"
                                       "}"
                                       "";

static const QString FRAGMENT_SHADER_180 = ""
                                           "varying lowp vec2 fragTexCoord;\n"
                                           "\n"
                                           "//Uniforms\n"
                                           "uniform vec2 cameraRotation;\n"
                                           "uniform float invAspectRatio; //height / width\n"
                                           "uniform float zoom;\n"
                                           "uniform bool onlyHalfOfTheScreen;\n"
                                           "\n"
                                           "//Texture\n"
                                           "uniform sampler2D texture0;\n"
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
                                           "varying lowp vec2 fragTexCoord;\n"
                                           "\n"
                                           "//Uniforms\n"
                                           "uniform vec2 cameraRotation;\n"
                                           "uniform float invAspectRatio;\n"
                                           "uniform float zoom;\n"
                                           "\n"
                                           "//Texture\n"
                                           "uniform sampler2D texture0;\n"
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

RendererGL::RendererGL(QObject *parent) :
    Renderer(parent),
    _sink(0),
    _reshapeId(0),
    _drawId(0),
    _notify(0),
    _texture(0),
    _needsInit(true),
    _program(0),
    _displaySet(false)
{
}

RendererGL::~RendererGL()
{

}

void RendererGL::paint(const QMatrix4x4 &matrix, const QRectF &viewport)
{
    QOpenGLContext *ctx = QOpenGLContext::currentContext();
    if (!ctx) {
        qCritical() << "No current OpenGL context";
        return;
    }
    g_object_set(G_OBJECT(_sink), "context", ctx, NULL);

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

void RendererGL::resize(const QSizeF &size)
{
    if (size == _size) {
        return;
    }

    _size = size;

    _renderArea = QRectF();

    calculateVertexCoords();

    _needsInit = true;

    emit renderAreaChanged();
}

void RendererGL::reset()
{
}

GstElement *RendererGL::sinkElement()
{
    if (!_sink) {
        _sink = gst_element_factory_make("glimagesinkelement",
                                         "GLImageSink");
        if (!_sink) {
            qCritical() << "Failed to create glimagesink";
            return 0;
        }

        g_object_add_toggle_ref(G_OBJECT(_sink), (GToggleNotify)sink_notify, this);
        _displaySet = false;
    }

    QOpenGLContext *ctx = QOpenGLContext::currentContext();
    if (!ctx) {
        qCritical() << "No current OpenGL context";
    } else {
        g_object_set(G_OBJECT(_sink), "context", ctx, NULL);
    }

    _dpy = eglGetCurrentDisplay();
    if (_dpy == EGL_NO_DISPLAY) {
        qCritical() << "Failed to obtain EGL Display";
    } else {
        g_object_set(G_OBJECT(_sink), "egl-display", _dpy, NULL);
        _displaySet = true;
    }

    _reshapeId = g_signal_connect(G_OBJECT(_sink), "client-reshape", G_CALLBACK (reshapeCallback), NULL);
    _drawId = g_signal_connect(G_OBJECT(_sink), "client-draw", G_CALLBACK (drawCallback), NULL);

    GstPad *pad = gst_element_get_static_pad(_sink, "sink");
    _notify = g_signal_connect(G_OBJECT(pad), "notify::caps",
                               G_CALLBACK(sink_caps_changed), this);
    gst_object_unref(pad);

    return _sink;
}

QRectF RendererGL::renderArea()
{
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

QSizeF RendererGL::videoResolution()
{
    return _videoSize;
}

void RendererGL::setVideoSize(const QSizeF &size)
{
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

void RendererGL::reshapeCallback(GstVideoSink *, GLuint width, GLuint height, RendererGL *r)
{
    QMetaObject::invokeMethod(r, "setVideoSize", Qt::QueuedConnection, Q_ARG(QSizeF, QSizeF(width, height)));
}

void RendererGL::drawCallback(GstVideoSink *sink, GLuint texture, GLuint width, GLuint height, RendererGL *r)
{

}

void RendererGL::sink_notify(RendererGL *q, GObject *object, gboolean is_last_ref)
{
    Q_UNUSED(object);

    if (is_last_ref) {
        q->cleanup();
    }
}

void RendererGL::sink_caps_changed(GObject *obj, GParamSpec *pspec, RendererGL *q)
{
    Q_UNUSED(pspec);

    if (!obj) {
        return;
    }

    if (!GST_IS_PAD(obj)) {
        return;
    }

    GstPad *pad = GST_PAD(obj);
    GstCaps *caps = gst_pad_get_current_caps(pad);
    if (!caps) {
        return;
    }

    if (gst_caps_get_size(caps) < 1) {
        gst_caps_unref(caps);
        return;
    }

    GstVideoInfo info;
    if (!gst_video_info_from_caps(&info, caps)) {
        qWarning() << "failed to get video info";
        gst_caps_unref(caps);
        return;
    }

    QMetaObject::invokeMethod(q, "setVideoSize", Qt::QueuedConnection,
                              Q_ARG(QSizeF, QSizeF(info.width, info.height)));

    gst_caps_unref(caps);
}

void RendererGL::calculateProjectionMatrix(const QRectF &rect)
{
    _projectionMatrix = QMatrix4x4();
    _projectionMatrix.ortho(rect);
}

void RendererGL::createProgram()
{
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

void RendererGL::paintFrame(const QMatrix4x4 &matrix)
{

}

void RendererGL::calculateVertexCoords()
{
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

void RendererGL::cleanup()
{
    if (!_sink) {
        return;
    }

    if (_reshapeId) {
        g_signal_handler_disconnect(_sink, _reshapeId);
        _reshapeId = 0;
    }

    if (_drawId) {
        g_signal_handler_disconnect(_sink, _drawId);
        _drawId = 0;
    }


    if (_notify) {
        g_signal_handler_disconnect(_sink, _notify);
        _notify = 0;
    }

    g_object_remove_toggle_ref(G_OBJECT(_sink), (GToggleNotify)sink_notify, this);

    _sink = nullptr;
}
