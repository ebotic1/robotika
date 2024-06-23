set(movementTest test)

file(GLOB MOV_SOURCES  ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)
file(GLOB MOV_HEADERS  ${CMAKE_CURRENT_LIST_DIR}/src/*.h)
file(GLOB MOV_PLAYERSRC  ${CMAKE_CURRENT_LIST_DIR}/src/playerMovements/*.cpp)
file(GLOB MOV_PLAYERHEADER ${CMAKE_CURRENT_LIST_DIR}/src/playerMovements/*.h)

# add executable
add_executable(${movementTest} ${MOV_SOURCES} ${MOV_HEADERS} ${MOV_PLAYERSRC} ${MOV_PLAYERHEADER})

include(${CMAKE_CURRENT_LIST_DIR}/../Extrusion/Extrusion.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../viewMovement/viewMovement.cmake)

source_group("inc"            FILES ${MOV_SOURCES})
source_group("src"            FILES ${MOV_HEADERS})

include(${CMAKE_CURRENT_LIST_DIR}/../sceneClass/scene.cmake)


target_link_libraries(${movementTest} ${EXTRUSION_SHLIB_NAME} 
									  ${SCENECLASS_NAME}
									  ${viewMovementLib} 
									debug ${MU_LIB_DEBUG} optimized ${MU_LIB_RELEASE} 
									debug ${NATGUI_LIB_DEBUG} optimized ${NATGUI_LIB_RELEASE} 
								debug ${NATGL_LIB_DEBUG} optimized ${NATGL_LIB_RELEASE})

addOpenGLLibraries(${movementTest})


set(smece  ${CMAKE_CURRENT_LIST_DIR}/src/Info.plist)
setTargetPropertiesForGUIApp(${movementTest} ${smece})


setIDEPropertiesForGUIExecutable(${movementTest} ${CMAKE_CURRENT_LIST_DIR})

setPlatformDLLPath(${movementTest})

