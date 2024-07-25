#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <iostream>

void lambda_lambdabar() {
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

    Double_t invariant_mass;

    ntp_Lambda->SetBranchAddress("invariant_mass", &invariant_mass);

    TH1D *h_lambda = new TH1D("h_lambda", "Invariant Mass of Lambda (GeV/c^2)", 300, 1, 1.4);
    TH1D *h_lbar = new TH1D("h_lbar", "Invariant Mass of Lbar (GeV/c^2)", 300, 1, 1.4);

    int nEntries = ntp_Lambda->GetEntries();

    for (int i = 0; i < nEntries; i++) {
        ntp_Lambda->GetEntry(i);

        if (invariant_mass < 1.4 && invariant_mass > 1.0) { // Ensure invariant mass is within reasonable range
            if (invariant_mass < 1.115) {
                // Lambda candidates
                h_lambda->Fill(invariant_mass);
            } else {
                // Lbar candidates
                h_lbar->Fill(invariant_mass);
            }
        }
    }

    TCanvas *c1 = new TCanvas("c1", "Lambda Calculation", 800, 600);
    c1->Divide(2, 1);

    c1->cd(1);
    h_lambda->Draw();
    h_lambda->SetXTitle("Invariant Mass (GeV/c^2)");
    h_lambda->SetLineColor(kBlue);
    h_lambda->SetLineWidth(3);

    c1->cd(2);
    h_lbar->Draw();
    h_lbar->SetXTitle("Invariant Mass (GeV/c^2)");
    h_lbar->SetLineColor(kRed);
    h_lbar->SetLineWidth(3);

    c1->SaveAs("lambda_lbar_distributions.png");

    f->Close();
    delete f;
}

int main() {
    lambda_lambdabar();
    return 0;
}