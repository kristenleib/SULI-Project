#include <TLorentzVector.h>
#include <TChain.h>
#include <TTree.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <iostream>
#include <vector>
#include <TStyle.h>
#include <cmath>

using namespace std;

// Function to compute the invariant mass of two particles
double invariantMass(const TLorentzVector &p1, const TLorentzVector &p2) {
    TLorentzVector p = p1 + p2;
    return p.M();
}

void final_lpair_signal_pp(){
    // Create a TChain
    TChain *ntp_Lambda = new TChain("ntp_Lambda");

    // Add ROOT files to the TChain
    ntp_Lambda->Add("output_1.root");
    ntp_Lambda->Add("output_2.root");
    ntp_Lambda->Add("output_3.root");
    ntp_Lambda->Add("output_4.root");
    ntp_Lambda->Add("output_5.root");
    ntp_Lambda->Add("output_6.root");

    // Check if the TChain has been successfully created and files added
    if (ntp_Lambda->GetEntries() == 0) {
        cerr << "Error: Could not retrieve entries from TChain 'ntp_Lambda'." << endl;
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
    vector<TLorentzVector> proton_ls;
    vector<Int_t> proton_id_lambda_ls;
    vector<Int_t> pion_id_lambda_ls;

    // lambda_us vectors
    vector<TLorentzVector> lambda_us;
    vector<TLorentzVector> proton_us;
    vector<Int_t> proton_id_lambda_us;
    vector<Int_t> pion_id_lambda_us;

    // lambda_bar_ls vectors
    vector<TLorentzVector> lambda_bar_ls;
    vector<TLorentzVector> anti_proton_ls;
    vector<Int_t> anti_proton_id_lambda_bar_ls;
    vector<Int_t> anti_pion_id_lambda_bar_ls;

    // lambda_bar_us vectors
    vector<TLorentzVector> lambda_bar_us;
    vector<TLorentzVector> anti_proton_us;
    vector<Int_t> anti_proton_id_lambda_bar_us;
    vector<Int_t> anti_pion_id_lambda_bar_us;

    // Four Momenta Vectors
    vector<TLorentzVector> fourmoment_Lus_Lus;
    vector<TLorentzVector> fourmoment_proton_us_1;
    vector<TLorentzVector> fourmoment_proton_us_2;
    vector<TLorentzVector> fourmoment_lambda_us_1;
    vector<TLorentzVector> fourmoment_lambda_us_2;

    vector<TLorentzVector> fourmoment_Lus_Lls;
    vector<TLorentzVector> fourmoment_proton_us3;
    vector<TLorentzVector> fourmoment_proton_ls3;
    vector<TLorentzVector> fourmoment_lambda_us3;
    vector<TLorentzVector> fourmoment_lambda_ls3;

    vector<TLorentzVector> fourmoment_Lus_LBarus;
    vector<TLorentzVector> fourmoment_proton_us4;
    vector<TLorentzVector> fourmoment_anti_proton_us4;
    vector<TLorentzVector> fourmoment_lambda_us4;
    vector<TLorentzVector> fourmoment_lambda_bar_us4;

    vector<TLorentzVector> fourmoment_Lus_LBarls;
    vector<TLorentzVector> fourmoment_proton_us5;
    vector<TLorentzVector> fourmoment_anti_proton_ls5;
    vector<TLorentzVector> fourmoment_lambda_us5;
    vector<TLorentzVector> fourmoment_lambda_bar_ls5;

    vector<TLorentzVector> fourmoment_Lls_LBarus;
    vector<TLorentzVector> fourmoment_proton_ls6;
    vector<TLorentzVector> fourmoment_anti_proton_us6;
    vector<TLorentzVector> fourmoment_lambda_ls6;
    vector<TLorentzVector> fourmoment_lambda_bar_us6;

    vector<TLorentzVector> fourmoment_LBarus_LBarus;
    vector<TLorentzVector> fourmoment_anti_proton_us_1;
    vector<TLorentzVector> fourmoment_anti_proton_us_2;
    vector<TLorentzVector> fourmoment_lambda_bar_us_1;
    vector<TLorentzVector> fourmoment_lambda_bar_us_2;

    vector<TLorentzVector> fourmoment_LBarus_LBarls;
    vector<TLorentzVector> fourmoment_anti_proton_us7;
    vector<TLorentzVector> fourmoment_anti_proton_ls7;
    vector<TLorentzVector> fourmoment_lambda_bar_us7;
    vector<TLorentzVector> fourmoment_lambda_bar_ls7;

    // Invariant Mass Histograms before Cuts
    TH2F *h2_lambda_us_lambda_us = new TH2F("h2_lambda_us_lambda_us", "Unlike Sign Lambda - Unlike Sign Lambda (Includes Background)", 500, 1.0, 1.4, 500, 1.0, 1.4);
    TH2F *h2_lambda_us_lambda_ls = new TH2F("h2_lambda_us_lambda_ls", "Unlike Sign Lambda - Like Sign Lambda (Background)", 500, 1.0, 1.4, 500, 1.0, 1.4);
    TH2F *h2_lambda_us_lambda_bar_us = new TH2F("h2_lambda_us_lambda_bar_us", "Unlike Sign Lambda - Unlike Sign Lambda Bar (Includes Background)", 500, 1.0, 1.4, 500, 1.0, 1.4);
    TH2F *h2_lambda_us_lambda_bar_ls = new TH2F("h2_lambda_us_lambda_bar_ls", "Unlike Sign Lambda - Like Sign Lambda Bar (Background)", 500, 1.0, 1.4, 500, 1.0, 1.4);
    TH2F *h2_lambda_bar_us_lambda_bar_us = new TH2F("h2_lambda_bar_us_lambda_bar_us", "Unlike Sign Lambda Bar - Unlike Sign Lambda Bar (Includes Background)", 500, 1.0, 1.4, 500, 1.0, 1.4);
    TH2F *h2_lambda_bar_us_lambda_bar_ls = new TH2F("h2_lambda_bar_us_lambda_ls", "Unlike Sign Lambda Bar - Like Sign Lambda Bar (Background)", 500, 1.0, 1.4, 500, 1.0, 1.4);
    
    // Invariant Mass Histograms after Cuts
    TH2F *h2_invMass_lambda_us_lambda_us = new TH2F("h2_invMass_lambda_us_lambda_us", "Unlike Sign Lambda - Unlike Sign Lambda (test)", 500, 1.0, 1.4, 500, 1.0, 1.4);
    TH2F *h2_invMass_lambda_us_lambda_ls = new TH2F("h2_invMass_lambda_us_lambda_ls", "Unlike Sign Lambda - Like Sign Lambda (test2)", 500, 1.0, 1.4, 500, 1.0, 1.4);
    TH2F *h2_invMass_lambda_us_lambda_bar_us = new TH2F("h2_invMass_lambda_us_lambda_bar_us", "Unlike Sign Lambda - Unlike Sign Lambda Bar (test)", 500, 1.0, 1.4, 500, 1.0, 1.4);
    TH2F *h2_invMass_lambda_us_lambda_bar_ls = new TH2F("h2_invMass_lambda_us_lambda_bar_ls", "Unlike Sign Lambda - Like Sign Lambda Bar (test2)", 500, 1.0, 1.4, 500, 1.0, 1.4);
    TH2F *h2_invMass_lambda_bar_us_lambda_bar_us = new TH2F("h2_invMass_lambda_bar_us_lambda_bar_us", "Unlike Sign Lambda Bar - Unlike Sign Lambda Bar (test)", 500, 1.0, 1.4, 500, 1.0, 1.4);
    TH2F *h2_invMass_lambda_bar_us_lambda_bar_ls = new TH2F("h2_invMass_lambda_bar_us_lambda_bar_ls", "Unlike Sign Lambda Bar - Like Sign Lambda Bar (test2)", 500, 1.0, 1.4, 500, 1.0, 1.4);

    // Histograms for the Angluar Distribution (Cos(theta*))
    TH1D *h_cos_theta_star = new TH1D("h_cos_theta_star", "Angular Distribution for Unlike Sign Lambda - Unlike Sign Lambda (cos_theta_star*)", 10, -1, 1);
    TH1D *h_cos_theta_star2 = new TH1D("h_cos_theta_star2", "Angular Distribution for Unlike Sign Lambda - Like Sign Lambda (cos_theta_star*)", 10, -1, 1);
    TH1D *h_cos_theta_star3 = new TH1D("h_cos_theta_star3", "Angular Distribution for Unlike Sign Lambda - Unlike Sign Lambda Bar (cos_theta_star*)", 10, -1, 1);
    TH1D *h_cos_theta_star4 = new TH1D("h_cos_theta_star4", "Angular Distribution for Unlike Sign Lambda - Like Sign Lambda Bar (cos_theta_star*)", 10, -1, 1);
    TH1D *h_cos_theta_star5 = new TH1D("h_cos_theta_star5", "Angular Distribution for Unlike Sign Lambda Bar - Unlike Sign Lambda Bar (cos_theta_star*)", 10, -1, 1);
    TH1D *h_cos_theta_star6 = new TH1D("h_cos_theta_star6", "Angular Distribution for Unlike Sign Lambda Bar - Like Sign Lambda Bar (cos_theta_star*)", 10, -1, 1);

    // Define histograms for Delta R distributions
    TH1D *h_delta_R_US_US_ll = new TH1D("h_delta_R_US_US_ll", "Delta R Distribution for US-US Lambda - Lambda Pairs", 50, 0, 5);
    TH1D *h_delta_R_US_LS_ll = new TH1D("h_delta_R_US_LS_ll", "Delta R Distribution for US-LS Lambda - Lambda Pairs", 50, 0, 5);
    TH1D *h_delta_R_US_US_llbar = new TH1D("h_delta_R_US_US_llbar", "Delta R Distribution for US-US Lambda - Lambda Bar Pairs", 50, 0, 5);
    TH1D *h_delta_R_US_LS_llbar = new TH1D("h_delta_R_US_LS_llbar", "Delta R Distribution for US-LS Lambda - Lambda Bar Pairs", 50, 0, 5);
    TH1D *h_delta_R_US_US_lbarlbar = new TH1D("h_delta_R_US_US_lbarlbar", "Delta R Distribution for US-US Lambda Bar - Lambda Bar Pairs", 50, 0, 5);
    TH1D *h_delta_R_US_LS_lbarlbar = new TH1D("h_delta_R_US_LS_lbarlbar", "Delta R Distribution for US-LS Lambda Bar - Lambda Bar Pairs", 50, 0, 5);

    // Histograms for Delta Phi Distributions
    TH1D *h_delta_phi_US_US_ll = new TH1D("h_delta_phi_US_US_ll", "Delta Phi for US-US Lambda - Lambda Pairs", 50, 0, TMath::Pi());
    TH1D *h_delta_phi_US_LS_ll = new TH1D("h_delta_phi_US_LS_ll", "Delta Phi for US-LS Lambda - Lambda Pairs", 50, 0, TMath::Pi());
    TH1D *h_delta_phi_US_US_llbar = new TH1D("h_delta_phi_US_US_llbar", "Delta Phi for US-US Lambda - Lambda Bar Pairs", 50, 0, TMath::Pi());
    TH1D *h_delta_phi_US_LS_llbar = new TH1D("h_delta_phi_US_LS_llbar", "Delta Phi for US-LS Lambda - Lambda Bar Pairs", 50, 0, TMath::Pi());
    TH1D *h_delta_phi_US_US_lbarlbar = new TH1D("h_delta_phi_US_US_lbarlbar", "Delta Phi for US-US Lambda Bar - Lambda Bar Pairs", 50, 0, TMath::Pi());
    TH1D *h_delta_phi_US_LS_lbarlbar = new TH1D("h_delta_phi_US_LS_lbarlbar", "Delta Phi for US-LS Lambda Bar - Lambda Bar Pairs", 50, 0, TMath::Pi());

    // This main loop is taking the daughter information (p1 (proton) and p2 (pion)) and pairing in the invariant mass distributions for Lambda - Lambda, Lambda - Lambda Bar, and 
    // Lambda Bar - Lambda Bar pairs. This results in 6 distributions that are subsquently used to extract the signal: US Lambda - US Lambda, US Lambda - LS Lambda, US Lambda - US Lambda Bar,
    // US Lambda - LS Lambda Bar, Ls Lambda - US Lambda, US Lambda Bar - US Lambda Bar, and US Lambda Bar - LS Lambda Bar.

    // Main for loop that Fills the Vector Pairs
    int q = 0;
    int b = 0;

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
                proton_us.push_back(p1);
                proton_id_lambda_us.push_back(p1_InEventID);
                pion_id_lambda_us.push_back(p2_InEventID);
            } else if (p1_ch < 0) {
                // Lambda-bar pair
                lambda_bar_us.push_back(fourmomentum);
                anti_proton_us.push_back(p1);
                anti_proton_id_lambda_bar_us.push_back(p1_InEventID);
                anti_pion_id_lambda_bar_us.push_back(p2_InEventID);
            }
        } else {
            // Like-sign pairs
            if (p1_ch > 0) {
                // Lambda pair
                lambda_ls.push_back(fourmomentum);
                proton_ls.push_back(p1);
                proton_id_lambda_ls.push_back(p1_InEventID);
                pion_id_lambda_ls.push_back(p2_InEventID);
            } else if (p1_ch < 0) {
                // Lambda-bar pair
                lambda_bar_ls.push_back(fourmomentum);
                anti_proton_ls.push_back(p1);
                anti_proton_id_lambda_bar_ls.push_back(p1_InEventID);
                anti_pion_id_lambda_bar_ls.push_back(p2_InEventID);
            }
        }

        // Pairing the Lambdas and Lambda Bars
        if (eventId != prevEventID && prevEventID != -1){
            for (size_t j = 0; j < lambda_us.size(); ++j) {
                for (size_t k = j + 1; k < lambda_us.size(); ++k) {
                    if (proton_id_lambda_us[j] == proton_id_lambda_us[k] || pion_id_lambda_us[j] == pion_id_lambda_us[k])
                        continue;

                    h2_lambda_us_lambda_us->Fill(lambda_us[j].M(), lambda_us[k].M());
                    fourmoment_lambda_us_1.push_back(lambda_us[j]);
                    fourmoment_lambda_us_2.push_back(lambda_us[k]);
                    fourmoment_proton_us_1.push_back(proton_us[j]);
                    fourmoment_proton_us_2.push_back(proton_us[k]);
                }

                for (size_t k = 0; k < lambda_ls.size(); ++k) {
                    if (proton_id_lambda_us[j] == proton_id_lambda_ls[k] || pion_id_lambda_us[j] == pion_id_lambda_ls[k])
                        continue;

                    if (q % 2 == 0){
                        h2_lambda_us_lambda_ls->Fill(lambda_us[j].M(), lambda_ls[k].M());
                    } else {
                        h2_lambda_us_lambda_ls->Fill(lambda_ls[k].M(), lambda_us[j].M());
                    }

                    fourmoment_lambda_us3.push_back(lambda_us[j]);
                    fourmoment_lambda_ls3.push_back(lambda_ls[k]);
                    fourmoment_proton_us3.push_back(proton_us[j]);
                    fourmoment_proton_ls3.push_back(proton_ls[k]);

                    q++; // increase the value by 1
                }
            }

            for (size_t j = 0; j < lambda_us.size(); ++j) {
                for (size_t k = 0; k < lambda_bar_us.size(); ++k) {
                    if (proton_id_lambda_us[j] == anti_proton_id_lambda_bar_us[k] || pion_id_lambda_us[j] == anti_pion_id_lambda_bar_us[k])
                        continue;

                    h2_lambda_us_lambda_bar_us->Fill(lambda_us[j].M(), lambda_bar_us[k].M());

                    fourmoment_lambda_us4.push_back(lambda_us[j]);
                    fourmoment_lambda_bar_us4.push_back(lambda_bar_us[k]);
                    fourmoment_proton_us4.push_back(proton_us[j]);
                    fourmoment_anti_proton_us4.push_back(anti_proton_us[k]);
                }

                for (size_t k = 0; k < lambda_bar_ls.size(); ++k){
                    if (proton_id_lambda_us[j] == anti_proton_id_lambda_bar_ls[k] || pion_id_lambda_us[j] == anti_pion_id_lambda_bar_ls[k])
                        continue;

                    h2_lambda_us_lambda_bar_ls->Fill(lambda_us[j].M(), lambda_bar_ls[k].M());

                    fourmoment_lambda_us5.push_back(lambda_us[j]);
                    fourmoment_lambda_bar_ls5.push_back(lambda_bar_ls[k]);
                    fourmoment_proton_us5.push_back(proton_us[j]);
                    fourmoment_anti_proton_ls5.push_back(anti_proton_ls[k]);
                }
            }

            for (size_t j = 0; j < lambda_ls.size(); ++j){
                for (size_t k = 0; k < lambda_bar_us.size(); ++k){
                    if (proton_id_lambda_ls[j] == anti_proton_id_lambda_bar_us[k] || pion_id_lambda_ls[j] == anti_pion_id_lambda_bar_us[k])
                        continue;

                    h2_lambda_us_lambda_bar_ls->Fill(lambda_ls[j].M(), lambda_bar_us[k].M());

                    fourmoment_lambda_ls6.push_back(lambda_ls[j]);
                    fourmoment_lambda_bar_us6.push_back(lambda_bar_us[k]);
                    fourmoment_proton_ls6.push_back(proton_ls[j]);
                    fourmoment_anti_proton_us6.push_back(anti_proton_us[k]);
                }
            }

            for (size_t j = 0; j < lambda_bar_us.size(); ++j) {
                for (size_t k = j + 1; k < lambda_bar_us.size(); ++k) {
                    if (anti_proton_id_lambda_bar_us[j] == anti_proton_id_lambda_bar_us[k] || anti_pion_id_lambda_bar_us[j] == anti_pion_id_lambda_bar_us[k])
                        continue;

                    h2_lambda_bar_us_lambda_bar_us->Fill(lambda_bar_us[j].M(), lambda_bar_us[k].M());
                    fourmoment_lambda_bar_us_1.push_back(lambda_bar_us[j]);
                    fourmoment_lambda_bar_us_2.push_back(lambda_bar_us[k]);
                    fourmoment_anti_proton_us_1.push_back(anti_proton_us[j]);
                    fourmoment_anti_proton_us_2.push_back(anti_proton_us[k]);
                }

                for (size_t k = 0; k < lambda_ls.size(); ++k){
                    if (anti_proton_id_lambda_bar_us[j] == anti_proton_id_lambda_bar_ls[k] || anti_pion_id_lambda_bar_us[j] == anti_pion_id_lambda_bar_ls[k])
                        continue;

                    if (b % 2 == 0){
                        h2_lambda_bar_us_lambda_bar_ls->Fill(lambda_bar_us[j].M(), lambda_bar_ls[k].M());
                    } else {
                        h2_lambda_bar_us_lambda_bar_ls->Fill(lambda_bar_ls[k].M(), lambda_bar_us[j].M());
                    }

                    fourmoment_lambda_bar_us7.push_back(lambda_bar_us[j]);
                    fourmoment_lambda_bar_ls7.push_back(lambda_bar_ls[k]);
                    fourmoment_anti_proton_us7.push_back(anti_proton_us[j]);
                    fourmoment_anti_proton_ls7.push_back(anti_proton_ls[k]);

                    b++; // increase the value by 1
                }
            }

            lambda_us.clear();
            proton_us.clear();
            proton_id_lambda_us.clear();
            pion_id_lambda_us.clear();

            lambda_ls.clear();
            proton_ls.clear();
            proton_id_lambda_ls.clear();
            pion_id_lambda_ls.clear();

            lambda_bar_us.clear();
            anti_proton_us.clear();
            anti_proton_id_lambda_bar_us.clear();
            anti_pion_id_lambda_bar_us.clear();

            lambda_bar_ls.clear();
            anti_proton_ls.clear();
            anti_proton_id_lambda_bar_ls.clear();
            anti_pion_id_lambda_bar_ls.clear();
        }

        prevEventID = eventId;
    }

    // Background subtractions (US-LS is being subtracted from US-US) for Invariant Mass Distributions
    TH2F *h2_LL_Signal = new TH2F(*h2_lambda_us_lambda_us);
    h2_LL_Signal->Add(h2_lambda_us_lambda_ls, -1.0);

    TH2F *h2_LLBar_Signal = new TH2F(*h2_lambda_us_lambda_bar_us);
    h2_LLBar_Signal->Add(h2_lambda_us_lambda_bar_ls, -1.0);

    TH2F *h2_LBarLBar_Signal = new TH2F(*h2_lambda_bar_us_lambda_bar_us);
    h2_LBarLBar_Signal->Add(h2_lambda_bar_us_lambda_bar_ls, -1.0);

    gStyle->SetPalette(kPastel);
    gStyle->SetOptStat(0);

    // Canvas 1: Invariant Mass Distributions (Signal) and Gaussiant Fit of the Signal
    std::unique_ptr<TCanvas> c1(new TCanvas("c1", "2D Histograms", 1800, 900));
    c1->Divide(3, 2);

    double minX = 1.11;
    double maxX = 1.12;
    double minY = 1.11;
    double maxY = 1.12;

    c1->cd(1);
    int binMinX = h2_LL_Signal->GetXaxis()->FindBin(minX);
    int binMaxX = h2_LL_Signal->GetXaxis()->FindBin(maxX);
    int binMinY = h2_LL_Signal->GetYaxis()->FindBin(minY);
    int binMaxY = h2_LL_Signal->GetYaxis()->FindBin(maxY);

    h2_LL_Signal->GetXaxis()->SetRange(binMinX, binMaxX);
    h2_LL_Signal->GetYaxis()->SetRange(binMinY, binMaxY);
    h2_LL_Signal->Draw("SURF2");
    h2_LL_Signal->SetTitle("Lambda - Lambda Pair (Signal)");

    TF2 *gaus2D_LL = new TF2("gaus2D_LL", "[0]*TMath::Gaus(x,[1],[2])*TMath::Gaus(y,[3],[4])", minX, maxX, minY, maxY);
    gaus2D_LL->SetParameters(190.681, 1.11571, 0.001478, 1.11571, 0.001369);
    h2_LL_Signal->Fit("gaus2D_LL", "R0");

    c1->cd(4);
    gaus2D_LL->Draw("SURF2");
    gaus2D_LL->SetTitle("Gaussian fit for Lambda - Lambda Pairs");

    c1->cd(2);
    binMinX = h2_LLBar_Signal->GetXaxis()->FindBin(minX);
    binMaxX = h2_LLBar_Signal->GetXaxis()->FindBin(maxX);
    binMinY = h2_LLBar_Signal->GetYaxis()->FindBin(minY);
    binMaxY = h2_LLBar_Signal->GetYaxis()->FindBin(maxY);

    h2_LLBar_Signal->GetXaxis()->SetRange(binMinX, binMaxX);
    h2_LLBar_Signal->GetYaxis()->SetRange(binMinY, binMaxY);
    h2_LLBar_Signal->Draw("SURF2");
    h2_LLBar_Signal->SetTitle("Lambda - Lambda Bar Pair (Signal)");

    TF2 *gaus2D_LLBar = new TF2("gaus2D_LLBar", "[0]*TMath::Gaus(x,[1],[2])*TMath::Gaus(y,[3],[4])", minX, maxX, minY, maxY);
    gaus2D_LLBar->SetParameters(360.198, 1.11573, 0.00137157, 1.11574, 0.00139988);
    h2_LLBar_Signal->Fit("gaus2D_LLBar", "R0");

    c1->cd(5);
    gaus2D_LLBar->Draw("SURF2");
    gaus2D_LLBar->SetTitle("Gaussian fit for Lambda - Lambda Bar Pairs");

    c1->cd(3);
    binMinX = h2_LBarLBar_Signal->GetXaxis()->FindBin(minX);
    binMaxX = h2_LBarLBar_Signal->GetXaxis()->FindBin(maxX);
    binMinY = h2_LBarLBar_Signal->GetYaxis()->FindBin(minY);
    binMaxY = h2_LBarLBar_Signal->GetYaxis()->FindBin(maxY);

    h2_LBarLBar_Signal->GetXaxis()->SetRange(binMinX, binMaxX);
    h2_LBarLBar_Signal->GetYaxis()->SetRange(binMinY, binMaxY);
    h2_LBarLBar_Signal->Draw("SURF2");
    h2_LBarLBar_Signal->SetTitle("Lambda Bar - Lambda Bar Pair (Signal)");

    TF2 *gaus2D_LBarLBar = new TF2("gaus2D_LBarLBar", "[0]*TMath::Gaus(x,[1],[2])*TMath::Gaus(y,[3],[4])", minX, maxX, minY, maxY);
    gaus2D_LBarLBar->SetParameters(146.732, 1.11578, 0.00143841, 1.11565, 0.00123124);
    h2_LBarLBar_Signal->Fit("gaus2D_LBarLBar", "R0");

    c1->cd(6);
    gaus2D_LBarLBar->Draw("SURF2");
    gaus2D_LBarLBar->SetTitle("Gaussian fit for Lambda Bar - Lambda Bar Pairs");

    // Canvas 2: Invariant Mass Projections of the x axis and y axis
    std::unique_ptr<TCanvas> c2(new TCanvas("c2", "X-Y Projections of the 2D histograms", 1800, 900));
    c2->Divide(3, 2);

    c2->cd(1);
    TH1D *projX_LL = h2_LL_Signal->ProjectionX("projX_LL", binMinX, binMaxX);
    projX_LL->GetXaxis()->SetRange(binMinX, binMaxX);
    projX_LL->SetLineColor(kBlue+2);
    projX_LL->SetLineWidth(3);
    projX_LL->Draw();
    projX_LL->SetTitle("Lambda - Lambda Signal (x axis)");

    int MeanX_LL = h2_LL_Signal->GetXaxis()->FindBin(gaus2D_LL->GetParameter(3));
    TH1D *meanX_LL = h2_LL_Signal->ProjectionX("meanX_LL", MeanX_LL, MeanX_LL);
    meanX_LL->SetLineColor(kViolet-2);
    meanX_LL->SetLineWidth(3);
    meanX_LL->Draw("same");

    TF12 *proj_gausX_LL = new TF12("proj_gausX_LL", gaus2D_LL, gaus2D_LL->GetParameter(3), "x");
    proj_gausX_LL->SetLineColor(kRed+1);
    proj_gausX_LL->SetLineWidth(3);
    proj_gausX_LL->Draw("same");

    c2->cd(2);
    TH1D *projX_LLBar = h2_LLBar_Signal->ProjectionX("projX_LLBar", binMinX, binMaxX);
    projX_LLBar->GetXaxis()->SetRange(binMinX, binMaxX);
    projX_LLBar->SetLineColor(kBlue+2);
    projX_LLBar->SetLineWidth(3);
    projX_LLBar->Draw();
    projX_LLBar->SetTitle("Lambda - Lambda Bar Signal (x axis)");

    int MeanX_LLBar = h2_LLBar_Signal->GetXaxis()->FindBin(gaus2D_LLBar->GetParameter(3));
    TH1D *meanX_LLBar = h2_LLBar_Signal->ProjectionX("meanX_LLBar", MeanX_LLBar, MeanX_LLBar);
    meanX_LLBar->SetLineColor(kViolet-2);
    meanX_LLBar->SetLineWidth(3);
    meanX_LLBar->Draw("same");

    TF12 *proj_gausX_LLBar = new TF12("proj_gausX_LLBar", gaus2D_LLBar, gaus2D_LLBar->GetParameter(3), "x");
    proj_gausX_LLBar->SetLineColor(kRed+1);
    proj_gausX_LLBar->SetLineWidth(3);
    proj_gausX_LLBar->Draw("same");

    c2->cd(3);
    TH1D *projX_LBarLBar = h2_LBarLBar_Signal->ProjectionX("projX_LBarLBar", binMinX, binMaxX);
    projX_LBarLBar->GetXaxis()->SetRange(binMinX, binMaxX);
    projX_LBarLBar->SetLineColor(kBlue+2);
    projX_LBarLBar->SetLineWidth(3);
    projX_LBarLBar->Draw();
    projX_LBarLBar->SetTitle("Lambda Bar - Lambda Bar Signal (x axis)");

    int MeanX_LBarLBar = h2_LBarLBar_Signal->GetXaxis()->FindBin(gaus2D_LBarLBar->GetParameter(3));
    TH1D *meanX_LBarLBar = h2_LBarLBar_Signal->ProjectionX("meanX_LBarLBar", MeanX_LBarLBar, MeanX_LBarLBar);
    meanX_LBarLBar->SetLineColor(kViolet-2);
    meanX_LBarLBar->SetLineWidth(3);
    meanX_LBarLBar->Draw("same");

    TF12 *proj_gausX_LBarLBar = new TF12("proj_gausX_LBarLBar", gaus2D_LBarLBar, gaus2D_LBarLBar->GetParameter(3), "x");
    proj_gausX_LBarLBar->SetLineColor(kRed+1);
    proj_gausX_LBarLBar->SetLineWidth(3);
    proj_gausX_LBarLBar->Draw("same");

    c2->cd(4);
    TH1D *projY_LL = h2_LL_Signal->ProjectionY("projY_LL", binMinY, binMaxY);
    projY_LL->GetYaxis()->SetRange(binMinY, binMaxY);
    projY_LL->SetLineColor(kBlue+2);
    projY_LL->SetLineWidth(3);
    projY_LL->Draw();
    projY_LL->SetTitle("Lambda - Lambda Signal (y axis)");

    int MeanY_LL = h2_LL_Signal->GetYaxis()->FindBin(gaus2D_LL->GetParameter(1));
    TH1D *meanY_LL = h2_LL_Signal->ProjectionY("meanY_LL", MeanY_LL, MeanY_LL);
    meanY_LL->SetLineColor(kViolet-2);
    meanY_LL->SetLineWidth(3);
    meanY_LL->Draw("same");

    TF12 *proj_gausY_LL = new TF12("proj_gausY_LL", gaus2D_LL, gaus2D_LL->GetParameter(1), "y");
    proj_gausY_LL->SetLineColor(kRed+1);
    proj_gausY_LL->SetLineWidth(3);
    proj_gausY_LL->Draw("same");

    c2->cd(5);
    TH1D *projY_LLBar = h2_LLBar_Signal->ProjectionY("projY_LLBar", binMinY, binMaxY);
    projY_LLBar->GetYaxis()->SetRange(binMinY, binMaxY);
    projY_LLBar->SetLineColor(kBlue+2);
    projY_LLBar->SetLineWidth(3);
    projY_LLBar->Draw();
    projY_LLBar->SetTitle("Lambda - Lambda Bar Signal (y axis)");

    int MeanY_LLBar = h2_LLBar_Signal->GetYaxis()->FindBin(gaus2D_LLBar->GetParameter(1));
    TH1D *meanY_LLBar = h2_LLBar_Signal->ProjectionY("meanY_LLBar", MeanY_LLBar, MeanY_LLBar);
    meanY_LLBar->SetLineColor(kViolet-2);
    meanY_LLBar->SetLineWidth(3);
    meanY_LLBar->Draw("same");

    TF12 *proj_gausY_LLBar = new TF12("proj_gausY_LLBar", gaus2D_LLBar, gaus2D_LLBar->GetParameter(1), "y");
    proj_gausY_LLBar->SetLineColor(kRed+1);
    proj_gausY_LLBar->SetLineWidth(3);
    proj_gausY_LLBar->Draw("same");

    c2->cd(6);
    TH1D *projY_LBarLBar = h2_LBarLBar_Signal->ProjectionY("projY_LBarLBar", binMinY, binMaxY);
    projY_LBarLBar->GetYaxis()->SetRange(binMinY, binMaxY);
    projY_LBarLBar->SetLineColor(kBlue+2);
    projY_LBarLBar->SetLineWidth(3);
    projY_LBarLBar->Draw();
    projY_LBarLBar->SetTitle("Lambda Bar - Lambda Bar Signal (y axis)");

    int MeanY_LBarLBar = h2_LBarLBar_Signal->GetYaxis()->FindBin(gaus2D_LBarLBar->GetParameter(1));
    TH1D *meanY_LBarLBar = h2_LBarLBar_Signal->ProjectionY("meanY_LBarLBar", MeanY_LBarLBar, MeanY_LBarLBar);
    meanY_LBarLBar->SetLineColor(kViolet-2);
    meanY_LBarLBar->SetLineWidth(3);
    meanY_LBarLBar->Draw("same");

    TF12 *proj_gausY_LBarLBar = new TF12("proj_gausY_LBarLBar", gaus2D_LBarLBar, gaus2D_LBarLBar->GetParameter(1), "y");
    proj_gausY_LBarLBar->SetLineColor(kRed+1);
    proj_gausY_LBarLBar->SetLineWidth(3);
    proj_gausY_LBarLBar->Draw("same");

    // ± 3 sigma check and polarization calculations for Lambda - Lambda Pairs
    double meanX_LL_2 = gaus2D_LL->GetParameter(1);
    double sigmaX_LL_2 = gaus2D_LL->GetParameter(2);
    double meanY_LL_2 = gaus2D_LL->GetParameter(3);
    double sigmaY_LL_2 = gaus2D_LL->GetParameter(4);

    double minX_range_LL_2 = meanX_LL_2 - 3.0 * sigmaX_LL_2;
    double maxX_range_LL_2 = meanX_LL_2 + 3.0 * sigmaX_LL_2;
    double minY_range_LL_2 = meanY_LL_2 - 3.0 * sigmaY_LL_2;
    double maxY_range_LL_2 = meanY_LL_2 + 3.0 * sigmaY_LL_2;

    for (size_t i = 0; i < fourmoment_lambda_us_1.size(); ++i) {
        double invariantMass_lambda_us_1 = fourmoment_lambda_us_1[i].M();
        double invariantMass_lambda_us_2 = fourmoment_lambda_us_2[i].M();

        // Check if invariant mass values are within 3 sigma range
        if (invariantMass_lambda_us_1 >= minX_range_LL_2 && invariantMass_lambda_us_1 <= maxX_range_LL_2 &&
            invariantMass_lambda_us_2 >= minY_range_LL_2 && invariantMass_lambda_us_2 <= maxY_range_LL_2) {

                double delta_eta = fabs(fourmoment_lambda_us_1[i].Eta() - fourmoment_lambda_us_2[i].Eta());
                double delta_phi = fabs(fourmoment_lambda_us_1[i].Phi() - fourmoment_lambda_us_2[i].Phi());
                if (delta_phi > M_PI) delta_phi = 2 * M_PI - delta_phi;
                
                double delta_R = sqrt(delta_eta * delta_eta + delta_phi * delta_phi);

                h_delta_R_US_US_ll->Fill(delta_R);
                h_delta_phi_US_US_ll->Fill(delta_phi);

                double delta_R_Threshold = 0.93;
                if (delta_R > delta_R_Threshold)
                    continue;
            
            h2_invMass_lambda_us_lambda_us->Fill(invariantMass_lambda_us_1, invariantMass_lambda_us_2);
        } else {
            continue; // Skip if outside 3 sigma range
        }

        fourmoment_proton_us_1[i].Boost(-fourmoment_lambda_us_1[i].BoostVector());
        fourmoment_proton_us_2[i].Boost(-fourmoment_lambda_us_2[i].BoostVector());
        double theta_star = fourmoment_proton_us_1[i].Angle(fourmoment_proton_us_2[i].Vect());
        double cos_theta_star = cos(theta_star);

        // cos_theta_star_values.push_back(cos_theta_star);
        h_cos_theta_star->Fill(cos_theta_star);
    }

    // for loop for lambda_us - lambda_ls
    for (size_t i = 0; i < fourmoment_lambda_us3.size(); ++i){
        double invariantMass_lambda_us_3 = fourmoment_lambda_us3[i].M();
        double invariantMass_lambda_ls_3 = fourmoment_lambda_ls3[i].M();

        if (invariantMass_lambda_us_3 >= minX_range_LL_2 && invariantMass_lambda_us_3 <= maxX_range_LL_2 &&
            invariantMass_lambda_ls_3 >= minX_range_LL_2 && invariantMass_lambda_ls_3 <= maxX_range_LL_2) {

                double delta_eta = std::abs(fourmoment_lambda_us3[i].Eta() - fourmoment_lambda_ls3[i].Eta());
                double delta_phi = std::abs(fourmoment_lambda_us3[i].Phi() - fourmoment_lambda_ls3[i].Phi());

                if (delta_phi > M_PI) delta_phi = 2 * M_PI - delta_phi;
                
                double delta_R = sqrt(delta_eta * delta_eta + delta_phi * delta_phi);

                h_delta_R_US_LS_ll->Fill(delta_R);
                h_delta_phi_US_LS_ll->Fill(delta_phi);

                double delta_R_Threshold = 0.93;
                if (delta_R > delta_R_Threshold)
                    continue;

            h2_invMass_lambda_us_lambda_ls->Fill(invariantMass_lambda_us_3, invariantMass_lambda_ls_3);
        } else {
            continue;
        }

        fourmoment_proton_us3[i].Boost(-fourmoment_lambda_us3[i].BoostVector());
        fourmoment_proton_ls3[i].Boost(-fourmoment_lambda_ls3[i].BoostVector());
        double theta_star2 = fourmoment_proton_us3[i].Angle(fourmoment_proton_ls3[i].Vect());
        double cos_theta_star2 = cos(theta_star2);

        // cos_theta_star2_values.push_back(cos_theta_star2);
        h_cos_theta_star2->Fill(cos_theta_star2);
    }

    // ± 3 sigma check and polarization calculations for Lambda - Lambda Bar Pairs
    double meanX_LLBar_2 = gaus2D_LLBar->GetParameter(1);
    double sigmaX_LLBar_2 = gaus2D_LLBar->GetParameter(2);
    double meanY_LLBar_2 = gaus2D_LLBar->GetParameter(3);
    double sigmaY_LLBar_2 = gaus2D_LLBar->GetParameter(4);

    double minX_range_LLBar_2 = meanX_LLBar_2 - 3.0 * sigmaX_LLBar_2;
    double maxX_range_LLBar_2 = meanX_LLBar_2 + 3.0 * sigmaX_LLBar_2;
    double minY_range_LLBar_2 = meanY_LLBar_2 - 3.0 * sigmaY_LLBar_2;
    double maxY_range_LLBar_2 = meanY_LLBar_2 + 3.0 * sigmaY_LLBar_2;

    for (size_t i = 0; i < fourmoment_lambda_us4.size(); ++i){
        double invariantMass_lambda_us_4 = fourmoment_lambda_us4[i].M();
        double invariantMass_lambda_bar_us_4 = fourmoment_lambda_bar_us4[i].M();

        if (invariantMass_lambda_us_4 >= minX_range_LLBar_2 && invariantMass_lambda_us_4 <= maxX_range_LLBar_2 &&
            invariantMass_lambda_bar_us_4 >= minY_range_LLBar_2 && invariantMass_lambda_bar_us_4 <= maxY_range_LLBar_2) {

                double delta_eta = std::abs(fourmoment_lambda_us4[i].Eta() - fourmoment_lambda_bar_us4[i].Eta());
                double delta_phi = std::abs(fourmoment_lambda_us4[i].Phi() - fourmoment_lambda_bar_us4[i].Phi());

                if (delta_phi > M_PI) delta_phi = 2 * M_PI - delta_phi;
                
                double delta_R = sqrt(delta_eta * delta_eta + delta_phi * delta_phi);

                h_delta_R_US_US_llbar->Fill(delta_R);
                h_delta_phi_US_US_llbar->Fill(delta_phi);

                double delta_R_Threshold = 0.93;
                if (delta_R > delta_R_Threshold)
                    continue;

            h2_invMass_lambda_us_lambda_bar_us->Fill(invariantMass_lambda_us_4, invariantMass_lambda_bar_us_4);
        } else {
            continue;
        }

        fourmoment_proton_us4[i].Boost(-fourmoment_lambda_us4[i].BoostVector());
        fourmoment_anti_proton_us4[i].Boost(-fourmoment_lambda_bar_us4[i].BoostVector());
        double theta_star3 = fourmoment_proton_us4[i].Angle(fourmoment_anti_proton_us4[i].Vect());
        double cos_theta_star3 = cos(theta_star3);

        // cos_theta_star3_values.push_back(cos_theta_star3);
        h_cos_theta_star3->Fill(cos_theta_star3);
    }
    
    // for loop for lambda_us - lambda_bar_ls
    for (size_t i = 0; i < fourmoment_lambda_us5.size(); ++i){
        double invariantMass_lambda_us_5 = fourmoment_lambda_us5[i].M();
        double invariantMass_lambda_bar_ls_5 = fourmoment_lambda_bar_ls5[i].M();

        if (invariantMass_lambda_us_5 >= minX_range_LLBar_2 && invariantMass_lambda_us_5 <= maxX_range_LLBar_2 &&
            invariantMass_lambda_bar_ls_5 >= minY_range_LLBar_2 && invariantMass_lambda_bar_ls_5 <= maxY_range_LLBar_2) {

                double delta_eta = std::abs(fourmoment_lambda_us5[i].Eta() - fourmoment_lambda_bar_ls5[i].Eta());
                double delta_phi = std::abs(fourmoment_lambda_us5[i].Phi() - fourmoment_lambda_bar_ls5[i].Phi());

                if (delta_phi > M_PI) delta_phi = 2 * M_PI - delta_phi;
                
                double delta_R = sqrt(delta_eta * delta_eta + delta_phi * delta_phi);

                h_delta_R_US_LS_llbar->Fill(delta_R);
                h_delta_phi_US_LS_llbar->Fill(delta_phi);

                double delta_R_Threshold = 0.93;
                if (delta_R > delta_R_Threshold)
                    continue;

            h2_invMass_lambda_us_lambda_bar_ls->Fill(invariantMass_lambda_us_5, invariantMass_lambda_bar_ls_5);
        } else {
            continue;
        }

        fourmoment_proton_us5[i].Boost(-fourmoment_lambda_us5[i].BoostVector());
        fourmoment_anti_proton_ls5[i].Boost(-fourmoment_lambda_bar_ls5[i].BoostVector());
        double theta_star4 = fourmoment_proton_us5[i].Angle(fourmoment_anti_proton_ls5[i].Vect());
        double cos_theta_star4 = cos(theta_star4);

        // cos_theta_star4_values.push_back(cos_theta_star4);
        h_cos_theta_star4->Fill(cos_theta_star4);
    }
    
    // for loop for lambda_ls - lambda_bar_us
    for (size_t i = 0; i < fourmoment_lambda_ls6.size(); ++i){
        double invariantMass_lambda_ls_6 = fourmoment_lambda_ls6[i].M();
        double invariantMass_lambda_bar_us_6 = fourmoment_lambda_bar_us6[i].M();

        if (invariantMass_lambda_ls_6 >= minX_range_LLBar_2 && invariantMass_lambda_ls_6 <= maxX_range_LLBar_2 &&
            invariantMass_lambda_bar_us_6 >= minY_range_LLBar_2 && invariantMass_lambda_bar_us_6 <= maxY_range_LLBar_2){

                double delta_eta = std::abs(fourmoment_lambda_ls6[i].Eta() - fourmoment_lambda_bar_us6[i].Eta());
                double delta_phi = std::abs(fourmoment_lambda_ls6[i].Phi() - fourmoment_lambda_bar_us6[i].Phi());

                if (delta_phi > M_PI) delta_phi = 2 * M_PI - delta_phi;
                
                double delta_R = sqrt(delta_eta * delta_eta + delta_phi * delta_phi);

                h_delta_R_US_LS_llbar->Fill(delta_R);
                h_delta_phi_US_LS_llbar->Fill(delta_phi);

                double delta_R_Threshold = 0.93;
                if (delta_R > delta_R_Threshold)
                    continue;

            h2_invMass_lambda_us_lambda_bar_ls->Fill(invariantMass_lambda_ls_6, invariantMass_lambda_bar_us_6);
        } else {
            continue;
        }

        fourmoment_proton_ls6[i].Boost(-fourmoment_lambda_ls6[i].BoostVector());
        fourmoment_anti_proton_us6[i].Boost(-fourmoment_lambda_bar_us6[i].BoostVector());
        double theta_star4 = fourmoment_proton_ls6[i].Angle(fourmoment_anti_proton_us6[i].Vect());
        double cos_theta_star4 = cos(theta_star4);

        // cos_theta_star4_values.push_back(cos_theta_star4);
        h_cos_theta_star4->Fill(cos_theta_star4);
    }

    // ± 3 sigma check and polarization calculations for Lambda Bar - Lambda Bar Pairs
    double meanX_LBarLBar_2 = gaus2D_LBarLBar->GetParameter(1);
    double sigmaX_LBarLBar_2 = gaus2D_LBarLBar->GetParameter(2);
    double meanY_LBarLBar_2 = gaus2D_LBarLBar->GetParameter(3);
    double sigmaY_LBarLBar_2 = gaus2D_LBarLBar->GetParameter(4);

    double minX_range_LBarLBar_2 = meanX_LBarLBar_2 - 3.0 * sigmaX_LBarLBar_2;
    double maxX_range_LBarLBar_2 = meanX_LBarLBar_2 + 3.0 * sigmaX_LBarLBar_2;
    double minY_range_LBarLBar_2 = meanY_LBarLBar_2 - 3.0 * sigmaY_LBarLBar_2;
    double maxY_range_LBarLBar_2 = meanY_LBarLBar_2 + 3.0 * sigmaY_LBarLBar_2;

    for (size_t i = 0; i < fourmoment_lambda_bar_us_1.size(); ++i) {
        double invariantMass_lambda_bar_us_1 = fourmoment_lambda_bar_us_1[i].M();
        double invariantMass_lambda_bar_us_2 = fourmoment_lambda_bar_us_2[i].M();

        if (invariantMass_lambda_bar_us_1 >= minX_range_LBarLBar_2 && invariantMass_lambda_bar_us_1 <= maxX_range_LBarLBar_2 &&
            invariantMass_lambda_bar_us_2 >= minY_range_LBarLBar_2 && invariantMass_lambda_bar_us_2 <= maxY_range_LBarLBar_2) {

                double delta_eta = std::abs(fourmoment_lambda_bar_us_1[i].Eta() - fourmoment_lambda_bar_us_2[i].Eta());
                double delta_phi = std::abs(fourmoment_lambda_bar_us_1[i].Phi() - fourmoment_lambda_bar_us_2[i].Phi());

                if (delta_phi > M_PI) delta_phi = 2 * M_PI - delta_phi;
                
                double delta_R = sqrt(delta_eta * delta_eta + delta_phi * delta_phi);

                h_delta_R_US_US_lbarlbar->Fill(delta_R);
                h_delta_phi_US_US_lbarlbar->Fill(delta_phi);

                double delta_R_Threshold = 0.93;
                if (delta_R > delta_R_Threshold)
                    continue;

            h2_invMass_lambda_bar_us_lambda_bar_us->Fill(invariantMass_lambda_bar_us_1, invariantMass_lambda_bar_us_2);
        } else {
            continue;
        }

        fourmoment_anti_proton_us_1[i].Boost(-fourmoment_lambda_bar_us_1[i].BoostVector());
        fourmoment_anti_proton_us_2[i].Boost(-fourmoment_lambda_bar_us_2[i].BoostVector());
        double theta_star5 = fourmoment_anti_proton_us_1[i].Angle(fourmoment_anti_proton_us_2[i].Vect());
        double cos_theta_star5 = cos(theta_star5);

        // cos_theta_star5_values.push_back(cos_theta_star5);
        h_cos_theta_star5->Fill(cos_theta_star5);
    }

    // for loop for lambda_bar_us - lambda_bar_ls 
    for (size_t i = 0; i < fourmoment_lambda_bar_us7.size(); ++i) {
        double invariantMass_lambda_bar_us_7 = fourmoment_lambda_bar_us7[i].M();
        double invariantMass_lambda_bar_ls_7 = fourmoment_lambda_bar_ls7[i].M();

        if (invariantMass_lambda_bar_us_7 >= minX_range_LBarLBar_2 && invariantMass_lambda_bar_us_7 <= maxX_range_LBarLBar_2 &&
            invariantMass_lambda_bar_ls_7 >= minY_range_LBarLBar_2 && invariantMass_lambda_bar_ls_7 <= maxY_range_LBarLBar_2) {

                double delta_eta = std::abs(fourmoment_lambda_bar_us7[i].Eta() - fourmoment_lambda_bar_ls7[i].Eta());
                double delta_phi = std::abs(fourmoment_lambda_bar_us7[i].Phi() - fourmoment_lambda_bar_ls7[i].Phi());

                if (delta_phi > M_PI) delta_phi = 2 * M_PI - delta_phi;
                
                double delta_R = sqrt(delta_eta * delta_eta + delta_phi * delta_phi);

                h_delta_R_US_LS_lbarlbar->Fill(delta_R);
                h_delta_phi_US_LS_lbarlbar->Fill(delta_phi);

                double delta_R_Threshold = 0.93;
                if (delta_R > delta_R_Threshold)
                    continue;

            h2_invMass_lambda_bar_us_lambda_bar_ls->Fill(invariantMass_lambda_bar_us_7, invariantMass_lambda_bar_ls_7);
        } else {
            continue;
        }

        fourmoment_anti_proton_us7[i].Boost(-fourmoment_lambda_bar_us7[i].BoostVector());
        fourmoment_anti_proton_ls7[i].Boost(-fourmoment_lambda_bar_ls7[i].BoostVector());
        double theta_star6 = fourmoment_anti_proton_us7[i].Angle(fourmoment_anti_proton_ls7[i].Vect());
        double cos_theta_star6 = cos(theta_star6);

        // cos_theta_star6_values.push_back(cos_theta_star6);
        h_cos_theta_star6->Fill(cos_theta_star6);
    }

    // Canvas 3: Invariant Mass Distribution within ±3 sigma range of the gaussian fit
    std::unique_ptr<TCanvas> c3(new TCanvas("c3", "3D Invariant Mass Plots (check)", 1800, 900));
    c3->Divide(3, 1);

    // Lambda - Lambda Signal
    TH2F *h2_invMass_LL_Signal = new TH2F(*h2_invMass_lambda_us_lambda_us);
    h2_invMass_LL_Signal->Add(h2_invMass_lambda_us_lambda_ls, -1.0);

    double minX_2 = 1.11;
    double maxX_2 = 1.12;
    double minY_2 = 1.11;
    double maxY_2 = 1.12;

    c3->cd(1);
    int binMinX_2 = h2_invMass_LL_Signal->GetXaxis()->FindBin(minX_2);
    int binMaxX_2 = h2_invMass_LL_Signal->GetXaxis()->FindBin(maxX_2);
    int binMinY_2 = h2_invMass_LL_Signal->GetYaxis()->FindBin(minY_2);
    int binMaxY_2 = h2_invMass_LL_Signal->GetYaxis()->FindBin(maxY_2);

    h2_invMass_LL_Signal->GetXaxis()->SetRange(binMinX_2, binMaxX_2);
    h2_invMass_LL_Signal->GetYaxis()->SetRange(binMinY_2, binMaxY_2);
    h2_invMass_LL_Signal->Draw("SURF2");
    h2_invMass_LL_Signal->SetTitle("Lambda - Lambda Pair (Signal)");

    // Lambda - Lambda Bar Signal 
    TH2F *h2_invMass_LLBar_Signal = new TH2F(*h2_invMass_lambda_us_lambda_bar_us);
    h2_invMass_LLBar_Signal->Add(h2_invMass_lambda_us_lambda_bar_ls, -1.0);

    c3->cd(2);
    binMinX_2 = h2_invMass_LLBar_Signal->GetXaxis()->FindBin(minX_2);
    binMaxX_2 = h2_invMass_LLBar_Signal->GetXaxis()->FindBin(maxX_2);
    binMinY_2 = h2_invMass_LLBar_Signal->GetYaxis()->FindBin(minY_2);
    binMaxY_2 = h2_invMass_LLBar_Signal->GetYaxis()->FindBin(maxY_2);

    h2_invMass_LLBar_Signal->GetXaxis()->SetRange(binMinX_2, binMaxX_2);
    h2_invMass_LLBar_Signal->GetYaxis()->SetRange(binMinY_2, binMaxY_2);
    h2_invMass_LLBar_Signal->Draw("SURF2");
    h2_invMass_LLBar_Signal->SetTitle("Lambda - Lambda Bar Pair (Signal)");

    // Lambda Bar - Lambda Bar Signal
    TH2F *h2_invMass_LBarLBar_Signal = new TH2F(*h2_invMass_lambda_bar_us_lambda_bar_us);
    h2_invMass_LBarLBar_Signal->Add(h2_invMass_lambda_bar_us_lambda_bar_ls, -1.0);

    // Canvas 4: Lambda Bar - Lambda Bar Values within the ±3 sigma range of the gaussian fit
    c3->cd(3);
    binMinX_2 = h2_invMass_LBarLBar_Signal->GetXaxis()->FindBin(minX_2);
    binMaxX_2 = h2_invMass_LBarLBar_Signal->GetXaxis()->FindBin(maxX_2);
    binMinY_2 = h2_invMass_LBarLBar_Signal->GetYaxis()->FindBin(minY_2);
    binMaxY_2 = h2_invMass_LBarLBar_Signal->GetYaxis()->FindBin(maxY_2);

    h2_invMass_LBarLBar_Signal->GetXaxis()->SetRange(binMinX_2, binMaxX_2);
    h2_invMass_LBarLBar_Signal->GetYaxis()->SetRange(binMinY_2, binMaxY_2);
    h2_invMass_LBarLBar_Signal->Draw("SURF2");
    h2_invMass_LBarLBar_Signal->SetTitle("Lambda Bar - Lambda Bar Pair (Signal)");

    // Canvas 4: Delta R and Delta Phi Distribution (the cut used in the main loop)
    std::unique_ptr<TCanvas> c4(new TCanvas("c4", "Delta R and Delta Phi Distribution", 1800, 900));
    c4->Divide(3, 2);

    c4->cd(1);
    h_delta_R_US_US_ll->Draw();
    h_delta_R_US_US_ll->SetLineColor(kBlue);
    h_delta_R_US_US_ll->SetTitle("Delta R Distribution for Lambda - Lambda Pairs");
    h_delta_R_US_LS_ll->Draw("same");
    h_delta_R_US_LS_ll->SetLineColor(kRed);

    c4->cd(2);
    h_delta_R_US_US_llbar->Draw();
    h_delta_R_US_US_llbar->SetLineColor(kBlue);
    h_delta_R_US_US_llbar->SetTitle("Delta R Distribution for Lambda - Lambda Bar Pairs");
    h_delta_R_US_LS_llbar->Draw("same");
    h_delta_R_US_LS_llbar->SetLineColor(kRed);

    c4->cd(3);
    h_delta_R_US_US_lbarlbar->Draw();
    h_delta_R_US_US_lbarlbar->SetLineColor(kBlue);
    h_delta_R_US_US_lbarlbar->SetTitle("Delta R Distribution for Lambda Bar - Lambda Bar Pairs");
    h_delta_R_US_LS_lbarlbar->Draw("same");
    h_delta_R_US_LS_lbarlbar->SetLineColor(kRed);

    c4->cd(4);
    h_delta_phi_US_US_ll->SetMinimum(0);
    h_delta_phi_US_US_ll->Draw();
    h_delta_phi_US_US_ll->SetLineColor(kBlue);
    h_delta_phi_US_US_ll->SetTitle("Delta Phi Distribution for Lambda - Lambda Pairs");
    h_delta_phi_US_LS_ll->Draw("same");
    h_delta_phi_US_LS_ll->SetLineColor(kRed);

    c4->cd(5);
    h_delta_phi_US_US_llbar->SetMinimum(0);
    h_delta_phi_US_US_llbar->Draw();
    h_delta_phi_US_US_llbar->SetLineColor(kBlue);
    h_delta_phi_US_US_llbar->SetTitle("Delta Phi Distribution for Lambda - Lambda Bar Pairs");
    h_delta_phi_US_LS_llbar->Draw("same");
    h_delta_phi_US_LS_llbar->SetLineColor(kRed);

    c4->cd(6);
    h_delta_phi_US_US_lbarlbar->SetMinimum(0);
    h_delta_phi_US_US_lbarlbar->Draw();
    h_delta_phi_US_US_lbarlbar->SetLineColor(kBlue);
    h_delta_phi_US_US_lbarlbar->SetTitle("Delta Phi Distribution for Lambda Bar - Lambda Bar Pairs");
    h_delta_phi_US_LS_lbarlbar->Draw("same");
    h_delta_phi_US_LS_lbarlbar->SetLineColor(kRed);

    // Canvas 5: Angular Distribution for US-US pairs and US-LS pairs
    std::unique_ptr<TCanvas> c5(new TCanvas("c5", "Angular Distributions", 1800, 900));
    c5->Divide(3, 1);

    h_cos_theta_star->Sumw2();
    h_cos_theta_star2->Sumw2();
    h_cos_theta_star3->Sumw2();
    h_cos_theta_star4->Sumw2();
    h_cos_theta_star5->Sumw2();
    h_cos_theta_star6->Sumw2();

    c5->cd(1);
    h_cos_theta_star->SetMinimum(0);
    h_cos_theta_star->SetMarkerStyle(20);
    h_cos_theta_star->SetMarkerColor(kBlue);
    h_cos_theta_star->Draw("PE");
    h_cos_theta_star->SetTitle("Lambda - Lambda Angular Correlation");
    h_cos_theta_star2->SetLineColor(kRed);
    h_cos_theta_star2->SetMarkerStyle(20);
    h_cos_theta_star2->SetMarkerColor(kRed);
    h_cos_theta_star2->Draw("same PE");

    c5->cd(2);
    h_cos_theta_star3->SetMinimum(0);
    h_cos_theta_star3->SetMarkerStyle(20);
    h_cos_theta_star3->SetMarkerColor(kBlue);
    h_cos_theta_star3->Draw("PE"); // Draw with error bars
    h_cos_theta_star3->SetTitle("Lambda - Lambda Bar Angular Correlation");
    h_cos_theta_star4->SetMarkerStyle(20);
    h_cos_theta_star4->SetLineColor(kRed);
    h_cos_theta_star4->SetMarkerColor(kRed);
    h_cos_theta_star4->Draw("same PE"); // Draw with error bars

    c5->cd(3);
    h_cos_theta_star5->SetMinimum(0);
    h_cos_theta_star5->SetMarkerStyle(20);
    h_cos_theta_star5->SetMarkerColor(kBlue);
    h_cos_theta_star5->Draw("PE"); // Draw with error bars
    h_cos_theta_star5->SetTitle("Lambda Bar - Lambda Bar Angular Correlation");
    h_cos_theta_star6->SetLineColor(kRed);
    h_cos_theta_star6->SetMarkerStyle(20);
    h_cos_theta_star6->SetMarkerColor(kRed);
    h_cos_theta_star6->Draw("same PE"); // Draw with error bars

    // Canvas 6: Angular Distribution of the Signal with Fit
    std::unique_ptr<TCanvas> c6(new TCanvas("c6", "Angular Distributions of Signal with Fit and Background", 1800, 900));
    c6->Divide(3, 1);

    // Angular Correlation for Lambda - Lambda Signal
    c6->cd(1);
    TH1D *h_cos_theta_starLL = new TH1D(*h_cos_theta_star);
    h_cos_theta_starLL->Add(h_cos_theta_star2, -1.0);

    // Define the fitting function
    TF1 *fit_cos_theta_starLL = new TF1("fit_cos_theta_starLL", "[0] * (1 + [1] * x)", -1, 1);
    fit_cos_theta_starLL->SetParameters(2050.4, -0.0071055); // Initial guess for A and B

    // Perform the fit
    h_cos_theta_starLL->Fit(fit_cos_theta_starLL, "R");

    // Print out the fit parameters
    double A = fit_cos_theta_starLL->GetParameter(0);
    double B = fit_cos_theta_starLL->GetParameter(1);

    cout << "Fit Parameters for Lambda - Lambda :" << endl;
    cout << "A: " << A << endl;
    cout << "B: " << B << endl;

    h_cos_theta_starLL->SetMinimum(0);
    h_cos_theta_starLL->Draw("PE");
    h_cos_theta_starLL->SetTitle("Lambda - Lambda Angular Correlation (signal)");
    fit_cos_theta_starLL->Draw("same");

    // Angular Correlation for Lambda - Lambda Bar Signal
    c6->cd(2);
    TH1D *h_cos_theta_starLLBar = new TH1D(*h_cos_theta_star3);
    h_cos_theta_starLLBar->Add(h_cos_theta_star4, -1.0);

    // Define the fitting function
    TF1 *fit_cos_theta_starLLBar = new TF1("fit_cos_theta_starLLBar", "[0] * (1 + [1] * x)", -1, 1);
    fit_cos_theta_starLLBar->SetParameters(1, 1); // Initial guess for A and B

    // Perform the fit
    h_cos_theta_starLLBar->Fit(fit_cos_theta_starLLBar, "R");

    // Print out the fit parameters
    double A2 = fit_cos_theta_starLLBar->GetParameter(0);
    double B2 = fit_cos_theta_starLLBar->GetParameter(1);

    cout << "Fit Parameters for Lambda - Lambda Bar:" << endl;
    cout << "A: " << A2 << endl;
    cout << "B: " << B2 << endl;

    h_cos_theta_starLLBar->SetMinimum(0);
    h_cos_theta_starLLBar->Draw("PE");
    h_cos_theta_starLLBar->SetTitle("Lambda - Lambda Bar Angular Correlation (signal)");
    fit_cos_theta_starLLBar->Draw("same");

    // Angular Correlation for Lambda Bar - Lambda Bar Signal
    c6->cd(3);
    TH1D *h_cos_theta_starLBarLBar = new TH1D(*h_cos_theta_star5);
    h_cos_theta_starLBarLBar->Add(h_cos_theta_star6, -1.0);

    // Define the fitting function
    TF1 *fit_cos_theta_starLBarLBar = new TF1("fit_cos_theta_starLBarLBar", "[0] * (1 + [1] * x)", -1, 1);
    fit_cos_theta_starLBarLBar->SetParameters(1, 1); // Initial guess for A and B

    // Perform the fit
    h_cos_theta_starLBarLBar->Fit(fit_cos_theta_starLBarLBar, "R");

    // Print out the fit parameters
    double A3 = fit_cos_theta_starLBarLBar->GetParameter(0);
    double B3 = fit_cos_theta_starLBarLBar->GetParameter(1);

    cout << "Fit Parameters for Lambda Bar - Lambda Bar:" << endl;
    cout << "A: " << A3 << endl;
    cout << "B: " << B3 << endl;

    h_cos_theta_starLBarLBar->SetMinimum(0);
    h_cos_theta_starLBarLBar->Draw("PE");
    h_cos_theta_starLBarLBar->SetTitle("Lambda Bar - Lambda Bar Angular Correlation (signal)");
    fit_cos_theta_starLBarLBar->Draw("same");

    // c1->SaveAs("canvas1.jpg");
    // c2->SaveAs("canvas2.jpg");
    // c3->SaveAs("canvas3.jpg");
    // c4->SaveAs("canvas4.jpg");
    // c5->SaveAs("canvas5.jpg");
    // c6->SaveAs("canvas6.jpg");

    c1->Update();
    c2->Update();
    c3->Update();
    c4->Update();
    c5->Update();
    c6->Update();

    c1->WaitPrimitive();
    c2->WaitPrimitive();
    c3->WaitPrimitive();
    c4->WaitPrimitive();
    c5->WaitPrimitive();
    c6->WaitPrimitive();
}

int main() {
    final_lpair_signal_pp();
    return 0;
}