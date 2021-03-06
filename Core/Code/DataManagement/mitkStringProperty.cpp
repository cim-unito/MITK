/*=========================================================================

Program:   Medical Imaging & Interaction Toolkit
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) German Cancer Research Center, Division of Medical and
Biological Informatics. All rights reserved.
See MITKCopyright.txt or http://www.mitk.org/copyright.html for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#include "mitkStringProperty.h"

const char* mitk::StringProperty::PATH = "path";
mitk::StringProperty::StringProperty( const char* string ) 
: m_Value()
{
  if ( string )
    m_Value = string;
}

mitk::StringProperty::StringProperty( const std::string& s )
: m_Value( s )
{

}

bool mitk::StringProperty::IsEqual(const BaseProperty& property ) const
{
  return this->m_Value == static_cast<const Self&>(property).m_Value;
}

bool mitk::StringProperty::Assign(const BaseProperty& property )
{
  this->m_Value = static_cast<const Self&>(property).m_Value;
  return true;
}

std::string mitk::StringProperty::GetValueAsString() const 
{
  return m_Value;
}
