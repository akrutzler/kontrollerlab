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
#include "klproject.h"
#include "klprojectmanagerwidget.h"
#include <klocale.h>
#include <kglobal.h>
#include "klprogrammeruisp.h"
#include "kontrollerlab.h"

#include <kfiledialog.h>
#include <kmessagebox.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kinputdialog.h>
#include "kldocumentview.h"

#include "klcompilerinterface.h"
#include "klavrgcccompiler.h"
#include "kldebugger.h"
#include <kstandarddirs.h>

#define NO_RAM 0
#define RAM 1

#define STATE_IS_MAXIMIZED 1
#define STATE_IS_MINIMIZED 2
#define STATE_IS_ATTACHED 4

#include "klmemoryviewwidget.h"

KLProject::KLProject( KontrollerLab* parent )
{
    // The project takes care of the pointers.
    m_currentlyOpeningProject = false;
    m_parent = parent;
    // m_projectManager = 0L;
    // m_projectManagerInConfigWdg = 0L;
    m_name = i18n("Unnamed project");
    // m_sourcesNode = m_headersNode = m_notesNode = m_othersNode = 0L;
    recreateListView();
    m_programmerInterface = 0L;
    m_initialized = false;
    m_unsaved = false;
    m_programmerBusy = false;
    m_currentHighlightLine = -1;

    // READ THE CONFIGURATION:

    KStandardDirs *dirs = KGlobal::dirs();

    QString resPathConf = dirs->findResource( "data", "kontrollerlab/cpu_config.xml" );

    // Open it:
    if ( resPathConf.isNull() || resPathConf.isEmpty() )
    {
        qWarning( "Could not locate %s", "cpu_config.xml" );
    }
    else
    {
        KUrl urlConf( resPathConf );
        QFile qfile( urlConf.path() );
        QDomDocument docConf("CPU_CONFIG");
        QString errorMesg;
        int errorLine, errorCol;
        if ( !docConf.setContent( &qfile, false, &errorMesg, &errorLine, &errorCol ) ) {
            qWarning( "Error in file %s: %d(%d) %s", "config", errorLine, errorCol, errorMesg.ascii() );
            qfile.close();
            return;
        }
        qfile.close();
        QDomElement docElem = docConf.documentElement();
        m_cpuFeatures.readFromDOMElement( docConf, docElem );
    }
    
    //qSort(m_cpuFeatures);
    //qHeapSort( m_cpuFeatures );
    QList<KLCPUFeatures>::iterator it;
    for (it=m_cpuFeatures.begin(); it!=m_cpuFeatures.end(); ++it)
        m_cpus.append((*it).name());

    m_compilerInterface = (KLCompilerInterface*) new KLAVRGCCCompiler( this, m_parent->procManager() );
}


KLProject::~KLProject()
{

}


void KLProject::addDocument( KLDocument * doc, bool forceCopyToProjectDir )
{
    if (!initialized())
        return;
    if ( m_documents.indexOf( doc ) < 0 )
    {
        m_unsaved=true;
        m_documents.append( doc );
        doc->setProject( this );
        if (!m_projectBaseURL.isParentOf( doc->url() ))
        {
            if ( !forceCopyToProjectDir )
                KMessageBox::information( m_parent,
                                          i18n("The file is not in your " \
                                               "project base directory. It will be copied " \
                                               "to it."),
                                          i18n("File not relative to project base directory"),
                                          "file_not_relative_to_base_dir" );
            KUrl old = doc->url();
            KUrl newOne = m_projectBaseURL;
            newOne.addPath( old.fileName() );
            bool ok = false;
            int i = 1;
            while (!ok)
            {
                if ( QFile::exists( newOne.path() ) )
                {
                    newOne = m_projectBaseURL;
                    newOne.addPath( QString("copy_%1_%2").arg(i).arg(old.fileName()) );
                }
                else
                    ok = true;
                i++;
            }
            doc->setUrl( newOne );
            doc->save();
            doc->kateDoc()->setModified( false );
        }
        KLProjectManagerWidget* it;
        QListIterator<KLProjectManagerWidget *> i(m_projectManagerWidgets);

        while (i.hasNext() )
        {
            it = i.next();
            it->addDocument( doc );
        }
        m_parent->beAwareOfChangesOf( doc );
    }
    else
        qWarning( "Tried to append a document pointer twice." );

    // recreateListView();
}


