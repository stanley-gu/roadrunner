cmake_minimum_required(VERSION 2.8)
set(target lmfit)

include_directories(
../../source 
../../Wrappers/C 
${THIRD_PARTY_INSTALL_FOLDER}/include
)

set(sources 
lm 
lm_thread
lmDataStructure
lmUtils
)


add_definitions(
-DPOCO_STATIC
-DSTATIC_PUGI
-DLIBSBML_STATIC 
-DSTATIC_RR
-DSTATIC_LIBSTRUCT
-DLIBLAX_STATIC  
-DSTATIC_NLEQ
)

add_library(${target} SHARED ${sources})

if(WIN32)
add_definitions( -DWIN32)
target_link_libraries (${target} 
lmfit-static
rrc_api
)
endif()

#Not sure why linux linker need all libraries at linktime?? All symbols 'should' be in roadrunner-static
if(UNIX)
target_link_libraries (${target} 
lmfit-static
rrc_api
)
endif()

install (TARGETS ${target} 		
DESTINATION plugins 
COMPONENT plugins
)

#Skip installing any header