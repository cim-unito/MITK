/*=========================================================================

Program:   Medical Imaging & Interaction Toolkit
Module:    $RCSfile: $
Language:  C++
Date:      $Date: $
Version:   $Revision: $

Copyright (c) German Cancer Research Center, Division of Medical and
Biological Informatics. All rights reserved.
See MITKCopyright.txt or http://www.mitk.org/copyright.html for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef mitkClippingPlaneRotationTool_h_Included
#define mitkClippingPlaneRotationTool_h_Included

#include "ClippingToolsExports.h"

#include "mitkAffineInteractor3D.h"
#include "mitkCommon.h"
#include "mitkDataNode.h"
#include "mitkTool.h"
#include "mitkVector.h"

namespace mitk
{

  /**
  \brief A tool which allows you to rotate planes.
  */
  class ClippingTools_EXPORT ClippingPlaneRotationTool : public Tool
  {
  public:

    mitkClassMacro(ClippingPlaneRotationTool, Tool);
    itkNewMacro(ClippingPlaneRotationTool);

    virtual const char** GetXPM() const;
    virtual const char* GetName() const;
    virtual const char* GetGroup() const;


  protected:

    ClippingPlaneRotationTool(); // purposely hidden
    virtual ~ClippingPlaneRotationTool();

    virtual void Activated();
    virtual void Deactivated();

    mitk::DataNode::Pointer           m_ClippingPlaneNode;
    mitk::AffineInteractor3D::Pointer m_AffineInteractor;

  private:

    void ClippingPlaneChanged();
  };

} //end namespace mitk

#endif


