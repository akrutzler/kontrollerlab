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


#include "kontrollerlab.h"

#include <qlabel.h>

#include <klocale.h>

#include <QActionGroup>

#include <kparts/partmanager.h>
#include <kparts/part.h>
#include <ktabwidget.h>
#include <kedittoolbar.h>

#include "kldocument.h"
#include "kldocumentview.h"
#include "klproject.h"
#include <kfiledialog.h>
#include "klprojectmanagerwidget.h"

#include <kshortcut.h>
#include "klprogrammerconfigwidget.h"

#include "klprocess.h"
#include "klprogrammeruisp.h"
#include "klprogrammeravrdude.h"
#include "klprojectconfigwidget.h"
#include <kfiledialog.h>
#include <kapplication.h>
#include <kconfig.h>
#include <qregexp.h>
#include <kmessagebox.h>
#include <kiconloader.h>
#include "klprogramfuseswidget.h"
#include "klserialterminalwidget.h"
#include "klfilenewdialog.h"

#include "klsevensegmentwidget.h"
#include "kldotmatrixwidget.h"
#include "kldotmatrixcharacterwizard.h"

#include "klmemoryviewwidget.h"
#include "kldebugger.h"
#include "kldebuggerconfigwidget.h"

#include <QAction>
#include <kstandardaction.h>
#include <kactioncollection.h>

#include <QDockWidget>
#include <kedittoolbar.h>
#include <kxmlguifactory.h>

#include <kaction.h>


KontrollerLab::KontrollerLab( bool doNotOpenProjectFromSession )
{
    setStandardToolBarMenuEnabled( true );
    createStandardStatusBarAction();

    m_lastDownloadHexURL = "";
    m_lastUploadHexURL = "";
    m_serialTerminalWidget = 0L;

    m_mdiArea = new QMdiArea(this);
    m_mdiArea->setViewMode(QMdiArea::TabbedView);
    m_mdiArea->setDocumentMode(true);
    m_mdiArea->setTabsMovable(true);
    m_mdiArea->setTabsClosable(true);

    m_oldKTextEditor = 0L;
    m_viewToBeOpened = 0L;
    m_doNotOpenProjectFromSession = doNotOpenProjectFromSession;

    // Settings menu*/

    //KStandardAction::configureToolbars(this, SLOT(slotConfToolbar()), actionCollection());
    //KStandardAction::keyBindings(this, SLOT(configureShortcuts()), actionCollection());

    createActions();
    setCentralWidget(m_mdiArea);
    
    //setupGUI(KXmlGuiWindow::Default,QDir::currentDirPath() + "/kontrollerlabui.rc");    //absolut just for debug

    //stored in ~/.kde4/share/apps/kontrollerlab/kontrollerlabui.rc or
    //          /usr/local/share/apps/kontrollerlab/kontrollerlabui.rc  depends on version which one is taken(?)
    setupGUI(Default, "kontrollerlabui.rc");

    // The procmanager must be set up before the project is built.
    m_procManager = new KLProcessManager( this, "processManager" );
    
    m_project = new KLProject( this );

    // ADD NEW PROGRAMMERS HERE:
    m_programmerInterface[ UISP ] = new KLProgrammerUISP( m_procManager, m_project );
    m_programmerInterface[ AVRDUDE ] = new KLProgrammerAVRDUDE( m_procManager, m_project );

    m_project->setProgrammerInterface( getProgrammer( UISP ) );

    m_kateGuiClientAdded = 0L;
    m_sevenSegmentWizardWidget = 0L;
    m_dotMatrixWizardWidget = 0L;
    m_dotMatrixCharacterWizardWidget = 0L;

    createDocks();

    m_debugger = new KLDebugger( m_serialTerminalWidget, this, "debugger" );
    m_debugger->setMemoryViewWidget( m_memoryViewWidget );

    m_partManager = new KParts::PartManager( this );
    m_partManager->addManagedTopLevelWidget( this );
    
    m_msgBoxPopup = new KMenu("msgBoxPopup", this);
    KIconLoader kico;
    m_msgBoxPopup->insertItem( kico.loadIcon( "clear_left", KIconLoader::Toolbar ), i18n("Clear messages"), this, SLOT( clearMessages() ) );
    
    connect(m_partManager, SIGNAL(activePartChanged(KParts::Part * )),this, SLOT(slotActivePartChanged(KParts::Part * )));

    connect(m_msgBox, SIGNAL(doubleClicked(Q3ListBoxItem *)), this, SLOT(slotMessageBoxDblClicked(Q3ListBoxItem *)));
    connect(m_msgBox, SIGNAL(rightButtonPressed( Q3ListBoxItem*, const QPoint& )),this,SLOT( rightButtonClickedOnMsgBox( Q3ListBoxItem*, const QPoint& ) ) );

    //hideViewTaskBar();
    m_configProgrammerWidget = new KLProgrammerConfigWidget( this, "programmerConfigWdg" );
    
    m_configProgrammerWidget->updateConfigFromGUI();
    m_programmerConfig = m_configProgrammerWidget->configuration();
    
    m_projectConfigWidget = new KLProjectConfigWidget(m_project, this, "projectConfig");
    // This is neccessary because we need the standard values:
    m_projectConfigWidget->updateSettingsFromGUI();
    m_project->setSettings( m_projectConfigWidget->settings() );
    // This is the fuse bit programmer:
    m_fuseConfigWidget = new KLProgramFusesWidget( this, m_project, "programFuses" );
    
    /*
    KLProcess* proc = new KLProcess("uisp --help");
    connect( proc, SIGNAL(processExited( KLProcess* )),
             this, SLOT(delme( KLProcess* ) ) );
    proc->start();
    */
    readProperties( KSharedConfig::openConfig());

    m_hideShowMessageBox->setChecked( !m_tvaMsg->isHidden() );
    m_hideShowProjectManager->setChecked( !m_tvaProjectManager->isHidden() );
    m_hideShowSerialTerminal->setChecked( !m_tvaSerialTerminal->isHidden() );
    m_hideShowMemoryView->setChecked( !m_tvaMemoryView->isHidden() );
}

