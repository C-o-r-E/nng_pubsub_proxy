cmake_minimum_required(VERSION 3.10)
project(proxy C)

set(CMAKE_C_STANDARD 23)

# Find the nng library
find_package(nng REQUIRED)

# Add the executable target
add_executable(proxy proxy.c)

target_compile_options(proxy PRIVATE -Wall -Wextra -Wpedantic -Werror -O2)

# Link against the nng library
target_link_libraries(proxy PRIVATE nng)
