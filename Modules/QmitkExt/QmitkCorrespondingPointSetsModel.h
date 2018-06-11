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

#ifndef QMITK_CORRESPONDINGPOINTSETS_MODEL_H_INCLUDED
#define QMITK_CORRESPONDINGPOINTSETS_MODEL_H_INCLUDED

#include <QAbstractTableModel>
#include <mitkStepper.h>
#include "QmitkExtExports.h"

#include "mitkDataNode.h"
#include "mitkPointSet.h"
#include <mitkPointSetInteractor.h>
#include "QmitkStdMultiWidget.h"

class QmitkExt_EXPORT QmitkCorrespondingPointSetsModel : public QAbstractTableModel
{
  Q_OBJECT
  Q_PROPERTY(bool QTPropShowCoordinates READ QTPropCoordinatesEnabled WRITE QTPropSetCoordinatesEnabled)
  Q_PROPERTY(bool QTPropShowIds READ QTPropIdsEnabled WRITE QTPropSetIdsEnabled)

public:

  QmitkCorrespondingPointSetsModel( int t = 0, QObject* parent = 0 );
  ~QmitkCorrespondingPointSetsModel();

  Qt::ItemFlags flags(const QModelIndex& index) const;

  void UpdateSelection(mitk::DataNode* selectedNode);

  void RemoveInteractor();

  // returns PointIdentifier of selected point (-1 if no point is selected)
  mitk::PointSet::PointIdentifier SearchSelectedPoint();

  /// interface of QAbstractTableModel
  int rowCount( const QModelIndex& parent = QModelIndex() ) const;

  /// interface of QAbstractTableModel
  int columnCount( const QModelIndex& parent = QModelIndex() ) const;

  /// interface of QAbstractTableModel
  QVariant data(const QModelIndex& index, int role) const;

  /// interface of QAbstractTableModel
  QVariant headerData(int section, Qt::Orientation orientation,
    int role = Qt::DisplayRole) const;

  /// which point set to work on
  void SetPointSetNodes( std::vector<mitk::DataNode*> nodes );

  /// which time step to display/model
  void SetTimeStep(int t);

  /// which time step to display/model
  int GetTimeStep() const;

  /**
  * \brief get point and point ID that correspond to a given QModelIndex
  *
  * The mitk::PointSet uses a map to store points in an ID<-->Point relation.
  * The IDs are not neccesarily continuously numbered, therefore, we can not
  * directly use the QModelIndex as point ID. This method returns the point and
  * the corresponding point id for a given QModelIndex. The point and the point ID
  * are returned in the outgoing parameters p and id. If a valid point and ID were
  * found, the method returns true, otherwise it returns false
  * \param[in] QModelIndex &index the index for which a point is requested.
  The row() part of the index is used to find a corresponding point
  * \param[out] mitk::Point3D& p If a valid point is found, it will be stored in the p parameter
  * \param[out] mitk::PointSet::PointIdentifier& id If a valid point is found, the corresponding ID will be stored in id
  * \return Returns true, if a valid point was found, false otherwise
  */
  bool GetPointForModelIndex( const QModelIndex &index, mitk::PointSet::PointType& p, mitk::PointSet::PointIdentifier& id) const;
  bool GetPointForModelIndex( int row, int column, mitk::PointSet::PointType& p, mitk::PointSet::PointIdentifier& id) const;

  /**Documentation
  * \brief returns a QModelIndex for a given point ID
  *
  * The mitk::PointSet uses a map to store points in an ID<-->Point relation.
  * The IDs are not neccesarily continuously numbered, therefore, we can not
  * directly use the point ID as a QModelIndex. This method returns a QModelIndex
  * for a given point ID in the outgoing parameter index.
  * \param[in] mitk::PointSet::PointIdentifier id The point ID for which the QModelIndex will be created
  * \param[out] QModelIndex& index if a point with the ID id was found, index will contain a corresponding QModelIndex for that point
  * \return returns true, if a valid QModelIndex was created, false otherwise
  */
  bool GetModelIndexForPointID(mitk::PointSet::PointIdentifier id, QModelIndex& index, int column) const;

  bool QTPropCoordinatesEnabled() const;

  void QTPropSetCoordinatesEnabled(bool qShowCoordinates);

  bool QTPropIdsEnabled() const;

  void QTPropSetIdsEnabled(bool qShowIds);

  std::vector<mitk::DataNode*> GetPointSetNodes();

  void SetSelectedPointSetIndex(int index);

  int GetSelectedPointSetIndex();

  void ClearSelectedPointSet();

  void MoveSelectedPointUp();

  void MoveSelectedPointDown();

  void RemoveSelectedPoint();

  void ClearCurrentTimeStep();

  void SetStepper(mitk::Stepper::Pointer stepper);

  mitk::Stepper::Pointer GetStepper();

  Qt::DropActions supportedDropActions() const;

  bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);

  bool GetModelIndexForSelectedPoint(QModelIndex& index) const;

  void SetMultiWidget( QmitkStdMultiWidget* multiWidget ); ///< assign a QmitkStdMultiWidget for updating render window crosshair

  QmitkStdMultiWidget* GetMultiWidget();  ///< return the QmitkStdMultiWidget that is used for updating render window crosshair

  void OnPointSetChanged( const itk::EventObject &  e );

private:


public slots:

signals:

  void SignalPointSetChanged();

protected:
  //initially checks if there is a PointSet as data in the DataNode.
  //returns PointSet if so and NULL if other data is set to node
  mitk::PointSet* CheckForPointSetInNode(mitk::DataNode* node) const;

protected:

  bool QTPropShowIds;
  bool QTPropShowCoordinates;
  mitk::DataNode::Pointer m_PointSetNode;
  mitk::DataNode::Pointer m_ReferencePointSetNode;
  mitk::Stepper::Pointer m_TimeStepper;
  int m_SelectedPointSetIndex;
  mitk::PointSetInteractor::Pointer   m_Interactor;
  QmitkStdMultiWidget*    m_MultiWidget;
  unsigned long m_PointSetModifiedObserverTag;
  unsigned long m_ReferencePointSetModifiedObserverTag;

  void MoveSelectedPoint(mitk::PointSet::PointIdentifier targetID);
  void RemoveObservers();
  void AddObservers();
};
#endif