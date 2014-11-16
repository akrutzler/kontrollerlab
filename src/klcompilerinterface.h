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
#ifndef KLCOMPILERINTERFACE_H
#define KLCOMPILERINTERFACE_H

#include "klprocesshandler.h"
#include <kurl.h>

class KLProject;
class KLDocument;

/**
This class implements an interface to the compiler. For example, for the AVRs there is a child class, the KLAVRGCCCompiler class.
This class is a child class of KLProcessHandler, which does all the system
stuff for executing processes.

	@author Martin Strasser <strasser@kontrollerlab.org>
*/
class KLCompilerInterface : public KLProcessHandler
{
public:
    KLCompilerInterface( KLProject* project, KLProcessManager* processManager );

    /**
     * Schedules a compilation process.
     * @param what The document to be compiled.
     */
    virtual void compile( KLDocument* what ) = 0;
    /**
     * Builds the whole project, given on construction of the class instance.
     * @param forceRebuild If true, all files are recompiled,
     * even if there are valid object files for them.
     */
    virtual void buildAll( bool forceRebuild = false ) = 0;
    /**
     * For future use. Currently the same as buildAll( true );
     */
    virtual void rebuildAll() { buildAll( true ); }
    /**
     * This is used currently to start "make" when using
     * external Makefiles.
     * @param outHexFileName The name of the created hex file.
     * @param projectBuildDir The project build directory.
     * @param sources The list of sources.
     */
    virtual void buildExternal( const QString& outHexFileName,
                                const QString& projectBuildDir,
                                const QStringList& sources ) = 0;
    /**
     * Reads the assembler code from obj-dump
     */
    virtual void readHEXProgram() = 0;
    /**
     * Reads the debug info from obj-dump
     */
    virtual void readDebugInfoFromProgram() = 0;

    ~KLCompilerInterface();

protected:
    /**
     * Returns the hierarchical name of the file.
     * For example, if there is a file world.c in a subdirectory
     * sub in the project's root directory, then this
     * function returns sub/main.c.
     * @param filename The URL of the file
     * @return The hierarchical name of the file
     */
    QString getHierarchyName( const KUrl& filename ) const;
    KLProject* m_project;
};

#endif
