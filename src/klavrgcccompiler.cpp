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
#include "klavrgcccompiler.h"
#include "kldebugger.h"
#include "kldocument.h"
#include "klproject.h"
#include "klprocess.h"
#include "klprocessmanager.h"
#include "kontrollerlab.h"
#include <klocale.h>
#include "klsourcecodetoasmrelation.h"
#include <qregexp.h>
#include "kldocumentview.h"


KLAVRGCCCompiler::KLAVRGCCCompiler( KLProject* project, KLProcessManager* processManager )
 : KLCompilerInterface( project, processManager )
{
    m_nextCmdIsReadHex = false;
    m_nextCmdIsDebugInfo = false;
}


KLAVRGCCCompiler::~KLAVRGCCCompiler()
{
}

void KLAVRGCCCompiler::compile( KLDocument* what )
{
    if ( !what )
        return;
    if ( what->type() != KLDocType_Source )
        return;
    // m_cmdSchedule.append( "cd " + m_project->projectBaseURL().path() );
    m_processManager->setCWD( m_project->projectBaseURL().path() );
    // First, save all of them:
    if ( what->kateDoc()->isModified() )
        what->save();
    QFileInfo srcFI( what->url().path() );
    QFileInfo objFI( m_project->getObjectFilePathFor( what->url() ) );
    // Do we need to compile the file?
    int compilerJobsStarted = 0;
    if ( !( objFI.exists() && ( objFI.lastModified() >= srcFI.lastModified() ) ) )
    {
        m_cmdSchedule.append( getCompileCommandFor( what->url() ) );
        compilerJobsStarted++;
    }
    // m_cmdSchedule.append( getLinkerCommand().ascii() );
    // m_cmdSchedule.append( getObjCopyCommand() );
    if ( compilerJobsStarted > 0 )
    {
        QString firstCmd = m_cmdSchedule.front();
        m_cmdSchedule.pop_front();
        m_processManager->addProcess( firstCmd, (KLProcessHandler*) this );
    }
    else
    {
        m_processManager->slotMessage( 0,
            i18n("%1 is up to date. No compiler job started.")
                    .arg( what->hierarchyName()) );
    }
}


void KLAVRGCCCompiler::buildAll( bool forceRebuild )
{
    // m_cmdSchedule.append( "cd " + m_project->projectBaseURL().path() );
    m_processManager->setCWD( m_project->projectBaseURL().path() );
    // First, save all of them:
    QList< KLDocument* > list = m_project->getAllDocsOfType( KLDocType_Source );
    
    int startedCompilerJobs = 0;
    bool useBuiltInBuildSystem = m_project->conf( PRJ_BUILD_SYSTEM,
            PRJ_BUILT_IN_BUILD_SYSTEM ) == PRJ_BUILT_IN_BUILD_SYSTEM;

    foreach(KLDocument *it, list)
    {
        // qDebug( "%s is modified: %d", it->name().ascii(), it->kateDoc()->isModified() );
        if ( it->kateDoc()->isModified() )
        {
            it->save();
            it->kateDoc()->setModified( false );
        }
        QFileInfo srcFI( it->url().path() );
        QFileInfo objFI( m_project->getObjectFilePathFor( it->url() ) );
        // Do we need to compile the file?
        if ( useBuiltInBuildSystem )
        {
            // qDebug("objFI.exists() == %d", objFI.exists() );
            // qDebug("objFI.lastModified() == %d", objFI.lastModified().toTime_t() );
            // qDebug("srcFI.lastModified() == %d", srcFI.lastModified().toTime_t() );
            if ( ( !( objFI.exists() && ( objFI.lastModified() >= srcFI.lastModified() ) ) ) || forceRebuild )
            {
                m_processManager->addProcess( getCompileCommandFor( it->url() ), (KLProcessHandler*) this );
                startedCompilerJobs++;
            }
        }
    }
    if ( useBuiltInBuildSystem )
    {
        // if ( list.count() > 0 )
        if ( startedCompilerJobs > 0 )
        {
            m_processManager->addProcess( getLinkerCommand(), (KLProcessHandler*) this );
            m_processManager->addProcess( getObjCopyCommand(), (KLProcessHandler*) this );
        }
        else
        {
            m_processManager->slotMessage(0, i18n("Project is up to date. Nothing to be done."));
        }
    }
    else
    {
        if ( forceRebuild )
        {
            m_processManager->addProcess( m_project->conf( PRJ_MAKE_COMMAND, "make" ) + " " +
                                          m_project->conf( PRJ_MAKE_CLEAN_TARGET, "clean" ),
                                          (KLProcessHandler*) this );
            m_processManager->addProcess( m_project->conf( PRJ_MAKE_COMMAND, "make" ) + " " +
                                          m_project->conf( PRJ_MAKE_DEFAULT_TARGET, "all" ),
                                          (KLProcessHandler*) this );
        }
        else
        {
            m_processManager->addProcess( m_project->conf( PRJ_MAKE_COMMAND, "make" ) + " " +
                                          m_project->conf( PRJ_MAKE_DEFAULT_TARGET, "all" ),
                                          (KLProcessHandler*) this );
        }
    }
}


