find_package(Qt5 REQUIRED COMPONENTS Widgets Core DBus)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set (HEADERS
    adtexecutable.h

    adtjsonconverter.h
    alteratorexecutordbusinterface.h

    treemodelbuilderinterface.h
    treemodel.h
    treeitem.h
    treemodelbulderfromexecutable.h

    adtdesktopfileparser.h

)

set (SOURCES
    adtexecutable.cpp

    adtjsonconverter.cpp

    treemodelbuilderinterface.cpp
    treemodel.cpp
    treeitem.cpp
    treemodelbulderfromexecutable.cpp

    adtdesktopfileparser.cpp
)

ADD_LIBRARY(adtcore STATIC ${SOURCES} ${HEADERS})
target_link_libraries(adtcore PRIVATE Qt5::Core Qt5::DBus Qt5::Widgets)
