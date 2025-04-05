## Dataset

This folder contains a detailed parsing of the `Pegasus Workflow Benchmarks`. The parsing was performed using `Go`, converting XML files into TXT files for easier access and processing.

The parsed TXT files are located in the `Parsed` directory.

### Parsed Format

The parsed files have a format of this form in txt files.

```
Number_of_Nodes Realibility_Contraints Deadline_Contraints 

This is followed by `Number_of_Nodes` lines, each representing a node in the workflow graph.

Node Description (Per Line)

Each node line contains:

1. `Number_of_Outgoing_Edges` — The number of edges going out from this node.
2. Followed by `Number_of_Outgoing_Edges` pairs:
   - Each pair consists of:
     - The destination node (to which the edge points).
     - The weight of the edge.

---

### Example Structure (cadsr.txt)

4 0.9 30
5 1 1 15728640
5 1 2 10485760
5 0
5 2 0 10485760 1 10485760

```

This structure represents a Directed Acyclic Graph (DAG), where:

1. Nodes correspond to tasks.
2. Edges represent dependencies between tasks along with their respective weights.

