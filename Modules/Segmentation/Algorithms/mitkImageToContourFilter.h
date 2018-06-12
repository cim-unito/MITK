/*=========================================================================
Program: Medical Imaging & Interaction Toolkit
Module: $RCSfile$
Language: C++
Date: $Date: $
Version: $Revision: $
Copyright (c) German Cancer Research Center, Division of Medical and
Biological Informatics. All rights reserved.
See MITKCopyright.txt or http://www.mitk.org/copyright.html for details.
This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notices for more information.
=========================================================================*/

#ifndef mitkImageToContourFilter_h_Included
#define mitkImageToContourFilter_h_Included

//#include "MitkSBExports.h"
#include "SegmentationExports.h"
#include "itkImage.h"
#include "mitkImage.h"
#include "itkContourExtractor2DImageFilter.h"
#include "mitkImageToSurfaceFilter.h"
#include "mitkSurface.h"
#include "vtkPolyData.h"
#include "vtkPolygon.h"
#include "vtkCellArray.h"

#include "mitkProgressBar.h"

namespace mitk {

/**
  \brief A filter that can extract contours out of a 2D binary image
  
  This class takes an 2D mitk::Image as input and extracts all contours which are drawn it. The contour
  extraction is done by using the itk::ContourExtractor2DImageFilter.
  
  The output is a mitk::Surface.

  $Author: fetzer$
*/
class Segmentation_EXPORT ImageToContourFilter : public ImageToSurfaceFilter
{
 public:

   mitkClassMacro(ImageToContourFilter,ImageToSurfaceFilter);
   itkNewMacro(Self);

    /**
      \brief Set macro for the geometry of the slice. If it is not set explicitly the geometry will be taken from the slice 
      
      \a Parameter The slice`s geometry 
    */
   itkSetMacro(SliceGeometry, Geometry3D*);
   
   //typedef unsigned int VDimension;
   typedef itk::PolyLineParametricPath<2> PolyLineParametricPath2D;
   typedef PolyLineParametricPath2D::VertexListType ContourPath;

   /**
     \brief Set whether the mitkProgressBar should be used

     \a Parameter true for using the progress bar, false otherwise
   */
   void SetUseProgressBar(bool);

   /**
     \brief Set the stepsize which the progress bar should proceed

     \a Parameter The stepsize for progressing
   */
   void SetProgressStepSize(unsigned int stepSize);
   
 protected:
   ImageToContourFilter();
   virtual ~ImageToContourFilter();
   virtual void GenerateData();
   virtual void GenerateOutputInformation();
   
 private:
   const Geometry3D* m_SliceGeometry;
   bool m_UseProgressBar;
   unsigned int m_ProgressStepSize;

   template<typename TPixel, unsigned int VImageDimension>
   void Itk2DContourExtraction (itk::Image<TPixel, VImageDimension>* sliceImage);

};//class

}//namespace
#endif
