cmake_minimum_required(VERSION 2.8)
set(CMAKE_COLOR_MAKEFILE ON)

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})

set(CMAKE_MODULE_PATH "${SOURCE_DIR}/cmake/")

if(NOT EXISTS ${CMAKE_MODULE_PATH}/xml-editor-config.cmake)
  message(FATAL_ERROR "Cannot find: ${CMAKE_MODULE_PATH}/xml-editor-config.cmake")
endif()

include(${CMAKE_MODULE_PATH}/xml-editor-config.cmake)

# {{{ Sources
set(XML_EDITOR_SOURCES
  src/main.cc
  src/main-window.cc
  src/preferences.cc
  src/utils/lineedit.cc
  src/filter-lineedit.cc
  src/home.cc
  src/conf-model.cc
  src/categories-view.cc
  src/keys-view.cc
  )

# header (moc)
set(XML_EDITOR_QT_HEADER
  src/main-window.hh
  src/preferences.hh
  src/utils/lineedit.hh
  src/utils/lineedit_p.hh
  src/filter-lineedit.hh
  src/conf-model.hh
  src/home.hh
  src/categories-view.hh
  src/keys-view.hh
  )

# resources (rcs)
set(XML_EDITOR_RCS
  icons/xml-editor/xml.qrc
  icons/tango/tango.qrc
  )

# }}}

find_package(Qt4 REQUIRED)

include(${QT_USE_FILE})

include_directories(
  ${SOURCE_DIR}
  ${SOURCE_DIR}/src/
  ${BUILD_DIR}
  )

# process Qt special files
qt4_wrap_cpp(XML_EDITOR_MOCS ${XML_EDITOR_QT_HEADER})
qt4_add_resources(XML_EDITOR_RESSOURCES ${XML_EDITOR_RCS})

# generating executable
add_executable(${XML_EDITOR_APPLICATION_NAME} WIN32
  ${XML_EDITOR_SOURCES}
  ${XML_EDITOR_MOCS}
  ${XML_EDITOR_RESSOURCES}
  ${COMPILED_TRANSLATIONS}
  )

if(WIN32)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-enable-auto-import")
endif(WIN32)

list(APPEND LIBRARIES ${QT_LIBRARIES} ${OpenCV_LIBS})

target_link_libraries(${XML_EDITOR_APPLICATION_NAME} ${LIBRARIES})

# {{{ Internationalization
set (TRANSLATIONS
  lang/xml-editor_fr.ts)

set (COMPILED_TRANSLATIONS
  lang/xml-editor_fr.qm)

add_custom_target(
  translations_target
  COMMAND "${QT_LRELEASE_EXECUTABLE}" ${TRANSLATIONS}
  DEPENDS ${TRANSLATIONS}
  COMMENT "Updating translations"
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
#uncomment and run `make translations_target` to update translations ts files from sources
#warning, if uncommented, running make clean will remove ts files
#qt4_create_translation(COMPILED_TRANSLATIONS ${XML_EDITOR_SOURCES} ${TRANSLATIONS} )
# }}}

# {{{ Manpages
if(GENERATE_MANPAGES)

  set(XML_EDITOR_MAN_SRCS
    ${SOURCE_DIR}/manpages/xml-editor.1.txt
    )

  set(XML_EDITOR_MAN_LANGS default fr)

  foreach(lang ${XML_EDITOR_MAN_LANGS})

    foreach(txtfile ${XML_EDITOR_MAN_SRCS})
      # figure the base name of the file (ie "xml-editor.1")
      GET_FILENAME_COMPONENT(tmpname ${txtfile} NAME)
      string(REGEX REPLACE ".txt\$" "" basename ${tmpname})

      # figure the relative path of the file
      GET_FILENAME_COMPONENT(tmppath ${txtfile} PATH)
      string(REPLACE ${SOURCE_DIR}/ "" relpath ${tmppath})

      # figure the manpage section to install to from filename
      string(REGEX REPLACE "^.*\\.([0-9])$" "\\1" section ${basename})

      # construct the language specific versions of the basename and path
      if (lang STREQUAL default)
        set(basename2 ${basename})
        set(relpath2 ${relpath}/man${section})
      else()
        set(basename2 ${basename}.${lang})
        set(relpath2 ${relpath}/${lang}/man${section})
      endif()

      # create the build directory (if it does not exist)
      file(MAKE_DIRECTORY ${BUILD_DIR}/${relpath2})

      # set the final filenames
      set(txtfile ${SOURCE_DIR}/${relpath}/${basename2}.txt)
      set(xmlfile ${BUILD_DIR}/${relpath2}/${basename}.xml)
      set(gzfile  ${BUILD_DIR}/${relpath2}/${basename}.gz)
      set(manfile ${BUILD_DIR}/${relpath2}/${basename})

      add_custom_command(
        COMMAND ${ASCIIDOC_EXECUTABLE} -d manpage -b docbook -o ${xmlfile} - < ${txtfile}
        WORKING_DIRECTORY ${BUILD_DIR}/${relpath2}
        OUTPUT  ${xmlfile}
        DEPENDS ${txtfile}
        VERBATIM)
      add_custom_command(
        COMMAND ${XMLTO_EXECUTABLE} --skip-validation man ${xmlfile}
        OUTPUT  ${manfile}
        WORKING_DIRECTORY ${BUILD_DIR}/${relpath2}
        DEPENDS ${xmlfile})

      if(COMPRESS_MANPAGES)
        add_custom_command(
          COMMAND ${GZIP_EXECUTABLE} < ${manfile} > ${gzfile}
          OUTPUT  ${gzfile}
          WORKING_DIRECTORY ${BUILD_DIR}/${relpath2}
          DEPENDS ${manfile}
          VERBATIM)

        set(MAN_FILES ${MAN_FILES} ${gzfile})
      else()
        set(MAN_FILES ${MAN_FILES} ${manfile})
      endif()
    endforeach()

  endforeach()

  add_custom_target(man ALL DEPENDS ${MAN_FILES})
endif()
# }}}

# {{{ Installation
install(TARGETS ${XML_EDITOR_APPLICATION_NAME} RUNTIME DESTINATION bin)
if(GENERATE_MANPAGES)
  if(COMPRESS_MANPAGES)
    set(regex "\\.(xml|txt|[0-9])$")
  else()
    set(regex "\\.(xml|txt|gz)$")
  endif()
  install(DIRECTORY ${BUILD_DIR}/${relpath}/ DESTINATION ${XML_EDITOR_MAN_PATH}
    REGEX ${regex} EXCLUDE)
endif()
install(FILES ${XML_EDITOR_APPLICATION_NAME}.desktop DESTINATION share/applications)
install(FILES icons/xml-editor/src/xml-editor.svg DESTINATION share/icons)
install(FILES ${COMPILED_TRANSLATIONS} DESTINATION ${XML_EDITOR_DATA_PATH}/lang)
# }}}