void KLProject::removeDocument( KLDocument * doc )
{
    if ( m_documents.indexOf( doc ) >= 0 )
    {
        m_documents.remove( doc );
        m_unsaved=true;
        update();
    }
}


QList<KLDocument *> KLProject::getAllDocsOfType( const KLDocumentType & type )
{
    QList< KLDocument* > retVal;

    foreach (KLDocument* it,m_documents)
    {
        if ( it->type() == type )
            retVal.append( it );
    }
    return retVal;
}


void KLProject::recreateListView( )
{
    foreach (KLProjectManagerWidget* it,m_projectManagerWidgets)
        it->update();
}


void KLProject::recreateListItemFor( KLDocument * doc )
{
    foreach (KLProjectManagerWidget* it,m_projectManagerWidgets)
        it->update( doc );
}


void KLProject::compile( )
{
    if ( activeDocument() )
    {
        m_parent->clearMessages();
        m_parent->procManager()->setFinishMessage( i18n("File compiled successfully.") );
        m_compilerInterface->compile( activeDocument() );
    }
}


void KLProject::erase()
{
    m_programmerInterface->configuration() = m_parent->programmerConfig();
    m_parent->procManager()->setFinishMessage( i18n("Chip successfully erased.") );
    m_programmerInterface->erase();
}


bool KLProject::upload( const KUrl & url )
{
    bool retVal = false;
    m_programmerInterface->configuration() = m_parent->programmerConfig();
    QString selCpu = KInputDialog::getItem( i18n("Select CPU"),
                                            i18n("Please select the type of CPU to upload to."), m_cpus, cpuIndex(), false );
    if ( (!selCpu.isNull()) && (!selCpu.isEmpty()) )
    {
        QString oldCpu = cpu();
        setCpu( selCpu );
        m_parent->procManager()->setFinishMessage( i18n("File uploaded successfully.") );
        retVal = m_programmerInterface->upload( url );
        setCpu( oldCpu );
    }
    return retVal;
}


bool KLProject::verify()
{
    m_programmerInterface->configuration() = m_parent->programmerConfig();
    m_parent->procManager()->setFinishMessage( i18n("Project verified successfully.") );
    return m_programmerInterface->verify( getHEXURL() );
}


bool KLProject::download( const KUrl & url )
{
    bool retVal = false;
    m_programmerInterface->configuration() = m_parent->programmerConfig();
    QString selCpu = KInputDialog::getItem( i18n("Select CPU"),
                                            i18n("Please select the type of CPU to download from."), m_cpus, cpuIndex(), false );
    if ( (!selCpu.isNull()) && (!selCpu.isEmpty()) )
    {
        QString oldCpu = cpu();
        setCpu( selCpu );
        m_parent->procManager()->setFinishMessage( i18n("File downloaded successfully.") );
        retVal = m_programmerInterface->download( url );
        setCpu( oldCpu );
    }
    return retVal;
}


bool KLProject::ignite()
{
    if ( m_projectFileURL.isValid() )
        save( m_projectFileURL );
    m_programmerInterface->configuration() = m_parent->programmerConfig();
    buildAll();
    m_parent->procManager()->setFinishMessage( i18n("Project built and uploaded successfully.") );
    m_programmerInterface->ignite( getHEXURL() );
    return true;
}


void KLProject::buildAndDownloadExternal( const QString & outHexFileName,
                                          const QString & projectBuildDir,
                                          const QStringList & sources )
{
    m_programmerInterface->configuration() = m_parent->programmerConfig();
    m_compilerInterface->buildExternal( outHexFileName, projectBuildDir, sources );
    m_parent->procManager()->setFinishMessage( i18n("Monitor built and uploaded successfully.") );
    m_programmerInterface->ignite( KUrl( projectBuildDir, getHEXFileName() ) );
}


