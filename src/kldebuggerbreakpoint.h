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
#ifndef KLDEBUGGERBREAKPOINT_H
#define KLDEBUGGERBREAKPOINT_H

#include <kurl.h>

/**
This class stores the URL and the line number of a breakpoint.

	@author Martin Strasser <strasser@kontrollerlab.org>
*/
class KLDebuggerBreakpoint{
public:
    KLDebuggerBreakpoint()
    {
        m_line = -1;
        m_realBreakpointLine = -1;
    }

    KLDebuggerBreakpoint( const KUrl& url, int line, int realBreakpointLine = -1 );

    ~KLDebuggerBreakpoint();

    void setUrl(const KUrl& theValue) { m_url = theValue; }
    KUrl url() const { return m_url; }
    void setLine(const int& theValue) { m_line = theValue; }
    int line() const { return m_line; }

    bool operator==( const KLDebuggerBreakpoint& other ) const;

    // A user can set a breakpoint on every line at any time.
    // But some lines have no corresponding ASM command.
    // This was the reason that the debugger did not break at some breakpoints.
    // Now, whenever the debugger is started, the real breakpoint lines
    // are calculated and stored:
    void setRealBreakpointLine(const int& theValue)
    { m_realBreakpointLine = theValue; }
    int realBreakpointLine() const
    { return m_realBreakpointLine; }

protected:
    KUrl m_url;
    int m_line, m_realBreakpointLine;
};

#endif
