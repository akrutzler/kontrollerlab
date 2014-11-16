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
#include "kldebuggerbreakpoint.h"

KLDebuggerBreakpoint::KLDebuggerBreakpoint(const KUrl& url, int line, int realBreakpointLine)
    : m_url( url ), m_line( line ), m_realBreakpointLine( realBreakpointLine )
{
}


KLDebuggerBreakpoint::~KLDebuggerBreakpoint()
{
}

bool KLDebuggerBreakpoint::operator ==( const KLDebuggerBreakpoint & other ) const
{
    // real breakpoints are not part of the comparison!
    return ( other.line() == m_line ) && ( other.url() == m_url );
}


