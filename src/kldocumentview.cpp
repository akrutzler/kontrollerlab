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
#include "kldocumentview.h"
#include "kldocument.h"
#include <qlayout.h>
#include "klproject.h"
#include <kxmlguifactory.h>
#include <kactioncollection.h>
#include <kactionmenu.h>

#include <ktexteditor/cursor.h>
#include <kmessagebox.h>



KLDocumentView::KLDocumentView( KLDocument *doc, KontrollerLab* parent ) : QMdiSubWindow( parent)
{
    // Store the document for this view:
    setObjectName(doc->name());
    //setWindowState(Qt::WindowMaximized);
    setAttribute(Qt::WA_DeleteOnClose);

    m_document = doc;
    m_parent = parent;

    // The document only represents the document, to view
    // the document's content
    // we have to create a view for the document.

    m_view = (KTextEditor::View *) doc->kateDoc()->createView( this);

    setWidget(m_view);

    parent->m_mdiArea->addSubWindow( this );
    setWindowTitle(doc->name());

    // remove the unwanted actions

    QAction *a = m_view->actionCollection()->action( "file_export" );

    if (a)
        m_view->actionCollection()->removeAction(a);

    a =  m_view->actionCollection()->action( "file_save" );
    if (a)
        m_view->actionCollection()->removeAction(a);

    a = m_view->actionCollection()->action( "file_save_as" );
    if (a)
        m_view->actionCollection()->removeAction(a);
    
    a = m_view->actionCollection()->action( "file_reload" );
    
    if (a)
        connect( a, SIGNAL(activated()), this, SLOT(slotCheckForModifiedFiles()) );
    // m_view->actionCollection()->take(a);
    
    a = m_view->actionCollection()->action( "edit_undo" );
    if (a)
        connect( a, SIGNAL(activated()), this, SLOT(slotCheckForModifiedFiles()) );
    //     m_view->actionCollection()->take(a);
    a = m_view->actionCollection()->action( "edit_redo" );
    if (a)
        connect( a, SIGNAL(activated()), this, SLOT(slotCheckForModifiedFiles()) );
    // m_view->actionCollection()->take(a);
    /*
    //because they are not implemented in VPL
    
    a = m_view->actionCollection()->action( "edit_copy" );
    if (a)
        m_view->actionCollection()->take(a);
    a = m_view->actionCollection()->action( "edit_cut" );
    if (a)
        m_view->actionCollection()->take(a);
    a = m_view->actionCollection()->action( "edit_paste" );
    if (a)
        m_view->actionCollection()->take(a);
    */
    KActionMenu *bookmarkAction = dynamic_cast<KActionMenu*>(m_view->actionCollection()->action( "bookmarks" ));
    if (bookmarkAction)
    {
        m_view->actionCollection()->removeAction(bookmarkAction);
        //kdDebug(24000) << "Bookmarks found!" << endl;
        //bookmarkAction->insert(quantaApp->actionCollection()->action( "file_quit" ));
    }
    //    viewCursorIf = dynamic_cast<KTextEditor::ViewCursorInterface *>(m_view);
    codeCompletionIf = dynamic_cast<KTextEditor::CodeCompletionInterface *>(m_view);
    
    /* KTextEditor::PopupMenuInterface* popupIf = dynamic_cast<KTextEditor::PopupMenuInterface*>(m_view);
    if (popupIf)
    {
        QPopupMenu *thePopup = (QPopupMenu*)parent->factory()->container("ktexteditor_popup", parent);
        if ( !m_parent->debugToggleBreakpoint()->isPlugged( thePopup ) )
            m_parent->debugToggleBreakpoint()->plug( thePopup );
        popupIf->installPopup ( thePopup );
    }*/
    setFocusProxy( m_view );
    m_view->setFocusPolicy(Qt::WheelFocus);
    m_parent->slotNewPart(doc->kateDoc(), true);
    // add the view's XML GUI Client
    if ( !m_parent->kateGuiClientAdded() )
    {
        m_parent->guiFactory()->addClient( m_view );
        m_parent->setKateGuiClientAdded( (KXMLGUIClient*) m_view );
    }

    doc->registerKLDocumentView( this );
    //QGridLayout *m_layout = new QGridLayout( this, 1, 1 );
    //m_layout->addWidget( m_view, 1, 1 );
    m_view->show();
    show();
    //activate();
    connect( this,SIGNAL(aboutToActivate()),this,SLOT(mdiViewActivated()));
    connect( this, SIGNAL( gotFocus( KMdiChildView* ) ),
             this, SLOT( mdiViewActivated( KMdiChildView* ) ) );
    // connect( m_mdiView, SIGNAL( activated( KMdiChildView* ) ),
    //          this, SLOT( mdiViewActivated( KMdiChildView* ) ) );
    connect( m_view, SIGNAL( gotFocus( Kate::View* ) ) , this, SLOT( mdiViewActivated() ) );
    // connect( m_document->kateDoc(), SIGNAL(editorGotFocus()), this, SLOT( mdiViewActivated() ) );
    m_inhibitFocusRecursion = false;


    //m_parent->m_editorWidget->addDocumentView(this);
}