KontrollerLab::~KontrollerLab()
{
    saveProperties( KSharedConfig::openConfig() );
    foreach(KLDocument *it, m_project->documents())
        delete it;
}

void KontrollerLab::createActions()
{
    KActionCollection *actionc = actionCollection();
    m_newFile       =   (QAction*) actionc->addAction(KStandardAction::New,"file_new",this,SLOT(slotNewFile()));
    m_openFile      =   (QAction*) actionc->addAction(KStandardAction::Open,"file_open",this,SLOT(slotOpenFile()));
    m_saveFile      =   (QAction*) actionc->addAction(KStandardAction::Save,"file_save",this,SLOT(slotSaveFile()));
    m_saveFileAs    =   (QAction*) actionc->addAction(KStandardAction::SaveAs,"file_save_as",this,SLOT(slotSaveFileAs()));
    m_closeFile     =   (QAction*) actionc->addAction(KStandardAction::Close,"file_close",this,SLOT(slotCloseFile()));

    m_closeProgram  =   (QAction*) actionc->addAction(KStandardAction::Quit,"file_quit",this,SLOT(close()));
    //Project
    m_openProject = (QAction*) actionc->addAction("open_project",this, SLOT( slotProjectOpen() ));
    m_openProject->setText(i18n("Open project"));
    m_openProject->setIcon(KIcon("document-open"));

    m_newProject = (QAction*) actionc->addAction("new_project",this, SLOT( slotProjectNew() ));
    m_newProject->setText(i18n("New project"));
    m_newProject->setIcon(KIcon("document-new"));

    m_saveProject = (QAction*) actionc->addAction("save_project",this, SLOT( slotProjectSave() ));
    m_saveProject->setText(i18n("Save project"));
    m_saveProject->setIcon(KIcon("document-save"));

    m_saveProjectAs = (QAction*) actionc->addAction("save_project_as",this, SLOT( slotProjectSaveAs() ));
    m_saveProjectAs->setText(i18n("Save project as"));
    m_saveProjectAs->setIcon(KIcon("document-save-as"));

    m_closeProject = (QAction*) actionc->addAction("close_project",this, SLOT( slotProjectClose() ));
    m_closeProject->setText(i18n("Close project"));
    m_closeProject->setIcon(KIcon("dialog-close"));
    //Project - Building
    m_compileAssemble = (QAction*) actionc->addAction("compile",this, SLOT( slotCompileAssemble() ));
    m_compileAssemble->setShortcut(QKeySequence("F7"));
    m_compileAssemble->setText(i18n("Compile file"));
    m_compileAssemble->setIcon(KIcon("run-build-file"));

    m_build = (QAction*) actionc->addAction("build",this, SLOT( slotBuild() ));
    m_build->setShortcut(QKeySequence("F9"));
    m_build->setText(i18n("Build project"));
    m_build->setIcon(KIcon("run-build"));

    m_rebuildAll = (QAction*) actionc->addAction("rebuild",this, SLOT( slotRebuildAll() ));
    m_rebuildAll->setText(i18n("Rebuild all"));
    m_rebuildAll->setIcon(KIcon("run-build"));

    m_erase = (QAction*) actionc->addAction("erase",this, SLOT( slotErase() ));
    m_erase->setText(i18n("Erase device"));
    m_erase->setIcon(KIcon("run-build-clean"));

    m_upload = (QAction*) actionc->addAction("upload",this, SLOT( slotUpload() ));
    m_upload->setText(i18n("Upload"));
    m_upload->setIcon(KIcon("arrow-up"));

    m_uploadHex = (QAction*) actionc->addAction("uploadHex",this, SLOT( slotUploadHex() ));
    m_uploadHex->setText(i18n("Upload hex file"));
    m_uploadHex->setIcon(KIcon("arrow-up"));

    m_verify = (QAction*) actionc->addAction("verify",this, SLOT( slotVerify() ));
    m_verify->setText(i18n("Verify"));
    m_verify->setIcon(KIcon("document-edit-verify"));

    m_download = (QAction*) actionc->addAction("download",this, SLOT( slotDownload() ));
    m_download->setText(i18n("Download"));
    m_download->setIcon(KIcon("arrow-down"));

    m_download = (QAction*) actionc->addAction("ignite",this, SLOT( slotIgnite() ));
    m_download->setText(i18n("Ignite"));
    m_download->setIcon(KIcon("arrow-up-double"));

    m_stopKillingProc = (QAction*) actionc->addAction("stop_programming",this, SLOT( slotStopProgrammer() ));
    m_stopKillingProc->setShortcut(QKeySequence("ESC"));
    m_stopKillingProc->setText(i18n("Stop"));
    m_stopKillingProc->setIcon(KIcon("process-stop"));

    m_fuses = (QAction*) actionc->addAction("fuses",this, SLOT( slotFuses() ));
    m_fuses->setText(i18n("Fuses"));
    m_fuses->setIcon(KIcon("configure"));

    m_configProgrammer = (QAction*) actionc->addAction("configProgrammer",this, SLOT( slotConfigProgrammer() ));
    m_configProgrammer->setText(i18n("Configure programmer"));
    m_configProgrammer->setIcon(KIcon("configure"));

    m_configProgrammer = (QAction*) actionc->addAction("configProject",this, SLOT( slotConfigProject() ));
    m_configProgrammer->setText(i18n("Configure project"));
    m_configProgrammer->setIcon(KIcon("run-build-configure"));
    // Wizards:
    m_sevenSegmentsWizardAction = (QAction*) actionc->addAction("seven_segment_wizard",this, SLOT( slotSevenSegmentsWizard() ));
    m_sevenSegmentsWizardAction->setText(i18n("Seven segments wizard"));
    m_sevenSegmentsWizardAction->setIcon(KIcon("tools-wizard"));

    m_dotMatrixWizardAction = (QAction*) actionc->addAction("dot_matrix_wizard",this, SLOT( slotDotMatrixWizard() ));
    m_dotMatrixWizardAction->setText(i18n("Dot matrix display wizard"));
    m_dotMatrixWizardAction->setIcon(KIcon("tools-wizard"));

    m_dotMatrixCharacterWizardAction = (QAction*) actionc->addAction("dot_matrix_character_wizard",this, SLOT( slotDotMatrixCharacterWizard() ));
    m_dotMatrixCharacterWizardAction->setText(i18n("Dot matrix character wizard"));
    m_dotMatrixCharacterWizardAction->setIcon(KIcon("tools-wizard"));

    QActionGroup *debugMode = new QActionGroup(this);
    debugMode->setExclusive(true);

    m_directMemoryDebug = (QAction*) actionc->addAction("direct_memory_debug",this, SLOT( slotDirectMemoryDebug() ));
    m_directMemoryDebug->setText(i18n("Direct memory debugger"));
    //m_directMemoryDebug->setIcon(KIcon("application-cancel"));
    m_directMemoryDebug->setCheckable(true);
    debugMode->addAction(m_directMemoryDebug);

    m_inCircuitDebugger = (QAction*) actionc->addAction("in_circuit_debugger",this, SLOT( slotInCircuitDebugger() ));
    m_inCircuitDebugger->setText(i18n("In circuit debugger"));
    //m_inCircuitDebugger->setIcon(KIcon("application-cancel"));
    m_inCircuitDebugger->setCheckable(true);
    debugMode->addAction(m_inCircuitDebugger);

    m_PCOnlyDebug = (QAction*) actionc->addAction("pc_only_debug",this, SLOT( slotPCOnlyDebug() ));
    m_PCOnlyDebug->setText(i18n("PC only debugging"));
    //m_PCOnlyDebug->setIcon(KIcon("application-cancel"));
    m_PCOnlyDebug->setCheckable(true);
    m_PCOnlyDebug->setChecked(true);
    debugMode->addAction(m_PCOnlyDebug);

    m_debugStart = (QAction*) actionc->addAction("debug_start",this, SLOT( slotDebugStart() ));
    m_debugStart->setText(i18n("Enable"));
    m_debugStart->setIcon(KIcon("debug-run"));

    m_debugStop = (QAction*) actionc->addAction("debug_stop",this, SLOT( slotDebugStop() ));
    m_debugStop->setText(i18n("Stop"));
    m_debugStop->setIcon(KIcon("media-playback-stop"));

    m_debugPause = (QAction*) actionc->addAction("debug_pause",this, SLOT( slotDebugPause() ));
    m_debugPause->setText(i18n("Start"));
    m_debugPause->setIcon(KIcon("media-playback-start"));

    m_debugRunToCursor = (QAction*) actionc->addAction("debug_run_to_cursor",this, SLOT( slotDebugRunToCursor() ));
    m_debugRunToCursor->setText(i18n("Run to cursor"));
    m_debugRunToCursor->setIcon(KIcon("debug-run-to-cursor"));

    m_debugStepOver = (QAction*) actionc->addAction("debug_step_over",this, SLOT( slotDebugStepOver() ));
    m_debugStepOver->setText(i18n("Step over"));
    m_debugStepOver->setIcon(KIcon("debug-step-over"));

    m_debugStepInto = (QAction*) actionc->addAction("debug_step_into",this, SLOT( slotDebugStepInto() ));
    m_debugStepInto->setText(i18n("Step into"));
    m_debugStepInto->setIcon(KIcon("debug-step-into"));

    m_debugStepOut = (QAction*) actionc->addAction("debug_step_out",this, SLOT( slotDebugStepOut() ));
    m_debugStepOut->setText(i18n("Step out"));
    m_debugStepOut->setIcon(KIcon("debug-step-out"));

    m_debugRestart = (QAction*) actionc->addAction("debug_restart",this, SLOT( slotDebugRestart() ));
    m_debugRestart->setText(i18n("Restart"));
    m_debugRestart->setIcon(KIcon("appointment-recurring"));

    m_debugConfigureICD = (QAction*) actionc->addAction("debug_configure_icd",this, SLOT( slotDebugConfigureICD() ));
    m_debugConfigureICD->setText(i18n("Configure ICD"));
    m_debugConfigureICD->setIcon(KIcon("configure"));

    m_debugToggleBreakpoint = (QAction*) actionc->addAction("debug_toggle_breakpoint",this, SLOT( slotDebugToggleBreakpoint() ));
    m_debugToggleBreakpoint->setText(i18n("Toggle breakpoint"));
    m_debugToggleBreakpoint->setIcon(KIcon("tools-report-bug"));
    activateDebuggerActions(false);
    // new view:
    m_hideShowMessageBox = (QAction*) actionc->addAction("showmessagebox",this, SLOT( slotHideShowMessageBox() ));
    m_hideShowMessageBox->setText(i18n("Show message box"));
    //m_hideShowMessageBox->setIcon(KIcon("application-cancel"));
    m_hideShowMessageBox->setCheckable(true);

    m_hideShowProjectManager = (QAction*) actionc->addAction("showprojectmanager",this, SLOT( slotHideShowProjectManager() ));
    m_hideShowProjectManager->setText(i18n("Show project manager"));
    //m_hideShowProjectManager->setIcon(KIcon("application-cancel"));
    m_hideShowProjectManager->setCheckable(true);

    m_hideShowSerialTerminal = (QAction*) actionc->addAction("showserialterminal",this, SLOT( slotHideShowSerialTerminal() ));
    m_hideShowSerialTerminal->setText(i18n("Show serial terminal"));
    //m_hideShowSerialTerminal->setIcon(KIcon("application-cancel"));
    m_hideShowSerialTerminal->setCheckable(true);

    m_hideShowMemoryView = (QAction*) actionc->addAction("showmemoryview",this, SLOT( slotHideShowMemoryView() ));
    m_hideShowMemoryView->setText(i18n("Show memory terminal"));
    //m_hideShowMemoryView->setIcon(KIcon("application-cancel"));
    m_hideShowMemoryView->setCheckable(true);
}