bool KLProject::programFuses( QMap< QString, QString > fuses, const QString mmcu )
{
    bool retVal = false;
    m_programmerInterface->configuration() = m_parent->programmerConfig();
    QString selCpu = mmcu;
    if ( selCpu.isEmpty() || selCpu.isNull() )
        selCpu = KInputDialog::getItem( i18n("Select CPU"),
                                        i18n("Please select the type of CPU to write the fuses."), m_cpus, cpuIndex(), false );
    if ( (!selCpu.isNull()) && (!selCpu.isEmpty()) )
    {
        QString oldCpu = cpu();
        setCpu( selCpu );
        m_parent->procManager()->setFinishMessage( i18n("Successfully programmed the fuse bits.") );
        retVal = m_programmerInterface->programFuses( fuses );
        setCpu( oldCpu );
    }

    return retVal;
}


bool KLProject::readFuses( const QString mmcu )
{
    bool retVal = false;
    m_programmerInterface->configuration() = m_parent->programmerConfig();
    QString selCpu = mmcu;
    if ( selCpu.isEmpty() || selCpu.isNull() )
        selCpu = KInputDialog::getItem( i18n("Select CPU"),
                                        i18n("Please select the type of CPU to read the fuses from."), m_cpus, cpuIndex(), false );
    if ( (!selCpu.isNull()) && (!selCpu.isEmpty()) )
    {
        QString oldCpu = cpu();
        setCpu( selCpu );
        m_parent->procManager()->setFinishMessage( i18n("Successfully read the fuse bits.") );
        retVal = m_programmerInterface->readFuses();
        setCpu( oldCpu );
    }

    return retVal;
}


bool KLProject::upload( )
{
    m_parent->procManager()->setFinishMessage( i18n("File successfully uploaded.") );
    m_programmerInterface->configuration() = m_parent->programmerConfig();
    return m_programmerInterface->upload( getHEXURL() );
}

bool KLProject::hexFileIsUpToDate( )
{
    buildAll();
    return true;
}


void KLProject::registerProjectManager( KLProjectManagerWidget * prMan )
{
    if ( m_projectManagerWidgets.indexOf( prMan ) < 0 )
        m_projectManagerWidgets.append( prMan );
    else
        qWarning(i18n("Tried to append a project manager twice: %s.").ascii(), prMan->name());
}


void KLProject::unregisterProjectManager( KLProjectManagerWidget * prMan )
{
    if ( m_projectManagerWidgets.indexOf( prMan ) >= 0 )
        m_projectManagerWidgets.remove( prMan );
    else
        qWarning(i18n("Tried to remove a project manager twice: %s"), prMan->name());
}

KLCPUFeatures KLProject::cpuFeaturesFor( const QString & cpu )
{
    QList<KLCPUFeatures>::iterator it;
    for (it=m_cpuFeatures.begin(); it!=m_cpuFeatures.end(); ++it)
        if ((*it).name()==cpu)
            return (*it);
    return KLCPUFeatures();
}

void KLProject::initialize( const KUrl& url )
{
    if ( url.isEmpty() || (!url.isValid()) )
        m_projectBaseURL = KFileDialog::getExistingDirectory( KUrl("."), m_parent, i18n( "Select project base directory" ) );
    else
        m_projectBaseURL = url;

    if ( m_projectBaseURL.isValid() &&
         (!m_projectBaseURL.isEmpty()) &&
         m_projectBaseURL.isLocalFile() )
    {
        m_initialized = true;
        m_documents.clear();

        m_name = m_projectBaseURL.fileName();

        foreach(KLProjectManagerWidget *it, m_projectManagerWidgets)
            it->setProjectName( m_projectBaseURL.fileName() );
    }
    else
        m_initialized = false;
    m_unsaved=true;
}


QString KLProject::attribute( const QString & attr, const QString & confKey, bool ifTrueAttribOnly ) const
{
    QString retVal;
    if ( m_settings.contains( confKey ) )
    {
        retVal = m_settings[ confKey ];
        if ( (!retVal.isNull()) && (!retVal.isEmpty()) )
        {
            if ( ifTrueAttribOnly && (retVal == TRUE_STRING) )
                return " " + attr;
            else if (!ifTrueAttribOnly)
                return " " + attr + retVal;
        }
    }
    return "";
}


