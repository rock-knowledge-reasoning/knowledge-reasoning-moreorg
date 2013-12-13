rock_library(owl_om
    SOURCES Dummy.cpp
    HEADERS Dummy.hpp)

rock_executable(owl_om_bin Main.cpp
    DEPS owl_om)

