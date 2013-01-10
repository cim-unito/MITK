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
#include "mitkClippingPlaneTranslationTool.h"
#include "mitkClippingPlaneTranslationTool.xpm"

#include "mitkBaseRenderer.h"
#include "mitkGlobalInteraction.h"
#include "mitkRenderingManager.h"
#include "mitkToolManager.h"

namespace mitk
{
  MITK_TOOL_MACRO(ClippingTools_EXPORT, ClippingPlaneTranslationTool, "Clipping Tool Translation");
}

mitk::ClippingPlaneTranslationTool::ClippingPlaneTranslationTool()
: Tool( "global" )
{
}

mitk::ClippingPlaneTranslationTool::~ClippingPlaneTranslationTool()
{
}

const char** mitk::ClippingPlaneTranslationTool::GetXPM() const
{
  return mitkClippingPlaneTranslationTool_xpm;
}

const char* mitk::ClippingPlaneTranslationTool::GetName() const
{
  return "Translation";
}

const char* mitk::ClippingPlaneTranslationTool::GetGroup() const
{
  return "ClippingTool";
}

void mitk::ClippingPlaneTranslationTool::Activated()
{
  Superclass::Activated();

  //check if the Clipping plane is changed.
  if(m_ClippingPlaneNode != m_ToolManager->GetWorkingData(0))
  {
    mitk::GlobalInteraction::GetInstance()->RemoveInteractor(m_AffineInteractor);
    this->ClippingPlaneChanged();
  }

  m_AffineInteractor->SetInteractionModeToTranslation();
  mitk::GlobalInteraction::GetInstance()->AddInteractor(m_AffineInteractor);
}

void mitk::ClippingPlaneTranslationTool::Deactivated()
{
  Superclass::Deactivated();

  mitk::GlobalInteraction::GetInstance()->RemoveInteractor(m_AffineInteractor);
}

//Checks the working data node, if it has an interactor. Otherwise initial a new one.
void mitk::ClippingPlaneTranslationTool::ClippingPlaneChanged()
{
  m_ClippingPlaneNode = m_ToolManager->GetWorkingData(0);
  m_AffineInteractor = dynamic_cast<mitk::AffineInteractor3D*>(m_ClippingPlaneNode->GetInteractor());

  if (m_AffineInteractor.IsNull())
    m_AffineInteractor = mitk::AffineInteractor3D::New("AffineInteractor3D", m_ClippingPlaneNode);
}
