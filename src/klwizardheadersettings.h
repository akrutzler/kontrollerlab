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
#ifndef KLWIZARDHEADERSETTINGS_H
#define KLWIZARDHEADERSETTINGS_H

#include "klwizardheadersetting.h"
#include <qlist.h>


class KLDocument;

/**
This class stores KLWizardHeaderSetting class instances.
It can read their values from a document (which contains the header file),
and can write them to that document when the wizard finishes.

	@author Martin Strasser <strasser@kontrollerlab.org>
*/
class KLWizardHeaderSettings : public QList<KLWizardHeaderSetting>
{
public:
    KLWizardHeaderSettings( KLDocument* doc=0L );

    ~KLWizardHeaderSettings();
    KLWizardHeaderSetting getSettingFor( const QString defName );
    void setSetting( const KLWizardHeaderSetting& val );
    void writeValuesToDocument();
    void readValuesFromDocument();
    void setDocument (KLDocument* doc) { m_doc = doc; }
protected:
    KLDocument* m_doc;
};

#endif
