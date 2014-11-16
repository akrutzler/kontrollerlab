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
#include "klwizardheadersettings.h"
#include "kldocument.h"
#include "kldocumentview.h"
#include <qregexp.h>


KLWizardHeaderSettings::KLWizardHeaderSettings( KLDocument* doc ) :
        QList< KLWizardHeaderSetting >()
{
    m_doc = doc;
}


KLWizardHeaderSettings::~KLWizardHeaderSettings()
{
}

KLWizardHeaderSetting KLWizardHeaderSettings::getSettingFor( const QString defName )
{
    KLWizardHeaderSettings buffer = (*this);
    KLWizardHeaderSettings::iterator it;
    
    for ( it = buffer.begin(); it != buffer.end(); ++it )
    {
        if ( (*it).defName() == defName )
            return (*it);
    }
    return KLWizardHeaderSetting();
}

void KLWizardHeaderSettings::setSetting( const KLWizardHeaderSetting & val )
{
    KLWizardHeaderSettings::iterator it;
    
    for ( it = begin(); it != end(); ++it )
    {
        if ( (*it).defName() == val.defName() )
        {
            (*it) = val;
            return;
        }
    }
}


void KLWizardHeaderSettings::writeValuesToDocument( )
{
    KLWizardHeaderSettings copyMe = (*this);
    //QString fullText = m_doc->editIf->text();
    // Search and replace by regexp
    KLWizardHeaderSettings::iterator it;
    for ( it = copyMe.begin(); it != copyMe.end(); ++it )
    {
        QRegExp re(QString("#define(\\s+)%1(\\s+)[^\\s]*").arg((*it).defName()));
        //fullText.replace( re, QString( "#define\\1%1\\2%2" ).arg((*it).defName()).arg((*it).valueAsString()) );
    }
    //m_doc->editIf->setText( fullText );
}

void KLWizardHeaderSettings::readValuesFromDocument( )
{
   // QString fullText = m_doc->editIf->text();
    // Search by regexp
    KLWizardHeaderSettings::iterator it;
    for ( it = begin(); it != end(); ++it )
    {
        QRegExp re(QString("#define\\s+%1\\s+([^\\s]*)").arg((*it).defName()));
        //if ( re.search( fullText ) >=0 )
        {
            (*it).setValue( re.cap( 1 ) );
        }
    }
}

