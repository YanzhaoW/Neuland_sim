{
    // FairLogger::GetLogger()->SetLogScreenLevel("INFO");
    // auto file = new TFile{"params_tcal_180522xx.root"};
    // TString name = "R3BTCalPar";
    // TIter next{file->GetListOfKeys()};
    // TKey* key;            
    // R3BTCalPar* calPar;
    // while ((key = dynamic_cast<TKey*>(next()))) {
    //     if(strcmp(key->GetClassName(), name.Data()) == 0)
    //     calPar = dynamic_cast<R3BTCalPar*>(key->ReadObj());
    // }
    // // calPar->printParams();


    // // auto CalParams = calPar->GetListOfModulePar();
    // // for(auto it = CalParams->begin(); it != CalParams->end(); ++it){
    // //     (dynamic_cast<R3BTCalModulePar*>(it())) -> printParams();
    // //   
    // // }
    // auto Param = calPar->GetModuleParAt(25, 36, 1);
    // Param->printParams();

    // for(int i = 0; i < Param->GetNofChannels(); i++){
    //     cout << i << ": " << Param->GetBinLowAt(i) << "\t"<< Param->GetOffsetAt(i) << endl;
    // }
}


// // peak for params_hit
// {

//     FairLogger::GetLogger()->SetLogScreenLevel("INFO");
//     auto file = new TFile{"params_sync_s522_0999_310522x.root"};
//     TString name = "R3BNeulandHitPar";
//     TIter next{file->GetListOfKeys()};
//     TKey* key;              
//     R3BNeulandHitPar* hitPar;
//     while ((key = dynamic_cast<TKey*>(next()))) {
//         cout << key->GetClassName() << endl;

//         if(strcmp(key->GetClassName(), name.Data()) == 0)
//         hitPar = dynamic_cast<R3BNeulandHitPar*>(key->ReadObj());
//     }
//     // hitPar->printParams();

//     // const auto nPars = hitPar->GetNumModulePar();

//     // for (auto i = 0; i < nPars; i++){
//     //     const auto id = hitPar->GetModuleParAt(i)->GetModuleId() - 1;
//     //     std::cout << id << '\t';

//     //     // fParMap[id] = *hitPar->GetModuleParAt(i);
//     //     // fAttenuationValues[id] = exp(Neuland::TotalBarLength / fParMap[id].GetLightAttenuationLength());
//     // }

//     auto hitParams = hitPar->GetListOfModulePar();
//     for(auto it = hitParams->begin(); it != hitParams->end(); it){
//         // (dynamic_cast<R3BNeulandHitModulePar*>(it())) -> printParams();
//         cout << (dynamic_cast<R3BNeulandHitModulePar*>(it())) -> GetModuleId() << '\t';
//     
//     }
//     // auto Param = calPar->GetModuleParAt(25, 36, 1);
//     // Param->printParams();

//     // for(int i = 0; i < Param->GetNofChannels(); i++){
//     //     cout << i << ": " << Param->GetBinLowAt(i) << "\t"<< Param->GetOffsetAt(i) << endl;
//     // }


// }
