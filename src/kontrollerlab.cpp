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
#include <kstdaction.h>



#include <ktexteditor/markinterface.h>

#include <ktexteditor/document.h>
#include <ktexteditor/view.h>
#include <ktexteditor/editorchooser.h>

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




KontrollerLab::KontrollerLab( bool doNotOpenProjectFromSession )
    : KMainWindow( 0 )
{
    //setStandardToolBarMenuEnabled( true );
    //createStandardStatusBarAction();

    m_lastDownloadHexURL = "";
    m_lastUploadHexURL = "";
    m_serialTerminalWidget = 0L;

    // set the shell's ui resource file
    // setXMLFile("kontrollerlabui.rc");
    //

    m_mdiArea = new QMdiArea(this);
    m_mdiArea->setViewMode(QMdiArea::TabbedView);
    m_mdiArea->setDocumentMode(true);
    m_mdiArea->setTabsMovable(true);
    m_mdiArea->setTabsClosable(true);

    setCentralWidget(m_mdiArea);

    //m_editorWidget = new KLEditorWidget(this);

    KLDocument* doc = new KLDocument(this);
    doc->activateCHighlighting();
    doc->setUrl(KUrl("unnamed"));
    new KLDocumentView( doc, this );

    m_oldKTextEditor = 0L;
    m_viewToBeOpened = 0L;
    m_doNotOpenProjectFromSession = doNotOpenProjectFromSession;

    // ACTIONS:
    /*
    m_newFile = KStandardAction::openNew( this, SLOT( slotNewFile() ), actionCollection());
    m_openFile = KStandardAction::open( this, SLOT( slotOpenFile() ), actionCollection());
    m_saveFile = KStandardAction::save( this, SLOT( slotSaveFile() ), actionCollection());
    m_saveFileAs = KStandardAction::saveAs( this, SLOT( slotSaveFileAs() ), actionCollection());
    m_closeProgram = KStandardAction::quit( this, SLOT( close() ), actionCollection());
    m_newProject = new QAction( i18n("Close file"), "fileclose",
                                KShortcut(), this, SLOT( slotCloseFile() ),
                                actionCollection());
    // Project:
    m_newProject = new QAction( i18n("New project"),QKeySequence(), this, SLOT( slotProjectNew() );

    m_openProject = new QAction( i18n("Open project"), "fileopen",
                                KShortcut(), this, SLOT( slotProjectOpen() ),
                                actionCollection(), "open_project" );
    m_saveProject = new QAction( i18n("Save project"), "filesave",
                                KShortcut(), this, SLOT( slotProjectSave() ),
                                actionCollection(), "save_project" );
    m_saveProjectAs = new QAction( i18n("Save project as"), "filesave",
                                 KShortcut(), this, SLOT( slotProjectSaveAs() ),
                                 actionCollection(), "save_project_as" );
    m_closeProject = new QAction( i18n("Close project"), "fileclose",
                                   KShortcut(), this, SLOT( slotProjectClose() ),
                                   actionCollection(), "close_project" );

    m_compileAssemble = new QAction( i18n("Compile file"), "compfile",
                                     KShortcut("F7"), this, SLOT( slotCompileAssemble() ),
                                     actionCollection(), "compile" );
    m_build = new QAction( i18n("Build project"), "gear",
                           KShortcut("F9"), this, SLOT( slotBuild() ),
                           actionCollection(), "build" );
    m_rebuildAll = new QAction( i18n("Rebuild all"), "rebuild",
                                KShortcut(), this, SLOT( slotRebuildAll() ),
                                actionCollection(), "rebuild" );
    m_erase = new QAction( i18n("Erase device"), "eraser",
                           KShortcut(), this, SLOT( slotErase() ),
                           actionCollection(), "erase" );
    m_upload = new QAction( i18n("Upload"), "up",
                            KShortcut(), this, SLOT( slotUpload() ),
                            actionCollection(), "upload" );
    m_uploadHex = new QAction( i18n("Upload hex file"), "up",
                               KShortcut(), this, SLOT( slotUploadHex() ),
                               actionCollection(), "uploadHex" );
    m_verify = new QAction( i18n("Verify"), "viewmag1",
                            KShortcut(), this, SLOT( slotVerify() ),
                            actionCollection(), "verify" );
    m_download = new QAction( i18n("Download"), "down",
                              KShortcut(), this, SLOT( slotDownload() ),
                              actionCollection(), "download" );
    m_ignite = new QAction( i18n("Ignite"), "fork",
                            KShortcut("Shift+F9"), this, SLOT( slotIgnite() ),
                            actionCollection(), "ignite" );
    m_stopKillingProc = new QAction( i18n("Stop"), "cancel",
                                     KShortcut("ESC"), this, SLOT( slotStopProgrammer() ),
                                     actionCollection(), "stop_programming" );
    m_fuses = new QAction( i18n("Fuses"), "flag",
                           KShortcut(), this, SLOT( slotFuses() ),
                           actionCollection(), "fuses" );
    m_configProgrammer = new QAction( i18n("Configure programmer"), "configure",
                                      KShortcut(), this, SLOT( slotConfigProgrammer() ),
                                      actionCollection(), "configProgrammer" );
    m_configProgrammer = new QAction( i18n("Configure project"), "configure",
                                      KShortcut(), this, SLOT( slotConfigProject() ),
                                      actionCollection(), "configProject" );
    // Wizards:
    m_sevenSegmentsWizardAction = new QAction( i18n("Seven segments wizard"), "wizard",
                                      KShortcut(), this, SLOT( slotSevenSegmentsWizard() ),
                                      actionCollection(), "seven_segment_wizard" );
    m_dotMatrixWizardAction = new QAction( i18n("Dot matrix display wizard"), "wizard",
                                        KShortcut(), this, SLOT( slotDotMatrixWizard() ),
                                        actionCollection(), "dot_matrix_wizard" );
    m_dotMatrixCharacterWizardAction = new QAction( i18n("Dot matrix character wizard"), "wizard",
                                        KShortcut(), this, SLOT( slotDotMatrixCharacterWizard() ),
                                        actionCollection(), "dot_matrix_character_wizard" );
    // Debug:
    m_directMemoryDebug = new KRadioAction( i18n("Direct memory debugger"), "dbg_dm",
                                            KShortcut(), this, SLOT( slotDirectMemoryDebug() ),
                                            actionCollection(), "direct_memory_debug" );
    m_directMemoryDebug->setExclusiveGroup( "debugMode" );
    m_inCircuitDebugger = new KRadioAction( i18n("In circuit debugger"), "dbg_icd",
                                            KShortcut(), this, SLOT( slotInCircuitDebugger() ),
                                            actionCollection(), "in_circuit_debugger" );
    m_inCircuitDebugger->setExclusiveGroup( "debugMode" );
    m_PCOnlyDebug = new KRadioAction( i18n("PC only debugging"), "dbg_pco",
                                      KShortcut(), this, SLOT( slotPCOnlyDebug() ),
                                      actionCollection(), "pc_only_debug" );
    m_PCOnlyDebug->setChecked( true );
    m_PCOnlyDebug->setExclusiveGroup( "debugMode" );

    m_debugStart = new QAction( i18n("Enable"), "dbgrun",
                                KShortcut(), this, SLOT( slotDebugStart() ),
                                actionCollection(), "debug_start" );
    m_debugStop = new QAction( i18n("Stop"), "stop",
                               KShortcut(), this, SLOT( slotDebugStop() ),
                               actionCollection(), "debug_stop" );
    m_debugPause = new QAction( i18n("Start"), "player_play",
                                KShortcut(), this, SLOT( slotDebugPause() ),
                                actionCollection(), "debug_pause" );
    m_debugRunToCursor = new QAction( i18n("Run to cursor"), "dbgrunto",
                                      KShortcut(), this, SLOT( slotDebugRunToCursor() ),
                                      actionCollection(), "debug_run_to_cursor" );
    m_debugStepOver = new QAction( i18n("Step over"), "dbgnext",
                                   KShortcut(), this, SLOT( slotDebugStepOver() ),
                                   actionCollection(), "debug_step_over" );
    m_debugStepInto = new QAction( i18n("Step into"), "dbgstep",
                                   KShortcut(), this, SLOT( slotDebugStepInto() ),
                                   actionCollection(), "debug_step_into" );
    m_debugStepOut = new QAction( i18n("Step out"), "dbgstepout",
                                  KShortcut(), this, SLOT( slotDebugStepOut() ),
                                  actionCollection(), "debug_step_out" );
    m_debugRestart = new QAction( i18n("Restart"), "reload",
                                  KShortcut(), this, SLOT( slotDebugRestart() ),
                                  actionCollection(), "debug_restart" );
    m_debugConfigureICD = new QAction( i18n("Configure ICD"), "configure",
                                       KShortcut(), this, SLOT( slotDebugConfigureICD() ),
                                       actionCollection(), "debug_configure_icd" );
    m_debugToggleBreakpoint = new QAction( i18n("Toggle breakpoint"), "player_pause",
                                         KShortcut(), this, SLOT( slotDebugToggleBreakpoint() ),
                                         actionCollection(), "debug_toggle_breakpoint" );

    activateDebuggerActions( false );


    // new view:
    m_newViewForDocument = new QAction( i18n("New view for document"), "contents",
                                        KShortcut(), this, SLOT( slotNewViewForDocument() ),
                                        actionCollection(), "view_new_view" );

    // Message box:
    m_hideShowMessageBox = new KToggleAction( i18n("Show message box"), "",
                                              KShortcut::KShortcut( "" ), this,
            SLOT(slotHideShowMessageBox()), actionCollection(), "showmessagebox" );
    m_hideShowProjectManager = new KToggleAction( i18n("Show project manager"), "",
                                              KShortcut::KShortcut( "" ), this,
                                              SLOT(slotHideShowProjectManager()),
                                              actionCollection(), "showprojectmanager" );
    m_hideShowSerialTerminal = new KToggleAction( i18n("Show serial terminal"), "",
            KShortcut::KShortcut( "" ), this,
            SLOT(slotHideShowSerialTerminal()),
            actionCollection(), "showserialterminal" );

    m_hideShowMemoryView = new KToggleAction( i18n("Show memory view"), "dbgmemview",
                                              KShortcut::KShortcut( "" ), this,
                                              SLOT(slotHideShowMemoryView()),
                                              actionCollection(), "showmemoryview" );

    // Settings menu
    KStdAction::configureToolbars(this, SLOT(slotConfToolbar()), actionCollection());
    KStdAction::keyBindings(guiFactory(), SLOT(configureShortcuts()), actionCollection());*/

    
    //createGUI( 0L );
    // The procmanager must be set up before the project is built.

    createActions();
    createMenus();
    createToolBars();

    m_procManager = new KLProcessManager( this, "processManager" );
    
    m_project = new KLProject( this );

    // Add the tool window for the project manager
    //m_tvaProjectManager = addToolWindow( m_projectManager, KDockWidget::DockRight,
    //                      getMainDockWidget(), 80 );
    // ADD NEW PROGRAMMERS HERE:
    m_programmerInterface[ UISP ] = new KLProgrammerUISP( m_procManager, m_project );
    m_programmerInterface[ AVRDUDE ] = new KLProgrammerAVRDUDE( m_procManager, m_project );

    m_project->addDocument( doc );
    m_project->setProgrammerInterface( getProgrammer( UISP ) );

    m_kateGuiClientAdded = 0L;
    m_sevenSegmentWizardWidget = 0L;
    m_dotMatrixWizardWidget = 0L;
    m_dotMatrixCharacterWizardWidget = 0L;


    //m_msgBox = new Q3ListBox(this, "msgBox");
    // m_msgBox->setReadOnly( true );
    //m_msgBox->setCaption(i18n("Messages"));
    //m_tvaMsg = addDockWidget( QDockWidget::DockBottom, getMainDockWidget() );
    //m_tvaMsg->setName("messageBox");

    //Setting up UI, TODO -> inline func

    //setCentralWidget(m_editorWidget);

    QDockWidget *dock;

    dock = new QDockWidget(tr("Project Manager"), this);
    m_projectManager = new KLProjectManagerWidget( m_project, this, "projectManager" );
    dock->setWidget(m_projectManager);
    addDockWidget (Qt::LeftDockWidgetArea,dock );

    dock = new QDockWidget(tr("Serial terminal"), this);

    m_serialTerminalWidget = new KLSerialTerminalWidget( this, "serialTerminalWidget" );
    dock->setWidget(m_serialTerminalWidget);
    addDockWidget (Qt::LeftDockWidgetArea,dock );
    m_serialTerminalWidget->hide();

    dock = new QDockWidget(tr("Memory View"), this);
    
    m_memoryViewWidget = new KLMemoryViewWidget(this, "memoryView");
    dock->setWidget(m_memoryViewWidget);
    addDockWidget(Qt::LeftDockWidgetArea, dock );
    m_memoryViewWidget->hide();
    
    m_debugger = new KLDebugger( m_serialTerminalWidget, this, "debugger" );
    m_debugger->setMemoryViewWidget( m_memoryViewWidget );


    m_partManager = new KParts::PartManager( this );
    m_partManager->addManagedTopLevelWidget( this );
    
    //m_msgBoxPopup = new KPopupMenu(this,"msgBoxPopup");
    KIconLoader kico;
    //m_msgBoxPopup->insertItem( kico.loadIcon( "clear_left", KIcon::Toolbar ), i18n("Clear messages"), this, SLOT( clearMessages() ) );
    
    connect(m_partManager, SIGNAL(activePartChanged(KParts::Part * )),
            this, SLOT(slotActivePartChanged(KParts::Part * )));
    //    connect( m_partManager, SIGNAL( activePartChanged( KParts::Part * ) ),
    //             this, SLOT( createGUI( KParts::Part * ) ) );
    connect(this, SIGNAL(dockWidgetHasUndocked(KDockWidget *)), this, SLOT(slotDockWidgetHasUndocked(KDockWidget *)));
    // connect(tabWidget(), SIGNAL(initiateDrag(QWidget *)), this, SLOT(slotTabDragged(QWidget*)));

    //connect(m_msgBox, SIGNAL(doubleClicked(Q3ListBoxItem *)), this, SLOT(slotMessageBoxDblClicked(Q3ListBoxItem *)));
    //connect(m_msgBox, SIGNAL(rightButtonPressed( QListBoxItem*, const QPoint& )), this,
    //        SLOT( rightButtonClickedOnMsgBox( Q3ListBoxItem*, const QPoint& ) ) );
    connect(this, SIGNAL(mdiModeHasBeenChangedTo(KMdi::MdiMode)), this, SLOT(slotMdiModeHasBeenChangedTo (KMdi::MdiMode)) );
    connect( this, SIGNAL( activePartChanged( KParts::Part* ) ),
             this, SLOT( slotActivePartChanged( KParts::Part* ) ) );

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
    
    //dockManager->finishReadDockConfig();
    //dockManager->readConfig( KGlobal::config(), "kontrollerlab_dockinfo" );

    //m_hideShowMessageBox->setChecked( m_tvaMsg->wrapperWidget()->isShown() );
    //m_hideShowProjectManager->setChecked( m_tvaProjectManager->wrapperWidget()->isShown() );
    //m_hideShowSerialTerminal->setChecked( m_tvaSerialTerminal->wrapperWidget()->isShown() );
    /*
    KLProcess* proc = new KLProcess("uisp --help");
    connect( proc, SIGNAL(processExited( KLProcess* )),
             this, SLOT(delme( KLProcess* ) ) );
    proc->start();
    */
    
    readProperties( KSharedConfig::openConfig());
}

