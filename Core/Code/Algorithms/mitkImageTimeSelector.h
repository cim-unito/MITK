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


#ifndef IMAGETIMESELECTOR_H_HEADER_INCLUDED_C1E4861D
#define IMAGETIMESELECTOR_H_HEADER_INCLUDED_C1E4861D

#include <MitkExports.h>
#include "mitkSubImageSelector.h"

namespace mitk {

//##Documentation
//## @brief Provides access to a volume at a specific time of the input image
//##
//## If the input is generated by a ProcessObject, only the required data is
//## requested.
//## @ingroup Process
class MITK_CORE_EXPORT ImageTimeSelector : public SubImageSelector
{
public:
  mitkClassMacro(ImageTimeSelector,SubImageSelector);

  itkNewMacro(Self);  

  itkGetConstMacro(TimeNr,int);
  itkSetMacro(TimeNr,int);

  itkGetConstMacro(ChannelNr,int);
  itkSetMacro(ChannelNr,int);

protected:
  ImageTimeSelector();

  virtual ~ImageTimeSelector();

  virtual void GenerateOutputInformation();

  virtual void GenerateInputRequestedRegion();

  virtual void GenerateData();

  int m_TimeNr;

  int m_ChannelNr;
};

} // namespace mitk



#endif /* IMAGETIMESELECTOR_H_HEADER_INCLUDED_C1E4861D */