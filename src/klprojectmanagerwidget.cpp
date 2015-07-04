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
#include "klprojectmanagerwidget.h"
#include "klproject.h"

#include <kiconloader.h>
#include <kglobal.h>
#include <klocale.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <kmessagebox.h>
#include "kontrollerlab.h"
#include "kldocumentview.h"

KLProjectManagerWidget::KLProjectManagerWidget(KLProject* project, QWidget *parent, const char *name)
    :QWidget(parent), ui(new Ui::KLProjectManagerWidgetBase)
{
    setObjectName(name);

    ui->setupUi(this);
    m_project = project;
    m_project->registerProjectManager( this );
    m_rootNode = m_sourcesNode = m_headersNode = m_notesNode = m_othersNode = 0L;
    m_iconLoader = KIconLoader::global();
    m_curHexSize = 0;
    setProjectName( project->name() );
    m_projectManagerPopup = new KMenu( "projectManagerPopup", this);
    KIconLoader kico;
    m_projectManagerPopup->addAction( kico.loadIcon( "quickopen-file", KIconLoader::Toolbar ), i18n("Create new view"),
                                       this, SLOT( slotCreateNewView() ) );
    m_projectManagerPopup->addAction( kico.loadIcon( "edit-delete", KIconLoader::Toolbar ), i18n("Remove from project"),
                                       this, SLOT( slotTrash() ) );

    ui->tbConfigure->setIcon(kico.loadIcon("configure", KIconLoader::Toolbar));
    ui->tbTrash->setIcon(kico.loadIcon("edit-delete", KIconLoader::Toolbar));


    ui->lvFiles->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->lvFiles, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(slotShowPopupMenu(const QPoint &)));
    
    connect( ui->lvFiles, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
               this, SLOT(slotDoubleClicked(QTreeWidgetItem*,int)));

    connect( ui->tbTrash, SIGNAL(clicked()), this, SLOT(slotTrash()));
    connect( ui->tbConfigure, SIGNAL(clicked()), this, SLOT(slotConfigure()));

    ui->lvFiles->setIndentation(10);

    ui->lvFiles->setColumnWidth(0,160);
    ui->lvFiles->setColumnWidth(1,50);
}

void KLProjectManagerWidget::slotDoubleClicked( QTreeWidgetItem* item, int column )
{
    Q_UNUSED(column)
    if ( !item )
        return;
    if ( item->type() == KLQListViewDocumentItem_RTTI )
    {
        KLQListViewDocumentItem *itemCast = (KLQListViewDocumentItem*) item;
        itemCast->doc()->makeLastActiveViewVisible();
        //qDebug( "%s", itemCast->doc()->name().ascii() );
    }
}

KLProjectManagerWidget::~ KLProjectManagerWidget( )
{
    m_project->unregisterProjectManager( this );
}

void KLProjectManagerWidget::addDocument( KLDocument * doc )
{
    KLQListViewDocumentItem *item;
    if ( doc->type() == KLDocType_Source )
    {
        item = new KLQListViewDocumentItem( getSourceParentFor( doc ), doc->name() );
        item->setIcon( 0, m_iconLoader->loadMimeTypeIcon( "text/x-csrc",KIconLoader::Desktop, KIconLoader::SizeSmall ) );
    }
    else if ( doc->type() == KLDocType_Header )
    {
        item = new KLQListViewDocumentItem( getHeaderParentFor( doc ), doc->name() );
        item->setIcon( 0, m_iconLoader->loadMimeTypeIcon( "text/x-chdr",KIconLoader::Desktop, KIconLoader::SizeSmall ) );
    }
    else if ( doc->type() == KLDocType_Note )
    {
        item = new KLQListViewDocumentItem( getNotesParentFor( doc ), doc->name() );
        item->setIcon( 0, m_iconLoader->loadMimeTypeIcon( "text/plain",KIconLoader::Desktop, KIconLoader::SizeSmall ) );
    }
    else
    {
        item = new KLQListViewDocumentItem( getOthersParentFor( doc ), doc->name() );
        item->setIcon( 0, m_iconLoader->loadMimeTypeIcon( "all/all",KIconLoader::Desktop, KIconLoader::SizeSmall ) );
    }
    item->setDoc( doc );
    item->updateFileInfo();
    // doc->setListViewItem( item );
}

