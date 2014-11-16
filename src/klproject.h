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
#ifndef KLPROJECT_H
#define KLPROJECT_H

#include <QList>
#include "kldocument.h"
#include <kurl.h>
#include "klcpufeatures.h"
#include "klcpufuses.h"
#include <qdom.h>
#include "klsourcecodetoasmrelation.h"
#include "klasminstruction.h"


// The constant keys for the project settings:
#define PRJ_CPU                         "CPU"
#define PRJ_CLOCKSPEED                  "CLOCK"
#define PRJ_EXTERNAL_RAM                "EXTERNAL_RAM"
#define PRJ_HEX_FILE                    "HEX_FILE"
#define PRJ_MAP_FILE                    "MAP_FILE"
#define PRJ_COMPILER_COMMAND            "COMPILER_COMMAND"
#define PRJ_COMPILER_CALL_PROLOGUES     "COMPILER_CALL_PROLOGUES"
#define PRJ_COMPILER_STRICT_PROTOTYPES  "COMPILER_STRICT_PROTOTYPES"
#define PRJ_COMPILER_WALL               "COMPILER_WALL"
#define PRJ_COMPILER_GDEBUG             "COMPILER_GDEBUG"
#define PRJ_COMPILER_OPT_LEVEL          "COMPILER_OPT_LEVEL"
#define PRJ_COMPILER_F_CPU              "COMPILER_F_CPU"
#define PRJ_LINKER_COMMAND              "LINKER_COMMAND"
#define PRJ_OBJCOPY_COMMAND             "OBJCOPY_COMMAND"
#define PRJ_OBJDUMP_COMMAND             "OBJDUMP_COMMAND"
#define PRJ_LINKER_START_TEXT           "LINKER_START_TEXT"
#define PRJ_LINKER_START_DATA           "LINKER_START_DATA"
#define PRJ_LINKER_START_BSS            "LINKER_START_BSS"
#define PRJ_LINKER_START_HEAP           "LINKER_START_HEAP"
#define PRJ_LINKER_END_HEAP             "LINKER_END_HEAP"
#define PRJ_LINKER_FLAGS                "LINKER_FLAGS"
#define PRJ_ASSEMBLER_COMMAND           "ASSEMBLER_COMMAND"

#define PRJ_BUILD_SYSTEM                "BUILD_SYSTEM"
#define PRJ_BUILT_IN_BUILD_SYSTEM       "BUILT_IN_BUILD"
#define PRJ_EXECUTE_MAKE                "EXECUTE_MAKE"

#define PRJ_MAKE_COMMAND                "MAKE_COMMAND"
#define PRJ_MAKE_DEFAULT_TARGET         "MAKE_DEFAULT_TARGET"
#define PRJ_MAKE_CLEAN_TARGET           "MAKE_CLEAN_TARGET"

#define TRUE_STRING                     "TRUE"
#define FALSE_STRING                    "FALSE"

class KIconLoader;
class KontrollerLab;
class KLProgrammerInterface;
class KLProjectManagerWidget;
class KLCompilerInterface;


/**
This is the project class which stores all documents belonging to the project.

	@author Martin Strasser <strasser@kontrollerlab.org>
*/
class KLProject{
public:
    KLProject( KontrollerLab* parent );

    ~KLProject();
    
    friend class KLAVRGCCCompiler;
    friend class KLCompilerInterface;

    void addDocument( KLDocument* doc, bool forceCopyToProjectDir = false );
    void removeDocument( KLDocument* doc );
    QList< KLDocument* > getAllDocsOfType( const KLDocumentType& type );

    // void triggerListRecreation() { recreateListView(); }

    void setName(const QString& theValue) { m_name = theValue; m_unsaved = true; }
    QString name() const { return m_name; }
    
    void saveAllDocs();
    
    void compile();
    void buildAll();
    void buildAndDownloadExternal( const QString& outHexFileName,
                                   const QString& projectBuildDir,
                                   const QStringList& sources );
    void rebuildAll();
    void erase();
    bool upload( );
    bool upload( const KUrl& url );
    bool verify();
    bool download( const KUrl& url );
    bool ignite();
    bool programFuses( QMap<QString, QString> fuses, const QString mmcu = "" );
    bool readFuses( const QString mmcu = "" );

    void backannotateFuses( );
    void backannotateFuses( QMap< QString, QString > fuses );

    void backannotateSTK500( const QString& stdout="" );

    void setProgrammerInterface(KLProgrammerInterface* theValue) { m_programmerInterface = theValue; m_unsaved=true; }
    KLProgrammerInterface* programmerInterface() const { return m_programmerInterface; }
    bool hexFileIsUpToDate();

    void registerProjectManager( KLProjectManagerWidget* prMan );
    void unregisterProjectManager( KLProjectManagerWidget* prMan );

    void update() { recreateListView(); }
    void update( KLDocument* doc ) { recreateListItemFor( doc ); }

    QList< KLDocument* > documents() const { return m_documents;}
    QStringList cpus() const { return m_cpus; }
    KLCPUFeatures cpuFeaturesFor( const QString& cpu );
    void setCpuFeaturesFor( const QString& cpu, const KLCPUFeatures& feat );
    
    QString cpu() const { return m_settings[PRJ_CPU]; }
    void setCpu( const QString& cpuname );
    int cpuIndex() const;
    double clock() const
    {
        bool ok;
        double val = attribute( "", PRJ_CLOCKSPEED ).stripWhiteSpace().toDouble( &ok );
        if ( !ok ) val = 0;
        return val;
    }

