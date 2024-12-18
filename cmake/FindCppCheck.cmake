find_program(CPPCHECK_BIN NAMES cppcheck)

set(CPPCHECK_THREADS "-j 4" CACHE STRING "Number of threads to use for cppcheck")
set(CPPCHECK_ARG "${CPPCHECK_THREADS}" CACHE STRING "Additional arguments to pass to cppcheck")

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(
    CppCheck
    DEFAULT_MSG
    CPPCHECK_BIN
    CPPCHECK_THREADS
    CPPCHECK_ARG
)

mark_as_advanced(
    CPPCHECK_BIN
    CPPCHECK_THREADS
    CPPCHECK_ARG
)