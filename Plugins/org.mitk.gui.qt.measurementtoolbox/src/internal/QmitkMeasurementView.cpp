/*=========================================================================

Program:   Medical Imaging & Interaction Toolkit
Language:  C++
Date:      $Date$
Version:   $Revision: 15412 $

Copyright (c) German Cancer Research Center, Division of Medical and
Biological Informatics. All rights reserved.
See MITKCopyright.txt or http://www.mitk.org/copyright.html for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <berryIEditorPart.h>
#include <berryIWorkbenchPage.h>
#include <berryPlatform.h>

#include "mitkGlobalInteraction.h"
#include "mitkPointSet.h"
#include "mitkProperties.h"
#include "mitkStringProperty.h"
#include "mitkIDataStorageService.h"
#include "mitkDataNodeObject.h"
#include <mitkNodePredicateProperty.h>
#include <mitkNodePredicateData.h>
#include <mitkNodePredicateNot.h>
#include <mitkNodePredicateAnd.h>
#include <mitkDataNodeSelection.h>
#include <mitkILinkedRenderWindowPart.h>
#include <mitkIRenderingManager.h>

#include "mitkPlanarCircle.h"
#include "mitkPlanarPolygon.h"
#include "mitkPlanarAngle.h"
#include "mitkPlanarRectangle.h"
#include "mitkPlanarLine.h"
#include "mitkPlanarCross.h"
#include "mitkPlanarFourPointAngle.h"
#include "mitkPlanarFigureInteractor.h"
#include "mitkPlaneGeometry.h"
#include "QmitkPlanarFiguresTableModel.h"

#include "QmitkMeasurementView.h"
#include <QmitkRenderWindow.h>

#include <QApplication>
#include <QGridLayout>
#include <QMainWindow>
#include <QToolBar>
#include <QLabel>
#include <QTableView>
#include <QClipboard>
#include <QTextBrowser>

#include <QmitkDataStorageTableModel.h>
#include "mitkNodePredicateDataType.h"
#include "mitkPlanarFigure.h"

#include <vtkTextProperty.h>
#include <vtkRenderer.h>
#include <vtkCornerAnnotation.h>
#include <mitkVtkLayerController.h>

const std::string QmitkMeasurementView::VIEW_ID =
"org.mitk.views.measurement";

QmitkMeasurementView::QmitkMeasurementView() :
m_Parent(0), m_Layout(0), m_DrawActionsToolBar(0),
m_DrawActionsGroup(0), m_MeasurementInfoRenderer(0),
m_MeasurementInfoAnnotation(0), m_SelectedPlanarFigures(0),
m_SelectedImageNode(),
m_LineCounter(0), m_PathCounter(0),
m_AngleCounter(0), m_FourPointAngleCounter(0), m_EllipseCounter(0),
  m_RectangleCounter(0), m_PolygonCounter(0), m_Visible(false),
  m_CurrentFigureNodeInitialized(false), m_Activated(false),
  m_LastRenderWindow(0)
{

}

QmitkMeasurementView::~QmitkMeasurementView()
{
  this->GetDataStorage()->AddNodeEvent -= mitk::MessageDelegate1<QmitkMeasurementView
    , const mitk::DataNode*>( this, &QmitkMeasurementView::NodeAddedInDataStorage );

  m_SelectedPlanarFigures->NodeChanged.RemoveListener( mitk::MessageDelegate1<QmitkMeasurementView
    , const mitk::DataNode*>( this, &QmitkMeasurementView::NodeChanged ) );

  m_SelectedPlanarFigures->NodeRemoved.RemoveListener( mitk::MessageDelegate1<QmitkMeasurementView
    , const mitk::DataNode*>( this, &QmitkMeasurementView::NodeRemoved ) );

  m_SelectedPlanarFigures->PropertyChanged.RemoveListener( mitk::MessageDelegate2<QmitkMeasurementView
    , const mitk::DataNode*, const mitk::BaseProperty*>( this, &QmitkMeasurementView::PropertyChanged ) );

  m_SelectedImageNode->NodeChanged.RemoveListener( mitk::MessageDelegate1<QmitkMeasurementView
    , const mitk::DataNode*>( this, &QmitkMeasurementView::NodeChanged ) );

  m_SelectedImageNode->NodeRemoved.RemoveListener( mitk::MessageDelegate1<QmitkMeasurementView
    , const mitk::DataNode*>( this, &QmitkMeasurementView::NodeRemoved ) );

  m_SelectedImageNode->PropertyChanged.RemoveListener( mitk::MessageDelegate2<QmitkMeasurementView
    , const mitk::DataNode*, const mitk::BaseProperty*>( this, &QmitkMeasurementView::PropertyChanged ) );

  if(this->m_LastRenderWindow != NULL)
  {
    this->SetMeasurementInfoToRenderWindow("",m_LastRenderWindow);
    mitk::VtkLayerController::GetInstance(m_LastRenderWindow->GetRenderWindow())->RemoveRenderer(
      m_MeasurementInfoRenderer);
  }
  this->m_MeasurementInfoRenderer->Delete();
}

void QmitkMeasurementView::CreateQtPartControl(QWidget* parent)
{
  m_Parent = parent;
  m_MeasurementInfoRenderer = vtkRenderer::New();
  m_MeasurementInfoAnnotation = vtkCornerAnnotation::New();
  vtkTextProperty *textProp = vtkTextProperty::New();

  m_MeasurementInfoAnnotation->SetMaximumFontSize(12);
  textProp->SetColor(1.0, 1.0, 1.0);
  m_MeasurementInfoAnnotation->SetTextProperty(textProp);

  m_MeasurementInfoRenderer->AddActor(m_MeasurementInfoAnnotation);
  m_DrawActionsToolBar = new QToolBar;
  m_DrawActionsGroup = new QActionGroup(this);
  m_DrawActionsGroup->setExclusive(true);

  //# add actions
  QAction* currentAction = m_DrawActionsToolBar->addAction(QIcon(
    ":/measurement/line.png"), "Draw Line");
  m_DrawLine = currentAction;
  m_DrawLine->setCheckable(true);
  m_DrawActionsToolBar->addAction(currentAction);
  m_DrawActionsGroup->addAction(currentAction);
  QObject::connect( currentAction, SIGNAL( triggered(bool) )
    , this, SLOT( ActionDrawLineTriggered(bool) ) );

  currentAction = m_DrawActionsToolBar->addAction(QIcon(
    ":/measurement/path.png"), "Draw Path");
  m_DrawPath = currentAction;
  m_DrawPath->setCheckable(true);
  m_DrawActionsToolBar->addAction(currentAction);
  m_DrawActionsGroup->addAction(currentAction);
  QObject::connect( currentAction, SIGNAL( triggered(bool) )
    , this, SLOT( ActionDrawPathTriggered(bool) ) );

  currentAction = m_DrawActionsToolBar->addAction(QIcon(
    ":/measurement/angle.png"), "Draw Angle");
  m_DrawAngle = currentAction;
  m_DrawAngle->setCheckable(true);
  m_DrawActionsToolBar->addAction(currentAction);
  m_DrawActionsGroup->addAction(currentAction);
  QObject::connect( currentAction, SIGNAL( triggered(bool) )
    , this, SLOT( ActionDrawAngleTriggered(bool) ) );

  currentAction = m_DrawActionsToolBar->addAction(QIcon(
    ":/measurement/four-point-angle.png"), "Draw Four Point Angle");
  m_DrawFourPointAngle = currentAction;
  m_DrawFourPointAngle->setCheckable(true);
  m_DrawActionsToolBar->addAction(currentAction);
  m_DrawActionsGroup->addAction(currentAction);
  QObject::connect( currentAction, SIGNAL( triggered(bool) )
    , this, SLOT( ActionDrawFourPointAngleTriggered(bool) ) );

  currentAction = m_DrawActionsToolBar->addAction(QIcon(
    ":/measurement/circle.png"), "Draw Circle");
  m_DrawEllipse = currentAction;
  m_DrawEllipse->setCheckable(true);
  m_DrawActionsToolBar->addAction(currentAction);
  m_DrawActionsGroup->addAction(currentAction);
  QObject::connect( currentAction, SIGNAL( triggered(bool) )
    , this, SLOT( ActionDrawEllipseTriggered(bool) ) );

  currentAction = m_DrawActionsToolBar->addAction(QIcon(
    ":/measurement/rectangle.png"), "Draw Rectangle");
  m_DrawRectangle = currentAction;
  m_DrawRectangle->setCheckable(true);
  m_DrawActionsToolBar->addAction(currentAction);
  m_DrawActionsGroup->addAction(currentAction);
  QObject::connect( currentAction, SIGNAL( triggered(bool) )
    , this, SLOT( ActionDrawRectangleTriggered(bool) ) );

  currentAction = m_DrawActionsToolBar->addAction(QIcon(
    ":/measurement/polygon.png"), "Draw Polygon");
  m_DrawPolygon = currentAction;
  m_DrawPolygon->setCheckable(true);
  m_DrawActionsToolBar->addAction(currentAction);
  m_DrawActionsGroup->addAction(currentAction);
  QObject::connect( currentAction, SIGNAL( triggered(bool) )
    , this, SLOT( ActionDrawPolygonTriggered(bool) ) );

  currentAction = m_DrawActionsToolBar->addAction(QIcon(
    ":/Qmitk/Images_48.png"), "Reproduce potential bug");
  m_DrawActionsToolBar->addAction(currentAction);
  QObject::connect( currentAction, SIGNAL( triggered(bool) )
    , this, SLOT( ReproducePotentialBug(bool) ) );


  QLabel* selectedImageLabel = new QLabel("Selected Image: ");
  m_SelectedImage = new QLabel;
  m_SelectedImage->setStyleSheet("font-weight: bold;");
  m_SelectedPlanarFiguresText = new QTextBrowser;

  m_CopyToClipboard = new QPushButton("Copy to Clipboard");
  QObject::connect( m_CopyToClipboard, SIGNAL( clicked(bool) )
    , this, SLOT( CopyToClipboard(bool) ) );

  m_Layout = new QGridLayout;
  m_Layout->addWidget(selectedImageLabel, 0, 0, 1, 1);
  m_Layout->addWidget(m_SelectedImage, 0, 1, 1, 1);
  m_Layout->addWidget(m_DrawActionsToolBar, 1, 0, 1, 2);
  m_Layout->addWidget(m_SelectedPlanarFiguresText, 2, 0, 1, 2);
  m_Layout->addWidget(m_CopyToClipboard, 3, 0, 1, 2);
  m_Layout->setRowStretch(0, 1);
  m_Layout->setRowStretch(1, 1);
  m_Layout->setRowStretch(2, 10);
  m_Layout->setRowStretch(3, 1);
  m_Layout->setContentsMargins(2, 2, 2, 2);

  parent->setLayout(m_Layout);


  m_SelectedPlanarFigures = mitk::DataStorageSelection::New(this->GetDataStorage(), false);

  m_SelectedPlanarFigures->NodeChanged.AddListener( mitk::MessageDelegate1<QmitkMeasurementView
    , const mitk::DataNode*>( this, &QmitkMeasurementView::NodeChanged ) );

  m_SelectedPlanarFigures->NodeRemoved.AddListener( mitk::MessageDelegate1<QmitkMeasurementView
    , const mitk::DataNode*>( this, &QmitkMeasurementView::NodeRemoved ) );

  m_SelectedPlanarFigures->PropertyChanged.AddListener( mitk::MessageDelegate2<QmitkMeasurementView
    , const mitk::DataNode*, const mitk::BaseProperty*>( this, &QmitkMeasurementView::PropertyChanged ) );

  m_SelectedImageNode = mitk::DataStorageSelection::New(this->GetDataStorage(), false);

  m_SelectedImageNode->PropertyChanged.AddListener( mitk::MessageDelegate2<QmitkMeasurementView
    , const mitk::DataNode*, const mitk::BaseProperty*>( this, &QmitkMeasurementView::PropertyChanged ) );

  m_SelectedImageNode->NodeChanged.AddListener( mitk::MessageDelegate1<QmitkMeasurementView
    , const mitk::DataNode*>( this, &QmitkMeasurementView::NodeChanged ) );

  m_SelectedImageNode->NodeRemoved.AddListener( mitk::MessageDelegate1<QmitkMeasurementView
    , const mitk::DataNode*>( this, &QmitkMeasurementView::NodeRemoved ) );

  this->GetDataStorage()->AddNodeEvent.AddListener( mitk::MessageDelegate1<QmitkMeasurementView
    , const mitk::DataNode*>( this, &QmitkMeasurementView::NodeAddedInDataStorage ) );

}

void QmitkMeasurementView::OnSelectionChanged(berry::IWorkbenchPart::Pointer /*part*/,
                                              const QList<mitk::DataNode::Pointer> &nodes)
{
  if ( nodes.isEmpty() ) return;

  m_SelectedImageNode->RemoveAllNodes();

  mitk::BaseData* _BaseData;
  mitk::PlanarFigure* _PlanarFigure;
  mitk::Image* selectedImage;
  m_SelectedPlanarFigures->RemoveAllNodes();

  foreach (mitk::DataNode::Pointer _DataNode, nodes)
  {
    _PlanarFigure = 0;

    if (!_DataNode)
      continue;

    _BaseData = _DataNode->GetData();

    if (!_BaseData)
      continue;

    // planar figure selected
    if ((_PlanarFigure = dynamic_cast<mitk::PlanarFigure *> (_BaseData)))
    {
      // add to the selected planar figures
      m_SelectedPlanarFigures->AddNode(_DataNode);
      // take parent image as the selected image
      mitk::DataStorage::SetOfObjects::ConstPointer parents =
        this->GetDataStorage()->GetSources(_DataNode);
      if (parents->size() > 0)
      {
        mitk::DataNode::Pointer parent = parents->front();
        if ((selectedImage = dynamic_cast<mitk::Image *> (parent->GetData())))
        {
          *m_SelectedImageNode = parent;
        }
      }

    }
    else if ((selectedImage = dynamic_cast<mitk::Image *> (_BaseData)))
    {
      *m_SelectedImageNode = _DataNode;
      /*mitk::RenderingManager::GetInstance()->InitializeViews(
      selectedImage->GetTimeSlicedGeometry(), mitk::RenderingManager::REQUEST_UPDATE_ALL, true );*/
    }
  } // end for

  this->PlanarFigureSelectionChanged();
}

