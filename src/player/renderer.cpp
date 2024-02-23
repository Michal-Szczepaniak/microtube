#include "renderer.h"
#include "renderergl.h"
#include "renderernemo.h"

Renderer *Renderer::create(QObject *parent, bool droid)
{
    if (droid) {
        return new RendererNemo(parent);
    } else {
        return new RendererGL(parent);
    }
}

Renderer::~Renderer()
{

}

float Renderer::getProjectionX() const
{
    return _projectionX;
}

void Renderer::setProjectionX(float projectionX)
{
    _projectionX = std::clamp(projectionX, -1.0f, 1.0f);

    if (_projectionX == 1.0f) _projectionX = -1.0f;
    else if (_projectionX == -1.0f) _projectionX = 1.0f;

    QMetaObject::invokeMethod(this, "updateRequested", Qt::QueuedConnection);
}

float Renderer::getProjectionY() const
{
    return _projectionY;
}

void Renderer::setProjectionY(float projectionY)
{
    _projectionY = std::clamp(projectionY, -1.0f, 1.0f);

    QMetaObject::invokeMethod(this, "updateRequested", Qt::QueuedConnection);
}

void Renderer::setProjection(Projection projection)
{
    _projection = projection;
}

Renderer::Renderer(QObject *parent) :
    QObject(parent),
    _projection(Projection::Flat),
    _projectionX(0.0f),
    _projectionY(0.0f)
{

}
