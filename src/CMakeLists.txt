cmake_minimum_required(VERSION 3.10)

project(GodeyeNoteImgCrow)
set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/package")
#set(PACKVERSION 2.0.0.1)
######################################################################
#全局编译标志
######################################################################

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

######################################################################
#全局链接和包含
######################################################################

include_directories(${PROJECT_SOURCE_DIR})

link_directories(${CMAKE_SOURCE_DIR}/opencv3.4.5/lib)
link_directories(${CMAKE_SOURCE_DIR}/eureka/lib)


#################################################################L#####
#全局编译输出指定
######################################################################

#IF(CMAKE_BUILD_TYPE MATCHES Debug)
set(CMAKE_VERBOSE_MAKEFILE ON)
#ENDIF(CMAKE_BUILD_TYPE MATCHES Debug)

string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE)

#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/${BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/${BUILD_TYPE})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${BUILD_TYPE})


######################################################################
#子工程目录
######################################################################
add_subdirectory(base/third_party/dynamic_annotations)
add_subdirectory(base/third_party/symbolize)
add_subdirectory(base/third_party/xdg_mime)
add_subdirectory(base/third_party/libevent)
add_subdirectory(third_party)
add_subdirectory(base)
add_subdirectory(base/i18n)
add_subdirectory(url)
add_subdirectory(eureka)
add_subdirectory(grpc)
add_subdirectory(breakpad)
add_subdirectory(det_layout)
#add_subdirectory(det_ge_mma_ctpn)
#add_subdirectory(caffe_inference_base)
######################################################################
#编译对象
######################################################################


add_executable(GodeyeNoteImgCrow
            godeye_video_crow.cpp
            godeyesetting.cpp
            godeyesetting.h)

target_include_directories(GodeyeNoteImgCrow PRIVATE
    ${PROJECT_SOURCE_DIR}/breakpad/src
    ${PROJECT_SOURCE_DIR}/opencv3.4.5/include
        ${PROJECT_SOURCE_DIR}/eureka)

target_link_libraries(GodeyeNoteImgCrow
        breakpad_client
        base
        det_layout
#        det_ge_mma_ctpn
        curl
        eureka
        )
######################################################################
#程序发布设置
######################################################################

install(DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ DESTINATION bin)
install(DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/ DESTINATION lib)
install(DIRECTORY ai_model DESTINATION bin)
install(DIRECTORY det_ge_mma_ctpn/data DESTINATION bin)
install(DIRECTORY opencv3.4.5/lib DESTINATION .)
install(DIRECTORY eureka/lib DESTINATION .)
install(DIRECTORY config/ DESTINATION bin/config)
install(FILES scripts/requirments.sh DESTINATION .)
install(FILES scripts/pythonrequirements.txt DESTINATION .)
install(FILES scripts/install.sh DESTINATION .)
install(FILES scripts/ReadMe.txt DESTINATION .)
