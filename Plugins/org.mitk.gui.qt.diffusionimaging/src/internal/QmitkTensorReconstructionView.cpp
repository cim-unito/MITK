/*=========================================================================

Program:   Medical Imaging & Interaction Toolkit
Module:    $RCSfile$
Language:  C++
Date:      $Date: 2009-05-28 17:19:30 +0200 (Do, 28 Mai 2009) $
Version:   $Revision: 17495 $

Copyright (c) German Cancer Research Center, Division of Medical and
Biological Informatics. All rights reserved.
See MITKCopyright.txt or http://www.mitk.org/copyright.html for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "QmitkTensorReconstructionView.h"
#include "mitkDiffusionImagingConfigure.h"

// qt includes
#include <QMessageBox>
#include <QImage>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsLinearLayout>


// itk includes
#include "itkTimeProbe.h"
//#include "itkTensor.h"

// mitk includes
#include "mitkProgressBar.h"
#include "mitkStatusBar.h"

#include "mitkNodePredicateDataType.h"
#include "QmitkDataStorageComboBox.h"
#include "QmitkStdMultiWidget.h"

#include "mitkTeemDiffusionTensor3DReconstructionImageFilter.h"
#include "itkDiffusionTensor3DReconstructionImageFilter.h"
#include "itkTensorImageToDiffusionImageFilter.h"
#include "itkPointShell.h"
#include "itkVector.h"
#include "itkB0ImageExtractionImageFilter.h"

#include "mitkProperties.h"
#include "mitkDataNodeObject.h"
#include "mitkOdfNormalizationMethodProperty.h"
#include "mitkOdfScaleByProperty.h"
#include "mitkDiffusionImageMapper.h"
#include "mitkLookupTableProperty.h"
#include "mitkLookupTable.h"
#include "mitkImageStatisticsHolder.h"

#include <itkTensorImageToQBallImageFilter.h>
#include <itkResidualImageFilter.h>


const std::string QmitkTensorReconstructionView::VIEW_ID =
"org.mitk.views.tensorreconstruction";

#define DI_INFO MITK_INFO("DiffusionImaging")

typedef float                                       TTensorPixelType;
typedef itk::DiffusionTensor3D< TTensorPixelType >  TensorPixelType;
typedef itk::Image< TensorPixelType, 3 >            TensorImageType;

QmitkTensorReconstructionView::QmitkTensorReconstructionView()
  : QmitkFunctionality(),
    m_Controls(NULL),
    m_MultiWidget(NULL)
{
  m_DiffusionImages = mitk::DataStorage::SetOfObjects::New();
  m_TensorImages = mitk::DataStorage::SetOfObjects::New();
}

QmitkTensorReconstructionView::QmitkTensorReconstructionView(const QmitkTensorReconstructionView& other)
{
  Q_UNUSED(other)
  throw std::runtime_error("Copy constructor not implemented");
}

QmitkTensorReconstructionView::~QmitkTensorReconstructionView()
{

}

void QmitkTensorReconstructionView::CreateQtPartControl(QWidget *parent)
{
  if (!m_Controls)
  {
    // create GUI widgets
    m_Controls = new Ui::QmitkTensorReconstructionViewControls;
    m_Controls->setupUi(parent);
    this->CreateConnections();

    QStringList items;
    items << "LLS (Linear Least Squares)"
          << "MLE (Maximum Likelihood)"
          << "NLS (Nonlinear Least Squares)"
          << "WLS (Weighted Least Squares)";
    m_Controls->m_TensorEstimationTeemEstimationMethodCombo->addItems(items);
    m_Controls->m_TensorEstimationTeemEstimationMethodCombo->setCurrentIndex(0);

    m_Controls->m_TensorEstimationManualThreashold->setChecked(false);
    m_Controls->m_TensorEstimationTeemSigmaEdit->setText("NaN");
    m_Controls->m_TensorEstimationTeemNumItsSpin->setValue(1);
    m_Controls->m_TensorEstimationTeemFuzzyEdit->setText("0.0");
    m_Controls->m_TensorEstimationTeemMinValEdit->setText("1.0");

    m_Controls->m_TensorEstimationTeemNumItsLabel_2->setEnabled(true);
    m_Controls->m_TensorEstimationTeemNumItsSpin->setEnabled(true);

    m_Controls->m_TensorsToDWIBValueEdit->setText("1000");

    Advanced1CheckboxClicked();
    Advanced2CheckboxClicked();
    TeemCheckboxClicked();

#ifndef DIFFUSION_IMAGING_EXTENDED
    m_Controls->m_TeemToggle->setVisible(false);
#endif

    // define data type for combobox
    //m_Controls->m_ImageSelector->SetDataStorage( this->GetDefaultDataStorage() );
    //m_Controls->m_ImageSelector->SetPredicate( mitk::NodePredicateDataType::New("DiffusionImage") );
  }
}

void QmitkTensorReconstructionView::StdMultiWidgetAvailable (QmitkStdMultiWidget &stdMultiWidget)
{
  m_MultiWidget = &stdMultiWidget;
}

void QmitkTensorReconstructionView::StdMultiWidgetNotAvailable()
{
  m_MultiWidget = NULL;
}

void QmitkTensorReconstructionView::CreateConnections()
{
  if ( m_Controls )
  {
    connect( (QObject*)(m_Controls->m_TeemToggle), SIGNAL(clicked()), this, SLOT(TeemCheckboxClicked()) );
    connect( (QObject*)(m_Controls->m_ItkReconstruction), SIGNAL(clicked()), this, SLOT(ItkReconstruction()) );
    connect( (QObject*)(m_Controls->m_TeemReconstruction), SIGNAL(clicked()), this, SLOT(TeemReconstruction()) );
    connect( (QObject*)(m_Controls->m_TensorEstimationTeemEstimationMethodCombo), SIGNAL(currentIndexChanged(int)), this, SLOT(MethodChoosen(int)) );
    connect( (QObject*)(m_Controls->m_Advanced1), SIGNAL(clicked()), this, SLOT(Advanced1CheckboxClicked()) );
    connect( (QObject*)(m_Controls->m_Advanced2), SIGNAL(clicked()), this, SLOT(Advanced2CheckboxClicked()) );
    connect( (QObject*)(m_Controls->m_TensorEstimationManualThreashold), SIGNAL(clicked()), this, SLOT(ManualThresholdClicked()) );
    connect( (QObject*)(m_Controls->m_TensorsToDWIButton), SIGNAL(clicked()), this, SLOT(TensorsToDWI()) );
    connect( (QObject*)(m_Controls->m_TensorsToQbiButton), SIGNAL(clicked()), this, SLOT(TensorsToQbi()) );
    connect( (QObject*)(m_Controls->m_ResidualButton), SIGNAL(clicked()), this, SLOT(ResidualCalculation()) );
    connect( (QObject*)(m_Controls->m_PerSliceView), SIGNAL(pointSelected(int, int)), this, SLOT(ResidualClicked(int, int)) );
  }
}

void QmitkTensorReconstructionView::ResidualClicked(int slice, int volume)
{
  // Use image coord to reset crosshair

  // Find currently selected diffusion image

  // Update Label


  // to do: This position should be modified in order to skip B0 volumes that are not taken into account
  // when calculating residuals

  // Find the diffusion image
  mitk::DiffusionImage<DiffusionPixelType>* diffImage;
  mitk::DataNode::Pointer correctNode;
  mitk::Geometry3D* geometry;

  if (m_DiffusionImage.IsNotNull())
  {
    diffImage = static_cast<mitk::DiffusionImage<DiffusionPixelType>*>(m_DiffusionImage->GetData());

    geometry = diffImage->GetGeometry();

    // Remember the node whose display index must be updated
    correctNode = mitk::DataNode::New();
    correctNode = m_DiffusionImage;
  }

  if(diffImage != NULL)
  {
    typedef vnl_vector_fixed< double, 3 >       GradientDirectionType;
    typedef itk::VectorContainer< unsigned int,
        GradientDirectionType >                   GradientDirectionContainerType;

    GradientDirectionContainerType::Pointer dirs = diffImage->GetDirections();

    for(int i=0; i<dirs->Size() && i<=volume; i++)
    {
      GradientDirectionType grad = dirs->ElementAt(i);

      // check if image is b0 weighted
      if(fabs(grad[0]) < 0.001 && fabs(grad[1]) < 0.001 && fabs(grad[2]) < 0.001)
      {
        volume++;
      }
    }

    QString pos = "Volume: ";
    pos.append(QString::number(volume));
    pos.append(", Slice: ");
    pos.append(QString::number(slice));
    m_Controls->m_PositionLabel->setText(pos);

    if(correctNode)
    {
      int oldDisplayVal;
      correctNode->GetIntProperty("DisplayChannel", oldDisplayVal);
      std::string oldVal = QString::number(oldDisplayVal).toStdString();
      std::string newVal = QString::number(volume).toStdString();
      correctNode->SetIntProperty("DisplayChannel",volume);
      correctNode->SetSelected(true);
      this->FirePropertyChanged("DisplayChannel", oldVal, newVal);
      correctNode->UpdateOutputInformation();


      mitk::Point3D p3 = m_MultiWidget->GetCrossPosition();
      itk::Index<3> ix;
      geometry->WorldToIndex(p3, ix);
      // ix[2] = slice;

      mitk::Vector3D vec;
      vec[0] = ix[0];
      vec[1] = ix[1];
      vec[2] = slice;

      mitk::Vector3D v3New;
      geometry->IndexToWorld(vec, v3New);
      mitk::Point3D origin = geometry->GetOrigin();
      mitk::Point3D p3New;
      p3New[0] = v3New[0] + origin[0];
      p3New[1] = v3New[1] + origin[1];
      p3New[2] = v3New[2] + origin[2];

      m_MultiWidget->MoveCrossToPosition(p3New);
      m_MultiWidget->RequestUpdate();
    }
  }
}

void QmitkTensorReconstructionView::TeemCheckboxClicked()
{
  m_Controls->groupBox_3->setVisible(m_Controls->
                                     m_TeemToggle->isChecked());
}

void QmitkTensorReconstructionView::Advanced1CheckboxClicked()
{
  bool check = m_Controls->
      m_Advanced1->isChecked();

  m_Controls->frame->setVisible(check);
}

void QmitkTensorReconstructionView::Advanced2CheckboxClicked()
{
  bool check = m_Controls->
      m_Advanced2->isChecked();

  m_Controls->frame_2->setVisible(check);
}

void QmitkTensorReconstructionView::ManualThresholdClicked()
{
  m_Controls->m_TensorReconstructionThreasholdEdit_2->setEnabled(
        m_Controls->m_TensorEstimationManualThreashold->isChecked());
}

void QmitkTensorReconstructionView::Activated()
{
  QmitkFunctionality::Activated();
}

void QmitkTensorReconstructionView::Deactivated()
{
  QmitkFunctionality::Deactivated();
}

void QmitkTensorReconstructionView::MethodChoosen(int method)
{
  m_Controls->m_TensorEstimationTeemNumItsLabel_2->setEnabled(method==3);
  m_Controls->m_TensorEstimationTeemNumItsSpin->setEnabled(method==3);
}

void QmitkTensorReconstructionView::ResidualCalculation()
{
  // Extract dwi and dti from current selection
  // In case of multiple selections, take the first one, since taking all combinations is not meaningful

  mitk::DataStorage::SetOfObjects::Pointer set =
      mitk::DataStorage::SetOfObjects::New();

  mitk::DiffusionImage<DiffusionPixelType>::Pointer diffImage
      = mitk::DiffusionImage<DiffusionPixelType>::New();

  TensorImageType::Pointer tensorImage;

  std::string nodename;

  if(m_DiffusionImage.IsNotNull())
  {
    diffImage = static_cast<mitk::DiffusionImage<DiffusionPixelType>*>(m_DiffusionImage->GetData());
  }
  else
    return;
  if(m_TensorImage.IsNotNull())
  {
    mitk::TensorImage* mitkVol;
    mitkVol = static_cast<mitk::TensorImage*>(m_TensorImage->GetData());
    mitk::CastToItkImage<TensorImageType>(mitkVol, tensorImage);
    m_TensorImage->GetStringProperty("name", nodename);
  }
  else
    return;

  typedef itk::TensorImageToDiffusionImageFilter<
      TTensorPixelType, DiffusionPixelType > FilterType;

  FilterType::GradientListType gradientList;
  mitk::DiffusionImage<DiffusionPixelType>::GradientDirectionContainerType* gradients
      = diffImage->GetDirections();

  // Copy gradients vectors from gradients to gradientList
  for(int i=0; i<gradients->Size(); i++)
  {
    mitk::DiffusionImage<DiffusionPixelType>::GradientDirectionType vec = gradients->at(i);
    itk::Vector<double,3> grad;

    grad[0] = vec[0];
    grad[1] = vec[1];
    grad[2] = vec[2];

    gradientList.push_back(grad);
  }

  // Find the min and the max values from a baseline image
  mitk::ImageStatisticsHolder *stats = diffImage->GetStatistics();

  //Initialize filter that calculates the modeled diffusion weighted signals
  FilterType::Pointer filter = FilterType::New();
  filter->SetInput( tensorImage );
  filter->SetBValue(diffImage->GetB_Value());
  filter->SetGradientList(gradientList);
  filter->SetMin(stats->GetScalarValueMin());
  filter->SetMax(500);
  filter->Update();


  // TENSORS TO DATATREE
  mitk::DiffusionImage<DiffusionPixelType>::Pointer image = mitk::DiffusionImage<DiffusionPixelType>::New();
  image->SetVectorImage( filter->GetOutput() );
  image->SetB_Value(diffImage->GetB_Value());
  image->SetDirections(gradientList);
  image->SetOriginalDirections(gradientList);
  image->InitializeFromVectorImage();
  mitk::DataNode::Pointer node = mitk::DataNode::New();
  node->SetData( image );
  mitk::DiffusionImageMapper<short>::SetDefaultProperties(node);

  QString newname;
  newname = newname.append(nodename.c_str());
  newname = newname.append("_dwi");
  node->SetName(newname.toAscii());


  GetDefaultDataStorage()->Add(node);


  std::vector<int> b0Indices = image->GetB0Indices();



  typedef itk::ResidualImageFilter<DiffusionPixelType, float> ResidualImageFilterType;

  ResidualImageFilterType::Pointer residualFilter = ResidualImageFilterType::New();
  residualFilter->SetInput(diffImage->GetVectorImage());
  residualFilter->SetSecondDiffusionImage(image->GetVectorImage());
  residualFilter->SetGradients(gradients);
  residualFilter->SetB0Index(b0Indices[0]);
  residualFilter->SetB0Threshold(30);
  residualFilter->Update();

  itk::Image<float, 3>::Pointer residualImage = itk::Image<float, 3>::New();
  residualImage = residualFilter->GetOutput();

  mitk::Image::Pointer mitkResImg = mitk::Image::New();

  mitk::CastToMitkImage(residualImage, mitkResImg);

  stats = mitkResImg->GetStatistics();
  float min = stats->GetScalarValueMin();
  float max = stats->GetScalarValueMax();

  mitk::LookupTableProperty::Pointer lutProp = mitk::LookupTableProperty::New();
  mitk::LookupTable::Pointer lut = mitk::LookupTable::New();


  vtkSmartPointer<vtkLookupTable> lookupTable =
      vtkSmartPointer<vtkLookupTable>::New();

  lookupTable->SetTableRange(min, max);


  // If you don't want to use the whole color range, you can use
  // SetValueRange, SetHueRange, and SetSaturationRange
  lookupTable->Build();

  int size = lookupTable->GetTable()->GetSize();

  vtkSmartPointer<vtkLookupTable> reversedlookupTable =
      vtkSmartPointer<vtkLookupTable>::New();
  reversedlookupTable->SetTableRange(min+1, max);
  reversedlookupTable->Build();

  for(int i=0; i<256; i++)
  {
    double* rgba = reversedlookupTable->GetTableValue(255-i);

    lookupTable->SetTableValue(i, rgba[0], rgba[1], rgba[2], rgba[3]);
  }

  lut->SetVtkLookupTable(lookupTable);
  lutProp->SetLookupTable(lut);

  // Create lookuptable

  mitk::DataNode::Pointer resNode=mitk::DataNode::New();
  resNode->SetData( mitkResImg );
  resNode->SetName("Residual Image");

  resNode->SetProperty("LookupTable", lutProp);

  bool b;
  resNode->GetBoolProperty("use color", b);
  resNode->SetBoolProperty("use color", false);

  GetDefaultDataStorage()->Add(resNode);

  m_MultiWidget->RequestUpdate();



  // Draw Graph
  std::vector<double> means = residualFilter->GetMeans();
  std::vector<double> q1s = residualFilter->GetQ1();
  std::vector<double> q3s = residualFilter->GetQ3();
  std::vector<double> percentagesOfOUtliers = residualFilter->GetPercentagesOfOutliers();

  m_Controls->m_ResidualAnalysis->SetMeans(means);
  m_Controls->m_ResidualAnalysis->SetQ1(q1s);
  m_Controls->m_ResidualAnalysis->SetQ3(q3s);
  m_Controls->m_ResidualAnalysis->SetPercentagesOfOutliers(percentagesOfOUtliers);

  if(m_Controls->m_PercentagesOfOutliers->isChecked())
  {
    m_Controls->m_ResidualAnalysis->DrawPercentagesOfOutliers();
  }
  else
  {
    m_Controls->m_ResidualAnalysis->DrawMeans();
  }



  // Draw Graph for volumes per slice in the QGraphicsView
  std::vector< std::vector<double> > outliersPerSlice = residualFilter->GetOutliersPerSlice();
  int xSize = outliersPerSlice.size();
  if(xSize == 0)
  {
    return;
  }
  int ySize = outliersPerSlice[0].size();


  // Find maximum in outliersPerSlice
  double maxOutlier= 0.0;
  for(int i=0; i<xSize; i++)
  {
    for(int j=0; j<ySize; j++)
    {
      if(outliersPerSlice[i][j]>maxOutlier)
      {
        maxOutlier = outliersPerSlice[i][j];
      }
    }
  }


  // Create some QImage
  QImage qImage(xSize, ySize, QImage::Format_RGB32);
  QImage legend(1, 256, QImage::Format_RGB32);
  QRgb value;

  vtkSmartPointer<vtkLookupTable> lookup =
      vtkSmartPointer<vtkLookupTable>::New();

  lookup->SetTableRange(0.0, maxOutlier);
  lookup->Build();

  reversedlookupTable->SetTableRange(0, maxOutlier);
  reversedlookupTable->Build();

  for(int i=0; i<256; i++)
  {
    double* rgba = reversedlookupTable->GetTableValue(255-i);
    lookup->SetTableValue(i, rgba[0], rgba[1], rgba[2], rgba[3]);
  }


  // Fill qImage
  for(int i=0; i<xSize; i++)
  {
    for(int j=0; j<ySize; j++)
    {
      double out = outliersPerSlice[i][j];

      unsigned char *_rgba = lookup->MapValue(out);
      int r, g, b;
      r = _rgba[0];
      g = _rgba[1];
      b = _rgba[2];

      value = qRgb(r, g, b);

      qImage.setPixel(i,j,value);

    }
  }

  for(int i=0; i<256; i++)
  {
    double* rgba = lookup->GetTableValue(i);
    int r, g, b;
    r = rgba[0]*255;
    g = rgba[1]*255;
    b = rgba[2]*255;
    value = qRgb(r, g, b);
    legend.setPixel(0,255-i,value);
  }

  QString upper = QString::number(maxOutlier, 'g', 3);
  upper.append(" %");
  QString lower = QString::number(0.0);
  lower.append(" %");
  m_Controls->m_UpperLabel->setText(upper);
  m_Controls->m_LowerLabel->setText(lower);

  QGraphicsScene* scene = new QGraphicsScene;
  QGraphicsScene* scene2 = new QGraphicsScene;


  QPixmap pixmap(QPixmap::fromImage(qImage));
  QGraphicsPixmapItem *item = new QGraphicsPixmapItem( pixmap, 0, scene);
  item->scale(10.0, 3.0);

  QPixmap pixmap2(QPixmap::fromImage(legend));
  QGraphicsPixmapItem *item2 = new QGraphicsPixmapItem( pixmap2, 0, scene2);
  item2->scale(20.0, 1.0);

  m_Controls->m_PerSliceView->SetResidualPixmapItem(item);



  m_Controls->m_PerSliceView->setScene(scene);
  m_Controls->m_LegendView->setScene(scene2);
  m_Controls->m_PerSliceView->show();
  m_Controls->m_PerSliceView->repaint();

  m_Controls->m_LegendView->setHorizontalScrollBarPolicy ( Qt::ScrollBarAlwaysOff );
  m_Controls->m_LegendView->setVerticalScrollBarPolicy ( Qt::ScrollBarAlwaysOff );
  m_Controls->m_LegendView->show();
  m_Controls->m_LegendView->repaint();
}

void QmitkTensorReconstructionView::ItkReconstruction()
{
  Reconstruct(0);
}

void QmitkTensorReconstructionView::TeemReconstruction()
{
  Reconstruct(1);
}

void QmitkTensorReconstructionView::Reconstruct(int method)
{
  if(method == 0)
    ItkTensorReconstruction(m_DiffusionImages);

  if(method == 1)
    TeemTensorReconstruction(m_DiffusionImages);
}

void QmitkTensorReconstructionView::ItkTensorReconstruction
(mitk::DataStorage::SetOfObjects::Pointer inImages)
{
  try
  {
    itk::TimeProbe clock;

    int nrFiles = inImages->size();
    if (!nrFiles) return;

    QString status;
    mitk::ProgressBar::GetInstance()->AddStepsToDo(nrFiles);

    mitk::DataStorage::SetOfObjects::const_iterator itemiter( inImages->begin() );
    mitk::DataStorage::SetOfObjects::const_iterator itemiterend( inImages->end() );

    std::vector<mitk::DataNode::Pointer> nodes;
    while ( itemiter != itemiterend ) // for all items
    {

      mitk::DiffusionImage<DiffusionPixelType>* vols =
          static_cast<mitk::DiffusionImage<DiffusionPixelType>*>(
            (*itemiter)->GetData());

      std::string nodename;
      (*itemiter)->GetStringProperty("name", nodename);
      ++itemiter;

      // TENSOR RECONSTRUCTION
      clock.Start();
      MBI_INFO << "Tensor reconstruction ";
      mitk::StatusBar::GetInstance()->DisplayText(status.sprintf(
                                                    "Tensor reconstruction for %s", nodename.c_str()).toAscii());
      typedef itk::DiffusionTensor3DReconstructionImageFilter<
          DiffusionPixelType, DiffusionPixelType, TTensorPixelType > TensorReconstructionImageFilterType;
      TensorReconstructionImageFilterType::Pointer tensorReconstructionFilter =
          TensorReconstructionImageFilterType::New();
      tensorReconstructionFilter->SetGradientImage( vols->GetDirections(), vols->GetVectorImage() );
      tensorReconstructionFilter->SetBValue(vols->GetB_Value());
      tensorReconstructionFilter->SetThreshold( m_Controls->m_TensorReconstructionThreasholdEdit->text().toFloat() );
      tensorReconstructionFilter->Update();
      clock.Stop();
      MBI_DEBUG << "took " << clock.GetMeanTime() << "s.";

      // TENSORS TO DATATREE
      mitk::TensorImage::Pointer image = mitk::TensorImage::New();


      typedef itk::Image<itk::DiffusionTensor3D<TTensorPixelType>, 3> TensorImageType;

      TensorImageType::Pointer tensorImage;
      tensorImage = tensorReconstructionFilter->GetOutput();


      // Check the tensor for negative eigenvalues
      if(m_Controls->m_CheckNegativeEigenvalues->isChecked())
      {
        typedef itk::ImageRegionIterator<TensorImageType> TensorImageIteratorType;
        TensorImageIteratorType tensorIt(tensorImage, tensorImage->GetRequestedRegion());
        tensorIt.GoToBegin();

        while(!tensorIt.IsAtEnd())
        {

          typedef itk::DiffusionTensor3D<TTensorPixelType> TensorType;
          //typedef itk::Tensor<TTensorPixelType, 3> TensorType2;

          TensorType tensor = tensorIt.Get();
          // TensorType2 tensor2;

          /*
          for(int i=0; i<tensor.GetNumberOfComponents(); i++)
          {
            tensor2.SetNthComponent(i, tensor.GetNthComponent(i));
          }

          typedef vnl_symmetric_eigensystem< TTensorPixelType >  SymEigenSystemType;
          SymEigenSystemType eig (tensor2.GetVnlMatrix());
          for(unsigned int i=0; i<eig.D.size(); i++)
          {
            if (eig.D[i] < 0.0 )
            {
              tensor.Fill(0.0);
              tensorIt.Set(tensor);
            }
          }*/


          TensorType::EigenValuesArrayType ev;
          tensor.ComputeEigenValues(ev);
          for(unsigned int i=0; i<ev.Size(); i++)
          {
            if(ev[i] < 0.0)
            {
              tensor.Fill(0.0);
              tensorIt.Set(tensor);
              break;
            }
          }


          ++tensorIt;
        }
      }


      image->InitializeByItk( tensorImage.GetPointer() );
      image->SetVolume( tensorReconstructionFilter->GetOutput()->GetBufferPointer() );
      mitk::DataNode::Pointer node=mitk::DataNode::New();
      node->SetData( image );

      QString newname;
      newname = newname.append(nodename.c_str());
      newname = newname.append("_dti");

      SetDefaultNodeProperties(node, newname.toStdString());
      nodes.push_back(node);

      mitk::ProgressBar::GetInstance()->Progress();

    }

    std::vector<mitk::DataNode::Pointer>::iterator nodeIt;
    for(nodeIt = nodes.begin(); nodeIt != nodes.end(); ++nodeIt)
      GetDefaultDataStorage()->Add(*nodeIt);

    mitk::StatusBar::GetInstance()->DisplayText(status.sprintf("Finished Processing %d Files", nrFiles).toAscii());
    m_MultiWidget->RequestUpdate();

  }
  catch (itk::ExceptionObject &ex)
  {
    MBI_INFO << ex ;
    return ;
  }
}



