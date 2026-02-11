#pragma once

#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QLabel>
#include "SignalManager.h"
#include "WebRTCManager.h"
#include "VideoRenderer.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onConnectClicked();
    void onConnectClicked();
    void onJoinClicked();
    void onShareScreenClicked();

private:
    SignalManager *m_signalManager;
    WebRTCManager *m_webrtcManager;
    VideoRenderer *m_localRenderer;
    VideoRenderer *m_remoteRenderer;

    // UI Elements
    QWidget *m_centralWidget;
    QVBoxLayout *m_mainLayout;
    QLineEdit *m_serverUrlEdit;
    QPushButton *m_connectButton;
    QLineEdit *m_roomIdEdit;
    QLineEdit *m_roomIdEdit;
    QPushButton *m_joinButton;
    QPushButton *m_shareScreenButton;
    QLabel *m_statusLabel;

    // Placeholder for Video Renderers
    QWidget *m_localVideoWidget;
    QWidget *m_remoteVideoWidget;

    void setupUi();
};