    KLCPUFeatures currentCpuFeatures() { return cpuFeaturesFor( cpu() ); }
    
    void setSettings(const QMap< QString, QString >& theValue);
    QMap< QString, QString > settings() const
    { return m_settings; }

    void setProjectBaseURL(const KUrl& theValue) { m_projectBaseURL = theValue; m_unsaved=true; }
    KUrl projectBaseURL() const { return m_projectBaseURL; }

    void setInitialized(bool theValue) { m_initialized = theValue; m_unsaved=true; }
    bool initialized() const { return m_initialized; }
    
    void initialize( const KUrl& url = KUrl("") );

    KontrollerLab* parent() const { return m_parent; }

    void save( const KUrl& url );
    void open( const KUrl& url );
    void close();
    
    void setActiveDocument( KLDocument* doc ) { m_activeDocument = doc; }
    KLDocument* getDocumentNamed( const QString& name );
    KLDocument* getDocumentByURL( const KUrl& url );
    KLDocument* activeDocument()
    { return m_documents.indexOf( m_activeDocument ) >= 0 ? m_activeDocument : 0L; }

    void setProjectFileURL(const KUrl& theValue) { m_projectFileURL = theValue; m_unsaved = true; }
    KUrl projectFileURL() const { return m_projectFileURL; }
    
    int getHexFileSize();
    void updateHexFileSize();
    void checkForModifiedFiles();

    // QList< KLCPUFuses > cpuFuses() const { return m_cpuFuses; }
    KLCPUFuses getFusesFor( const QString& name );

    bool unsaved();
    void setUnsaved() { m_unsaved = true; }
    
    void setProgrammerBusy( bool val );
    bool programmerBusy() const { return m_programmerBusy; }
    
    KLDocument* getDocumentForView( KTextEditor::View * view ) const;

    void readDebugHexAndInfo();
    void setASMRelations( QList< KLSourceCodeToASMRelation > rlist );
    void setASMInstructions( QList< KLASMInstruction > ilist,
                             const QString& asmLines );
    
    void markOnlyLineInDocument( const KUrl& name, int line );
    void markBreakpointInDocument( const KUrl& name, int line, bool setMark = true );

    void setDebuggerSettings(const QMap< QString, QString >& theValue)
    { m_debuggerSettings = theValue; }
    QMap< QString, QString > debuggerSettings() const
    { return m_debuggerSettings; }

    QString getHierarchyName(const KUrl & filename) const;
protected:
    void readFromDOMElement( QDomDocument & document, QDomElement & parent );
    void createDOMElement( QDomDocument & document, QDomElement & parent );
    void fromQMapToDOM( QDomDocument & document, QDomElement & parent, QMap<QString, QString> map );
    QMap<QString, QString> fromDOMToQMap( QDomDocument & document, QDomElement & parent );
    QStringList fromDOMToQStringList( QDomDocument & document, QDomElement & parent,
                                      const QString& tag, const QString & attribName,
                                      const QString & attribDefValue,
                                      QStringList &attribs );
    
    bool m_currentlyOpeningProject;
    KLCompilerInterface* m_compilerInterface;
    // QString attribute( const QString& attr, const QString& confKey ) const;
    QString attribute( const QString& attr, const QString& confKey, bool ifTrueAttribOnly = false ) const;
    QString attributeLinkerSection( const QString & attr, const QString & confKey ) const;
    QString conf( const QString& confKey, const QString& defval ) const;
    QList< KLProjectManagerWidget* > m_projectManagerWidgets;
    QString m_name;
    void recreateListView();
    void recreateListItemFor( KLDocument* doc );
    QList< KLDocument* > m_documents;
    KLDocument* m_activeDocument;

    /**
     * This routine returns the name of the object file for one source file.
     * @param file The source file's URL
     * @return The object file name. For example, it returns project.o. No path is returned!
     * If You need the path, use getObjectFilePath() instead.}
     */
    QString getObjectFileNameFor( const KUrl& file );
    /**
     * This routine returns the name of the object file path for one source file.
     * @param file The source file's URL
     * @return The object file path. For example, this may return /home/user/project/test.o
     */
    QString getObjectFilePathFor( const KUrl& file );
    QString getOUTFileName();
    QString getHEXFileName() { return conf( PRJ_HEX_FILE, "project.hex" ); }
    QString getMAPFileName() { return conf( PRJ_MAP_FILE, "project.map" ); }
    KUrl getHEXURL() { KUrl retVal = m_projectBaseURL; retVal.addPath(getHEXFileName()); return retVal; }
    KUrl getMAPURL() { KUrl retVal = m_projectBaseURL; retVal.addPath(getMAPFileName()); return retVal; }
    
    KontrollerLab* m_parent;
    KLProgrammerInterface* m_programmerInterface;
    KUrl m_hexUrl;
    // All files have to be below this LOCAL URL:
    KUrl m_projectBaseURL, m_projectFileURL;
    bool m_initialized;
    // All cpu features:
    KLCPUFeaturesList m_cpuFeatures;
    KLCPUFusesList m_cpuFuses;
    // All CPU names:
    QStringList m_cpus;
    // The settings for this project, except for the
    // programmer configuration, which is stored in m_programmerInterface.
    QMap<QString, QString> m_settings, m_debuggerSettings;
    bool m_unsaved;
    bool m_programmerBusy;
    KUrl m_currentHighlightURL;
    int m_currentHighlightLine;
};

#endif
