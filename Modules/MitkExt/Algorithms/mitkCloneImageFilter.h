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


#ifndef MITKCLONEIMAGEFILTER_H_HEADER_INCLUDED_C1F48A22
#define MITKCLONEIMAGEFILTER_H_HEADER_INCLUDED_C1F48A22

#include "mitkCommon.h"
#include "MitkExtExports.h"
#include "mitkImageToImageFilter.h"
#include "mitkImageTimeSelector.h"

#include "itkImage.h"

namespace mitk {

//##Documentation
//## @brief 
//## @ingroup Process
class MitkExt_EXPORT CloneImageFilter : public ImageToImageFilter
{
public:
  mitkClassMacro(CloneImageFilter, ImageToImageFilter);

  itkNewMacro(Self);

protected:
  CloneImageFilter();

  ~CloneImageFilter();

  virtual void GenerateInputRequestedRegion();

  virtual void GenerateOutputInformation();

  virtual void GenerateData();

  template < typename TPixel, unsigned int VImageDimension >
  void InternalComputeMask(itk::Image<TPixel, VImageDimension>* itkImage);

  mitk::ImageTimeSelector::Pointer m_InputTimeSelector;
  mitk::ImageTimeSelector::Pointer m_OutputTimeSelector;

  //##Description 
  //## @brief Time when Header was last initialized
  itk::TimeStamp m_TimeOfHeaderInitialization;
  
};

} // namespace mitk

#endif /* MITKCLONEIMAGEFILTER_H_HEADER_INCLUDED_C1F48A22 */