void QmitkTensorReconstructionView::TeemTensorReconstruction
(mitk::DataStorage::SetOfObjects::Pointer inImages)
{
  try
  {
    itk::TimeProbe clock;

    int nrFiles = inImages->size();
    if (!nrFiles) return;

    QString status;
    mitk::ProgressBar::GetInstance()->AddStepsToDo(nrFiles);

    mitk::DataStorage::SetOfObjects::const_iterator itemiter( inImages->begin() );
    mitk::DataStorage::SetOfObjects::const_iterator itemiterend( inImages->end() );

    std::vector<mitk::DataNode::Pointer> nodes;
    while ( itemiter != itemiterend ) // for all items
    {

      mitk::DiffusionImage<DiffusionPixelType>* vols =
          static_cast<mitk::DiffusionImage<DiffusionPixelType>*>(
            (*itemiter)->GetData());

      std::string nodename;
      (*itemiter)->GetStringProperty("name", nodename);
      ++itemiter;

      // TENSOR RECONSTRUCTION
      clock.Start();
      MBI_INFO << "Teem Tensor reconstruction ";
      mitk::StatusBar::GetInstance()->DisplayText(status.sprintf(
                                                    "Teem Tensor reconstruction for %s", nodename.c_str()).toAscii());
      typedef mitk::TeemDiffusionTensor3DReconstructionImageFilter<
          DiffusionPixelType, TTensorPixelType > TensorReconstructionImageFilterType;
      TensorReconstructionImageFilterType::Pointer tensorReconstructionFilter =
          TensorReconstructionImageFilterType::New();
      tensorReconstructionFilter->SetInput( vols );
      if(!m_Controls->m_TensorEstimationTeemSigmaEdit->text().contains(QString("NaN")))
        tensorReconstructionFilter->SetSigma( m_Controls->m_TensorEstimationTeemSigmaEdit->text().toFloat() );
      switch(m_Controls->m_TensorEstimationTeemEstimationMethodCombo->currentIndex())
      {
      //  items << "LLS (Linear Least Squares)"
      //<< "MLE (Maximum Likelihood)"
      //<< "NLS (Nonlinear Least Squares)"
      //<< "WLS (Weighted Least Squares)";
      case 0:
        tensorReconstructionFilter->SetEstimationMethod(mitk::TeemTensorEstimationMethodsLLS);
        break;
      case 1:
        tensorReconstructionFilter->SetEstimationMethod(mitk::TeemTensorEstimationMethodsMLE);
        break;
      case 2:
        tensorReconstructionFilter->SetEstimationMethod(mitk::TeemTensorEstimationMethodsNLS);
        break;
      case 3:
        tensorReconstructionFilter->SetEstimationMethod(mitk::TeemTensorEstimationMethodsWLS);
        break;
      default:
        tensorReconstructionFilter->SetEstimationMethod(mitk::TeemTensorEstimationMethodsLLS);
      }
      tensorReconstructionFilter->SetNumIterations( m_Controls->m_TensorEstimationTeemNumItsSpin->value() );
      if(m_Controls->m_TensorEstimationManualThreashold->isChecked())
        tensorReconstructionFilter->SetConfidenceThreshold( m_Controls->m_TensorReconstructionThreasholdEdit_2->text().toDouble() );
      tensorReconstructionFilter->SetConfidenceFuzzyness( m_Controls->m_TensorEstimationTeemFuzzyEdit->text().toFloat() );
      tensorReconstructionFilter->SetMinPlausibleValue( m_Controls->m_TensorEstimationTeemMinValEdit->text().toDouble() );
      tensorReconstructionFilter->Update();
      clock.Stop();
      MBI_DEBUG << "took " << clock.GetMeanTime() << "s." ;

      // TENSORS TO DATATREE
      mitk::DataNode::Pointer node2=mitk::DataNode::New();
      node2->SetData( tensorReconstructionFilter->GetOutputItk() );

      QString newname;
      newname = newname.append(nodename.c_str());
      newname = newname.append("_dtix");

      SetDefaultNodeProperties(node2, newname.toStdString());
      nodes.push_back(node2);

      mitk::ProgressBar::GetInstance()->Progress();

    }

    std::vector<mitk::DataNode::Pointer>::iterator nodeIt;
    for(nodeIt = nodes.begin(); nodeIt != nodes.end(); ++nodeIt)
      GetDefaultDataStorage()->Add(*nodeIt);

    mitk::StatusBar::GetInstance()->DisplayText(status.sprintf("Finished Processing %d Files", nrFiles).toAscii());
    m_MultiWidget->RequestUpdate();

  }
  catch (itk::ExceptionObject &ex)
  {
    MBI_INFO << ex ;
    return ;
  }
}

