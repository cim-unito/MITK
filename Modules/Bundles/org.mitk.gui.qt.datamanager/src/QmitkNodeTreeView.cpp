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
#include <QInputDialog>
#include <QLineEdit>
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
  mitk::DataStorage::Pointer ds = m->GetDataStorage();

  mitk::DataNode::Pointer draggedNode = m->GetNode(currentIndex());
  const QModelIndex& dropOntoNodeIndex = indexAt(event->pos());
  mitk::DataNode::Pointer dropOntoNode = m->GetNode(dropOntoNodeIndex);
  mitk::DataNode::Pointer parentOfDraggedNode = m->GetNode(dropOntoNodeIndex.parent());

  Qt::DropAction dropAction = event->dropAction();

  mitk::DataNode::Pointer clonedNode;
  try
  {
    bool binary = false;
    draggedNode->GetBoolProperty("binary", binary);
    bool dropOntoGroup = dropOntoNode->GetData() == 0;
    if (dropOntoGroup)
    {
      clonedNode = cloneNode(draggedNode);
    }
    else {
      if (binary)
      {
        clonedNode = cloneMask(draggedNode, dropOntoNode);
      }
      else
      {
        QMessageBox::information(this, tr("Information"),
            tr("It is not supported to copy a node containing a normal\n"
               "image (not segmentation) onto a not empty node."));
        return;
      }
    }
  }
  catch (std::exception& exception)
  {
    QMessageBox msgBox;
    msgBox.setWindowTitle("Error");
    msgBox.setText(tr("Error occurred during copying the image.\n"));
    msgBox.setDetailedText(tr(exception.what()));
    msgBox.exec();
    return;
  }
  catch (...)
  {
    QMessageBox::critical(this, tr("Error"), tr("Error occurred during copying the image.\n"));
    return;
  }

  if (clonedNode.IsNull())
  {
    return;
  }

  std::string name = draggedNode->GetName();
  QString qname = QString::fromStdString(name);
  bool ok = false;
  while (ds->GetNamedDerivedNode(name.c_str(), parentOfDraggedNode, true) &&
      !qname.isNull())
  {
    qname = QInputDialog::getText(this,
        tr(""),
        tr("An image with this name already exists.\n"
           "Please choose a different name:"),
           QLineEdit::Normal, qname, &ok);
  }
  clonedNode->SetName(qname.toStdString());

  ds->Add(clonedNode, dropOntoNode);
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

  if (dim == dropOntoDim)
  {
    for (int i = 0; i < dim; ++i)
    {
      if (image->GetDimension(i) != dropOntoImage->GetDimension(i))
      {
        QMessageBox::critical(this, tr("Error"),
            tr("The dimensions of the segmentation must be equal to\n"
               "the dimension of the image that it is copied to."));
        return 0;
      }
    }

    mitk::CloneImageFilter::Pointer cloner = mitk::CloneImageFilter::New();
    cloner->SetInput(image);
    cloner->Update();
    clonedImage = cloner->GetOutput();
  }
  else if (dropOntoDim == dim + 1)
  {
    for (int i = 0; i < dim; ++i)
    {
      if (image->GetDimension(i) != dropOntoImage->GetDimension(i))
      {
        QMessageBox::critical(this, tr("Error"),
            tr("The dimensions of the segmentation must be equal to\n"
               "the dimension of the image that it is copied to."));
        return 0;
      }
    }
    // This gets the last dimenstion that is the time in 3D+t images:
    int timeNo = dropOntoImage->GetDimension(dim);
//    QMessageBox::StandardButton answer =
//        QMessageBox::question(this, tr(""),
//            tr("You are dragging a 3D segmentation image onto a 3D+t\n"
//                "reference image. This will result in a 3D+t segmentation\n"
//                "image. As default, the copied segmentation will be\n"
//                "propagated to all time steps. If you want to copy the\n"
//                "segmentation only to the first time step, select \"No\".\n"
//                "Do you want to copy the segmentation to all time steps?"),
//            QMessageBox::Yes | QMessageBox::No);
    clonedImage = tileImage(image, timeNo);
  }
  else
  {
    QString message = QString("Copying a %1D segmentation onto a %2D image\n"
           "is not supported.").arg(dim).arg(dropOntoDim);
    QMessageBox::critical(this, tr("Error"), message);
    return 0;
  }

  mitk::DataNode::Pointer clonedNode = mitk::DataNode::New();
  clonedNode->SetData(clonedImage);
  return clonedNode;
}

// This should be generalized to other dimensions as well.
mitk::Image::Pointer
QmitkNodeTreeView::tileImage(mitk::Image::Pointer image, unsigned timeNo)
{
  typedef itk::Image<unsigned char, 3> Mask3D;
  typedef itk::Image<unsigned char, 4> Mask4D;

  Mask3D::Pointer itkImage;
  mitk::CastToItkImage(image, itkImage);

  typedef itk::TileImageFilter<Mask3D, Mask4D> Tiler;
  Tiler::Pointer tiler = Tiler::New();

  itk::FixedArray<unsigned, 4> layout;
  layout[0] = 1;
  layout[1] = 1;
  layout[2] = 1;
  layout[3] = 0;
  tiler->SetLayout(layout);

  for (unsigned i = 0; i < timeNo; ++i)
  {
    tiler->SetInput(i, itkImage);
  }
  tiler->Update();
  Mask4D::Pointer itkTiledMask = tiler->GetOutput();

  mitk::Image::Pointer tiledMask;
  mitk::CastToMitkImage(itkTiledMask, tiledMask);

  return tiledMask;
}