void QmitkMeasurementView::PlanarFigureSelectionChanged()
{
  if ( !this->m_Activated ) return;

  if (m_SelectedImageNode->GetNode().IsNotNull())
  {
    mitk::Image* selectedImage = dynamic_cast<mitk::Image*>(m_SelectedImageNode->GetNode()->GetData());
    if(selectedImage && selectedImage->GetDimension() > 3)
    {
      m_SelectedImageNode->RemoveAllNodes();
      m_SelectedImage->setText( "4D images are not supported." );
      m_DrawActionsToolBar->setEnabled(false);
    }
    else
    {
      m_SelectedImage->setText(QString::fromStdString(
        m_SelectedImageNode->GetNode()->GetName()));
      m_DrawActionsToolBar->setEnabled(true);
    }
  }
  else
  {
    m_SelectedImage->setText(
      "None. Please select an image.");

    m_DrawActionsToolBar->setEnabled(false);
  }

  if (m_SelectedPlanarFigures->GetSize() == 0 && this->GetRenderWindowPart() != 0)
  {
    foreach (QmitkRenderWindow* renderWindow, this->GetRenderWindowPart()->GetRenderWindows().values())
    {
      this->SetMeasurementInfoToRenderWindow("", renderWindow);
    }
  }

  unsigned int j = 1;
  mitk::PlanarFigure* _PlanarFigure = 0;
  mitk::PlanarAngle* planarAngle = 0;
  mitk::PlanarFourPointAngle* planarFourPointAngle = 0;
  mitk::DataNode::Pointer node = 0;
  m_SelectedPlanarFiguresText->clear();
  QString infoText;
  QString plainInfoText;
  std::vector<mitk::DataNode*> nodes = m_SelectedPlanarFigures->GetNodes();

  for (std::vector<mitk::DataNode*>::iterator it = nodes.begin(); it
    != nodes.end(); ++it, ++j)
  {
    plainInfoText.clear();
    node = *it;
    if(j>1)
      infoText.append("<br />");

    infoText.append(QString("<b>%1</b><hr />").arg(QString::fromStdString(
      node->GetName())));
    plainInfoText.append(QString("%1").arg(QString::fromStdString(
      node->GetName())));

    _PlanarFigure = dynamic_cast<mitk::PlanarFigure*> (node->GetData());

    planarAngle = dynamic_cast<mitk::PlanarAngle*> (_PlanarFigure);
    if(!planarAngle)
    {
      planarFourPointAngle = dynamic_cast<mitk::PlanarFourPointAngle*> (_PlanarFigure);
    }

    if(!_PlanarFigure)
      continue;

    double featureQuantity = 0.0;
    for (unsigned int i = 0; i < _PlanarFigure->GetNumberOfFeatures(); ++i)
    {
      if ( !_PlanarFigure->IsFeatureActive( i ) ) continue;

      featureQuantity = _PlanarFigure->GetQuantity(i);
      if ((planarAngle && i == planarAngle->FEATURE_ID_ANGLE)
        || (planarFourPointAngle && i == planarFourPointAngle->FEATURE_ID_ANGLE))
        featureQuantity = featureQuantity * 180 / vnl_math::pi;

      infoText.append(
        QString("<i>%1</i>: %2 %3") .arg(QString(
        _PlanarFigure->GetFeatureName(i))) .arg(featureQuantity, 0, 'f',
        2) .arg(QString(_PlanarFigure->GetFeatureUnit(i))));

      plainInfoText.append(
        QString("\n%1: %2 %3") .arg(QString(_PlanarFigure->GetFeatureName(i))) .arg(
        featureQuantity, 0, 'f', 2) .arg(QString(
        _PlanarFigure->GetFeatureUnit(i))));

      if(i+1 != _PlanarFigure->GetNumberOfFeatures())
        infoText.append("<br />");
    }

    if (j != nodes.size())
      infoText.append("<br />");
  }

  m_SelectedPlanarFiguresText->setHtml(infoText);

  // for the last selected planar figure ...
  if (_PlanarFigure)
  {
    const mitk::PlaneGeometry
      * _PlaneGeometry =
      dynamic_cast<const mitk::PlaneGeometry*> (_PlanarFigure->GetGeometry2D());

    QmitkRenderWindow* selectedRenderWindow = 0;
    bool PlanarFigureInitializedWindow = false;
    mitk::IRenderWindowPart* renderPart = this->GetRenderWindowPart();
    if (renderPart)
    {
      foreach(QmitkRenderWindow* renderWindow, renderPart->GetRenderWindows().values())
      {
        if (node->GetBoolProperty("PlanarFigureInitializedWindow", PlanarFigureInitializedWindow,
                                  renderWindow->GetRenderer()))
        {
          selectedRenderWindow = renderWindow;
          break;
        }
      }
    }

    // make node visible
    if (selectedRenderWindow)
    {
      mitk::Point3D centerP = _PlaneGeometry->GetOrigin();
      //selectedRenderWindow->GetSliceNavigationController()->ReorientSlices(
      //centerP, _PlaneGeometry->GetNormal());
      selectedRenderWindow->GetSliceNavigationController()->SelectSliceByPoint(
        centerP);

      // now paint infos also on renderwindow
      this->SetMeasurementInfoToRenderWindow(plainInfoText, selectedRenderWindow);
    }
  }
  // no last planarfigure
  else
    this->SetMeasurementInfoToRenderWindow("", 0);

  this->RequestRenderWindowUpdate();
}

