/***************************************************************************
 *   Copyright (C) 2006 by Martin Strasser                                 *
 *   strasser  a t  cadmaniac  d o t  org                                  *
 *   Special thanks to Mario Boikov                                        *
 *   squeeze  a t  cadmaniac  d o t  org                                   *
 *   je  d o t  sid  a t  gmx  d o t  de                                   *
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
#ifndef KLAVRGCCCOMPILER_H
#define KLAVRGCCCOMPILER_H

#include "klcompilerinterface.h"
#include <qstringlist.h>
#include <QFileInfo>
#include <QDateTime>

/**
This is the class which knows how to trigger the compilation of a c file with the avr-gcc.

	@author Martin Strasser <strasser@kontrollerlab.org>
*/
class KLAVRGCCCompiler : public KLCompilerInterface
{
public:
    KLAVRGCCCompiler( KLProject* project, KLProcessManager* processManager );

    ~KLAVRGCCCompiler();
    
    virtual void compile( KLDocument* what );
    virtual void buildAll( bool forceRebuild = false );
    virtual void buildExternal( const QString& outHexFileName,
                                const QString& projectBuildDir,
                                const QStringList& sources );
    virtual void shellProcessExitedWith( int exitVal,
                                         const QString& stdout,
                                         const QString& stderr );
    virtual void readHEXProgram();
    virtual void readDebugInfoFromProgram();
    
protected:
    QString getCompileCommandFor( const KUrl& file );
    QString getLinkerCommand( const QString& listOfObjectFiles = "" );
    QString getObjCopyCommand();
    QString getObjDumpCommand();
    QString getReadHexCommand();
    QString getReadDebugCommand();
    QStringList m_cmdSchedule;
    
    void parseDebugInfos( const QString& str );
    void parseHexCmds( const QString& str );
    
    bool m_nextCmdIsReadHex, m_nextCmdIsDebugInfo;
};

#endif
