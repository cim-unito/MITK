/*=========================================================================
Copyright (c) German Cancer Research Center, Division of Medical and
Biological Informatics. All rights reserved.
See MITKCopyright.txt or http://www.mitk.org/copyright.html for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

// Blueberry
#include <berryISelectionService.h>
#include <berryIWorkbenchWindow.h>

// Qmitk
#include "QmitkODFDetailsView.h"
#include <QmitkStdMultiWidget.h>
#include <QTableWidgetItem>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkLookupTable.h>
#include <mitkOdfNormalizationMethodProperty.h>
#include <QTextEdit>
#include <mitkTensorImage.h>
#include <QMessageBox>
#include <QmitkRenderingManager.h>

const std::string QmitkODFDetailsView::VIEW_ID = "org.mitk.views.odfdetails";

QmitkODFDetailsView::QmitkODFDetailsView()
  : QmitkFunctionality()
  , m_Controls( 0 )
  , m_MultiWidget( NULL )
  , m_OdfNormalization(0)
  , m_ImageNode(NULL)
{
  m_VtkActor = vtkActor::New();
  m_VtkMapper = vtkPolyDataMapper::New();
  m_Renderer = vtkRenderer::New();
  m_VtkRenderWindow = vtkRenderWindow::New();
  m_RenderWindowInteractor = vtkRenderWindowInteractor::New();
  m_Camera = vtkCamera::New();
  m_VtkRenderWindow->SetSize(300,300);
}

QmitkODFDetailsView::~QmitkODFDetailsView()
{
  QmitkStdMultiWidget* MultiWidget = this->GetActiveStdMultiWidget(false);

  if(MultiWidget)
  {
    //unregister observers when view is destroyed
    if( MultiWidget->mitkWidget1 != NULL && m_SliceObserverTag1 != 0)
    {
      mitk::SliceNavigationController* slicer = MultiWidget->mitkWidget1->GetSliceNavigationController();
      slicer->RemoveObserver( m_SliceObserverTag1 );
    }

    if( MultiWidget->mitkWidget2 != NULL && m_SliceObserverTag2 != 0)
    {
      mitk::SliceNavigationController* slicer = MultiWidget->mitkWidget2->GetSliceNavigationController();
      slicer->RemoveObserver( m_SliceObserverTag2 );
    }

    if( MultiWidget->mitkWidget3!= NULL && m_SliceObserverTag3 != 0)
    {
      mitk::SliceNavigationController* slicer = MultiWidget->mitkWidget3->GetSliceNavigationController();
      slicer->RemoveObserver( m_SliceObserverTag3 );
    }
  }
}

void QmitkODFDetailsView::CreateQtPartControl( QWidget *parent )
{
  // build up qt view, unless already done
  if ( !m_Controls )
  {
    // create GUI widgets from the Qt Designer's .ui file
    m_Controls = new Ui::QmitkODFDetailsViewControls;
    m_Controls->setupUi( parent );
    m_Controls->m_OdfBox->setVisible(false);
    m_Controls->m_ODFRenderWidget->setVisible(false);
  }
}

void QmitkODFDetailsView::StdMultiWidgetAvailable (QmitkStdMultiWidget &stdMultiWidget)
{
  m_MultiWidget = &stdMultiWidget;

  {
    mitk::SliceNavigationController* slicer = m_MultiWidget->mitkWidget1->GetSliceNavigationController();
    itk::ReceptorMemberCommand<QmitkODFDetailsView>::Pointer command = itk::ReceptorMemberCommand<QmitkODFDetailsView>::New();
    command->SetCallbackFunction( this, &QmitkODFDetailsView::OnSliceChanged );
    m_SliceObserverTag1 = slicer->AddObserver( mitk::SliceNavigationController::GeometrySliceEvent(NULL, 0), command );
  }

  {
    mitk::SliceNavigationController* slicer = m_MultiWidget->mitkWidget2->GetSliceNavigationController();
    itk::ReceptorMemberCommand<QmitkODFDetailsView>::Pointer command = itk::ReceptorMemberCommand<QmitkODFDetailsView>::New();
    command->SetCallbackFunction( this, &QmitkODFDetailsView::OnSliceChanged );
    m_SliceObserverTag2 = slicer->AddObserver( mitk::SliceNavigationController::GeometrySliceEvent(NULL, 0), command );
  }

  {
    mitk::SliceNavigationController* slicer = m_MultiWidget->mitkWidget3->GetSliceNavigationController();
    itk::ReceptorMemberCommand<QmitkODFDetailsView>::Pointer command = itk::ReceptorMemberCommand<QmitkODFDetailsView>::New();
    command->SetCallbackFunction( this, &QmitkODFDetailsView::OnSliceChanged );
    m_SliceObserverTag3 = slicer->AddObserver( mitk::SliceNavigationController::GeometrySliceEvent(NULL, 0), command );
  }
}

void QmitkODFDetailsView::StdMultiWidgetNotAvailable()
{

  {
    mitk::SliceNavigationController* slicer = m_MultiWidget->mitkWidget1->GetSliceNavigationController();
    slicer->RemoveObserver( m_SliceObserverTag1 );
  }

  {
    mitk::SliceNavigationController* slicer = m_MultiWidget->mitkWidget2->GetSliceNavigationController();
    slicer->RemoveObserver( m_SliceObserverTag2 );
  }

  {
    mitk::SliceNavigationController* slicer = m_MultiWidget->mitkWidget3->GetSliceNavigationController();
    slicer->RemoveObserver( m_SliceObserverTag3 );
  }

  m_MultiWidget = NULL;
}

void QmitkODFDetailsView::OnSelectionChanged( std::vector<mitk::DataNode*> nodes )
{
  if (m_ImageNode.IsNotNull())
    m_ImageNode->RemoveObserver( m_PropertyObserverTag );

  m_ImageNode = NULL;
  m_Controls->m_InputImageLabel->setText("-");
  // iterate selection
  for( std::vector<mitk::DataNode*>::iterator it = nodes.begin(); it != nodes.end(); ++it )
  {
    mitk::DataNode::Pointer node = *it;

    if( node.IsNotNull() && (dynamic_cast<mitk::QBallImage*>(node->GetData()) || dynamic_cast<mitk::TensorImage*>(node->GetData())) )
    {
      m_Controls->m_InputImageLabel->setText(node->GetName().c_str());
      m_ImageNode = node;
    }
  }

  UpdateOdf();
  if (m_ImageNode.IsNotNull())
  {
    itk::ReceptorMemberCommand<QmitkODFDetailsView>::Pointer command = itk::ReceptorMemberCommand<QmitkODFDetailsView>::New();
    command->SetCallbackFunction( this, &QmitkODFDetailsView::OnSliceChanged );
    m_PropertyObserverTag = m_ImageNode->AddObserver( itk::ModifiedEvent(), command );
  }
}

void QmitkODFDetailsView::UpdateOdf()
{

  try
    {
    m_Values.clear();
    m_Controls->m_OverviewBox->setVisible(true);
    if (m_ImageNode.IsNull() || !m_MultiWidget)
    {
      m_Controls->m_ODFRenderWidget->setVisible(false);
      m_Controls->m_OdfBox->setVisible(false);
      m_Controls->m_OverviewBox->setVisible(false);
      return;
    }

    // ODF Normalization Property
    mitk::OdfNormalizationMethodProperty* nmp = dynamic_cast<mitk::OdfNormalizationMethodProperty*>(m_ImageNode->GetProperty( "Normalization" ));
    if(nmp)
      m_OdfNormalization = nmp->GetNormalization();

    m_TemplateOdf = itk::OrientationDistributionFunction<float,QBALL_ODFSIZE>::GetBaseMesh();
    m_OdfTransform = vtkSmartPointer<vtkTransform>::New();
    m_OdfTransform->Identity();
    m_OdfVals = vtkSmartPointer<vtkDoubleArray>::New();
    m_OdfSource = vtkSmartPointer<vtkOdfSource>::New();
    itk::OrientationDistributionFunction<double, QBALL_ODFSIZE> odf;

    mitk::Point3D world = m_MultiWidget->GetCrossPosition();
    mitk::Point3D index;
    mitk::Image::Pointer img = dynamic_cast<mitk::Image*>(m_ImageNode->GetData());
    img->GetTimeSlicedGeometry()->WorldToIndex(world, index);

    float sum = 0;
    float max = itk::NumericTraits<float>::min();
    float min = itk::NumericTraits<float>::max();
    QString values;
    QString overviewText;

    // check if dynamic_cast successfull and if the crosshair position is inside of the geometry of the ODF data
    // otherwise possible crash for a scenario with multiple nodes
    if (dynamic_cast<mitk::QBallImage*>(m_ImageNode->GetData()) && ( m_ImageNode->GetData()->GetGeometry()->IsInside(world) ) )
    {
      m_Controls->m_ODFRenderWidget->setVisible(true);
      m_Controls->m_OdfBox->setVisible(true);
      OdfVectorImgType::Pointer itkQBallImage = OdfVectorImgType::New();
      mitk::CastToItkImage<OdfVectorImgType>(dynamic_cast<mitk::QBallImage*>(m_ImageNode->GetData()), itkQBallImage);
      OdfVectorImgType::IndexType ind;
      ind[0] = (int)(index[0]+0.5);
      ind[1] = (int)(index[1]+0.5);
      ind[2] = (int)(index[2]+0.5);

      OdfVectorImgType::PixelType pixel = itkQBallImage->GetPixel(ind);

      for (int i=0; i<QBALL_ODFSIZE; i++)
      {
        float val = pixel[i];
        odf.SetNthComponent(i, val);
        values += QString::number(i)+": "+QString::number(val)+"\n";
        sum += val;
        if (val>max)
          max = val;
        if (val<min)
          min = val;
      }
      float mean = sum/QBALL_ODFSIZE;

      QString pos = QString::number(ind[0])+", "+QString::number(ind[1])+", "+QString::number(ind[2]);
      overviewText += "Coordinates: "+pos+"\n";
      overviewText += "GFA: "+QString::number(odf.GetGeneralizedFractionalAnisotropy())+"\n";
      overviewText += "Sum: "+QString::number(sum)+"\n";
      overviewText += "Mean: "+QString::number(mean)+"\n";
      overviewText += "Min: "+QString::number(min)+"\n";
      overviewText += "Max: "+QString::number(max)+"\n";
      vnl_vector_fixed<double, 3> pd = odf.GetDirection(odf.GetPrincipleDiffusionDirection());
      overviewText += "Main Diffusion:\n     "+QString::number(pd[0])+"\n     "+QString::number(pd[1])+"\n     "+QString::number(pd[2])+"\n";

      m_Controls->m_OdfValuesTextEdit->setText(values);
    }
    else if (dynamic_cast<mitk::TensorImage*>(m_ImageNode->GetData()))
    {
      m_Controls->m_ODFRenderWidget->setVisible(true);
      m_Controls->m_OdfBox->setVisible(false);
      TensorImageType::Pointer itkQBallImage = TensorImageType::New();
      mitk::CastToItkImage<TensorImageType>(dynamic_cast<mitk::TensorImage*>(m_ImageNode->GetData()), itkQBallImage);

      TensorImageType::IndexType ind;
      ind[0] = (int)(index[0]+0.5);
      ind[1] = (int)(index[1]+0.5);
      ind[2] = (int)(index[2]+0.5);

      TensorImageType::PixelType pixel = itkQBallImage->GetPixel(ind);

      float tensorelems[6] = {
        (float)pixel[0],
        (float)pixel[1],
        (float)pixel[2],
        (float)pixel[3],
        (float)pixel[4],
        (float)pixel[5],
      };
      itk::DiffusionTensor3D<float> tensor(tensorelems);

      odf.InitFromTensor(tensor);


      /** Array of eigen-values. */
      typedef itk::FixedArray<float, 3> EigenValuesArrayType;
      /** Matrix of eigen-vectors. */
      typedef itk::Matrix<float, 3, 3> MatrixType;
      typedef itk::Matrix<float, 3, 3> EigenVectorsMatrixType;

      EigenValuesArrayType eigenValues;
      EigenVectorsMatrixType eigenVectors;

      QString pos = QString::number(ind[0])+", "+QString::number(ind[1])+", "+QString::number(ind[2]);
      overviewText += "Coordinates: "+pos+"\n";
      overviewText += "FA: "+QString::number(tensor.GetFractionalAnisotropy())+"\n";
      overviewText += "RA: "+QString::number(tensor.GetRelativeAnisotropy())+"\n";
      overviewText += "Trace: "+QString::number(tensor.GetTrace())+"\n";
      tensor.ComputeEigenAnalysis(eigenValues,eigenVectors);
      overviewText += "Eigenvalues:\n     "+QString::number(eigenValues[2])+"\n     "+QString::number(eigenValues[1])+"\n     "+QString::number(eigenValues[0])+"\n";
      overviewText += "Main Diffusion:\n     "+QString::number(eigenVectors[0][0])+"\n     "+QString::number(eigenVectors[1][0])+"\n     "+QString::number(eigenVectors[2][0])+"\n";
      overviewText += "Values:\n     "+QString::number(tensorelems[0])+"\n     "+QString::number(tensorelems[1])+"\n     "+QString::number(tensorelems[2])+"\n     "+QString::number(tensorelems[3])+"\n     "+QString::number(tensorelems[4])+"\n     "+QString::number(tensorelems[5])+"\n     "+"\n";
    }
    else
    {
      m_Controls->m_ODFRenderWidget->setVisible(false);
      m_Controls->m_OdfBox->setVisible(false);
      overviewText += "Please reinit image geometry.\n";
    }

    m_Controls->m_ODFDetailsWidget->SetParameters(odf);

    switch(m_OdfNormalization)
    {
    case 0:
      odf = odf.MinMaxNormalize();
      break;
    case 1:
      odf = odf.MaxNormalize();
      break;
    case 2:
      odf = odf.MaxNormalize();
      break;
    default:
      odf = odf.MinMaxNormalize();
    }

    m_Controls->m_ODFRenderWidget->GenerateODF(odf);
    m_Controls->m_OverviewTextEdit->setText(overviewText.toStdString().c_str());
  }
  catch(...)
  {
    QMessageBox::critical(0, "Error", "Data could not be analyzed. The image might be corrupted.");
  }
}

void QmitkODFDetailsView::OnSliceChanged(const itk::EventObject& /*e*/)
{
  UpdateOdf();
}
