void pair_decayL(){
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

    TH1F *h1 = new TH1F("hpair_decayL","pair_decayL", 100, 0, 26);
    h1->SetDirectory(nullptr);

    Float_t pair_decayL;
    ntp_Lambda->SetBranchAddress("pair_decayL", &pair_decayL);

    Long64_t nEntries = ntp_Lambda->GetEntries();
    for (Long64_t i=0; i<nEntries; i++){
        ntp_Lambda->GetEntry(i);
        h1->Fill(pair_decayL);
    }

    TCanvas *c1 = new  TCanvas("c1","pair_decayL");
    c1->cd();
    c1->ToggleEventStatus();
    h1->Draw();
    h1->SetXTitle("pair_decayL");
    h1->SetLineWidth(2);
    f->Close();
}

int main() {
    pair_decayL();
    return 0;
}