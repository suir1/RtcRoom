#pragma once

#include <QObject>
#include <QWebSocket>
#include <QJsonDocument>
#include <QJsonObject>

class SignalManager : public QObject
{
    Q_OBJECT
public:
    explicit SignalManager(QObject *parent = nullptr);
    void connectToServer(const QUrl &url);
    void joinRoom(const QString &roomId);
    void sendOffer(const QJsonObject &sdp);
    void sendAnswer(const QJsonObject &sdp);
    void sendCandidate(const QJsonObject &candidate);

signals:
    void connected();
    void disconnected();
    void offerReceived(const QJsonObject &sdp);
    void answerReceived(const QJsonObject &sdp);
    void candidateReceived(const QJsonObject &candidate);
    void joinedRoom();
    void newPeerJoined();

private slots:
    void onConnected();
    void onTextMessageReceived(const QString &message);

private:
    QWebSocket m_webSocket;
    QString m_roomId;
};
