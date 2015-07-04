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
#ifndef KLFILENEWDIALOG_H
#define KLFILENEWDIALOG_H

#include "ui_klfilenewdialogbase.h"
#include <kurl.h>
class KontrollerLab;
class KLProject;


class KLFileNewDialog: public QDialog {
Q_OBJECT
public:
    KLFileNewDialog(KontrollerLab *parent, KLProject* project, const char *name = 0);
private:
    Ui::KLFileNewDialogBase *ui;
public slots:
    virtual void slotCancel();
    virtual void slotOK();
    virtual void slotFileNameChanged( const QString& fn );
    virtual void slotChoose();
protected:
    KontrollerLab *m_parent;
    KLProject* m_project;
    KUrl m_targetDirectory;
    QListWidgetItem *m_cSource, *m_asmSource, *m_cHeader, *m_txt, *m_other;
};

#endif