QString KLProject::attributeLinkerSection( const QString & attr,
                                           const QString & confKey ) const
{
    QString retVal;
    int intRetVal;
    bool ok;

    if ( m_settings.contains( confKey ) )
    {
        retVal = m_settings[ confKey ];
        intRetVal = retVal.toInt( &ok, 16 );
        if ( (!retVal.isNull()) && (!retVal.isEmpty()) && ok )
        {
            return " " + attr + retVal.setNum( (ulong) (0x800000 + intRetVal) , 16 );
        }
    }
    return "";
}


QString KLProject::getObjectFileNameFor( const KUrl & file )
{
    QString oFileName = getHierarchyName( file );
    oFileName = oFileName.left( oFileName.find( "." ) ) + ".o";
    return oFileName;
}

QString KLProject::conf( const QString & confKey, const QString & defval ) const
{
    if ( m_settings.find( confKey ) != m_settings.end() )
    {
        return m_settings[ confKey ];
    }
    else
    {
        KConfigGroup group( KSharedConfig::openConfig(), "KontrollerLab" );

        QString val = group.readEntry( confKey, "" );
        // qDebug("%s", val.ascii() );
        if ( (!val.isEmpty()) && (!val.isNull()) )
            return val;
        else
            return defval;
    }
}

QString KLProject::getOUTFileName( )
{
    QString buf = conf( PRJ_HEX_FILE, "project.hex" );
    buf = buf.left( buf.find( "." ) ) + ".out";
    return buf;
}

void KLProject::createDOMElement( QDomDocument & document, QDomElement & parent )
{
    QDomElement settings = document.createElement( "SETTINGS" );
    fromQMapToDOM( document, settings, m_settings );
    QDomElement dbg_settings = document.createElement( "DEBUGGER_SETTINGS" );
    fromQMapToDOM( document, dbg_settings, m_debuggerSettings );
    QDomElement programmerConfig = document.createElement( "PROGRAMMERCONFIG" );
    fromQMapToDOM( document, programmerConfig, m_parent->programmerConfig() );
    
    QDomElement files = document.createElement( "FILES" );
    KLDocument* it;
    foreach (KLDocument *it, m_documents)
    {
        QDomElement curfile = document.createElement( "FILE" );
        QString relativePath = it->url().path();
        curfile.setAttribute( "NAME", it->hierarchyName() );
        curfile.setAttribute( "SHOWN", it->registeredViewsCount()>0 ? TRUE_STRING : FALSE_STRING );

        // To store the position and number of registered views
        QList< KLDocumentView*> views = it->registeredViews();
        QString positions = "";

        foreach(KLDocumentView *itView,views)
        {
            //itView = i.next();
            // if ( !itView->isAttached() )
            //     continue;
            QRect geo;
            //if (itView->isAttached())
            //geo = itView->internalGeometry();
            //else
            geo = itView->geometry();
            int state = 0;
            if ( itView->isMaximized() )
                state = STATE_IS_MAXIMIZED;
            else if ( itView->isMinimized() )
                state = STATE_IS_MINIMIZED;
            //if ( itView->isAttached() )
            state |= STATE_IS_ATTACHED;
            positions += QString("%1,%2,%3,%4,%5,")
                    .arg(geo.x())
                    .arg(geo.y())
                    .arg(geo.width())
                    .arg(geo.height())
                    .arg(state);
        }
        if ( ! ( positions.isEmpty() || positions.isNull() ) )
            curfile.setAttribute( "VIEWS", positions );
        files.appendChild( curfile );
    }

    parent.appendChild( files );
    parent.appendChild( settings );
    parent.appendChild( dbg_settings );
    parent.appendChild( programmerConfig );
}

void KLProject::fromQMapToDOM( QDomDocument & document, QDomElement & parent,
                               QMap<QString, QString> map )
{
    QMap<QString, QString>::Iterator it;
    for (it=map.begin(); it!=map.end(); ++it)
    {
        QDomElement elem = document.createElement( it.key().upper() );
        elem.setAttribute( "VALUE", it.data() );
        parent.appendChild( elem );
    }
}

