/*
 * QmitkNodeTreeView.h
 *
 *  Created on: 2011.10.17.
 *      Author: espakm
 */

#ifndef QMITKNODETREEVIEW_H_
#define QMITKNODETREEVIEW_H_

#include <org_mitk_gui_qt_datamanager_Export.h>

#include <QTreeView>

#include <mitkDataNode.h>

class MITK_QT_DATAMANAGER QmitkNodeTreeView: public QTreeView
{
  Q_OBJECT

public:
  QmitkNodeTreeView(QWidget* parent = 0);
  virtual ~QmitkNodeTreeView();

protected:
  virtual void dropEvent(QDropEvent* event);

private:
  mitk::DataNode::Pointer cloneNode(mitk::DataNode::Pointer node);
  mitk::DataNode::Pointer cloneMask(mitk::DataNode::Pointer mask, mitk::DataNode::Pointer dropOntoNode);

  mitk::Image::Pointer tileImage(mitk::Image::Pointer image, unsigned timeNo);
};

#endif /* QMITKNODETREEVIEW_H_ */
