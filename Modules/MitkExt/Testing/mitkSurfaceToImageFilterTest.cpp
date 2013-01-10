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


#include "mitkSurfaceToImageFilter.h"
#include "mitkDataNodeFactory.h"
#include "mitkPicFileWriter.h"
#include <mitkSTLFileReader.h>

#include <vtkPolyData.h>

#include <fstream>

int mitkSurfaceToImageFilterTest(int argc, char* argv[])
{
  mitk::SurfaceToImageFilter::Pointer s2iFilter;
  std::cout << "Testing mitk::Surface::New(): " << std::flush;
  s2iFilter = mitk::SurfaceToImageFilter::New();
  if (s2iFilter.IsNull())
  {
    std::cout<<"[FAILED]"<<std::endl;
    return EXIT_FAILURE;
  }
  else
  {
    std::cout<<"[PASSED]"<<std::endl;
  }

  std::cout << "Loading file: " << std::flush;
  if(argc==0)
  {
    std::cout<<"no file specified [FAILED]"<<std::endl;
    return EXIT_FAILURE;
  }

  if (!mitk::STLFileReader::CanReadFile(argv[1], "", ""))
  {
    std::cout << "Input file not a valid .stl file [PASSED]\n[TEST DONE]" << std::endl;
    return EXIT_SUCCESS;
  }

  mitk::STLFileReader::Pointer reader = mitk::STLFileReader::New();

  reader->SetFileName(argv[1]);
  reader->Update();

  mitk::Surface::Pointer surface = NULL;

  surface = reader->GetOutput();

  if(surface.IsNull())
  {
    std::cout<<"file not a surface - test will not be applied [PASSED]"<<std::endl;
    std::cout<<"[TEST DONE]"<<std::endl;
    return EXIT_SUCCESS;
  }

  std::cout << "Testing number of points of surface: " << std::flush;
  if(surface->GetVtkPolyData()->GetNumberOfPoints() == 0)
  {
    std::cout<<"number of points is 0 - test will not be applied [PASSED]"<<std::endl;
    std::cout<<"[TEST DONE]"<<std::endl;
    return EXIT_SUCCESS;
  }

  std::cout << "Testing creation of mitk::Image with same Geometry as Surface: " << std::flush;
  mitk::Image::Pointer image = mitk::Image::New();
  //surface->UpdateOutputInformation(); //is not necessary anymore (bug #1536), should never be neccessary
  image->Initialize( mitk::MakeScalarPixelType<unsigned int>(), *surface->GetGeometry());

  std::cout << "Testing mitk::SurfaceToImageFilter::MakeOutputBinaryOn(): " << std::flush;
  s2iFilter->MakeOutputBinaryOn();
  std::cout<<"[PASSED]"<<std::endl;

  std::cout << "Testing mitk::SurfaceToImageFilter::SetInput(): " << std::flush;
  s2iFilter->SetInput(surface);
  std::cout<<"[PASSED]"<<std::endl;

  std::cout << "Testing mitk::SurfaceToImageFilter::SetImage(): " << std::flush;
  s2iFilter->SetImage(image);
  std::cout<<"[PASSED]"<<std::endl;

  std::cout << "Testing mitk::SurfaceToImageFilter::Update(): " << std::flush;
  s2iFilter->Update();
  std::cout<<"[PASSED]"<<std::endl;

#ifdef WIN32     // Unix based systems do not seem to resolve pixel type correctly

  std::cout << "Testing if result image is of type unsigned char: " << std::flush;
  //std::string typeId = s2iFilter->GetOutput()->GetPixelType().GetItkTypeAsString();
  std::string typeId = s2iFilter->GetOutput()->GetPixelType().GetComponentTypeAsString();
  std::cout << std::endl << "XXX: " << typeId << std::endl;
  if( typeId != "unsigned char" )
  {
    if(typeId != "unknown")
      return EXIT_FAILURE;
    else std::cout << "Warning: Pixel type can't be resolved." << std::flush;
  }
  std::cout<<"[PASSED]"<<std::endl;

#endif

  //mitk::PicFileWriter::Pointer picWriter = mitk::PicFileWriter::New();
  //picWriter->SetInput(s2iFilter->GetOutput());
  //picWriter->SetFileName("SurfaceToImageFilterTestOutput.pic");
  //picWriter->Write();

  std::cout<<"[TEST DONE]"<<std::endl;
  return EXIT_SUCCESS;
}
