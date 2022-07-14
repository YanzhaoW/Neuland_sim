{
    auto file = new TFile("build/output.root");
    TTree *t = nullptr;
    file->GetObject("evt", t);
    auto b = t->GetListOfBranches();
    for(const auto it : *b){
        cout << it->GetName() << endl;
        cout << "split level: " << ((TBranch*)it)->GetSplitLevel() << endl;
        auto sb = ((TBranch*)it)->GetListOfBranches();
        for(const auto itt: * sb){
            cout << itt->GetName() << endl;
        }
        // auto leaves = ((TBranch*)it)->GetListOfLeaves();
        // cout << "how many leaves: " << leaves->GetEntries() << endl;
        // for(const auto leaf : *leaves){
        //     cout << leaf->GetName()<< endl;
        // }
    }





    // TClonesArray* hits = new TClonesArray("R3BNeulandHit");
    // t->SetBranchAddress("NeulandHits", &hits);

    // R3BNeulandHit* hit;
    // cout << t->GetEntries() << endl;
    // for(int i = 0; i < 10000; i++){
    //     t->GetEntry(i);
    //     // cout << "entry: "<< hits->GetEntriesFast() << endl;
    //     if(hits->GetEntriesFast() != 0){
    //         hit = (R3BNeulandHit*)hits->At(0);
    //         cout << "paddle: " << hit->GetPaddle() << endl;
    //     }

    // }

    TClonesArray* hits ;
    t->SetBranchAddress("NeulandHits", &hits);

    TObject* hit;
    cout << t->GetEntries() << endl;
    for(int i = 0; i < 10000; i++){
        t->GetEntry(i);
        cout << "entry: "<< hits->GetEntriesFast() << endl;
        if(hits->GetEntriesFast() != 0){
            hit = hits->At(0);
            cout << hit->ClassName() << endl;
            // cout << "paddle: " << hit->GetPaddle() << endl;
        }

    }


    
}