void KontrollerLab::createDocks()
{
    m_msgBox = new Q3ListBox(this, "msgBox");
    // m_msgBox->setReadOnly( true );
    m_msgBox->setWindowTitle(i18n("Messages"));
    m_tvaMsg = new QDockWidget(tr("Messages"), this);
    m_tvaMsg->setWidget(m_msgBox);
    addDockWidget( Qt::BottomDockWidgetArea, m_tvaMsg );
    m_tvaMsg->setName("messageBox");

    // Add the dock window for the project manager
    m_tvaProjectManager = new QDockWidget(i18n("Project Manager"), this);
    m_projectManager = new KLProjectManagerWidget( m_project, this, "projectManager" );
    m_tvaProjectManager->setWidget(m_projectManager);
    m_tvaProjectManager->setObjectName("projectManagerDock");
    addDockWidget (Qt::LeftDockWidgetArea,m_tvaProjectManager );

    m_tvaSerialTerminal = new QDockWidget(i18n("Serial terminal"), this);
    m_serialTerminalWidget = new KLSerialTerminalWidget( this, "serialTerminalWidget" );
    m_tvaSerialTerminal->setWidget(m_serialTerminalWidget);
    m_tvaSerialTerminal->setObjectName("serialTerminalDock");
    addDockWidget (Qt::RightDockWidgetArea,m_tvaSerialTerminal );
    //m_tvaSerialTerminal->hide();

    m_tvaMemoryView = new QDockWidget(i18n("Memory View"), this);
    m_memoryViewWidget = new KLMemoryViewWidget(this, "memoryViewWidget");
    m_tvaMemoryView->setWidget(m_memoryViewWidget);
    m_tvaMemoryView->setObjectName("memoryViewDock");
    addDockWidget(Qt::RightDockWidgetArea, m_tvaMemoryView );
    m_memoryViewWidget->setEnabled( false );
    //m_tvaMemoryView->hide();

    tabifyDockWidget(m_tvaSerialTerminal,m_tvaMemoryView);
    m_tvaSerialTerminal->raise();
}

