# Organization modelling with OWL

The main purpose of this implementation of an organization model is to describe (reconfigurable)
multirobot systems so that one can reason about structure and function.
The organization model relies on a basic system
description -- provided in OWL -- to infer system combinations from it.

These system combinations (coalition / composite actors) have quantifiable
requirements. These requirements are defined by the combination of
restrictions which apply to the model of atomic actors.

The organization model allows to augment an organization model for a given
set of atomic actors.

## Benchmark

./build/src/organization_model-bm -s test/data/benchmark/bm-spec1 -e 1 -t
connectivity

```
http://www.rock-robotics.org/2015/12/projects/TransTerrA
http://www.rock-robotics.org/2014/01/om-schema#Sherpa 1
http://www.rock-robotics.org/2014/01/om-schema#CoyoteIII 1
http://www.rock-robotics.org/2014/01/om-schema#BaseCamp 1
http://www.rock-robotics.org/2014/01/om-schema#Payload 4
```
