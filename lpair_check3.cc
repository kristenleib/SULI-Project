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

void lpair_check3() {
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

    // Vectors to store TLorentzVectors and particle IDs
    vector<TLorentzVector> lambdas;
    vector<Int_t> proton_ids;
    vector<Int_t> pion_ids;

    vector<TLorentzVector> lambda_bars;
    vector<Int_t> anti_proton_ids;
    vector<Int_t> anti_pion_ids;

    // Vectors to store the like-sign and unlike-sign data
    vector<TLorentzVector> lambda_ls;
    vector<TLorentzVector> lambda_us;
    vector<TLorentzVector> lambda_bar_ls;
    vector<TLorentzVector> lambda_bar_us;

    // Create 1D histograms for invariant masses
    TH1F *h1_lambda_mass = new TH1F("h1_lambda_mass", "Lambda Invariant Mass", 300, 1.0, 1.4);
    TH1F *h1_lambda_bar_mass = new TH1F("h1_lambda_bar_mass", "Lambda-bar Invariant Mass", 300, 1.0, 1.4);

    // Create 2D histograms for pairings
    TH2F *h2_lambda_lambda = new TH2F("h2_lambda_lambda", "Lambda-Lambda Invariant Mass", 150, 1, 1.2, 150, 1, 1.2);
    TH2F *h2_lambda_lambda_bar = new TH2F("h2_lambda_lambda_bar", "Lambda-Lambda-bar Invariant Mass", 150, 1, 1.2, 150, 1, 1.2);
    TH2F *h2_lambda_bar_lambda_bar = new TH2F("h2_lambda_bar_lambda_bar", "Lambda-bar-Lambda-bar Invariant Mass", 150, 1, 1.2, 150, 1, 1.2);

    TH2D* h1 = new TH2D("h1", "Unlike Sign Lambda - Unlike Sign Lambda", 100, 1.0, 1.4, 100, 1.0, 1.4);
    TH2D* h2 = new TH2D("h2", "Unlike Sign Lambda - Like Sign Lambda", 100, 1.0, 1.4, 100, 1.0, 1.4);
    TH2D* h3 = new TH2D("h3", "Like Sign Lambda - Like Sign Lambda", 100, 1.0, 1.4, 100, 1.0, 1.4);
    TH2D* h4 = new TH2D("h4", "Like Sign Lambda - Unlike Sign Lambda", 100, 1.0, 1.4, 100, 1.0, 1.4);
    TH2D* h5 = new TH2D("h5", "Unlike Sign Lambda - Unlike Sign Lambda Bar", 100, 1.0, 1.4, 100, 1.0, 1.4);
    TH2D* h6 = new TH2D("h6", "Unlike Sign Lambda - Like Sign Lambda Bar", 100, 1.0, 1.4, 100, 1.0, 1.4);
    TH2D* h7 = new TH2D("h7", "Like Sign Lambda - Like Sign Lambda Bar", 100, 1.0, 1.4, 100, 1.0, 1.4);
    TH2D* h8 = new TH2D("h8", "Like Sign Lambda - Unlike Sign Lambda Bar", 100, 1.0, 1.4, 100, 1.0, 1.4);
    TH2D* h9 = new TH2D("h9", "Unlike Sign Lambda Bar - Unlike Sign Lambda Bar", 100, 1.0, 1.4, 100, 1.0, 1.4);
    TH2D* h10 = new TH2D("h10", "Unlike Sign Lambda Bar - Like Sign Lambda Bar", 100, 1.0, 1.4, 100, 1.0, 1.4);
    TH2D* h11 = new TH2D("h11", "Like Sign Lambda Bar - Like Sign Lambda Bar", 100, 1.0, 1.4, 100, 1.0, 1.4);
    TH2D* h12 = new TH2D("h12", "Like Sign Lambda Bar - Unlike Sign Lambda Bar", 100, 1.0, 1.4, 100, 1.0, 1.4);

    // Clear vectors before processing a new event or as needed
    lambda_ls.clear();
    lambda_us.clear();
    lambda_bar_ls.clear();
    lambda_bar_us.clear();

    // Main for loop that Fills the Vector Pairs
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
                lambdas.push_back(fourmomentum);
                proton_ids.push_back(p1_InEventID);
                pion_ids.push_back(p2_InEventID);
                h1_lambda_mass->Fill(invariantMass(p1, p2));
            } else if (p1_ch < 0) {
                // Lambda-bar pair
                lambda_bar_us.push_back(fourmomentum);
                lambda_bars.push_back(fourmomentum);
                anti_proton_ids.push_back(p1_InEventID);
                anti_pion_ids.push_back(p2_InEventID);
                h1_lambda_bar_mass->Fill(invariantMass(p1, p2));
            }
        } else {
            // Like-sign pairs
            if (p1_ch > 0) {
                // Lambda pair
                lambda_ls.push_back(fourmomentum);
                lambdas.push_back(fourmomentum);
                proton_ids.push_back(p1_InEventID);
                pion_ids.push_back(p2_InEventID);
                h1_lambda_mass->Fill(invariantMass(p1, p2));
            } else if (p1_ch < 0) {
                // Lambda-bar pair
                lambda_bar_ls.push_back(fourmomentum);
                lambda_bars.push_back(fourmomentum);
                anti_proton_ids.push_back(p1_InEventID);
                anti_pion_ids.push_back(p2_InEventID);
                h1_lambda_bar_mass->Fill(invariantMass(p1, p2));
            }
        }

        // Pairing the Lambdas and Lambda Bars
        if (eventId != prevEventID && prevEventID != -1){
            for (size_t j=0; j < lambdas.size(); ++j){
                for (size_t k = j+1; k < lambdas.size(); ++k){
                    if (proton_ids[j] == proton_ids[k] || pion_ids[j] == pion_ids[k])
                        continue;

                    h2_lambda_lambda->Fill(lambdas[j].M(), lambdas[k].M());
                }

                for (size_t z=0; z < lambdas.size(); ++z){
                    if (proton_ids[j] == proton_ids[z] || pion_ids[j] == pion_ids[z])
                        continue;

                    h1->Fill(lambda_us[j].M(), lambda_us[z].M());
                    h2->Fill(lambda_us[j].M(), lambda_ls[z].M());
                    h3->Fill(lambda_ls[j].M(), lambda_ls[z].M());
                    h4->Fill(lambda_ls[j].M(), lambda_us[z].M());
                }
            }

            for (size_t j = 0; j < lambdas.size(); ++j){
                for (size_t k = 0; k < lambda_bars.size(); ++k){
                    if (proton_ids[j] == anti_proton_ids[k] || pion_ids[j] == anti_pion_ids[k])
                            continue;

                    h2_lambda_lambda_bar->Fill(lambdas[j].M(), lambda_bars[k].M());
                }

                for (size_t z=0; z < lambdas.size(); ++z){
                    if (proton_ids[j] == anti_proton_ids[z] || pion_ids[j] == anti_pion_ids[z])
                        continue;

                    h5->Fill(lambda_us[j].M(), lambda_bar_us[z].M());
                    h6->Fill(lambda_us[j].M(), lambda_bar_ls[z].M());
                    h7->Fill(lambda_ls[j].M(), lambda_bar_ls[z].M());
                    h8->Fill(lambda_ls[j].M(), lambda_bar_us[z].M());
                }
            }

            for (size_t j = 0; j < lambda_bars.size(); ++j){
                for (size_t k = j+1; k < lambda_bars.size(); ++k){
                    if (anti_proton_ids[j] == anti_proton_ids[k] || anti_pion_ids[j] == anti_pion_ids[k])
                            continue;

                    h2_lambda_bar_lambda_bar->Fill(lambda_bars[j].M(), lambda_bars[k].M());
                }

                for (size_t z=0; z < lambdas.size(); ++z){
                    if (anti_proton_ids[j] == anti_proton_ids[z] || anti_pion_ids[z] == anti_pion_ids[z])
                        continue;

                    h9->Fill(lambda_bar_us[j].M(), lambda_bar_us[z].M());
                    h10->Fill(lambda_bar_us[j].M(), lambda_bar_ls[z].M());
                    h11->Fill(lambda_bar_ls[j].M(), lambda_bar_ls[z].M());
                    h12->Fill(lambda_bar_ls[j].M(), lambda_bar_us[z].M());
                }
            }

            // Clear vectors after processing each event
            lambdas.clear();
            lambda_bars.clear();
            proton_ids.clear();
            pion_ids.clear();
            anti_proton_ids.clear();
            anti_pion_ids.clear();
        }

        // Update previous event ID
        prevEventID = eventId;

        // The end of the main loop
    }

    gStyle->SetPalette(kRainBow);

    // Create TCanvas for displaying histograms
    std::unique_ptr<TCanvas> c1(new TCanvas("c1", "1D Histograms", 1500, 400));
    c1->Divide(2, 1);

    std::unique_ptr<TCanvas> c2(new TCanvas("c2", "2D Histograms", 1500, 400));
    c2->Divide(3, 1);

    std::unique_ptr<TCanvas> c3(new TCanvas("c3", "3D Histograms", 1500, 400));
    c3->Divide(3, 1);

    std::unique_ptr<TCanvas> c4(new TCanvas("c4", "Pair Combinations", 1500, 400));
    c4->Divide(4,3);

    std::unique_ptr<TCanvas> c5(new TCanvas("c5", "Pair Combinations (3D)", 1500, 400));
    c5->Divide(4,3);

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

    // Pair Charge Histograms
    c4->cd(1);
    h1->Draw("COLZ");

    c4->cd(2);
    h2->Draw("COLZ");

    c4->cd(3);
    h3->Draw("COLZ");

    c4->cd(4);
    h4->Draw("COLZ");

    c4->cd(5);
    h5->Draw("COLZ");

    c4->cd(6);
    h6->Draw("COLZ");

    c4->cd(7);
    h7->Draw("COLZ");

    c4->cd(8);
    h8->Draw("COLZ");

    c4->cd(9);
    h9->Draw("COLZ");

    c4->cd(10);
    h10->Draw("COLZ");

    c4->cd(11);
    h11->Draw("COLZ");

    c4->cd(12);
    h12->Draw("COLZ");

    // Pair Charge Histograms
    c5->cd(1);
    h1->Draw("SURF2");

    c5->cd(2);
    h2->Draw("SURF2");

    c5->cd(3);
    h3->Draw("SURF2");

    c5->cd(4);
    h4->Draw("SURF2");

    c5->cd(5);
    h5->Draw("SURF2");

    c5->cd(6);
    h6->Draw("SURF2");

    c5->cd(7);
    h7->Draw("SURF2");

    c5->cd(8);
    h8->Draw("SURF2");

    c5->cd(9);
    h9->Draw("SURF2");

    c5->cd(10);
    h10->Draw("SURF2");

    c5->cd(11);
    h11->Draw("SURF2");

    c5->cd(12);
    h12->Draw("SURF2");

    // Update canvases
    c1->Update();
    c2->Update();
    c3->Update();
    c4->Update();
    c5->Update();

    // Wait for user input or handle events
    c1->WaitPrimitive();
    c2->WaitPrimitive();
    c3->WaitPrimitive();
    c4->WaitPrimitive();
    c5->WaitPrimitive();

    // Clean up
    delete h1_lambda_mass;
    delete h1_lambda_bar_mass;
    delete h2_lambda_lambda;
    delete h2_lambda_lambda_bar;
    delete h2_lambda_bar_lambda_bar;

    // delete ntp_Lambda;
    f->Close();
    // delete f;

    // The end of the macro
}