void KontrollerLab::slotNewPart(KParts::Part *newPart, bool setActiv)
{
    m_partManager->addPart(newPart, setActiv);
}

void KontrollerLab::keyPressedOnDocument( )
{
}


void KontrollerLab::slotOpenFile( )
{
    QString startDir = ".";
    if ( m_project->initialized() )
        startDir = m_project->projectBaseURL().path();
    KUrl openURL = KFileDialog::getOpenUrl( startDir );
    if ( !openURL.isEmpty() )
    {
        if ( openURL.path().endsWith( ".kontrollerlab" ) )
        {
            openProject( openURL );
        }
        else
        {
            if (!m_project->initialized())
                m_project->initialize();
            // If the init failed, return!
            if (!m_project->initialized())
                return;

            // If this file is in the project already, show it, but don't add it.
            QList< KLDocument*> docs = m_project->documents();
            foreach(KLDocument* it, docs )
            {
                if ( it->url() == openURL )
                {
                    it->makeLastActiveViewVisible();
                    return;
                }
            }

            KLDocument* doc = new KLDocument(this);
            doc->open( openURL );
            m_project->addDocument( doc );

            slotNewPart( doc->kateDoc(), true );
            // orig: KLDocumentView *view = new KLDocumentView( doc, this );
            new KLDocumentView( doc, this );
        }
    }
}


void KontrollerLab::slotActivePartChanged(KParts::Part *)
{
    // Block adding the same part from two different sides twice:
    // Fixes bug #1652750 (Menu items appear twice sometimes)

    QWidget * activeWid = m_partManager->activeWidget();
    // qDebug("start activeWid %d", (unsigned int) activeWid);
    if (!dynamic_cast<KTextEditor::View *>(activeWid))
        return;

    //disable updates on a window to avoid toolbar flickering on xmlgui client change
    setUpdatesEnabled(false);

    //If there is an old widget showing the document, delete the old view:
    if ( guiFactory()->clients().indexOf( m_oldKTextEditor ) >= 0 )
    {
        //qDebug("REMOVE: %d", m_oldKTextEditor);
        guiFactory()->removeClient( m_oldKTextEditor );
        disconnect (m_oldKTextEditor, SIGNAL(destroyed(QObject*)), this, 0);
        //qDebug("remove %d", (unsigned int) m_oldKTextEditor );
    }
    //Activate the new one:
    m_oldKTextEditor = dynamic_cast<KTextEditor::View *>(activeWid);
    if ( m_oldKTextEditor )
    {
        if ( guiFactory()->clients().indexOf( m_oldKTextEditor ) < 0 )
        {
            guiFactory()->addClient(m_oldKTextEditor);
            //qDebug("added %d", (unsigned int) m_oldKTextEditor );
        }
        if ( m_project && m_project->getDocumentForView(m_oldKTextEditor) )
            setWindowTitle( m_project->getDocumentForView(m_oldKTextEditor)->name() );
    }
    //qDebug("end activeWdg %d", (unsigned int) activeWid );
    setUpdatesEnabled(true);
}


void KontrollerLab::slotCompileAssemble( )
{
    m_project->compile();
}

void KontrollerLab::slotBuild( )
{
    m_project->buildAll();
}

void KontrollerLab::slotRebuildAll( )
{
    m_project->rebuildAll();
}

void KontrollerLab::setCurrentProgInterface()
{
    m_project->setProgrammerInterface( getProgrammer( config( PROGRAMMER_NAME, UISP ) ) );
}