void QmitkMeasurementView::NodeAddedInDataStorage(const mitk::DataNode* node)
{
  if(!m_Visible)
    return;
  mitk::DataNode* nonConstNode = const_cast<mitk::DataNode*>(node);
  mitk::PlanarFigure* figure = dynamic_cast<mitk::PlanarFigure*>(nonConstNode->GetData());
  if(figure)
  {
    mitk::PlanarFigureInteractor::Pointer figureInteractor
      = dynamic_cast<mitk::PlanarFigureInteractor*>(node->GetInteractor());

    if(figureInteractor.IsNull())
      figureInteractor = mitk::PlanarFigureInteractor::New("PlanarFigureInteractor", nonConstNode);

    // remove old interactor if present
    if( m_CurrentFigureNode.IsNotNull() && m_CurrentFigureNodeInitialized == false )
    {
      mitk::Interactor::Pointer oldInteractor = m_CurrentFigureNode->GetInteractor();
      if(oldInteractor.IsNotNull())
        mitk::GlobalInteraction::GetInstance()->RemoveInteractor(oldInteractor);

      this->GetDataStorage()->Remove(m_CurrentFigureNode);
    }

    mitk::GlobalInteraction::GetInstance()->AddInteractor(figureInteractor);
  }
}


void QmitkMeasurementView::PlanarFigureInitialized()
{
  if(m_CurrentFigureNode.IsNull())
    return;

  m_CurrentFigureNodeInitialized = true;

  this->PlanarFigureSelectionChanged();

  m_DrawLine->setChecked(false);
  m_DrawPath->setChecked(false);
  m_DrawAngle->setChecked(false);
  m_DrawFourPointAngle->setChecked(false);
  m_DrawEllipse->setChecked(false);
  m_DrawRectangle->setChecked(false);
  m_DrawPolygon->setChecked(false);
  // enable the crosshair navigation
  this->EnableCrosshairNavigation();
}


