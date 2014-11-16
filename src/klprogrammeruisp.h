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
#ifndef KLPROGRAMMERUISP_H
#define KLPROGRAMMERUISP_H

#include "klprogrammerinterface.h"

#define UISP                                 "UISP"
#define UISP_SPECIFY_PART                    "UISP_SPECIFY_PART"
#define UISP_PROGRAMMER_TYPE                 "UISP_PROGRAMMER_TYPE"
#define UISP_PARALLEL_PORT                   "UISP_PARALLEL_PORT"
#define UISP_PARALLEL_NO_DATA_POLLING        "UISP_PARALLEL_NO_DATA_POLLING"
#define UISP_PARALLEL_DISABLE_RETRIES        "UISP_PARALLEL_DISABLE_RETRIES"
#define UISP_PARALLEL_AT89S                  "UISP_PARALLEL_AT89S"
#define UISP_PARALLEL_VOLTAGE                "UISP_PARALLEL_VOLTAGE"
#define UISP_PARALLEL_SCK_HIGH_LOW_TIME      "UISP_PARALLEL_SCK_HIGH_LOW_TIME"
#define UISP_PARALLEL_FLASH_MAX_WRITE_DELAY  "UISP_PARALLEL_FLASH_MAX_WRITE_DELAY"
#define UISP_PARALLEL_EEPROM_MAX_WRITE_DELAY "UISP_PARALLEL_EEPROM_MAX_WRITE_DELAY"
#define UISP_PARALLEL_RESET_HIGH_TIME        "UISP_PARALLEL_RESET_HIGH_TIME"
#define UISP_SERIAL_PORT                     "UISP_SERIAL_PORT"
#define UISP_SERIAL_SPEED                    "UISP_SERIAL_SPEED"

#define UISP_STK500_USE_HIGH_VOLTAGE         "UISP_STK500_USE_HIGH_VOLTAGE"
#define UISP_STK500_AREF_VOLTAGE             "UISP_STK500_AREF_VOLTAGE"
#define UISP_STK500_VTARGET_VOLTAGE          "UISP_STK500_VTARGET_VOLTAGE"
#define UISP_STK500_OSCILLATOR_FREQUENCY     "UISP_STK500_OSCILLATOR_FREQUENCY"

#define USER_KEY                             "USER"

/**
This is the controller for the UISP.

	@author Martin Strasser <strasser@kontrollerlab.org>
*/
class KLProgrammerUISP : public KLProgrammerInterface
{
public:
    KLProgrammerUISP(KLProcessManager* processManager, KLProject* project);

    ~KLProgrammerUISP();

    virtual bool erase();
    virtual bool upload( const KUrl& url );
    virtual bool verify( const KUrl& url );
    virtual bool download(const KUrl &url );
    virtual bool ignite( const KUrl& url );
    virtual bool programFuses( QMap<QString, QString> fuses );
    virtual bool readFuses();


    void writeSTK500( double a_ref, double v_sup, double clk );
    void readSTK500(  );
protected:
    virtual QMap< QString, QString > parseFuses( const QString& stdout );

    QString attribute( const QString& attr, const QString confKey ) const;
    QString formBaseCommand() const;
};

#endif
