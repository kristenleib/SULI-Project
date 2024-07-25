#include <TLorentzVector.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TCanvas.h>
#include <iostream>
#include <vector>
#include <memory>
#include <TStyle.h>

// Function to compute the invariant mass of two particles
double invariantMass(const TLorentzVector &p1, const TLorentzVector &p2) {
    TLorentzVector p = p1 + p2;
    return p.M();
}

void l_pairs3() {
    // Open ROOT file
    std::unique_ptr<TFile> f(TFile::Open("output_1.root", "READ"));
    if (!f || f->IsZombie()) {
        std::cerr << "Error: Could not open file 'output_1.root'." << std::endl;
        return;
    }

    // Retrieve TTree
    TTree *ntp_Lambda = dynamic_cast<TTree*>(f->Get("ntp_Lambda"));
    if (!ntp_Lambda) {
        std::cerr << "Error: Could not retrieve tree 'ntp_Lambda' from file 'output_1.root'." << std::endl;
        return;
    }

    // Variables for tree branches
    float p1_pt, p1_eta, p1_phi;
    float p2_pt, p2_eta, p2_phi;
    Int_t pair_charge, p1_ch;
    Int_t p1_InEventID, p2_InEventID; // Event IDs for particles
    Int_t eventId, prevEventID = -1;

    // Set branch addresses
    ntp_Lambda->SetBranchAddress("p1_pt", &p1_pt);
    ntp_Lambda->SetBranchAddress("p1_eta", &p1_eta);
    ntp_Lambda->SetBranchAddress("p1_phi", &p1_phi);
    ntp_Lambda->SetBranchAddress("p2_pt", &p2_pt);
    ntp_Lambda->SetBranchAddress("p2_eta", &p2_eta);
    ntp_Lambda->SetBranchAddress("p2_phi", &p2_phi);
    ntp_Lambda->SetBranchAddress("pair_charge", &pair_charge);
    ntp_Lambda->SetBranchAddress("p1_ch", &p1_ch);
    ntp_Lambda->SetBranchAddress("p1_InEventID", &p1_InEventID);
    ntp_Lambda->SetBranchAddress("p2_InEventID", &p2_InEventID);
    ntp_Lambda->SetBranchAddress("eventId", &eventId);

    // Constants for particle masses
    const float mass_p1 = 0.938272;  // Proton mass
    const float mass_p2 = 0.139570;  // Pion mass

    // Vectors to store TLorentzVectors and particle IDs
    std::vector<TLorentzVector> lambdas;
    std::vector<Int_t> proton_ids;
    std::vector<Int_t> pion_ids;

    std::vector<TLorentzVector> lambda_bars;
    std::vector<Int_t> anti_proton_ids;
    std::vector<Int_t> anti_pion_ids;

    // Create 1D histograms for invariant masses
    std::unique_ptr<TH1F> h1_lambda_mass(new TH1F("h1_lambda_mass", "Lambda Invariant Mass", 300, 1.0, 1.4));
    std::unique_ptr<TH1F> h1_lambda_bar_mass(new TH1F("h1_lambda_bar_mass", "Lambda-bar Invariant Mass", 300, 1.0, 1.4));

    // Create 2D histograms for pairings based on invariant masses
    std::unique_ptr<TH2F> h2_lambda_lambda(new TH2F("h2_lambda_lambda", "Lambda-Lambda Invariant Mass vs Invariant Mass", 300, 1.0, 1.4, 300, 1.0, 1.4));
    std::unique_ptr<TH2F> h2_lambda_lambda_bar(new TH2F("h2_lambda_lambda_bar", "Lambda-Lambda-bar Invariant Mass vs Invariant Mass", 300, 1.0, 1.4, 300, 1.0, 1.4));
    std::unique_ptr<TH2F> h2_lambda_bar_lambda_bar(new TH2F("h2_lambda_bar_lambda_bar", "Lambda-bar-Lambda-bar Invariant Mass vs Invariant Mass", 300, 1.0, 1.4, 300, 1.0, 1.4));

    // Loop over entries in the TTree
    const int nEntries = ntp_Lambda->GetEntries();
    for (int i = 0; i < nEntries; ++i) {
        ntp_Lambda->GetEntry(i);

        // Skip pairing if p1_InEventID equals p2_InEventID
        if (p1_InEventID == p2_InEventID)
            continue;

        // Fill TLorentzVector with current event particles
        TLorentzVector p1, p2;
        p1.SetPtEtaPhiM(p1_pt, p1_eta, p1_phi, mass_p1);
        p2.SetPtEtaPhiM(p2_pt, p2_eta, p2_phi, mass_p2);

        double mass = invariantMass(p1, p2);

        if (p1_ch > 0) {
            lambdas.push_back(p1);
            proton_ids.push_back(p1_InEventID);
            pion_ids.push_back(p2_InEventID);
            h1_lambda_mass->Fill(mass);
        } else if (p1_ch < 0) {
            lambda_bars.push_back(p1);
            anti_proton_ids.push_back(p1_InEventID);
            anti_pion_ids.push_back(p2_InEventID);
            h1_lambda_bar_mass->Fill(mass);
        }

        // Process pairs when encountering a new eventID
        if (eventId != prevEventID && prevEventID != -1) {
            for (size_t j = 0; j < lambdas.size(); ++j) {
                for (size_t k = j + 1; k < lambdas.size(); ++k) {
                    // Check IDs before filling histograms
                    if (proton_ids[j] == proton_ids[k] || pion_ids[j] == pion_ids[k])
                        continue;

                    double mass1 = invariantMass(lambdas[j], lambdas[k]);
                    h2_lambda_lambda->Fill(mass1, mass1);
                }

                for (size_t k = 0; k < lambda_bars.size(); ++k) {
                    // Check IDs before filling histograms
                    if (proton_ids[j] == anti_proton_ids[k] || pion_ids[j] == anti_pion_ids[k])
                        continue;

                    double mass1 = invariantMass(lambdas[j], lambda_bars[k]);
                    h2_lambda_lambda_bar->Fill(mass1, mass1);
                }
            }

            for (size_t j = 0; j < lambda_bars.size(); ++j) {
                for (size_t k = j + 1; k < lambda_bars.size(); ++k) {
                    // Check IDs before filling histograms
                    if (anti_proton_ids[j] == anti_proton_ids[k] || anti_pion_ids[j] == anti_pion_ids[k])
                        continue;

                    double mass1 = invariantMass(lambda_bars[j], lambda_bars[k]);
                    h2_lambda_bar_lambda_bar->Fill(mass1, mass1);
                }
            }

            // Clear vectors for new event
            lambdas.clear();
            proton_ids.clear();
            pion_ids.clear();
            lambda_bars.clear();
            anti_proton_ids.clear();
            anti_pion_ids.clear();
        }

        prevEventID = eventId;
    }

    // Process the last event's pairs
    for (size_t j = 0; j < lambdas.size(); ++j) {
        for (size_t k = j + 1; k < lambdas.size(); ++k) {
            // Check IDs before filling histograms
            if (proton_ids[j] == proton_ids[k] || pion_ids[j] == pion_ids[k])
                continue;

            double mass1 = invariantMass(lambdas[j], lambdas[k]);
            h2_lambda_lambda->Fill(mass1, mass1);
        }

        for (size_t k = 0; k < lambda_bars.size(); ++k) {
            // Check IDs before filling histograms
            if (proton_ids[j] == anti_proton_ids[k] || pion_ids[j] == anti_pion_ids[k])
                continue;

            double mass1 = invariantMass(lambdas[j], lambda_bars[k]);
            h2_lambda_lambda_bar->Fill(mass1, mass1);
        }
    }

    for (size_t j = 0; j < lambda_bars.size(); ++j) {
        for (size_t k = j + 1; k < lambda_bars.size(); ++k) {
            // Check IDs before filling histograms
            if (anti_proton_ids[j] == anti_proton_ids[k] || anti_pion_ids[j] == anti_pion_ids[k])
                continue;

            double mass1 = invariantMass(lambda_bars[j], lambda_bars[k]);
            h2_lambda_bar_lambda_bar->Fill(mass1, mass1);
        }
    }

    // Create TCanvas for displaying histograms
    std::unique_ptr<TCanvas> c1(new TCanvas("c1", "1D Histograms", 1200, 400));
    c1->Divide(2, 1);

    std::unique_ptr<TCanvas> c2(new TCanvas("c2", "2D Histograms", 1200, 400));
    c2->Divide(3, 1);

    std::unique_ptr<TCanvas> c3(new TCanvas("c3", "3D Histograms", 1200, 400));
    c3->Divide(3, 1);

    // Drawing 1D histograms on c1
    c1->cd(1);
    h1_lambda_mass->Draw();

    c1->cd(2);
    h1_lambda_bar_mass->Draw();

    // Drawing 2D histograms on c2
    c2->cd(1);
    h2_lambda_lambda->Draw("COLZ");

    c2->cd(2);
    h2_lambda_lambda_bar->Draw("COLZ");

    c2->cd(3);
    h2_lambda_bar_lambda_bar->Draw("COLZ");

    // Drawing 3D histograms on c3
    c3->cd(1);
    h2_lambda_lambda->Draw("SURF2");

    c3->cd(2);
    h2_lambda_lambda_bar->Draw("SURF2");

    c3->cd(3);
    h2_lambda_bar_lambda_bar->Draw("SURF2");

    // Update canvases
    c1->Update();
    c2->Update();
    c3->Update();

    // Wait for user input or handle events
    c1->WaitPrimitive();
    c2->WaitPrimitive();
    c3->WaitPrimitive();

    // Close file
    f->Close();
}