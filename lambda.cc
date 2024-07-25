void lambda(){
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

    TH1F *h1 = new TH1F("h1", "Invariant Mass", 200, 0, 1);
    h1->SetDirectory(nullptr);

    Float_t p1_pt, p2_pt, p1_eta, p2_eta, p1_phi, p2_phi;
    ntp_Lambda->SetBranchAddress("p1_pt",&p1_pt);
    ntp_Lambda->SetBranchAddress("p2_pt",&p2_pt);
    ntp_Lambda->SetBranchAddress("p1_eta",&p1_eta);
    ntp_Lambda->SetBranchAddress("p2_eta",&p2_eta);
    ntp_Lambda->SetBranchAddress("p1_phi",&p1_phi);
    ntp_Lambda->SetBranchAddress("p2_phi",&p2_phi);

    Long64_t nentries = ntp_Lambda->GetEntries();
    for (Long64_t i = 0; i < nentries; i++) {
        ntp_Lambda->GetEntry(i);
        // Calculate M using the equation
        Double_t M_squared = 2 * p1_pt * p2_pt * (cosh(p1_eta - p2_eta) - cos(p1_phi - p2_phi));
        Double_t M = sqrt(M_squared);
        // Add M to histogram
        //h1->Fill(M_squared);
        h1->Fill(M);
    }

    // Calculate and print the integral
    Double_t integral = h1->Integral();
    std::cout << "Area Under the Curve: " << integral << std::endl;


    TCanvas *c1 = new  TCanvas("c1","Invariant Mass");
    c1->cd();
    c1->ToggleEventStatus();
    h1->Draw();
    h1->SetXTitle("Invariant Mass (GeV/c^2)");
    h1->SetLineWidth(2);
    f->Close();
}