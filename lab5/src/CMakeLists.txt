cmake_minimum_required(VERSION 2.8)
 
PROJECT(Slam)
 # Locate libraries and headers
#find_package(WiringPi REQUIRED)
#find_package(Threads REQUIRED) 

# Include header

# Link against libraries


find_package(VTK REQUIRED)

include(${VTK_USE_FILE})
 add_definitions(-std=c++0x -L/usr/local/lib/libwringPi.so -lwiringPi )
add_executable(Slam MACOSX_BUNDLE lab5_main.cpp SLAM.cpp PanTilt.cpp Viz.cpp LidarLite.cpp Camera.cpp )
 target_link_libraries(Slam wiringPi)
target_link_libraries(Slam ${VTK_LIBRARIES})
#target_link_libraries(Slam${WIRINGPI_LIBRARIES}) 
#target_link_libraries(Slam ${CMAKE_THREAD_LIBS_INIT}) 