void KontrollerLab::slotErase( )
{
    setCurrentProgInterface();
    m_project->erase();
}

void KontrollerLab::slotUpload( )
{
    setCurrentProgInterface();
    m_project->upload();
}

void KontrollerLab::slotUploadHex( )
{
    KUrl url = KFileDialog::getOpenUrl( m_lastUploadHexURL, "*.hex", this, i18n("Select a file to upload") );
    if (url.isValid())
    {
        setCurrentProgInterface();
        m_project->upload( url );
        m_lastUploadHexURL = url.path();
    }
}

void KontrollerLab::slotVerify( )
{
    setCurrentProgInterface();
    m_project->verify();
}

void KontrollerLab::slotDownload( )
{
    KUrl url = KFileDialog::getSaveUrl( m_lastDownloadHexURL, "*.hex", this, i18n("Select a file to download") );
    if (url.isValid())
    {
        setCurrentProgInterface();
        m_project->download( url );
        m_lastDownloadHexURL = url.path();
    }
}

void KontrollerLab::slotIgnite( )
{
    setCurrentProgInterface();
    m_project->ignite();
}

void KontrollerLab::slotFuses( )
{
    setCurrentProgInterface();
    m_fuseConfigWidget->setModal( true );
    m_fuseConfigWidget->show();
}

void KontrollerLab::slotConfigProgrammer( )
{
    m_configProgrammerWidget->show();
}


KLProgrammerInterface * KontrollerLab::getProgrammer( const QString & name )
{
    if ( m_programmerInterface.find( name ) != m_programmerInterface.end() )
        return m_programmerInterface[ name ];
    else
        return 0L;
}

void KontrollerLab::slotConfigProject( )
{
    m_projectConfigWidget->setModal( true );
    m_projectConfigWidget->show();
}


void KontrollerLab::slotProjectNew( )
{
    if ( m_project->initialized() && m_project->unsaved() )
    {
        int retVal = KMessageBox::questionYesNo( this,
                                                 i18n("Do you really want to open a new project?\n"
                                                      "All unsaved data will be lost."),
                                                 i18n("New project") );
        if ( retVal == KMessageBox::No )
            return;
    }

    KUrl url( KFileDialog::getSaveFileName( KUrl("."), "*.kontrollerlab", this, i18n("New project") ) );
    if ((!url.isEmpty()) && url.isValid())
    {
        if (!url.path().endsWith( ".kontrollerlab" ))
        {
            url = KUrl( url.path() + ".kontrollerlab" );
        }
        m_project->initialize( url.directory() );
        m_project->update();
        m_project->save( url );
    }
}


void KontrollerLab::slotProjectOpen( )
{
    openProject();
}


void KontrollerLab::slotProjectSave( )
{
    if ( m_project->projectFileURL().isValid() )
    {
        m_project->save( m_project->projectFileURL() );
    }
    else
    {
        slotProjectSaveAs();
    }
}


void KontrollerLab::slotProjectSaveAs( )
{
    QString startDir = ".";
    if ( m_project->initialized() )
        startDir = m_project->projectBaseURL().path();

    KUrl url( KFileDialog::getSaveFileName( startDir, "*.kontrollerlab", this, i18n("Save project") ) );
    if ((!url.isEmpty()) && url.isValid())
    {
        m_project->save( url );
    }
}

QString KontrollerLab::config( const QString & key, const QString & defVal )
{
    if ( m_programmerConfig.find( key ) != m_programmerConfig.end() )
        return m_programmerConfig[ key ];
    else
    {
        KConfigGroup conf ( KSharedConfig::openConfig(), "KontrollerLab" );
        QString val = conf.readEntry( key, "" );
        if ( (!val.isEmpty()) && (!val.isNull()) )
            return val;
        else
            return defVal;
    }
}

void KontrollerLab::saveProperties( KSharedConfig::Ptr config )
{
    KConfigGroup conf ( config, "KontrollerLab" );
    KConfigGroup dockConf ( config, "MainWindow" );

    dockConf.writeEntry("Geometry",saveGeometry().toBase64());
    dockConf.writeEntry("State",saveState().toBase64());

    conf.writeEntry( "CURRENT_PROJECT_PATH", m_project->projectFileURL().url() );
    if ( m_project->activeDocument() )
    {
        conf.writeEntry( "CURRENT_ACTIVE_DOCUMENT_NAME", m_project->activeDocument()->name() );
    }
    else
        conf.writeEntry( "CURRENT_ACTIVE_DOCUMENT_NAME", "" );
    //conf.writeEntry( "KLAB_MDI_MODE", mdiMode() );
    m_serialTerminalWidget->saveProperties( config );
    conf.sync();
}


