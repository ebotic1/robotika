set(SCENECLASS_NAME objectSystem)				#Naziv prvog projekta u solution-u


file(GLOB SCENECLASS_INC ${CMAKE_CURRENT_LIST_DIR}/include/*.h)
file(GLOB SCENECLASS_SRC ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)


add_library(${SCENECLASS_NAME} STATIC ${SCENECLASS_INC} ${SCENECLASS_SRC} )


target_link_libraries(${SCENECLASS_NAME} debug ${MU_LIB_DEBUG} optimized ${MU_LIB_RELEASE} 
									debug ${NATGUI_LIB_DEBUG} optimized ${NATGUI_LIB_RELEASE} 
								debug ${NATGL_LIB_DEBUG} optimized ${NATGL_LIB_RELEASE})


addOpenGLLibraries(${SCENECLASS_NAME})


setPlatformDLLPath(${SCENECLASS_NAME})

