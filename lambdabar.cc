void lambdabar() {
    TFile *f = new TFile("output_6.root", "READ");

    // Check if the file was opened successfully
    if (!f || f->IsZombie()) {
        std::cerr << "Error: Could not open file 'output_6.root'." << std::endl;
        return;
    }

    TTree *ntp_Lambda = (TTree*) f->Get("ntp_Lambda");

    // Check if the tree was retrieved successfully
    if (!ntp_Lambda) {
        std::cerr << "Error: Could not retrieve tree 'ntp_Lambda' from file 'output_6.root'." << std::endl;
        f->Close();
        delete f;
        return;
    }

    float p1_pt, p1_eta, p1_phi;
    float p2_pt, p2_eta, p2_phi;

    ntp_Lambda->SetBranchAddress("p1_pt", &p1_pt);
    ntp_Lambda->SetBranchAddress("p1_eta", &p1_eta);
    ntp_Lambda->SetBranchAddress("p1_phi", &p1_phi);
    ntp_Lambda->SetBranchAddress("p2_pt", &p2_pt);
    ntp_Lambda->SetBranchAddress("p2_eta", &p2_eta);
    ntp_Lambda->SetBranchAddress("p2_phi", &p2_phi);

    TH1F *h_lambda = new TH1F("h_lambda", "Invariant Mass of Lambda (GeV/c^2)", 300, 1, 1.4);
    TH1F *h_lambda_bar = new TH1F("h_lambda_bar", "Invariant Mass of Lambda Bar (GeV/c^2)", 300, 1, 1.4);

    int nEntries = ntp_Lambda->GetEntries();

    for (int i = 0; i < nEntries; i++) {
        ntp_Lambda->GetEntry(i);

        float mass_p1 = 0.938272; // Proton mass in GeV/c^2 (anti-proton mass)
        float mass_p2 = 0.139570; // Pion mass in GeV/c^2 (positive pion mass)

        TLorentzVector p1, p2;
        p1.SetPtEtaPhiM(p1_pt, p1_eta, p1_phi, mass_p1);
        p2.SetPtEtaPhiM(p2_pt, p2_eta, p2_phi, mass_p2);

        double invariant_mass_lambda = invariantMass(p1, p2);
        double invariant_mass_lambda_bar = invariantMass(p2, p1); // Note the order is switched for anti-Lambda

        h_lambda->Fill(invariant_mass_lambda);
        h_lambda_bar->Fill(invariant_mass_lambda_bar);
    }

    TCanvas *c1 = new TCanvas("c1", "Lambda and Lambda Bar Calculation");
    c1->Divide(2,1);

    c1->cd(1);
    h_lambda->Draw();
    h_lambda->SetLineColor(kBlue);
    h_lambda->SetLineWidth(3);
    h_lambda->SetXTitle("Invariant Mass (GeV/c^2)");

    c1->cd(2);
    h_lambda_bar->Draw();
    h_lambda_bar->SetLineColor(kRed);
    h_lambda_bar->SetLineWidth(3);
    h_lambda_bar->SetXTitle("Invariant Mass (GeV/c^2)");

    c1->Update();
    c1->WaitPrimitive();
    
    f->Close();
}

int main() {
    lambdabar();
    return 0;
}