void eventDisplay()
{
    const TString input_file = "test.digi.root";
    const TString par_file = "test.para.root";

    FairRunAna *fRun= new FairRunAna();

    FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
    FairParRootFileIo*  parIo1 = new FairParRootFileIo();
    parIo1->open(par_file);
    rtdb->setFirstInput(parIo1);
    rtdb->print();

    fRun->SetSource(new FairFileSource(input_file));
    fRun->SetOutputFile("test.root");

    FairEventManager *fMan= new FairEventManager();
    FairMCTracks *Track =  new FairMCTracks ("Monte-Carlo Tracks");
    // FairMCTracksDraw *Track =  new FairMCTracksDraw ("Monte-Carlo Tracks");
    FairMCPointDraw *LandPoints =   new FairMCPointDraw ("NeulandPoints",kOrange,  kFullSquare);

    fMan->AddTask(Track);

    fMan->AddTask(LandPoints);

    fMan->Init();
}
