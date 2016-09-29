include (${SWIG_USE_FILE})
include(LibAddMacros)

message(STATUS "Include Binding swig_node")

add_headers (HDR_FILES)
add_cppheaders (HDR_FILES)

# for node.js@0.12 from the Debian package
#include_directories ("/usr/include/nodejs/src" "/usr/include/nodejs/deps/uv/include")
include_directories ("/usr/include/node")
include_directories (${CMAKE_CURRENT_SOURCE_DIR})

set (CMAKE_SWIG_OUTDIR "${CMAKE_CURRENT_BINARY_DIR}")
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SWIG_OUTDIR}")
set (CMAKE_SWIG_FLAGS "-O")

set_source_files_properties (kdb.i PROPERTIES CPLUSPLUS ON)
set_source_files_properties (kdb.i PROPERTIES SWIG_FLAGS "-node")
swig_add_module (swig-node javascript kdb.i)
swig_link_libraries (swig-node elektra-core elektra-kdb)
set_target_properties (swig-node PROPERTIES OUTPUT_NAME kdb)
set_target_properties (${SWIG_MODULE_swig-node_REAL_NAME} PROPERTIES PREFIX "")

set_source_files_properties (${swig_generated_file_fullname} PROPERTIES COMPILE_FLAGS "${SWIG_COMPILE_FLAGS} -DSWIG_TYPE_TABLE=kdb")

install (
  TARGETS swig-node
  LIBRARY DESTINATION "/testlib"
)

include_directories (${CMAKE_CURRENT_SOURCE_DIR})
include_directories (${CMAKE_SOURCE_DIR}/src/swig/)
