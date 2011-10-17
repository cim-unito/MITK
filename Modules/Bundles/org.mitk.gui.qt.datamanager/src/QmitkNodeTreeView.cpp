/*
 * QmitkNodeTreeView.cpp
 *
 *  Created on: 2011.10.17.
 *      Author: espakm
 */

#include "QmitkNodeTreeView.h"
#include <QmitkDataStorageTreeModel.h>

#include <QDropEvent>
#include <QMessageBox>
#include <QDebug>

#include <exception>

#include <mitkImage.h>
#include <mitkCloneImageFilter.h>
#include <mitkImageCast.h>
#include <mitkImageToItk.h>

#include <itkTileImageFilter.h>

QmitkNodeTreeView::QmitkNodeTreeView(QWidget* parent)
: QTreeView(parent)
{
  this->setDragEnabled(true);
  this->setDragDropMode(InternalMove);
  this->setDefaultDropAction(Qt::CopyAction);
  this->setDragDropOverwriteMode(false);
}

QmitkNodeTreeView::~QmitkNodeTreeView()
{
}

void
QmitkNodeTreeView::dropEvent(QDropEvent* event)
{
  QmitkDataStorageTreeModel* m = qobject_cast<QmitkDataStorageTreeModel*>(model());
  QModelIndex draggedNodeIndex = this->currentIndex();

  mitk::DataNode::Pointer draggedNode = m->GetNode(draggedNodeIndex);
  std::string draggedNodeName = draggedNode->GetName();
  qDebug() << "dragged node:" << QString::fromStdString(draggedNodeName);

  QModelIndex dropOntoNodeIndex = indexAt(event->pos());
  mitk::DataNode* dropOntoNode = m->GetNode(dropOntoNodeIndex);
  qDebug() << "drop onto node:" << QString::fromStdString(dropOntoNode->GetName());

  mitk::DataNode::Pointer clonedNode;
  try {
    bool binary = false;
    draggedNode->GetBoolProperty("binary", binary);
    if (binary) {
      clonedNode = cloneMask(draggedNode, dropOntoNode);
    }
    else {
      clonedNode = cloneNode(draggedNode);
    }
  }
  catch (std::exception& exception) {
    QMessageBox::critical(0,
        tr("Error"),
        tr("Some error occurred during cloning the image.\n"));
    return;
  }
  catch (...) {
    QMessageBox::critical(0,
        tr("Error"),
        tr("Some error occurred during cloning the image.\n"));
    return;
  }

  mitk::DataNode::Pointer parentOfDraggedNode = m->GetNode(dropOntoNodeIndex.parent());
  std::string name = draggedNode->GetName();
  if (parentOfDraggedNode == dropOntoNode) {
    QString qname = QString::fromStdString(name);
    // check if there is a serial number in the name
    qname += "'";
    clonedNode->SetName(qname.toStdString());
  }
  else {
    clonedNode->SetName(name);
  }

  m->GetDataStorage()->Add(clonedNode, dropOntoNode);
}

mitk::DataNode::Pointer
QmitkNodeTreeView::cloneNode(mitk::DataNode::Pointer node)
{
  mitk::Image* image = dynamic_cast<mitk::Image*>(node->GetData());
  int dim = image->GetDimension();

  mitk::Image::Pointer clonedImage;
  mitk::CloneImageFilter::Pointer cloner = mitk::CloneImageFilter::New();
  cloner->SetInput(image);
  cloner->Update();
  clonedImage = cloner->GetOutput();

  mitk::DataNode::Pointer clonedNode = mitk::DataNode::New();
  clonedNode->SetData(clonedImage);
  return clonedNode;
}

mitk::DataNode::Pointer
QmitkNodeTreeView::cloneMask(mitk::DataNode::Pointer node, mitk::DataNode::Pointer dropOntoNode)
{
  mitk::Image* image = dynamic_cast<mitk::Image*>(node->GetData());
  int dim = image->GetDimension();

  mitk::Image* dropOntoImage = dynamic_cast<mitk::Image*>(dropOntoNode->GetData());
  int dropOntoDim = dropOntoImage->GetDimension();

  mitk::Image::Pointer clonedImage;

  if (dim == dropOntoDim) {
    mitk::CloneImageFilter::Pointer cloner = mitk::CloneImageFilter::New();
    cloner->SetInput(image);
    cloner->Update();
    clonedImage = cloner->GetOutput();
  }
  else if (dim == 3 && dropOntoDim == 4) {
    int timeNo = dropOntoImage->GetDimension(3);
    clonedImage = tileImage(image, timeNo);
  }
  else {
    throw std::runtime_error("dimensions not equal");
  }

  mitk::DataNode::Pointer clonedNode = mitk::DataNode::New();
  clonedNode->SetData(clonedImage);
  return clonedNode;
}

mitk::Image::Pointer
QmitkNodeTreeView::tileImage(mitk::Image::Pointer image, unsigned timeNo)
{
  typedef itk::Image<unsigned char, 3> Mask3D;
  typedef itk::Image<unsigned char, 4> Mask4D;
  typename Mask3D::Pointer itkImage;
  mitk::CastToItkImage(image, itkImage);

  typedef itk::TileImageFilter<Mask3D, Mask4D> Tiler;
  typename Tiler::Pointer tiler = Tiler::New();

  itk::FixedArray<unsigned, 4> layout;
  layout[0] = 1;
  layout[1] = 1;
  layout[2] = 1;
  layout[3] = 0;
  tiler->SetLayout(layout);

  for (unsigned i = 0; i < timeNo; ++i) {
    tiler->SetInput(i, itkImage);
  }
  tiler->Update();
  Mask4D::Pointer itkTiledMask = tiler->GetOutput();

  mitk::Image::Pointer tiledMask;
  mitk::CastToMitkImage(itkTiledMask, tiledMask);

  return tiledMask;
}
