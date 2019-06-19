cmake_minimum_required (VERSION 3.1.0)

project (server)

set(CMAKE_INCLUDE_CURRENT_DIR ON) 
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++0x")

file(GLOB SRCS "*.cpp") 
file(GLOB HEADERS "*.h")

#编译为可执行文件
add_executable (${PROJECT_NAME} ${SRCS} ${HEADERS})

include_directories (/usr/local/mycpp/include)
target_link_libraries (${PROJECT_NAME} rt uuid pthread ssl odbc jsoncpp /usr/local/mycpp/libmycpp.so )

# Install section
#INSTALL (TARGETS ${PROJECT_NAME} RUNTIME DESTINATION /usr/local/${PROJECT_NAME} ARCHIVE DESTINATION /usr/local/${PROJECT_NAME})

# Packaging section
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VERSION "0.0.1")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_NAME}")
set(CPACK_PACKAGE_DESCRIPTION "mycpp test")

set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "yueyemingming@163.com")
set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS 1)
#set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_CURRENT_SOURCE_DIR}/debian/postinst;${CMAKE_CURRENT_SOURCE_DIR}/debian/prerm;${CMAKE_CURRENT_SOURCE_DIR}/debian/conffiles")

include(CPack)

