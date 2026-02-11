#include "VideoRenderer.h"
#include <QDebug>

#ifdef USE_WEBRTC
#include "api/video/i420_buffer.h"
#include "third_party/libyuv/include/libyuv.h" // Assuming libyuv is available via WebRTC build
#endif

VideoRenderer::VideoRenderer(QObject *parent)
    : QObject(parent)
{
}

VideoRenderer::~VideoRenderer()
{
}

#ifdef USE_WEBRTC
void VideoRenderer::OnFrame(const webrtc::VideoFrame& frame)
{
    rtc::scoped_refptr<webrtc::I420BufferInterface> buffer(frame.video_frame_buffer()->ToI420());
    if (!buffer) {
        return;
    }

    // Prepare QImage
    // Note: This simple conversion is for demonstration.
    // In production, you might want to use OpenGL or a more efficient mechanism.
    // Also, QImage creation/allocation in a hot loop can be expensive.

    int width = buffer->width();
    int height = buffer->height();

    QImage image(width, height, QImage::Format_ARGB32);

    // Using libyuv to convert I420 to ARGB
    // libyuv::I420ToARGB(buffer->DataY(), buffer->StrideY(),
    //                    buffer->DataU(), buffer->StrideU(),
    //                    buffer->DataV(), buffer->StrideV(),
    //                    image.bits(), width * 4,
    //                    width, height);

    // Emit signal to UI thread
    // Note: Creating a copy of QImage here is necessary to pass across threads safely
    // unless we use a shared buffer mechanism.
    emit frameReady(image.copy());
}
#endif
