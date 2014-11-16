/***************************************************************************
 *   Copyright (C) 2006 by Mario Boikov                                    *
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
#ifndef KLSERIALPORT_H
#define KLSERIALPORT_H

#include <termios.h>
#include <qstring.h>
#include <vector>


class KLSerialPortListener;

/**
This is the serial port class for the serial terminal.

	@author Mario Boikov <mario  d o t  boikov  a t  beblue  d o t  org>
*/
class KLSerialPort{
public:
    KLSerialPort();

    /**
     * Opens the port given by the file argument at the specified baud rate.
     *
     * @param file the device file (/dev/ttyS0)
     * @param baudRate the baud rate mask as specified in termios.h (B38400)
     */
    bool open(const QString& file, int baudRate);


    /**
     * Write to the serial port.
     *
     * @param data the data to be written
     * @return the number of bytes written or the error code from the system call write()
     */
    ssize_t write( const std::vector< unsigned char >& data );

    /**
     * Closes the port. The port can be re-used by calling the open() method.
     */
    void close();

    /**
     * Virtual destructor. Calles the close() method and free resources used by this port.
     */
    virtual ~KLSerialPort();

    /**
     * Check if the port is open.
     * 
     * @return true if the port is open
     */
    bool isOpen() const { return (m_fd != -1); }

    void setRtscts(bool theValue)
    { m_rtscts = theValue; }
    bool rtscts() const { return m_rtscts; }
    void setXonxoff(bool theValue)
    { m_xonxoff = theValue; }
    bool xonxoff() const { return m_xonxoff; }

private:
    struct termios m_oldtio;
    struct termios m_newtio;
    int m_fd;
    bool m_xonxoff, m_rtscts;
    friend class KLSerialPortListener;
};

#endif
