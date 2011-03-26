/*
* Copyright (C) 2008-2011 J-P Nurmi <jpnurmi@gmail.com>
*
* This library is free software; you can redistribute it and/or modify it
* under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
* License for more details.
*/

#ifndef IRCSESSION_P_H
#define IRCSESSION_P_H

#include <QString>
#include <QByteArray>
#include <QMultiHash>
#include <QAbstractSocket>
#include "ircbuffer.h"
#include "ircparser_p.h"

class IrcSessionPrivate
{
    Q_DECLARE_PUBLIC(IrcSession)

public:
    IrcSessionPrivate();

    void init(IrcSession* session);

    void _q_connected();
    void _q_disconnected();
    void _q_reconnect();
    void _q_error();
    void _q_state(QAbstractSocket::SocketState state);
    void _q_readData();

    void readLines(const QByteArray& delimiter);
    void processLine(const QByteArray& line);
    bool isConnected() const;
    //QString resolveTarget(const QString& sender, const QString& receiver) const;

    //Buffer* createBuffer(const QString& receiver);
    //void removeBuffer(Buffer* buffer);

    bool raw(const QString& msg);

    IrcSession* q_ptr;
    IrcParser parser;
    QByteArray buffer;
    QAbstractSocket* socket;
    QString host;
    quint16 port;
    QString motd;
    bool welcomed;
    IrcBuffer* mainBuffer;
    QMultiHash<QString, IrcBuffer*> buffers;
};

#endif // IRCSESSION_P_H
