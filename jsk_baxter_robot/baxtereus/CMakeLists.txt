cmake_minimum_required(VERSION 2.8.3)
project(baxtereus)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
if($ENV{ROS_DISTRO} STREQUAL "indigo") # indigo requres to add find_pacakge(roseus) to compile roseus message
  set(ROSEUS_package roseus)
endif()
find_package(catkin REQUIRED COMPONENTS
  ${ROSEUS_package} # required in indigo
  collada_urdf
  euscollada
#  baxter_description
)

catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES baxtereus
#  CATKIN_DEPENDS collada_urdf euslisp
#  DEPENDS system_lib
)

###########
## Build ##
###########
if(EXISTS ${baxter_description_SOURCE_PREFIX}/urdf)
  set(_baxter_urdf ${baxter_description_SOURCE_PREFIX}/urdf)
else()
  set(_baxter_urdf ${baxter_description_PREFIX}/share/baxter_description/urdf)
endif()
set(_urdf_to_collada ${collada_urdf_PREFIX}/lib/collada_urdf/urdf_to_collada)
set(_collada2eus ${euscollada_PREFIX}/lib/euscollada/collada2eus)

message("baxter_urdf: ${_baxter_urdf}")
message("urdf_to_collada: ${_urdf_to_collada}")
message("collada2eus: ${_collada2eus}")

# as of 2014/1/20, *_simple.DAE files are commited in https://github.com/k-okada/baxter_common
# as of 2014/10/31 we use git://github.com/rethink-rlinsalata/baxter_common.git@k_rob-simple_meshes
# as of 2015/05/01 we use git://github.com/k-okada/baxter_common.git@fix_simple_urdf
if(EXISTS ${_baxter_urdf}/baxter_simple.urdf)
  add_custom_command(OUTPUT ${PROJECT_SOURCE_DIR}/baxter.urdf
    COMMAND sed s/_collision.STL/_collision.DAE/ ${_baxter_urdf}/baxter_simple.urdf > ${PROJECT_SOURCE_DIR}/baxter.urdf
    DEPENDS ${_baxter_urdf}/baxter_simple.urdf)
else()
  add_custom_command(OUTPUT ${PROJECT_SOURCE_DIR}/baxter.urdf
    COMMAND cp ${_baxter_urdf}/baxter.urdf ${PROJECT_SOURCE_DIR}/baxter.urdf
    DEPENDS ${_baxter_urdf}/baxter.urdf)
endif()
if(NOT EXISTS ${PROJECT_SOURCE_DIR}/baxter.l)
  add_custom_command(OUTPUT ${PROJECT_SOURCE_DIR}/baxter.dae
    COMMAND echo "${_urdf_to_collada} ${PROJECT_SOURCE_DIR}/baxter.urdf baxter.dae"
    COMMAND ${_urdf_to_collada} ${PROJECT_SOURCE_DIR}/baxter.urdf ${PROJECT_SOURCE_DIR}/baxter.dae
    DEPENDS ${PROJECT_SOURCE_DIR}/baxter.urdf)
  add_custom_command(OUTPUT ${PROJECT_SOURCE_DIR}/baxter.l
    COMMAND echo "${_collada2eus} baxter.dae baxter.l"
    COMMAND ${_collada2eus} ${PROJECT_SOURCE_DIR}/baxter.dae ${PROJECT_SOURCE_DIR}/baxter.yaml ${PROJECT_SOURCE_DIR}/baxter.l
    DEPENDS ${PROJECT_SOURCE_DIR}/baxter.dae ${PROJECT_SOURCE_DIR}/baxter.yaml ${_collada2eus})
  add_custom_target(compile_baxter ALL DEPENDS ${PROJECT_SOURCE_DIR}/baxter.l)
endif()


install(DIRECTORY euslisp test
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
  USE_SOURCE_PERMISSIONS)

install(FILES baxter.l baxter-interface.l baxter-util.l baxter.yaml DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION})

if(CATKIN_ENABLE_TESTING)
  find_package(catkin REQUIRED COMPONENTS rostest)
  add_rostest(test/test-baxter.test)
  if ("$ENV{ROS_DISTRO}" STRGREATER "indigo")  # >= kinetic
    add_rostest(test/baxter-moveit.test)
  elseif ("$ENV{ROS_DISTRO}" STRGREATER "hydro")  # >= indigo
    add_rostest(test/baxter-moveit-nogui.test)
  else()
    message(WARNING "Skip baxter-moveit.test")
  endif()
endif()
