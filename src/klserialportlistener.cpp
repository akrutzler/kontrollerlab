/***************************************************************************
 *   Copyright (C) 2006 by Martin Strasser                                 *
 *   strasser  a t  cadmaniac  d o t  org                                  *
 *   Special thanks to Mario Boikov                                        *
 *   squeeze  a t  cadmaniac  d o t  org                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "klserialportlistener.h"
#include "klserialport.h"
#include "klserialterminalwidget.h"
#include <qapplication.h>
#include <kglobal.h>


KLSerialPortListener::KLSerialPortListener(KLSerialTerminalWidget *parent, KLSerialPort* spInstance, const char *name)
    : QThread()
{
    m_spInstance = spInstance;
    m_receiverActive = false;
    m_parent = parent;
}


void KLSerialPortListener::setReceiverActive( bool theValue )
{
    if ( theValue )
    {
        if ( !m_receiverActive )
        {
            m_receiverActive = theValue;
            start();
        }
    }
    else
    {
        m_receiverActive = theValue;
    }
}


void KLSerialPortListener::run( )
{
    char buffer[256];
    int size = 0;

    KLSerialPort* sp = static_cast<KLSerialPort*>(m_spInstance);

    while (m_receiverActive)
    {
        // qDebug("sleeping");
        msleep( 10 );
        // qDebug("woke up");
        size = 0;//::read(sp->m_fd, buffer, 255);
        // qDebug("read %d bytes", size);
        if (size > 0)
        {
            // buffer[size] = 0;
            KLCharVector received;
            for (int i=0; i<size; i++)
                received.push_back( buffer[i] );
            dataReceived( received );
        }
    }
}

void KLSerialPortListener::dataReceived( const KLCharVector & data )
{
    for ( unsigned int i=0; i < data.size(); i++ )
        m_outData.push_back( data[i] );
    qApp->postEvent( (QObject*) m_parent, new QCustomEvent(QEvent::User+55) );
    // qApp->postEvent( (QObject*) m_parent, new QPaintEvent( QRect(), false ) );
}


