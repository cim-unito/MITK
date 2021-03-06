/*=========================================================================

Program:   Medical Imaging & Interaction Toolkit
Language:  C++
Date:      $Date$
Version:   $Revision: 7837 $

Copyright (c) German Cancer Research Center, Division of Medical and
Biological Informatics. All rights reserved.
See MITKCopyright.txt or http://www.mitk.org/copyright.html for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "mitkNDIPassiveTool.h"

#include "mitkTestingMacros.h"

#include "mitkStandardFileLocations.h"

#include <iostream>

/**Documentation
* NDIPassiveTool has a protected constructor and a protected itkNewMacro
* so that only it's friend class NDITrackingDevice is able to instantiate
* tool objects. Therefore, we derive from NDIPassiveTool and add a 
* public itkNewMacro, so that we can instantiate and test the class
*/
class NDIPassiveToolTestClass : public mitk::NDIPassiveTool
{
public:
  mitkClassMacro(NDIPassiveToolTestClass, NDIPassiveTool);

  /** make a public constructor, so that the test is able
  *   to instantiate NDIPassiveTool
  */
  itkNewMacro(Self);

protected:
  NDIPassiveToolTestClass() : mitk::NDIPassiveTool()  
  {
  }
};

/**Documentation
*  Test for mitk::NDIPassiveTool
*/
int mitkNDIPassiveToolTest(int /* argc */, char* /*argv*/[])
{
  // always start with this!
  MITK_TEST_BEGIN("NDIPassiveTool");

  // let's create an object of our class  
  mitk::NDIPassiveTool::Pointer myNDIPassiveTool = NDIPassiveToolTestClass::New().GetPointer();

  // using MITK_TEST_CONDITION_REQUIRED makes the test stop after failure, since
  // it makes no sense to continue without an object.
  MITK_TEST_CONDITION_REQUIRED(myNDIPassiveTool.IsNotNull(),"Testing instantiation");

  myNDIPassiveTool->SetTrackingPriority(mitk::NDIPassiveTool::Dynamic);
  MITK_TEST_CONDITION(myNDIPassiveTool->GetTrackingPriority()==mitk::NDIPassiveTool::Dynamic,"Testing Set/GetTrackingPriority() with 'Dynamic'");

  myNDIPassiveTool->SetTrackingPriority(mitk::NDIPassiveTool::ButtonBox);
  MITK_TEST_CONDITION(myNDIPassiveTool->GetTrackingPriority()==mitk::NDIPassiveTool::ButtonBox,"Testing Set/GetTrackingPriority() with 'ButtonBox'");

  myNDIPassiveTool->SetTrackingPriority(mitk::NDIPassiveTool::Static);
  MITK_TEST_CONDITION(myNDIPassiveTool->GetTrackingPriority()==mitk::NDIPassiveTool::Static,"Testing Set/GetTrackingPriority() with 'Static'");

  std::string file = mitk::StandardFileLocations::GetInstance()->FindFile("SROMFile.rom", "Modules/IGT/Testing/Data");
  const char *name = file.c_str();
  const char *name2 = "";
  MITK_TEST_CONDITION(myNDIPassiveTool->LoadSROMFile(name) == true ,"Test LoadSROMFile() with valid file")

  const unsigned char *temp = myNDIPassiveTool->GetSROMData();
  unsigned int templen = myNDIPassiveTool->GetSROMDataLength();

  MITK_TEST_CONDITION(myNDIPassiveTool->LoadSROMFile(name2) == false ,"Test LoadSROMFile() without file")

  MITK_TEST_CONDITION(myNDIPassiveTool->GetSROMData()== temp, "Test GetSROMData() returns same after failed load") 
  MITK_TEST_CONDITION(myNDIPassiveTool->GetSROMDataLength()== templen,"Test GetSROMDataLength() returns same after failed load") 

  // always end with this!
  MITK_TEST_END();
}