QString KLAVRGCCCompiler::getCompileCommandFor( const KUrl & file )
{
    QString retVal;

    // There is a difference if it is an assembler (.s) or a C file:
    if ( file.fileName().toLower().endsWith( ".s" ) )
    {
        retVal = m_project->m_settings[ PRJ_ASSEMBLER_COMMAND ];
        
        retVal += " -mmcu=" + m_project->currentCpuFeatures().mmcuFlag();
        retVal += m_project->attribute( "-g", PRJ_COMPILER_GDEBUG, true );
        retVal += m_project->attribute( "-O", PRJ_COMPILER_OPT_LEVEL );
        
        // Append the clock speed if desired:
        if ( m_project->attribute( "", PRJ_CLOCKSPEED ).trimmed().length() > 0 )
        {
            bool ok;
            double val = m_project->attribute( "", PRJ_CLOCKSPEED ).trimmed().toDouble( &ok );
            if (ok && (m_project->conf( PRJ_COMPILER_F_CPU, FALSE_STRING ) == TRUE_STRING) )
                retVal += QString(" -DF_CPU=%1UL").arg(val, 0, 'f', 0);
        }
        
        retVal += " -c " + getHierarchyName( file ) + " -o " + m_project->getObjectFileNameFor( file );
    }
    else
    {
        retVal = m_project->m_settings[ PRJ_COMPILER_COMMAND ];
        
        retVal += " -mmcu=" + m_project->currentCpuFeatures().mmcuFlag();
        retVal += m_project->attribute( "-mcall-prologues", PRJ_COMPILER_CALL_PROLOGUES, true );
        retVal += m_project->attribute( "-Wstrict-prototypes", PRJ_COMPILER_STRICT_PROTOTYPES, true );
        retVal += m_project->attribute( "-Wall", PRJ_COMPILER_WALL, true );
        retVal += m_project->attribute( "-g", PRJ_COMPILER_GDEBUG, true );
        retVal += m_project->attribute( "-O", PRJ_COMPILER_OPT_LEVEL );
        if ( m_project->attribute( "", PRJ_CLOCKSPEED ).trimmed().length() > 0 )
        {
            bool ok;
            double val = m_project->attribute( "", PRJ_CLOCKSPEED ).trimmed().toDouble( &ok );
            if (ok && (m_project->conf( PRJ_COMPILER_F_CPU, FALSE_STRING ) == TRUE_STRING) )
                retVal += QString(" -DF_CPU=%1UL").arg(val, 0, 'f', 0);
        }
        
        retVal += " -c " + getHierarchyName( file ) + " -o " + m_project->getObjectFileNameFor( file );
    }
    return retVal;
}


/**
 * The linker string generator.
 * @param listOfObjectFiles If necessary, a string containing a space separated
 * list of object files to be linked can be specified.
 * @return A string containing the linker command.
 */
