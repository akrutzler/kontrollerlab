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
#ifndef KLDEBUGGERCONFIGWIDGET_H
#define KLDEBUGGERCONFIGWIDGET_H

#include "ui_kldebuggerconfigwidgetbase.h"
#include <kurl.h>
#include "kldebuggermemorymapping.h"
#include <kio/job.h>
#include <kio/jobclasses.h>


class KontrollerLab;

class KLDebuggerConfigWidget: public QDialog {
Q_OBJECT
public:
    KLDebuggerConfigWidget(KontrollerLab *parent = 0, const char *name = 0);
    void updateSettingsFromGUI();
    void updateGUIFromSettings();
    QMap< QString, QString > settings() const { return m_settings; }

    KLDebuggerMemoryMappingList mappingList();
public slots:
    virtual void slotUBRRChanged( int value );
    virtual void slotBAUDRateChanged( const QString& value );
    virtual void slotBuildAndDownload();
    virtual void slotCheckState();
    virtual void slotCancel();
    virtual void slotOK();
    virtual void slotAdd();
    virtual void slotSetDefault();
    virtual void copyMonitorFilesDone(KIO::Job *, const KUrl &, const KUrl &, time_t, bool , bool);
    virtual void slotSelectedMappingChanged( Q3ListViewItem* item );
    virtual void slotToChanged( int val );
    virtual void slotFromChanged( int val );
    virtual void slotRemove();
protected:
    virtual void showEvent( QShowEvent* );
    QString conf( const QString& confKey, const QString& defval ) const;

    KontrollerLab* m_parent;
    KUrl::List m_buildUrlList;
    KUrl m_tempURL;
    bool m_noCallback;
    QMap< QString, QString > m_settings;
private:
    Ui_KLDebuggerConfigWidgetBase *ui;
};

#endif
