# CMakeLists.txt has to be located in the project folder and cmake has to be
# executed from 'project/build' with 'cmake ../'.
cmake_minimum_required(VERSION 2.6)
find_package(Rock)
rock_init(organization_model 0.2)
rock_activate_cxx11()
rock_standard_layout()

find_package(owlapi)

owlapi_install_ontology("http://www.rock-robotics.org/2017/08/om-base"
    "test/data/om-base-v0.1.owl")
owlapi_install_ontology("http://www.rock-robotics.org/2014/01/om-schema"
    "test/data/om-schema-v0.19.owl")
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
owlapi_install_ontology("http://www.rock-robotics.org/2015/12/projects/TransTerrA"
    "test/data/om-project-transterra.owl")
owlapi_install_ontology("http://www.rock-robotics.org/2017/10/om-lego"
    "test/data/om-lego-v0.1.owl")
