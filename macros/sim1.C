int sim1(){

    // ----------------------------initial parameters--------------------------------------------
    const TString workDirectory = getenv("VMCWORKDIR");
    const TString simufile = "test.simu.root";
    const TString parafile = "test.para.root";
    const Int_t eventNum = 10000;
    const Int_t printEventNum = 100;

    FairBoxGenerator* boxGen = new FairBoxGenerator{2112, 4};
    boxGen->SetXYZ(0, 0, 0.);
    boxGen->SetThetaRange(0., 3.);
    boxGen->SetPhiRange(0., 360.);
    boxGen->SetEkinRange(0.6, 0.6);

    auto neuland = new R3BNeuland(30, {0., 0., 1650.});

    // ----------------------------Start timer--------------------------------------------
    TStopwatch* timer = new TStopwatch();
    timer->Start();
    FairLogger::GetLogger()->SetLogVerbosityLevel("verylow");
    FairLogger::GetLogger()->SetLogScreenLevel("nolog");
    // FairLogger::GetLogger()->SetLogToScreen(false);
    gSystem->Setenv("GEOMPATH", workDirectory + "/geometry");
    gSystem->Setenv("CONFIG_DIR", workDirectory + "/gconfig");

    // ----------------------------define simulation run--------------------------------------------
    FairRunSim* run = new FairRunSim();
    run->SetName("TGeant4");
    run->SetStoreTraj(false);
    run->SetMaterials("media_r3b.geo");
    run->SetSink(new FairRootFileSink(simufile));

    // ----------------------------create particle generation--------------------------------------------
    FairPrimaryGenerator* primGen = new FairPrimaryGenerator;
    primGen->AddGenerator(boxGen);
    run->SetGenerator(primGen);

    // ----------------------------specify geometry--------------------------------------------
    // Geometry: Cave
    R3BCave* cave = new R3BCave("CAVE");
    cave->SetGeometryFileName("r3b_cave.geo");
    run->AddModule(cave);
    run->AddModule(neuland);

    // ----------------------------Initialize run--------------------------------------------
    run->Init();
    // auto grun = G4RunManager::GetRunManager();
    // grun->SetPrintProgress(printEventNum);
    // auto event = dynamic_cast<TG4EventAction*>(
    //     const_cast<G4UserEventAction*>(grun->GetUserEventAction()));
    // event->VerboseLevel(0);

    // ----------------------------Specify parameter output--------------------------------------------
    FairParRootFileIo* parFileIO = new FairParRootFileIo(true);
    parFileIO->open(parafile);
    auto rtdb = run->GetRuntimeDb();
    rtdb->setOutput(parFileIO);
    rtdb->saveOutput();

    // ----------------------------start run--------------------------------------------
    run->Run(eventNum);

    // ----------------------------stop the timer--------------------------------------------
    timer->Stop();
    std::cout << "CPU time: " << timer->CpuTime() << std::endl;


    return 0;
}
