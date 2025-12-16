cmake_minimum_required(VERSION 3.14)
project(terminal_bank LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# --- Dependencies ---

# FTXUI
FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG v5.0.0
)
FetchContent_MakeAvailable(ftxui)

# nlohmann_json
FetchContent_Declare(json
    GIT_REPOSITORY https://github.com/nlohmann/json
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(json)

# CPR (C++ Requests)
FetchContent_Declare(cpr
    GIT_REPOSITORY https://github.com/libcpr/cpr.git
    GIT_TAG 1.10.5
)
FetchContent_MakeAvailable(cpr)

# SQLite3
find_package(SQLite3 REQUIRED)

# --- Source Files ---
file(GLOB_RECURSE SOURCES "src/*.cpp")

# --- Executable ---
add_executable(terminal_bank ${SOURCES})

# --- Include Directories ---
target_include_directories(terminal_bank PRIVATE src)

# --- Linking ---
target_link_libraries(terminal_bank
  PRIVATE
    ftxui::screen
    ftxui::dom
    ftxui::component
    SQLite::SQLite3
    nlohmann_json::nlohmann_json
    cpr::cpr
)