void QmitkTensorReconstructionView::SetDefaultNodeProperties(mitk::DataNode::Pointer node, std::string name)
{
  node->SetProperty( "ShowMaxNumber", mitk::IntProperty::New( 500 ) );
  node->SetProperty( "Scaling", mitk::FloatProperty::New( 1.0 ) );
  node->SetProperty( "Normalization", mitk::OdfNormalizationMethodProperty::New());
  node->SetProperty( "ScaleBy", mitk::OdfScaleByProperty::New());
  node->SetProperty( "IndexParam1", mitk::FloatProperty::New(2));
  node->SetProperty( "IndexParam2", mitk::FloatProperty::New(1));
  node->SetProperty( "visible", mitk::BoolProperty::New( true ) );
  node->SetProperty( "VisibleOdfs", mitk::BoolProperty::New( false ) );
  node->SetProperty ("layer", mitk::IntProperty::New(100));
  node->SetProperty( "DoRefresh", mitk::BoolProperty::New( true ) );
  //node->SetProperty( "opacity", mitk::FloatProperty::New(1.0f) );

  node->SetProperty( "name", mitk::StringProperty::New(name) );
}


//node->SetProperty( "volumerendering", mitk::BoolProperty::New( false ) );
//node->SetProperty( "use color", mitk::BoolProperty::New( true ) );
//node->SetProperty( "texture interpolation", mitk::BoolProperty::New( true ) );
//node->SetProperty( "reslice interpolation", mitk::VtkResliceInterpolationProperty::New() );
//node->SetProperty( "layer", mitk::IntProperty::New(0));
//node->SetProperty( "in plane resample extent by geometry", mitk::BoolProperty::New( false ) );
//node->SetOpacity(1.0f);
//node->SetColor(1.0,1.0,1.0);
//node->SetVisibility(true);
//node->SetProperty( "IsTensorVolume", mitk::BoolProperty::New( true ) );

