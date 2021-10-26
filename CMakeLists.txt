# CMakeLists.txt has to be located in the project folder and cmake has to be
# executed from 'project/build' with 'cmake ../'.
cmake_minimum_required(VERSION 3.1)
find_package(Rock)
project(moreorg
    VERSION 0.2
    DESCRIPTION "Model for Reconfigurable Multi-Robot Organizations"
)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_custom_target(tidy
    COMMAND clang-tidy -format-style=file -checks=* -header-filter=/usr/include/*,/opt/*,. -p ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/src/* > ${CMAKE_BINARY_DIR}/clang-tidy.report.log
    COMMAND cmake -E echo 'Generated clang-tidy report: ${CMAKE_BINARY_DIR}/clang-tidy.report.log'
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

if(ENABLE_CLANG_TIDY)
    set(CMAKE_CXX_CLANG_TIDY
        clang-tidy;
        -header-filter=/usr/include/*,/opt/*,.;
        -checks=*;
    )
endif()

if(ENABLE_COVERAGE)
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        add_definitions(--coverage)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --coverage")
        set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} --coverage")

        add_custom_target(coverage
            COMMAND lcov --directory . --capture --no-external --output-file cov.info
            COMMAND lcov --remove cov.info '/opt' '*/test/*' '/usr/*' '*/install/*' --output-file cov.info.general_cleaned
            COMMAND lcov --remove cov.info.general_cleaned '*/moreorg/ccf/*' '*/moreorg/src/ccf/*' '*/moreorg/src/organization_model/*' --output-file cov.info.cleaned
            COMMAND genhtml -o ./cov cov.info.cleaned
            COMMAND cmake -E remove cov.info cov.info.cleaned cov.info.general_cleaned
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        )

        add_custom_target(cleancov
            COMMAND find . -type f -regex '.*.gcno' -exec rm {} +
            COMMAND find . -type f -regex '.*.gcda' -exec rm {} +
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        )
    else()
        message(FATAL_ERROR "Code coverage only works in Debug versions" )
    endif()
endif()


rock_init()
rock_standard_layout()

find_package(owlapi)

owlapi_install_ontology("http://www.rock-robotics.org/2014/01/om-base"
    "test/data/om-base-v0.4.owl")
owlapi_install_ontology("http://www.rock-robotics.org/2014/01/om-schema"
    "test/data/om-schema-v0.21.owl")
owlapi_install_ontology("http://www.rock-robotics.org/2015/12/robots/BaseCamp"
    "test/data/om-robot-basecamp.owl")
owlapi_install_ontology("http://www.rock-robotics.org/2015/12/robots/CoyoteIII"
    "test/data/om-robot-coyote-iii.owl")
owlapi_install_ontology("http://www.rock-robotics.org/2015/12/robots/Payload"
    "test/data/om-robot-payload.owl")
owlapi_install_ontology("http://www.rock-robotics.org/2015/12/robots/CREX"
    "test/data/om-robot-crex.owl")
owlapi_install_ontology("http://www.rock-robotics.org/2015/12/robots/Sherpa"
    "test/data/om-robot-sherpa.owl")
owlapi_install_ontology("http://www.rock-robotics.org/2015/12/robots/SherpaTT"
    "test/data/om-robot-sherpa-tt.owl")
owlapi_install_ontology("http://www.rock-robotics.org/2015/12/projects/TransTerrA"
    "test/data/om-project-transterra.owl")
owlapi_install_ontology("http://www.rock-robotics.org/2017/10/om-lego"
    "test/data/om-lego-v0.1.owl")
owlapi_install_ontology("http://www.rock-robotics.org/2017/10/om-icaps"
    "test/data/om-icaps-v0.1.owl")
owlapi_install_ontology("http://www.rock-robotics.org/2017/11/vrp"
    "test/data/om-vrp-v0.2.owl")

owlapi_install_ontology("http://www.rock-robotics.org/2018/10/robots/extrabots"
    "test/data/om-extrabots.owl")
owlapi_install_ontology("http://www.rock-robotics.org/2018/10/robots/robotpool"
    "test/data/om-robotpool.owl")
owlapi_install_ontology("http://qudt.org/2.1/schema/qudt"
    "test/data/qudt/qudt.ttl")
owlapi_install_ontology("http://qudt.org/2.1/schema/extensions/functions"
    "test/data/qudt/http___qudt_org_2_1_schema_extensions_functions.ttl")
