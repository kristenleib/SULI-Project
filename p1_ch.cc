#include <TLorentzVector.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TH1I.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <iostream>
#include <cmath>

// Function to compute the invariant mass of two particles
double invariantMass(const TLorentzVector &p1, const TLorentzVector &p2) {
    TLorentzVector p = p1 + p2;
    return p.M();
}

void analyzeLambdaPairs() {
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

    float p1_pt, p1_eta, p1_phi;
    float p2_pt, p2_eta, p2_phi;
    Int_t pair_charge, p1_ch;

    ntp_Lambda->SetBranchAddress("p1_pt", &p1_pt);
    ntp_Lambda->SetBranchAddress("p1_eta", &p1_eta);
    ntp_Lambda->SetBranchAddress("p1_phi", &p1_phi);
    ntp_Lambda->SetBranchAddress("p2_pt", &p2_pt);
    ntp_Lambda->SetBranchAddress("p2_eta", &p2_eta);
    ntp_Lambda->SetBranchAddress("p2_phi", &p2_phi);
    ntp_Lambda->SetBranchAddress("pair_charge", &pair_charge);
    ntp_Lambda->SetBranchAddress("p1_ch", &p1_ch);

    TH1F *h1_unlike = new TH1F("h1_unlike", "Invariant Mass of Unlike-Sign Pairs (GeV/c^2)", 300, 1, 1.4);
    TH1F *h1_like = new TH1F("h1_like", "Invariant Mass of Like-Sign Pairs (GeV/c^2)", 300, 1, 1.4);
    TH1F *h1 = new TH1F("h1", "Invariant Mass (GeV/c^2)", 300, 1, 1.4);
    TH1I *h1_p1_ch = new TH1I("h1_p1_ch", "Charge of Proton p1", 100, -2, 2);

    int nEntries = ntp_Lambda->GetEntries();

    for (int i = 0; i < nEntries; i++) {
        ntp_Lambda->GetEntry(i);

        // Proton mass in GeV/c^2
        float mass_p1 = 0.938272;
        // Pion mass in GeV/c^2
        float mass_p2 = 0.139570;

        TLorentzVector p1, p2;
        p1.SetPtEtaPhiM(p1_pt, p1_eta, p1_phi, mass_p1);
        p2.SetPtEtaPhiM(p2_pt, p2_eta, p2_phi, mass_p2);

        double invariant_mass = invariantMass(p1, p2);

        h1->Fill(invariant_mass);
        h1_p1_ch->Fill(p1_ch);

        if (pair_charge == 0) {
            h1_unlike->Fill(invariant_mass);
        } else {
            h1_like->Fill(invariant_mass);
        }
    }

    // Create a histogram for the difference data
    TH1F *h1_diff = new TH1F(*h1_unlike); // Clone the unlike-sign pairs histogram

    // Subtract histograms
    h1_diff->Add(h1_like, -1);

    TCanvas *c1 = new TCanvas("c1", "Lambda Calculation", 1200, 800);
    c1->Divide(2,2);

    // Pad 1: Both like-sign and unlike-sign pairs on the same histogram
    c1->cd(1);
    c1->GetPad(1)->SetGrid();
    h1->SetLineColor(kBlue-3);
    h1->Draw();
    h1->SetXTitle("Invariant Mass (GeV/c^2)");
    h1->SetLineWidth(3);

    h1_unlike->SetLineColor(kBlue-3);
    h1_unlike->Draw("SAME");
    h1_like->SetLineColor(kRed+1);
    h1_like->Draw("SAME");
    h1_unlike->SetXTitle("Invariant Mass (GeV/c^2)");
    h1_unlike->SetLineWidth(3);
    h1_like->SetLineWidth(3);

    // Add a legend to Pad 1 to distinguish between the histograms
    TLegend *leg = new TLegend(0.7, 0.7, 0.9, 0.9);
    leg->AddEntry(h1_unlike, "Unlike-Sign Pairs", "l");
    leg->AddEntry(h1_like, "Like-Sign Pairs", "l");
    leg->Draw();

    // Pad 2: Invariant mass difference (unlike - like)
    c1->cd(2);
    c1->GetPad(2)->SetGrid();
    h1_diff->SetLineColor(kMagenta+2);
    h1_diff->Draw();
    h1_diff->SetXTitle("Invariant Mass Difference (GeV/c^2)");
    h1_diff->SetLineWidth(3);

    // Pad 3: Charge of Proton p1
    c1->cd(3);
    c1->GetPad(3)->SetGrid();
    h1_p1_ch->SetLineColor(kGreen+2);
    h1_p1_ch->Draw();
    h1_p1_ch->SetXTitle("Charge of Proton p1");
    h1_p1_ch->SetLineWidth(3);

    c1->Update();
    c1->WaitPrimitive();

    f->Close();
}

int main() {
    analyzeLambdaPairs();
    return 0;
}
