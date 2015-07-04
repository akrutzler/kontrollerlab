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


#ifndef _KONTROLLERLAB_H_
#define _KONTROLLERLAB_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kmainwindow.h>
#include <kstandardaction.h>
#include <kparts/partmanager.h>
#include <ktexteditor/view.h>
#include "klprocessmanager.h"
#include <kate/kate.h>
#include <kparts/mainwindow.h>
#include <ksharedconfig.h>
#include <kmenubar.h>

#include <QAction>
#include <ktoolbar.h>

#include <QMdiArea>

class KLEditorWidget;
class KXMLGUIClient;
class KLProcess;
class KLProgrammerConfigWidget;
class KLProgrammerInterface;
class QListWidget;
class QListWidgetItem;
class KMenu;
class KLDocument;
class KLDocumentView;
class KLProject;
class KLDebuggerConfigWidget;

class KRadioAction;


/**
 * @short Application Main Window
 * @author Martin Strasser <strasser@kontrollerlab.org>
 * @version 0.1
 */
class KontrollerLab : public KParts::MainWindow
{
    Q_OBJECT
public:
    /**
     * Default Constructor
     */
    KontrollerLab( bool doNotOpenProjectFromSession = false );

    /**
     * Default Destructor
     */
    virtual ~KontrollerLab();
    QMdiArea * m_mdiArea;

    KXMLGUIClient* kateGuiClientAdded() const { return m_kateGuiClientAdded; }
    KParts::PartManager* partManager() const { return m_partManager; }
    
    KLProgrammerInterface* getProgrammer( const QString& name );
    
    void setOldKTextEditor(KTextEditor::View* theValue) { m_oldKTextEditor = theValue; }
    KTextEditor::View* oldKTextEditor() const { return m_oldKTextEditor; }

    void beAwareOfChangesOf( KLDocument* doc );
    void newDocumentOpened( KTextEditor::Document* doc );
    
    void setProgrammerBusy( bool val );
    KLProject* project() { return m_project; }
private:
    inline void createDocks();
    inline void createActions();
protected:
    virtual void saveProperties(KSharedConfig::Ptr config );
    virtual void readProperties(KSharedConfig::Ptr config );
    virtual void closeEvent( QCloseEvent* e );
    virtual void resizeEvent( QResizeEvent* e );
public slots:
    void keyPressedOnDocument();
    // File ACTION SLOTS:
    void slotNewFile();
    void slotSaveFile();
    void slotSaveFileAs();
    void slotOpenFile();
    void slotCloseFile();
    void slotNewPart(KParts::Part *newPart, bool setActiv);

    void setKateGuiClientAdded(KXMLGUIClient* theValue)
    { m_kateGuiClientAdded = theValue; }
    /** connected to the part manager, activates a new part */
    void slotActivePartChanged(KParts::Part * );

    // void slotPartWillGetDeleted(KParts::Part * );
    
    // Project new, open, save:
    void slotProjectNew();
    void slotProjectOpen();
    void slotProjectSave();
    void slotProjectSaveAs();
    void slotProjectClose();
    // Project compilation slots:
    void slotCompileAssemble();
    void slotBuild();
    void slotRebuildAll();
    // Download interface
    void slotErase();
    void slotUpload();
    void slotUploadHex();
    void slotVerify();
    void slotDownload();
    void slotIgnite();
    void slotStopProgrammer();
    void slotFuses();
    // Config:
    void slotConfigProgrammer();
    void slotConfigProject();
    void slotConfToolbar();
    void configureShortcuts();
    void slotNewToolbarConfig();
    // Wizards:
    void slotSevenSegmentsWizard();
    void slotDotMatrixWizard();
    void slotDotMatrixCharacterWizard();
    // Debug:
    void slotDirectMemoryDebug();
    void slotInCircuitDebugger();
    void slotPCOnlyDebug();
    void slotDebugStart();
    void slotDebugStop();
    void slotDebugPause();
    void slotDebugRunToCursor();
    void slotDebugStepOver();
    void slotDebugStepInto();
    void slotDebugStepOut();
    void slotDebugRestart();
    void slotDebugConfigureICD();
    void slotDebugToggleBreakpoint();
    // Message box:
    void rightButtonClickedOnMsgBox(const QPoint& );
    void clearMessages();
    void slotMessageBox( int exitVal, const QString& msg );
    void slotMessageBoxDblClicked( QListWidgetItem * item );
    
    void slotNewViewForDocument();
    void slotHideShowMessageBox();
    void slotHideShowProjectManager();
    void slotHideShowSerialTerminal();
    void slotHideShowMemoryView();
    
    void docIsChanged(KTextEditor::Document *);
    void slotDocumentUrlChanged( KTextEditor::Document *document );
    
    //void slotMdiModeHasBeenChangedTo(KMdi::MdiMode mode);
    