void QmitkMeasurementView::PlanarFigureSelected( itk::Object* object, const itk::EventObject& /*event*/ )
{
  // Mark to-be-edited PlanarFigure as selected
  mitk::PlanarFigure* figure = dynamic_cast< mitk::PlanarFigure* >( object );
  if ( figure != NULL )
  {
    // Get node corresponding to PlanarFigure
    mitk::DataNode::Pointer figureNode = this->GetDataStorage()->GetNode(
      mitk::NodePredicateData::New( figure ) );

    // Select this node (and deselect all others)
    QList< mitk::DataNode::Pointer > selectedNodes = this->GetDataManagerSelection();
    for ( int i = 0; i < selectedNodes.size(); i++ )
    {
      selectedNodes[i]->SetSelected( false );
    }
    std::vector< mitk::DataNode* > selectedNodes2 = m_SelectedPlanarFigures->GetNodes();
    for ( unsigned int i = 0; i < selectedNodes2.size(); i++ )
    {
      selectedNodes2[i]->SetSelected( false );
    }
    figureNode->SetSelected( true );

    m_CurrentFigureNode = figureNode;

    *m_SelectedPlanarFigures = figureNode;

    // Re-initialize after selecting new PlanarFigure
    this->PlanarFigureSelectionChanged();
  }
}


