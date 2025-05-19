message(STATUS "Using ${CMAKE_CXX_COMPILER_ID} compiler")

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "aarch64")
        # Enable branch protection against ROP and JOP attacks on AArch64
        # https://developer.arm.com/documentation/102433/0200/Applying-these-techniques-to-real-code
        add_compile_options(-mbranch-protection=standard)
    elseif (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
        # Enable control-flow protection against return-oriented programming (ROP) and jump-priented programming (JOP) attacks on x86_64
        add_compile_options(-fcf-protection=full)
    endif ()

    if(${ENABLE_RUNTIME_CHECKS})
       # Precondition checks for C++ standard library calls. Can impact performance.
       add_compile_options(-D_GLIBCXX_ASSERTIONS)
       # Enable run-time checks for variable-size stack allocation validity. Can impact performance.
       add_compile_options(-fstack-clash-protection)
       # Enable run-time checks for stack-based buffer overflows. Can impact performance.
       add_compile_options(-fstack-protector-strong)
    endif ()

    # Force retention of null pointer checks
    add_compile_options(-fno-delete-null-pointer-checks)
    # Do not assume strict aliasing
    add_compile_options(-fno-strict-aliasing)

    # compile time checks
    add_compile_options(
        -W
        -Wunused-variable
        -Wunused-parameter
        -Wunused-function
        -Wunused
        -Wno-system-headers
        -Wno-deprecated
        -Woverloaded-virtual
        -Wwrite-strings
        -Wall
        -Wextra
        -Wformat
        -Wformat=2
        -Wconversion
        -Wsign-conversion
        -Wimplicit-fallthrough
        -Werror=format-security
    )
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        message(STATUS "Setting Clang specific compiler flags")

        # compiler hardening
        # https://best.openssf.org/Compiler-Hardening-Guides/Compiler-Options-Hardening-Guide-for-C-and-C++.html
        # Consider a trailing array in a struct as a flexible array if declared as []
        add_compile_options(-fstrict-flex-arrays=3)
        # Fortify sources with compile- and run-time checks for unsafe libc usage and buffer overflows.
        add_compile_options(-D_FORTIFY_SOURCE=3)
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        message(STATUS "Setting GNU specific compiler flags")

        # compiler hardening
        # https://best.openssf.org/Compiler-Hardening-Guides/Compiler-Options-Hardening-Guide-for-C-and-C++.html
        if (${CMAKE_CXX_COMPILER_VERSION} VERSION_GREATER_EQUAL "13.0")
            if(CMAKE_BUILD_TYPE MATCHES "Release|RelWithDebInfo|MinSizeRel")
                # Fortify sources with compile- and run-time checks for unsafe libc usage and buffer overflows.
                add_compile_options(-D_FORTIFY_SOURCE=3)
            endif()
            # Consider a trailing array in a struct as a flexible array if declared as []
            add_compile_options(-fstrict-flex-arrays=3)
            # Enable warnings for possibly misleading Unicode bidirectional control characters
            add_compile_options(-Wbidi-chars=any)
        elseif (${CMAKE_CXX_COMPILER_VERSION} VERSION_GREATER_EQUAL "12.0")
            if(CMAKE_BUILD_TYPE MATCHES "Release|RelWithDebInfo|MinSizeRel")
                # Fortify sources with compile- and run-time checks for unsafe libc usage and buffer overflows.
                add_compile_options(-D_FORTIFY_SOURCE=3)
            endif()
            # Perform trivial auto variable initialization
            # add_compile_options(-ftrivial-auto-var-init=zero)
        elseif (CMAKE_BUILD_TYPE MATCHES "Release|RelWithDebInfo|MinSizeRel")
            # Fortify sources with compile- and run-time checks for unsafe libc usage and buffer overflows.
            add_compile_options(-D_FORTIFY_SOURCE=2)
        endif ()

        # Define behavior for signed integer and pointer arithmetic overflows
        add_compile_options(-fno-strict-overflow)

        if (NOT WIN32)
            # Build as position-independent executable. Can impact performance on 32-bit architectures. (-fPIE -pie)
            # Mark relocation table entries resolved at load-time as read-only. -Wl,-z,now can impact startup performance.
            # Full RELRO (-Wl,-z,relro -Wl,-z,now) disables lazy binding.
            # This allows ld.so to resolve the entire GOT at application startup and mark also the PLT portion of the GOT as read-only.
            add_compile_options(-Wl,-z,relro -Wl,-z,now)
            add_link_options(-Wl,--disable-new-dtags -pie -Wl,-z,relro -Wl,-z,now)

            if (COVERAGE_TRACKING)
                add_compile_options(-fprofile-arcs -ftest-coverage)
                add_link_options(--coverage)
                set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE 1)

                add_definitions(
                    -fdiagnostics-color=auto
                )
            endif()
        endif()
    endif()
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  # setup your MSVC toolchain here
endif()