QMap< QString, QString > KLProject::fromDOMToQMap( QDomDocument &, QDomElement & element )
{
    QMap< QString, QString > retVal;

    QDomNode n = element.firstChild();
    while( !n.isNull() )
    {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if( !e.isNull() )
        {
            retVal[ e.tagName().upper() ] = e.attribute( "VALUE", "" );
            // qDebug("retVal[ %s ] = %s", e.tagName().upper().ascii(), retVal[ e.tagName().upper() ].ascii() );
        }
        n = n.nextSibling();
    }
    return retVal;
}

void KLProject::save( const KUrl & url )
{
    if ( url.isLocalFile() )
    {
        m_projectFileURL = url;
        m_name = m_projectBaseURL.fileName();
        QDomDocument doc("KontrollerLab");
        QDomElement root = doc.createElement( "PROJECT" );
        root.setAttribute( "VERSION", "1.0" );
        doc.appendChild( root );

        createDOMElement( doc, root );

        QFileInfo finfo( url.path() );
        QFile file( url.path() );

        if ( finfo.exists() )
            file.remove();
        file.open( IO_ReadWrite );
        QString xml = doc.toString( 2 );
        QByteArray ba;
        ba.setRawData( xml.ascii(), xml.length() );
        QTextStream stream( &file );
        stream << xml;
        ba.resetRawData( xml.ascii(), xml.length() );
        file.close();
        saveAllDocs();
        m_unsaved=false;
        checkForModifiedFiles();
        update();
    }
}


void KLProject::open( const KUrl & url )
{
    QString localfilename;
    QString fname = url.fileName();
    
    m_currentlyOpeningProject = true;
    if (url.isLocalFile())
        localfilename = url.path();
    else if (!KIO::NetAccess::download(url, localfilename, (QWidget*)parent()))
    {
        qWarning( "Download failed: %s", fname.ascii() );
        return;
    }
    
    QFile qfile (localfilename);
    if (qfile.open(IO_ReadOnly))
    {
        QDomDocument doc("KontrollerLab");

        initialize( url.directory() );
        m_projectFileURL = url;

        QString errorMesg;
        int errorLine, errorCol;
        if ( !doc.setContent( &qfile, false, &errorMesg, &errorLine, &errorCol ) ) {
            qWarning( "Error in file %s: %d(%d) %s", fname.ascii(), errorLine, errorCol, errorMesg.ascii() );
            qfile.close();
            return;
        }
        qfile.close();

        QDomElement docElem = doc.documentElement();
        readFromDOMElement( doc, docElem );
        m_unsaved=false;
        updateHexFileSize();
    }
    else
        qWarning("Could not open file.");
    m_currentlyOpeningProject = false;
}


