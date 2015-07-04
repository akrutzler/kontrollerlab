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
#ifndef KLMEMORYVIEWWIDGET_H
#define KLMEMORYVIEWWIDGET_H

#include "ui_klmemoryview.h"
#include <QDialog>
#include <qtimer.h>
#include "klcpuregisterdescription.h"


class KontrollerLab;


class KLMemoryViewWidget: public QDialog {
Q_OBJECT
public:
    KLMemoryViewWidget(KontrollerLab *parent = 0, const char *name = 0);
    void allowSetMemoryCell( bool value );
    void setRamEnd(const int& theValue);
    int ramEnd() const { return m_ramEnd; }

private:
    Ui::KLMemoryViewWidgetBase *ui;

public slots:
    virtual void slotCPUNameChanged( const QString& newName );
    virtual void slotCurrentItemChanged( QTreeWidgetItem* current, QTreeWidgetItem* previous );
    virtual void slotSelectionChanged();
    virtual void slotUpdateEveryChanged( int value );
    virtual void slotUpdateEnable( bool value );
    virtual void slotBinaryCBChanged();
    virtual void slotBinaryChanged( int value );
    virtual void slotCharChanged( const QString& value );
    virtual void slotDecChanged( int value );
    virtual void slotHexChanged( int value );
    virtual void slotNameChanged( const QString& name );
    virtual void slotUpdate();
    virtual void slotSetMemoryCell();
    void timeoutOfTimer();
    void slotMemoryReadCompleted( unsigned int adr, unsigned char val );

    void setEnableAutomaticGUIUpdates(bool theValue) { m_enableAutomaticGUIUpdates = theValue; }
    bool enableAutomaticGUIUpdates() const { return m_enableAutomaticGUIUpdates; }

    void updateGUI();
    void setAllValuesToZero();
signals:
    void memoryCellSet( unsigned int adr, unsigned char val );
protected:
    void setGUIValueTo(int value);

    int m_ramEnd;
    int m_nextUpdateIn;
    bool m_enableAutomaticGUIUpdates;
    KontrollerLab* m_parent;
    KLCPURegisterDescriptionList m_currentlyDisplayedRdl;
    QTimer m_updateTimer;
};

#endif
