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
#ifndef KLPROJECTCONFIGWIDGET_H
#define KLPROJECTCONFIGWIDGET_H

#include "ui_klprojectconfigwidgetbase.h"
#include <qvalidator.h>
#include "kldocumentview.h"


class KLProject;
class KLProjectManagerWidget;

class KLProjectConfigWidget: public  QDialog {
Q_OBJECT
public:
    KLProjectConfigWidget(KLProject* project, QWidget *parent = 0, const char *name = 0);
    void updateSettingsFromGUI();
    void updateGUIFromSettings();
    QMap< QString, QString > settings() const { return m_settings; }
private:
    Ui::KLProjectConfigWidgetBase *ui;
public slots:
    virtual void slotCPUChanged( const QString& name );
    virtual void slotCancel();
    virtual void slotSetDefault();
    virtual void slotOK();
    virtual void slotBuildSystemChanged();
protected:
    virtual void showEvent( QShowEvent* );
    QString conf( const QString& confKey, const QString& defval ) const;
    void updateCPUInfo( const QString& name );
    KLProject* m_project;
    QRegExpValidator m_validator;
    KLProjectManagerWidget* m_projectManagerInConfigWidget;
    QMap< QString, QString > m_settings;
    QMap< QString, QString > m_possibleLinkerFlags;
    QList< QTableWidgetItem*> m_linkerFlagsCBs;
};

#endif