void KLProject::readFromDOMElement( QDomDocument & doc, QDomElement & element )
{
    QDomNode n = element.firstChild();
    while( !n.isNull() ) {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if( !e.isNull() )
        {
            if ( e.tagName().lower() == "settings" )
            {
                setSettings( fromDOMToQMap( doc, e ) );
                // qDebug( "m_settings[CPU]=%s", m_settings[PRJ_CPU].ascii() );
            }
            else if ( e.tagName().lower() == "debugger_settings" )
            {
                m_debuggerSettings = fromDOMToQMap( doc, e );
            }
            else if ( e.tagName().lower() == "programmerconfig" )
            {
                m_parent->setProgrammerConfig( fromDOMToQMap( doc, e ) );
            }
            else if ( e.tagName().lower() == "files" )
            {
                QStringList fileShown;
                KLDocumentView* firstview=0L;
                QStringList files = fromDOMToQStringList( doc, e, "FILE", "VIEWS", "", fileShown );
                QStringList::iterator it, showIt;
                // See if we have the old or the new way:
                bool newWay = false;
                showIt = fileShown.begin();
                for (it = files.begin(); it != files.end(); ++it)
                {
                    if ( (!(*it).isEmpty()) && (!(*it).isNull()) )
                    {
                        QString viewListStr = *showIt;
                        if ( ! (viewListStr.isEmpty() || viewListStr.isNull() ) )
                        {
                            newWay = true;
                            break;
                        }
                    }
                    ++showIt;
                }
                if ( newWay )
                {
                    // NEW WAY TO STORE IF A DOCUMENT IS OPEN
                    // qDebug("NEW WAY");
                    showIt = fileShown.begin();
                    for (it = files.begin(); it != files.end(); ++it)
                    {
                        if ( (!(*it).isEmpty()) && (!(*it).isNull()) )
                        {
                            // If the init failed, return!
                            if (!initialized())
                                break;

                            KLDocument* docu = new KLDocument(m_parent);
                            KUrl openURL = m_projectBaseURL;
                            openURL.addPath( *it );
                            docu->open( openURL );
                            addDocument( docu );
                            if ( !( (*showIt).isEmpty() || (*showIt).isNull() ) )
                            {
                                QString viewListStr = *showIt;
                                QStringList viewList;
                                viewList = viewList.split(",", viewListStr, FALSE);
                                for (unsigned int i=0; i+4<viewList.size(); i+=5)
                                {
                                    KLDocumentView* view = new KLDocumentView( docu, m_parent );
                                    bool ok;
                                    int x, y, wid, hei;
                                    x = viewList[i].toInt( &ok );
                                    y = viewList[i+1].toInt( &ok );
                                    wid = viewList[i+2].toInt( &ok );
                                    hei = viewList[i+3].toInt( &ok );
                                    int state = viewList[i+4].toInt( &ok );
                                    view->show();
                                    QRect geo(x, y, wid, hei);
                                    if (state & STATE_IS_ATTACHED)
                                    {
                                        /*if (state & STATE_IS_MAXIMIZED)
                                            view->maximize( false );
                                        else if (state & STATE_IS_MINIMIZED)
                                            view->minimize( false );
                                        else
                                            view->setInternalGeometry( geo );*/
                                    }
                                    else
                                    {
                                        //view->mdiParent()->show();
                                        //view->detach();
                                        //if (state & STATE_IS_MAXIMIZED)
                                        //view->maximize( false );
                                        //else if (state & STATE_IS_MINIMIZED)
                                        //{
                                        //view->setGeometry( geo );
                                        //view->minimize( false );
                                        //}
                                        //else
                                        view->setGeometry( geo );
                                    }
                                    if (!firstview)
                                        firstview = view;
                                }
                            }
                        }
                        ++showIt;
                    }
                }
                else
                {
                    // qDebug("OLD WAY");
                    // OLD WAY TO STORE IF A DOCUMENT IS OPEN:
                    files = fromDOMToQStringList( doc, e, "FILE", "SHOWN", TRUE_STRING, fileShown );
                    showIt = fileShown.begin();
                    for (it = files.begin(); it != files.end(); ++it)
                    {
                        if ( (!(*it).isEmpty()) && (!(*it).isNull()) )
                        {
                            // If the init failed, return!
                            if (!initialized())
                                break;

                            KLDocument* docu = new KLDocument(m_parent);
                            KUrl openURL = m_projectBaseURL;
                            openURL.addPath( *it );
                            docu->open( openURL );
                            addDocument( docu );
                            // qDebug("showIt %s", (*showIt).ascii() );
                            if ( (*showIt).upper() == TRUE_STRING )
                            {
                                KLDocumentView* view = new KLDocumentView( docu, m_parent );
                                if (!firstview)
                                    firstview = view;
                                view->show();
                            }
                        }
                        ++showIt;
                    }
                }
                // To avoid multiple toolbar entries (occurred on older KDE's):
                //if (firstview)
                //    firstview->show();
            }
        }
        n = n.nextSibling();
    }
}

QStringList KLProject::fromDOMToQStringList( QDomDocument &, QDomElement & element, const QString & tag,
                                             const QString & attribName, const QString & attribDefValue,
                                             QStringList &attribs )
{
    QStringList retVal;
    attribs.clear();

    QDomNode n = element.firstChild();
    while( !n.isNull() )
    {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if( !e.isNull() )
        {
            if ( e.tagName().upper() == tag.upper() )
            {
                retVal.append( e.attribute( "NAME", "" ) );
                attribs.append( e.attribute( attribName, attribDefValue ) );
            }
        }
        n = n.nextSibling();
    }
    return retVal;
}

