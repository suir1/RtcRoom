#pragma once

#include <QObject>
#include <QJsonObject>
#include <QString>
#include <memory>
#include <functional>

// Use guards to prevent compilation errors if WebRTC headers are missing in this environment
// In the actual Windows build, these should be available.
#ifdef USE_WEBRTC
#include "api/peer_connection_interface.h"
#include "api/media_stream_interface.h"
#include "api/create_peerconnection_factory.h"
#include "api/video_codecs/video_decoder_factory.h"
#include "api/video_codecs/video_encoder_factory.h"
#include "api/audio_codecs/audio_decoder_factory.h"
#include "api/audio_codecs/audio_decoder_factory.h"
#include "api/audio_codecs/audio_encoder_factory.h"
#include "modules/desktop_capture/desktop_capturer.h"
#include "modules/desktop_capture/desktop_capture_options.h"
#endif

class WebRTCManager : public QObject
#ifdef USE_WEBRTC
    , public webrtc::PeerConnectionObserver
    , public webrtc::CreateSessionDescriptionObserver
    , public webrtc::SetSessionDescriptionObserver
#endif
{
    Q_OBJECT

public:
    explicit WebRTCManager(QObject *parent = nullptr);
    ~WebRTCManager();

    void init();
    void createOffer();
    void createAnswer();
    void setRemoteDescription(const QString &type, const QString &sdp);
    void addIceCandidate(const QString &sdpMid, int sdpMLineIndex, const QString &candidate);

signals:
    void localDescriptionGenerated(const QString &type, const QString &sdp);
    void iceCandidateGenerated(const QJsonObject &candidate);
    void remoteStreamAdded();
    void localStreamAdded();

    struct SourceInfo {
        int64_t id;
        QString title;
        bool isWindow;
    };

    std::vector<SourceInfo> getScreenSourceList();
    void setScreenCapture(int64_t sourceId, bool isWindow);

private:
#ifdef WIN32
    // PeerConnectionObserver implementation
    void OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state) override {}
    void OnAddStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) override;
    void OnRemoveStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) override {}
    void OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel) override {}
    void OnRenegotiationNeeded() override {}
    void OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state) override;
    void OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state) override {}
    void OnIceCandidate(const webrtc::IceCandidateInterface* candidate) override;

    // CreateSessionDescriptionObserver implementation
    void OnSuccess(webrtc::SessionDescriptionInterface* desc) override;
    void OnFailure(webrtc::RTCError error) override;

    // SetSessionDescriptionObserver implementation -- using dummy for now as it shares OnSuccess/OnFailure names if not careful or using headers
    // Actually SetSessionDescriptionObserver only has OnSuccess() and OnFailure(const std::string& error) in older versions
    // or OnFailure(webrtc::RTCError) in newer.
    // We will separate these if needed, usually implemented via helper classes or checking context.
    // For simplicity in this skeleton, we'll assume we use separate observer objects or this main class handles Create.

    rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> peer_connection_factory_;
    rtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_connection_;
    rtc::scoped_refptr<webrtc::VideoTrackInterface> local_video_track_;
    rtc::scoped_refptr<webrtc::AudioTrackInterface> local_audio_track_;

    rtc::scoped_refptr<webrtc::VideoTrackSourceInterface> OpenVideoCaptureDevice();
#endif
};
