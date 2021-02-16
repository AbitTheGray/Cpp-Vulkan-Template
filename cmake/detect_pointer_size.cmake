if(NOT DEFINED CMAKE_SIZEOF_VOID_P)
    message(NOTICE "CMAKE_SIZEOF_VOID_P not defined, detecting...")

    set(archdetect_c_code "
#define COMPILE_TIME_ASSERT(val, comp)\
    char preprocessor_array[val == comp ? -1 : 0];

COMPILE_TIME_ASSERT(sizeof(void*), 4) // SIZEOF_VOID_P 4
COMPILE_TIME_ASSERT(sizeof(void*), 8) // SIZEOF_VOID_P 8
")
    file(WRITE "${CMAKE_BINARY_DIR}/SIZEOF_VOID_P.c" "${archdetect_c_code}")

    enable_language(C)

    try_run(
            RUN_RESULT_DONT_CARE
            COMPILE_RESULT_DONT_CARE
            "${CMAKE_BINARY_DIR}"
            "${CMAKE_BINARY_DIR}/SIZEOF_VOID_P.c"
            COMPILE_OUTPUT_VARIABLE SIZEOF_VOID_P
    )

    # Parse value from try_run output
    string(REGEX MATCH "SIZEOF_VOID_P ([a-zA-Z0-9_]+)" SIZEOF_VOID_P "${SIZEOF_VOID_P}")

    # Remove value marker
    string(REPLACE "SIZEOF_VOID_P " "" SIZEOF_VOID_P "${SIZEOF_VOID_P}")

    if ((${SIZEOF_VOID_P} STREQUAL "") OR (${SIZEOF_VOID_P} EQUAL 0))
        message(FATAL_ERROR "Invalid, unrecognized or unsupported void* size ${SIZEOF_VOID_P}")
    endif()
    set(CMAKE_SIZEOF_VOID_P ${SIZEOF_VOID_P}) # Store in CMAKE_ procedure

    message(NOTICE "CMAKE_SIZEOF_VOID_P: ${CMAKE_SIZEOF_VOID_P}")
endif()
