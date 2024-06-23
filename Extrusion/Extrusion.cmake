set(EXTRUSION_SHLIB_NAME Extrusion)				#Naziv prvog projekta u solution-u

file(GLOB EXTRUSION_SHLIB_SOURCES  ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)
file(GLOB EXTRUSION_SHLIB_INCS  ${CMAKE_CURRENT_LIST_DIR}/../common/include/*.h)
file(GLOB EXTRUSION_SHLIB_INC_TD  ${MY_INC}/td/*.h)
file(GLOB EXTRUSION_SHLIB_INC_GUI ${MY_INC}/gui/*.h)
file(GLOB EXTRUSION_SHLIB_INC_GUI_GL ${MY_INC}/gui/gl/*.h)

# add executable
add_library(${EXTRUSION_SHLIB_NAME} SHARED ${EXTRUSION_SHLIB_INCS} ${EXTRUSION_SHLIB_SOURCES} ${EXTRUSION_SHLIB_INC_TD} ${EXTRUSION_SHLIB_INC_GUI} ${EXTRUSION_SHLIB_INC_GUI_GL})

source_group("inc"            FILES ${EXTRUSION_SHLIB_INCS})
source_group("inc\\td"        FILES ${EXTRUSION_SHLIB_INC_TD})
source_group("inc\\gui"        FILES ${EXTRUSION_SHLIB_INC_GUI})
source_group("inc\\gl"        FILES ${EXTRUSION_SHLIB_INC_GUI_GL})
source_group("src"        FILES ${EXTRUSION_SHLIB_SOURCES})

target_link_libraries(${EXTRUSION_SHLIB_NAME} debug ${MU_LIB_DEBUG} optimized ${MU_LIB_RELEASE} 
									debug ${NATGUI_LIB_DEBUG} optimized ${NATGUI_LIB_RELEASE} 
								debug ${NATGL_LIB_DEBUG} optimized ${NATGL_LIB_RELEASE})

target_compile_definitions(${EXTRUSION_SHLIB_NAME} PUBLIC EXTRUSION_SHLIB_EXPORTS)

setApplicationIcon(${EXTRUSION_SHLIB_NAME} "${WORK_ROOT}/Common/WinIcons/apple.ico" "${CMAKE_CURRENT_LIST_DIR}/Common/Icons/apple128.png")

addOpenGLLibraries(${EXTRUSION_SHLIB_NAME})

#setTargetPropertiesForGUIApp(${EXTRUSION_SHLIB_NAME} ${EXTRUSION_SHLIB_PLIST})

#setIDEPropertiesForGUIExecutable(${EXTRUSION_SHLIB_NAME} ${CMAKE_CURRENT_LIST_DIR})

setPlatformDLLPath(${EXTRUSION_SHLIB_NAME})

set_target_properties(${EXTRUSION_SHLIB_NAME} PROPERTIES LINKER_LANGUAGE CXX)



