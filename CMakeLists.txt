cmake_minimum_required (VERSION 3.1.0)

project (mycpp)

set(CMAKE_INCLUDE_CURRENT_DIR ON) 
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++0x")

file(GLOB SRCS "*.cpp") 
file(GLOB HEADERS "*.h")

#编译为动态库
add_library (mycpp SHARED ${SRCS})
set_target_properties(mycpp PROPERTIES VERSION 1.0 SOVERSION 1)

#编译为静态库
add_library (mycpp_static STATIC ${SRCS})
set_target_properties(mycpp_static PROPERTIES OUTPUT_NAME "mycpp")

# Install section
INSTALL (TARGETS mycpp mycpp_static LIBRARY DESTINATION /usr/local/${PROJECT_NAME} ARCHIVE DESTINATION /usr/local/${PROJECT_NAME})
INSTALL (FILES ${HEADERS} DESTINATION /usr/local/${PROJECT_NAME}/include)

#find_package(Qt5Core)



# Packaging section
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VERSION "0.0.1")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_NAME}")
set(CPACK_PACKAGE_DESCRIPTION "database admin")

set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "lynx@lynxattractions.com")
set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS 1)
#set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_CURRENT_SOURCE_DIR}/debian/postinst;${CMAKE_CURRENT_SOURCE_DIR}/debian/prerm;${CMAKE_CURRENT_SOURCE_DIR}/debian/conffiles")

include(CPack)

