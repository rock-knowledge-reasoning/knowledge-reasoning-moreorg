# MoreOrg - Model for Reconfigurable Multi-Robot Organizations
![Build Status](https://github.com/rock-knowledge-reasoning/knowledge-reasoning-moreorg/workflows/test/badge.svg)

This library implements MoreOrg which is described in the PhD Thesis
"Autonomous Operation of a Reconfigurable Multi-Robot System for Planetary Space
Missions" ([Roehr 2019](http://nbn-resolving.de/urn:nbn:de:gbv:46-00107698-18))
and in ([Roehr 2021](https://doi.org/10.1109/TRO.2021.3118284).

This implementation of an organization model allows to describe (reconfigurable)
multirobot systems, so that one can reason on structure and function of so-called composite agents,
i.e. agents that are composed from two or more (atomic) agents.
The organization model relies on a basic system
description of each atomic agent, which is provided as ontology in the Web Ontology Language (OWL).

System combinations (aka coalition or composite agent) have quantifiable
requirements and properties. The requirements and properties are defined by the combination of
restrictions which apply to the model of atomic actors.

## Examples
A general usage example can be found as part of the unit test in the subfolder
test/, e.g., in [test/test_OrganizationModelAsk.cpp](test/test_OrganizationModelAsk)

A typical usage is illustrated in the following example.
Execution of the example requires a previous installation of the organisation
model, so that the ontologies are properly installed (see CMakeLists.txt).

```
#include <moreorg/OrganizationModel.hpp>
#include <moreorg/OrganizationModelAsk.hpp>
#include <moreorg/vocabularies/OM.hpp>

using namespace moreorg;

OrganizationModel::Ptr om =
make_shared<OrganizationModel>(owlapi::model::IRI("http://www.rock-robotics.org/2015/12/projects/TransTerrA"));

owlapi::model::IRI payload = owlapi::vocabulary::OM::resolve("Payload");
owlapi::model::IRI sherpa = owlapi::vocabulary::OM::resolve("Sherpa");

ModelPool modelPool;
modelPool[payload] = 10;
modelPool[sherpa] = 2;

OrganizationModelAsk ask(om, modelPool, true);
facades::Robot robot(modelPool, ask);

bool isAMobileCompositeAgent = robot.isMobile();
```

## Commandline Tools

### Organisation Model Reader
The utility *moreorg-reader* allows you to visualise and render the
information contained in an ontology.
For example, to create a latex table to describe the properties of the agent
type SherpaTT:
```
./build/src/moreorg-reader --om http://www.rock-robotics.org/2015/12/robots/SherpaTT --latex --compact --columns 3
```

### Multiagent Scenario Analysis (deprecated)
The 'deprecated' utility *multiagent_scenario_analysis* allows for a generative
approach to evaluate the to-be-expected feasible agent types:
```
./build/src/multiagent_scenario_analysis --configuration test/data/test.scenario --coalition_size 5
```

### Benchmark
The utility *moreorg-bm* allows to test the connectivity
for a given scenario file.

```
./build/src/moreorg-bm -s test/data/benchmark/bm-spec1 -e 1 -t connectivity
```

The benchmark requires a scenario file, consisting of the description of an
agent pool for which the connectivity shall be tested.
This first line must describe the top ontology (which has to include the
ontological information about all agent types).

```
http://www.rock-robotics.org/2015/12/projects/TransTerrA
http://www.rock-robotics.org/2014/01/om-schema#Sherpa 1
http://www.rock-robotics.org/2014/01/om-schema#CoyoteIII 1
http://www.rock-robotics.org/2014/01/om-schema#BaseCamp 1
http://www.rock-robotics.org/2014/01/om-schema#Payload 4
```

# Installation

Create a new Rock-based installation in a development folder, here called dev:
```
    mkdir dev
    cd dev
    wget http://www.rock-robotics.org/master/autoproj_bootstrap
    ruby autoproj_bootstrap
```

In autoproj/manifest add the respective manifest and add the package to the layout section:
```
    package_set:
        - github: rock-core/rock-package_set

    layout:
        - knowledge_reasoning/moreorg
```

Then use the following commands to update your workspace:
```
$>source env.sh
$>autoproj update
$>autoproj osdeps
$>amake knowledge_reasoning/moreorg
```

# Generate the documentation

To see the documentation of this project please do the following after checkout to generate the doxygen-base documentation:

```
    mkdir build
    cd build
    make doc
```

Open the doxygen documentation with a browser: build/doc/index.html The doxygen documentation contains all information on how to start using the library (as described here) along with the general API documentation.


# Testing

Running test in Rock is controlled via the Flag ROCK_TEST_ENABLED. To activate the tests let autoproj set this cmake build flag:
```
$> autoproj test enable knowledge_reasoning/moreorg
$> amake knowledge_reasoning/moreorg
```

Then you can run the tests, which are implemented with Boost Testing Framework with:
```
$> ./build/test/moreorg-test --log_level=all
```

# Merge Requests and Issue Tracking

GitHub will be used for pull requests and issue tracking: https://github.com/rock-knowledge-reasoning/knowledge-reasoning-moreorg

# License

This software is distributed under the New/3-clause BSD license.

# Copyright

Copyright (c) 2022- Thomas M. Roehr, Simula Research Laboratory
Copyright (c) 2013-2021 Thomas M. Roehr, DFKI GmbH Robotics Innovation Center