void KontrollerLab::readProperties( KSharedConfig::Ptr config )
{

    KConfigGroup conf ( config, "KontrollerLab" );
    KConfigGroup dockConf ( config, "MainWindow" );

    restoreGeometry(QByteArray::fromBase64(dockConf.readEntry("Geometry","").toLatin1()));
    restoreState(QByteArray::fromBase64(dockConf.readEntry("State","").toLatin1()));

    // restore MDI mode (toplevel, childframe, tabpage)

    /*int mdiMode = conf->readNumEntry( "KLAB_MDI_MODE", 2 );
    switch (mdiMode) {
        case KMdi::ToplevelMode:
        {
            int childFrmModeHt = conf->readNumEntry( "Childframe mode height", QApplication::desktop()->height() - 50);
            resize( width(), childFrmModeHt );
            switchToToplevelMode();
        }
        break;
        case KMdi::ChildframeMode:
            break;
        case KMdi::TabPageMode:
        {
            int childFrmModeHt = conf->readNumEntry( "Childframe mode height", QApplication::desktop()->height() - 50);
            resize( width(), childFrmModeHt );
            switchToTabPageMode();
        }
        break;
        case KMdi::IDEAlMode:
            switchToIDEAlMode();
            break;
        default:
            break;
    }

    
    if (!isFakingSDIApplication()) {
        menuBar()->insertItem( tr("&Window"), windowMenu(), -1, menuBar()->count()-2 );
    }*/
    // OTHER CONFIGS:
    // qDebug( "1: %s", conf->readEntry( "CURRENT_PROJECT_PATH", "__unset__" ).ascii() );
    // qDebug( "2: %s", conf->readEntry( "CURRENT_ACTIVE_DOCUMENT_NAME", "__unset__" ).ascii() );
    // Show it before we open the project. Otherwise, the windows of the
    // files don't come up as expected if they are detached.
    show();
    if ( !m_doNotOpenProjectFromSession )
    {
        QString activeDocName = conf.readEntry( "CURRENT_ACTIVE_DOCUMENT_NAME", "__unset__" );

        if ( conf.readEntry( "CURRENT_PROJECT_PATH", "__unset__" ) != "__unset__" )
        {
            m_project->open( KUrl( conf.readEntry( "CURRENT_PROJECT_PATH", "" ) ) );
        }
        if ( activeDocName != "__unset__" )
        {
            // qDebug( "activeDocName: %s", activeDocName.ascii() );
            QList< KLDocument* > list = m_project->documents();
            foreach( KLDocument *it ,list )
            {
                // qDebug( "%s == %s", it->name().ascii(), activeDocName.ascii() );
                if ( it->name() == activeDocName )
                {
                    it->makeLastActiveViewVisible();
                    break;
                }
            }
        }
    }
    m_serialTerminalWidget->readProperties( config );
}

void KontrollerLab::slotHideShowMessageBox( )
{
    if ( !m_tvaMsg->isHidden() )
        m_tvaMsg->hide();
    else
        m_tvaMsg->show();
}


void KontrollerLab::slotHideShowMemoryView( )
{
    if ( !m_tvaMemoryView->isHidden() )
        m_tvaMemoryView->hide();
    else
        m_tvaMemoryView->show();
}


void KontrollerLab::slotMessageBox( int, const QString & msg )
{
    m_tvaMsg->show();
    m_hideShowMessageBox->setChecked( true );
    QStringList list = QStringList::split( "\n", msg );
    m_msgBox->insertStringList( list );
    m_msgBox->setCurrentItem( m_msgBox->count() - 1 );
    m_msgBox->ensureCurrentVisible();
    m_msgBox->setCurrentItem( -1 );
}

void KontrollerLab::clearMessages( )
{
    m_msgBox->clear();
}

void KontrollerLab::slotMessageBoxDblClicked( Q3ListBoxItem * item )
{
    qDebug() << item->text();
    // Find a file name in this line:

    QList<KLDocument*> list = m_project->documents();
    QString fname;

    foreach (KLDocument* it, list)
    {
        fname = it->name();
        // qDebug(fname.ascii());
        QRegExp re(fname+"\\s*:\\s*([0-9]*)");

        if ( re.search( item->text() ) >= 0 )
        {
            // qDebug( re.cap( 1 ).ascii() );
            bool ok;
            int lineNr = re.cap( 1 ).toInt( &ok );
            if (ok)
                it->setCursorToLine( lineNr );
        }
    }
}

void KontrollerLab::slotProjectClose( )
{
    if ( !m_project->unsaved() )
    {
        m_project->close();
    }
    else
    {
        int retVal = KMessageBox::questionYesNo( this, i18n("Do You really want to close the project?\n"
                                                            "Unsaved data will be lost."), i18n("Close project") );
        if ( retVal == KMessageBox::Yes )
            m_project->close();
    }
}
#include <QCloseEvent>
void KontrollerLab::closeEvent( QCloseEvent * e )
{
    if ( !m_project->unsaved() )
    {
        e->accept();
    }
    else
    {
        int retVal = KMessageBox::questionYesNoCancel( this,
                                                       i18n("Do You want to save all unsaved data before closing?"),
                                                       i18n("Closing KontrollerLab") );
        if ( retVal == KMessageBox::No )
            e->accept();
        else if ( retVal == KMessageBox::Cancel )
            e->ignore();
        else
        {
            slotProjectSave();
            e->accept();
        }
    }
}

void KontrollerLab::slotConfToolbar( )
{
    KConfigGroup group(KSharedConfig::openConfig(),autoSaveGroup());
    saveMainWindowSettings(group);
    KEditToolBar dlg(actionCollection());
    connect(&dlg,SIGNAL(newToolBarConfig()),this,SLOT(slotNewToolbarConfig()));
    dlg.exec();
}


void KontrollerLab::slotNewToolbarConfig()
{
    // this slot is called when user clicks "Ok" or "Apply" in the toolbar editor.
    // recreate our GUI, and re-apply the settings (e.g. "text under icons", etc.)
    //createShellGUI( false );
    //createGUI( 0L );
    //applyMainWindowSettings(KGlobal::config(), autoSaveGroup());
}


void KontrollerLab::configureShortcuts( )
{
    /// @todo implement me
    qDebug("KLayoutLab::configureShortcuts not implemented yet.");
}

void KontrollerLab::rightButtonClickedOnMsgBox( Q3ListBoxItem *, const QPoint & pos )
{
    m_msgBoxPopup->popup( pos );
}

void KontrollerLab::docIsChanged( KTextEditor::Document* )
{
    m_project->checkForModifiedFiles();
}

void KontrollerLab::beAwareOfChangesOf( KLDocument * doc )
{
    connect( doc->kateDoc(), SIGNAL( modifiedChanged(KTextEditor::Document*) ), this, SLOT( docIsChanged(KTextEditor::Document*) ) );
}

