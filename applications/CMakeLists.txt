foreach(prog list-wells)
    set(py "${prog}.py")
    add_custom_command(OUTPUT ${py}
                       COMMAND cmake -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${py}
                       ${CMAKE_BINARY_DIR}/${py}
                       DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${py}
                      )
    list(APPEND py_dest ${py})
endforeach()

add_custom_target(pyapp DEPENDS ${py_dest})
add_dependencies(opym pyapp)

