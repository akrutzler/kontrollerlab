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
#include "klfilenewdialog.h"
#include "kontrollerlab.h"
#include "klproject.h"
#include "kldocument.h"
#include "kldocumentview.h"
#include <qlineedit.h>
#include <kurl.h>
#include <q3iconview.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kglobal.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <kiconloader.h>


#define PROJECT_ROOT_STRING i18n("project root")


/**
 * Builds the "file->new" dialog.
 * @param parent The KontrollerLab widget is was called by.
 * @param project The project to add the file to.
 * @param name The name of the dialog.
 */
KLFileNewDialog::KLFileNewDialog(KontrollerLab *parent, KLProject* project, const char *name)
    :QDialog(parent, name), ui(new Ui_KLFileNewDialogBase)
{
    ui->setupUi(this);
    m_parent = parent;
    m_project = project;
    //TODO:  QListView::setViewMode(QListView::IconMode)
    KIconLoader *iconLoader = KIconLoader::global();

    m_cSource = new Q3IconViewItem( ui->klNewFile, i18n("C source"), iconLoader->loadMimeTypeIcon( "text/x-csrc",KIconLoader::Desktop, KIconLoader::SizeLarge ));
    m_asmSource = new Q3IconViewItem( ui->klNewFile, i18n("ASM source"), iconLoader->loadMimeTypeIcon( "text/x-hex",KIconLoader::Desktop, KIconLoader::SizeLarge ));
    m_cHeader = new Q3IconViewItem( ui->klNewFile, i18n("C header"), iconLoader->loadMimeTypeIcon( "text/x-chdr",KIconLoader::Desktop, KIconLoader::SizeLarge ));
    m_txt = new Q3IconViewItem( ui->klNewFile, i18n("Text"), iconLoader->loadMimeTypeIcon( "text/plain",KIconLoader::Desktop, KIconLoader::SizeLarge ));
    m_other = new Q3IconViewItem( ui->klNewFile, i18n("Other"), iconLoader->loadMimeTypeIcon( "all/all",KIconLoader::Desktop, KIconLoader::SizeLarge ));
    if (m_project)
        m_targetDirectory = m_project->projectBaseURL();
    ui->tlDirectory->setText( PROJECT_ROOT_STRING );
}


/**
 * Closes the dialog without any further warning.
 */
void KLFileNewDialog::slotCancel()
{
    close();
}


/**
 * Called when the user presses the OK button.
 * If the name is ok, a document is created.
 * We also check if the suffix for the chosen type
 * matches the file name. If not, we correct this.
 * Afterwards, the dialog window is closed.
 */
void KLFileNewDialog::slotOK()
{
    if (ui->leName->text().isEmpty() || ui->leName->text().isNull())
        return;

    KLDocument* doc = new KLDocument(m_parent);
    KUrl newFileURL = m_targetDirectory;
    QString fname = ui->leName->text();

    if ( ui->klNewFile->currentItem() == m_cSource )
    {
        fname = fname.lower().endsWith(".c") ? fname : fname + ".c";
    }
    else if ( ui->klNewFile->currentItem() == m_asmSource )
    {
        fname = fname.lower().endsWith(".s") ? fname : fname + ".s";
    }
    else if ( ui->klNewFile->currentItem() == m_cHeader )
    {
        fname = fname.lower().endsWith(".h") ? fname : fname + ".h";
    }
    else if ( ui->klNewFile->currentItem() == m_txt )
    {
        fname = fname.lower().endsWith(".txt") ? fname : fname + ".txt";
    }

    newFileURL.addPath( fname );

    if ( !newFileURL.isValid() )
        return;
    doc->newFile( newFileURL );
    doc->kateDoc()->setModified( true );
    m_project->addDocument( doc );

    m_parent->slotNewPart( doc->kateDoc(), true );
    // KLDocumentView *view = 
    new KLDocumentView( doc, m_parent );

    close();
}


/**
 * This slot is called when the file name is changed.
 * If the name is not at least one character long,
 * the OK button is disabled.
 * @param fn The currently chosen filename
 */
void KLFileNewDialog::slotFileNameChanged( const QString& fn )
{
    ui->pbOK->setEnabled( fn.length() > 0 ? true: false );
}


/**
 * This slot is called when the user wants to choose a
 * target directory of the new file. Here, we open a
 * directory chooser dialog and wait for it to be closed.
 * After that, we check if the directory is a subdirectory
 * of the project folder. If not, the target is set to
 * be the project folder. We also update the label
 * tlDirectory here and set the caption showing the
 * selected directory.
 */
void KLFileNewDialog::slotChoose()
{
    if ( m_project )
    {
        m_targetDirectory = KFileDialog::getExistingDirectory( m_targetDirectory.path(), m_parent, i18n( "Select target directory" ) );
        if ( (! m_project->projectBaseURL().isParentOf( m_targetDirectory ) ) ||
                ( m_project->projectBaseURL() == m_targetDirectory ) )
        {
            m_targetDirectory = m_project->projectBaseURL();
            ui->tlDirectory->setText( PROJECT_ROOT_STRING );
        }
        else
            ui->tlDirectory->setText( m_project->getHierarchyName( m_targetDirectory ) );
    }
    else
    {
        m_targetDirectory = KFileDialog::getExistingDirectory( KUrl("."), m_parent, i18n( "Select target directory" ) );
        ui->tlDirectory->setText( m_targetDirectory.path() );
    }
}