void KontrollerLab::createActions()
{
    newAct = new QAction(QIcon(":/images/new.png"), tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, SIGNAL(triggered()), this, SLOT(slotNewFile()));

    openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(slotOpenFile()));

    saveAct = new QAction(QIcon(":/images/save.png"), tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(slotSaveFile()));

    saveAsAct = new QAction(tr("Save &As..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(slotSaveFileAs()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));

    cutAct = new QAction(QIcon(":/images/cut.png"), tr("Cu&t"), this);
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
    //connect(cutAct, SIGNAL(triggered()), this, SLOT(cut()));

    copyAct = new QAction(QIcon(":/images/copy.png"), tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    //connect(copyAct, SIGNAL(triggered()), this, SLOT(copy()));

    pasteAct = new QAction(QIcon(":/images/paste.png"), tr("&Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    //connect(pasteAct, SIGNAL(triggered()), this, SLOT(paste()));

    closeAct = new QAction(tr("Cl&ose"), this);
    closeAct->setStatusTip(tr("Close the active window"));
    //connect(closeAct, SIGNAL(triggered()),
    //mdiArea, SLOT(closeActiveSubWindow()));

    closeAllAct = new QAction(tr("Close &All"), this);
    closeAllAct->setStatusTip(tr("Close all the windows"));
    //connect(closeAllAct, SIGNAL(triggered()),
    //mdiArea, SLOT(closeAllSubWindows()));

    tileAct = new QAction(tr("&Tile"), this);
    tileAct->setStatusTip(tr("Tile the windows"));
    //connect(tileAct, SIGNAL(triggered()), mdiArea, SLOT(tileSubWindows()));

    cascadeAct = new QAction(tr("&Cascade"), this);
    cascadeAct->setStatusTip(tr("Cascade the windows"));
    //connect(cascadeAct, SIGNAL(triggered()), mdiArea, SLOT(cascadeSubWindows()));

    nextAct = new QAction(tr("Ne&xt"), this);
    nextAct->setShortcuts(QKeySequence::NextChild);
    nextAct->setStatusTip(tr("Move the focus to the next window"));
    //connect(nextAct, SIGNAL(triggered()),
    //mdiArea, SLOT(activateNextSubWindow()));

    previousAct = new QAction(tr("Pre&vious"), this);
    previousAct->setShortcuts(QKeySequence::PreviousChild);
    previousAct->setStatusTip(tr("Move the focus to the previous "
                                 "window"));
    //connect(previousAct, SIGNAL(triggered()),
    //mdiArea, SLOT(activatePreviousSubWindow()));

    separatorAct = new QAction(this);
    separatorAct->setSeparator(true);

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void KontrollerLab::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();
    QAction *action = fileMenu->addAction(tr("Switch layout direction"));
    connect(action, SIGNAL(triggered()), this, SLOT(switchLayoutDirection()));
    fileMenu->addAction(exitAct);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);

    windowMenu = menuBar()->addMenu(tr("&Window"));
    //updateWindowMenu();
    connect(windowMenu, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
}

void KontrollerLab::createToolBars()
{
    fileToolBar = toolBar(i18n("File"));
    fileToolBar->setCaption(i18n("File"));
    fileToolBar->addAction(newAct);
    fileToolBar->addAction(openAct);
    fileToolBar->addAction(saveAct);

    editToolBar = toolBar(i18n("Edit"));
    editToolBar->setCaption(i18n("Edit"));
    editToolBar->addAction(cutAct);
    editToolBar->addAction(copyAct);
    editToolBar->addAction(pasteAct);
}

KontrollerLab::~KontrollerLab()
{
    saveProperties( KSharedConfig::openConfig() );
    //dockManager->writeConfig( KGlobal::config(), "kontrollerlab_dockinfo" );
}


void KontrollerLab::slotNewPart(KParts::Part *newPart, bool setActiv)
{
    //m_partManager->addPart(newPart, setActiv);
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

    // If there is an old widget showing the document, delete the old view:
    //if ( guiFactory()->clients().findRef( m_oldKTextEditor ) >= 0 )
    //{
    // qDebug("REMOVE: %d", m_oldKTextEditor);
    //guiFactory()->removeClient( m_oldKTextEditor );
    // qDebug("remove %d", (unsigned int) m_oldKTextEditor );
    //}
    // Activate the new one:
    //m_oldKTextEditor = dynamic_cast<KTextEditor::View *>(activeWid);
    //if ( m_oldKTextEditor )
    //{
    //if ( guiFactory()->clients().findRef( m_oldKTextEditor ) < 0 )
    //{
    //guiFactory()->addClient(m_oldKTextEditor);
    // qDebug("added %d", (unsigned int) m_oldKTextEditor );
    //}
    //if ( m_project && m_project->getDocumentForView(m_oldKTextEditor) )
    //setCaption( m_project->getDocumentForView(m_oldKTextEditor)->name() );
    //}
    // qDebug("end activeWdg %d", (unsigned int) activeWid );
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

    KUrl url( KFileDialog::getOpenFileName( KUrl("."), "*.kontrollerlab", this, i18n("New project") ) );
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
    // qDebug("KontrollerLab::saveProperties( KConfig * conf )");
    KConfigGroup conf ( config, "KontrollerLab" );
    
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

void KontrollerLab::slotDockWidgetHasUndocked(K3DockWidget * )
{
    
}


void KontrollerLab::slotHideShowMessageBox( )
{
    /*if ( m_tvaMsg->wrapperWidget()->isShown() )
        m_tvaMsg->hide();
    else
        m_tvaMsg->show();*/
}


void KontrollerLab::slotHideShowMemoryView( )
{
    /*if ( m_tvaMemoryView->wrapperWidget()->isShown() )
        m_tvaMemoryView->hide();
    else
        m_tvaMemoryView->show();*/
}


void KontrollerLab::slotMessageBox( int, const QString & msg )
{
    /*m_tvaMsg->show();
    m_hideShowMessageBox->setChecked( true );
    QStringList list = QStringList::split( "\n", msg );
    m_msgBox->insertStringList( list );
    m_msgBox->setCurrentItem( m_msgBox->count() - 1 );
    m_msgBox->ensureCurrentVisible();
    m_msgBox->setCurrentItem( -1 );*/
}

void KontrollerLab::clearMessages( )
{
    m_msgBox->clear();
}

void KontrollerLab::slotMessageBoxDblClicked( Q3ListBoxItem * item )
{
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
    //KEditToolbar dlg(actionCollection());
    //connect(&dlg,SIGNAL(newToolbarConfig()),this,SLOT(slotNewToolbarConfig()));
    //dlg.exec();
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

void KontrollerLab::docIsChanged( )
{
    m_project->checkForModifiedFiles();
}

void KontrollerLab::beAwareOfChangesOf( KLDocument * doc )
{
    connect( doc->kateDoc(), SIGNAL( textChanged() ), this, SLOT( docIsChanged() ) );
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
    /* if ( m_tvaProjectManager->wrapperWidget()->isShown() )
        m_tvaProjectManager->hide();
    else
        m_tvaProjectManager->show();*/
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
    /*if ( m_tvaSerialTerminal->wrapperWidget()->isShown() )
        m_tvaSerialTerminal->hide();
    else
        m_tvaSerialTerminal->show();*/
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
    connect( doc, SIGNAL(nameChanged(Kate::Document*)),
             this, SLOT(slotDocumentUrlChanged( Kate::Document* )) );
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

/*void KontrollerLab::childWindowCloseRequest( KMdiChildView * pWnd )
{
    KLDocumentView *view = static_cast<KLDocumentView*>(pWnd);
    if ( !view )
        return;
    if ( !view->document() )
    {
        setOldKTextEditor( view->view() );
        emitActivePartChanged( 0L );
        KMdiMainFrm::childWindowCloseRequest( pWnd );
    }
    else if ( ( !view->document()->isModified() ) ||
              ( view->document()->kateDoc() &&
                ( view->document()->kateDoc()->views().count() > 1 ) ) )
    {
        setOldKTextEditor( view->view() );
        emitActivePartChanged( 0L );
        KMdiMainFrm::childWindowCloseRequest( pWnd );
    }
    else
    {
        int retVal = KMessageBox::questionYesNoCancel( this,
                         i18n("Do you want to save the document before closing?"),
                         i18n("Close document") );

        if ( retVal == KMessageBox::No )
        {
            KLDocument* doc = view->document();
            setOldKTextEditor( view->view() );
            emitActivePartChanged( 0L );
            KMdiMainFrm::childWindowCloseRequest( pWnd );
            doc->revert();
        }
        else if ( retVal == KMessageBox::Yes )
        {
            if (view->document()->save())
            {
                setOldKTextEditor( view->view() );
                emitActivePartChanged( 0L );
                KMdiMainFrm::childWindowCloseRequest( pWnd );
            }
        }
    }
}*/

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
    //m_debugStart->setEnabled( false );
}


void KontrollerLab::slotInCircuitDebugger( )
{
    m_debugger->setICDMode( true );
    //m_debugStart->setEnabled( true );
}


void KontrollerLab::slotPCOnlyDebug( )
{
    m_debugger->setICDMode( false );
    m_debugger->setDMMode( false );
    //m_debugStart->setEnabled( true );
}


void KontrollerLab::slotDebugStart( )
{
    m_debugger->startDebugger();
    //m_directMemoryDebug->setEnabled( false );
}


void KontrollerLab::slotDebugStop( )
{
    m_debugger->stopDebugger();
    if ( m_debugger->state() == DBG_Started )
    {
        //m_debugPause->setIcon( "player_pause" );
    }
    else
    {
        //m_debugPause->setIcon( "player_play" );
    }
    //m_directMemoryDebug->setEnabled( true );
}


void KontrollerLab::slotDebugPause( )
{
    m_debugger->playpauseDebugger();
    if ( m_debugger->state() == DBG_Started )
    {
        //m_debugPause->setIcon( "player_pause" );
        //m_debugPause->setText( i18n("Pause") );
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
        //m_debugger->runToCLine( doc->url(), view->view()->cursorLine()+1 );
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
    //m_debuggerConfigWidget->setModal( true );
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
    m_memoryViewWidget->updateGUI();
}


void KontrollerLab::notifyDebuggerStopped( )
{
    activateDebuggerActions( false );
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
            //m_debugger->toggleBreakpoint( doc->url(),
            //doc->lastActiveView()->view()->cursorLine()+1 );
        }
    }
}


void KontrollerLab::activateDebuggerActions( bool activate )
{
    /*m_debugStart->setEnabled( !activate );
    m_debugStop->setEnabled( activate );
    m_debugPause->setEnabled( activate );
    m_debugRunToCursor->setEnabled( activate );
    m_debugStepOver->setEnabled( activate );
    m_debugStepInto->setEnabled( activate );
    m_debugStepOut->setEnabled( activate );
    m_debugRestart->setEnabled( activate );*/
}


void KontrollerLab::notifyDebuggerPaused( )
{
    //m_debugPause->setIcon( "player_play" );
    //m_debugPause->setText( i18n("Start") );
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


