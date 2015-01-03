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
#include "kldocument.h"
#include <ktexteditor/document.h>
#include <ktexteditor/highlightinterface.h>
#include <knotification.h>
#include <qfile.h>
#include <klocale.h>
#include "kldocumentview.h"
#include "klproject.h"
#include "kontrollerlab.h"

#include <ktexteditor/cursor.h>

#include <ktexteditor/document.h>
#include <ktexteditor/view.h>

#include <ktexteditor/codecompletioninterface.h>
// #include <ktexteditor/sessionconfiginterface.h>
#include <ktexteditor/configinterface.h>
#include <ktexteditor/editor.h>
#include <ktexteditor/annotationinterface.h>
#include <ktexteditor/containerinterface.h>
#include <ktexteditor/editorchooser.h>


#include <ktexteditor/markinterface.h>
#include <ktexteditor/cursor.h>

#include <kconfiggroup.h>
#include <kmessagebox.h>
#include <kxmlguifactory.h>

KLDocument::KLDocument( KontrollerLab* parent ) : QObject(parent)
{
    m_doc = 0L;

    m_lastActiveView = 0L;
    m_type = KLDocType_NoType;

    // m_listViewItem = 0L;

    // The library was found, so create the Kate::Document
    KLibFactory* factory = KLibLoader::self()->factory("katepart");

    KTextEditor::Factory* kte_factory = qobject_cast<KTextEditor::Factory*>(factory);

    if(kte_factory) {
        // valid editor factory, it is possible to access the editor now
        m_editor = kte_factory->editor();

        //hide some advanced stuff from the user -> can be changed in "Configure editor" -> Power user mode
        m_editor->setSimpleMode(true);

        m_doc = m_editor->createDocument(NULL);
        // m_doc->setText("#include <io.h>\n");
        m_doc->setModified(false);

        /* object is of type KTextEditor::Document* or View* or Plugin*
        sessionConfigIf = qobject_cast<KTextEditor::SessionConfigInterface*>( m_doc );

        if( sessionConfigIf ) {

            KConfigGroup group( KSharedConfig::openConfig(), "KontrollerLab" );
            sessionConfigIf->readSessionConfig(group);
        }
        */
        markIf = dynamic_cast<KTextEditor::MarkInterface *>(m_doc);

        m_parent = parent;
        m_parent->newDocumentOpened( m_doc );


        if (markIf) {
            markIf->setMarkPixmap(KTextEditor::MarkInterface::markType07, SmallIcon("stop"));
            markIf->setMarkPixmap(KTextEditor::MarkInterface::markType02, SmallIcon("tools-report-bug"));
            markIf->setMarkDescription(KTextEditor::MarkInterface::markType02, i18n("Breakpoint"));
            markIf->setMarkPixmap(KTextEditor::MarkInterface::markType05, SmallIcon("debug_currentline"));
            markIf->setMarkDescription(KTextEditor::MarkInterface::markType08, i18n("Annotation"));
            markIf->setMarkPixmap(KTextEditor::MarkInterface::markType08, SmallIcon("stamp"));
            // This is allows user to set breakpoints and bookmarks by clicking or rightclicking on the icon border.
            markIf->setEditableMarks(KTextEditor::MarkInterface::markType01 + KTextEditor::MarkInterface::markType02);
        }
    }

    m_parent = parent;
    m_project = 0L;
    m_markedOnlyLine = 0;
}


KLDocument::~KLDocument()
{
    /*
    if (sessionConfigIf)
    {
        KConfigGroup group( KSharedConfig::openConfig(), "KontrollerLab" );
        sessionConfigIf->writeSessionConfig(group);
    }
    */

    foreach (KLDocumentView *it,m_registeredViews)
    {
        //m_parent->removeWindowFromMdi( it );
        // To avoid, that the doc view deletes the
        // KATE view when being removed, we set the view pointer
        // to 0:
        if ( m_parent->guiFactory()->clients().indexOf( it->view() ) >= 0 )
            m_parent->guiFactory()->removeClient( it->view() );
        delete it->view();
        it->setView( 0L );
        // To prevent that the view unregisters:
        it->setDocument( 0L );
        it->close();
        
        delete it;
    }
    m_registeredViews.clear();
    
    if (m_doc)
    {
        m_doc->closeUrl(false); //TODO: Workaround for a Kate bug. Remove when KDE < 3.2.0 support is dropped.

        //m_parent->slotPartWillGetDeleted( m_doc );
        // m_parent->slotActivePartChanged( 0L );
        delete m_doc;
        // delete m_editInt;
        // delete m_listViewItem;
    }
}