mitk::DataNode::Pointer QmitkMeasurementView::DetectTopMostVisibleImage()
{
  // get all images from the data storage
  mitk::DataStorage::SetOfObjects::ConstPointer Images = this->GetDataStorage()->GetSubset( mitk::NodePredicateDataType::New("Image") );

  mitk::DataNode::Pointer currentNode( m_SelectedImageNode->GetNode() );
  int maxLayer = itk::NumericTraits<int>::min();

  // iterate over selection
  for (mitk::DataStorage::SetOfObjects::ConstIterator sofIt = Images->Begin(); sofIt != Images->End(); ++sofIt)
  {
    mitk::DataNode::Pointer node = sofIt->Value();
    if ( node.IsNull() )
      continue;
    if (node->IsVisible(NULL) == false)
      continue;
    int layer = 0;
    node->GetIntProperty("layer", layer);
    if ( layer < maxLayer )
      continue;

    currentNode = node;
  }

  return currentNode;
}

void QmitkMeasurementView::AddFigureToDataStorage(mitk::PlanarFigure* figure, const QString& name,
                                              const char *propertyKey, mitk::BaseProperty *property )
{
  if ( m_CurrentFigureNode.IsNotNull() )
  {
    m_CurrentFigureNode->GetData()->RemoveObserver( m_EndPlacementObserverTag );
  }

  mitk::DataNode::Pointer newNode = mitk::DataNode::New();
  newNode->SetName(name.toStdString());
  newNode->SetData(figure);

  // Add custom property, if available
  if ( (propertyKey != NULL) && (property != NULL) )
  {
    newNode->AddProperty( propertyKey, property );
  }

  // add observer for event when figure has been placed
  typedef itk::SimpleMemberCommand< QmitkMeasurementView > SimpleCommandType;
  SimpleCommandType::Pointer initializationCommand = SimpleCommandType::New();
  initializationCommand->SetCallbackFunction( this, &QmitkMeasurementView::PlanarFigureInitialized );
  m_EndPlacementObserverTag = figure->AddObserver( mitk::EndPlacementPlanarFigureEvent(), initializationCommand );

  // add observer for event when figure is picked (selected)
  typedef itk::MemberCommand< QmitkMeasurementView > MemberCommandType;
  MemberCommandType::Pointer selectCommand = MemberCommandType::New();
  selectCommand->SetCallbackFunction( this, &QmitkMeasurementView::PlanarFigureSelected );
  m_SelectObserverTag = figure->AddObserver( mitk::SelectPlanarFigureEvent(), selectCommand );

  // add observer for event when interaction with figure starts
  SimpleCommandType::Pointer startInteractionCommand = SimpleCommandType::New();
  startInteractionCommand->SetCallbackFunction( this, &QmitkMeasurementView::DisableCrosshairNavigation);
  m_StartInteractionObserverTag = figure->AddObserver( mitk::StartInteractionPlanarFigureEvent(), startInteractionCommand );

  // add observer for event when interaction with figure starts
  SimpleCommandType::Pointer endInteractionCommand = SimpleCommandType::New();
  endInteractionCommand->SetCallbackFunction( this, &QmitkMeasurementView::EnableCrosshairNavigation);
  m_EndInteractionObserverTag = figure->AddObserver( mitk::EndInteractionPlanarFigureEvent(), endInteractionCommand );


  // figure drawn on the topmost layer / image
  this->GetDataStorage()->Add(newNode, this->DetectTopMostVisibleImage() );
  QList<mitk::DataNode::Pointer> selectedNodes = GetDataManagerSelection();
  for(int i = 0; i < selectedNodes.size(); i++)
  {
    selectedNodes[i]->SetSelected(false);
  }
  std::vector<mitk::DataNode*> selectedNodes2 = m_SelectedPlanarFigures->GetNodes();
  for(unsigned int i = 0; i < selectedNodes2.size(); i++)
  {
    selectedNodes2[i]->SetSelected(false);
  }
  newNode->SetSelected(true);


  m_CurrentFigureNodeInitialized = false;
  m_CurrentFigureNode = newNode;

  *m_SelectedPlanarFigures = newNode;

  this->RequestRenderWindowUpdate();
}

bool QmitkMeasurementView::AssertDrawingIsPossible(bool checked)
{
  if (m_SelectedImageNode->GetNode().IsNull())
  {
    checked = false;
    this->HandleException("Please select an image!", this->m_Parent, true);
    m_DrawLine->setChecked(false);
    return false;
  }

  mitk::ILinkedRenderWindowPart* linkedRenderWindow =
      dynamic_cast<mitk::ILinkedRenderWindowPart*>(this->GetRenderWindowPart());
  if (linkedRenderWindow)
  {
    linkedRenderWindow->EnableSlicingPlanes(false);
  }
  // disable the crosshair navigation during the drawing
  this->DisableCrosshairNavigation();

  return checked;
}

