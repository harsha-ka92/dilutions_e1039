# A Program to Run Full Chain of E1039 Simulation

## Environment

This program depends on the E1039 resource+share+core packages.
Unless you have a special reason, you are recommended to use "spinquestgpvm01.fnal.gov",
where the E1039 packages are ready to use.

You can just execute the following command to set up the environment properly;
```
source /exp/seaquest/app/software/osg/software/e1039/this-e1039.sh
```
You have to do so when you start a new shell environment (i.e. text terminal).

## Download

You move to a working directory and check out the repository;
```
cd /path/to/your_working_directory
git clone https://github.com/E1039-Collaboration/e1039-analysis.git
```
If you are a member of the GitHub E1039 group, you better use the following command to obtain the write access;
```
git clone git@github.com:E1039-Collaboration/e1039-analysis.git
```

## Test Process

You run the following commands to run a small test.
It will take 10-20(?) minutes.
Note that you have to set up the environment beforehand, as explained above.

```
cd e1039-analysis/SimChainDev
root -b -q Fun4Sim.C
```

By default a simple muon-pair event (via PHG4SimpleEventGenerator) is simulated.
You can/should change various options to simulate what you need.

The simulation result is written into multiple ROOT files.
The main output file is "DST.root", which is structured in the E1039 standard data format.
It is usually analyzed by `e1039-analysis/AnaSimDst`.

## Large Process

When you want a large set of simulated events,
you should run the simulation on the grid.
The procedure is explained in
[this Wiki page](https://github.com/E1039-Collaboration/e1039-wiki/wiki/Submit-jobs-to-the-grid).

## More information

You could refer to
[E1039-simulation-tutorial---Apr.-19](https://github.com/E1039-Collaboration/e1039-wiki/wiki/E1039-simulation-tutorial---Apr.-19).
