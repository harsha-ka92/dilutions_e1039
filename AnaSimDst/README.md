# e1039-analysis/AnaSimDst

A compact program to analyze the simulated DST file, which is created by the `SimChainDev` package.
Since it is yet under development and also depends on other software under development,
it might stop working occasionally.  In such cases, please contact the author.

## Analysis Flow

1. DST -> Micro DST (uDST)
    - By macro `Fun4SimDst.C`.
    - To reduce the data size.
1. uDST -> Tree
    - By macro `Fun4SimMicroDst.C`
    - To further reduce the data size & calculate your custom quantities.
1. Tree -> Plots
    - By macro `Fun4SimTree.C`
    - To draw your quantities with various conditions.

When the data size is small enough or the analysis logic is simple enough in your analysis, you can put all the steps in one macro.

## Basic Usage

You can try the following procedure to go through the whole analysis flow.

### Preparation of Simulated DST Files

You first produce a set of simulated DST files by `SimChainDev`.
This document assumes that the top directory of your simDST files is `/your/dst/dir`.
It can be either relative or absolute.

The following command lists up all simDSTs into `list_dst.txt`;
```
cd e1039-analysis/AnaSimDst/work
./make-dst-list.sh /your/dst/dir
```
You might manually create or edit the list file as you need.

### Compilation of Analysis Code

The analysis code is located in `e1039-analysis/AnaSimDst/src`.
The following commands set up the shell environment and compile the code;
```
cd e1039-analysis/AnaSimDst
source setup.sh
cmake-this
make-this
```
You are expected to later look into the script and the analysis code to make your own analysis logic.

### Execution of Analysis

The 1st step is to create uDST from DSTs.
The following commands reads five simDSTs for test.
The uDST file is `uDST.root`.
```
cd e1039-analysis/AnaSimDst
source setup.sh
cd work
root -b -q 'Fun4SimDst.C(5)'
```

The 2nd step is to create Tree from uDST, by the following command.
The TTree object is outputted to `sim_tree.root`.
```
root -b -q Fun4SimMicroDst.C
```

The last step is draw plots from Tree.
```
root -b -q Fun4SimTree.C
```
Various plots are outputted to `result/`.

If all the commands ran fine, you delete `(5)` (or change to `(0)`) in the 1st step to use all DSTs.

## Advanced Usage

### `cmake-this`

You should execute this function when you add/delete files under `src/`.
It is a shell function defined in `setup.sh`.
It calls 'cmake' with a proper configuration.
It cleans up all files in the build & install directories.

### `make-this`

You should execute this function when you modify files under `src/`.
It is a shell function defined in `setup.sh`.
It calls 'make install' with a proper configuration.

## Author

Kenichi Naknao <knakano@nucl.phys.titech.ac.jp>
