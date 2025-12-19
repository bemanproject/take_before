cmake_minimum_required(VERSION 3.24)

include(FetchContent)

if(NOT BEMAN_TAKE_BEFORE_LOCKFILE)
    set(BEMAN_TAKE_BEFORE_LOCKFILE
        "lockfile.json"
        CACHE FILEPATH
        "Path to the dependency lockfile for Beman Take Before."
    )
endif()

set(BemanTakeBefore_projectDir "${CMAKE_CURRENT_LIST_DIR}/../..")
message(TRACE "BemanTakeBefore_projectDir=\"${BemanTakeBefore_projectDir}\"")

message(TRACE "BEMAN_TAKE_BEFORE_LOCKFILE=\"${BEMAN_TAKE_BEFORE_LOCKFILE}\"")
file(
    REAL_PATH
    "${BEMAN_TAKE_BEFORE_LOCKFILE}"
    BemanTakeBefore_lockfile
    BASE_DIRECTORY "${BemanTakeBefore_projectDir}"
    EXPAND_TILDE
)
message(DEBUG "Using lockfile: \"${BemanTakeBefore_lockfile}\"")

# Force CMake to reconfigure the project if the lockfile changes
set_property(
    DIRECTORY "${BemanTakeBefore_projectDir}"
    APPEND
    PROPERTY CMAKE_CONFIGURE_DEPENDS "${BemanTakeBefore_lockfile}"
)

# For more on the protocol for this function, see:
# https://cmake.org/cmake/help/latest/command/cmake_language.html#provider-commands
function(BemanTakeBefore_provideDependency method package_name)
    # Read the lockfile
    file(READ "${BemanTakeBefore_lockfile}" BemanTakeBefore_rootObj)

    # Get the "dependencies" field and store it in BemanTakeBefore_dependenciesObj
    string(
        JSON
        BemanTakeBefore_dependenciesObj
        ERROR_VARIABLE BemanTakeBefore_error
        GET "${BemanTakeBefore_rootObj}"
        "dependencies"
    )
    if(BemanTakeBefore_error)
        message(FATAL_ERROR "${BemanTakeBefore_lockfile}: ${BemanTakeBefore_error}")
    endif()

    # Get the length of the libraries array and store it in BemanTakeBefore_dependenciesObj
    string(
        JSON
        BemanTakeBefore_numDependencies
        ERROR_VARIABLE BemanTakeBefore_error
        LENGTH "${BemanTakeBefore_dependenciesObj}"
    )
    if(BemanTakeBefore_error)
        message(FATAL_ERROR "${BemanTakeBefore_lockfile}: ${BemanTakeBefore_error}")
    endif()

    if(BemanTakeBefore_numDependencies EQUAL 0)
        return()
    endif()

    # Loop over each dependency object
    math(EXPR BemanTakeBefore_maxIndex "${BemanTakeBefore_numDependencies} - 1")
    foreach(BemanTakeBefore_index RANGE "${BemanTakeBefore_maxIndex}")
        set(BemanTakeBefore_errorPrefix
            "${BemanTakeBefore_lockfile}, dependency ${BemanTakeBefore_index}"
        )

        # Get the dependency object at BemanTakeBefore_index
        # and store it in BemanTakeBefore_depObj
        string(
            JSON
            BemanTakeBefore_depObj
            ERROR_VARIABLE BemanTakeBefore_error
            GET "${BemanTakeBefore_dependenciesObj}"
            "${BemanTakeBefore_index}"
        )
        if(BemanTakeBefore_error)
            message(
                FATAL_ERROR
                "${BemanTakeBefore_errorPrefix}: ${BemanTakeBefore_error}"
            )
        endif()

        # Get the "name" field and store it in BemanTakeBefore_name
        string(
            JSON
            BemanTakeBefore_name
            ERROR_VARIABLE BemanTakeBefore_error
            GET "${BemanTakeBefore_depObj}"
            "name"
        )
        if(BemanTakeBefore_error)
            message(
                FATAL_ERROR
                "${BemanTakeBefore_errorPrefix}: ${BemanTakeBefore_error}"
            )
        endif()

        # Get the "package_name" field and store it in BemanTakeBefore_pkgName
        string(
            JSON
            BemanTakeBefore_pkgName
            ERROR_VARIABLE BemanTakeBefore_error
            GET "${BemanTakeBefore_depObj}"
            "package_name"
        )
        if(BemanTakeBefore_error)
            message(
                FATAL_ERROR
                "${BemanTakeBefore_errorPrefix}: ${BemanTakeBefore_error}"
            )
        endif()

        # Get the "git_repository" field and store it in BemanTakeBefore_repo
        string(
            JSON
            BemanTakeBefore_repo
            ERROR_VARIABLE BemanTakeBefore_error
            GET "${BemanTakeBefore_depObj}"
            "git_repository"
        )
        if(BemanTakeBefore_error)
            message(
                FATAL_ERROR
                "${BemanTakeBefore_errorPrefix}: ${BemanTakeBefore_error}"
            )
        endif()

        # Get the "git_tag" field and store it in BemanTakeBefore_tag
        string(
            JSON
            BemanTakeBefore_tag
            ERROR_VARIABLE BemanTakeBefore_error
            GET "${BemanTakeBefore_depObj}"
            "git_tag"
        )
        if(BemanTakeBefore_error)
            message(
                FATAL_ERROR
                "${BemanTakeBefore_errorPrefix}: ${BemanTakeBefore_error}"
            )
        endif()

        if(method STREQUAL "FIND_PACKAGE")
            if(package_name STREQUAL BemanTakeBefore_pkgName)
                string(
                    APPEND
                    BemanTakeBefore_debug
                    "Redirecting find_package calls for ${BemanTakeBefore_pkgName} "
                    "to FetchContent logic.\n"
                )
                string(
                    APPEND
                    BemanTakeBefore_debug
                    "Fetching ${BemanTakeBefore_repo} at "
                    "${BemanTakeBefore_tag} according to ${BemanTakeBefore_lockfile}."
                )
                message(DEBUG "${BemanTakeBefore_debug}")
                FetchContent_Declare(
                    "${BemanTakeBefore_name}"
                    GIT_REPOSITORY "${BemanTakeBefore_repo}"
                    GIT_TAG "${BemanTakeBefore_tag}"
                    EXCLUDE_FROM_ALL
                )
                set(INSTALL_GTEST OFF) # Disable GoogleTest installation
                FetchContent_MakeAvailable("${BemanTakeBefore_name}")

                # Important! <PackageName>_FOUND tells CMake that `find_package` is
                # not needed for this package anymore
                set("${BemanTakeBefore_pkgName}_FOUND" TRUE PARENT_SCOPE)
            endif()
        endif()
    endforeach()
endfunction()

cmake_language(
    SET_DEPENDENCY_PROVIDER BemanTakeBefore_provideDependency
    SUPPORTED_METHODS FIND_PACKAGE
)

# Add this dir to the module path so that `find_package(beman-install-library)` works
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_LIST_DIR}")
