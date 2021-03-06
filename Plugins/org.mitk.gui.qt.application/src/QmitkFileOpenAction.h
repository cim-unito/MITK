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

#ifndef QMITKFILEOPENACTION_H_
#define QMITKFILEOPENACTION_H_

#ifdef __MINGW32__
// We need to inlclude winbase.h here in order to declare
// atomic intrinsics like InterlockedIncrement correctly.
// Otherwhise, they would be declared wrong within qatomic_windows.h .
#include <windows.h>
#endif

#include <QAction>
#include <QIcon>

#include <berryIWorkbenchWindow.h>

#include <org_mitk_gui_qt_application_Export.h>

#include <mitkIDataStorageReference.h>

class QmitkFileOpenActionPrivate;

/**
 * \ingroup org_mitk_gui_qt_application
 */
class MITK_QT_APP QmitkFileOpenAction : public QAction
{
  Q_OBJECT

public:
  QmitkFileOpenAction(berry::IWorkbenchWindow::Pointer window);
  QmitkFileOpenAction(const QIcon & icon, berry::IWorkbenchWindow::Pointer window);

  ~QmitkFileOpenAction();

protected slots:

  virtual void Run();

private:

  const QScopedPointer<QmitkFileOpenActionPrivate> d;

};


#endif /*QMITKFILEOPENACTION_H_*/
