/*
 * Copyright (c) 2012-2013 Dickson Leong.
 * All rights reserved.
 *
 * This file is part of GagBook.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "networkmanager.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

static const QByteArray USER_AGENT = QByteArray("GagBook/") + APP_VERSION;

QScopedPointer<NetworkManager> NetworkManager::m_instance(new NetworkManager);

NetworkManager::NetworkManager(QObject *parent) :
    QObject(parent), m_networkAccessManager(new QNetworkAccessManager(this)),
    m_downloadCounter(0), m_downloadCounterStr("0.00")
{
    connect(m_networkAccessManager, SIGNAL(finished(QNetworkReply*)), SLOT(increaseDownloadCounter(QNetworkReply*)));
}

NetworkManager *NetworkManager::instance()
{
    return m_instance.data();
}

QNetworkReply *NetworkManager::createGetRequest(const QUrl &url, AcceptType acceptType)
{
    QNetworkRequest request;
    request.setUrl(url);
    request.setRawHeader("User-Agent", USER_AGENT);

    switch (acceptType) {
    case None: break;
    case JSON: request.setRawHeader("Accept", "application/json"); break;
    case HTML: request.setRawHeader("Accept", "text/html"); break;
    case Image: request.setRawHeader("Accept", "image/*"); break;
    default: qWarning("NetworkManager::createGetRequest(): Invalid acceptType"); break;
    }

    return m_instance->m_networkAccessManager->get(request);
}

void NetworkManager::increaseDownloadCounter(QNetworkReply *reply)
{
    m_downloadCounter += reply->size();
    const QString downloadCounterStr = QString::number(qreal(m_downloadCounter) / 1024 / 1024, 'f', 2);
    if (m_downloadCounterStr != downloadCounterStr) {
        m_downloadCounterStr = downloadCounterStr;
        emit downloadCounterChanged();
    }
}
