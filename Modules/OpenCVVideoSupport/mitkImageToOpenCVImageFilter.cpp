/*===================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center,
Division of Medical and Biological Informatics.
All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.

See LICENSE.txt or http://www.mitk.org for details.

===================================================================*/

#include "mitkImageToOpenCVImageFilter.h"

#include <itkImportImageFilter.h>
#include <itkRGBPixel.h>
#include <mitkImageAccessByItk.h>

mitk::ImageToOpenCVImageFilter::ImageToOpenCVImageFilter()
: m_OpenCVImage(0)
{
}

mitk::ImageToOpenCVImageFilter::~ImageToOpenCVImageFilter()
{
  m_OpenCVImage = 0;
}

int mitk::ImageToOpenCVImageFilter::GetDepth(const std::type_info& typeInfo) const
{
  if(typeInfo == typeid(unsigned char))
  {
    return IPL_DEPTH_8U;
  }
  else if(typeInfo == typeid(char))
  {
    return IPL_DEPTH_8S;
  }
  else if(typeInfo == typeid(unsigned short))
  {
    return IPL_DEPTH_16U;
  }
  else if(typeInfo == typeid(short))
  {
    return IPL_DEPTH_16S;
  }
  else if(typeInfo == typeid(int))
  {
    return IPL_DEPTH_32S;
  }
  else if(typeInfo == typeid(float))
  {
    return IPL_DEPTH_32F;
  }
  else if(typeInfo == typeid(double))
  {
    return IPL_DEPTH_64F;
  }
  // rgb image
  else if(typeInfo == typeid(UCRGBPixelType))
  {
    return IPL_DEPTH_8U;
  }
  else if(typeInfo == typeid(USRGBPixelType))
  {
    return IPL_DEPTH_16U;
  }
  else if(typeInfo == typeid(FloatRGBPixelType))
  {
    return IPL_DEPTH_64F;
  }
  else if(typeInfo == typeid(DoubleRGBPixelType))
  {
    return IPL_DEPTH_64F;
  }
  else return -1;
}

bool mitk::ImageToOpenCVImageFilter::CheckImage( mitk::Image* image )
{
  if(image == 0)
  {
    MITK_WARN << "MITK Image is 0";
    return false;
  }
  if(image->GetDimension() != 2)
  {
    MITK_WARN << "Only 2D Images allowed";
    return false;
  }
  return true;
}

IplImage* mitk::ImageToOpenCVImageFilter::GetOpenCVImage()
{
  if(!this->CheckImage( m_Image ))
    return 0;
  if(!m_OpenCVImage)
  {
    try
    {
      AccessFixedTypeByItk(m_Image.GetPointer(), ItkImageProcessing,
                           MITK_ACCESSBYITK_PIXEL_TYPES_SEQ  // gray image
                           (UCRGBPixelType)(USRGBPixelType)(FloatRGBPixelType)(DoubleRGBPixelType), // rgb image
                           (2) // dimensions
                           )
    }
    catch (const AccessByItkException& e) {
      std::cout << "Caught exception [from AccessFixedTypeByItk]: \n" << e.what() << "\n";
      return 0;
    }
  }
  return m_OpenCVImage;
}

void mitk::ImageToOpenCVImageFilter::SetImage( mitk::Image* _Image )
{
//  if(m_Image == _Image) return;
  m_Image = _Image;
  m_OpenCVImage = 0;
}
