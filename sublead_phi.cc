void sublead_phi(){
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

    TH1F *h1 = new TH1F("hSublead_phi","sublead_phi", 100, -3.5, 3.5);
    h1->SetDirectory(nullptr);

    Float_t sublead_phi;
    ntp_Lambda->SetBranchAddress("sublead_phi", &sublead_phi);

    Long64_t nEntries = ntp_Lambda->GetEntries();
    for (Long64_t i=0; i<nEntries; i++){
        ntp_Lambda->GetEntry(i);
        h1->Fill(sublead_phi);
    }

    TCanvas *c1 = new  TCanvas("c1","sublead_phi");
    c1->cd();
    c1->ToggleEventStatus();
    h1->Draw();
    h1->SetXTitle("sublead_phi");
    h1->SetLineWidth(2);
    f->Close();
}

int main() {
    sublead_phi();
    return 0;
}