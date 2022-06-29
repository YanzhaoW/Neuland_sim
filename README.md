To download this repository, run the following command in your terminal:
```shell
git clone https://github.com/YanzhaoW/Neuland_sim.git
```

NOTE: To continue the following steps, an access to the GSI servers is required.
## How to run a R3BRoot Macro
### Sourcing  and installation

1. Log in to a GSI analysis server (`lxlandana01` or `lxlandana02`) with your own account and run the following command:
    ```shell
    source /u/land/fake_cvmfs/sourceme.sh
    ```
    The command above does not work at other GSI servers, such as the server you log in to by just using `ssh username@lx-pool.gsi.de`.

2. Choose any folder and download R3BRoot inside it with
    ```shell
    git clone https://github.com/R3BRootGroup/R3BRoot.git
    cd R3BRoot
    git checkout dev
    git clone https://github.com/R3BRootGroup/macros.git
    cd macros
    ```

3. At the root folder `R3BRoot/`, compile the whole project with:
    ```shell
    mkdir build
    cd build
    cmake ..
    make -j8
    ```
    where the number `8` after `-j` in the last line represents the number of cores that are used for the compilation. You can choose any number of cores you would like.

### Executing macro files
Before running any macro files, make sure you have already done the following:

```shell
source /u/land/fake_cvmfs/sourceme.sh
source ${R3BRoot_PATH}/build/config.sh
```
where `${R3BRoot_PATH}` is the root folder of your downloaded R3BRoot.

Download this repository to any folder in the analysis servers and go into the folder `macros` with:

```shell
git clone https://github.com/YanzhaoW/Neuland_sim.git
cd Neuland_sim/macros
```
Inside you can find two macro files named `sim1.C` and `digi1.C`. Simply run these macros with the following command:
```shell
root -l -q sim1.C           # simulation
root -l -q digi1.C          # digitization
```
The flag `-l` prevents root from popping up its fancy logo during the startup and the flag `-q` enables root to quit automatically when the macro is executed successfully.


## Configuration of macro files
A macro is a file that contains a definition of a function whose name should be the same as the file name. It can be executed or, more precisely, interpreted by a c++ interpreter called `ROOTCINT`, just like python files are interpreted by python3. Each macro file normally performs a certain task. In R3BRoot, they are either used for *simulation*, *analysis* or *online analysis*, which correspond to three different derived classes of `FairRun`: `FairRunSim`, `FairRunAna` and `FairRunOnline` respectively.

### Simulation macro file

The file `sim1.C` in the folder `macros/` is an example of a simple simulation macro file. The main components of the code can be explained in the following parts:

1. Instantiation of `FairRunSim`:

    For every macro used for the simulation, an instantiation of the derived class `FairRunSim` is needed:
    ```cpp
    FairRunSim* run = new FairRunSim();
    run->SetName("TGeant4");
    run->SetStoreTraj(false);

    const TString simufile = "test.simu.root";
    run->SetSink(new FairRootFileSink(simufile));
    ```
    The method `SetName` is used to specify which simulation engine is utilized. Here Geant4 is used. And `run->SetSink()` specifies the file in which all the simulated data is stored.
2. Geometrical configuration:

    Before implementing any detectors, there are two environment variables that need to be set with the following code:
    ```cpp
    const TString workDirectory = getenv("VMCWORKDIR");
    gSystem->Setenv("GEOMPATH", workDirectory + "/geometry");
    gSystem->Setenv("CONFIG_DIR", workDirectory + "/gconfig");
    ```
    The variable `GEOMPATH` is the name of the folder containing files with geometrical details of all detectors needed and definitions of various materials of the detectors.  

    Next, the file containing all detector materials needs to be implemented:
    ```cpp
    run->SetMaterials("media_r3b.geo");
    ```

    Then, multiple detectors can be defined by:
    ```cpp
    R3BCave* cave = new R3BCave("CAVE");
    cave->SetGeometryFileName("r3b_cave.geo");

    R3BNeuland* neuland = new R3BNeuland(30, {0., 0., 1650.});
    ```
    The NeuLAND detector needs to be defined with 4 parameters: number of planes and x, y, z positions. Here we have 30 planes and the NeuLAND detector is placed at 16.5 meter in z direction (downstream). The geometry "CAVE" must be defined and implemented in all simulations of the R3B experiment.

    The implementation of those geometries can be done with:
    ```cpp
    run->AddModule(cave);
    run->AddModule(neuland);
    ```

