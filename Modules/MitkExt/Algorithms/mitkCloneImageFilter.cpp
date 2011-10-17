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


#include "mitkCloneImageFilter.h"
#include "mitkImageTimeSelector.h"
#include "mitkTimeHelper.h"
#include "mitkProperties.h"

#include "mitkImageToItk.h"
#include "mitkImageAccessByItk.h"

#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIteratorWithIndex.h"

mitk::CloneImageFilter::CloneImageFilter()
{
  this->SetNumberOfInputs(1);
  this->SetNumberOfRequiredInputs(1);
  m_InputTimeSelector  = mitk::ImageTimeSelector::New();
  m_OutputTimeSelector = mitk::ImageTimeSelector::New();
}

mitk::CloneImageFilter::~CloneImageFilter()
{

}

void mitk::CloneImageFilter::GenerateInputRequestedRegion()
{
  Superclass::GenerateInputRequestedRegion();

  mitk::Image* output = this->GetOutput();
  mitk::Image* input = const_cast< mitk::Image * > ( this->GetInput() );
  if (output->IsInitialized() == false)
    return;

  input->SetRequestedRegionToLargestPossibleRegion();

  GenerateTimeInInputRegion(output, input);
}

void mitk::CloneImageFilter::GenerateOutputInformation()
{
  mitk::Image::ConstPointer input = this->GetInput();
  mitk::Image::Pointer output = this->GetOutput();

  if ((output->IsInitialized()) && (this->GetMTime() <= m_TimeOfHeaderInitialization.GetMTime()))
    return;

  itkDebugMacro(<<"GenerateOutputInformation()");

  output->Initialize(input->GetPixelType(), *input->GetTimeSlicedGeometry());

  output->SetPropertyList(input->GetPropertyList()->Clone());    

  m_TimeOfHeaderInitialization.Modified();
}

template < typename TPixel, unsigned int VImageDimension >
void mitk::CloneImageFilter::InternalComputeMask(itk::Image<TPixel, VImageDimension>* inputItkImage)
{
  typedef itk::Image<TPixel, VImageDimension> ItkInputImageType;
  typedef itk::Image<TPixel, VImageDimension> ItkOutputImageType;

  typedef itk::ImageRegionConstIterator< ItkInputImageType > ItkInputImageIteratorType;
  typedef itk::ImageRegionIteratorWithIndex< ItkOutputImageType > ItkOutputImageIteratorType;

  typename mitk::ImageToItk<ItkOutputImageType>::Pointer outputimagetoitk = mitk::ImageToItk<ItkOutputImageType>::New();
  outputimagetoitk->SetInput(m_OutputTimeSelector->GetOutput());
  outputimagetoitk->Update();
  typename ItkOutputImageType::Pointer outputItkImage = outputimagetoitk->GetOutput();

  // create the iterators
  typename ItkInputImageType::RegionType inputRegionOfInterest = inputItkImage->GetLargestPossibleRegion();
  ItkInputImageIteratorType  inputIt( inputItkImage, inputRegionOfInterest );
  ItkOutputImageIteratorType outputIt( outputItkImage, inputRegionOfInterest );

  for ( inputIt.GoToBegin(), outputIt.GoToBegin(); !inputIt.IsAtEnd(); ++inputIt, ++outputIt)
  {
    outputIt.Set(inputIt.Get());
  }
}

void mitk::CloneImageFilter::GenerateData()
{
  mitk::Image::ConstPointer input = this->GetInput();
  mitk::Image::Pointer output = this->GetOutput();

  if (output->IsInitialized() == false)
    return;

  m_InputTimeSelector->SetInput(input);
  m_OutputTimeSelector->SetInput(this->GetOutput());

  mitk::Image::RegionType outputRegion = output->GetRequestedRegion();
  const mitk::TimeSlicedGeometry *outputTimeGeometry = output->GetTimeSlicedGeometry();
  const mitk::TimeSlicedGeometry *inputTimeGeometry = input->GetTimeSlicedGeometry();
  ScalarType timeInMS;

  int timestep = 0;
  int tstart = outputRegion.GetIndex(3);
  int tmax = tstart + outputRegion.GetSize(3);

  int t;
  for(t=tstart;t<tmax;++t)
  {
    timeInMS = outputTimeGeometry->TimeStepToMS( t );

    timestep = inputTimeGeometry->MSToTimeStep( timeInMS );

    m_InputTimeSelector->SetTimeNr(timestep);
    m_InputTimeSelector->UpdateLargestPossibleRegion();
    m_OutputTimeSelector->SetTimeNr(t);
    m_OutputTimeSelector->UpdateLargestPossibleRegion();

    AccessByItk(m_InputTimeSelector->GetOutput(), InternalComputeMask);
  }

  m_TimeOfHeaderInitialization.Modified();
}