KLDocumentView::~KLDocumentView()
{
    qDebug() << "removing" << this;

    if ( m_parent->oldKTextEditor() == m_view )
    {
        if ( m_parent->guiFactory()->clients().indexOf( m_view ) >= 0 )
        {
            // qDebug("REMOVE: %d", m_oldKTextEditor);
            m_parent->guiFactory()->removeClient( m_view );
        }
        m_parent->setOldKTextEditor( 0L );
    }
    
    if (m_document)
        m_document->unregisterKLDocumentView( this );
    if ( m_parent->kateGuiClientAdded() )
    {
        m_parent->guiFactory()->removeClient( m_parent->kateGuiClientAdded() );
        m_parent->setKateGuiClientAdded( 0L );
    }

    delete m_view;
}


void KLDocumentView::mdiViewActivated(QMdiSubWindow * )
{
    if ( m_document )
    {
        m_parent->partManager()->setActivePart(m_document->kateDoc(), m_view);
        m_document->setActiveView( this );
    }
}

void KLDocumentView::mdiViewActivated( )
{
    if ( m_inhibitFocusRecursion )
        return;

    m_inhibitFocusRecursion = true;
    setFocus();
    m_view->setFocus();
    m_inhibitFocusRecursion = false;

    if ( m_document )
    {
        m_parent->partManager()->setActivePart(m_document->kateDoc(), m_view);
        m_document->setActiveView( this );
    }
}


void KLDocumentView::activated( )
{
    // if ( m_document )
    // m_parent->m_mdiArea->setActiveSubWindow();
}


void KLDocumentView::setCursorToLine( int lineNr )
{
    m_view->setCursorPosition(KTextEditor::Cursor( lineNr-1, 0 ));
}

void KLDocumentView::closeEvent(QCloseEvent *e)
{
    if ( !m_document )
    {
        m_parent->setOldKTextEditor( m_view );
        e->accept();
    }
    else if ( ( !m_document->isModified() ) ||
              ( m_document->kateDoc() &&
                ( m_document->kateDoc()->views().count() > 1 ) ) )
    {
        m_parent->setOldKTextEditor( m_view );
        e->accept();
    }
    else
    {
        int retVal = KMessageBox::questionYesNoCancel( this,
                                                       i18n("Do you want to save the document before closing?"),
                                                       i18n("Close document") );

        if ( retVal == KMessageBox::No )
        {
            m_parent->setOldKTextEditor( m_view );
            m_document->revert();
            e->accept();
        }
        else if ( retVal == KMessageBox::Yes )
        {
            if (m_document->save())
            {
                m_parent->setOldKTextEditor( m_view );
                e->accept();
            }
        }
        else if ( retVal == KMessageBox::Cancel )
            e->ignore();
    }
}


void KLDocumentView::slotCheckForModifiedFiles()
{
    m_document->project()->checkForModifiedFiles();
}







