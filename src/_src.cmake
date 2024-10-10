# Configure header
configure_file(
        "${CMAKE_CURRENT_LIST_DIR}/Version.h.in"
        "${CMAKE_CURRENT_LIST_DIR}/Version.h")

# Create executable
add_executable(${PROJECT_NAME} WIN32
        "${CMAKE_CURRENT_SOURCE_DIR}/res/res.qrc"
        "${CMAKE_CURRENT_LIST_DIR}/Main.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Version.h")

cmdseqedit_configure_target(${PROJECT_NAME})
target_link_libraries(
        ${PROJECT_NAME} PUBLIC
        Qt::Core
        Qt::Gui
        Qt::Widgets
        sourcepp::toolpp)

target_include_directories(
        ${PROJECT_NAME} PUBLIC
        "${QT_INCLUDE}"
        "${QT_INCLUDE}/QtCore"
        "${QT_INCLUDE}/QtGui"
        "${QT_INCLUDE}/QtWidgets")

# Copy these next to the executable
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/LICENSE" "${CMAKE_BINARY_DIR}/LICENSE" COPYONLY)
# Don't copy the .nonportable file, we're debugging in standalone mode

# Copy these so the user doesn't have to
if(WIN32)
    configure_file("${QT_BASEDIR}/bin/Qt6Core${QT_LIB_SUFFIX}.dll"    "${CMAKE_BINARY_DIR}/Qt6Core${QT_LIB_SUFFIX}.dll"    COPYONLY)
    configure_file("${QT_BASEDIR}/bin/Qt6Gui${QT_LIB_SUFFIX}.dll"     "${CMAKE_BINARY_DIR}/Qt6Gui${QT_LIB_SUFFIX}.dll"     COPYONLY)
    configure_file("${QT_BASEDIR}/bin/Qt6Widgets${QT_LIB_SUFFIX}.dll" "${CMAKE_BINARY_DIR}/Qt6Widgets${QT_LIB_SUFFIX}.dll" COPYONLY)

    configure_file("${QT_BASEDIR}/plugins/platforms/qwindows${QT_LIB_SUFFIX}.dll" "${CMAKE_BINARY_DIR}/platforms/qwindows${QT_LIB_SUFFIX}.dll" COPYONLY)

    configure_file("${QT_BASEDIR}/plugins/styles/qwindowsvistastyle${QT_LIB_SUFFIX}.dll" "${CMAKE_BINARY_DIR}/styles/qwindowsvistastyle${QT_LIB_SUFFIX}.dll" COPYONLY)
