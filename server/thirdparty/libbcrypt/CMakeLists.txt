# bcrypt library
add_library(bcrypt STATIC
    bcrypt.c
    crypt_blowfish/crypt_blowfish.c
    crypt_blowfish/crypt_gensalt.c
    crypt_blowfish/wrapper.c
    # Add other .c files from crypt_blowfish/ directory
)

# Include directories
target_include_directories(bcrypt PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/crypt_blowfish
)

# Compilation flags (similar to original Makefile)
target_compile_options(bcrypt PRIVATE
    -O2
    -Wall
    -fomit-frame-pointer
    -funroll-loops
)

# Hide symbols by default
set_target_properties(bcrypt PROPERTIES
    C_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
)