void KLDocument::activateCHighlighting( )
{
    if ( !m_doc )
        return;
    bool foundMode = false;

    foreach(const QString mode, m_doc->highlightingModes())
    {
        if ( !mode.compare("c",Qt::CaseInsensitive) )
        {
            m_doc->setHighlightingMode( mode );
            foundMode = true;
            break;
        }
    }
    if (!foundMode)
        qWarning( "No C highlight mode found in %s:%d", __FILE__, __LINE__ );
}


bool KLDocument::newFile( const KUrl& url )
{
    m_url = url;
    setWindowTitleOfAllViews( name() );
    m_type = typeForName( name() );
    if (m_project)
        m_project->update();

    return true;
}


bool KLDocument::open( const KUrl& url )
{
    if (!m_doc)
        return false;

    QString localFileName;
    
    if ( url.isLocalFile() )
        localFileName = url.path();
    else if (!KIO::NetAccess::download( url, localFileName, m_parent ))
    {
        KNotification::event( KNotification::Error, i18n("Could not download the file.") );
        return false;
    }

    m_doc->openUrl( KUrl( localFileName ) );
    
    KIO::NetAccess::removeTempFile(localFileName);

    // m_name = url.fileName();
    m_url = url;
    setWindowTitleOfAllViews( name() );
    m_type = typeForName( name() );
    

    if ( ( m_type == KLDocType_Source ) || ( m_type == KLDocType_Header ) )
        activateCHighlighting();

    if (m_project)
        m_project->update();

    return true;
}

void KLDocument::registerKLDocumentView( KLDocumentView * view )
{
    m_registeredViews.append( view );
    setWindowTitleOfAllViews( name() );
}

void KLDocument::setWindowTitleOfAllViews( const QString cap )
{
    int counter = 1;
    QString modified = "";
    if (isModified())
        modified = "*";

    foreach (KLDocumentView *it,m_registeredViews )
    {
        if ( m_registeredViews.count() > 1 )
            it->setWindowTitle(QString("%1 %2: %3").arg( cap ).arg(modified).arg( counter ) );
        else
            it->setWindowTitle( QString("%1 %2").arg( cap ).arg( modified ) );
        counter++;
    }
}

void KLDocument::unregisterKLDocumentView( KLDocumentView * view )
{
    m_registeredViews.remove( view );
    if ( view == m_lastActiveView )
        m_lastActiveView = 0L;
    setWindowTitleOfAllViews( name() );
}


bool KLDocument::save( )
{
    bool retVal = false;
    if (!m_doc)
        return false;

    // qDebug("&&&&& SAVING: %s", name().ascii());
    if ( m_doc->saveAs( m_url ) )
    {
        // m_unsaved = false;
        m_doc->setModified( false );
        m_project->update( this );
        retVal = true;
    }
    m_project->checkForModifiedFiles();
    setWindowTitleOfAllViews( name() );
    return retVal;
}

bool KLDocument::saveAs( const KUrl & url )
{
    bool retVal = false;
    if (!m_doc)
        return false;

    if ( url.isValid() && (!url.isEmpty()) )
    {
        if (m_doc->saveAs( url ))
        {
            // m_unsaved = false;
            m_doc->setModified( false );
            m_url = url;
            setWindowTitleOfAllViews( name() );
            m_project->update( this );
            retVal = true;
        }
        m_project->checkForModifiedFiles();
        setWindowTitleOfAllViews( name() );
    }
    return retVal;
}


