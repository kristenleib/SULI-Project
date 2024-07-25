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

void lpair_check4() {
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
                // lambdas.push_back(fourmomentum);
                proton_id_lambda_us.push_back(p1_InEventID);
                pion_id_lambda_us.push_back(p2_InEventID);
                h1_lambda_mass->Fill(invariantMass(p1, p2));
            } else if (p1_ch < 0) {
                // Lambda-bar pair
                lambda_bar_us.push_back(fourmomentum);
                // lambda_bars.push_back(fourmomentum);
                anti_proton_id_lambda_bar_us.push_back(p1_InEventID);
                anti_pion_id_lambda_bar_us.push_back(p2_InEventID);
                h1_lambda_bar_mass->Fill(invariantMass(p1, p2));
            }
        } else {
            // Like-sign pairs
            if (p1_ch > 0) {
                // Lambda pair
                lambda_ls.push_back(fourmomentum);
                // lambdas.push_back(fourmomentum);
                proton_id_lambda_ls.push_back(p1_InEventID);
                pion_id_lambda_ls.push_back(p2_InEventID);
                h1_lambda_mass->Fill(invariantMass(p1, p2));
            } else if (p1_ch < 0) {
                // Lambda-bar pair
                lambda_bar_ls.push_back(fourmomentum);
                // lambda_bars.push_back(fourmomentum);
                anti_proton_id_lambda_bar_ls.push_back(p1_InEventID);
                anti_pion_id_lambda_bar_ls.push_back(p2_InEventID);
                h1_lambda_bar_mass->Fill(invariantMass(p1, p2));
            }
        }

        // Pairing the Lambdas and Lambda Bars
        if (eventId != prevEventID && prevEventID != -1){

            //
            Int_t q = 0;

            for (size_t j = 0; j < lambda_us.size(); ++j) {
                for (size_t k = j + 1; k < lambda_us.size(); ++k) {
                    if (proton_id_lambda_us[j] == proton_id_lambda_us[k] || pion_id_lambda_us[j] == pion_id_lambda_us[k])
                        continue;
                    h2_lambda_us_lambda_us->Fill(lambda_us[j].M(), lambda_us[k].M());
                }

                for (size_t k = 0; k < lambda_ls.size(); ++k) {
                    if (proton_id_lambda_us[j] == proton_id_lambda_ls[k] || pion_id_lambda_us[j] == pion_id_lambda_ls[k])
                        continue;
                    // this helps define the background for lambda - lambda pairs
                    if (q% 2 == 0){     // (q% 2 == 0) checks if the q is divisble by 2
                        h2_lambda_us_lambda_ls->Fill(lambda_us[j].M(), lambda_ls[k].M());
                    } else {
                        h2_lambda_us_lambda_ls->Fill(lambda_us[j].M(), lambda_ls[k].M());
                    }

                    q++; // increase the value by 1

                    // these need to be combined into 1 histogram (but they cannot double count)
                    // h2_lambda_us_lambda_ls->Fill(lambda_us[j].M(), lambda_ls[k].M());
                    // h2_lambda_ls_lambda_us->Fill(lambda_ls[j].M();lambda_us[k].M());
                }
            }

            // this data is all found in the us data 
            // for (size_t j = 0; j < lambda_ls.size(); ++j){
            //     for (size_t k = 0; k < lambda_ls.size(); ++k){
            //         if (proton_id_lambda_ls[j] == proton_id_lambda_ls[k] || pion_id_lambda_ls[j] == pion_id_lambda_ls[k])
            //             continue;
            //     }
            // }


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

            for (size_t j = 0; j < lambda_bar_us.size(); ++j) {
                for (size_t k = j + 1; k < lambda_bar_us.size(); ++k) {
                    if (anti_proton_id_lambda_bar_us[j] == anti_proton_id_lambda_bar_us[k] || anti_pion_id_lambda_bar_us[j] == anti_pion_id_lambda_bar_us[k])
                        continue;
                    h2_lambda_bar_us_lambda_bar_us->Fill(lambda_bar_us[j].M(), lambda_bar_us[k].M());
                }

                for (size_t k = 0; k < lambda_bar_ls.size(); ++k) {
                    if (proton_id_lambda_bar_us[j] == proton_id_lambda_bar_ls[k] || pion_id_lambda_bar_us[j] == pion_id_lambda_bar_ls[k])
                        continue;

                    if (q% 2 == 0){
                        h2_lambda_bar_us_lambda_ls->Fill(lambda_bar_us[j].M(), lambda_bar_ls[k].M());
                    } else {
                        h2_lambda_bar_us_lambda_bar_ls->Fill(lambda_bar_us[j].M(), lambda_bar_ls[k].M());
                    }

                    q++; // increase the value by 1
                }

            }


            // Clear vectors after processing each event
            // lambdas.clear();
            // lambda_bars.clear();
            // proton_ids.clear();
            // pion_ids.clear();
            // anti_proton_ids.clear();
            // anti_pion_ids.clear();

            // lambda_ls.clear();
            // lambda_us.clear();
            // lambda_bar_ls.clear();
            // lambda_bar_us.clear();
        }

        // Update previous event ID
        prevEventID = eventId;
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