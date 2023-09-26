cmake_minimum_required(VERSION 3.7 FATAL_ERROR)
project(HPTT C CXX)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

string(TOLOWER ${CMAKE_SYSTEM_PROCESSOR} cm_sys_processor)

set(HPTT_DEFAULT_IBM OFF)
if(cm_sys_processor STREQUAL "ppc64le")
  set(HPTT_DEFAULT_IBM ON)
endif()

set(HPTT_DEFAULT_ARM OFF)
if(cm_sys_processor STREQUAL "arm64")
  set(HPTT_DEFAULT_ARM ON)
endif()

set(HPTT_DEFAULT_AVX OFF)
if(NOT cm_sys_processor STREQUAL "arm64")
  set(HPTT_DEFAULT_AVX ON)
endif()

option(HPTT_ENABLE_IBM "Enable IBM processor" ${HPTT_DEFAULT_IBM})
option(HPTT_ENABLE_ARM "Enable build for ARM processor" ${HPTT_DEFAULT_ARM})
option(HPTT_ENABLE_AVX "Enable build for x86-64 processor" ${HPTT_DEFAULT_AVX})

message(STATUS "HPTT_ENABLE_IBM ${HPTT_ENABLE_IBM}")
message(STATUS "HPTT_ENABLE_ARM ${HPTT_ENABLE_ARM}")
message(STATUS "HPTT_ENABLE_AVX ${HPTT_ENABLE_AVX}")

find_package(OpenMP MODULE COMPONENTS CXX)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
  set(HPTT_CXX_FLAGS ${HPTT_CXX_FLAGS} -qopenmp -xhost)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  if(HPTT_ENABLE_IBM)
    set(HPTT_CXX_FLAGS ${HPTT_CXX_FLAGS} -fopenmp)
  else()
    set(HPTT_CXX_FLAGS ${HPTT_CXX_FLAGS} -fopenmp -march=native -mtune=native)
  endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(HPTT_CXX_FLAGS ${HPTT_CXX_FLAGS} -fopenmp -march=native)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "PGI")
  set(HPTT_CXX_FLAGS ${HPTT_CXX_FLAGS} -silent -w -Mnovect)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "XL")
  set(HPTT_CXX_FLAGS ${HPTT_CXX_FLAGS} -qsmp=omp)
endif()

if(HPTT_ENABLE_AVX)
  set(HPTT_CXX_FLAGS ${HPTT_CXX_FLAGS} -mavx -DHPTT_ARCH_AVX)
elseif(HPTT_ENABLE_ARM)
  set(HPTT_CXX_FLAGS ${HPTT_CXX_FLAGS} -mfpu=neon -DHPTT_ARCH_ARM)
elseif(HPTT_ENABLE_IBM)
  set(HPTT_CXX_FLAGS ${HPTT_CXX_FLAGS} -mtune=native -DHPTT_ARCH_IBM -maltivec
                     -mabi=altivec)
endif()

set(HPTT_SRCS src/hptt.cpp src/plan.cpp src/transpose.cpp src/utils.cpp)

add_library(hptt OBJECT ${HPTT_SRCS})
target_include_directories(
  hptt PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
              $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_compile_options(hptt PRIVATE ${HPTT_CXX_FLAGS})
target_link_libraries(hptt PUBLIC OpenMP::OpenMP_CXX)

add_library(hptt::hptt ALIAS hptt)

install(
  TARGETS hptt
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

set(HPTT_INCLUDES
    include/hptt/compute_node.h
    include/hptt/hptt_types.h
    include/hptt/hptt.h
    include/hptt/macros.h
    include/hptt/plan.h
    include/hptt/utils.h
    include/hptt/transpose.h)

install(FILES ${HPTT_INCLUDES} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
