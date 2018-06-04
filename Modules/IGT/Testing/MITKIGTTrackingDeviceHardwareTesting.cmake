option(MITK_TRACKING_DEVICES_HARDWARE_TESTS_ENABLED "Enable support for testing tracking device hardware (this hardware must be connected to the system)" OFF)
mark_as_advanced(MITK_TRACKING_DEVICES_HARDWARE_TESTS_ENABLED)

if(MITK_TRACKING_DEVICES_HARDWARE_TESTS_ENABLED)
  option(MITK_NDI_AURORA_CONNECTED "Turn on if an Aurora system is connected and should be tested." OFF)
  option(MITK_NDI_POLARIS_CONNECTED "Turn on if a Polaris system is connected and should be tested." OFF)
  option(MITK_CLARON_MICRONTRACKER_CONNECTED "Turn on if a MicronTracker system is connected and should be tested." OFF)
  mark_as_advanced(MITK_NDI_AURORA_CONNECTED MITK_NDI_POLARIS_CONNECTED MITK_CLARON_MICRONTRACKER_CONNECTED MITK_CLARON_MICRONTRACKER_CONNECTED)
endif(MITK_TRACKING_DEVICES_HARDWARE_TESTS_ENABLED)

if(MITK_NDI_AURORA_CONNECTED)
   set(MITK_NDI_AURORA_COM_PORT "1" CACHE STRING "Serial port where the system is connected.")
endif(MITK_NDI_AURORA_CONNECTED)

if(MITK_NDI_POLARIS_CONNECTED)
   set(MITK_NDI_POLARIS_COM_PORT "1" CACHE STRING "Serial port where the system is connected.")
endif(MITK_NDI_POLARIS_CONNECTED)