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
#include "klwizardheadersetting.h"


KLWizardHeaderSetting::KLWizardHeaderSetting()
{
    m_type = NoType;
    m_intValue = m_min = m_max = m_base = 0;
}


KLWizardHeaderSetting::~KLWizardHeaderSetting()
{
}


KLWizardHeaderSetting::KLWizardHeaderSetting( const QString& grpName, const QString & defName,
                                              const QString& description,
                                              int value, int min, int max, int base )
{
    m_type = IntType;
    m_defName = defName;
    m_intValue = value;
    m_min = min;
    m_max = max;
    m_base = base;
    m_description = description;
    m_groupName = grpName;
}


KLWizardHeaderSetting::KLWizardHeaderSetting( const QString& grpName, const QString & defName,
                                              const QString& description,
                                              const QString & value )
{
    m_type = StringType;
    m_defName = defName;
    m_strValue = value;
    m_intValue = m_min = m_max = m_base = 0;
    m_description = description;
    m_groupName = grpName;
}


KLWizardHeaderSetting::KLWizardHeaderSetting( const QString& grpName, const QString & defName,
                                              const QString& description,
                                              const QString & value,
                                              const QStringList possibleValues )
{
    m_type = StringListType;
    m_intValue = m_min = m_max = m_base = 0;
    m_defName = defName;
    m_strValue = value;
    m_possibleValues = possibleValues;
    m_description = description;
    m_groupName = grpName;
}


void KLWizardHeaderSetting::setValue( const QString & val_ )
{
    QString val = val_.stripWhiteSpace();
    if ( (m_type == IntType) || (m_type == StringListType) )
    {
        bool ok;
        if ( val.upper().startsWith( "0X" ) )
        {
            m_intValue = val.toInt( &ok, 16 );
        }
        else if ( val.startsWith( "0" ) && (val.length() > 1) )
        {
            m_intValue = val.toInt( &ok, 8 );
        }
        else
        {
            m_intValue = val.toInt( &ok, 10 );
        }
        if ( !ok )
            qWarning( "Error while reading value for %s: \"%s\"", m_defName.ascii(), val.ascii() );
    }
    else if ( m_type == StringType )
    {
        m_strValue = val;
    }
}


QString KLWizardHeaderSetting::valueAsString( ) const
{
    QString retVal;
    if ( (m_type == IntType) || (m_type == StringListType) )
    {
        retVal = retVal.number( m_intValue, m_base );
        if ( m_base == 16 )
            retVal = "0x" + retVal;
        if ( m_base == 8 )
            retVal = "0" + retVal;
    }
    else if ( m_type == StringType )
    {
        retVal = m_strValue;
    }
    return retVal;
}

