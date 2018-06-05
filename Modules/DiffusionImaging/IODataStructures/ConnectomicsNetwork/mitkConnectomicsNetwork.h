
/*=========================================================================

 Program:   Medical Imaging & Interaction Toolkit
 Module:    $RCSfile$
 Language:  C++
 Date:      $Date$
 Version:   $Revision: 11989 $

 Copyright (c) German Cancer Research Center, Division of Medical and
 Biological Informatics. All rights reserved.
 See MITKCopyright.txt or http://www.mitk.org/copyright.html for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.

 =========================================================================*/


#ifndef _MITK_ConnectomicsNetwork_H
#define _MITK_ConnectomicsNetwork_H

#include "MitkDiffusionImagingExports.h"

#include "mitkBaseData.h"

#include <boost/graph/adjacency_list.hpp>

namespace mitk {

  /**
  * \brief Base Class for Connectomics Networks   */
  class MitkDiffusionImaging_EXPORT ConnectomicsNetwork : public BaseData
  {
  public:
    /** Structs for the graph */

    /** The Node */
    struct NetworkNode
    {
      int id;
      std::string label;
      std::vector< float > coordinates;
    };

    /** The Edge */
    struct NetworkEdge
    {
      int sourceId;
      int targetId;
      int weight; // For now the number of times it was present
      double edge_weight; // For boost, currently set to 1 by default for unweighted calculations
    };

    /** Typedefs **/
    //typedef boost::adjacency_list< boost::listS, boost::listS, boost::undirectedS, NetworkNode, NetworkEdge > NetworkType;
    typedef boost::adjacency_list< boost::vecS, boost::vecS, boost::undirectedS, NetworkNode, NetworkEdge > NetworkType;
    typedef boost::graph_traits<NetworkType>::vertex_descriptor VertexDescriptorType;
    typedef boost::graph_traits<NetworkType>::edge_descriptor EdgeDescriptorType;

    // virtual methods that need to be implemented
    virtual void UpdateOutputInformation();
    virtual void SetRequestedRegionToLargestPossibleRegion();
    virtual bool RequestedRegionIsOutsideOfTheBufferedRegion();
    virtual bool VerifyRequestedRegion();
    virtual void SetRequestedRegion( itk::DataObject *data );


    // Macros
    mitkClassMacro( ConnectomicsNetwork, BaseData );
    itkNewMacro( Self );

    ////////////////// Interface ///////////////////
    /** return whether an edge exists between the two given vertices */
    bool EdgeExists( VertexDescriptorType vertexA, VertexDescriptorType vertexB ) const; 

    /** increase the weight of an edge between the two given vertices */
    void IncreaseEdgeWeight( VertexDescriptorType vertexA, VertexDescriptorType vertexB );

    /** add an edge between two given vertices */
    void AddEdge( VertexDescriptorType vertexA, VertexDescriptorType vertexB);

    /** add an edge between two given vertices ( with a specific weight ) */
    void AddEdge( VertexDescriptorType vertexA, VertexDescriptorType vertexB, int sourceID, int targetID, int weight = 1 );

    /** add a vertex with a specified id */
    VertexDescriptorType AddVertex( int id);

    /** set the label of a vertex */
    void SetLabel( VertexDescriptorType vertex, std::string inLabel );

    /** set the coordinates of a vertex */
    void SetCoordinates( VertexDescriptorType vertex, std::vector< float > inCoordinates );

    /** clear the graph */
    void clear();

    /** return the node struct for a given node descriptor */
    NetworkNode GetNode( VertexDescriptorType vertex ) const;

    /** return the edge struct for two given node descriptors */
    NetworkEdge GetEdge( VertexDescriptorType vertexA, VertexDescriptorType vertexB ) const;

    /** get vector containing all the nodes of the network */
    std::vector< NetworkNode > GetVectorOfAllNodes() const;

    /** get vector containing all the vertex descriptors of the network */
    std::vector< VertexDescriptorType > GetVectorOfAllVertexDescriptors() const;

    /** get vector containing the descriptors of nodes adjacent to the vertex denoted by the given descriptor  */
    std::vector< VertexDescriptorType > GetVectorOfAdjacentNodes( VertexDescriptorType vertex ) const;

    /** get vector containing all the edges of the network and the connected nodes */
    std::vector< std::pair< std::pair< NetworkNode, NetworkNode > , NetworkEdge > > GetVectorOfAllEdges() const;

    /** get overall number of vertices in the network */
    int GetNumberOfVertices() const;

    /** get overall number of edges in the network */
    int GetNumberOfEdges();

    /** get number of vertices, that are connected to themselves */
    int GetNumberOfSelfLoops();

    /** get number of vertices, that are connected to themselves */
    double GetAverageDegree();

    /** get number of edges divided by number of possible edges */
    double GetConnectionDensity();

    /** Get the maximum weight of all edges */
    int GetMaximumWeight() const;

    /** Get a vector in the format vector[ vertexID ] = degree */
    std::vector< int > GetDegreeOfNodes( ) const;

    /** Get the maximum degree of all nodes */
    int GetMaximumDegree() const;

    /** Get a vector in the format vector[ vertexID ] = clustering coefficient */
    std::vector< double > GetLocalClusteringCoefficients( );

    /** Get a vector in the format vector[ degree ] = average clustering coefficient */
    std::vector< double > GetClusteringCoefficientsByDegree( );

    /** Get the global clustering coefficient */
    double GetGlobalClusteringCoefficient( );

    /** Access boost graph directly */
    NetworkType* GetBoostGraph();

    /** Get the modified flag */
    bool GetIsModified() const;

    /** Set the modified flag */
    void SetIsModified( bool );

    /** Update the bounds of the geometry to fit the network */
    void UpdateBounds( );

  protected:
    ConnectomicsNetwork();
    virtual ~ConnectomicsNetwork();

    NetworkType m_Network;

    /// Flag which indicates whether the network has been modified since the last check
    ///
    /// mainly for rendering purposes

    bool m_IsModified;

  private:

  };

} // namespace mitk

#endif /*  _MITK_ConnectomicsNetwork_H */
