
void WriteModule2Csv(R3BNeulandHitPar* pars)
{
    auto ofile = ofstream("HitModulePar.csv", std::ofstream::trunc);

    ofile << "moduleId,tDiff,tSync,timeOffsetL,timeOffsetR,effectiveSpeed,pedestalL,pedestalR,energyGainL,energyGainR,lightAtt,PMTSatL,PMTSatR,PMTThresL,PMTThresR" << endl;
    TIter next2{pars->GetListOfModulePar()};

    while(auto obj = dynamic_cast<R3BNeulandHitModulePar*>(next2()))
    {
        ofile <<obj->GetModuleId()  << "," 
            << obj->GetTDiff() << ","
            << obj->GetTSync() << ","
            << obj->GetTimeOffset(2) << ","
            << obj->GetTimeOffset(1) << ","
            << obj->GetEffectiveSpeed() << ","
            << obj->GetPedestal(2) << ","
            << obj->GetPedestal(1) << ","
            << obj->GetEnergyGain(2) << ","
            << obj->GetEnergyGain(1) << ","
            << obj->GetLightAttenuationLength() << ","
            << obj->GetPMTSaturation(2) << ","
            << obj->GetPMTSaturation(1) << ","
            << obj->GetPMTThreshold(2) << ","
            << obj->GetPMTThreshold(1)
            << endl;
    }

    ofile.close();
}


int Cal2Hit()
{
    TString filepath = "../../parameters/";
    TString filename = "params_sync_s522_0999_310522x.root";
    TString parName = "NeulandHitPar";
    auto parfile = new TFile(filepath + filename);
    if (!parfile)
        cout << "file cannot be read! " << endl;

    
    auto keys = parfile->GetListOfKeys();

    TIter next{keys};

    while(auto key = static_cast<TKey*>(next()))
    {
        auto obj = key->ReadObj();
        if(obj->GetName() == parName)
        {
            auto hitpars = dynamic_cast<R3BNeulandHitPar*>(obj);

            if(!hitpars)
                continue;
            WriteModule2Csv(hitpars);

            
        }

    }



    parfile->Close();
    return 0;
}