void QmitkMeasurementView::ActionDrawLineTriggered(bool checked)
{
  if(!this->AssertDrawingIsPossible(checked))
    return;
  mitk::PlanarLine::Pointer figure = mitk::PlanarLine::New();
  QString qString;
  if(m_CurrentFigureNode.IsNull() || m_LineCounter == 0 || m_CurrentFigureNodeInitialized){
	  qString = QString("Line%1").arg(++m_LineCounter);
  }
  else{
	  qString = QString("Line%1").arg(m_LineCounter);
  }
  this->AddFigureToDataStorage(figure, qString);

  MITK_INFO << "PlanarLine initialized...";
}

void QmitkMeasurementView::ActionDrawPathTriggered(bool checked)
{
  if(!this->AssertDrawingIsPossible(checked))
    return;

  mitk::PlanarPolygon::Pointer figure = mitk::PlanarPolygon::New();
  figure->ClosedOff();

  mitk::BoolProperty::Pointer closedProperty = mitk::BoolProperty::New( false );

  QString qString;
  if(m_CurrentFigureNode.IsNull() || m_PathCounter == 0 || m_CurrentFigureNodeInitialized){
	  qString = QString("Path%1").arg(++m_PathCounter);
  }
  else{
	  qString = QString("Path%1").arg(m_PathCounter);
  }
  this->AddFigureToDataStorage(figure, qString,
	  "ClosedPlanarPolygon", closedProperty);

  MITK_INFO << "PlanarPath initialized...";
}

void QmitkMeasurementView::ActionDrawAngleTriggered(bool checked)
{
  if(!this->AssertDrawingIsPossible(checked))
    return;

  mitk::PlanarAngle::Pointer figure = mitk::PlanarAngle::New();
  QString qString;
  if(m_CurrentFigureNode.IsNull() || m_AngleCounter == 0 || m_CurrentFigureNodeInitialized){
	  qString = QString("Angle%1").arg(++m_AngleCounter);
  }
  else{
	  qString = QString("Angle%1").arg(m_AngleCounter);
  }
  this->AddFigureToDataStorage(figure, qString);

  MITK_INFO << "PlanarAngle initialized...";
}

void QmitkMeasurementView::ActionDrawFourPointAngleTriggered(bool checked)
{
  if(!this->AssertDrawingIsPossible(checked))
    return;

  mitk::PlanarFourPointAngle::Pointer figure =
	  mitk::PlanarFourPointAngle::New();
  QString qString;
  if(m_CurrentFigureNode.IsNull() || m_FourPointAngleCounter == 0 || m_CurrentFigureNodeInitialized){
	  qString = QString("Four Point Angle%1").arg(++m_FourPointAngleCounter);
  }
  else{
	  qString = QString("Four Point Angle%1").arg(m_FourPointAngleCounter);
  }
  this->AddFigureToDataStorage(figure, qString);

  MITK_INFO << "PlanarFourPointAngle initialized...";
}

void QmitkMeasurementView::ActionDrawEllipseTriggered(bool checked)
{
  if(!this->AssertDrawingIsPossible(checked))
    return;

  mitk::PlanarCircle::Pointer figure = mitk::PlanarCircle::New();
  QString qString;
  if(m_CurrentFigureNode.IsNull() || m_EllipseCounter == 0 || m_CurrentFigureNodeInitialized){
	  qString = QString("Circle%1").arg(++m_EllipseCounter);
  }
  else{
	  qString = QString("Circle%1").arg(m_EllipseCounter);
  }
  this->AddFigureToDataStorage(figure, qString);

  MITK_INFO << "PlanarCircle initialized...";
}

void QmitkMeasurementView::ActionDrawRectangleTriggered(bool checked)
{
  if(!this->AssertDrawingIsPossible(checked))
    return;

  mitk::PlanarRectangle::Pointer figure = mitk::PlanarRectangle::New();
  QString qString;
  if(m_CurrentFigureNode.IsNull() || m_RectangleCounter == 0 || m_CurrentFigureNodeInitialized){
	  qString = QString("Rectangle%1").arg(++m_RectangleCounter);
  }
  else{
	  qString = QString("Rectangle%1").arg(m_RectangleCounter);
  }
  this->AddFigureToDataStorage(figure, qString);

  MITK_INFO << "PlanarRectangle initialized...";
}

void QmitkMeasurementView::ActionDrawPolygonTriggered(bool checked)
{
  if(!this->AssertDrawingIsPossible(checked))
    return;

  mitk::PlanarPolygon::Pointer figure = mitk::PlanarPolygon::New();
  figure->ClosedOn();
	QString qString;
	if(m_CurrentFigureNode.IsNull() || m_PolygonCounter == 0 || m_CurrentFigureNodeInitialized){
	  qString = QString("Polygon%1").arg(++m_PolygonCounter);
  }
  else{
	  qString = QString("Polygon%1").arg(m_PolygonCounter);
  }
  this->AddFigureToDataStorage(figure, qString);

  MITK_INFO << "PlanarPolygon initialized...";
}

void QmitkMeasurementView::ActionDrawArrowTriggered(bool checked)
{
  if(!this->AssertDrawingIsPossible(checked))
    return;

  MITK_WARN << "Draw Arrow not implemented yet.";
}

void QmitkMeasurementView::ActionDrawTextTriggered(bool checked)
{
  if(!this->AssertDrawingIsPossible(checked))
    return;

  MITK_WARN << "Draw Text not implemented yet.";
}

