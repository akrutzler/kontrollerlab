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
#ifndef KLDOCUMENT_H
#define KLDOCUMENT_H
#include <QObject>

#include <kparts/factory.h> // KPart Factory
#include <klibloader.h>     // LibLoader, contains factories
#include <kurl.h>           // For opening files
#include <kio/netaccess.h>  // eventually via the network
#include <qlistview.h>
#include <ktexteditor/document.h>
#include <ktexteditor/editor.h>
#include <ktexteditor/markinterface.h>
#include <ktexteditor/factory.h>
#include <klibloader.h>

//#include <ktexteditor/markinterfaceextension.h>

#define BREAKPOINT_MARK 2
#define STEP_MARK 1

namespace KTextEditor
{
class CodeCompletionInterface;
class CompletionEntry;
class Document;
class MarkInterface;
class View;
class ViewCursorInterface;
class Mark;
}

class KLDocumentView;
class KLProject;
class KontrollerLab;


typedef enum {
    KLDocType_NoType,
    KLDocType_Source,
    KLDocType_Header,
    KLDocType_Note
} KLDocumentType;

/**
This is a class that stores a single document, e.g. a C source.

    @author Martin Strasser <strasser@kontrollerlab.org>
*/
class KLDocument : public QObject
{
    Q_OBJECT

public:
    KLDocument(KontrollerLab* parent );

    ~KLDocument();

    KTextEditor::Document* kateDoc() const { return m_doc; }
    bool newFile( const KUrl& url );
    bool open( const KUrl& url );
    bool save();
    bool saveAs( const KUrl& url );
    void activateCHighlighting();
    void makeLastActiveViewVisible();
    void registerKLDocumentView( KLDocumentView* view );
    void unregisterKLDocumentView( KLDocumentView* view );
    bool hasKTextEditorView( KTextEditor::View * view ) const;

    void setType(const KLDocumentType& theValue) { m_type = theValue; }
    KLDocumentType type() const { return m_type; }
    KLDocumentType typeForName( const QString& name );

    // void setName(const QString& theValue) { m_name = theValue; }
    QString name() const { return m_url.fileName(); }
    /**
     * Returns the name of the file in hierarchy.
     * For a file a.c which is in the subdirectory sub of the project,
     * it returns "sub/a.c"
     * @return The hierarchy name of the file.
     */
    QString hierarchyName() const;

    void setProject(KLProject* theValue) { m_project = theValue; }
    KLProject* project() const { return m_project; }

    // void setListViewItem(QListViewItem* theValue) { m_listViewItem = theValue; qDebug("%d: %d", (int)this, (int)theValue ); }
    // QListViewItem* listViewItem() const { return m_listViewItem; }

    void setUrl(const KUrl& theValue) { m_url = theValue; }
    KUrl url() const { return m_url; }
    
    void setActiveView( KLDocumentView* view );
    KLDocumentView* lastActiveView() const { return m_lastActiveView; }

    void setCursorToLine( int lineNr );
    void markOnlyLine( int lineNr, unsigned int markType );
    void markBreakpoint( int lineNr, bool setMark=true );

    KTextEditor::Editor* configIf;
    KTextEditor::MarkInterface* markIf;

    unsigned int registeredViewsCount() const { return m_registeredViews.count(); }
    QList< KLDocumentView* > registeredViews() { return m_registeredViews; }
    void updateModified();
    bool isModified() { return m_doc?m_doc->isModified():false; }
    //     void setUnsaved(bool theValue) { m_unsaved = theValue; }
    //     bool unsaved() const { return m_unsaved; }
    bool revert();

protected:
    void setWindowTitleOfAllViews( const QString cap );

    int m_markedOnlyLine;
    KTextEditor::Document *m_doc;
    KontrollerLab* m_parent;
    QList< KLDocumentView* > m_registeredViews;
    KLDocumentView *m_lastActiveView;
    KLDocumentType m_type;
    KLProject* m_project;
    // QString m_name;
    KUrl m_url;

    //bool m_unsaved;
    // QListViewItem* m_listViewItem;
private slots:
    void slotModified(KTextEditor::Document* doc );
    void slotTabChanged(KLDocumentView *tab);
};

#endif