//mitk::LevelWindowProperty::Pointer levWinProp = mitk::LevelWindowProperty::New();
//mitk::LevelWindow levelwindow;
////  levelwindow.SetAuto( image );
//levWinProp->SetLevelWindow( levelwindow );
//node->GetPropertyList()->SetProperty( "levelwindow", levWinProp );

//// add a default rainbow lookup table for color mapping
//if(!node->GetProperty("LookupTable"))
//{
//  mitk::LookupTable::Pointer mitkLut = mitk::LookupTable::New();
//  vtkLookupTable* vtkLut = mitkLut->GetVtkLookupTable();
//  vtkLut->SetHueRange(0.6667, 0.0);
//  vtkLut->SetTableRange(0.0, 20.0);
//  vtkLut->Build();
//  mitk::LookupTableProperty::Pointer mitkLutProp = mitk::LookupTableProperty::New();
//  mitkLutProp->SetLookupTable(mitkLut);
//  node->SetProperty( "LookupTable", mitkLutProp );
//}
//if(!node->GetProperty("binary"))
//  node->SetProperty( "binary", mitk::BoolProperty::New( false ) );

//// add a default transfer function
//mitk::TransferFunction::Pointer tf = mitk::TransferFunction::New();
//node->SetProperty ( "TransferFunction", mitk::TransferFunctionProperty::New ( tf.GetPointer() ) );

