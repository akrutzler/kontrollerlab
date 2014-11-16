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
#ifndef KLSERIALPORTLISTENER_H
#define KLSERIALPORTLISTENER_H

#include <qobject.h>
#include <qtimer.h>
#include <qthread.h>
#include "klcharvector.h"

// using namespace std;


class KLSerialPort;
class KLSerialTerminalWidget;

/**
This is the class which listens to the serial port.

	@author Martin Strasser <strasser@kontrollerlab.org>
*/
class KLSerialPortListener : public QThread
{
public:
    KLSerialPortListener(KLSerialTerminalWidget *parent = 0, KLSerialPort* spInstance=0, const char *name = 0);
    /**
     * Virtual destructor.
     */
    virtual ~KLSerialPortListener() {};

    virtual void run();
    
    void setReceiverActive(bool theValue);
    bool receiverActive() const { return m_receiverActive; }

    /**
     * Called when serial data has arrived. This will be maximum 255 bytes at a time.
     *
     * @param data a pointer to the data
     * @param size the size of the data
     */
    void dataReceived(const KLCharVector& data);

    KLCharVector fetchOutData()
    {
        KLCharVector retVal = m_outData;
        m_outData.clear();
        return retVal;
    }
	

protected:
    KLCharVector m_outData;
    KLSerialPort* m_spInstance;
    bool m_receiverActive;
    KLSerialTerminalWidget* m_parent;
};

#endif