void QmitkMeasurementView::Activated()
{
  m_Activated = true;
  MITK_INFO << "QmitkMeasurementView::Activated";

  if (mitk::ILinkedRenderWindowPart* linkedRenderWindow =
      dynamic_cast<mitk::ILinkedRenderWindowPart*>(this->GetRenderWindowPart()))
  {
    linkedRenderWindow->EnableSlicingPlanes(false);
  }

  mitk::TNodePredicateDataType<mitk::PlanarFigure>::Pointer isPlanarFigure
    = mitk::TNodePredicateDataType<mitk::PlanarFigure>::New();

  mitk::DataStorage::SetOfObjects::ConstPointer _NodeSet = this->GetDataStorage()->GetAll();
  mitk::DataNode* node = 0;
  mitk::PlanarFigure* figure = 0;
  mitk::PlanarFigureInteractor::Pointer figureInteractor = 0;
  // finally add all nodes to the model
  for(mitk::DataStorage::SetOfObjects::ConstIterator it=_NodeSet->Begin(); it!=_NodeSet->End()
    ; it++)
  {
    node = const_cast<mitk::DataNode*>(it->Value().GetPointer());
    figure = dynamic_cast<mitk::PlanarFigure*>(node->GetData());
    if(figure)
    {
      figureInteractor = dynamic_cast<mitk::PlanarFigureInteractor*>(node->GetInteractor());

      if(figureInteractor.IsNull())
        figureInteractor = mitk::PlanarFigureInteractor::New("PlanarFigureInteractor", node);

      mitk::GlobalInteraction::GetInstance()->AddInteractor(figureInteractor);
    }
  }

  m_Visible = true;

}

void QmitkMeasurementView::Deactivated()
{
  MITK_INFO << "QmitkMeasurementView::Deactivated";
}

void QmitkMeasurementView::ActivatedZombieView(berry::IWorkbenchPartReference::Pointer /*newZombiePart*/)
{
  if (mitk::ILinkedRenderWindowPart* linkedRenderWindow =
      dynamic_cast<mitk::ILinkedRenderWindowPart*>(this->GetRenderWindowPart()))
  {
    linkedRenderWindow->EnableSlicingPlanes(true);
  }

  this->SetMeasurementInfoToRenderWindow("", m_LastRenderWindow);
  this->EnableCrosshairNavigation();

  mitk::DataStorage::SetOfObjects::ConstPointer _NodeSet = this->GetDataStorage()->GetAll();
  mitk::DataNode* node = 0;
  mitk::PlanarFigure* figure = 0;
  mitk::PlanarFigureInteractor::Pointer figureInteractor = 0;
  // finally add all nodes to the model
  for(mitk::DataStorage::SetOfObjects::ConstIterator it=_NodeSet->Begin(); it!=_NodeSet->End()
    ; it++)
  {
    node = const_cast<mitk::DataNode*>(it->Value().GetPointer());
    figure = dynamic_cast<mitk::PlanarFigure*>(node->GetData());
    if(figure)
    {

      figure->RemoveAllObservers();
      figureInteractor = dynamic_cast<mitk::PlanarFigureInteractor*>(node->GetInteractor());

      if(figureInteractor)
        mitk::GlobalInteraction::GetInstance()->RemoveInteractor(figureInteractor);
    }
  }

  m_Activated = false;
}

void QmitkMeasurementView::Visible()
{
  m_Visible = true;
  MITK_INFO << "QmitkMeasurementView::Visible";
}

void QmitkMeasurementView::Hidden()
{
  m_Visible = false;
  MITK_INFO << "QmitkMeasurementView::Hidden";
}

void QmitkMeasurementView::PropertyChanged(const mitk::DataNode* /*node*/, const mitk::BaseProperty* /*prop*/)
{
  this->PlanarFigureSelectionChanged();
}

void QmitkMeasurementView::NodeChanged(const mitk::DataNode* /*node*/)
{
  this->PlanarFigureSelectionChanged();
}

void QmitkMeasurementView::NodeRemoved(const mitk::DataNode* /*node*/)
{
  this->PlanarFigureSelectionChanged();
}

void QmitkMeasurementView::CopyToClipboard(bool)
{
  std::vector<QString> headerRow;
  std::vector<std::vector<QString> > rows;
  QString featureName;
  QString featureQuantity;
  std::vector<QString> newRow;
  headerRow.push_back("Name");

  std::vector<mitk::DataNode*> nodes = m_SelectedPlanarFigures->GetNodes();

  for (std::vector<mitk::DataNode*>::iterator it = nodes.begin(); it
    != nodes.end(); ++it)
  {
    mitk::PlanarFigure* planarFigure =
      dynamic_cast<mitk::PlanarFigure*> ((*it)->GetData());
    if (!planarFigure)
      continue;

    newRow.clear();
    newRow.push_back(QString::fromStdString((*it)->GetName()));
    newRow.resize(headerRow.size());
    for (unsigned int i = 0; i < planarFigure->GetNumberOfFeatures(); ++i)
    {
      if ( !planarFigure->IsFeatureActive( i ) ) continue;

      featureName = planarFigure->GetFeatureName(i);
      featureName.append(QString(" [%1]").arg(planarFigure->GetFeatureUnit(i)));
      std::vector<QString>::iterator itColumn = std::find(headerRow.begin(),
        headerRow.end(), featureName);

      featureQuantity
        = QString("%1").arg(planarFigure->GetQuantity(i)).replace(QChar('.'),
        ",");
      if (itColumn == headerRow.end())
      {
        headerRow.push_back(featureName);
        newRow.push_back(featureQuantity);
      } else
      {
        newRow[std::distance(headerRow.begin(), itColumn)] = featureQuantity;
      }

    }
    rows.push_back(newRow);
  }

  QString clipboardText;
  for (std::vector<QString>::iterator it = headerRow.begin(); it
    != headerRow.end(); ++it)
    clipboardText.append(QString("%1 \t").arg(*it));

  for (std::vector<std::vector<QString> >::iterator it = rows.begin(); it
    != rows.end(); ++it)
  {
    clipboardText.append("\n");
    for (std::vector<QString>::iterator it2 = (*it).begin(); it2 != (*it).end(); ++it2)
    {
      clipboardText.append(QString("%1 \t").arg(*it2));
    }
  }

  QApplication::clipboard()->setText(clipboardText, QClipboard::Clipboard);

}

