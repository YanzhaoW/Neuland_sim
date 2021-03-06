void digi1(){
    TStopwatch timer;
    timer.Start();

    const TString workDirectory = getenv("VMCWORKDIR");
    gSystem->Setenv("GEOMPATH", workDirectory + "/geometry");
    gSystem->Setenv("CONFIG_DIR", workDirectory + "/gconfig");
    FairLogger::GetLogger()->SetLogScreenLevel("DEBUG4");

    FairRunAna run;
    run.SetSource(new FairFileSource("test.simu.root"));
    run.SetSink(new FairRootFileSink("test.digi.root"));

    auto io = new FairParRootFileIo();
    io->open("test.para.root");           
    auto runtimeDb = run.GetRuntimeDb();
    runtimeDb->setFirstInput(io);

    run.AddTask(new R3BNeulandDigitizer());

    run.Init();
    run.Run(0,0);


    std::cout << "*************" << std::endl;
    timer.Stop(); 
    std::cout << "Macro finished successfully." << std::endl;
    std::cout << "Real time: " << timer.RealTime() << "s, CPU time: " << timer.CpuTime() << "s" << std::endl;
}