//// set foldername as string property
//mitk::StringProperty::Pointer nameProp = mitk::StringProperty::New( name );
//node->SetProperty( "name", nameProp );

void QmitkTensorReconstructionView::TensorsToDWI()
{
  DoTensorsToDWI(m_TensorImages);
}

void QmitkTensorReconstructionView::TensorsToQbi()
{
  for (int i=0; i<m_TensorImages->size(); i++)
  {
    mitk::DataNode::Pointer tensorImageNode = m_TensorImages->at(i);
    MITK_INFO << "starting Q-Ball estimation";

    typedef float                                       TTensorPixelType;
    typedef itk::DiffusionTensor3D< TTensorPixelType >  TensorPixelType;
    typedef itk::Image< TensorPixelType, 3 >            TensorImageType;

    TensorImageType::Pointer itkvol = TensorImageType::New();
    mitk::CastToItkImage<TensorImageType>(dynamic_cast<mitk::TensorImage*>(tensorImageNode->GetData()), itkvol);

    typedef itk::TensorImageToQBallImageFilter< TTensorPixelType, TTensorPixelType > FilterType;
    FilterType::Pointer filter = FilterType::New();
    filter->SetInput( itkvol );
    filter->Update();

    typedef itk::Vector<TTensorPixelType,QBALL_ODFSIZE>  OutputPixelType;
    typedef itk::Image<OutputPixelType,3>                OutputImageType;

    mitk::QBallImage::Pointer image = mitk::QBallImage::New();
    OutputImageType::Pointer outimg = filter->GetOutput();
    image->InitializeByItk( outimg.GetPointer() );
    image->SetVolume( outimg->GetBufferPointer() );
    mitk::DataNode::Pointer node = mitk::DataNode::New();
    node->SetData( image );
    QString newname;
    newname = newname.append(tensorImageNode->GetName().c_str());
    newname = newname.append("_qbi");
    node->SetName(newname.toAscii());
    GetDefaultDataStorage()->Add(node);
  }
}

