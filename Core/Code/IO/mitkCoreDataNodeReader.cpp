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


#include "mitkCoreDataNodeReader.h"

#include <mitkDataNodeFactory.h>
#include <mitkDataStorage.h>

namespace mitk {

int CoreDataNodeReader::Read(const std::string &fileName, DataStorage &storage)
{
  mitk::DataNodeFactory::Pointer nodeReader = mitk::DataNodeFactory::New();

  // the ITK Nrrd file reader cannot handle '/' in file path on Win 64bit
  std::string name(fileName);
  std::replace(name.begin(), name.end(), '\\', '/');
  nodeReader->SetFileName(name);
  nodeReader->Update();
  int n = 0;
  for ( unsigned int i = 0 ; i < nodeReader->GetNumberOfOutputs( ); ++i )
  {
    mitk::DataNode::Pointer node;
    node = nodeReader->GetOutput(i);
    if ( node->GetData() != NULL )
    {
      storage.Add(node);
      ++n;
    }
  }
  return n;
}

}
