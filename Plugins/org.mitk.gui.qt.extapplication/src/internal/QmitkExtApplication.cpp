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

#include "QmitkExtApplication.h"

#include <berryPlatformUI.h>

#include "QmitkExtAppWorkbenchAdvisor.h"

QmitkExtApplication::QmitkExtApplication()
{

}

int QmitkExtApplication::Start()
{
  berry::Display* display = berry::PlatformUI::CreateDisplay();

  int code = berry::PlatformUI::CreateAndRunWorkbench(display, new QmitkExtAppWorkbenchAdvisor());
  
  // exit the application with an appropriate return code
  return code == berry::PlatformUI::RETURN_RESTART
              ? EXIT_RESTART : EXIT_OK;
}

void QmitkExtApplication::Stop()
{
  
}
