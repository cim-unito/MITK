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

#ifndef QMITKLEVELWINDOWWIDGET_H_
#define QMITKLEVELWINDOWWIDGET_H_

#include <QmitkExports.h>

#include "ui_QmitkLevelWindowWidget.h"

#include <QWidget>

class QMITK_EXPORT QmitkLevelWindowWidget : public QWidget, public Ui::QmitkLevelWindow
{
  Q_OBJECT
public:
  QmitkLevelWindowWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);
  mitk::LevelWindowManager* GetManager();
  
  void SetIntensityRangeType(QmitkLineEditLevelWindowWidget::IntensityRangeType intensityRangeType);

  void SetExponentialFormat(bool value);

  void SetPrecision(int precision);

public slots:
  void SetDataStorage( mitk::DataStorage* ds );

protected:
  //unsigned long m_ObserverTag;
  mitk::LevelWindowManager::Pointer m_Manager;
};
#endif /*QMITKLEVELWINDOWWIDGET_H_*/