void KLDocument::makeLastActiveViewVisible( )
{
    if (!m_doc)
        return;

    if (m_registeredViews.count())
    {
        KLDocumentView *vi = m_registeredViews.at( 0 );
        if ( m_registeredViews.indexOf( m_lastActiveView ) >= 0 )
            vi = m_registeredViews.at( m_registeredViews.indexOf( m_lastActiveView ) );
        vi->setFocus();
    }
    else
    {
        KLDocumentView *view = new KLDocumentView( this, m_parent );
        m_parent->emitActivePartChanged( m_doc, view );
    }
}


void KLDocument::setActiveView( KLDocumentView * view )
{
    // qDebug() << "ActiveView" << view->objectName();
    m_lastActiveView = view;
    if(m_project)   //the document is assigned to an project
        m_project->setActiveDocument( this );
}


void KLDocument::setCursorToLine( int lineNr )
{
    makeLastActiveViewVisible();
    if ( m_lastActiveView )
    {
        m_lastActiveView->setCursorToLine( lineNr );
    }
}

void KLDocument::updateModified( )
{
    setWindowTitleOfAllViews( name() );
    bool mod = m_doc->isModified();

    foreach ( KLDocumentView* it, m_registeredViews)
    {
        // qDebug() << it->objectName() << "modified?" << mod;
        it->setWindowTitle( mod ? name() + " *" : name() );
    }
}

KLDocumentType KLDocument::typeForName( const QString& name )
{
    KLDocumentType type_;
    if ( name.lower().endsWith( ".c" ) )
        type_ = KLDocType_Source;
    else if ( name.lower().endsWith( ".cc" ) )
        type_ = KLDocType_Source;
    else if ( name.lower().endsWith( ".s" ) )
        type_ = KLDocType_Source;
    else if ( name.lower().endsWith( ".cpp" ) )
        type_ = KLDocType_Source;
    else if ( name.lower().endsWith( ".h" ) )
        type_ = KLDocType_Header;
    else if ( name.lower().endsWith( ".txt" ) )
        type_ = KLDocType_Note;
    else
        type_ = KLDocType_NoType;
    return type_;
}

bool KLDocument::revert( )
{
    if (!m_doc)
        return false;

    m_doc->setModified( false );
    return open( m_url );
}


bool KLDocument::hasKTextEditorView( KTextEditor::View * view ) const
{
    QList<KLDocumentView*>list = m_registeredViews;

    foreach ( KLDocumentView* it, list)
    {
        if ( it->kateView() == view )
            return true;
    }
    return false;
}

void KLDocument::markOnlyLine( int lineNr, unsigned int markType )
{
    if (!m_doc)
        return;

    if ( !markIf )
        return;

    if ( m_markedOnlyLine > 0 )
        markIf->setMark( m_markedOnlyLine-1, markIf->mark( m_markedOnlyLine-1) & (~STEP_MARK) );
    if ( lineNr > 0 )
    {
        if (markType)
            markIf->setMark( lineNr-1, markIf->mark( lineNr-1 ) | STEP_MARK );
        else
            markIf->setMark( lineNr-1, markIf->mark( lineNr-1 ) & (~STEP_MARK) );
        if ( m_lastActiveView && m_lastActiveView->view() )
            m_lastActiveView->view()->setCursorPosition(KTextEditor::Cursor( lineNr-1, 0 ));
    }
    m_markedOnlyLine = lineNr;
}


void KLDocument::markBreakpoint( int lineNr, bool setMark )
{
    if (!m_doc)
        return;

    KTextEditor::Document* it = static_cast< KTextEditor::Document* >( m_doc );
    if ( !it )
        return;

    if ( lineNr > 0 )
    {
        if ( setMark )
            markIf->setMark( lineNr-1, markIf->mark( lineNr - 1 ) | BREAKPOINT_MARK );
        else
            markIf->setMark( lineNr-1, markIf->mark( lineNr - 1 ) & (~BREAKPOINT_MARK) );
    }
}


QString KLDocument::hierarchyName() const
{
    int cutTheFirst = 0;
    if ( m_project )
        cutTheFirst = m_project->projectBaseURL().path().length() + 1;
    QString relativePath = url().path();
    relativePath = relativePath.right( relativePath.length() - cutTheFirst );
    return relativePath;
}

