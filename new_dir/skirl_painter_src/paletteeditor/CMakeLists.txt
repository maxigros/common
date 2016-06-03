cmake_minimum_required( VERSION 2.6 )
PROJECT(PaletteEditor)

SET (EXE_NAME PaletteEditor)

#Uncomment the following to set debug enabled
if (NOT MSVC)
    SET ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -g " )
    SET ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g" )
endif (NOT MSVC)

# Windows
if(${CMAKE_SYSTEM_NAME} STREQUAL Windows)
    MESSAGE(STATUS "OS: Windows")

    SET(LIB_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/lib)
    MESSAGE(STATUS "Library install dir: ${LIB_INSTALL_DIR}")

    SET(INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/include)
    MESSAGE(STATUS "Headers install dir: ${INCLUDE_INSTALL_DIR}")

    #MESSAGE( "WARNING: cmake may (and most definitely will) fail on Windows" )
endif(${CMAKE_SYSTEM_NAME} STREQUAL Windows)

# Linux
if (${CMAKE_SYSTEM_NAME} STREQUAL Linux)

    MESSAGE(STATUS "OS: Linux")

    # Linux libdir
    IF(CMAKE_SIZEOF_VOID_P EQUAL 4)
        SET(LIB_SUFFIX "")
    ELSE(CMAKE_SIZEOF_VOID_P EQUAL 4)
        SET(LIB_SUFFIX 64)
    ENDIF(CMAKE_SIZEOF_VOID_P EQUAL 4)
    #SET(CMAKE_INSTALL_PREFIX /usr)
    SET(LIB_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX})
    MESSAGE(STATUS "Library install dir: ${LIB_INSTALL_DIR}")

    SET(INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/include)
    MESSAGE(STATUS "Headers install dir: ${INCLUDE_INSTALL_DIR}")
endif (${CMAKE_SYSTEM_NAME} STREQUAL Linux)

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}/.. )

LINK_DIRECTORIES(${LIB_INSTALL_DIR})
INCLUDE_DIRECTORIES(${INCLUDE_INSTALL_DIR})

find_package( Qt4 COMPONENTS QtCore QtGui REQUIRED )

#MESSAGE("Qt version: ${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}")

include( ${QT_USE_FILE} )

### SVN-related stuff ###
#FIND_PACKAGE(Subversion)
#IF(Subversion_FOUND)
#    Subversion_WC_INFO(${PROJECT_SOURCE_DIR} cline)
#    MESSAGE("Revision of last commit is ${cline_WC_LAST_CHANGED_REV}")
#
#    MESSAGE("Date of last commit ${cline_WC_LAST_CHANGED_DATE}")
#
#    SET (CP9BCLIENT_REVISION ${cline_WC_LAST_CHANGED_REV})
#    SET (CP9BCLIENT_DATE ${cline_WC_LAST_CHANGED_DATE})
#ELSE (Subversion_FOUND)
#    SET (CP9BCLIENT_REVISION 0)
#    SET (CP9BCLIENT_DATE "unknown")
#ENDIF(Subversion_FOUND)
#CONFIGURE_FILE(revision.h.in ${PROJECT_SOURCE_DIR}/revision.h)

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

set( MOC_HEADERS paletteeditorwindow.h
                 paletteviewer.h
                 ../qcolorpicker.h
                 )

set( HEADERS ../colorpalette.h
             )

set( SOURCES main.cpp
             paletteeditorwindow.cpp
             paletteviewer.cpp
             ../qcolorpicker.cpp
             )

set( FORMS   paletteeditorwindow.ui
             ColorEditor.ui
             colorinfo.ui
             )

qt4_wrap_cpp( MOC_SOURCES ${MOC_HEADERS} )

qt4_wrap_ui( UI_HEADERS ${FORMS} )

# Windows
if(${CMAKE_SYSTEM_NAME} STREQUAL Windows)
add_executable( ${EXE_NAME} WIN32
                            ${SOURCES}
                            ${HEADERS}
                            ${MOC_SOURCES}
                            ${UI_HEADERS} )
#                revision.h )
    if (MSVC)
        target_link_libraries( ${EXE_NAME} ${QT_QTMAIN_LIBRARY})
    endif (MSVC)
endif(${CMAKE_SYSTEM_NAME} STREQUAL Windows)

# Linux
if (${CMAKE_SYSTEM_NAME} STREQUAL Linux)
add_executable( ${EXE_NAME}
                            ${SOURCES}
                            ${HEADERS}
                            ${MOC_SOURCES}
                            ${UI_HEADERS} )
#                revision.h )
endif (${CMAKE_SYSTEM_NAME} STREQUAL Linux)


target_link_libraries( ${EXE_NAME}  ${QT_LIBRARIES} )

# Windows
if(${CMAKE_SYSTEM_NAME} STREQUAL Windows)
INSTALL(TARGETS ${EXE_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/)
endif(${CMAKE_SYSTEM_NAME} STREQUAL Windows)

# Linux
if (${CMAKE_SYSTEM_NAME} STREQUAL Linux)
INSTALL(TARGETS ${EXE_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
endif (${CMAKE_SYSTEM_NAME} STREQUAL Linux)

CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET(${EXE_NAME}_uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