void QmitkMeasurementView::SetMeasurementInfoToRenderWindow(const QString& text,
                                                        QmitkRenderWindow* _RenderWindow)
{
  if(m_LastRenderWindow != _RenderWindow)
  {

    if(m_LastRenderWindow)
    {
      QObject::disconnect( m_LastRenderWindow, SIGNAL( destroyed(QObject*) )
        , this, SLOT( OnRenderWindowDelete(QObject*) ) );
    }
    m_LastRenderWindow = _RenderWindow;
    if(m_LastRenderWindow)
    {
      QObject::connect( m_LastRenderWindow, SIGNAL( destroyed(QObject*) )
        , this, SLOT( OnRenderWindowDelete(QObject*) ) );
    }
  }

  if(m_LastRenderWindow)
  {
    if (!text.isEmpty() && m_SelectedPlanarFigures->GetNode()->IsSelected())
    {
      m_MeasurementInfoAnnotation->SetText(1, text.toLatin1().data());
      mitk::VtkLayerController::GetInstance(m_LastRenderWindow->GetRenderWindow())->InsertForegroundRenderer(
        m_MeasurementInfoRenderer, true);
    }
    else
    {
      if (mitk::VtkLayerController::GetInstance(
        m_LastRenderWindow->GetRenderWindow()) ->IsRendererInserted(
        m_MeasurementInfoRenderer))
        mitk::VtkLayerController::GetInstance(m_LastRenderWindow->GetRenderWindow())->RemoveRenderer(
        m_MeasurementInfoRenderer);
    }
  }
}

void QmitkMeasurementView::EnableCrosshairNavigation()
{
  // enable the crosshair navigation
  if (mitk::ILinkedRenderWindowPart* linkedRenderWindow =
      dynamic_cast<mitk::ILinkedRenderWindowPart*>(this->GetRenderWindowPart()))
  {
    linkedRenderWindow->EnableLinkedNavigation(true);
  }
}

void QmitkMeasurementView::DisableCrosshairNavigation()
{
  // disable the crosshair navigation during the drawing
  if (mitk::ILinkedRenderWindowPart* linkedRenderWindow =
      dynamic_cast<mitk::ILinkedRenderWindowPart*>(this->GetRenderWindowPart()))
  {
    linkedRenderWindow->EnableLinkedNavigation(false);
  }
}

void QmitkMeasurementView::SetFocus()
{

}

void QmitkMeasurementView::OnRenderWindowDelete(QObject * obj)
{
  if(obj == m_LastRenderWindow)
    m_LastRenderWindow = 0;
}
    
void QmitkMeasurementView::ReproducePotentialBug(bool)
{
  std::vector<mitk::DataNode*> nodes = m_SelectedPlanarFigures->GetNodes();
  QString output;

  for (std::vector<mitk::DataNode*>::iterator it = nodes.begin(); it
    != nodes.end(); ++it)
  {
    mitk::DataNode* node = *it;
    if (!node) continue;

    mitk::PlanarFigure* pf = dynamic_cast<mitk::PlanarFigure*>( node->GetData() );
    if (!pf) continue;

    output.append("huhu");
    output.append( QString::fromStdString( node->GetName() ) );

    /**
      Bug reproduction:

      1. get geometry of planar figure from object

      2. use this geometry to initialize rendering manager via InitializeViews

      3. see what is produced. the DisplayGeometry of the render window will NOT contain the planar figure nicely.

    */

    mitk::PlaneGeometry::Pointer planarFigureGeometry = dynamic_cast<mitk::PlaneGeometry*>( pf->GetGeometry() );
    if (planarFigureGeometry.IsNull()) continue; // not expected


    mitk::PlaneGeometry::Pointer geometryForRendering = dynamic_cast<mitk::PlaneGeometry*>( planarFigureGeometry->Clone().GetPointer() );

    bool applyWorkaround(true);
    geometryForRendering->SetImageGeometry( applyWorkaround );


    std::cout << "==== with" << (applyWorkaround?"":"OUT") << " workaround ====================================" << std::endl;
    std::cout << "--- PlanarFigure geometry --------------" << std::endl;
    planarFigureGeometry->Print(std::cout);
    std::cout << "----------------------------------------" << std::endl;
    
    mitk::RenderingManager::GetInstance()->InitializeViews( geometryForRendering, mitk::RenderingManager::REQUEST_UPDATE_ALL, false );

    QmitkRenderWindow* renderWindow = this->GetRenderWindowPart()->GetRenderWindows().values().front();
    if (renderWindow == 0)
    {
      std::cout << "** No QmitkkRenderWindow available **" << std::endl;
      return;
    }

    std::cout << "--- Renderer->GetDisplayGeometry() ------------" << std::endl;

    renderWindow->GetRenderer()->GetDisplayGeometry()->Print(std::cout);
    std::cout << "--- Renderer->GetCurrentWorldGeometry2D() -----" << std::endl;
    renderWindow->GetRenderer()->GetCurrentWorldGeometry2D()->Print(std::cout);
    std::cout << "--- Renderer->GetWorldGeometry() --------------" << std::endl;
    renderWindow->GetRenderer()->GetWorldGeometry()->Print(std::cout);
  }
  
  m_SelectedPlanarFiguresText->setText(output);

}

