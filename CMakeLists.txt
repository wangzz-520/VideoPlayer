cmake_minimum_required (VERSION 3.10)

project(VideoPlayer)

set(QT_PATH "E:/Qt/Qt5.9.0/5.9/msvc2015_64" CACHE PATH "qt5 cmake dir")

set(CMAKE_PREFIX_PATH ${QT_PATH})

# set output dir
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/x64)

#不需要寻找所有的组件，只查找需要的即可
find_package (Qt5Core)
find_package (Qt5Widgets)
find_package (Qt5Gui)
find_package (Qt5Core)

#添加cmake build目录 以保证ui_xx.h再#includ的时候被找到
include_directories(${CMAKE_BINARY_DIR})
#添加外部库依赖目录
link_directories(${QT5_LIB_DIR})
#添加每个组件的头文件目录，方便引用
include_directories(
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
    ${Qt5OpenGL_INCLUDE_DIRS}
 )
#需要用到的qt库
list(APPEND QT_LIB
     ${Qt5Core_LIBRARIES}
     ${Qt5Widgets_LIBRARIES}
     ${Qt5OpenGL_LIBRARIES}
)

# 包含头文件路径
include_directories(${PROJECT_SOURCE_DIR}/3rd/ffmpeg/include)
# 包含静态库路径
link_directories(${PROJECT_SOURCE_DIR}/3rd/ffmpeg/lib)

SET(FFMPEG_LIB swscale.lib 
                swresample.lib
                avutil.lib
                avformat.lib
                avfilter.lib
                avdevice.lib
                avcodec.lib)

SET(SOURCES main.cpp MainWidget.h 
					MainWidget.cpp
					MainWidget.ui
					DecodeThread.h
					DecodeThread.cpp
					WCustomSlider.h
					WCustomSlider.cpp
					WCtrlBarWidget.h
					WCtrlBarWidget.cpp
					WCtrlBarWidget.ui
					)

qt5_wrap_cpp(MOC_SRC MainWidget.h
					DecodeThread.h
					WCtrlBarWidget.h
					WCustomSlider.h)

qt5_wrap_ui(UI_SRC MainWidget.ui
					WCtrlBarWidget.ui)

source_group("moc" FILES ${MOC_SRC})
source_group("ui_src" FILES ${UI_SRC})
source_group("src" FILES ${SOURCES})

#设定工程的生成方式 并添加工程源文件
add_executable(VideoPlayer ${SOURCES} ${UI_SRC} ${MOC_SRC})

target_link_libraries(VideoPlayer ${QT_LIB} ${FFMPEG_LIB})
