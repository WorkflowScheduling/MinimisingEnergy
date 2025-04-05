## Minimizing Energy usage in Reliability and Deadline aware Workflow Scheduling with Data-Dependent Execution time

### Dataset

The `Dataset` folder contains a detailed parsing of the `Pegasus Workflow Benchmarks`. The parsing was performed using `Go`, converting XML files into TXT files for easier access and processing.

The parsed TXT files are located in the `Dataset/Parsed` directory.

### Code

The `Code` folder contains detailed implementations of the approaches discussed in the paper

### Implementations

1. `bcp.cpp` — Implements a greedy approach that schedules all tasks on the best Virtual Machines (VMs).
2. `lef.cpp` — Implements `Algorithm 1` and `Algorithm 2`.
3. `dy.cpp` — Implements `Algorithm 1` and `Algorithm 3`.
4. `dynamicApproach.cpp` — Implements `Algorithm 5`.
5. `rews.cpp` — Implements a state-of-the-art approach that considers *reliability* as the only hard constraint.
6. `remsm_dvfs.cpp` — Implements a state-of-the-art approach that considers *deadline* as the only hard constraint.


### Code Structure
```
├── Code
│   ├── Results
│   ├── bcp.cpp
│   ├── dataClasses.cpp
│   ├── dy.cpp
│   ├── dynamicApproach.cpp
│   ├── lef.cpp
│   ├── remsm_dvfs.cpp
│   ├── rews.cpp
│   ├── simulatedAnnaleling.cpp
│   └── utils.cpp
├── Dataset
│   ├── Parsed
│   ├── System-Model
│   │   ├── sys_model.py
│   │   └── system_model_data.csv
│   ├── WorkFlowBenchmark
│   │   ├── Cybershake
│   │   │   ├── CyberShake_100.xml
│   │   │   ├── CyberShake_1000.xml
│   │   │   ├── CyberShake_30.xml
│   │   │   └── CyberShake_50.xml
│   │   ├── Epigenomics
│   │   │   ├── Epigenomics_100.xml
│   │   │   ├── Epigenomics_24.xml
│   │   │   ├── Epigenomics_46.xml
│   │   │   └── Epigenomics_997.xml
│   │   ├── Inspiral
│   │   │   ├── Inspiral_100.xml
│   │   │   ├── Inspiral_1000.xml
│   │   │   ├── Inspiral_30.xml
│   │   │   └── Inspiral_50.xml
│   │   ├── Montage
│   │   │   ├── Montage_100.xml
│   │   │   ├── Montage_1000.xml
│   │   │   ├── Montage_25.xml
│   │   │   └── Montage_50.xml
│   │   ├── Others
│   │   │   ├── avianflu_large.xml
│   │   │   ├── avianflu_medium.xml
│   │   │   ├── avianflu_small.xml
│   │   │   ├── cadsr.xml
│   │   │   ├── floodplain.xml
│   │   │   ├── gene2life.xml
│   │   │   ├── glimmer.xml
│   │   │   ├── leadadas.xml
│   │   │   ├── leaddm.xml
│   │   │   ├── leaddm.xml~
│   │   │   ├── leadmm.xml
│   │   │   ├── leadmm.xml~
│   │   │   ├── mcstas.xml
│   │   │   ├── mememast.xml
│   │   │   ├── molsci.xml
│   │   │   ├── motif_large.xml
│   │   │   ├── motif_medium.xml
│   │   │   ├── motif_small.xml
│   │   │   ├── psload_large.xml
│   │   │   ├── psload_medium.xml
│   │   │   ├── psload_small.xml
│   │   │   ├── psmerge_large.xml
│   │   │   ├── psmerge_medium.xml
│   │   │   ├── psmerge_small.xml
│   │   │   ├── scoop_large.xml
│   │   │   ├── scoop_large.xml~
│   │   │   ├── scoop_medium.xml
│   │   │   └── scoop_small.xml
│   │   └── SIPHT
│   │       ├── Sipht_100.xml
│   │       ├── Sipht_1000.xml
│   │       ├── Sipht_30.xml
│   │       └── Sipht_60.xml
│   ├── controller.go
│   ├── go.mod
│   ├── main.go
│   └── xmlstruct.go

```