QString KLAVRGCCCompiler::getLinkerCommand( const QString& listOfObjectFiles )
{
    QString retVal = m_project->m_settings[ PRJ_LINKER_COMMAND ];

    // Check if there were object files defined outside the routine:

    if ( listOfObjectFiles.isEmpty() || listOfObjectFiles.isNull() )
    {
        QList< KLDocument* > list = m_project->getAllDocsOfType( KLDocType_Source );
        QListIterator<KLDocument *> i(list);
        foreach (KLDocument *it, list )
        {
            retVal += " " + m_project->getObjectFileNameFor( it->url() );
        }
    }
    else
    {
        retVal += " " + listOfObjectFiles;
    }

    retVal += " -mmcu=" + m_project->currentCpuFeatures().mmcuFlag();
    retVal += m_project->attribute( "-g", PRJ_COMPILER_GDEBUG, true );
    retVal += m_project->attributeLinkerSection( "-Wl,-Ttext=0x",
                         PRJ_LINKER_START_TEXT );
    retVal += m_project->attributeLinkerSection( "-Wl,-Tdata=0x",
                         PRJ_LINKER_START_DATA );
    retVal += m_project->attributeLinkerSection( "-Wl,-Tbss=0x",
                         PRJ_LINKER_START_BSS );
    retVal += m_project->attributeLinkerSection( "-Wl,--defsym=__heap_start=0x",
                         PRJ_LINKER_START_HEAP );
    retVal += m_project->attributeLinkerSection( "-Wl,--defsym=__heap_end=0x",
                         PRJ_LINKER_END_HEAP );

    retVal += " -o " + m_project->getOUTFileName();
    retVal += " -Wl,-Map," + m_project->getMAPFileName();

    QString buf = m_project->conf( PRJ_LINKER_FLAGS, "" ).trimmed();
    // buf.replace( "#", "," );
    // I'm not sure about that:
    buf.replace( "#", " " );
    if ((!buf.isEmpty()) && (!buf.isNull()))
        retVal += " " + buf;

    return retVal;
}


QString KLAVRGCCCompiler::getObjCopyCommand( )
{
    QString retVal = m_project->m_settings[ PRJ_OBJCOPY_COMMAND ];
    retVal += " -R .eeprom -O ihex " + m_project->getOUTFileName() +
            " " + m_project->getHEXFileName();
    return retVal;
}

void KLAVRGCCCompiler::shellProcessExitedWith( int exitVal, const QString & stdout, const QString & stderr )
{
    if ( m_nextCmdIsDebugInfo )
    {
        if ( exitVal == 0 )
            parseDebugInfos( stdout );
        m_nextCmdIsDebugInfo = false;
    }
    else if ( m_nextCmdIsReadHex )
    {
        if ( exitVal == 0 )
            parseHexCmds( stdout );
        m_nextCmdIsReadHex = false;
    }
    else
    {
        if ( (!stdout.isEmpty()) && (!stdout.isNull()) )
            m_project->parent()->slotMessageBox( exitVal, stdout );
        if ( (!stderr.isEmpty()) && (!stderr.isNull()) )
            m_project->parent()->slotMessageBox( exitVal, stderr );
    }

    if (exitVal == 0)
    {
        if (m_cmdSchedule.count() > 0 )
        {
            QString nextCmd = m_cmdSchedule.front();
            m_cmdSchedule.pop_front();
            if ( nextCmd == getReadHexCommand() )
                m_nextCmdIsReadHex = true;
            if ( nextCmd == getReadDebugCommand() )
                m_nextCmdIsDebugInfo = true;
            m_processManager->setCWD( m_project->projectBaseURL().path() );
            m_processManager->addProcess( nextCmd, (KLProcessHandler*)this );
        }
        else
            m_project->updateHexFileSize();
    }
    else
    {
        m_project->parent()->slotMessageBox( exitVal, i18n("Error(s) occurred: The exit status was %1.").arg( exitVal ) );
        // qDebug("There were problems. Exit was %d.", exitVal);
        // if ( m_cmdSchedule.count() > 0 )
        //     m_cmdSchedule.clear();
        m_nextCmdIsDebugInfo = m_nextCmdIsReadHex = false;
    }
}


void KLAVRGCCCompiler::readHEXProgram( )
{
    m_cmdSchedule.append( getReadHexCommand() );
    m_processManager->setCWD( m_project->projectBaseURL().path() );
    if ( m_processManager->procsInQueue() == 0 )
    {
        QString firstCmd = m_cmdSchedule.front();
        m_nextCmdIsReadHex = true;
        m_cmdSchedule.pop_front();
        m_processManager->addProcess( firstCmd, (KLProcessHandler*) this );
    }
}


void KLAVRGCCCompiler::readDebugInfoFromProgram( )
{
    m_cmdSchedule.append( getReadDebugCommand() );
    m_processManager->setCWD( m_project->projectBaseURL().path() );
    if ( m_processManager->procsInQueue() == 0 )
    {
        QString firstCmd = m_cmdSchedule.front();
        m_nextCmdIsDebugInfo = true;
        m_cmdSchedule.pop_front();
        m_processManager->addProcess( firstCmd, (KLProcessHandler*) this );
    }
}


QString KLAVRGCCCompiler::getObjDumpCommand( )
{
    QString retVal = m_project->debuggerSettings()[ DEBUGGER_OBJDUMP_COMMAND ];

    if ( retVal.isEmpty() || retVal.isNull() )
        retVal = "avr-objdump";
    return retVal;
}


