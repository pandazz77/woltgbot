project(woltgbot C)

set(SRCS
    ${CMAKE_SOURCE_DIR}/main.c
    ${CMAKE_SOURCE_DIR}/wol.c ${CMAKE_SOURCE_DIR}/wol.h
)

add_subdirectory(${CMAKE_SOURCE_DIR}/telebot)
add_executable(${CMAKE_PROJECT_NAME} ${SRCS})
target_link_libraries(${CMAKE_PROJECT_NAME} telebot)