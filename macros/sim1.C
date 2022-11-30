
class mytask : public FairTask
{
    public:
        mytask(TString input = "NeulandHits", TString output = "mytask", const Option_t* option = "");

        ~mytask() override;

        InitStatus Init() override;

        void Exec(Option_t*) override;
        void Finish() override;
        void FinishEvent() override;

    private:
        TClonesArray* fHits = nullptr; 
        TString fOut;
        int nEvent = 0;
        std::ofstream fout;
};

int sim1()
{

    // ----------------------------initial parameters--------------------------------------------
    const TString workDirectory = getenv("VMCWORKDIR");
    const TString simufile = "test.simu.root";
    const TString parafile = "test.para.root";
    const Int_t eventNum = 50;
    // const Int_t printEventNum = 100;

    FairBoxGenerator* boxGen = new FairBoxGenerator{ 2112, 4 };
    boxGen->SetXYZ(0, 0, 0.);
    boxGen->SetThetaRange(0., 3.);
    boxGen->SetPhiRange(0., 360.);
    boxGen->SetEkinRange(0.6, 0.6);

    auto neuland = new R3BNeuland(30, { 0., 0., 1650. });

    // ----------------------------Start timer--------------------------------------------
    TStopwatch* timer = new TStopwatch();
    timer->Start();
    // FairLogger::GetLogger()->SetLogVerbosityLevel("info");
    FairLogger::GetLogger()->SetLogScreenLevel("ERROR");
    // FairLogger::GetLogger()->SetLogToScreen(false);
    gSystem->Setenv("GEOMPATH", workDirectory + "/geometry");
    gSystem->Setenv("CONFIG_DIR", workDirectory + "/gconfig");

    // ----------------------------define simulation run--------------------------------------------
    FairRunSim* run = new FairRunSim();
    run->SetName("TGeant4");
    run->SetStoreTraj(kTRUE);
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


    // run->AddTask(new mytask());
    // run->AddTask(new R3BNeulandDigitizer(new Neuland::DigitizingTamex()));
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
    // // ----------------------------start run--------------------------------------------
    run->Run(eventNum);

    // // ----------------------------stop the timer--------------------------------------------
    timer->Stop();
    std::cout << "CPU time: " << timer->CpuTime() << std::endl;

    return 0;
}

mytask::mytask(TString input = "NeulandHits", TString output = "mytask", const Option_t* option = "")
    : fOut{ output }
{
}

mytask::~mytask(){}

InitStatus mytask::Init() 
{
    FairRootManager* mgr = FairRootManager::Instance();
    fout.open("position.csv", std::ios::trunc);
    fout << "Event,X,Y,Z,Energy" << std::endl;
    fHits = (TClonesArray*)mgr->GetObject("NeulandPoints");
    if(!fHits)
    {
        LOG(FATAL) << "no hits";
    }
    return kSUCCESS;
}

void mytask::Exec(Option_t*) 
{
    cout << "-------" << endl;
    auto size = fHits->GetEntriesFast();
    cout << "size: " << size << endl;
    auto next = TIter{fHits};
    while(auto hit = dynamic_cast<R3BNeulandPoint*>(next()))
    {
        std::cout << "hit position:\t" << hit->GetPosition().X() << "\t" << hit->GetPosition().Y() << "\t"
                  << hit->GetPosition().Z() - 1650 << std::endl;
    }
    // const auto hits = fHits.Retrieve();
    // std::cout << ">>>>>>>>>>Event:\t" << nEvent << std::endl;
    // for (const auto& hit : hits)
    // {
    //     std::cout << "hit position:\t" << hit->GetPosition().X() << "\t" << hit->GetPosition().Y() << "\t"
    //               << hit->GetPosition().Z() - 1650 << std::endl;
    //     fout << nEvent << "," << hit->GetPosition().X() << "," << hit->GetPosition().Y() << ","
    //          << hit->GetPosition().Z() << "," << hit->GetE() << std::endl;
    // }
    // std::cout << ">>>>>>>>>>" << std::endl;
}

void mytask::Finish()  { fout.close(); }

void mytask::FinishEvent()  { 
    nEvent++; 
    fHits->Clear();
}

