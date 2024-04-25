macro(add_adt_executable name)
  # add executable
  add_executable(${name} ${ARGN})

  # add install target
  set_target_properties(${name} PROPERTIES OUTPUT_NAME ${name})
  install(TARGETS ${name} DESTINATION bin
      PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

  # copy binary
  add_custom_command(TARGET ${name} POST_BUILD COMMAND
      ${CMAKE_COMMAND} ARGS -E copy
        $<TARGET_FILE:${name}>
        ${CMAKE_BINARY_DIR}/bin/$<TARGET_FILE_NAME:${name}>
      COMMENT "Copy ${name} to ${CMAKE_BINARY_DIR}/bin")

endmacro(add_adt_executable)

find_package(Qt5 COMPONENTS Widgets Core Gui DBus Xml LinguistTools REQUIRED)

configure_file(version.h.in version.h)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(HEADERS
    adtapp.h
    adtexecutor.h
    adtservicechecker.h
    adttoolobjecthelper.h

    constants.h

    basecontroller.h
    mainwindowcontrollerimpl.h
    clcontroller.h

    treeproxymodel.h
    categoryproxymodel.h

    mainwindow/mainwindow.h
    mainwindow/maintoolswidget.h
    mainwindow/maintestswidget.h
    mainwindow/statuscommonwidget.h
    mainwindow/doubleclickablelabel.h
    mainwindow/detailsdialog.h
    mainwindow/clickablehighlightlabel.h
    mainwindow/serviceunregisteredwidget.h

    adtbuilderstrategies/adtmodelbuilder.h
    adtbuilderstrategies/adtmodelbuilderstrategyinterface.h
    adtbuilderstrategies/adtmodelbuilderstrategydbusinfodesktop.h

    interfaces/mainwindowcontrollerinterface.h
    interfaces/toolswidgetinterface.h
    interfaces/testswidgetinterface.h
    interfaces/mainwindowinterface.h
    interfaces/appcontrollerinterface.h

    parser/commandlineparser.h
    parser/commandlineoptions.h

    settings/adtsettingsinterface.h
    settings/adtsettingsimpl.h
)

set(SOURCES
    main.cpp

    adtapp.cpp
    adtexecutor.cpp
    adtservicechecker.cpp
    adttoolobjecthelper.cpp

    basecontroller.cpp
    mainwindowcontrollerimpl.cpp
    clcontroller.cpp

    treeproxymodel.cpp
    categoryproxymodel.cpp

    mainwindow/mainwindow.cpp
    mainwindow/maintoolswidget.cpp
    mainwindow/maintestswidget.cpp
    mainwindow/statuscommonwidget.cpp
    mainwindow/doubleclickablelabel.cpp
    mainwindow/detailsdialog.cpp
    mainwindow/clickablehighlightlabel.cpp
    mainwindow/serviceunregisteredwidget.cpp

    adtbuilderstrategies/adtmodelbuilder.cpp
    adtbuilderstrategies/adtmodelbuilderstrategydbusinfodesktop.cpp

    interfaces/mainwindowcontrollerinterface.cpp
    interfaces/toolswidgetinterface.cpp
    interfaces/testswidgetinterface.cpp
    interfaces/mainwindowinterface.cpp
    interfaces/appcontrollerinterface.cpp
    interfacedata.h

    parser/commandlineparser.cpp

    settings/adtsettingsimpl.cpp
)

set(UI_FORMS
    mainwindow/mainwindow.ui
    mainwindow/maintoolswidget.ui
    mainwindow/maintestswidget.ui

    mainwindow/statuscommonwidget.ui
    mainwindow/detailsdialog.ui

    mainwindow/serviceunregistered.ui
)
set(RESOURCES
    resources.qrc
)
set(TS_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/app_ru.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/app_en.ts
)

add_translation(QM_FILES ${TS_FILES})

qt5_wrap_ui(UI_SOURCES ${UI_FORMS})

add_translation_resource(RESOURCES_SRC "adt" ${QM_FILES})

qt5_add_resources(RCC_SOURCES ${RESOURCES} ${RESOURCES_SRC})

add_adt_executable(adt ${SOURCES} ${HEADERS} ${UI_SOURCES} ${RCC_SOURCES})

install (FILES ${CMAKE_SOURCE_DIR}/setup/adt.desktop DESTINATION share/applications)
install (FILES ${CMAKE_SOURCE_DIR}/setup/adt.svg DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps)
install (FILES ${CMAKE_SOURCE_DIR}/setup/adt.backend DESTINATION /usr/share/alterator/backends)

target_link_libraries(adt Qt5::Widgets Qt5::Core Qt5::DBus Qt5::Xml Qt5::Gui adtcore)
