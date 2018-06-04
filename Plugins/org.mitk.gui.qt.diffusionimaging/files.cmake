set(SRC_CPP_FILES
  QmitkODFDetailsWidget.cpp
  QmitkODFRenderWidget.cpp
  QmitkPartialVolumeAnalysisWidget.cpp
  QmitkIVIMWidget.cpp
  QmitkTbssRoiAnalysisWidget.cpp
  QmitkResidualAnalysisWidget.cpp
  QmitkResidualViewWidget.cpp
)

set(INTERNAL_CPP_FILES
  mitkPluginActivator.cpp
  QmitkQBallReconstructionView.cpp
  QmitkPreprocessingView.cpp
  QmitkDiffusionDicomImportView.cpp
  QmitkDiffusionQuantificationView.cpp
  QmitkTensorReconstructionView.cpp
  QmitkDiffusionImagingPublicPerspective.cpp
  QmitkControlVisualizationPropertiesView.cpp
  QmitkODFDetailsView.cpp
  QmitkGibbsTrackingView.cpp
  QmitkStochasticFiberTrackingView.cpp
  QmitkFiberProcessingView.cpp
  QmitkFiberBundleDeveloperView.cpp
  QmitkPartialVolumeAnalysisView.cpp
  QmitkIVIMView.cpp
  QmitkTractbasedSpatialStatisticsView.cpp
  QmitkTbssTableModel.cpp
  QmitkTbssMetaTableModel.cpp
  QmitkTbssSkeletonizationView.cpp
  Connectomics/QmitkBrainNetworkAnalysisView.cpp
  Connectomics/QmitkNetworkHistogramCanvas.cpp
)

set(UI_FILES
  src/internal/QmitkQBallReconstructionViewControls.ui
  src/internal/QmitkPreprocessingViewControls.ui
  src/internal/QmitkDiffusionDicomImportViewControls.ui
  src/internal/QmitkDiffusionQuantificationViewControls.ui
  src/internal/QmitkTensorReconstructionViewControls.ui
  src/internal/QmitkControlVisualizationPropertiesViewControls.ui
  src/internal/QmitkODFDetailsViewControls.ui
  src/internal/QmitkGibbsTrackingViewControls.ui
  src/internal/QmitkStochasticFiberTrackingViewControls.ui
  src/internal/QmitkFiberProcessingViewControls.ui
  src/internal/QmitkFiberBundleDeveloperViewControls.ui
  src/internal/QmitkPartialVolumeAnalysisViewControls.ui
  src/internal/QmitkIVIMViewControls.ui
  src/internal/QmitkTractbasedSpatialStatisticsViewControls.ui
  src/internal/QmitkTbssSkeletonizationViewControls.ui
  src/internal/Connectomics/QmitkBrainNetworkAnalysisViewControls.ui
)

set(MOC_H_FILES
  src/internal/mitkPluginActivator.h
  src/internal/QmitkQBallReconstructionView.h
  src/internal/QmitkPreprocessingView.h
  src/internal/QmitkDiffusionDicomImportView.h
  src/internal/QmitkDiffusionImagingPublicPerspective.h
  src/internal/QmitkDiffusionQuantificationView.h
  src/internal/QmitkTensorReconstructionView.h
  src/internal/QmitkControlVisualizationPropertiesView.h
  src/internal/QmitkODFDetailsView.h
  src/QmitkODFRenderWidget.h
  src/QmitkODFDetailsWidget.h
  src/internal/QmitkGibbsTrackingView.h
  src/internal/QmitkStochasticFiberTrackingView.h
  src/internal/QmitkFiberProcessingView.h
  src/internal/QmitkFiberBundleDeveloperView.h
  src/internal/QmitkPartialVolumeAnalysisView.h
  src/QmitkPartialVolumeAnalysisWidget.h
  src/internal/QmitkIVIMView.h
  src/internal/QmitkTractbasedSpatialStatisticsView.h
  src/internal/QmitkTbssSkeletonizationView.h
  src/QmitkTbssRoiAnalysisWidget.h
  src/QmitkResidualAnalysisWidget.h
  src/QmitkResidualViewWidget.h
  src/internal/Connectomics/QmitkBrainNetworkAnalysisView.h
  src/internal/Connectomics/QmitkNetworkHistogramCanvas.h
)

set(CACHED_RESOURCE_FILES
# list of resource files which can be used by the plug-in
# system without loading the plug-ins shared library,
# for example the icon used in the menu and tabs for the
# plug-in views in the workbench
  plugin.xml

  resources/preprocessing.png
  resources/dwiimport.png
  resources/quantification.png
  resources/reconodf.png
  resources/recontensor.png
  resources/vizControls.png
  resources/OdfDetails.png
  resources/GibbsTracking.png
  resources/FiberBundleOperations.png
  resources/PartialVolumeAnalysis_24.png
  resources/IVIM_48.png
  resources/stochFB.png
  resources/tbss.png
  resources/QmitkBrainNetworkAnalysisViewIcon_48.png
)

set(QRC_FILES
# uncomment the following line if you want to use Qt resources
  resources/QmitkDiffusionImaging.qrc
  #resources/QmitkTractbasedSpatialStatisticsView.qrc
)

set(CPP_FILES )

foreach(file ${SRC_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/${file})
endforeach(file ${SRC_CPP_FILES})

foreach(file ${INTERNAL_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/internal/${file})
endforeach(file ${INTERNAL_CPP_FILES})