void KLProject::buildAll( )
{
    m_parent->clearMessages();
    m_parent->procManager()->setFinishMessage( i18n("Project built successfully.") );
    m_compilerInterface->buildAll();
}

void KLProject::rebuildAll( )
{
    m_parent->clearMessages();
    m_parent->procManager()->setFinishMessage( i18n("Project built successfully.") );
    m_compilerInterface->rebuildAll();
}

void KLProject::close( )
{

    foreach(KLProjectManagerWidget *it , m_projectManagerWidgets)
    {
        it->clear();
        it->update();
    }

    foreach(KLDocument *it , m_documents)
        removeDocument( it);

    m_initialized = false;
    m_projectBaseURL = KUrl();
    m_projectFileURL = KUrl();
    m_name = i18n("Unnamed project");
    m_unsaved = false;
    m_parent->setCaption( "KontrollerLab" );
}

int KLProject::cpuIndex( ) const
{
    return m_cpus.findIndex( cpu() );
}

int KLProject::getHexFileSize( )
{
    QString buffer;
    int retVal = 0;
    QFile hexFile( getHEXURL().path() );
    
    if ( hexFile.open( QIODevice::ReadOnly ) )
    {
        while ( !hexFile.atEnd() )
        {
            buffer = hexFile.readLine(32786);
            if ( buffer.length() < 9 )
                continue;
            if ( buffer.mid( 0, 1 ) != ":" )
                continue;
            QString bCount_ = buffer.mid( 1, 2 );
            QString type_ = buffer.mid( 7, 2 );
            bool ok1, ok2;
            int bCount = bCount_.toInt( &ok1, 16 );
            int type = type_.toInt( &ok2, 16 );
            if ( ok1 && ok2 && (type==0) )
                retVal += bCount;
        }
        hexFile.close();
    }
    return retVal;
}

void KLProject::updateHexFileSize( )
{
    int size = getHexFileSize();

    if ( size > 0 )
    {
        foreach(KLProjectManagerWidget* it,m_projectManagerWidgets)
        {
            it->setHexFileSize( size );
        }
    }
}


void KLProject::checkForModifiedFiles( )
{
    // This is to ensure that this routine is not called
    // when we are loading a project. On RESTORE, KLab
    // may crash if this routine is called during
    // project load. The reason for that is not totally
    // clear. It seems that the factory for the KateDocument
    // is waiting for the KLab to come up, and another
    // thread tries to access the document at the same time
    // when it was not yet created.
    //if (m_currentlyOpeningProject) TODO
    //return;
    foreach ( KLProjectManagerWidget* itPM, m_projectManagerWidgets )
    {
        itPM->updateModified();
    }

    foreach ( KLDocument* it ,m_documents )
        it->updateModified();
}


KLCPUFuses KLProject::getFusesFor( const QString & name )
{
    return cpuFeaturesFor(name).fuses();
    
    
    
    
    /// TODO: FUSES ARE STORED IN THE FEATURE SET NOW!!!
    KLCPUFuses retVal;
    QList< KLCPUFuses >::iterator it;
    for ( it = m_cpuFuses.begin(); it != m_cpuFuses.end(); ++it )
    {
        if ( (*it).mcuName().upper() == name.upper() )
        {
            retVal = (*it);
            break;
        }
    }
    return retVal;
}


void KLProject::backannotateFuses( QMap< QString, QString > fuses )
{
    m_parent->backannotateFuses( fuses );
}

// This is ONLY called when an error occurred:
void KLProject::backannotateFuses( )
{
    m_parent->slotMessageBox( 1, i18n("The fuse bits could not be read correctly.") );
}

bool KLProject::unsaved( )
{
    if (m_unsaved)
    {
        return true;
    }

    foreach (KLDocument* it,m_documents)
    {
        if (it->isModified())
        {
            return true;
        }
    }
    return false;
}

void KLProject::saveAllDocs( )
{
    foreach (KLDocument* it,m_documents)
        it->save();
}


