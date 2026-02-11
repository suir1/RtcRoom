#include "SignalManager.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

SignalManager::SignalManager(QObject *parent)
    : QObject(parent)
{
    connect(&m_webSocket, &QWebSocket::connected, this, &SignalManager::onConnected);
    connect(&m_webSocket, &QWebSocket::textMessageReceived, this, &SignalManager::onTextMessageReceived);
}

void SignalManager::connectToServer(const QUrl &url)
{
    m_webSocket.open(url);
}

void SignalManager::joinRoom(const QString &roomId)
{
    m_roomId = roomId;
    QJsonObject json;
    json["type"] = "join";
    json["roomId"] = roomId;
    m_webSocket.sendTextMessage(QJsonDocument(json).toJson(QJsonDocument::Compact));
}

void SignalManager::sendOffer(const QJsonObject &sdp)
{
    QJsonObject json;
    json["type"] = "offer";
    json["payload"] = sdp;
    m_webSocket.sendTextMessage(QJsonDocument(json).toJson(QJsonDocument::Compact));
}

void SignalManager::sendAnswer(const QJsonObject &sdp)
{
    QJsonObject json;
    json["type"] = "answer";
    json["payload"] = sdp;
    m_webSocket.sendTextMessage(QJsonDocument(json).toJson(QJsonDocument::Compact));
}

void SignalManager::sendCandidate(const QJsonObject &candidate)
{
    QJsonObject json;
    json["type"] = "candidate";
    json["payload"] = candidate;
    m_webSocket.sendTextMessage(QJsonDocument(json).toJson(QJsonDocument::Compact));
}

void SignalManager::onConnected()
{
    qDebug() << "WebSocket connected";
    emit connected();
}

void SignalManager::onTextMessageReceived(const QString &message)
{
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
    if (!doc.isObject()) return;

    QJsonObject obj = doc.object();
    QString type = obj["type"].toString();

    if (type == "offer") {
        emit offerReceived(obj["payload"].toObject());
    } else if (type == "answer") {
        emit answerReceived(obj["payload"].toObject());
    } else if (type == "candidate") {
        emit candidateReceived(obj["payload"].toObject());
    } else if (type == "new-peer") {
        emit newPeerJoined();
    }
}
