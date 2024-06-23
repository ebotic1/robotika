set(viewMovementLib movementLib)

file(GLOB MOVLIB_SOURCES  ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)
file(GLOB MOVLIB_INCLUDES  ${CMAKE_CURRENT_LIST_DIR}/include/*.h)
file(GLOB MOVLIB_HEADERS  ${CMAKE_CURRENT_LIST_DIR}/src/*.h)


add_library(${viewMovementLib} STATIC ${MOVLIB_SOURCES} ${MOVLIB_INCLUDES} ${MOVLIB_HEADERS})


target_link_libraries(${viewMovementLib} debug ${MU_LIB_DEBUG} optimized ${MU_LIB_RELEASE} 
									debug ${NATGUI_LIB_DEBUG} optimized ${NATGUI_LIB_RELEASE} 
								debug ${NATGL_LIB_DEBUG} optimized ${NATGL_LIB_RELEASE})


