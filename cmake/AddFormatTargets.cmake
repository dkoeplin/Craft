function (add_format_targets SOURCES EXCLUDESOURCES TARGET_PREFIX)
    if (NOT __CLANG_FORMAT_INCLUDED)
        set(__CLANG_FORMAT_INCLUDED TRUE)
        find_program(CLANG_FORMAT "clang-format")
        if(CLANG_FORMAT)
            add_custom_target(
                    # inplace format the files
                    ${TARGET_PREFIX}-format
                    ${CMAKE_SOURCE_DIR}/cmake/run-clang-format.py -r -q --inplace -v
                    --clang-format-options="-style=file"
                    # add the following line to exclude formatting specific files
                    --exclude="${EXCLUDESOURCES}"
                    ${SOURCES}
            )

#            add_custom_target(
#                    # show the list of files will be formatted (without inplace format)
#                    ${TARGET_PREFIX}-format-list
#                    ${CMAKE_SOURCE_DIR}/cmake/run-clang-format.py -r -q --diff -v
#                    --clang-format-options="-style=file"
#                    # add the following line to exclude formatting sepcific files
#                    --exclude="${EXCLUDESOURCES}"
#                    ${SOURCES}
#            )
#
#            add_custom_target(
#                    # show the fomratted diff of the files (without inplace format)
#                    ${TARGET_PREFIX}-format-diff
#                    ${CMAKE_SOURCE_DIR}/cmake/run-clang-format.py -r --diff
#                    --clang-format-options="-style=file"
#                    # add the following line to exclude formatting sepcific files
#                    --exclude="${EXCLUDESOURCES}"
#                    ${SOURCES}
#            )
#            add_custom_target(
#                    # check if there are files not formatted (fail if there are)
#                    ${TARGET_PREFIX}-format-check
#                    ${CMAKE_SOURCE_DIR}/cmake/run-clang-format.py -r -q -v --faildiff
#                    --clang-format-options="-style=file"
#                    # add the following line to exclude formatting sepcific files
#                    --exclude="${EXCLUDESOURCES}"
#                    ${SOURCES}
#            )
        else(CLANG_FORMAT)
            message(FATAL_ERROR "Cannot find clang-format")
        endif(CLANG_FORMAT)
    endif (NOT __CLANG_FORMAT_INCLUDED)
endfunction()
