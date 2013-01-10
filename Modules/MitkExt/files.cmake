set(CPP_FILES
  Algorithms/mitkMaskAndCutRoiImageFilter.cpp
  Algorithms/mitkBoundingObjectToSegmentationFilter.cpp
  Algorithms/vtkPointSetSlicer.cxx
  Algorithms/mitkCoreExtObjectFactory.cpp
  Algorithms/mitkAngleCorrectByPointFilter.cpp
  Algorithms/mitkAutoCropImageFilter.cpp
  Algorithms/mitkBoundingObjectCutter.cpp
  Algorithms/mitkCloneImageFilter.cpp
  Algorithms/mitkCylindricToCartesianFilter.cpp
  Algorithms/mitkDopplerToStrainRateFilter.cpp
  Algorithms/mitkGeometryClipImageFilter.cpp
  Algorithms/mitkGeometryDataSource.cpp
  Algorithms/mitkHeightFieldSurfaceClipImageFilter.cpp
  Algorithms/mitkImageToLookupTableFilter.cpp
  Algorithms/mitkImageToSurfaceFilter.cpp
  Algorithms/mitkInterpolateLinesFilter.cpp
  Algorithms/mitkLabeledImageToSurfaceFilter.cpp
  Algorithms/mitkLabeledImageVolumeCalculator.cpp
  Algorithms/mitkLookupTableSource.cpp
  Algorithms/mitkMaskImageFilter.cpp
  Algorithms/mitkMeshSource.cpp
  Algorithms/mitkNonBlockingAlgorithm.cpp
  Algorithms/mitkPadImageFilter.cpp
  Algorithms/mitkPlaneCutFilter.cpp
  Algorithms/mitkPlaneFit.cpp
  Algorithms/mitkPlanesPerpendicularToLinesFilter.cpp
  Algorithms/mitkPointLocator.cpp
  Algorithms/mitkPointSetToCurvedGeometryFilter.cpp
  Algorithms/mitkPointSetToGeometryDataFilter.cpp
  Algorithms/mitkPointSetIndexToWorldTransformFilter.cpp
  Algorithms/mitkSurfaceIndexToWorldTransformFilter.cpp
  Algorithms/mitkPolygonToRingFilter.cpp
  Algorithms/mitkProbeFilter.cpp
  Algorithms/mitkSimpleHistogram.cpp
  Algorithms/mitkSimpleUnstructuredGridHistogram.cpp
  Algorithms/mitkSurfaceToImageFilter.cpp
  Algorithms/mitkUnstructuredGridHistogram.cpp
  Algorithms/mitkUnstructuredGridSource.cpp
  Algorithms/mitkVolumeVisualizationImagePreprocessor.cpp
  Controllers/mitkMovieGenerator.cpp
  Controllers/mitkMultiStepper.cpp
  Controllers/mitkToolManager.cpp
  DataManagement/mitkAffineTransformationOperation.cpp
  DataManagement/mitkApplyDiffImageOperation.cpp
  DataManagement/mitkBoundingObject.cpp
  DataManagement/mitkBoundingObjectGroup.cpp
  DataManagement/mitkCellOperation.cpp
  DataManagement/mitkColorConversions.cpp
  DataManagement/mitkColorSequence.cpp
  DataManagement/mitkColorSequenceCycleH.cpp
  DataManagement/mitkColorSequenceHalfTones.cpp
  DataManagement/mitkColorSequenceRainbow.cpp
  DataManagement/mitkCompressedImageContainer.cpp
  DataManagement/mitkCone.cpp
  DataManagement/mitkCuboid.cpp
  DataManagement/mitkCylinder.cpp
  DataManagement/mitkDataStorageSelection.cpp
  DataManagement/mitkDelegateManager.cpp
  DataManagement/mitkDrawOperation.cpp
  DataManagement/mitkEllipsoid.cpp
  DataManagement/mitkExternAbstractTransformGeometry.cpp
  DataManagement/mitkFrameOfReferenceUIDManager.cpp
  DataManagement/mitkGridRepresentationProperty.cpp
  DataManagement/mitkGridVolumeMapperProperty.cpp
  DataManagement/mitkItkBaseDataAdapter.cpp
  DataManagement/mitkLabeledImageLookupTable.cpp
  DataManagement/mitkLineOperation.cpp
  DataManagement/mitkMesh.cpp
  DataManagement/mitkObjectSet.cpp
  DataManagement/mitkOrganTypeProperty.cpp
  DataManagement/mitkPlaneLandmarkProjector.cpp
  DataManagement/mitkPlane.cpp
  DataManagement/mitkPropertyManager.cpp
  DataManagement/mitkPropertyObserver.cpp
  DataManagement/mitkSeedsImage.cpp
  DataManagement/mitkSeedsImageLookupTableSource.cpp
  DataManagement/mitkSphereLandmarkProjector.cpp
# DataManagement/mitkUSLookupTableSource.cpp
  DataManagement/mitkUnstructuredGrid.cpp
  DataManagement/mitkVideoSource.cpp
  DataManagement/vtkObjectSet.cpp
  IO/mitkObjFileIOFactory.cpp
  IO/mitkObjFileReader.cpp
  IO/mitkPACSPlugin.cpp
  IO/mitkParRecFileIOFactory.cpp
  IO/mitkParRecFileReader.cpp
  IO/mitkStlVolumeTimeSeriesIOFactory.cpp
  IO/mitkStlVolumeTimeSeriesReader.cpp
  IO/mitkUnstructuredGridVtkWriter.cpp
  IO/mitkUnstructuredGridVtkWriterFactory.cpp
  IO/mitkVtkUnstructuredGridIOFactory.cpp
  IO/mitkVtkUnstructuredGridReader.cpp
  IO/mitkVtkVolumeTimeSeriesIOFactory.cpp
  IO/mitkVtkVolumeTimeSeriesReader.cpp
  Interactions/mitkConferenceEventMapper.cpp
  Interactions/mitkConnectPointsInteractor.cpp
  #Interactions/mitkCoordinateSupplier.cpp
  #Interactions/mitkDisplayCoordinateOperation.cpp
  #Interactions/mitkDisplayInteractor.cpp
  Interactions/mitkAffineInteractor3D.cpp
  Interactions/mitkDisplayPointSetInteractor.cpp
  #Interactions/mitkDisplayVectorInteractor.cpp
  Interactions/mitkInteractionDebug.cpp
  Interactions/mitkInteractionDebugger.cpp
  Interactions/mitkPointInteractor.cpp
  Interactions/mitkPointSelectorInteractor.cpp
  #Interactions/mitkPositionTracker.cpp
  Interactions/mitkSeedsInteractor.cpp
  Interactions/mitkSocketClient.cpp
  Interactions/mitkSurfaceDeformationInteractor3D.cpp
  Interactions/mitkSurfaceInteractor.cpp
  Interactions/mitkTool.cpp
#  Interactions/mitkCreateSurfaceTool.cpp
  Interactions/mitkMorphologicTool.cpp
  Interactions/mitkErodeTool.cpp
  Interactions/mitkDilateTool.cpp
  Interactions/mitkOpeningTool.cpp
  Interactions/mitkClosingTool.cpp
  Interactions/mitkPixelManipulationTool.cpp
  Rendering/mitkEnhancedPointSetVtkMapper3D.cpp
  Rendering/mitkImageBackground2D.cpp
  Rendering/mitkLineMapper2D.cpp
# Rendering/mitkLineVtkMapper3D.cpp
  Rendering/mitkMeshMapper2D.cpp
  Rendering/mitkMeshVtkMapper3D.cpp
  Rendering/mitkNativeRenderWindowInteractor.cpp

  Rendering/mitkSplineMapper2D.cpp
  Rendering/mitkSplineVtkMapper3D.cpp
  Rendering/mitkUnstructuredGridMapper2D.cpp
  Rendering/mitkUnstructuredGridVtkMapper3D.cpp
  Rendering/mitkVectorImageMapper2D.cpp
  Rendering/vtkUnstructuredGridMapper.cpp
  Rendering/vtkMaskedGlyph2D.cpp
  Rendering/vtkMaskedGlyph3D.cpp
  Rendering/vtkMitkVolumeTextureMapper3D.cpp
  Rendering/vtkMitkOpenGLVolumeTextureMapper3D.cpp
  Rendering/mitkGPUVolumeMapper3D.cpp
  Rendering/vtkMitkGPUVolumeRayCastMapper.cpp
)

if(WIN32 AND NOT MINGW)
  set(CPP_FILES
    Controllers/mitkMovieGeneratorWin32.cpp
    ${CPP_FILES}
  )
endif(WIN32 AND NOT MINGW)