void QmitkTensorReconstructionView::OnSelectionChanged( std::vector<mitk::DataNode*> nodes )
{
  if ( !this->IsVisible() )
    return;

  m_DiffusionImages = mitk::DataStorage::SetOfObjects::New();
  m_TensorImages = mitk::DataStorage::SetOfObjects::New();
  bool foundDwiVolume = false;
  bool foundTensorVolume = false;
  m_Controls->m_DiffusionImageLabel->setText("-");
  m_Controls->m_TensorImageLabel->setText("-");
  m_DiffusionImage = NULL;
  m_TensorImage = NULL;

  // iterate selection
  for( std::vector<mitk::DataNode*>::iterator it = nodes.begin(); it != nodes.end(); ++it )
  {
    mitk::DataNode::Pointer node = *it;
    if (node.IsNull())
      continue;

    // only look at interesting types
    if(dynamic_cast<mitk::DiffusionImage<short>*>(node->GetData()))
    {
      foundDwiVolume = true;
      m_Controls->m_DiffusionImageLabel->setText(node->GetName().c_str());
      m_DiffusionImages->push_back(node);
      m_DiffusionImage = node;
    }
    else if(dynamic_cast<mitk::TensorImage*>(node->GetData()))
    {
      foundTensorVolume = true;
      m_Controls->m_TensorImageLabel->setText(node->GetName().c_str());
      m_TensorImages->push_back(node);
      m_TensorImage = node;
    }
  }

  m_Controls->m_ItkReconstruction->setEnabled(foundDwiVolume);
  m_Controls->m_TeemReconstruction->setEnabled(foundDwiVolume);

  m_Controls->m_TensorsToDWIButton->setEnabled(foundTensorVolume);
  m_Controls->m_TensorsToQbiButton->setEnabled(foundTensorVolume);


  m_Controls->m_ResidualButton->setEnabled(foundDwiVolume && foundTensorVolume);
  m_Controls->m_PercentagesOfOutliers->setEnabled(foundDwiVolume && foundTensorVolume);
  m_Controls->m_PerSliceView->setEnabled(foundDwiVolume && foundTensorVolume);
}

