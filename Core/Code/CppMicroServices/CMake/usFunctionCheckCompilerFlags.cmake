#
# Helper macro allowing to check if the given flags are supported 
# by the underlying build tool
#
# If the flag(s) is/are supported, they will be appended to the string identified by RESULT_VAR
#
# Usage:
#   MITKFunctionCheckCompilerFlags(FLAGS_TO_CHECK VALID_FLAGS_VAR)
#
# Example:
#
#   set(myflags)
#   usFunctionCheckCompilerFlags("-fprofile-arcs" myflags)
#   message(1-myflags:${myflags})
#   usFunctionCheckCompilerFlags("-fauto-bugfix" myflags)
#   message(2-myflags:${myflags})
#   usFunctionCheckCompilerFlags("-Wall" myflags)
#   message(1-myflags:${myflags})
#
#   The output will be:
#    1-myflags: -fprofile-arcs
#    2-myflags: -fprofile-arcs
#    3-myflags: -fprofile-arcs -Wall

INCLUDE(TestCXXAcceptsFlag)

FUNCTION(usFunctionCheckCompilerFlags CXX_FLAG_TO_TEST RESULT_VAR)
  
  IF(CXX_FLAG_TO_TEST STREQUAL "")
    MESSAGE(FATAL_ERROR "CXX_FLAG_TO_TEST shouldn't be empty")
  ENDIF()
  
  # Internally, the macro CMAKE_CXX_ACCEPTS_FLAG calls TRY_COMPILE. To avoid
  # the cost of compiling the test each time the project is configured, the variable set by
  # the macro is added to the cache so that following invocation of the macro with
  # the same variable name skip the compilation step.
  # For that same reason, ctkFunctionCheckCompilerFlags function appends a unique suffix to
  # the HAS_FLAG variable. This suffix is created using a 'clean version' of the flag to test.
  STRING(REGEX REPLACE "-\\s\\$\\+\\*\\{\\}\\(\\)\\#" "" suffix ${CXX_FLAG_TO_TEST})
  CHECK_CXX_ACCEPTS_FLAG(${CXX_FLAG_TO_TEST} HAS_FLAG_${suffix})

  IF(HAS_FLAG_${suffix})
    SET(${RESULT_VAR} "${${RESULT_VAR}} ${CXX_FLAG_TO_TEST}" PARENT_SCOPE)
  ENDIF()

ENDFUNCTION()

