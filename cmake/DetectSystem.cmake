function(detect_system)
    execute_process(
        COMMAND bash -c "uname -a"
        OUTPUT_VARIABLE BASH_OUTPUT
        RESULT_VARIABLE BASH_RESULT
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    string(FIND "${BASH_OUTPUT}" "microsoft" WSL_FOUND)

    if(${WSL_FOUND} )
        set(SYSTEM_WSL "true" PARENT_SCOPE)
    else()
    set(SYSTEM_WSL "false" PARENT_SCOPE)
    endif()
endfunction(detect_system)
