# ntuple_skimmer

Code to scan through a list of ROOT files, and skim trees with given cut strings.


## What does it do and how does it work?

The repository currently contains two main files:

- `ntuple-skimmer.cc`, which is a c++ skimming script for ntuples. When compiled, it
  takes an input file, and outputs the skimmed version of that file.

- `batch.py`, a python script that automatises skimming of a collection of files. It
  parses a given list of directories for a given list of file patterns. A list of files
  matching both criteria is compiled and then the c++ skimmer is called on them.
  Files belonging to the same pattern will then be merged using the `hadd` command.

Compile the c++ script with:

```
$ make
```

Then the skimming can be executed via

```
$ ./ntuple-skimmer.exe input.root output.root
```

The current implementation of the ntuple skimmer applies two cut strings
simultaneously and filters events fitting at least one of the two. The two cut
strings correspond to the hadron fake and the prompt photon control regions.

The batch python script looks for ntuples in the central ntuple directories, and
then saves the skimmed ones into a private folder (while keeping the
subdirectory structure). Call the batch script via

```
$ python batch.py
```
