{
    FairLogger::GetLogger()->SetLogScreenLevel("INFO");
    auto file = new TFile{"params_tcal_180522xx.root"};
    TString name = "R3BTCalPar";
    TIter next{file->GetListOfKeys()};
    TKey* key;           
    R3BTCalPar* calPar;
    while ((key = dynamic_cast<TKey*>(next()))) {
        if(strcmp(key->GetClassName(), name.Data()) == 0)
        calPar = dynamic_cast<R3BTCalPar*>(key->ReadObj());
    }
    // calPar->printParams();


    // auto CalParams = calPar->GetListOfModulePar();
    // for(auto it = CalParams->begin(); it != CalParams->end(); ++it){
    //     (dynamic_cast<R3BTCalModulePar*>(it())) -> printParams();
    //  
    // }
    auto Param = calPar->GetModuleParAt(13, 5, 2);
    Param->printParams();

    for(int i = 0; i < Param->GetNofChannels(); i++){
        cout << i << ": " << Param->GetBinLowAt(i) << "\t"<< Param->GetOffsetAt(i) << endl;
    }
}
