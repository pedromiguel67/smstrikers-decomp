cmake_minimum_required(VERSION 3.20)

project(smstrikers_pc_bootstrap LANGUAGES CXX)

add_library(smstrikers_pc_platform
    src/async_file_reader.cpp
    src/content_mount.cpp
    src/frame_scheduler.cpp
    src/game_image.cpp
    src/input_mapper.cpp
)

target_include_directories(smstrikers_pc_platform
    PUBLIC
        include
)

target_compile_features(smstrikers_pc_platform PUBLIC cxx_std_20)

add_executable(smstrikers_pc_bootstrap
    src/main.cpp
)

target_link_libraries(smstrikers_pc_bootstrap PRIVATE smstrikers_pc_platform)

if(MSVC)
    target_compile_options(smstrikers_pc_platform PRIVATE /W4 /permissive-)
    target_compile_options(smstrikers_pc_bootstrap PRIVATE /W4 /permissive-)
else()
    target_compile_options(smstrikers_pc_platform PRIVATE -Wall -Wextra -Wpedantic -Werror)
    target_compile_options(smstrikers_pc_bootstrap PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

include(CTest)
if(BUILD_TESTING)
    add_test(NAME bootstrap_help COMMAND smstrikers_pc_bootstrap --help)

    add_executable(smstrikers_pc_platform_tests
        tests/game_image_test.cpp
    )
    target_link_libraries(smstrikers_pc_platform_tests PRIVATE smstrikers_pc_platform)
    if(MSVC)
        target_compile_options(smstrikers_pc_platform_tests PRIVATE /W4 /permissive-)
    else()
        target_compile_options(smstrikers_pc_platform_tests PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endif()
    add_test(NAME platform_game_image COMMAND smstrikers_pc_platform_tests)

    add_executable(smstrikers_pc_content_mount_tests
        tests/content_mount_test.cpp
    )
    target_link_libraries(smstrikers_pc_content_mount_tests PRIVATE smstrikers_pc_platform)
    if(MSVC)
        target_compile_options(smstrikers_pc_content_mount_tests PRIVATE /W4 /permissive-)
    else()
        target_compile_options(smstrikers_pc_content_mount_tests PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endif()
    add_test(NAME platform_content_mount COMMAND smstrikers_pc_content_mount_tests)

    add_executable(smstrikers_pc_async_file_reader_tests
        tests/async_file_reader_test.cpp
    )
    target_link_libraries(smstrikers_pc_async_file_reader_tests PRIVATE smstrikers_pc_platform)
    if(MSVC)
        target_compile_options(smstrikers_pc_async_file_reader_tests PRIVATE /W4 /permissive-)
    else()
        target_compile_options(smstrikers_pc_async_file_reader_tests PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endif()
    add_test(NAME platform_async_file_reader COMMAND smstrikers_pc_async_file_reader_tests)

    add_executable(smstrikers_pc_frame_scheduler_tests
        tests/frame_scheduler_test.cpp
    )
    target_link_libraries(smstrikers_pc_frame_scheduler_tests PRIVATE smstrikers_pc_platform)
    if(MSVC)
        target_compile_options(smstrikers_pc_frame_scheduler_tests PRIVATE /W4 /permissive-)
    else()
        target_compile_options(smstrikers_pc_frame_scheduler_tests PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endif()
    add_test(NAME platform_frame_scheduler COMMAND smstrikers_pc_frame_scheduler_tests)

    add_executable(smstrikers_pc_input_mapper_tests
        tests/input_mapper_test.cpp
    )
    target_link_libraries(smstrikers_pc_input_mapper_tests PRIVATE smstrikers_pc_platform)
    if(MSVC)
        target_compile_options(smstrikers_pc_input_mapper_tests PRIVATE /W4 /permissive-)
    else()
        target_compile_options(smstrikers_pc_input_mapper_tests PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endif()
    add_test(NAME platform_input_mapper COMMAND smstrikers_pc_input_mapper_tests)
endif()
