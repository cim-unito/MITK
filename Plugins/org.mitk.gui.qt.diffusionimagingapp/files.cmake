set(SRC_CPP_FILES
  QmitkDiffusionImagingAppApplication.cpp
  QmitkDiffusionImagingAppWorkbenchAdvisor.cpp
)

set(INTERNAL_CPP_FILES
  mitkPluginActivator.cpp
  QmitkDiffusionImagingAppIntroPart.cpp
  QmitkDiffusionImagingAppPerspective.cpp
  QmitkWelcomePerspective.cpp
  QmitkDIAppConnectomicsPerspective.cpp
  QmitkDIAppDicomImportPerspective.cpp
  QmitkDIAppFiberTractographyPerspective.cpp
  QmitkDIAppIVIMPerspective.cpp
  QmitkDIAppPreprocessingReconstructionPerspective.cpp
  QmitkDIAppQuantificationPerspective.cpp
  QmitkDIAppScreenshotsMoviesPerspective.cpp
  QmitkDIAppTBSSPerspective.cpp
  QmitkDIAppVolumeVisualizationPerspective.cpp
  QmitkDIAppTrackingEvaluationPerspective.cpp
)

set(UI_FILES
  src/internal/QmitkWelcomeScreenViewControls.ui
)

set(MOC_H_FILES
  src/internal/QmitkDiffusionImagingAppIntroPart.h
  src/internal/mitkPluginActivator.h
  src/QmitkDiffusionImagingAppApplication.h
  src/internal/QmitkDiffusionImagingAppPerspective.h
  src/internal/QmitkWelcomePerspective.h
  src/internal/QmitkDIAppConnectomicsPerspective.h
  src/internal/QmitkDIAppDicomImportPerspective.h
  src/internal/QmitkDIAppFiberTractographyPerspective.h
  src/internal/QmitkDIAppIVIMPerspective.h
  src/internal/QmitkDIAppPreprocessingReconstructionPerspective.h
  src/internal/QmitkDIAppQuantificationPerspective.h
  src/internal/QmitkDIAppScreenshotsMoviesPerspective.h
  src/internal/QmitkDIAppTBSSPerspective.h
  src/internal/QmitkDIAppVolumeVisualizationPerspective.h
  src/internal/QmitkDIAppTrackingEvaluationPerspective.h
)

set(CACHED_RESOURCE_FILES
# list of resource files which can be used by the plug-in
# system without loading the plug-ins shared library,
# for example the icon used in the menu and tabs for the
# plug-in views in the workbench
  plugin.xml
  resources/icon_dicom.xpm
  resources/diffusionimaging.png
  resources/preprocessing.png
  resources/Measurement_48.png
  resources/volvis.png
  resources/perspectives/diffusionimaging.png
  resources/perspectives/icon_home.png
  resources/perspectives/connectomics.png
  resources/perspectives/dicomimport.png
  resources/perspectives/tractography.png
  resources/perspectives/ivim.png
  resources/perspectives/preprocessingreconstruction.png
  resources/perspectives/quantification.png
  resources/perspectives/screenshotsmovies.png
  resources/perspectives/tbss.png
  resources/perspectives/volumevizualization.png
)

set(QRC_FILES
# uncomment the following line if you want to use Qt resources
  resources/welcome/QmitkWelcomeScreenView.qrc
  resources/org_mitk_gui_qt_diffusionimagingapp.qrc
)

# set(CPP_FILES)

foreach(file ${SRC_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/${file})
endforeach(file ${SRC_CPP_FILES})

foreach(file ${INTERNAL_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/internal/${file})
endforeach(file ${INTERNAL_CPP_FILES})


#----------- Qt Help Collection Project -------------#

if(BLUEBERRY_USE_QT_HELP)
  set(_plugin_qhcp_input "${CMAKE_CURRENT_SOURCE_DIR}/documentation/MitkDiffusionImagingAppQtHelpCollectionProject.qhcp")
  set(_plugin_qhcp_output "${CMAKE_CURRENT_BINARY_DIR}/MitkDiffusionImagingAppQtHelpCollection.qhc")
  add_custom_command(OUTPUT ${_plugin_qhcp_output}
                     COMMAND ${QT_COLLECTIONGENERATOR_EXECUTABLE} ${_plugin_qhcp_input} -o ${_plugin_qhcp_output}
                     DEPENDS ${_plugin_qhcp_input}
                     )

  list(APPEND CACHED_RESOURCE_FILES ${_plugin_qhcp_output})
  #set(FILE_DEPENDENCIES ${_plugin_qhcp_output})
endif()
