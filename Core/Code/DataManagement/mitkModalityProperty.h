/*=========================================================================
 
Program:   Medical Imaging & Interaction Toolkit
Language:  C++
Date:      $Date: 2009-05-15 10:46:54 +0200 (Fr, 15 Mai 2009) $
Version:   $Revision: 17272 $
 
Copyright (c) German Cancer Research Center, Division of Medical and
Biological Informatics. All rights reserved.
See MITKCopyright.txt or http://www.mitk.org/copyright.html for details.
 
This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.
 
=========================================================================*/

#ifndef mitkModalityProperty_h_Included
#define mitkModalityProperty_h_Included

#include <MitkExports.h>
#include "mitkEnumerationProperty.h"
#include <itkObjectFactory.h>

namespace mitk
{

#ifdef _MSC_VER
# pragma warning(push)
# pragma warning(disable: 4522)
#endif

/**
  \brief Enumerates all known modalities

  \ingroup DataManagement
*/
class MITK_CORE_EXPORT ModalityProperty : public EnumerationProperty
{
  public:
    
    mitkClassMacro(ModalityProperty, EnumerationProperty);
    itkNewMacro(ModalityProperty);
    mitkNewMacro1Param(ModalityProperty, const IdType&);
    mitkNewMacro1Param(ModalityProperty, const std::string&);

    using BaseProperty::operator=;

  protected:
    ModalityProperty();
    ModalityProperty( const IdType& value );
    ModalityProperty( const std::string& value );
    
    virtual ~ModalityProperty();
    virtual void AddEnumerationTypes();

  private:

    // purposely not implemented
    ModalityProperty(const ModalityProperty&);
    const ModalityProperty& operator=(const ModalityProperty&);
};

#ifdef _MSC_VER
# pragma warning(pop)
#endif

} // namespace

#endif