void KontrollerLab::backannotateFuses( QMap< QString, QString > fuses )
{
    unsigned char lfuse = m_fuseConfigWidget->lowByte();
    unsigned char hfuse = m_fuseConfigWidget->highByte();
    unsigned char efuse = m_fuseConfigWidget->extByte();
    unsigned char lock = m_fuseConfigWidget->lockByte();

    bool ok1 = true;
    bool ok2 = true;
    bool ok3 = true;
    bool ok4 = true;

    if ( fuses.contains( FUSE_L ) )
        lfuse = fuses[ FUSE_L ].toUShort(&ok1, 16);
    if ( fuses.contains( FUSE_H ) )
        hfuse = fuses[ FUSE_H ].toUShort(&ok2, 16);
    if ( fuses.contains( FUSE_E ) )
        efuse = fuses[ FUSE_E ].toUShort(&ok3, 16);
    if ( fuses.contains( FUSE_LOCK ) )
        lock = fuses[ FUSE_LOCK ].toUShort(&ok4, 16);

    if (ok1 && ok2 && ok3 && ok4)
        m_fuseConfigWidget->setFuseBits( lfuse, hfuse, efuse, lock );
}

void KontrollerLab::slotHideShowProjectManager( )
{
    if ( !m_tvaProjectManager->isHidden() )
        m_tvaProjectManager->hide();
    else
        m_tvaProjectManager->show();
}

void KontrollerLab::slotNewFile( )
{
    if (!m_project->initialized())
        m_project->initialize();
    // If the init failed, return!
    if (!m_project->initialized())
        return;

    KLFileNewDialog* dlg = new KLFileNewDialog( this, m_project, "file_new_dialog" );
    dlg->show();
}


void KontrollerLab::slotHideShowSerialTerminal( )
{
    if ( !m_tvaSerialTerminal->isHidden() )
        m_tvaSerialTerminal->hide();
    else
        m_tvaSerialTerminal->show();
}

void KontrollerLab::slotSaveFile( )
{
    if (!m_project)
        return;
    if (!m_project->activeDocument())
        return;
    m_project->activeDocument()->save();
}

void KontrollerLab::slotSaveFileAs( )
{
    QString startDir = ".";

    if (!m_project)
        return;
    if (!m_project->activeDocument())
        return;

    KUrl url( KFileDialog::getSaveFileName( startDir, "*", this, i18n("Save file as") ) );
    if ((!url.isEmpty()) && url.isValid())
    {
        m_project->activeDocument()->saveAs( url );
    }
}

void KontrollerLab::slotDocumentUrlChanged( KTextEditor::Document *document )
{
    QList< KLDocument* > docs = m_project->documents();
    foreach ( KLDocument* doc,docs )
    {
        if ( doc->kateDoc() == document )
        {
            if ( document->url() != doc->url() )
            {
                doc->setUrl( document->url() );
                m_project->setUnsaved();
            }
        }
    }
    m_project->checkForModifiedFiles();
}

void KontrollerLab::newDocumentOpened( KTextEditor::Document * doc )
{
    connect( doc, SIGNAL(documentNameChanged(KTextEditor::Document*)),
             this, SLOT(slotDocumentUrlChanged(KTextEditor::Document*)) );
}

void KontrollerLab::resizeEvent( QResizeEvent * e )
{

    KMainWindow::resizeEvent( e );
    //setSysButtonsAtMenuPosition();
}

//void KontrollerLab::slotMdiModeHasBeenChangedTo( KMdi::MdiMode )
//{
//    setMenuForSDIModeSysButtons( menuBar() );
//    hideViewTaskBar();
//}

void KontrollerLab::slotSevenSegmentsWizard( )
{
    KLDocument* doc = m_project->activeDocument();
    if (!doc || !doc->lastActiveView() )
    {
        KMessageBox::information( this,
                                  i18n("The wizards need an opended document to work on."),
                                  i18n("Couldn't start wizard"),
                                  "couldnt_start_wizard" );
        return;
    }
    if ( m_sevenSegmentWizardWidget )
        delete m_sevenSegmentWizardWidget;
    m_sevenSegmentWizardWidget = new KLSevenSegmentWidget(this, "sevensegmentwizard", doc);
    m_sevenSegmentWizardWidget->show();
}


void KontrollerLab::slotCloseFile( )
{
    if (m_project->activeDocument())
    {
        KLDocumentView *view=m_project->activeDocument()->lastActiveView();
        if (view)
            view->close();
    }
}

//void KontrollerLab::slotChildClosed( KMdiChildView * )
//{

//}

//void KontrollerLab::childWindowCloseRequest( KMdiChildView * pWnd )
//{

//}

void KontrollerLab::emitActivePartChanged( KParts::Part * part, KLDocumentView* viewToBeOpened )
{
    m_viewToBeOpened = viewToBeOpened;
    emit( activePartChanged( part ) );
}


void KontrollerLab::openProject(const KUrl &url_ )
{
    if ( m_project->initialized() && m_project->unsaved() )
    {
        int retVal = KMessageBox::questionYesNo( this,
                                                 i18n("Do you really want to open a project?\n"
                                                      "All unsaved data will be lost."),
                                                 i18n("Open project") );
        if ( retVal == KMessageBox::No )
            return;
    }
    KUrl url = url_;
    if ( url.isEmpty() )
        url = KUrl( KFileDialog::getOpenFileName( KUrl("."), "*.kontrollerlab", this, i18n("Open project") ) );
    if ((!url.isEmpty()) && url.isValid())
    {
        m_project->close();
        m_project->open( url );
    }
}

void KontrollerLab::setProgrammerConfig( const QMap< QString, QString > & theValue )
{
    m_programmerConfig = theValue;
    getProgrammer( UISP )->configuration() = m_programmerConfig;
    getProgrammer( AVRDUDE )->configuration() = m_programmerConfig;
    setCurrentProgInterface();
    m_project->setUnsaved();
}

void KontrollerLab::backannotateSTK500( const QString & stdout )
{
    m_configProgrammerWidget->backannotateSTK500( stdout );
}