void KLProjectManagerWidget::removeDocument( KLDocument * )
{
    update();
}

void KLProjectManagerWidget::update( )
{
    KLQListViewDocumentItem *item;
    QList< KLDocument*> documents = m_project->documents();

    clear();
    foreach (KLDocument *it ,documents )
    {
        if ( it->type() == KLDocType_Source )
        {
            item = new KLQListViewDocumentItem( getSourceParentFor( it ), it->name() );
            item->setIcon( 0, m_iconLoader->loadMimeTypeIcon( "text/x-csrc", KIconLoader::Desktop, KIconLoader::SizeSmall ) );
        }
        else if ( it->type() == KLDocType_Header )
        {
            item = new KLQListViewDocumentItem( getHeaderParentFor( it ), it->name() );
            item->setIcon( 0, m_iconLoader->loadMimeTypeIcon( "text/x-chdr", KIconLoader::Desktop, KIconLoader::SizeSmall ) );
        }
        else if ( it->type() == KLDocType_Note )
        {
            item = new KLQListViewDocumentItem( getNotesParentFor( it ), it->name() );
            item->setIcon( 0, m_iconLoader->loadMimeTypeIcon( "text/plain", KIconLoader::Desktop, KIconLoader::SizeSmall ) );
        }
        else
        {
            item = new KLQListViewDocumentItem( getOthersParentFor( it ), it->name() );
            item->setIcon( 0, m_iconLoader->loadMimeTypeIcon( "all/all", KIconLoader::Desktop, KIconLoader::SizeSmall ) );
        }
        // it->setListViewItem( item );
        item->setDoc( it );
        item->updateFileInfo();
    }
    m_rootNode->setExpanded( true );
}


void KLQListViewDocumentItem::updateFileInfo( )
{
    QString buf;
    if (m_doc)
    {
        if ( QFile::exists( m_doc->url().path() ) )
        {
            QFileInfo info( m_doc->url().path() );

            setText( 1, buf.setNum( info.size() ) );
            setText( 2, info.lastModified().toString() );
        }
    }
}

void KLProjectManagerWidget::update( KLDocument * doc )
{
    QTreeWidgetItemIterator it(m_rootNode);
    while (*it) {
        if ((*it)->type() == KLQListViewDocumentItem_RTTI &&
                (((KLQListViewDocumentItem*)*it)->doc() == doc) )
               break;
        ++it;
    }

    if ( *it && doc )
    {
        (*it)->setText( 0, doc->name() );
        ((KLQListViewDocumentItem*)*it)->updateFileInfo();
    }
}

KLQListViewDocumentItem::KLQListViewDocumentItem(QTreeWidgetItem *parent, const QString & name )
    : QTreeWidgetItem( parent, QStringList() << name, KLQListViewDocumentItem_RTTI )
{
    m_doc = 0L;
}

void KLProjectManagerWidget::slotConfigure()
{
    m_project->parent()->slotConfigProject();
}


void KLProjectManagerWidget::slotTrash()
{
    QTreeWidgetItem *item = ui->lvFiles->currentItem();
    if ( !item )
        return;
    if (item->type() == KLQListViewDocumentItem_RTTI)
    {
        KLQListViewDocumentItem *castItem = (KLQListViewDocumentItem*) item;
        int retVal = KMessageBox::questionYesNo( this, i18n( "Do you really want "
                                                             "to remove %1 from the project?" ).arg( castItem->doc()->name() ) );
        int saveRetVal = KMessageBox::No;
        if ( retVal == KMessageBox::Yes )
        {
            if ( castItem->doc()->kateDoc()->isModified() )
            {
                saveRetVal = KMessageBox::questionYesNoCancel( this, i18n( "The file %1 has been modified.\n"
                                                                           "Shall it be saved now?").arg( castItem->doc()->name() ) );
                if ( saveRetVal == KMessageBox::Yes )
                    castItem->doc()->save();
                else if ( retVal == KMessageBox::Cancel )
                    return;
            }
            // In case the file was not saved, the user maybe wants to delete it from disk:
            if ( KMessageBox::No == saveRetVal )
            {
                retVal = KMessageBox::questionYesNoCancel( this, i18n( "Do you want "
                                                                       "to remove %1 from disk too?" ).arg( castItem->doc()->name() ) );
                if ( retVal == KMessageBox::Yes )
                {
                    QFile file( castItem->doc()->url().path() );
                    file.remove();
                }
                else if ( retVal == KMessageBox::Cancel )
                    return;
            }
            m_project->removeDocument( castItem->doc() );
        }
        // m_project->update();
    }
}


