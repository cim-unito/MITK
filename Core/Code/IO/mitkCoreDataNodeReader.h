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


#ifndef MITKCOREDATANODEREADER_H
#define MITKCOREDATANODEREADER_H

#include <mitkIDataNodeReader.h>

namespace mitk {

class CoreDataNodeReader : public itk::LightObject, public mitk::IDataNodeReader
{
public:

  itkNewMacro(CoreDataNodeReader)

  int Read(const std::string& fileName, mitk::DataStorage& storage);
};

}

#endif // MITKCOREDATANODEREADER_H
