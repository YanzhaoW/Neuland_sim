To download this repository, run following command in your terminal:
```shell
git clone https://github.com/YanzhaoW/Neuland_sim.git
```

NOTE: To continue the following steps, access to GSI servers is required.
## How to run R3BRoot Macro
### Sourcing  and installation

1. Log in to a GSI analysis server (`lxlandana01` or `lxlandana02`) with your own account and run the following command:
    ```shell
    source /u/land/fake_cvmfs/sourceme.sh
    ```
    The command above cannot work in other GSI servers, such as the server you log in to just using `ssh username@lx-pool.gsi.de`.

2. Choose any folder and inside it download R3BRoot with
    ```shell
    git clone https://github.com/R3BRootGroup/R3BRoot.git
    cd R3BRoot
    git checkout dev
    git clone https://github.com/R3BRootGroup/macros.git
    cd macros
    ```

3. Under the root folder `R3BRoot/`, compile the whole project with:
    ```shell
    mkdir build
    cd build
    cmake ..
    make -j8
    ```
    where the number `8` after `-j` in the last line represents the number of cores that are used for the compilation. You can choose any number of cores you would like.

### Execute Macro files
Before running any macro files, make sure you have already done following sourcing:

```shell
source /u/land/fake_cvmfs/sourceme.sh
source ${R3BRoot_PATH}/build/config.sh
```
where `${R3BRoot_PATH}` is the root folder of your downloaded R3BRoot.

Download this repository to any folder in analysis servers and go into the folder `macros` with:

```shell
git clone https://github.com/YanzhaoW/Neuland_sim.git
cd Neuland_sim/macros
```
Inside you can find a file named `sim1.C`. Simply run this macro with following command:
```shell
root -l -q sim1.C
```
The flag `-l` prevent root to pop up its fancy logo during the startup and flag `-q` enable root to quit automatically when the macro is executed successfully.


## Configuration of macro files
A macro is a file that contains a definition of a function whose name should be the same as the file name. It can be executed or, more precisely speaking, interpreted by a c++ interpreter called `ROOTCINT`, just like the case when people execute any python files. Each macro file normally performs a certain task. In R3BRoot, they are either used for *simulation*, *analysis* or *online analysis*, which correspond to three different derived classes of `FairRun`: `FairRunSim`, `FairRunAna` and `FairRunOnline` respectively.

### Simulation macro file

To make simulation macro run successfully, following steps need to be done in order:


1. Instantiation of `FairRunSim`:

    For every macro used for the simulation, the instantiation of derived class `FairRunSim` must be done with:
    ```cpp
    FairRunSim* run = new FairRunSim();
    run->SetName("TGeant4");
    run->SetStoreTraj(false);

    const TString simufile = "test.simu.root";
    run->SetSink(new FairRootFileSink(simufile));
    ```
    The method `SetName` is used to specify which simulation engine is utilized. Here it's using Geant4. And `run->SetSink()` specifies the file in which all the simulated data are stored.
2. Geometrical configuration:

    Before implementing any detectors, there are two environment variables that need to be set with following:
    ```cpp
    const TString workDirectory = getenv("VMCWORKDIR");
    gSystem->Setenv("GEOMPATH", workDirectory + "/geometry");
    gSystem->Setenv("CONFIG_DIR", workDirectory + "/gconfig");
    ```
    The variable `GEOMPATH` tells the program the folder of files that contain geometrical details of all detectors needed and definition of various materials of detectors.  

    For the next, the file containing all detector materials need to be implemented as:
    ```cpp
    run->SetMaterials("media_r3b.geo");
    ```

    Then, multiple detectors can be defined by:
    ```cpp
    R3BCave* cave = new R3BCave("CAVE");
    cave->SetGeometryFileName("r3b_cave.geo");

    R3BNeuland* neuland = new R3BNeuland(30, {0., 0., 1650.});
    ```
    NeuLAND detector need to be defined with 4 parameters: number of planes and x, y, z positions. Here we have 30 planes and NeuLAND detector is placed at 16.5 meter in z direction (downstream). Geometry "CAVE" must be defined and implemented in all simulations of R3B experiment.

    To implement those geometries, it can be done with:
    ```cpp
    run->AddModule(cave);
    run->AddModule(neuland);
    ```

3. Configuration of projectile particles:

    For each event, simulation engine generate user defined projectile particles with specific energy and momentum. These properties can be defined via:
    ```cpp
    FairBoxGenerator* boxGen = new FairBoxGenerator{2112, 4};
    boxGen->SetXYZ(0, 0, 0.);
    boxGen->SetThetaRange(0., 3.);
    boxGen->SetPhiRange(0., 360.);
    boxGen->SetEkinRange(0.6, 0.6);
    ```
    The first line of these codes defines the type of the particle using its *PDG number* and the multiplicity. `SetXYZ()` is used to specify the initial position of the projectile particles while `SetThetaRange()` and `SetPhiRange()` are used to set the range of polar and azimuthal angle of the particle momentum. Subsequently, the kinetic energy range of the particle can be set using `SetEkinRange()`. Here the project particle is defined as neutron with multiplicity equal to 4. The direction of projectile momentum is set with polar angle smaller than 3 degree. And the energy of projectiles is set to be 600 MeV.

    To implement this particle to the simulation, it can be done with following codes:
    ```cpp
    FairPrimaryGenerator* primGen = new FairPrimaryGenerator;
    primGen->AddGenerator(boxGen);
    run->SetGenerator(primGen);
    ```

4. Initiation of `FairSimRun`:
    Before running the simulation, the object instantiated from `FariSimRun` need to be initialized by following:
    ```cpp
    run->Init();
    ```
5. Configuration of parameter containers:
    Every simulation comes with various parameters which describe multiple properties useful for later data analysis. This may include the nodes of the detector, which is essential for later data mapping, or magnetic field applied throughout the whole simulation space. Each parameter is encapsulated in a data structure called parameter container. In the end of simulation, all parameter containers need to be stored in a separate file, with:
    ```cpp
    FairParRootFileIo* parFileIO = new FairParRootFileIo(true);
    const TString parafile = "test.para.root";
    parFileIO->open(parafile);
    auto rtdb = run->GetRuntimeDb();
    rtdb->setOutput(parFileIO);
    rtdb->saveOutput();
    ```
Here the parameter containers would be stored in a file named `test.para.root`.

6. Starting the run
    In the end, simulation will be performed event by event after following code is executed:
    ```cpp
    run->Run(eventNum);
    ```
    where `eventNum` defines how many events need to be simulated in a run.


## More information
For more information about installation of `FairRoot` or `R3BRoot`, please visit following websites:
- <https://github.com/FairRootGroup/FairRoot>
- <https://github.com/R3BRootGroup/R3BRoot>
- <https://github.com/R3BRootGroup/R3BRoot/wiki/Installation>
