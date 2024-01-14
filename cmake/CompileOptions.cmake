string(CONCAT GCC_COMPILE_OPTIONS
  "$<"
    "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU>:"
      "$<BUILD_INTERFACE:"
        "-Werror;"
        "-Wall;"
        "-Wextra;"
        "-Wpedantic;"
        "-Wcast-align;"
        "-Wcast-qual;"
        "-Wconversion;"
        "-Wctor-dtor-privacy;"
        "-Wenum-compare;"
        "-Wextra-semi;"
        "-Wfloat-equal;"
        "-Wnon-virtual-dtor;"
        "-Wold-style-cast;"
        "-Woverloaded-virtual;"
        "-Wredundant-decls;"
        "-Wshadow;"
        "-Wsign-conversion;"
        "-Wsign-promo;"
        "-Wzero-as-null-pointer-constant"
      ">"
  ">"
)

# TODO: Try on MSVC (may not work)
string(CONCAT MSVC_COMPILE_OPTIONS
  "$<"
    "$<COMPILE_LANG_AND_ID:CXX,MSVC>:"
      "$<BUILD_INTERFACE:"
        "-utf-8;"
        "-W4;"
        "-WX"
      ">"
  ">"
)

add_compile_options(
  "${GCC_COMPILE_OPTIONS}"
  "${MSVC_COMPILE_OPTIONS}"
)
