# CMakeLists.txt has to be located in the project folder and cmake has to be
# executed from 'project/build' with 'cmake ../'.
cmake_minimum_required(VERSION 2.6)
find_package(Rock)
rock_init(organization_model 0.2)
rock_activate_cxx11()
rock_standard_layout()

find_package(owlapi)

owlapi_install_ontology("http://www.rock-robotics.org/2014/01/om-schema"
    "test/data/om-schema-v0.15.owl")
owlapi_install_ontology("http://www.rock-robotics.org/2015/12/robots/BaseCamp"
    "test/data/om-robot-basecamp-v0.1.owl")
owlapi_install_ontology("http://www.rock-robotics.org/2015/12/robots/CoyoteIII"
    "test/data/om-robot-coyote-iii-v0.1.owl")
owlapi_install_ontology("http://www.rock-robotics.org/2015/12/robots/Payload"
    "test/data/om-robot-payload-v0.1.owl")
owlapi_install_ontology("http://www.rock-robotics.org/2015/12/robots/CREX"
    "test/data/om-robot-crex-v0.1.owl")
owlapi_install_ontology("http://www.rock-robotics.org/2015/12/robots/Sherpa"
    "test/data/om-robot-sherpa-v0.2.owl")
owlapi_install_ontology("http://www.rock-robotics.org/2015/12/projects/TransTerrA"
    "test/data/om-project-transterra.owl")
