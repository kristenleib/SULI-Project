#include <TLorentzVector.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <iostream>
#include <cmath>

// THIS ONE SAVES TO THE ROOT TREE

// Function to compute the invariant mass of two particles
double invariantMass(const TLorentzVector &p1, const TLorentzVector &p2) {
    TLorentzVector p = p1 + p2;
    return p.M();
}

void lambda_calculations() {
    TFile *f = new TFile("output_6.root", "UPDATE");

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
    float pair_mass;

    ntp_Lambda->SetBranchAddress("p1_pt", &p1_pt);
    ntp_Lambda->SetBranchAddress("p1_eta", &p1_eta);
    ntp_Lambda->SetBranchAddress("p1_phi", &p1_phi);
    ntp_Lambda->SetBranchAddress("p2_pt", &p2_pt);
    ntp_Lambda->SetBranchAddress("p2_eta", &p2_eta);
    ntp_Lambda->SetBranchAddress("p2_phi", &p2_phi);
    ntp_Lambda->SetBranchAddress("pair_mass", &pair_mass);

    // Create a new branch for invariant mass
    double invariant_mass;
    TBranch *newBranch = ntp_Lambda->Branch("invariant_mass", &invariant_mass, "invariant_mass/D");

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

        invariant_mass = invariantMass(p1, p2);
        newBranch->Fill();
    }

    // Save the updated tree to the file
    ntp_Lambda->Write("", TObject::kOverwrite);

    f->Close();
}

int main() {
    lambda_calculations();
    return 0;
}