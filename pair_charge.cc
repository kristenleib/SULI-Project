void pair_charge(){
    TFile *f = new TFile("output_6.root", "READ");

    // Check if the file was opened successfully
    if (!f || f->IsZombie()) {
        std::cerr << "Error: Could not open file 'output_6.root'." << std::endl;
        return;
    }

    gDirectory->ls("-d");

    TTree *ntp_Lambda = (TTree*) f->Get("ntp_Lambda");

    // Check if the tree was retrieved successfully
    if (!ntp_Lambda) {
        std::cerr << "Error: Could not retrieve tree 'ntp_Lambda' from file 'output_6.root'." << std::endl;
        f->Close();
        delete f;
        return;
    }

    TH1I *h1 = new TH1I("hpair_charge","pair_charge", 100, 0, 2);
    h1->SetDirectory(nullptr);

    Int_t pair_charge;
    ntp_Lambda->SetBranchAddress("pair_charge", &pair_charge);

    Long64_t nEntries = ntp_Lambda->GetEntries();
    for (Long64_t i=0; i<nEntries; i++){
        ntp_Lambda->GetEntry(i);
        h1->Fill(pair_charge);
    }

    TCanvas *c1 = new  TCanvas("c1","pair_charge");
    c1->cd();
    c1->ToggleEventStatus();
    h1->Draw();
    h1->SetXTitle("pair_charge");
    h1->SetLineWidth(2);
    f->Close();
}