QString KLAVRGCCCompiler::getReadHexCommand( )
{
    QString cmd = getObjDumpCommand();
    // Disassemble:
    cmd += " -d " + m_project->getOUTFileName();
    return cmd;
}


QString KLAVRGCCCompiler::getReadDebugCommand( )
{
    QString cmd = getObjDumpCommand();
    // Read debug symbols:
    cmd += " -g " + m_project->getOUTFileName();
    return cmd;
}

void KLAVRGCCCompiler::parseDebugInfos( const QString & str )
{
    QRegExp re( "/\\*\\sfile\\s(.*)\\sline\\s([0-9]*)\\saddr\\s0x([0-9a-fA-F]*)\\s*\\*/" );
    QStringList allLines = str.split("\n");
    QList< KLSourceCodeToASMRelation > relations;
    for (unsigned int i=0; i < allLines.count(); i++)
    {
        if ( re.indexIn( allLines[ i ] ) >= 0 )
        {
            bool ok1, ok2;
            // qDebug( "%s in line %s represents asm line %s", re.cap( 1 ).ascii(),
            //         re.cap( 2 ).ascii(), re.cap( 3 ).ascii() );
            QString fn = re.cap( 1 );
            QFileInfo myFile( fn );
            if ( !myFile.exists() )
            {
                KUrl myURL = m_project->projectBaseURL();
                myURL.addPath( fn );
                fn = myURL.path();
            }
            myFile = QFileInfo( fn );
            if ( myFile.exists() )
            {
                relations.append( KLSourceCodeToASMRelation( fn, re.cap( 2 ).toInt( &ok1 ),
                                  re.cap( 3 ).toInt( &ok2, 16 ) ) );
                if ( !(ok1 && ok2 ) )
                    qWarning( "There was a problem in the conversion of the line:\n%s\nin %s:%d",
                            allLines[i].toLatin1(), __FILE__, __LINE__ );
            }
            // else
            //     qWarning("Could not verify %s", fn.ascii() );
        }
    }
    m_project->setASMRelations( relations );
}

void KLAVRGCCCompiler::parseHexCmds( const QString & str )
{
    QRegExp re( "\\s*([0-9a-fA-F]*):\\s*([^\\t]*)" );
    QStringList allLines = str.split("\n");
    QList< KLASMInstruction > instr;
    for (unsigned int i=0; i < allLines.count(); i++)
    {
        if ( re.indexIn( allLines[ i ] ) >= 0 )
        {
            // qDebug( "inst %s is %s", re.cap( 1 ).ascii(), re.cap( 2 ).ascii() );
            bool ok1, ok2 = true;
            unsigned int adr = re.cap( 1 ).toUInt( &ok1, 16 );
            QString cmdStr = re.cap( 2 ).replace( " ", "" );
            // unsigned int cmd = cmdStr.toUInt( &ok2, 16 );
            if ( !(ok1 && ok2) )
            {
                // qWarning("Omitted instruction (%s:%d): %s", __FILE__, __LINE__,
                //          allLines[ i ].ascii() );
                continue;
            }
            QString hex = re.cap( 2 ).trimmed();
            QStringList hexStrings = hex.split(" ",QString::SkipEmptyParts);
            QList< unsigned int > hexAsInt;
            for ( unsigned int j=0; j<hexStrings.count(); j++ )
            {
                hexAsInt.append( hexStrings[ j ].toInt( &ok1, 16 ) );
            }
            instr.append( KLASMInstruction( adr, hexAsInt, i ) );
        }
    }
    m_project->setASMInstructions( instr, str );
}

void KLAVRGCCCompiler::buildExternal( const QString &,
                                      const QString & projectBuildDir,
                                      const QStringList & sources_ )
{
    m_processManager->setCWD( KUrl( projectBuildDir ) );
    QStringList sources = sources_;
    QString objects = "";
    for (QStringList::iterator it = sources.begin();
         it != sources.end(); ++it)
    {
        objects += m_project->getObjectFileNameFor( KUrl( projectBuildDir, (*it) ) ) + " ";
        m_processManager->addProcess( getCompileCommandFor( KUrl(projectBuildDir,(*it)) ), (KLProcessHandler*) this );
    }
    m_processManager->addProcess( getLinkerCommand( objects ),
                                  (KLProcessHandler*) this );
    m_processManager->addProcess( getObjCopyCommand(), (KLProcessHandler*) this );
}

