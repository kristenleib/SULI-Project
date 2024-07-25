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

void lpair_signal2(){
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
    TH2F *h2_lambda_us_lambda_us = new TH2F("h2_lambda_us_lambda_us", "Unlike Sign Lambda - Unlike Sign Lambda (Includes Background)", 500, 1.0, 1.4, 500, 1.0, 1.4);
    TH2F *h2_lambda_us_lambda_ls = new TH2F("h2_lambda_us_lambda_ls", "Unlike Sign Lambda - Like Sign Lambda (Background)", 500, 1.0, 1.4, 500, 1.0, 1.4);
    TH2F *h2_lambda_us_lambda_bar_us = new TH2F("h2_lambda_us_lambda_bar_us", "Unlike Sign Lambda - Unlike Sign Lambda Bar (Includes Background)", 500, 1.0, 1.4, 500, 1.0, 1.4);
    TH2F *h2_lambda_us_lambda_bar_ls = new TH2F("h2_lambda_us_lambda_bar_ls", "Unlike Sign Lambda - Like Sign Lambda Bar (Background)", 500, 1.0, 1.4, 500, 1.0, 1.4);
    TH2F *h2_lambda_bar_us_lambda_bar_us = new TH2F("h2_lambda_bar_us_lambda_bar_us", "Unlike Sign Lambda Bar - Unlike Sign Lambda Bar (Includes Background)", 500, 1.0, 1.4, 500, 1.0, 1.4);
    TH2F *h2_lambda_bar_us_lambda_bar_ls = new TH2F("h2_lambda_bar_us_lambda_ls", "Unlike Sign Lambda Bar - Like Sign Lambda Bar (Background)", 500, 1.0, 1.4, 500, 1.0, 1.4);

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

        // Pairing the Lambdas and Lambda Bars
        if (eventId != prevEventID && prevEventID != -1){

            // int q = 0;

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
                        h2_lambda_us_lambda_ls->Fill(lambda_ls[k].M(), lambda_us[j].M());
                    }

                    q++; // increase the value by 1
                }
            }

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

            for (size_t j = 0; j < lambda_ls.size(); ++j){
                for (size_t k = 0; k < lambda_bar_us.size(); ++k){
                    if (proton_id_lambda_ls[j] == anti_proton_id_lambda_bar_us[k] || pion_id_lambda_ls[j] == anti_pion_id_lambda_bar_us[k])
                        continue;
                    h2_lambda_us_lambda_bar_ls->Fill(lambda_ls[j].M(), lambda_bar_us[k].M());
                }
            }

            for (size_t j = 0; j < lambda_bar_us.size(); ++j) {
                for (size_t k = j + 1; k < lambda_bar_us.size(); ++k) {
                    if (anti_proton_id_lambda_bar_us[j] == anti_proton_id_lambda_bar_us[k] || anti_pion_id_lambda_bar_us[j] == anti_pion_id_lambda_bar_us[k])
                        continue;
                    h2_lambda_bar_us_lambda_bar_us->Fill(lambda_bar_us[j].M(), lambda_bar_us[k].M());
                }

                for (size_t k = 0; k < lambda_ls.size(); ++k){
                    if (anti_proton_id_lambda_bar_us[j] == anti_proton_id_lambda_bar_ls[k] || anti_pion_id_lambda_bar_us[j] == anti_pion_id_lambda_bar_ls[k])
                        continue;

                    if (b % 2 == 0){
                        h2_lambda_bar_us_lambda_bar_ls->Fill(lambda_bar_us[j].M(), lambda_bar_ls[k].M());
                    } else {
                        h2_lambda_bar_us_lambda_bar_ls->Fill(lambda_bar_ls[k].M(), lambda_bar_us[j].M());
                    }

                    b++; // increase the value by 1
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
        }

        prevEventID = eventId;
    }

    // L Pair Signals
    TH2F *h2_LL_Signal = new TH2F(*h2_lambda_us_lambda_us);
    h2_LL_Signal->Add(h2_lambda_us_lambda_ls, -1.0);

    TH2F *h2_LLBar_Signal = new TH2F(*h2_lambda_us_lambda_bar_us);
    h2_LLBar_Signal->Add(h2_lambda_us_lambda_bar_ls, -1.0);

    TH2F *h2_LBarLBar_Signal = new TH2F(*h2_lambda_bar_us_lambda_bar_us);
    h2_LBarLBar_Signal->Add(h2_lambda_bar_us_lambda_bar_ls, -1.0);

    gStyle->SetPalette(kRainBow);

    // Create TCanvas for displaying histograms
    std::unique_ptr<TCanvas> c1(new TCanvas("c1", "2D Histograms", 1500, 400));
    c1->Divide(3, 1);

    std::unique_ptr<TCanvas> c2(new TCanvas("c2", "X-Y Projections of the 2D histograms", 1500, 400));
    c2->Divide(3, 2);

    // // Define the Bins
    // minX = 1.105;
    // maxX = 1.125;
    // minY = 1.105;
    // maxY = 1.125;

    // c1->cd(1);
    // h2_LL_Signal->GetXaxis()->FindBin(minX, maxX);
    // h2_LL_Signal->GetYaxis()->FindBin(minY, maxY);
    // h2_LL_Signal->Draw("LEGO2");
    // h2_LL_Signal->SetTitle("Lambda - Lambda Pair (Signal)");

    // // Define the 2D Gaussian function
    // TF2 *gaus2D_LL = new TF2("gaus2D_LL", "[0]*TMath::Gaus(x,[1],[2])*TMath::Gaus(y,[3],[4])", 1.11, 1.12, 1.11, 1.12);

    // // Set initial parameters: amplitude, mean x, sigma x, mean y, sigma y
    // gaus2D_LL->SetParameters(1, 1.116, 0.001, 1.116, 0.001);

    // // Fit the 2D Gaussian function to the histogram
    // h2_LL_Signal->Fit("gaus2D_LL", "R0");

    // // Draw the fit result on the same canvas
    // // gaus2D_LL->Draw();

    // c1->cd(2);
    // h2_LLBar_Signal->GetXaxis()->FindBin(minX, maxX);
    // h2_LLBar_Signal->GetYaxis()->FindBin(minY, maxY);
    // h2_LLBar_Signal->Draw("LEGO2");
    // h2_LLBar_Signal->SetTitle("Lambda - Lambda Bar Pair (Signal)");

    // c1->cd(3);
    // h2_LBarLBar_Signal->GetXaxis()->FindBin(minY, maxY);
    // h2_LBarLBar_Signal->GetYaxis()->FindBin(minY, maxY);
    // h2_LBarLBar_Signal->Draw("LEGO2");
    // h2_LL_Signal->SetTitle("Lambda Bar - Lambda Bar Pair (Signal)");

    // // One Dimensional Projections
    // c2->cd(1);
    // TH1D *projX_LL = h2_LL_Signal->ProjectionX("projX_LL");
    // projX_LL->GetXaxis()->FindBin(minX, maxX); // use this for all
    // projX_LL->Draw();
    // projX_LL->SetTitle("Lambda - Lambda Signal (x axis)");
    
    // c2->cd(4);
    // TH1D *projY_LL = h2_LL_Signal->ProjectionY("projY_LL");
    // projY_LL->GetYaxis()->FindBin(minY, maxY);
    // projY_LL->Draw();
    // projY_LL->SetTitle("Lambda - Lambda Signal (y axis)");

    // c2->cd(2);
    // TH1D *projX_LLBar = h2_LLBar_Signal->ProjectionX("projX_LLBar");
    // projX_LLBar->GetXaxis()->FindBin(minX, maxX);
    // projX_LLBar->Draw();
    // projX_LLBar->SetTitle("Lambda - Lambda Bar Signal (x axis)");

    // c2->cd(5);
    // TH1D *projY_LLBar = h2_LLBar_Signal->ProjectionY("projY_LLBar");
    // projY_LLBar->GetYaxis()->FindBin(minY, maxY);
    // projY_LLBar->Draw();
    // projY_LLBar->SetTitle("Lambda - Lambda Bar Signal (y axis)");

    // c2->cd(3);
    // TH1D *projX_LBarLBar = h2_LBarLBar_Signal->ProjectionX("projX_LBarLBar");
    // projX_LBarLBar->GetXaxis()->FindBin(minX, maxX);
    // projX_LBarLBar->Draw();
    // projX_LBarLBar->SetTitle("Lambda Bar - Lambda Bar Signal (x axis)");

    // c2->cd(6);
    // TH1D *projY_LBarLBar = h2_LBarLBar_Signal->ProjectionY("projY_LBarLBar");
    // projY_LBarLBar->GetYaxis()->FindBin(minY, maxY);
    // projY_LBarLBar->Draw();
    // projY_LBarLBar->SetTitle("Lambda Bar - Lambda Bar Signal (y axis)");



    // Define the Bins
    double minX = 1.105;
    double maxX = 1.125;
    double minY = 1.105;
    double maxY = 1.125;

    c1->cd(1);
    // Find the bin numbers for the given x and y ranges
    int binMinX = h2_LL_Signal->GetXaxis()->FindBin(minX);
    int binMaxX = h2_LL_Signal->GetXaxis()->FindBin(maxX);
    int binMinY = h2_LL_Signal->GetYaxis()->FindBin(minY);
    int binMaxY = h2_LL_Signal->GetYaxis()->FindBin(maxY);

    // Set the bin range for drawing
    h2_LL_Signal->GetXaxis()->SetRange(binMinX, binMaxX);
    h2_LL_Signal->GetYaxis()->SetRange(binMinY, binMaxY);
    h2_LL_Signal->Draw("SURF2");
    h2_LL_Signal->SetTitle("Lambda - Lambda Pair (Signal)");

    // Define the 2D Gaussian function
    TF2 *gaus2D_LL = new TF2("gaus2D_LL", "[0]*TMath::Gaus(x,[1],[2])*TMath::Gaus(y,[3],[4])", minX, maxX, minY, maxY);

    // Set initial parameters: amplitude, mean x, sigma x, mean y, sigma y
    gaus2D_LL->SetParameters(1, 1.116, 0.001, 1.116, 0.001);

    // Fit the 2D Gaussian function to the histogram
    h2_LL_Signal->Fit("gaus2D_LL", "R0");

    // Draw the fit result on the same canvas
    // gaus2D_LL->Draw();

    c1->cd(2);
    // Repeat the bin finding and setting process for the next histogram
    binMinX = h2_LLBar_Signal->GetXaxis()->FindBin(minX);
    binMaxX = h2_LLBar_Signal->GetXaxis()->FindBin(maxX);
    binMinY = h2_LLBar_Signal->GetYaxis()->FindBin(minY);
    binMaxY = h2_LLBar_Signal->GetYaxis()->FindBin(maxY);

    h2_LLBar_Signal->GetXaxis()->SetRange(binMinX, binMaxX);
    h2_LLBar_Signal->GetYaxis()->SetRange(binMinY, binMaxY);
    h2_LLBar_Signal->Draw("SURF2");
    h2_LLBar_Signal->SetTitle("Lambda - Lambda Bar Pair (Signal)");

    // Define the 2D Gaussian function for LLBar
    TF2 *gaus2D_LLBar = new TF2("gaus2D_LLBar", "[0]*TMath::Gaus(x,[1],[2])*TMath::Gaus(y,[3],[4])", minX, maxX, minY, maxY);

    // Set initial parameters: amplitude, mean x, sigma x, mean y, sigma y
    gaus2D_LLBar->SetParameters(1, 1.116, 0.001, 1.116, 0.001);

    // Fit the 2D Gaussian function to the histogram
    h2_LLBar_Signal->Fit("gaus2D_LLBar", "R0");

    c1->cd(3);
    binMinX = h2_LBarLBar_Signal->GetXaxis()->FindBin(minX);
    binMaxX = h2_LBarLBar_Signal->GetXaxis()->FindBin(maxX);
    binMinY = h2_LBarLBar_Signal->GetYaxis()->FindBin(minY);
    binMaxY = h2_LBarLBar_Signal->GetYaxis()->FindBin(maxY);

    h2_LBarLBar_Signal->GetXaxis()->SetRange(binMinX, binMaxX);
    h2_LBarLBar_Signal->GetYaxis()->SetRange(binMinY, binMaxY);
    h2_LBarLBar_Signal->Draw("SURF2");
    h2_LBarLBar_Signal->SetTitle("Lambda Bar - Lambda Bar Pair (Signal)");

    // Define the 2D Gaussian function for LBarLBar
    TF2 *gaus2D_LBarLBar = new TF2("gaus2D_LBarLBar", "[0]*TMath::Gaus(x,[1],[2])*TMath::Gaus(y,[3],[4])", minX, maxX, minY, maxY);

    // Set initial parameters: amplitude, mean x, sigma x, mean y, sigma y
    gaus2D_LBarLBar->SetParameters(1, 1.116, 0.001, 1.116, 0.001);

    // Fit the 2D Gaussian function to the histogram
    h2_LBarLBar_Signal->Fit("gaus2D_LBarLBar", "R0");

    // One Dimensional Projections
    c2->cd(1);
    TH1D *projX_LL = h2_LL_Signal->ProjectionX("projX_LL", binMinX, binMaxX);
    projX_LL->GetXaxis()->SetRange(binMinX, binMaxX);
    projX_LL->Draw();
    projX_LL->SetTitle("Lambda - Lambda Signal (x axis)");

    c2->cd(4);
    TH1D *projY_LL = h2_LL_Signal->ProjectionY("projY_LL", binMinY, binMaxY);
    projY_LL->GetYaxis()->SetRange(binMinY, binMaxY);
    projY_LL->Draw();
    projY_LL->SetTitle("Lambda - Lambda Signal (y axis)");

    c2->cd(2);
    TH1D *projX_LLBar = h2_LLBar_Signal->ProjectionX("projX_LLBar", binMinX, binMaxX);
    projX_LLBar->GetXaxis()->SetRange(binMinX, binMaxX);
    projX_LLBar->Draw();
    projX_LLBar->SetTitle("Lambda - Lambda Bar Signal (x axis)");

    c2->cd(5);
    TH1D *projY_LLBar = h2_LLBar_Signal->ProjectionY("projY_LLBar", binMinY, binMaxY);
    projY_LLBar->GetYaxis()->SetRange(binMinY, binMaxY);
    projY_LLBar->Draw();
    projY_LLBar->SetTitle("Lambda - Lambda Bar Signal (y axis)");

    c2->cd(3);
    TH1D *projX_LBarLBar = h2_LBarLBar_Signal->ProjectionX("projX_LBarLBar", binMinX, binMaxX);
    projX_LBarLBar->GetXaxis()->SetRange(binMinX, binMaxX);
    projX_LBarLBar->Draw();
    projX_LBarLBar->SetTitle("Lambda Bar - Lambda Bar Signal (x axis)");

    c2->cd(6);
    TH1D *projY_LBarLBar = h2_LBarLBar_Signal->ProjectionY("projY_LBarLBar", binMinY, binMaxY);
    projY_LBarLBar->GetYaxis()->SetRange(binMinY, binMaxY);
    projY_LBarLBar->Draw();
    projY_LBarLBar->SetTitle("Lambda Bar - Lambda Bar Signal (y axis)");



    // Update canvases
    c1->Update();
    c2->Update();

    // Wait for user input or handle events
    c1->WaitPrimitive();
    c2->WaitPrimitive();

    f->Close();

    // end of macro
}

int main() {
    lpair_signal2();
    return 0;
}