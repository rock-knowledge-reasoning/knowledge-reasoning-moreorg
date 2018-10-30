# Organization modelling with OWL

The main purpose of this implementation of an organization model is to describe (reconfigurable)
multirobot systems so that one can reason about structure and function.
The organization model relies on a basic system
description -- provided as ontology in OWL -- to infer system combinations from it.

These system combinations (coalition / composite actors) have quantifiable
requirements. These requirements are defined by the combination of
restrictions which apply to the model of atomic actors.

The organization model allows to augment an organization model for a given
set of atomic actors.

## Examples
In general usage example can be found as part of the unit test in the subfolder
test/, e.g., in test/test_OrganizationModelAsk.cpp

A typical usage is illustrated in the following example.
Execution of the example requires a previous installation of the organisation
model, so that the ontologies are properly installed (see CMakeLists.txt).

```
#include <organization_model/OrganizationModel.hpp>
#include <organization_model/OrganizationModelAsk.hpp>
#include <organization_model/vocabularies/OM.hpp>

using namespace organization_model;

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
The utility *organization_model-reader* allows you to visualise and render the
information in an ontology. 
For example, to create a latex table to describe the properties of the agent
type SherpaTT:
```
./build/src/organization_model-reader --om http://www.rock-robotics.org/2015/12/robots/SherpaTT --latex --compact --columns 3
```

### Multiagent Scenario Analysis (deprecated)
The 'deprecated' utility *multiagent_scenario_analysis* allows for a generative
approach to evaluate the to-be-expected feasible agent types:
```
./build/src/multiagent_scenario_analysis --configuration test/data/test.scenario --coalition_size 5
```

### Benchmark
The utility *organization_model-bm* allows to test the connectivity
for a given scenario file.

```
./build/src/organization_model-bm -s test/data/benchmark/bm-spec1 -e 1 -t connectivity
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


# Copyright
Copyright (c) 2013-2018 Thomas M. Roehr, DFKI GmbH Robotics Innovation Center
