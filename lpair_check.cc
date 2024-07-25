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

void lpair_check() {
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

    // Vectors to store the sign charge data
    vector<double> h2_unlike_lj;
    vector<double> h2_unlike_lk;
    vector<double> h2_like_lj;
    vector<double> h2_like_lk;
    vector<double> h2_unlike_lbark;
    vector<double> h2_like_lbark;
    vector<double> h2_unlike_lbarj;
    vector<double> h2_like_lbarj;

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

    // Clear vectors before processing
    h2_unlike_lj.clear();
    h2_unlike_lk.clear();
    h2_like_lj.clear();
    h2_like_lk.clear();
    h2_unlike_lbark.clear();
    h2_like_lbark.clear();
    h2_unlike_lbarj.clear();
    h2_like_lbarj.clear();

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

        TLorentzVector fourmomentum = p1 + p2;

        if (p1_ch > 0) {
            lambdas.push_back(fourmomentum);
            proton_ids.push_back(p1_InEventID);
            pion_ids.push_back(p2_InEventID);
            h1_lambda_mass->Fill(invariantMass(p1, p2));

            if (eventId != prevEventID && prevEventID != -1){
                for (size_t j=0; j < lambdas.size(); ++j){
                    for (size_t k = j+1; k < lambdas.size(); ++k){
                        if (proton_ids[j] == proton_ids[k] || pion_ids[j] == pion_ids[k])
                            continue;

                        h2_lambda_lambda->Fill(lambdas[j].M(), lambdas[k].M());

                        if (pair_charge == 0){
                            h2_unlike_lj.push_back(lambdas[j].M());
                            h2_unlike_lk.push_back(lambdas[k].M());
                        } else {
                            h2_like_lj.push_back(lambdas[j].M());
                            h2_like_lk.push_back(lambdas[k].M());
                        }
                    }

                for (size_t j = 0; j < lambdas.size(); ++j){
                    for (size_t k = 0; k < lambda_bars.size(); ++k){
                        if (proton_ids[j] == anti_proton_ids[k] || pion_ids[j] == anti_pion_ids[k])
                            continue;

                        h2_lambda_lambda_bar->Fill(lambdas[j].M(), lambda_bars[k].M());

                        if (pair_charge == 0){
                            h2_unlike_lj.push_back(lambdas[j].M());
                            h2_unlike_lbark.push_back(lambda_bars[k].M());
                        } else{
                            h2_like_lj.push_back(lambdas[j].M());
                            h2_like_lbark.push_back(lambda_bars[k].M());
                        }
                    }
                }
            }

        } else if (p1_ch < 0) {
            lambda_bars.push_back(fourmomentum);
            anti_proton_ids.push_back(p1_InEventID);
            anti_pion_ids.push_back(p2_InEventID);
            h1_lambda_bar_mass->Fill(invariantMass(p1, p2));

            if (eventId != prevEventID && prevEventID != -1){
                for (size_t j = 0; j < lambda_bars.size(); ++j){
                    for (size_t k = j+1; k < lambda_bars.size(); ++k){
                        if (anti_proton_ids[j] == anti_proton_ids[k] || anti_pion_ids[j] == anti_pion_ids[k])
                            continue;

                        h2_lambda_bar_lambda_bar->Fill(lambda_bars[j].M(), lambda_bars[k].M());

                        if (pair_charge == 0){
                            h2_unlike_lbarj.push_back(lambda_bars[j].M());
                            h2_unlike_lbark.push_back(lambda_bars[k].M());
                        } else {
                            h2_like_lbarj.push_back(lambda_bars[j].M());
                            h2_like_lbark.push_back(lambda_bars[k].M());
                        }
                    }
                }
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

    // Now fill the histograms h1 to h12 with the vectors
    for (size_t idx = 0; idx < h2_unlike_lj.size(); ++idx) {
        h1->Fill(h2_unlike_lj[idx], h2_unlike_lk[idx]);
        h2->Fill(h2_unlike_lj[idx], h2_like_lk[idx]);
        h3->Fill(h2_like_lj[idx], h2_like_lk[idx]);
        h4->Fill(h2_like_lj[idx], h2_unlike_lk[idx]);
        h5->Fill(h2_unlike_lj[idx], h2_unlike_lbark[idx]);
        h6->Fill(h2_unlike_lj[idx], h2_like_lj[idx]);
        h7->Fill(h2_like_lj[idx], h2_like_lbark[idx]);
        h8->Fill(h2_like_lj[idx], h2_unlike_lbark[idx]);
        h9->Fill(h2_unlike_lbarj[idx], h2_unlike_lbark[idx]);
        h10->Fill(h2_unlike_lbarj[idx], h2_like_lbark[idx]);
        h11->Fill(h2_like_lbarj[idx], h2_like_lbark[idx]);
        h12->Fill(h2_like_lbarj[idx], h2_unlike_lbark[idx]);
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
}