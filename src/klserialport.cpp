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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstdio>
#include <cstdlib>
#include <errno.h>
#include <signal.h>
#include <strings.h>
#include <unistd.h>

#include "klserialport.h"


KLSerialPort::KLSerialPort()
{
    m_fd = -1;
    // flow control:
    m_rtscts = false;
    m_xonxoff = false;
}


KLSerialPort::~KLSerialPort()
{
}

bool KLSerialPort::open( const QString & file, int baudRate )
{
    close();

    m_fd = ::open( file.toLatin1(), O_RDWR | O_NOCTTY | O_NONBLOCK );

    if (m_fd == -1)
        return false;

    /* save current port settings */
    tcgetattr(m_fd, &m_oldtio);

    // fcntl(m_fd, F_SETFL, FASYNC);

    bzero(&m_newtio, sizeof(m_newtio));
    m_newtio.c_cflag = baudRate | CS8 | CSTOPB | CLOCAL | CREAD;
    if ( m_rtscts )
        m_newtio.c_cflag |= CRTSCTS;
    m_newtio.c_iflag = IGNPAR;
    if ( m_xonxoff )
        m_newtio.c_iflag = IXON | IXOFF;
    m_newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    m_newtio.c_lflag = 0;

    m_newtio.c_cc[VTIME] = 2;  /* inter-character timer */
    m_newtio.c_cc[VMIN] = 255; /* blocking read until x chars received */

    tcflush(m_fd, TCIFLUSH);
    tcsetattr(m_fd, TCSANOW, &m_newtio);

    return true;
}


ssize_t KLSerialPort::write( const std::vector< unsigned char >& data )
{
    unsigned char* data_ = new unsigned char[ data.size()+1 ];
    ssize_t retVal;

    for ( unsigned int i = 0; i < data.size(); i++ )
        data_[i] = data[i];
    retVal = ::write(m_fd, data_, data.size());
    delete[] data_;
    return retVal;
}

void KLSerialPort::close( )
{
    if (m_fd != -1)
    {
        tcsetattr(m_fd, TCSANOW, &m_oldtio);
        ::close( m_fd );
        m_fd = -1;
    }

}



