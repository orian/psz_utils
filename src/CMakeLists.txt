SET(BOOST_TRIVIAL_LOG_DEPS boost_date_time boost_filesystem boost_log boost_log_setup boost_system boost_thread pthread)
SET(HDRS_DIR ${PROJECT_BINARY_DIR}/include)

CONFIGURE_FILE(
  "version.h.in"
  "${HDRS_DIR}/version.h"
  )

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
#include_directories("${HDRS_DIR}")

SET(HDRS_CPY csv.h log_wrapper.h macros.h string.h string_split.h string_join.h time.h vector.h)
FILE(COPY ${HDRS_CPY} DESTINATION ${HDRS_DIR})
SET(HDRS_ALL ${HDRS_CPY} "${HDRS_DIR}/version.h")
INSTALL(FILES ${HDRS_ALL} DESTINATION include/psz/)

SET (LIB_INSTALL_DIR lib)

MACRO(DEF_PSZ_LIB short_libname cc_files)
  SET(libname "psz_${short_libname}")
  MESSAGE(STATUS lib_${libname})
  ADD_LIBRARY(${libname} SHARED ${cc_files})
  SET_TARGET_PROPERTIES(${libname} PROPERTIES COMPILE_FLAGS -DBOOST_LOG_DYN_LINK)
  ADD_LIBRARY(${libname}_static STATIC ${cc_files})

  TARGET_LINK_LIBRARIES(${libname} ${BOOST_TRIVIAL_LOG_DEPS})
  TARGET_LINK_LIBRARIES(${libname}_static ${BOOST_TRIVIAL_LOG_DEPS})

  # SOVERSION is a version number on the .so file you create and is
  # usually understood as an indicator for API compatibility (and ABI too).
  # So whenever you change the library API in a way that apps that linked
  # against an old version won't work against the new version increase the
  # SOVERSION. Its just an always increasing integer.
  # SET_TARGET_PROPERTIES(${libname} PROPERTIES VERSION ${VERSION} SOVERSION 2 )
  # SET_TARGET_PROPERTIES(${libname}_static PROPERTIES VERSION ${VERSION} SOVERSION 2 )

  INSTALL(TARGETS ${libname} DESTINATION ${LIB_INSTALL_DIR} )
  INSTALL(TARGETS ${libname}_static DESTINATION ${LIB_INSTALL_DIR} )
ENDMACRO(FANN_TARGET cc_file libname)

#
# csv
#
DEF_PSZ_LIB(csv csv.cc)

#
# string_split
#
DEF_PSZ_LIB(string_split string_split.cc)

#
# All string utils
#
DEF_PSZ_LIB("string" string_split.cc)

#install(TARGETS psz_csv psz_csv_static
#        RUNTIME DESTINATION bin
#        LIBRARY DESTINATION lib
#        ARCHIVE DESTINATION lib)

IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
  enable_testing()

  find_package(GTest REQUIRED)
  include_directories(${GTEST_INCLUDE_DIRS})

  add_executable(rbtree_test rbtree_test.cc)
  target_link_libraries(rbtree_test ${GTEST_BOTH_LIBRARIES} glog gmock pthread)
ENDIF()