3. Configuration of projectile particles:

    For each event, the simulation engine generates user defined projectile particles with specific energy and momentum. These properties can be defined via:
    ```cpp
    FairBoxGenerator* boxGen = new FairBoxGenerator{2112, 4};
    boxGen->SetXYZ(0, 0, 0.);
    boxGen->SetThetaRange(0., 3.);
    boxGen->SetPhiRange(0., 360.);
    boxGen->SetEkinRange(0.6, 0.6);
    ```
    The first line of these codes defines the type of the particle using its *PDG number* and the multiplicity. `SetXYZ()` is used to specify the initial position of the projectile particles while `SetThetaRange()` and `SetPhiRange()` are used to set the range of the polar and azimuthal angle of the particle momentum. Subsequently, the kinetic energy range of the particle can be set using `SetEkinRange()`. Here the project particle is defined as neutron with a multiplicity equal to 4. The direction of the projectile momentum is set with a polar angle smaller than 3 degrees, and the energy of the projectiles is set to 600 MeV.

    To implement this particle in the simulation, the following code can be used:
    ```cpp
    FairPrimaryGenerator* primGen = new FairPrimaryGenerator;
    primGen->AddGenerator(boxGen);
    run->SetGenerator(primGen);
    ```

4. Initiation of `FairSimRun`:

    Before running the simulation, the object instantiated from `FariSimRun` needs to be initialized:
    ```cpp
    run->Init();
    ```
5. Configuration of parameter containers:

    Every simulation comes with various parameters which describe multiple properties useful for the later data analysis. This may include the nodes of the detector, which is essential for later data mapping, or the magnetic field applied in the whole simulation space. Each parameter is encapsulated in a data structure called parameter container. In the end of the simulation, all parameter containers need to be stored in a separate file with:
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

    In the end, the simulation will be performed event by event after the following code is executed:
    ```cpp
    run->Run(eventNum);
    ```
    where `eventNum` defines how many events need to be simulated in one run.

### Analysis macro file
An analysis macro file is used to perform an analysis on data, either obtained from the experiment or from the simulation. Some analyses can be done on the simulation data with the purpose of transforming raw simulated data points to the data points akin to experimental ones. Such analysis is called *digitization*, which can be done with an analysis macro file. The file `digi1.C` is one example of performing a very simple digitization. The main components of the code are explained in the following parts:

1. Instantiation of `FairRunAna`:

    For every macro used for the data analysis, an instantiation of the derived class `FairRunAna` is needed:
    ```cpp
    FairRunAna run;
    run.SetSource(new FairFileSource("test.simu.root"));
    run.SetSink(new FairRootFileSink("test.digi.root"));
    ```
    The last two lines of the code specify the file from which the simulated data will be read and the file to which the analyzed data will be written.

2. Geometrical configuration:

    As with the simulation macro file, the program needs to know the location of the geometry and material files, which can be done with:
    ```cpp
    const TString workDirectory = getenv("VMCWORKDIR");
    gSystem->Setenv("GEOMPATH", workDirectory + "/geometry");
    gSystem->Setenv("CONFIG_DIR", workDirectory + "/gconfig");
    ```

3. Input of parameter file:

    At the end of the simulation, multiple parameter containers are written to a parameter file. Therefore, before starting the data analysis, that parameter file `test.para.root` need to be read in with:
    ```cpp
    auto io = new FairParRootFileIo();
    io->open("test.para.root");           
    auto runtimeDb = run.GetRuntimeDb();
    runtimeDb->setFirstInput(io);
    ```

4. Adding tasks:

    Different data analyses are manifested in different tasks. Those tasks are user-defined classes derived from `FairTask` and need to overload a function called `Exec()` to perform specific data manipulations. `R3BNeulandDigitizer` is one of such classes which transforms the simulated data from NeuLAND to digitized data. It can be implemented as:
    ```cpp
    run.AddTask(new R3BNeulandDigitizer());
    ```
5. Initiation and start of the analysis:
    ```cpp
    run.Init();
    run.Run(0,0);
    ```
    The two parameters for `Run()` specify the start and end number of events that need to be analyzed. If both numbers are zero, all events will be analyzed.

Once the file `digi1.C` is executed, a new root file `test.digi.root` will be created. This root file contains a root tree called "NeulandHit" and multiple other histograms defined in `R3BNeulandDigitizer.cxx`. For more information, please check its [source file](https://github.com/R3BRootGroup/R3BRoot/blob/master/neuland/digitizing/R3BNeulandDigitizer.cxx) and [R3B/neuland](https://github.com/R3BRootGroup/R3BRoot/tree/master/neuland#digitizing).




## More information
For more information about the installation of `FairRoot` or `R3BRoot`, please visit the following websites:
- <https://github.com/FairRootGroup/FairRoot>
- <https://github.com/R3BRootGroup/R3BRoot>
- <https://github.com/R3BRootGroup/R3BRoot/wiki/Installation>