void KLProject::backannotateSTK500( const QString& stdout )
{
    m_parent->backannotateSTK500( stdout );
}


/**
 *
 * @param name The name of the desired document.
 * @return a pointer to the document, or 0L is there is no such document.
 */
KLDocument * KLProject::getDocumentNamed( const QString & name )
{
    foreach (KLDocument* it,m_documents)
    {
        if ( it->name() == name )
        {
            return it;
        }
    }
    return 0L;
}


void KLProject::setProgrammerBusy( bool val )
{
    m_programmerBusy = val;
    m_parent->setProgrammerBusy( val );
}


KLDocument * KLProject::getDocumentForView( KTextEditor::View * view ) const
{
    foreach (KLDocument* it,m_documents)
    {
        if ( it->hasKTextEditorView( view ) )
        {
            return it;
        }
    }
    return 0L;
}


void KLProject::readDebugHexAndInfo( )
{
    // DO NOT CHANGE THE ORDER OF THESE COMMANDS!
    m_compilerInterface->readHEXProgram();
    // After this command has finished, the debugger will
    // enter the "Running" mode!
    m_compilerInterface->readDebugInfoFromProgram();
}


void KLProject::setASMRelations( QList< KLSourceCodeToASMRelation > rlist )
{
    m_parent->debugger()->setASMRelations( rlist );
}

void KLProject::setASMInstructions( QList< KLASMInstruction > ilist,
                                    const QString& asmLines )
{
    m_parent->debugger()->setASMInstructions( ilist, asmLines );
}


/**
 * @param name The name of the desired document.
 * @return a pointer to the document, or 0L is there is no such document.
 */
KLDocument * KLProject::getDocumentByURL( const KUrl & url )
{
    foreach (KLDocument* it,m_documents)
    {
        if ( it->url() == url )
        {
            return it;
        }
    }
    return 0L;
}


void KLProject::markOnlyLineInDocument( const KUrl& url, int line )
{
    KLDocument* doc = getDocumentByURL( m_currentHighlightURL );
    
    if ( ( url == m_currentHighlightURL ) && ( m_currentHighlightLine == line ) )
        return;
    if ( doc )
        doc->markOnlyLine( -1, 1 );
    doc = getDocumentByURL( url );
    if ( doc )
    {
        doc->markOnlyLine( line, 1 );
        doc->makeLastActiveViewVisible();
    }
    m_currentHighlightURL = url;
    m_currentHighlightLine = line;
}


void KLProject::markBreakpointInDocument( const KUrl & url, int line, bool setMark )
{
    KLDocument* doc = getDocumentByURL( url );

    if ( doc )
        doc->markBreakpoint( line, setMark );
}

void KLProject::setCpuFeaturesFor(const QString & cpu, const KLCPUFeatures & feat)
{
    QList<KLCPUFeatures>::iterator it;
    for (it=m_cpuFeatures.begin(); it!=m_cpuFeatures.end(); ++it)
    {
        if ((*it).name()==cpu)
        {
            (*it) = feat;
        }
    }
    // If not found, add it:
    m_cpuFeatures.append( feat );
}


void KLProject::setCpu(const QString & cpuname)
{
    m_settings[PRJ_CPU] = cpuname;
    m_unsaved = true;
    if ( m_parent && m_parent->memoryViewWidget() )
        m_parent->memoryViewWidget()->slotCPUNameChanged( cpuname );
}


void KLProject::setSettings(const QMap< QString, QString > & theValue)
{
    m_settings = theValue; m_unsaved=true;
    if ( m_parent && m_parent->memoryViewWidget() )
        m_parent->memoryViewWidget()->slotCPUNameChanged( m_settings[PRJ_CPU] );
}


QString KLProject::getHierarchyName(const KUrl & filename) const
{
    int cutTheFirst = projectBaseURL().path().length() + 1;
    return filename.path().right( filename.path().length() - cutTheFirst );
}

QString KLProject::getObjectFilePathFor(const KUrl & file)
{
    KUrl obj = projectBaseURL();
    obj.addPath( getObjectFileNameFor( file ) );
    return obj.path();
}