void KontrollerLab::slotDotMatrixWizard( )
{
    KLDocument* doc = m_project->activeDocument();
    if (!doc)
    {
        KMessageBox::information( this,
                                  i18n("The wizards need an opended document to work on."),
                                  i18n("Couldn't start wizard"),
                                  "couldnt_start_wizard" );
        return;
    }
    if ( m_dotMatrixWizardWidget )
        delete m_dotMatrixWizardWidget;
    m_dotMatrixWizardWidget = new KLDotMatrixWidget(this, "dotmatrixwizard", m_project, doc);
    m_dotMatrixWizardWidget->show();
}

void KontrollerLab::slotDotMatrixCharacterWizard( )
{
    KLDocument* doc = m_project->activeDocument();
    if (!doc || !doc->lastActiveView())
    {
        KMessageBox::information( this,
                                  i18n("The wizards need an opended document to work on."),
                                  i18n("Couldn't start wizard"),
                                  "couldnt_start_wizard" );
        return;
    }
    if ( m_dotMatrixCharacterWizardWidget )
        delete m_dotMatrixCharacterWizardWidget;
    m_dotMatrixCharacterWizardWidget = new KLDotMatrixCharacterWizard(this, "dotmatrixwizard", m_project, doc);
    m_dotMatrixCharacterWizardWidget->show();
}


void KontrollerLab::slotDirectMemoryDebug( )
{
    m_debugger->setDMMode( true );
    m_debugStart->setEnabled( false );
}


void KontrollerLab::slotInCircuitDebugger( )
{
    m_debugger->setICDMode( true );
    m_debugStart->setEnabled( true );
}


void KontrollerLab::slotPCOnlyDebug( )
{
    m_debugger->setICDMode( false );
    m_debugger->setDMMode( false );
    m_debugStart->setEnabled( true );
}


void KontrollerLab::slotDebugStart( )
{
    m_debugger->startDebugger();
    m_directMemoryDebug->setEnabled( false );
}


void KontrollerLab::slotDebugStop( )
{
    m_debugger->stopDebugger();
    if ( m_debugger->state() == DBG_Started )
    {
        m_debugPause->setText(i18n("Stop"));
        m_debugPause->setIcon( KIcon("media-playback-pause") );
    }
    else
    {
        m_debugPause->setText(i18n("Start"));
        m_debugPause->setIcon( KIcon("media-playback-start") );
    }
    m_directMemoryDebug->setEnabled( true );
}


void KontrollerLab::slotDebugPause( )
{
    m_debugger->playpauseDebugger();
    if ( m_debugger->state() == DBG_Started )
    {
        m_debugPause->setIcon( KIcon("media-playback-pause") );
        m_debugPause->setText( i18n("Pause") );
    }
    else
    {
        notifyDebuggerPaused();
    }
}


void KontrollerLab::slotDebugRunToCursor( )
{
    if ( m_project )
    {
        KLDocument* doc = m_project->activeDocument();
        if (!doc)
            return;
        KLDocumentView* view = doc->lastActiveView();
        if (!view)
            return;
        m_debugger->runToCLine( doc->url(), view->view()->cursorPosition().line() + 1 );
    }
}


void KontrollerLab::slotDebugStepOver( )
{
    m_debugger->runUntilCLineChangesSteppingOverFunctions();
}


void KontrollerLab::slotDebugStepInto( )
{
    m_debugger->runUntilCLineChanges();
}


void KontrollerLab::slotDebugStepOut( )
{
    m_debugger->stepOutOfFunction();
}


void KontrollerLab::slotDebugRestart( )
{
    m_debugger->stopDebugger();
    m_debugger->startDebugger();
}


void KontrollerLab::slotDebugConfigureICD( )
{
    m_debuggerConfigWidget = new KLDebuggerConfigWidget( this, "debugger_config_widget" );
    m_debuggerConfigWidget->setModal( true );
    m_debuggerConfigWidget->show();
}


void KontrollerLab::setProgrammerBusy( bool val )
{
    if ( m_serialTerminalWidget )
        m_serialTerminalWidget->setProgrammerBusy( val );
}




void KontrollerLab::notifyDebuggerReady( )
{
    activateDebuggerActions( true );
    m_memoryViewWidget->setEnabled( true );
    m_memoryViewWidget->updateGUI();
}


void KontrollerLab::notifyDebuggerStopped( )
{
    activateDebuggerActions( false );
    m_memoryViewWidget->setEnabled( false );
}


void KontrollerLab::setAllMemoryViewValuesToZero( )
{
    if (m_memoryViewWidget)
    {
        m_memoryViewWidget->setAllValuesToZero();
    }
}


void KontrollerLab::slotDebugToggleBreakpoint( )
{
    if ( m_project )
    {
        KLDocument* doc = m_project->activeDocument();
        if ( doc && doc->lastActiveView() && m_debugger )
        {
            m_debugger->toggleBreakpoint( doc->url(),
                                          doc->lastActiveView()->view()->cursorPosition().line() +1 );
        }
    }
}

void KontrollerLab::activateDebuggerActions( bool activate )
{
    m_debugStart->setEnabled( !activate );
    m_debugStop->setEnabled( activate );
    m_debugPause->setEnabled( activate );
    m_debugRunToCursor->setEnabled( activate );
    m_debugStepOver->setEnabled( activate );
    m_debugStepInto->setEnabled( activate );
    m_debugStepOut->setEnabled( activate );
    m_debugRestart->setEnabled( activate );
}


void KontrollerLab::notifyDebuggerPaused( )
{
    m_debugPause->setIcon( KIcon("media-playback-start") );
    m_debugPause->setText( i18n("Start") );
}


void KontrollerLab::slotStopProgrammer( )
{
    getProgrammer( config( PROGRAMMER_NAME, UISP ) )->stopKillingProc();
}


void KontrollerLab::slotNewViewForDocument()
{
    if ( !m_project )
        return;
    if ( !m_project->activeDocument() )
        return;
    new KLDocumentView( m_project->activeDocument(), this );
}