template<int ndirs>
std::vector<itk::Vector<double,3> > QmitkTensorReconstructionView::MakeGradientList()
{
  std::vector<itk::Vector<double,3> > retval;
  vnl_matrix_fixed<double, 3, ndirs>* U =
      itk::PointShell<ndirs, vnl_matrix_fixed<double, 3, ndirs> >::DistributePointShell();

  for(int i=0; i<ndirs;i++)
  {
    itk::Vector<double,3> v;
    v[0] = U->get(0,i); v[1] = U->get(1,i); v[2] = U->get(2,i);
    retval.push_back(v);
  }
  // Add 0 vector for B0
  itk::Vector<double,3> v;
  v.Fill(0.0);
  retval.push_back(v);

  return retval;
}

void QmitkTensorReconstructionView::DoTensorsToDWI
(mitk::DataStorage::SetOfObjects::Pointer inImages)
{
  try
  {
    itk::TimeProbe clock;

    int nrFiles = inImages->size();
    if (!nrFiles) return;

    QString status;
    mitk::ProgressBar::GetInstance()->AddStepsToDo(nrFiles);

    mitk::DataStorage::SetOfObjects::const_iterator itemiter( inImages->begin() );
    mitk::DataStorage::SetOfObjects::const_iterator itemiterend( inImages->end() );

    std::vector<mitk::DataNode::Pointer> nodes;
    while ( itemiter != itemiterend ) // for all items
    {

      std::string nodename;
      (*itemiter)->GetStringProperty("name", nodename);

      mitk::TensorImage* vol =
          static_cast<mitk::TensorImage*>((*itemiter)->GetData());

      ++itemiter;

      typedef float                                       TTensorPixelType;
      typedef itk::DiffusionTensor3D< TTensorPixelType >  TensorPixelType;
      typedef itk::Image< TensorPixelType, 3 >            TensorImageType;


      TensorImageType::Pointer itkvol = TensorImageType::New();
      mitk::CastToItkImage<TensorImageType>(vol, itkvol);

      typedef itk::TensorImageToDiffusionImageFilter<
          TTensorPixelType, DiffusionPixelType > FilterType;

      FilterType::GradientListType gradientList;

      switch(m_Controls->m_TensorsToDWINumDirsSelect->currentIndex())
      {
      case 0:
        gradientList = MakeGradientList<12>();
        break;
      case 1:
        gradientList = MakeGradientList<42>();
        break;
      case 2:
        gradientList = MakeGradientList<92>();
        break;
      case 3:
        gradientList = MakeGradientList<162>();
        break;
      case 4:
        gradientList = MakeGradientList<252>();
        break;
      case 5:
        gradientList = MakeGradientList<362>();
        break;
      case 6:
        gradientList = MakeGradientList<492>();
        break;
      case 7:
        gradientList = MakeGradientList<642>();
        break;
      case 8:
        gradientList = MakeGradientList<812>();
        break;
      case 9:
        gradientList = MakeGradientList<1002>();
        break;
      default:
        gradientList = MakeGradientList<92>();

      }

      double bVal = m_Controls->m_TensorsToDWIBValueEdit->text().toDouble();

      // DWI ESTIMATION
      clock.Start();
      MBI_INFO << "DWI Estimation ";
      mitk::StatusBar::GetInstance()->DisplayText(status.sprintf(
                                                    "DWI Estimation for %s", nodename.c_str()).toAscii());
      FilterType::Pointer filter = FilterType::New();
      filter->SetInput( itkvol );
      filter->SetBValue(bVal);
      filter->SetGradientList(gradientList);
      //filter->SetNumberOfThreads(1);
      filter->Update();
      clock.Stop();
      MBI_DEBUG << "took " << clock.GetMeanTime() << "s.";

      // TENSORS TO DATATREE
      mitk::DiffusionImage<DiffusionPixelType>::Pointer image = mitk::DiffusionImage<DiffusionPixelType>::New();
      image->SetVectorImage( filter->GetOutput() );
      image->SetB_Value(bVal);
      image->SetDirections(gradientList);
      image->SetOriginalDirections(gradientList);
      image->InitializeFromVectorImage();
      mitk::DataNode::Pointer node=mitk::DataNode::New();
      node->SetData( image );

      mitk::DiffusionImageMapper<short>::SetDefaultProperties(node);

      QString newname;
      newname = newname.append(nodename.c_str());
      newname = newname.append("_dwi");
      node->SetName(newname.toAscii());

      nodes.push_back(node);

      mitk::ProgressBar::GetInstance()->Progress();

    }

    std::vector<mitk::DataNode::Pointer>::iterator nodeIt;
    for(nodeIt = nodes.begin(); nodeIt != nodes.end(); ++nodeIt)
      GetDefaultDataStorage()->Add(*nodeIt);

    mitk::StatusBar::GetInstance()->DisplayText(status.sprintf("Finished Processing %d Files", nrFiles).toAscii());
    m_MultiWidget->RequestUpdate();

  }
  catch (itk::ExceptionObject &ex)
  {
    MBI_INFO << ex ;
    return ;
  }
}
