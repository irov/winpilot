cmake_minimum_required(VERSION 3.24)
project(winpilot CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

file(GLOB_RECURSE WINPILOT_HEADERS "include/Pilot/*.h")
file(GLOB_RECURSE WINPILOT_SOURCES "src/*.cpp" "src/*.h")

add_library(winpilot SHARED ${WINPILOT_SOURCES} ${WINPILOT_HEADERS})
target_include_directories(winpilot PUBLIC include)
target_include_directories(winpilot PRIVATE src)
target_link_libraries(winpilot PRIVATE wininet psapi)

if(NOT LIVEKIT_SDK_DIR)
    include(FetchContent)
    set(BUILD_SHARED_LIBS OFF)

    # ---- LiveKit C++ source ----
    FetchContent_Declare(livekit_src
        GIT_REPOSITORY https://github.com/livekit/client-sdk-cpp.git
        GIT_TAG v0.3.3
        GIT_SHALLOW TRUE
    )
    FetchContent_GetProperties(livekit_src)
    if(NOT livekit_src_POPULATED)
        FetchContent_Populate(livekit_src)
    endif()
    set(LIVEKIT_SRC_DIR "${livekit_src_SOURCE_DIR}")

    # ---- LiveKit Rust SDK workspace (proto files + FFI sources) ----
    FetchContent_Declare(livekit_rust
        GIT_REPOSITORY https://github.com/livekit/rust-sdks.git
        GIT_TAG livekit-ffi/v0.12.52
        GIT_SHALLOW TRUE
    )
    FetchContent_GetProperties(livekit_rust)
    if(NOT livekit_rust_POPULATED)
        FetchContent_Populate(livekit_rust)
    endif()
    set(LIVEKIT_RUST_DIR "${livekit_rust_SOURCE_DIR}")

    # ---- Abseil (protobuf dependency) ----
    set(ABSL_PROPAGATE_CXX_STD ON CACHE BOOL "" FORCE)
    set(ABSL_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(absl
        GIT_REPOSITORY https://github.com/abseil/abseil-cpp.git
        GIT_TAG 20240116.2
        GIT_SHALLOW TRUE
        OVERRIDE_FIND_PACKAGE
    )

    # ---- Protobuf library ----
    set(protobuf_MSVC_STATIC_RUNTIME OFF CACHE BOOL "" FORCE)
    set(protobuf_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(protobuf_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(protobuf_BUILD_CONFORMANCE OFF CACHE BOOL "" FORCE)
    set(protobuf_BUILD_PROTOC_BINARIES ON CACHE BOOL "" FORCE)
    set(protobuf_WITH_ZLIB OFF CACHE BOOL "" FORCE)
    set(protobuf_ABSL_PROVIDER "package" CACHE STRING "" FORCE)
    set(protobuf_INSTALL OFF CACHE BOOL "" FORCE)
    set(utf8_range_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(Protobuf
        GIT_REPOSITORY https://github.com/protocolbuffers/protobuf.git
        GIT_TAG v25.3
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(absl Protobuf)

    # ---- spdlog ----
    set(SPDLOG_BUILD_SHARED OFF CACHE BOOL "" FORCE)
    set(SPDLOG_BUILD_EXAMPLE OFF CACHE BOOL "" FORCE)
    set(SPDLOG_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(SPDLOG_INSTALL OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.15.1
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(spdlog)

    if(TARGET protobuf::protoc)
        set(PROTOC_EXECUTABLE "$<TARGET_FILE:protobuf::protoc>")
    else()
        find_program(PROTOC_EXECUTABLE NAMES protoc REQUIRED)
    endif()

    find_program(CARGO_EXECUTABLE NAMES cargo REQUIRED)

    set(RUST_ROOT "${LIVEKIT_RUST_DIR}")
    if(WIN32 AND CMAKE_SIZEOF_VOID_P EQUAL 4)
        message(FATAL_ERROR "LiveKit Rust FFI is not supported for Win32/x86. rust-sdks publishes Windows WebRTC artifacts only for x64/arm64.")
    endif()
    if(DEFINED ENV{PILOT_RUST_TARGET_DIR} AND NOT "$ENV{PILOT_RUST_TARGET_DIR}" STREQUAL "")
        file(TO_CMAKE_PATH "$ENV{PILOT_RUST_TARGET_DIR}" RUST_CARGO_TARGET_DIR)
    elseif(WIN32)
        cmake_path(GET CMAKE_BINARY_DIR ROOT_NAME _pilot_build_root)
        string(MD5 _pilot_target_hash "${LIVEKIT_RUST_DIR}|${CMAKE_SIZEOF_VOID_P}")
        string(SUBSTRING "${_pilot_target_hash}" 0 10 _pilot_target_hash_short)
        # Use a short root-level path on Windows because webrtc-sys expands deep scratch trees.
        set(RUST_CARGO_TARGET_DIR "${_pilot_build_root}/pilot-rt/${_pilot_target_hash_short}")
    else()
        # Keep cargo artifacts near the top-level build dir to avoid MAX_PATH failures in webrtc-sys.
        set(RUST_CARGO_TARGET_DIR "${CMAKE_BINARY_DIR}/pilot-rt")
    endif()
    set(RUST_TARGET_TRIPLE "")
    if(APPLE AND CMAKE_OSX_ARCHITECTURES)
        if(CMAKE_OSX_ARCHITECTURES STREQUAL "x86_64")
            set(RUST_TARGET_TRIPLE "x86_64-apple-darwin")
        elseif(CMAKE_OSX_ARCHITECTURES STREQUAL "arm64")
            set(RUST_TARGET_TRIPLE "aarch64-apple-darwin")
        endif()
    endif()

    if(RUST_TARGET_TRIPLE)
        set(RUST_TARGET_DIR "${RUST_CARGO_TARGET_DIR}/${RUST_TARGET_TRIPLE}")
    else()
        set(RUST_TARGET_DIR "${RUST_CARGO_TARGET_DIR}")
    endif()

    set(GCC_LIB_DIR "")
    if(NOT WIN32)
        get_filename_component(_gcc_bin_dir "${CMAKE_C_COMPILER}" DIRECTORY)
        get_filename_component(_gcc_root "${_gcc_bin_dir}" DIRECTORY)
        if(IS_DIRECTORY "${_gcc_root}/lib64")
            set(GCC_LIB_DIR "${_gcc_root}/lib64")
        elseif(IS_DIRECTORY "${_gcc_root}/lib")
            set(GCC_LIB_DIR "${_gcc_root}/lib")
        endif()
    endif()

    set(RUN_CARGO_SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/run_cargo.cmake")
    file(WRITE "${RUN_CARGO_SCRIPT}"
"if(NOT DEFINED CFG)
  set(CFG Debug)
endif()
if(NOT DEFINED RUST_ROOT)
  message(FATAL_ERROR \"RUST_ROOT not set\")
endif()
if(NOT DEFINED CARGO)
  message(FATAL_ERROR \"CARGO not set\")
endif()
if(NOT DEFINED CARGO_TARGET_DIR)
    message(FATAL_ERROR \"CARGO_TARGET_DIR not set\")
endif()

set(ARGS build -p livekit-ffi)
if(NOT CFG STREQUAL \"Debug\")
  list(APPEND ARGS --release)
endif()

if(DEFINED RUST_TARGET AND NOT RUST_TARGET STREQUAL \"\")
  list(APPEND ARGS --target \"\${RUST_TARGET}\")
endif()

set(ENV{PROTOC} \"\${PROTOC_PATH}\")
set(ENV{CARGO_TARGET_DIR} \"\${CARGO_TARGET_DIR}\")

if(DEFINED GCC_LIB_DIR AND NOT GCC_LIB_DIR STREQUAL \"\")
  set(ENV{RUSTFLAGS} \"-L \${GCC_LIB_DIR} \$ENV{RUSTFLAGS}\")
  set(ENV{LD_LIBRARY_PATH} \"\${GCC_LIB_DIR}:\$ENV{LD_LIBRARY_PATH}\")
endif()

execute_process(
  COMMAND \"\${CARGO}\" \${ARGS}
  WORKING_DIRECTORY \"\${RUST_ROOT}\"
  RESULT_VARIABLE rv
)
if(rv)
  message(FATAL_ERROR \"cargo build failed with code: \${rv}\")
endif()
")

    if(WIN32)
        add_library(livekit_ffi SHARED IMPORTED GLOBAL)
        set(RUST_IMPLIB_DEBUG "${RUST_TARGET_DIR}/debug/livekit_ffi.dll.lib")
        set(RUST_IMPLIB_RELEASE "${RUST_TARGET_DIR}/release/livekit_ffi.dll.lib")
        set(RUST_DLL_DEBUG "${RUST_TARGET_DIR}/debug/livekit_ffi.dll")
        set(RUST_DLL_RELEASE "${RUST_TARGET_DIR}/release/livekit_ffi.dll")
        set(RUST_LIB_DEBUG "${RUST_IMPLIB_DEBUG}")
        set(RUST_LIB_RELEASE "${RUST_IMPLIB_RELEASE}")
        set(RUST_RUNTIME_DEBUG "${RUST_DLL_DEBUG}")
        set(RUST_RUNTIME_RELEASE "${RUST_DLL_RELEASE}")

        set_target_properties(livekit_ffi PROPERTIES
            IMPORTED_CONFIGURATIONS "Debug;Release;RelWithDebInfo;MinSizeRel"
            IMPORTED_LOCATION_DEBUG "${RUST_DLL_DEBUG}"
            IMPORTED_LOCATION_RELEASE "${RUST_DLL_RELEASE}"
            IMPORTED_LOCATION_RELWITHDEBINFO "${RUST_DLL_RELEASE}"
            IMPORTED_LOCATION_MINSIZEREL "${RUST_DLL_RELEASE}"
            IMPORTED_IMPLIB_DEBUG "${RUST_IMPLIB_DEBUG}"
            IMPORTED_IMPLIB_RELEASE "${RUST_IMPLIB_RELEASE}"
            IMPORTED_IMPLIB_RELWITHDEBINFO "${RUST_IMPLIB_RELEASE}"
            IMPORTED_IMPLIB_MINSIZEREL "${RUST_IMPLIB_RELEASE}"
            INTERFACE_INCLUDE_DIRECTORIES "${RUST_ROOT}/livekit-ffi/include"
        )
    elseif(APPLE)
        add_library(livekit_ffi SHARED IMPORTED GLOBAL)
        set(RUST_LIB_DEBUG "${RUST_TARGET_DIR}/debug/liblivekit_ffi.dylib")
        set(RUST_LIB_RELEASE "${RUST_TARGET_DIR}/release/liblivekit_ffi.dylib")
        set(RUST_RUNTIME_DEBUG "${RUST_LIB_DEBUG}")
        set(RUST_RUNTIME_RELEASE "${RUST_LIB_RELEASE}")

        set_target_properties(livekit_ffi PROPERTIES
            IMPORTED_CONFIGURATIONS "Debug;Release;RelWithDebInfo;MinSizeRel"
            IMPORTED_LOCATION_DEBUG "${RUST_LIB_DEBUG}"
            IMPORTED_LOCATION_RELEASE "${RUST_LIB_RELEASE}"
            IMPORTED_LOCATION_RELWITHDEBINFO "${RUST_LIB_RELEASE}"
            IMPORTED_LOCATION_MINSIZEREL "${RUST_LIB_RELEASE}"
            IMPORTED_NO_SONAME TRUE
            INTERFACE_INCLUDE_DIRECTORIES "${RUST_ROOT}/livekit-ffi/include"
        )
    else()
        add_library(livekit_ffi SHARED IMPORTED GLOBAL)
        set(RUST_LIB_DEBUG "${RUST_TARGET_DIR}/debug/liblivekit_ffi.so")
        set(RUST_LIB_RELEASE "${RUST_TARGET_DIR}/release/liblivekit_ffi.so")
        set(RUST_RUNTIME_DEBUG "${RUST_LIB_DEBUG}")
        set(RUST_RUNTIME_RELEASE "${RUST_LIB_RELEASE}")

        set_target_properties(livekit_ffi PROPERTIES
            IMPORTED_CONFIGURATIONS "Debug;Release;RelWithDebInfo;MinSizeRel"
            IMPORTED_LOCATION_DEBUG "${RUST_LIB_DEBUG}"
            IMPORTED_LOCATION_RELEASE "${RUST_LIB_RELEASE}"
            IMPORTED_LOCATION_RELWITHDEBINFO "${RUST_LIB_RELEASE}"
            IMPORTED_LOCATION_MINSIZEREL "${RUST_LIB_RELEASE}"
            IMPORTED_NO_SONAME TRUE
            INTERFACE_INCLUDE_DIRECTORIES "${RUST_ROOT}/livekit-ffi/include"
        )
    endif()

    file(GLOB_RECURSE RUST_SOURCES
        "${RUST_ROOT}/livekit-ffi/src/*.rs"
        "${RUST_ROOT}/livekit-ffi/Cargo.toml"
        "${RUST_ROOT}/Cargo.lock"
        "${RUST_ROOT}/Cargo.toml"
    )

    set(RUST_ACTIVE_OUTPUT "$<IF:$<CONFIG:Debug>,${RUST_LIB_DEBUG},${RUST_LIB_RELEASE}>")
    set(RUST_ACTIVE_RUNTIME "$<IF:$<CONFIG:Debug>,${RUST_RUNTIME_DEBUG},${RUST_RUNTIME_RELEASE}>")

    add_custom_command(
        OUTPUT "${RUST_ACTIVE_OUTPUT}"
        BYPRODUCTS "${RUST_ACTIVE_RUNTIME}"
        COMMAND "${CMAKE_COMMAND}"
            -DCFG=$<CONFIG>
            -DRUST_ROOT=${RUST_ROOT}
            -DCARGO=${CARGO_EXECUTABLE}
            -DCARGO_TARGET_DIR=${RUST_CARGO_TARGET_DIR}
            -DPROTOC_PATH=${PROTOC_EXECUTABLE}
            -DRUST_TARGET=${RUST_TARGET_TRIPLE}
            -DGCC_LIB_DIR=${GCC_LIB_DIR}
            -P "${RUN_CARGO_SCRIPT}"
        WORKING_DIRECTORY "${RUST_ROOT}"
        DEPENDS ${RUST_SOURCES} protobuf::protoc
        COMMENT "Building Rust FFI via cargo"
        VERBATIM
    )

    add_custom_target(build_rust_ffi
        DEPENDS "${RUST_ACTIVE_OUTPUT}"
    )

    # ---- Generate build.h ----
    set(LIVEKIT_VERSION "0.3.3")
    set(GIT_COMMIT "v0.3.3")
    string(TIMESTAMP BUILD_DATE "%Y-%m-%d %H:%M:%S")
    set(GENERATED_COMMENT "Auto-generated by winpilot CMake on ${BUILD_DATE}")
    configure_file(
        "${LIVEKIT_SRC_DIR}/build.h.in"
        "${LIVEKIT_SRC_DIR}/include/livekit/build.h"
        @ONLY
    )

    # ---- Proto compilation ----
    set(FFI_PROTO_DIR "${LIVEKIT_RUST_DIR}/livekit-ffi/protocol")
    set(PROTO_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
    file(MAKE_DIRECTORY ${PROTO_BINARY_DIR})

    set(FFI_PROTO_FILES
        ${FFI_PROTO_DIR}/handle.proto
        ${FFI_PROTO_DIR}/ffi.proto
        ${FFI_PROTO_DIR}/participant.proto
        ${FFI_PROTO_DIR}/room.proto
        ${FFI_PROTO_DIR}/track.proto
        ${FFI_PROTO_DIR}/video_frame.proto
        ${FFI_PROTO_DIR}/audio_frame.proto
        ${FFI_PROTO_DIR}/e2ee.proto
        ${FFI_PROTO_DIR}/stats.proto
        ${FFI_PROTO_DIR}/data_stream.proto
        ${FFI_PROTO_DIR}/data_track.proto
        ${FFI_PROTO_DIR}/rpc.proto
        ${FFI_PROTO_DIR}/track_publication.proto
    )

    set(PROTO_SRCS)
    set(PROTO_HDRS)
    foreach(PROTO_FILE ${FFI_PROTO_FILES})
        get_filename_component(PROTO_NAME ${PROTO_FILE} NAME_WE)
        list(APPEND PROTO_SRCS "${PROTO_BINARY_DIR}/${PROTO_NAME}.pb.cc")
        list(APPEND PROTO_HDRS "${PROTO_BINARY_DIR}/${PROTO_NAME}.pb.h")
    endforeach()

    add_custom_command(
        OUTPUT ${PROTO_SRCS} ${PROTO_HDRS}
        COMMAND ${PROTOC_EXECUTABLE}
            --proto_path=${FFI_PROTO_DIR}
            --cpp_out=${PROTO_BINARY_DIR}
            ${FFI_PROTO_FILES}
        DEPENDS ${FFI_PROTO_FILES} protobuf::protoc
        COMMENT "Generating protobuf C++ files for LiveKit"
        VERBATIM
    )

    # ---- LiveKit C++ sources (compiled into winpilot.dll) ----
    set(LIVEKIT_CPP_SOURCES
        ${LIVEKIT_SRC_DIR}/src/audio_frame.cpp
        ${LIVEKIT_SRC_DIR}/src/audio_processing_module.cpp
        ${LIVEKIT_SRC_DIR}/src/audio_source.cpp
        ${LIVEKIT_SRC_DIR}/src/audio_stream.cpp
        ${LIVEKIT_SRC_DIR}/src/data_track_frame.cpp
        ${LIVEKIT_SRC_DIR}/src/data_stream.cpp
        ${LIVEKIT_SRC_DIR}/src/data_track_error.cpp
        ${LIVEKIT_SRC_DIR}/src/data_track_stream.cpp
        ${LIVEKIT_SRC_DIR}/src/e2ee.cpp
        ${LIVEKIT_SRC_DIR}/src/ffi_handle.cpp
        ${LIVEKIT_SRC_DIR}/src/ffi_client.cpp
        ${LIVEKIT_SRC_DIR}/src/livekit.cpp
        ${LIVEKIT_SRC_DIR}/src/logging.cpp
        ${LIVEKIT_SRC_DIR}/src/local_audio_track.cpp
        ${LIVEKIT_SRC_DIR}/src/local_data_track.cpp
        ${LIVEKIT_SRC_DIR}/src/remote_audio_track.cpp
        ${LIVEKIT_SRC_DIR}/src/remote_data_track.cpp
        ${LIVEKIT_SRC_DIR}/src/room.cpp
        ${LIVEKIT_SRC_DIR}/src/room_proto_converter.cpp
        ${LIVEKIT_SRC_DIR}/src/subscription_thread_dispatcher.cpp
        ${LIVEKIT_SRC_DIR}/src/local_participant.cpp
        ${LIVEKIT_SRC_DIR}/src/remote_participant.cpp
        ${LIVEKIT_SRC_DIR}/src/stats.cpp
        ${LIVEKIT_SRC_DIR}/src/track.cpp
        ${LIVEKIT_SRC_DIR}/src/track_proto_converter.cpp
        ${LIVEKIT_SRC_DIR}/src/track_publication.cpp
        ${LIVEKIT_SRC_DIR}/src/local_track_publication.cpp
        ${LIVEKIT_SRC_DIR}/src/remote_track_publication.cpp
        ${LIVEKIT_SRC_DIR}/src/rpc_error.cpp
        ${LIVEKIT_SRC_DIR}/src/video_frame.cpp
        ${LIVEKIT_SRC_DIR}/src/video_source.cpp
        ${LIVEKIT_SRC_DIR}/src/video_stream.cpp
        ${LIVEKIT_SRC_DIR}/src/local_video_track.cpp
        ${LIVEKIT_SRC_DIR}/src/remote_video_track.cpp
        ${LIVEKIT_SRC_DIR}/src/video_utils.cpp
    )

    target_sources(winpilot PRIVATE ${LIVEKIT_CPP_SOURCES} ${PROTO_SRCS} ${PROTO_HDRS})

    target_include_directories(winpilot PRIVATE
        ${LIVEKIT_SRC_DIR}/include
        ${LIVEKIT_SRC_DIR}/src
        ${LIVEKIT_RUST_DIR}/livekit-ffi/include
        ${PROTO_BINARY_DIR}
    )

    target_link_libraries(winpilot PRIVATE
        livekit_ffi
        protobuf::libprotobuf
        spdlog::spdlog
    )

    add_dependencies(winpilot build_rust_ffi)

    if(WIN32)
        target_link_libraries(winpilot PRIVATE
            ntdll userenv winmm iphlpapi msdmo dmoguids
            wmcodecdspuuid ws2_32 secur32 bcrypt crypt32
        )
    endif()

    target_compile_definitions(winpilot PRIVATE SPDLOG_ACTIVE_LEVEL=0)

    if(MSVC)
        target_compile_options(winpilot PRIVATE /permissive- /Zc:__cplusplus /utf-8)
    endif()

    # Copy livekit_ffi alongside winpilot.dll
    if(WIN32)
        add_custom_command(TARGET winpilot POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "$<IF:$<CONFIG:Debug>,${RUST_DLL_DEBUG},${RUST_DLL_RELEASE}>"
                "$<TARGET_FILE_DIR:winpilot>/livekit_ffi.dll"
            COMMENT "Copying livekit_ffi.dll"
        )
    elseif(APPLE)
        add_custom_command(TARGET winpilot POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "$<IF:$<CONFIG:Debug>,${RUST_LIB_DEBUG},${RUST_LIB_RELEASE}>"
                "$<TARGET_FILE_DIR:winpilot>/liblivekit_ffi.dylib"
            COMMENT "Copying liblivekit_ffi.dylib"
        )
    else()
        add_custom_command(TARGET winpilot POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "$<IF:$<CONFIG:Debug>,${RUST_LIB_DEBUG},${RUST_LIB_RELEASE}>"
                "$<TARGET_FILE_DIR:winpilot>/liblivekit_ffi.so"
            COMMENT "Copying liblivekit_ffi.so"
        )
    endif()
else()
    list(APPEND CMAKE_PREFIX_PATH "${LIVEKIT_SDK_DIR}")
    find_package(LiveKit REQUIRED)
    target_link_libraries(winpilot PRIVATE LiveKit::livekit)
endif()

option(WINPILOT_BUILD_EXAMPLE "Build example" OFF)
if(WINPILOT_BUILD_EXAMPLE)
    add_executable(winpilot_example example/main.cpp)
    target_link_libraries(winpilot_example PRIVATE winpilot)
endif()
