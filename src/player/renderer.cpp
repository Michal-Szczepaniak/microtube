#include "renderer.h"
#include "renderernemo.h"

QtCamViewfinderRenderer::QtCamViewfinderRenderer(QObject *parent) : QObject(parent) {

}

QtCamViewfinderRenderer::~QtCamViewfinderRenderer() {

}

QtCamViewfinderRenderer *QtCamViewfinderRenderer::create(QObject *parent) {
    return new QtCamViewfinderRendererNemo(parent);
}
