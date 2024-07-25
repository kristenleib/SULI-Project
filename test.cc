#include <TLorentzVector.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TLegend.h>
#include <TROOT.h>
#include <TSystem.h>
#include <iostream>
#include <cmath>

#define SafeDelete(p) { if(p) { delete p; p = nullptr; } }

// Function to compute the invariant mass of two particles
double invariantMass(const TLorentzVector &p1, const TLorentzVector &p2) {
    TLorentzVector p = p1 + p2;
    return p.M();
}

void test() {
    TFile *f = new TFile("output_6.root", "READ");

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
        SafeDelete(f);
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

    TH1F *h1_lambda = new TH1F("h1_lambda", "Invariant Mass of #Lambda (GeV/c^2)", 300, 1, 1.4);
    TH1F *h1_lambda_bar = new TH1F("h1_lambda_bar", "Invariant Mass of #bar{#Lambda} (GeV/c^2)", 300, 1, 1.4);

    TH1F *h1_unlike_l = new TH1F("h1_unlike_l", "#Lambda Signal (GeV/c^2)", 300, 1, 1.4);
    TH1F *h1_like_l = new TH1F("h1_like_l", "Invariant Mass of Like-Sign Pairs (GeV/c^2)", 300, 1, 1.4);

    TH1F *h1_unlike_lbar = new TH1F("h1_unlike_lbar", "#bar{#Lambda} Signal (GeV/c^2)", 300, 1, 1.4);
    TH1F *h1_like_lbar = new TH1F("h1_like_lbar", "Invariant Mass of Like-Sign Pairs (GeV/c^2)", 300, 1, 1.4);

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

        if (p1_ch > 0) {
            h1_lambda->Fill(invariant_mass); // Lambda
        } else if (p1_ch < 0) {
            h1_lambda_bar->Fill(invariant_mass); // Lambda-bar
        }

        if (pair_charge == 0) {
            h1_unlike_l->Fill(invariant_mass);
            h1_unlike_lbar->Fill(invariant_mass);
        } else {
            h1_like_l->Fill(invariant_mass);
            h1_like_lbar->Fill(invariant_mass);
        }
    }

    // Define the 2D histogram
    TH2F *h2_combined = new TH2F("h2_combined", "Combined Invariant Mass of #Lambda and #bar{#Lambda} (GeV/c^2)",
                                 300, 1, 1.4, 300, 1, 1.4);

    // Fill the 2D histogram by iterating over the bins of the 1D histograms
    for (int i = 1; i <= h1_lambda->GetNbinsX(); ++i) {
        for (int j = 1; j <= h1_lambda_bar->GetNbinsX(); ++j) {
            float bin_content_lambda = h1_lambda->GetBinContent(i);
            float bin_content_lambda_bar = h1_lambda_bar->GetBinContent(j);
            h2_combined->SetBinContent(i, j, bin_content_lambda * bin_content_lambda_bar);
        }
    }

    // Create histograms for the background subtraction
    TH1F *h1_diff_l = new TH1F(*h1_unlike_l);
    TH1F *h1_diff_lbar = new TH1F(*h1_unlike_lbar);

    // Subtract histograms
    h1_diff_l->Add(h1_like_l, -1);
    h1_diff_lbar->Add(h1_like_lbar, -1);

    // Determine the range of the peak for fitting
    double peakStart = 1.1;
    double peakEnd = 1.13;

    // Fit a Gaussian to the peak region of the difference histogram
    TF1 *gausFit_l = new TF1("gausFit_l", "gaus", peakStart, peakEnd);
    h1_diff_l->Fit(gausFit_l, "R");

    TF1 *gausFit_lbar = new TF1("gausFit_lbar", "gaus", peakStart, peakEnd);
    h1_diff_lbar->Fit(gausFit_lbar, "R");

    // Get the standard deviation (sigma) of the Gaussian
    double sigma_l = gausFit_l->GetParameter(2);
    double sigma_lbar = gausFit_lbar->GetParameter(2);

    // Calculate the FWHM
    double fwhm_l = 2.35482 * sigma_l;
    std::cout << "FWHM of the Gaussian fit on h1_diff_l: " << fwhm_l << " GeV/c^2" << std::endl;
    double fwhm_lbar = 2.35482 * sigma_lbar;
    std::cout << "FWHM of the Gaussian fit on h1_diff_lbar: " << fwhm_lbar << " GeV/c^2" << std::endl;

    TCanvas *canvas1 = new TCanvas("canvas1", "Lambda and Lambda-bar Invariant Mass", 1200, 600);
    canvas1->Divide(2, 1);

    // Pad 1: Invariant mass of Lambda
    canvas1->cd(1);
    gPad->SetGrid();
    h1_lambda->SetLineColor(kBlue);
    h1_lambda->Draw();
    h1_lambda->SetXTitle("Invariant Mass (GeV/c^2)");
    h1_lambda->SetYTitle("Counts");
    h1_lambda->SetLineWidth(3);

    // Pad 2: Invariant mass of Lambda-bar
    canvas1->cd(2);
    gPad->SetGrid();
    h1_lambda_bar->SetLineColor(kBlue);
    h1_lambda_bar->Draw();
    h1_lambda_bar->SetXTitle("Invariant Mass (GeV/c^2)");
    h1_lambda_bar->SetYTitle("Counts");
    h1_lambda_bar->SetLineWidth(3);

    canvas1->Update();

    // Draw the 2D histogram
    TCanvas *canvas2 = new TCanvas("canvas2", "Combined Invariant Mass of Lambda and Lambda-bar", 800, 600);
    h2_combined->Draw("COLZ");

    canvas2->Update();
    canvas2->WaitPrimitive();

    // Cleanup
    SafeDelete(h1_lambda);
    SafeDelete(h1_lambda_bar);
    SafeDelete(h1_unlike_l);
    SafeDelete(h1_like_l);
    SafeDelete(h1_unlike_lbar);
    SafeDelete(h1_like_lbar);
    SafeDelete(h1_diff_l);
    SafeDelete(h1_diff_lbar);
    SafeDelete(gausFit_l);
    SafeDelete(gausFit_lbar);
    SafeDelete(h2_combined);

    f->Close();
    SafeDelete(f);
}

int main(int argc, char **argv) {
    // Initialize ROOT
    TApplication app("ROOT Application", &argc, argv);
    test();
    return 0;
}