#include "MainWindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_signalManager(new SignalManager(this))
    , m_webrtcManager(new WebRTCManager(this))
    , m_localRenderer(new VideoRenderer(this))
    , m_remoteRenderer(new VideoRenderer(this))
{
    setupUi();

    m_webrtcManager->init();

    connect(m_connectButton, &QPushButton::clicked, this, &MainWindow::onConnectClicked);
    connect(m_joinButton, &QPushButton::clicked, this, &MainWindow::onJoinClicked);
    connect(m_shareScreenButton, &QPushButton::clicked, this, &MainWindow::onShareScreenClicked);

    connect(m_signalManager, &SignalManager::connected, this, [this]() {
        m_statusLabel->setText("Status: Connected to Server");
        m_joinButton->setEnabled(true);
        m_connectButton->setEnabled(false);
    });

    connect(m_signalManager, &SignalManager::offerReceived, this, [this](const QJsonObject &sdp){
        qDebug() << "Offer received";
        QString type = sdp["type"].toString();
        QString sdpStr = sdp["sdp"].toString();
        m_webrtcManager->setRemoteDescription(type, sdpStr);
        m_webrtcManager->createAnswer();
    });

    connect(m_signalManager, &SignalManager::answerReceived, this, [this](const QJsonObject &sdp){
        qDebug() << "Answer received";
        QString type = sdp["type"].toString();
        QString sdpStr = sdp["sdp"].toString();
        m_webrtcManager->setRemoteDescription(type, sdpStr);
    });

    connect(m_signalManager, &SignalManager::candidateReceived, this, [this](const QJsonObject &candidate){
        qDebug() << "Candidate received";
        QString sdpMid = candidate["sdpMid"].toString();
        int sdpMLineIndex = candidate["sdpMLineIndex"].toInt();
        QString candidateStr = candidate["candidate"].toString();
        m_webrtcManager->addIceCandidate(sdpMid, sdpMLineIndex, candidateStr);
    });

    connect(m_webrtcManager, &WebRTCManager::localDescriptionGenerated, this, [this](const QString &type, const QString &sdp){
        QJsonObject json;
        json["type"] = type;
        json["sdp"] = sdp;

        if (type == "offer") {
            m_signalManager->sendOffer(json);
        } else {
            m_signalManager->sendAnswer(json);
        }
    });

    connect(m_webrtcManager, &WebRTCManager::iceCandidateGenerated, this, [this](const QJsonObject &candidate){
        m_signalManager->sendCandidate(candidate);
    });

    connect(m_webrtcManager, &WebRTCManager::remoteStreamAdded, this, [this](){
        qDebug() << "Remote stream added!";
        // Attach stream to remote renderer
        // m_remoteRenderer is a VideoSinkInterface
        // stream->getVideoTracks()[0]->AddOrUpdateSink(m_remoteRenderer, rtc::VideoSinkWants());

        m_statusLabel->setText("Status: Remote Stream Received");
        m_remoteVideoWidget->setStyleSheet("background-color: #222;");
    });

    connect(m_localRenderer, &VideoRenderer::frameReady, this, [this](const QImage &img){
        // Display local video
        // m_localVideoLabel->setPixmap(QPixmap::fromImage(img));
    });

    connect(m_webrtcManager, &WebRTCManager::localStreamAdded, this, [this](){
        qDebug() << "Local stream added!";
        // Attach to local renderer
        // m_webrtcManager->getLocalVideoTrack()->AddOrUpdateSink(m_localRenderer, rtc::VideoSinkWants());
        m_localVideoWidget->setStyleSheet("background-color: #444;");
    });

    connect(m_remoteRenderer, &VideoRenderer::frameReady, this, [this](const QImage &img){
        // Display remote video
         // m_remoteVideoLabel->setPixmap(QPixmap::fromImage(img));
    });
    connect(m_signalManager, &SignalManager::newPeerJoined, this, [this](){
        qDebug() << "New peer joined, initiating offer";
        m_statusLabel->setText("Status: New Peer Joined - Calling...");
        m_webrtcManager->createOffer();
    });
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUi()
{
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);
    m_mainLayout = new QVBoxLayout(m_centralWidget);

    // Connection Controls
    QHBoxLayout *connLayout = new QHBoxLayout();
    m_serverUrlEdit = new QLineEdit("ws://64.186.238.105:3102/ws", this);
    m_connectButton = new QPushButton("Connect", this);
    connLayout->addWidget(new QLabel("Server:"));
    connLayout->addWidget(m_serverUrlEdit);
    connLayout->addWidget(m_connectButton);
    m_mainLayout->addLayout(connLayout);

    // Room Controls
    QHBoxLayout *roomLayout = new QHBoxLayout();
    m_roomIdEdit = new QLineEdit("testroom", this);
    m_joinButton = new QPushButton("Join Room", this);
    m_joinButton->setEnabled(false);
    roomLayout->addWidget(new QLabel("Room:"));
    roomLayout->addWidget(m_roomIdEdit);
    roomLayout->addWidget(m_joinButton);
    m_shareScreenButton = new QPushButton("Share Screen", this);
    m_shareScreenButton->setEnabled(false); // Enable after joining
    roomLayout->addWidget(m_shareScreenButton);
    m_mainLayout->addLayout(roomLayout);

    m_statusLabel = new QLabel("Status: Disconnected", this);
    m_mainLayout->addWidget(m_statusLabel);

    // Video Area (Placeholder)
    QHBoxLayout *videoLayout = new QHBoxLayout();
    m_localVideoWidget = new QWidget(this);
    m_localVideoWidget->setStyleSheet("background-color: black;");
    m_localVideoWidget->setMinimumSize(320, 240);

    m_remoteVideoWidget = new QWidget(this);
    m_remoteVideoWidget->setStyleSheet("background-color: gray;");
    m_remoteVideoWidget->setMinimumSize(320, 240);

    videoLayout->addWidget(m_localVideoWidget);
    videoLayout->addWidget(m_remoteVideoWidget);
    m_mainLayout->addLayout(videoLayout);

    setWindowTitle("RtcRoom Client");
    resize(800, 600);
}

void MainWindow::onConnectClicked()
{
    QUrl url(m_serverUrlEdit->text());
    m_signalManager->connectToServer(url);
    m_statusLabel->setText("Status: Connecting...");
}

void MainWindow::onJoinClicked()
{
    QString roomId = m_roomIdEdit->text();
    if (!roomId.isEmpty()) {
        m_signalManager->joinRoom(roomId);
        m_statusLabel->setText("Status: Joined Room " + roomId);
        m_shareScreenButton->setEnabled(true);
    }
}

#include <QInputDialog>

void MainWindow::onShareScreenClicked()
{
    auto sources = m_webrtcManager->getScreenSourceList();
    QStringList items;
    for (const auto& source : sources) {
        QString type = source.isWindow ? "Window" : "Screen";
        items << QString("%1: %2 (%3)").arg(source.id).arg(source.title).arg(type);
    }

    bool ok;
    QString item = QInputDialog::getItem(this, tr("Select Source"),
                                         tr("Source:"), items, 0, false, &ok);
    if (ok && !item.isEmpty()) {
        // Parse ID from string (simple parsing based on format above)
        // Format: "ID: Title (Type)"
        int64_t sourceId = item.split(":")[0].toLongLong();

        // Find isWindow
        bool isWindow = item.contains("(Window)");

        m_webrtcManager->setScreenCapture(sourceId, isWindow);
        m_statusLabel->setText("Status: Sharing " + item);
    }
}