elseif(UNIX AND DEFINED QT_BASEDIR)
    configure_file("${QT_BASEDIR}/lib/libQt6Core.so.6"    "${CMAKE_BINARY_DIR}/libQt6Core.so.6"    COPYONLY)
    configure_file("${QT_BASEDIR}/lib/libQt6Gui.so.6"     "${CMAKE_BINARY_DIR}/libQt6Gui.so.6"     COPYONLY)
    configure_file("${QT_BASEDIR}/lib/libQt6Widgets.so.6" "${CMAKE_BINARY_DIR}/libQt6Widgets.so.6" COPYONLY)

    # Required by plugins
    configure_file("${QT_BASEDIR}/lib/libicudata.so.56"                         "${CMAKE_BINARY_DIR}/libicudata.so.56"                         COPYONLY)
    configure_file("${QT_BASEDIR}/lib/libicui18n.so.56"                         "${CMAKE_BINARY_DIR}/libicui18n.so.56"                         COPYONLY)
    configure_file("${QT_BASEDIR}/lib/libicuuc.so.56"                           "${CMAKE_BINARY_DIR}/libicuuc.so.56"                           COPYONLY)
    configure_file("${QT_BASEDIR}/lib/libQt6DBus.so.6"                          "${CMAKE_BINARY_DIR}/libQt6DBus.so.6"                          COPYONLY)
    configure_file("${QT_BASEDIR}/lib/libQt6EglFSDeviceIntegration.so.6"        "${CMAKE_BINARY_DIR}/libQt6EglFSDeviceIntegration.so.6"        COPYONLY)
    configure_file("${QT_BASEDIR}/lib/libQt6EglFsKmsSupport.so.6"               "${CMAKE_BINARY_DIR}/libQt6EglFsKmsSupport.so.6"               COPYONLY)
    configure_file("${QT_BASEDIR}/lib/libQt6WaylandClient.so.6"                 "${CMAKE_BINARY_DIR}/libQt6WaylandClient.so.6"                 COPYONLY)
    configure_file("${QT_BASEDIR}/lib/libQt6WaylandEglClientHwIntegration.so.6" "${CMAKE_BINARY_DIR}/libQt6WaylandEglClientHwIntegration.so.6" COPYONLY)
    configure_file("${QT_BASEDIR}/lib/libQt6WlShellIntegration.so.6"            "${CMAKE_BINARY_DIR}/libQt6WlShellIntegration.so.6"            COPYONLY)
    configure_file("${QT_BASEDIR}/lib/libQt6XcbQpa.so.6"                        "${CMAKE_BINARY_DIR}/libQt6XcbQpa.so.6"                        COPYONLY)

    # Copy all this stuff wholesale, who knows if we need it now or later
    file(GLOB ${PROJECT_NAME}_QT_PLUGINS_EGLDEVICEINTEGRATIONS               "${QT_BASEDIR}/plugins/egldeviceintegrations/*.so*")
    file(GLOB ${PROJECT_NAME}_QT_PLUGINS_PLATFORMINPUTCONTEXTS               "${QT_BASEDIR}/plugins/platforminputcontexts/*.so*")
    file(GLOB ${PROJECT_NAME}_QT_PLUGINS_PLATFORMS                           "${QT_BASEDIR}/plugins/platforms/*.so*")
    file(GLOB ${PROJECT_NAME}_QT_PLUGINS_PLATFORMTHEMES                      "${QT_BASEDIR}/plugins/platformthemes/*.so*")
    file(GLOB ${PROJECT_NAME}_QT_PLUGINS_WAYLANDDECORATIONCLIENT             "${QT_BASEDIR}/plugins/wayland-decoration-client/*.so*")
    file(GLOB ${PROJECT_NAME}_QT_PLUGINS_WAYLANDGRAPHICSINTEGRATIONCLIENT    "${QT_BASEDIR}/plugins/wayland-graphics-integration-client/*.so*")
    file(GLOB ${PROJECT_NAME}_QT_PLUGINS_WAYLANDSHELLINTEGRATION             "${QT_BASEDIR}/plugins/wayland-shell-integration/*.so*")
    file(GLOB ${PROJECT_NAME}_QT_PLUGINS_XCBGLINTEGRATIONS                   "${QT_BASEDIR}/plugins/xcbglintegrations/*.so*")
    file(COPY ${${PROJECT_NAME}_QT_PLUGINS_EGLDEVICEINTEGRATIONS}            DESTINATION "${CMAKE_BINARY_DIR}/egldeviceintegrations")
    file(COPY ${${PROJECT_NAME}_QT_PLUGINS_PLATFORMINPUTCONTEXTS}            DESTINATION "${CMAKE_BINARY_DIR}/platforminputcontexts")
    file(COPY ${${PROJECT_NAME}_QT_PLUGINS_PLATFORMS}                        DESTINATION "${CMAKE_BINARY_DIR}/platforms")
    file(COPY ${${PROJECT_NAME}_QT_PLUGINS_PLATFORMTHEMES}                   DESTINATION "${CMAKE_BINARY_DIR}/platformthemes")
    file(COPY ${${PROJECT_NAME}_QT_PLUGINS_WAYLANDDECORATIONCLIENT}          DESTINATION "${CMAKE_BINARY_DIR}/wayland-decoration-client")
    file(COPY ${${PROJECT_NAME}_QT_PLUGINS_WAYLANDGRAPHICSINTEGRATIONCLIENT} DESTINATION "${CMAKE_BINARY_DIR}/wayland-graphics-integration-client")
    file(COPY ${${PROJECT_NAME}_QT_PLUGINS_WAYLANDSHELLINTEGRATION}          DESTINATION "${CMAKE_BINARY_DIR}/wayland-shell-integration")
    file(COPY ${${PROJECT_NAME}_QT_PLUGINS_XCBGLINTEGRATIONS}                DESTINATION "${CMAKE_BINARY_DIR}/xcbglintegrations")
endif()