void KLProjectManagerWidget::updateModified()
{
    QTreeWidgetItemIterator it(m_rootNode);
    while (*it) {
        if ((*it)->type() == KLQListViewDocumentItem_RTTI )
            ((KLQListViewDocumentItem*)*it)->updateModified();
        ++it;
    }
}

void KLQListViewDocumentItem::updateModified( )
{
    if ( !m_doc )
        return;
    if ( !m_doc->kateDoc() )
        return;
    if ( m_doc->kateDoc()->isModified() )
        setText( 0, m_doc->name() + " *" );
    else
        setText( 0, m_doc->name() );
}

void KLProjectManagerWidget::slotShowPopupMenu( const QPoint & point)
{
    QTreeWidgetItem *item = ui->lvFiles->itemAt(point);

    if ( !item )
        return;
    if ( item->type() == KLQListViewDocumentItem_RTTI )
        m_projectManagerPopup->popup( ui->lvFiles->mapToGlobal(point) );
}



void KLProjectManagerWidget::clear( )
{
    ui->lvFiles->clear();
    m_rootNode = new QTreeWidgetItem( ui->lvFiles, QStringList() << m_project->name() );
    delete m_sourcesNode;
    m_sourcesNode = 0L;
    delete m_headersNode;
    m_headersNode = 0L;
    delete m_notesNode;
    m_notesNode = 0L;
    delete m_othersNode;
    m_othersNode = 0L;
    m_rootNode->setExpanded( true );
}


QTreeWidgetItem * KLProjectManagerWidget::getDirectoryListViewItem(const KUrl & dir)
{
    QTreeWidgetItem* parent=0L;
    if ( m_project->projectBaseURL() == dir )
    {
        // Dir is the root
        return m_rootNode;
    }
    else if ( m_project->projectBaseURL() == KUrl( dir.directory( KUrl::ObeyTrailingSlash  ) ) )
    {
        // Root is the parent of dir
        parent = m_rootNode;
    }
    else if ( m_project->projectBaseURL().isParentOf( dir ) )
    {
        // The project directory is a parent dir -> dive in recursively
        parent = getDirectoryListViewItem( KUrl( dir.directory(KUrl::ObeyTrailingSlash ) ) );
    }

    QTreeWidgetItemIterator it(parent);
    while (*it) {
        if ((*it)->text( 0 ) == dir.fileName() )
            return *it;
        ++it;
    }

    // No matching item was found!
    // Create one as child of m_rootNode
    QTreeWidgetItem* newItem = new QTreeWidgetItem( parent, QStringList() << dir.fileName() );
    newItem->setIcon(0, m_iconLoader->loadIcon( "folder", KIconLoader::Small ) );
    return newItem;
}


QTreeWidgetItem * KLProjectManagerWidget::getParentNamedFor(KLDocument * doc, const QString & name)
{
    KUrl dUrl = doc->url();
    QTreeWidgetItem *lvi = getDirectoryListViewItem( KUrl( dUrl.directory( KUrl::ObeyTrailingSlash ) ) );
       
    QTreeWidgetItemIterator it(lvi);
    while (*it) {
        if ((*it)->text( 0 ) == name )
            return *it;
        ++it;
    }

    // Create a new item
    return new QTreeWidgetItem( lvi, QStringList() << name );
}


void KLProjectManagerWidget::slotCreateNewView()
{
    QTreeWidgetItem *item = ui->lvFiles->currentItem();
    if ( !item )
        return;
    if (item->type() == KLQListViewDocumentItem_RTTI)
    {
        KLQListViewDocumentItem *castItem = (KLQListViewDocumentItem*) item;
        new KLDocumentView( castItem->doc(), m_project->parent() );
        // m_project->update();
    }
}


