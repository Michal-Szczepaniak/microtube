#ifndef QMLSUBTITLESRENDERER_H
#define QMLSUBTITLESRENDERER_H

#include <QObject>
#include <gst/gst.h>
#include <gst/video/video.h>

static GstStaticPadTemplate sink_template_factory =
    GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("text/x-raw, format = { pango-markup, utf8 }")
);

class QmlSubtitlesRenderer : public QObject
{
    Q_OBJECT
public:
    explicit QmlSubtitlesRenderer(QObject *parent = nullptr);

signals:

};

#endif // QMLSUBTITLESRENDERER_H
