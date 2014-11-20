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
#ifndef KLDOCUMENTVIEW_H
#define KLDOCUMENTVIEW_H

#include <kparts/factory.h> // KPart Factory
#include <klibloader.h>     // LibLoader, contains factories
#include <ktexteditor/codecompletioninterface.h>
#include <ktexteditor/cursor.h>
#include <qwidget.h>
#include "kontrollerlab.h"
#include <qobject.h>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QCloseEvent>
#include <klocale.h>




class KLDocument;

/**
This is a view for a KLDocument

	@author Martin Strasser <strasser@kontrollerlab.org>
*/
class KLDocumentView : public QMdiSubWindow{
    Q_OBJECT
public:
    KLDocumentView( KLDocument *doc, KontrollerLab* parent ) ;

    virtual ~KLDocumentView();

    KTextEditor::View* kateView() const { return m_view; }

    KTextEditor::CodeCompletionInterface *codeCompletionIf;
    KontrollerLab* parent() const { return m_parent; }

    void setView(KTextEditor::View* theValue) { m_view = theValue; }
    KTextEditor::View* view() const { return m_view; }
    /** Called when this view become the active one */
    void activated();

    void setDocument(KLDocument* theValue) { m_document = theValue; }
    KLDocument* document() const { return m_document; }

    void setCursorToLine( int lineNr );

protected:
    virtual void closeEvent( QCloseEvent* e );

protected slots:
    void mdiViewActivated( QMdiSubWindow* view );
    void mdiViewActivated( );

    void slotCheckForModifiedFiles();

protected:
    KLDocument *m_document;
    KontrollerLab* m_parent;
    KTextEditor::View *m_view;
    bool m_inhibitFocusRecursion;
};

#endif
