# helper variables
find_package(Python3 3.7 REQUIRED COMPONENTS Interpreter)
execute_process(COMMAND ${Python3_EXECUTABLE} -m site --user-site
    OUTPUT_VARIABLE Python3_SITEUSER
    RESULT_VARIABLE rv
)
if (NOT ${rv} MATCHES "0")
    message(FATAL_ERROR "Could not determine Python3 user site-package location: ${rv}")
endif()
string(REPLACE "\n" "" Python3_SITEUSER ${Python3_SITEUSER})
find_program(DOTS-CG NAMES dcg.py PATHS ${Python3_SITEARCH} ${Python3_SITEUSER} PATH_SUFFIXES bin)
if(${DOTS-CG} STREQUAL DOTS-CG-NOTFOUND)
    message(FATAL_ERROR "Could not find DOTS code generator")
endif()
if(NOT DEFINED DOTS-CG-CPP_DIR)
    message(FATAL_ERROR "DOTS-CG-CPP_DIR variable is not set")
endif()

set(DOTS-CG_CONFIG "config_cpp" CACHE INTERNAL "Internal helper variable containing the DOTS-CG config file name for C++")
set(DOTS-CG_TEMPLATE_DIR ${DOTS-CG-CPP_DIR} CACHE INTERNAL "Internal helper variable containing the DOTS-CG template directory")
set(DOTS-CG_TEMPLATE_LIST
    ${DOTS-CG-CPP_DIR}/struct.dots.h.dotsT;
    ${DOTS-CG-CPP_DIR}/enum.dots.h.dotsT;
    CACHE INTERNAL "Internal helper variable containing the C++ code generation templates"
)
set(DOTS-CG-CPP-GENERATE_CMD 
    ${Python3_EXECUTABLE} ${DOTS-CG} --config=${DOTS-CG-CPP_DIR}/${DOTS-CG_CONFIG}.py --templatePath=${DOTS-CG_TEMPLATE_DIR}
    CACHE INTERNAL "Internal helper variable containing the DOTS-CG generate command"
)

# code generation
function(GET_GENERATED_DOTS_TYPES GENERATED_TYPES MODEL)
    execute_process(COMMAND ${DOTS-CG-CPP-GENERATE_CMD} --list-generated ${MODEL}
        OUTPUT_VARIABLE generated_types_list
        RESULT_VARIABLE rv
    )
    if (NOT ${rv} MATCHES "0")
        message(FATAL_ERROR "Could not generate type list: ${rv}")
    endif()
    string(REPLACE "\n" ";" out_types ${generated_types_list})
    set(${GENERATED_TYPES} ${out_types} PARENT_SCOPE)
endfunction(GET_GENERATED_DOTS_TYPES)

function(GENERATE_DOTS_TYPES GENERATED_SOURCES GENERATED_HEADERS)
    foreach(MODEL ${ARGN})
        # ensure that model path is absolute
        if(NOT IS_ABSOLUTE "${MODEL}")
            set(MODEL "${CMAKE_CURRENT_SOURCE_DIR}/${MODEL}")
        endif()

        # gather generated source and header files
        get_filename_component(Basename ${MODEL} NAME)
        GET_GENERATED_DOTS_TYPES(GENERATED_TYPES ${MODEL})
        list(APPEND generated_sources ${GENERATED_TYPES})
        foreach(TYPE ${GENERATED_TYPES})
            if(TYPE MATCHES "\\.dots\\.h$" )
                list(APPEND generated_headers ${CMAKE_CURRENT_BINARY_DIR}/${TYPE})
            endif()
        endforeach()

        # create generation target for all types in model
        add_custom_command(OUTPUT ${GENERATED_TYPES}
            COMMAND ${DOTS-CG-CPP-GENERATE_CMD} ${MODEL}
            DEPENDS ${DOTS-CG_TEMPLATE_LIST} ${MODEL}
            COMMENT "Generate DOTS C++ classes from ${MODEL}"
        )

        set(${GENERATED_SOURCES} ${generated_sources} PARENT_SCOPE)
        set(${GENERATED_HEADERS} ${generated_headers} PARENT_SCOPE)
    endforeach()
endfunction(GENERATE_DOTS_TYPES)