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

#ifndef mitkConnectomicsNetworkCreator_h
#define mitkConnectomicsNetworkCreator_h

#include <itkObject.h>
#include <itkObjectFactory.h>
#include <itkMacro.h>

#include "mitkCommon.h"
#include "mitkImage.h"

#include "mitkFiberBundleX.h"
#include "mitkConnectomicsNetwork.h"

#include "MitkDiffusionImagingExports.h"

namespace mitk
{

  class MitkDiffusionImaging_EXPORT ConnectomicsNetworkCreator : public itk::Object
  {
  public:

    /** Enum for different ways to create the mapping from fibers to network */

    enum MappingStrategy
    {
      EndElementPosition,
      PrecomputeAndDistance,
      JustEndPointVerticesNoLabel,
      EndElementPositionAvoidingWhiteMatter
    };

    /** Standard class typedefs. */
    /** Method for creation through the object factory. */

    mitkClassMacro(ConnectomicsNetworkCreator, itk::Object);
    itkNewMacro(Self);

    /** Types for the standardized Tract **/
    typedef itk::Point<float,3>                                          PointType; 
    typedef itk::VectorContainer<unsigned int, PointType>                TractType;
    typedef itk::VectorContainer< unsigned int, TractType::Pointer >     TractContainerType; //init via smartpointer
    

    /** Types for Network **/
    typedef mitk::ConnectomicsNetwork::VertexDescriptorType VertexType;
    typedef mitk::ConnectomicsNetwork::EdgeDescriptorType EdgeType;
    typedef mitk::ConnectomicsNetwork::NetworkNode NetworkNode;
    typedef std::pair< VertexType, VertexType >       ConnectionType;

    /** Types for labels **/
    typedef int                                             ImageLabelType;
    typedef std::pair< ImageLabelType, ImageLabelType >     ImageLabelPairType;




    void CreateNetworkFromFibersAndSegmentation();
    void SetFiberBundle(mitk::FiberBundleX::Pointer fiberBundle);
    void SetSegmentation(mitk::Image::Pointer segmentation);

    mitk::ConnectomicsNetwork::Pointer GetNetwork();

  protected:

    //////////////////// Functions ///////////////////////
    ConnectomicsNetworkCreator();
    ConnectomicsNetworkCreator( mitk::Image::Pointer segmentation, mitk::FiberBundleX::Pointer fiberBundle );
    ~ConnectomicsNetworkCreator();

    /** Add a connection to the network */
    void AddConnectionToNetwork(ConnectionType newConnection);

    /** Determine if a label is already identified with a vertex, otherwise create a new one */
    VertexType ReturnAssociatedVertexForLabel( ImageLabelType label );

    /** Return the vertexes associated with a pair of labels */
    ConnectionType ReturnAssociatedVertexPairForLabelPair( ImageLabelPairType labelpair );

    /** Return the pair of labels which identify the areas connected by a single fiber */
    ImageLabelPairType ReturnLabelForFiberTract( TractType::Pointer singleTract, MappingStrategy strategy );

    /** Assign the additional information which should be part of the vertex */
    void SupplyVertexWithInformation( ImageLabelType& label, VertexType& vertex );

    /** Create a string from the label */
    std::string LabelToString( ImageLabelType& label );

    /** Check whether the label in question belongs to white matter according to the freesurfer table */
    bool IsNonWhiteMatterLabel( int labelInQuestion );

    /** Check whether the label in question belongs to background according to the freesurfer table */
    bool IsBackgroundLabel( int labelInQuestion );

    /** Extend a straight line through the given points and look for the first non white matter label  

    It will try extend in the direction of the points in the vector so a vector {B,C} will result in
    extending from C in the direction C-B */
    void LinearExtensionUntilGreyMatter( std::vector<int> & indexVectorOfPointsToUse, TractType::Pointer singleTract, 
      int & label, mitk::Index3D & mitkIndex );

    /** Retract fiber until the first brain matter label is hit  

    The bool parameter controls whether the front or the end is retracted */
    void RetractionUntilBrainMatter( bool retractFront, TractType::Pointer singleTract, 
      int & label, mitk::Index3D & mitkIndex );

    /** Convert point to itk point */
    itk::Point<float, 3> GetItkPoint(double point[3]);

    ///////// Mapping strategies //////////

    /** Use the position of the end and starting element only to map to labels 

    Map a fiber to a vertex by taking the value of the parcellation image at the same world coordinates as the last
    and first element of the tract.*/
    ImageLabelPairType EndElementPositionLabel( TractType::Pointer singleTract );

    /** Map by distance between elements and vertices depending on their volume

    First go through the parcellation and compute the coordinates of the future vertices. Assign a radius according on their volume.    
    Then map an edge to a label by considering the nearest vertices and comparing the distance to them to their radii. */
    ImageLabelPairType PrecomputeVertexLocationsBySegmentation( TractType::Pointer singleTract );

        /** Use the position of the end and starting element only to map to labels 
    
    Just take first and last position, no labelling, nothing */
    ImageLabelPairType JustEndPointVerticesNoLabelTest( TractType::Pointer singleTract );

    /** Use the position of the end and starting element unless it is in white matter, then search for nearby parcellation to map to labels 

    Map a fiber to a vertex by taking the value of the parcellation image at the same world coordinates as the last
    and first element of the tract. If this happens to be white matter, then try to extend the fiber in a line and 
    take the first non-white matter parcel, that is intersected. */
    ImageLabelPairType EndElementPositionLabelAvoidingWhiteMatter( TractType::Pointer singleTract );

    ///////// Conversions //////////
    /** Convert fiber index to segmentation index coordinates */
    void FiberToSegmentationCoords( mitk::Point3D& fiberCoord, mitk::Point3D& segCoord );
    /** Convert segmentation index to fiber index coordinates */
    void SegmentationToFiberCoords( mitk::Point3D& segCoord, mitk::Point3D& fiberCoord );

    /////////////////////// Variables ////////////////////////
    mitk::FiberBundleX::Pointer m_FiberBundle;
    mitk::Image::Pointer m_Segmentation;

    // the graph itself
    mitk::ConnectomicsNetwork::Pointer m_ConNetwork;

    // the id counter
    int idCounter;

    // the map mapping labels to vertices
    std::map< ImageLabelType, VertexType > m_LabelToVertexMap;

    // mapping labels to additional information
    std::map< ImageLabelType, NetworkNode > m_LabelToNodePropertyMap;

    // toggles whether edges between a node and itself can exist
    bool allowLoops;

    //////////////////////// IDs ////////////////////////////

    // These IDs are the freesurfer ids used in parcellation

    static const int freesurfer_Left_Cerebral_White_Matter = 2;
    static const int freesurfer_Left_Cerebellum_White_Matter = 7;
    static const int freesurfer_Right_Cerebral_White_Matter = 41;
    static const int freesurfer_Right_Cerebellum_White_Matter = 46;

  };

}// end namespace mitk

#endif // _mitkConnectomicsNetworkCreator_H_INCLUDED