    //void childWindowCloseRequest( KMdiChildView* pWnd );
    //void slotChildClosed( KMdiChildView* );
public:
    void doNotOpenProjectFromSession() { m_doNotOpenProjectFromSession=true; }
    void setProcManager(KLProcessManager* theValue) { m_procManager = theValue; }
    KLProcessManager* procManager() const { return m_procManager; }
    class KLEditorWidget *m_editorWidget;

    void emitActivePartChanged( KParts::Part* part, KLDocumentView* viewToBeOpened=0L );
    void setProgrammerConfig(const QMap< QString, QString >& theValue);
    QMap< QString, QString > programmerConfig() const
    { return m_programmerConfig; }

    void backannotateFuses( QMap< QString, QString > fuses );
    void backannotateSTK500( const QString& stdout );
    void openProject( const KUrl& url = KUrl("") );
    void notifyDebuggerReady();
    void notifyDebuggerStopped();
    void notifyDebuggerPaused();
    void setAllMemoryViewValuesToZero();
    void activateDebuggerActions( bool activate );

    class KLDebugger* debugger() const
    { return m_debugger; }

    class KLSerialTerminalWidget* serialTerminalWidget() const
    { return m_serialTerminalWidget; }

    class KLMemoryViewWidget* memoryViewWidget() const
    { return m_memoryViewWidget; }

    QAction* debugToggleBreakpoint() const
    { return m_debugToggleBreakpoint; }

signals:
    void activePartChanged( KParts::Part* );

protected:
    QAction *m_newFile;
    QAction *m_openFile;
    QAction *m_saveFile;
    QAction *m_closeFile;
    QAction *m_saveFileAs;
    QAction *m_closeProgram;

    QAction *m_newProject;
    QAction *m_openProject;
    QAction *m_saveProject;
    QAction *m_saveProjectAs;
    QAction *m_closeProject;
    QAction *m_compileAssemble;
    QAction *m_build;
    QAction *m_rebuildAll;
    QAction *m_erase;
    QAction *m_upload;
    QAction *m_uploadHex;
    QAction *m_verify;
    QAction *m_download;
    QAction *m_ignite;
    QAction *m_stopKillingProc;
    QAction *m_fuses;
    QAction *m_configProgrammer;
    QAction *m_configProject;
    
    QAction *m_sevenSegmentsWizardAction;
    QAction *m_dotMatrixWizardAction;
    QAction *m_dotMatrixCharacterWizardAction;
    
    // Debug:

    QAction *m_directMemoryDebug;
    QAction *m_inCircuitDebugger;
    QAction *m_PCOnlyDebug;
    QAction *m_debugStart;
    QAction *m_debugStop;
    QAction *m_debugPause;
    QAction *m_debugRunToCursor;
    QAction *m_debugStepOver;
    QAction *m_debugStepInto;
    QAction *m_debugStepOut;
    QAction *m_debugRestart;
    QAction *m_debugConfigureICD;
    QAction *m_debugToggleBreakpoint;

    QAction *m_newViewForDocument;

    KToolBar *fileToolBar;
    KToolBar *editToolBar;


    QAction *m_hideShowMessageBox, *m_hideShowProjectManager;
    QAction *m_hideShowSerialTerminal, *m_hideShowMemoryView;
    KMenu *m_msgBoxPopup;


    class KLProjectManagerWidget* m_projectManager;
    class KLProjectConfigWidget* m_projectConfigWidget;
    class KLMemoryViewWidget* m_memoryViewWidget;
    class KLProgramFusesWidget* m_fuseConfigWidget;
    class KLSerialTerminalWidget* m_serialTerminalWidget;
    class KLSevenSegmentWidget* m_sevenSegmentWizardWidget;
    class KLDotMatrixWidget* m_dotMatrixWizardWidget;
    class KLDotMatrixCharacterWizard* m_dotMatrixCharacterWizardWidget;
    class KLProject* m_project;
    class KLDebugger* m_debugger;
    QListWidget* m_msgBox;
    QDockWidget* m_tvaMsg, *m_tvaProjectManager, *m_tvaSerialTerminal, *m_tvaMemoryView;
    KParts::PartManager* m_partManager;
    KXMLGUIClient* m_kateGuiClientAdded;
    KTextEditor::View * m_oldKTextEditor;
    KLProcessManager* m_procManager;
    KLProgrammerConfigWidget* m_configProgrammerWidget;
    QMap< QString, KLProgrammerInterface* > m_programmerInterface;
    QMap< QString, QString > m_programmerConfig;
    QString m_lastUploadHexURL, m_lastDownloadHexURL;

    void setCurrentProgInterface();
    QString config( const QString & key, const QString & defVal );
    KLDocumentView* m_viewToBeOpened;
    // If this flag is set, the project from the session manager will
    // not be read at startup:
    bool m_doNotOpenProjectFromSession;
    KLDebuggerConfigWidget* m_debuggerConfigWidget;
};


/**
 * @brief from QT3SUPPORT
 * @param cb
 * @param text
 */

#include <QComboBox>

void setComboBoxText(QComboBox * cb, const QString &text);

#endif // _KONTROLLERLAB_H_
