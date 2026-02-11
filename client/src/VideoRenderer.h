#pragma once

#include <QObject>
#include <QImage>
#include <mutex>

#ifdef USE_WEBRTC
#include "api/video/video_frame.h"
#include "api/video/video_sink_interface.h"
#endif

class VideoRenderer : public QObject
#ifdef USE_WEBRTC
    , public rtc::VideoSinkInterface<webrtc::VideoFrame>
#endif
{
    Q_OBJECT

public:
    explicit VideoRenderer(QObject *parent = nullptr);
    ~VideoRenderer();

#ifdef USE_WEBRTC
    void OnFrame(const webrtc::VideoFrame& frame) override;
#endif

signals:
    void frameReady(const QImage &image);
};
