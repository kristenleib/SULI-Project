#include <TLorentzVector.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TCanvas.h>
#include <iostream>
#include <vector>
#include <TStyle.h>

using namespace std;

// Function to compute the invariant mass of two particles
double invariantMass(const TLorentzVector &p1, const TLorentzVector &p2) {
    TLorentzVector p = p1 + p2;
    return p.M();
}

void lpair_check5(){

    // Open ROOT file
    TFile *f = TFile::Open("output_1.root", "READ");
    if (!f || f->IsZombie()) {
        cerr << "Error: Could not open file 'output_1.root'." << endl;
        return;
    }

    // Retrieve TTree
    TTree *ntp_Lambda = dynamic_cast<TTree*>(f->Get("ntp_Lambda"));
    if (!ntp_Lambda) {
        cerr << "Error: Could not retrieve tree 'ntp_Lambda' from file 'output_1.root'." << endl;
        return;
    }

    // Variables for tree branches
    float p1_pt, p1_eta, p1_phi;
    float p2_pt, p2_eta, p2_phi;
    Int_t pair_charge, p1_ch;
    Int_t p1_InEventID, p2_InEventID;
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

    // lambda_ls vectors
    vector<TLorentzVector> lambda_ls;
    vector<Int_t> proton_id_lambda_ls;
    vector<Int_t> pion_id_lambda_ls;

    // lambda_us vectors
    vector<TLorentzVector> lambda_us;
    vector<Int_t> proton_id_lambda_us;
    vector<Int_t> pion_id_lambda_us;

    // lambda_bar_ls vectors
    vector<TLorentzVector> lambda_bar_ls;
    vector<Int_t> anti_proton_id_lambda_bar_ls;
    vector<Int_t> anti_pion_id_lambda_bar_ls;

    // lambda_bar_us vectors
    vector<TLorentzVector> lambda_bar_us;
    vector<Int_t> anti_proton_id_lambda_bar_us;
    vector<Int_t> anti_pion_id_lambda_bar_us;

    // The six histograms will go here
    TH2F *h2_lambda_us_lambda_us = new TH2F("h2_lambda_us_lambda_us", "Unlike Sign Lambda - Unlike Sign Lambda (cd1)", 100, 1.0, 1.4, 100, 1.0, 1.4);
    TH2F *h2_lambda_us_lambda_ls = new TH2F("h2_lambda_us_lambda_ls", "Unlike Sign Lambda - Like Sign Lambda (cd4)", 100, 1.0, 1.4, 100, 1.0, 1.4);
    TH2F *h2_lambda_us_lambda_bar_us = new TH2F("h2_lambda_us_lambda_bar_us", "Unlike Sign Lambda - Unlike Sign Lambda Bar (cd2)", 100, 1.0, 1.4, 100, 1.0, 1.4);
    TH2F *h2_lambda_us_lambda_bar_ls = new TH2F("h2_lambda_us_lambda_bar_ls", "Unlike Sign Lambda - Like Sign Lambda Bar (cd5)", 100, 1.0, 1.4, 100, 1.0, 1.4);
    TH2F *h2_lambda_bar_us_lambda_bar_us = new TH2F("h2_lambda_bar_us_lambda_bar_us", "Unlike Sign Lambda Bar - Unlike Sign Lambda Bar (cd3)", 100, 1.0, 1.4, 100, 1.0, 1.4);
    TH2F *h2_lambda_bar_us_lambda_ls = new TH2F("h2_lambda_bar_us_lambda_ls", "Unlike Sign Lambda Bar - Like Sign Lambda (cd6)", 100, 1.0, 1.4, 100, 1.0, 1.4);

    const int nEntries = ntp_Lambda->GetEntries();
    for (int i = 0; i < nEntries; ++i){
        ntp_Lambda->GetEntry(i);

        // Skip pairing if p1_InEventID equals p2_InEventID
        if (p1_InEventID == p2_InEventID)
            continue;

        // Fill TLorentzVector with current event particles
        TLorentzVector p1, p2;
        p1.SetPtEtaPhiM(p1_pt, p1_eta, p1_phi, mass_p1);
        p2.SetPtEtaPhiM(p2_pt, p2_eta, p2_phi, mass_p2);

        TLorentzVector fourmomentum = p1 + p2;
                // Pair charge check
        if (pair_charge == 0) {
            // Un-like sign pairs
            if (p1_ch > 0) {
                // Lambda pair
                lambda_us.push_back(fourmomentum);
                proton_id_lambda_us.push_back(p1_InEventID);
                pion_id_lambda_us.push_back(p2_InEventID);
            } else if (p1_ch < 0) {
                // Lambda-bar pair
                lambda_bar_us.push_back(fourmomentum);
                anti_proton_id_lambda_bar_us.push_back(p1_InEventID);
                anti_pion_id_lambda_bar_us.push_back(p2_InEventID);
            }
        } else {
            // Like-sign pairs
            if (p1_ch > 0) {
                // Lambda pair
                lambda_ls.push_back(fourmomentum);
                proton_id_lambda_ls.push_back(p1_InEventID);
                pion_id_lambda_ls.push_back(p2_InEventID);
            } else if (p1_ch < 0) {
                // Lambda-bar pair
                lambda_bar_ls.push_back(fourmomentum);
                anti_proton_id_lambda_bar_ls.push_back(p1_InEventID);
                anti_pion_id_lambda_bar_ls.push_back(p2_InEventID);
            }
        }

        if (eventId != prevEventID && prevEventID != -1){
            int q = 0;

            // lambda - lambda pairing loop
            for (size_t j = 0; j < lambda_us.size(); ++j) {
                for (size_t k = j + 1; k < lambda_us.size(); ++k) {
                    if (proton_id_lambda_us[j] == proton_id_lambda_us[k] || pion_id_lambda_us[j] == pion_id_lambda_us[k])
                        continue;
                    h2_lambda_us_lambda_us->Fill(lambda_us[j].M(), lambda_us[k].M());
                }

                for (size_t k = 0; k < lambda_ls.size(); ++k) {
                    if (proton_id_lambda_us[j] == proton_id_lambda_ls[k] || pion_id_lambda_us[j] == pion_id_lambda_ls[k])
                        continue;

                    if (q % 2 == 0){
                        h2_lambda_us_lambda_ls->Fill(lambda_us[j].M(), lambda_ls[k].M());
                    } else {
                        h2_lambda_us_lambda_ls->Fill(lambda_us[j].M(), lambda_ls[k].M());
                    }

                    q++; // increase the value by 1
                }
            }

            // lambda - lambda bar pairing loop
            for (size_t j = 0; j < lambda_us.size(); ++j) {
                for (size_t k = 0; k < lambda_bar_us.size(); ++k) {
                    if (proton_id_lambda_us[j] == anti_proton_id_lambda_bar_us[k] || pion_id_lambda_us[j] == anti_pion_id_lambda_bar_us[k])
                        continue;
                    h2_lambda_us_lambda_bar_us->Fill(lambda_us[j].M(), lambda_bar_us[k].M());
                }

                for (size_t k = 0; k < lambda_bar_ls.size(); ++k){
                    if (proton_id_lambda_us[j] == anti_proton_id_lambda_bar_ls[k] || pion_id_lambda_us[j] == anti_pion_id_lambda_bar_ls[k])
                        continue;
                    h2_lambda_us_lambda_bar_ls->Fill(lambda_us[j].M(), lambda_bar_ls[k].M());
                }
            }

            // lambda bar - lambda bar pairing loop
            for (size_t j = 0; j < lambda_bar_us.size(); ++j) {
                for (size_t k = j + 1; k < lambda_bar_us.size(); ++k) {
                    if (anti_proton_id_lambda_bar_us[j] == anti_proton_id_lambda_bar_us[k] || anti_pion_id_lambda_bar_us[j] == anti_pion_id_lambda_bar_us[k])
                        continue;
                    h2_lambda_bar_us_lambda_bar_us->Fill(lambda_bar_us[j].M(), lambda_bar_us[k].M());
                }

                for (size_t k = 0; k < lambda_ls.size(); ++k){
                    if (anti_proton_id_lambda_bar_us[j] == anti_proton_id_lambda_bar_ls[k] || anti_pion_id_lambda_bar_us[j] == anti_pion_id_lambda_bar_ls[k])
                        continue;

                    if (q % 2 == 0){
                        h2_lambda_bar_us_lambda_bar_ls->Fill(lambda_bar_us[j].M(), lambda_bar_ls[k].M());
                    } else {
                        h2_lambda_bar_us_bar_lambda_ls->Fill(lambda_bar_us[j].M(), lambda_bar_ls[k].M());
                    }

                    q++; // increase the value by 1
                }
            }

            lambda_us.clear();
            proton_id_lambda_us.clear();
            pion_id_lambda_us.clear();

            lambda_ls.clear();
            proton_id_lambda_ls.clear();
            pion_id_lambda_ls.clear();

            lambda_bar_us.clear();
            anti_proton_id_lambda_bar_us.clear();
            anti_pion_id_lambda_bar_us.clear();

            lambda_bar_ls.clear();
            anti_proton_id_lambda_bar_ls.clear();
            anti_pion_id_lambda_bar_ls.clear();

            // end of the pairing loop
        }

        // end of main for loop
    }

    // end of macro
}