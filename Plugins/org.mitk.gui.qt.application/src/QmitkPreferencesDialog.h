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


#ifndef BERRYQMITKPREFERENCESDIALOG_H_
#define BERRYQMITKPREFERENCESDIALOG_H_

#include <org_mitk_gui_qt_application_Export.h>

#include <QDialog>
#include <QScopedPointer>


class QmitkPreferencesDialogPrivate;

/**
 * \ingroup org_mitk_gui_qt_application
 */
class MITK_QT_APP QmitkPreferencesDialog : public QDialog
{
    Q_OBJECT

public:

    QmitkPreferencesDialog(QWidget * parent = 0, Qt::WindowFlags f = 0);
    ~QmitkPreferencesDialog();

    void SetSelectedPage(const std::string& id);

protected slots:

  void OnImportButtonClicked();
  void OnExportButtonClicked();
  void OnDialogAccepted();
  void OnDialogRejected();

  void OnKeywordTextChanged(const QString & s);
  void OnKeywordEditingFinished();
  void OnPreferencesTreeItemSelectionChanged();
  
protected:

  //bool eventFilter(QObject *obj, QEvent *event);
  void UpdateTree();

  ///
  /// Saves all preferencepages.
  ///
  void SavePreferences();

  QScopedPointer<QmitkPreferencesDialogPrivate> d;

};

#endif /* BERRYQMITKPREFERENCESDIALOG_H_ */
