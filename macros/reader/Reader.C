// NeuLAND Online Monitoring
// run: root -l -q -b neuland_online.C

struct EXT_STR_h101_t
{
    EXT_STR_h101_unpack_t unpack;
    EXT_STR_h101_TPAT tpat;
    EXT_STR_h101_LOS_t los;
    EXT_STR_h101_raw_nnp_tamex_onion_t raw_nnp;
};

void Reader()
{
    TStopwatch timer;
    timer.Start();
  
    const Int_t nBarsPerPlane = 50; // number of scintillator bars per plane
    const Int_t nPlanes = 26;       // number of planes (for TCAL calibration)
    const double distanceToTarget = 1520.;

    const Int_t nev = -1;     /* number of events to read, -1 - until CTRL+C */
    const Int_t trigger = -1; // 1 - onspill, 2 - offspill. -1 - all

    const TString filename = "/d/land4/202205_s522/lmd/main0122_0021.lmd";
    
    const TString ucesbPath = "/u/land/fake_cvmfs/9.13/upexps/202205_s522/202205_s522";
    // const TString usesbCall = ucesbPath + " --allow-errors --input-buffer=135Mi";
    const TString usesbCall = ucesbPath + " --allow-errors --input-buffer=135Mi  --max-events=100";

    const TString outputFileName = "output.root";

    // Event IO Setup
    // -------------------------------------------
    EXT_STR_h101 ucesbStruct;
    // auto source = new R3BUcesbSource(filename, "RAW,time-stitch=1000", usesbCall, &ucesbStruct, sizeof(ucesbStruct));
    auto source = new R3BUcesbSource(filename, "UNPACK:EVENTNO,UNPACK:TRIGGER,RAW", usesbCall, &ucesbStruct, sizeof(ucesbStruct));
    //auto source = new R3BUcesbSource(filename, "RAW", usesbCall, &ucesbStruct, sizeof(ucesbStruct));
    source->SetMaxEvents(nev);

    source->AddReader(new R3BUnpackReader(&ucesbStruct.unpack, offsetof(EXT_STR_h101, unpack)));
    source->AddReader(new R3BTrloiiTpatReader((EXT_STR_h101_TPAT_t*)&ucesbStruct.tpat, offsetof(EXT_STR_h101, tpat)));
    source->AddReader(new R3BLosReader(&ucesbStruct.los, offsetof(EXT_STR_h101, los)));
    source->AddReader(new R3BNeulandTamexReader(&ucesbStruct.raw_nnp, offsetof(EXT_STR_h101, raw_nnp)));

    auto run = new FairRunOnline(source);
    run->SetRunId(999);
    // run->ActivateHttpServer(1, 8885);
	run->SetOutputFile(outputFileName.Data());
    // run->SetSink(new FairRootFileSink(outputFileName));

    // Parameter IO Setup
    // -------------------------------------------
    auto rtdb = run->GetRuntimeDb();

    auto parList = new TList();
    //parList->Add(new TObjString("LosTcalPar_0010.root")); // no time ref
    parList->Add(new TObjString("loscalpar_v1.root"));
    parList->Add(new TObjString("params_tcal_180522xx.root"));
    //parList->Add(new TObjString("params_sync_s522_0999_190522.root"));
    parList->Add(new TObjString("params_sync_s522_0999_310522x.root"));
    //parList->Add(new TObjString("params_ig_fine.root"));
     //parList->Add(new TObjString("params_sync_0047_49_0999.root"));
 
    auto parIO = new FairParRootFileIo(false);
    
    parIO->open(parList);
    rtdb->setFirstInput(parIO);

    rtdb->addRun(999);
    rtdb->getContainer("LosTCalPar");
    rtdb->setInputVersion(999, (char*)"LosTCalPar", 1, 1);
    rtdb->getContainer("LandTCalPar");
    rtdb->setInputVersion(999, (char*)"LandTCalPar", 1, 1);
    rtdb->getContainer("NeulandHitPar");
    rtdb->setInputVersion(999, (char*)"NeulandHitPar", 1, 1);
      
    auto losMapped2Cal=new R3BLosMapped2Cal("LosTCalPar", 1);
    losMapped2Cal->SetNofModules(1,8);
    losMapped2Cal->SetTrigger(1);
    run->AddTask( losMapped2Cal );
    
    auto tcal = new R3BNeulandMapped2Cal();
    tcal->SetTrigger(trigger);
    tcal->SetNofModules(nPlanes, nBarsPerPlane);
    tcal->SetNhitmin(1);
    tcal->EnableWalk(true);
    //tcal->SubtractTriggerTime(false);
    run->AddTask(tcal);

    run->AddTask(new R3BLosProvideTStart());
   
    auto nlhit = new R3BNeulandCal2Hit();
    nlhit->SetDistanceToTarget(distanceToTarget);
    nlhit->SetGlobalTimeOffset(-400);//5850
    nlhit->SetEnergyCutoff(0.0);
    run->AddTask(nlhit);
    
    // auto r3bNeulandOnlineSpectra = new R3BNeulandOnlineSpectra();
    // r3bNeulandOnlineSpectra->SetDistanceToTarget(distanceToTarget);
    // r3bNeulandOnlineSpectra->SetCosmicTpat(0x2000);
    // run->AddTask(r3bNeulandOnlineSpectra);
    
    // Go!
    // -------------------------------------------
    run->Init();
    FairLogger::GetLogger()->SetLogScreenLevel("ERROR");
    run->Run((nev < 0) ? nev : 0, (nev < 0) ? 0 : nev);

    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << endl << endl;
    cout << "Macro finished succesfully." << endl;
    cout << "Output file is " << outputFileName << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl << endl;
}
