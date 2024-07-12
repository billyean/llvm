macro(add_ldu_subdirectory name)
    add_llvm_subdirectory(LDU TOOL ${name})
endmacro()

macro(add_ldu_library name)
    if(BUILD_SHARED_LIBS)
        set(LIBTYPE SHARED)
    else()
        set(LIBTYPE STATIC)
    endif()
    llvm_add_library(${name} ${LIBTYPE} ${ARGN})
    if(TARGET ${name})
        target_link_libraries(${name} INTERFACE ${LLVM_COMMON_LIBS})
        install(TARGETS ${name}
                COMPONENT ${name}
                LIBRARY DESTINATION lib${LLVM_LIBDIR_SUFFIX}
                ARCHIVE DESTINATION lib${LLVM_LIBDIR_SUFFIX}
                RUNTIME DESTINATION bin)
    else()
        add_custom_target(${name})
    endif()
endmacro()

macro(add_ldu_executable name)
    add_llvm_executable(${name} ${ARGN} )
endmacro()

macro(add_ldu_main name)
    add_ldu_executable(${name} ${ARGN})
    install(TARGETS ${name}
            RUNTIME DESTINATION bin
            COMPONENT ${name})
endmacro()