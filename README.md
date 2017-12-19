# ntuple_skimmer
Code to scan through a list of ROOT files, and skim trees with given cut strings.

## What does it do and how does it work?

The repository currently contains two main files:
- `skim_file.cc`, which is a c++ skimming script for ntuples. When compiled, it takes an input file, and outputs the skimmed version of that file.
- `batch.py`, a python script to parse a given list of directories for a given list of file patterns. The script creates a list of files matching both criteria and then calls the skimmer on it.

Compile the c++ script with:
```
$ g++ -Wall `root-config --cflags --libs` skim_file.cc -o skimmer
```
Then the skimming can be executed via
```
$ ./skimmer input.root output.root
```
The current implementation of the ntuple skimmer applies two cut strings simultaneously and filters events fitting at least one of the two. The two cut strings correspond to the hadron fake and the prompt photon control regions.

The batch python script looks for ntuples in the central ntuple directories, and then saves the skimmed ones into a private folder (while keeping the subdirectory structure).
