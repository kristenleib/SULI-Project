#include <TLorentzVector.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TLegend.h>
#include <iostream>
#include <cmath>
#include <vector>

// Function to compute the invariant mass of two particles
double invariantMass(const TLorentzVector &p1, const TLorentzVector &p2) {
    TLorentzVector p = p1 + p2;
    return p.M();
}

void test3() {
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

    // Define bin edges for variable bin-width histogram
    std::vector<double> bin_edges;
    double low = 1.0;
    double high = 1.4;
    double peak_region_start = 1.11;
    double peak_region_end = 1.123;
    double bin_width_peak = 0.00025;  // Smaller bin width in the peak region
    double bin_width_background = 0.01;  // Larger bin width in the background region

    // Fill the bin edges with variable widths
    for (double x = low; x < peak_region_start; x += bin_width_background) {
        bin_edges.push_back(x);
    }
    for (double x = peak_region_start; x < peak_region_end; x += bin_width_peak) {
        bin_edges.push_back(x);
    }
    for (double x = peak_region_end; x <= high; x += bin_width_background) {
        bin_edges.push_back(x);
    }

    // Create histograms with variable bin-widths
    TH1F *h1_lambda = new TH1F("h1_lambda", "Invariant Mass of #Lambda (GeV/c^2)", bin_edges.size() - 1, &bin_edges[0]);
    TH1F *h1_lambda_bar = new TH1F("h1_lambda_bar", "Invariant Mass of #bar{#Lambda} (GeV/c^2)", bin_edges.size() - 1, &bin_edges[0]);

    TH1F *h1_unlike_l = new TH1F("h1_unlike_l", "#Lambda Signal (GeV/c^2)", bin_edges.size() - 1, &bin_edges[0]);
    TH1F *h1_like_l = new TH1F("h1_like_l", "Invariant Mass of Like-Sign Pairs (GeV/c^2)", bin_edges.size() - 1, &bin_edges[0]);

    TH1F *h1_unlike_lbar = new TH1F("h1_unlike_lbar", "#bar{#Lambda} Signal (GeV/c^2)", bin_edges.size() - 1, &bin_edges[0]);
    TH1F *h1_like_lbar = new TH1F("h1_like_lbar", "Invariant Mass of Like-Sign Pairs (GeV/c^2)", bin_edges.size() - 1, &bin_edges[0]);

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

    // Create histograms for the background subtraction
    TH1F *h1_diff_l = new TH1F(*h1_unlike_l);
    TH1F *h1_diff_lbar = new TH1F(*h1_unlike_lbar);

    // Subtract histograms
    h1_diff_l->Add(h1_like_l, -1);
    h1_diff_lbar->Add(h1_like_lbar, -1);

    // Determine the range of the peak for fitting
    double peakStart = 1.11;
    double peakEnd = 1.123;

    // Define combined fit functions: polynomial background + Double Gaussian peak
    TF1 *combinedFit_l = new TF1("combinedFit_l", "[0] + [1]*x + [2]*x*x + gaus(3) + gaus(6)", peakStart, peakEnd);
    combinedFit_l->SetParameters(1, 0, 0, h1_diff_l->GetMaximum(), 1.115, 0.001, h1_diff_l->GetMaximum()/2, 1.115, 0.002);
    h1_diff_l->Fit(combinedFit_l, "R");

    TF1 *combinedFit_lbar = new TF1("combinedFit_lbar", "[0] + [1]*x + [2]*x*x + gaus(3) + gaus(6)", peakStart, peakEnd);
    combinedFit_lbar->SetParameters(1, 0, 0, h1_diff_lbar->GetMaximum(), 1.115, 0.001, h1_diff_lbar->GetMaximum()/2, 1.115, 0.002);
    h1_diff_lbar->Fit(combinedFit_lbar, "R");

    // Extract Gaussian parameters
    double constant_l = combinedFit_l->GetParameter(3);
    double mean_l = combinedFit_l->GetParameter(4);
    double sigma_l = combinedFit_l->GetParameter(5);
    double fwhm_l = 2.35482 * sigma_l;

    double constant_lbar = combinedFit_lbar->GetParameter(3);
    double mean_lbar = combinedFit_lbar->GetParameter(4);
    double sigma_lbar = combinedFit_lbar->GetParameter(5);
    double fwhm_lbar = 2.35482 * sigma_lbar;

    std::cout << "FWHM of the Gaussian fit on h1_diff_l: " << fwhm_l << " GeV/c^2" << std::endl;
    std::cout << "FWHM of the Gaussian fit on h1_diff_lbar: " << fwhm_lbar << " GeV/c^2" << std::endl;

    // Integrate the bins in the defined range
    int bin_start_l = h1_diff_l->FindBin(peakStart);
    int bin_end_l = h1_diff_l->FindBin(peakEnd);
    double bin_integral_l = h1_diff_l->Integral(bin_start_l, bin_end_l);

    int bin_start_lbar = h1_diff_lbar->FindBin(peakStart);
    int bin_end_lbar = h1_diff_lbar->FindBin(peakEnd);
    double bin_integral_lbar = h1_diff_lbar->Integral(bin_start_lbar, bin_end_lbar);

    std::cout << "Integral of h1_diff_l from " << peakStart << " to " << peakEnd << " GeV/c^2: " << bin_integral_l << std::endl;
    std::cout << "Integral of h1_diff_lbar from " << peakStart << " to " << peakEnd << " GeV/c^2: " << bin_integral_lbar << std::endl;

    // Integrate the Gaussian fit functions in the same range
    double fit_integral_l = combinedFit_l->Integral(peakStart, peakEnd) - (combinedFit_l->GetParameter(0) * (peakEnd - peakStart) + combinedFit_l->GetParameter(1) * (peakEnd*peakEnd - peakStart*peakStart) / 2 + combinedFit_l->GetParameter(2) * (peakEnd*peakEnd*peakEnd - peakStart*peakStart*peakStart) / 3);
    std::cout << "Integral of the Gaussian fit in h1_diff_l from " << peakStart << " to " << peakEnd << " GeV/c^2: " << fit_integral_l << std::endl;

    double fit_integral_lbar = combinedFit_lbar->Integral(peakStart, peakEnd) - (combinedFit_lbar->GetParameter(0) * (peakEnd - peakStart) + combinedFit_lbar->GetParameter(1) * (peakEnd*peakEnd - peakStart*peakStart) / 2 + combinedFit_lbar->GetParameter(2) * (peakEnd*peakEnd*peakEnd - peakStart*peakStart*peakStart) / 3);
    std::cout << "Integral of the Gaussian fit in h1_diff_lbar from " << peakStart << " to " << peakEnd << " GeV/c^2: " << fit_integral_lbar << std::endl;

    TCanvas *c2 = new TCanvas("c2", "Lambda and Lambda-bar Invariant Mass", 1200, 600);
    c2->Divide(2, 2);

    // Pad 1: Invariant mass of Lambda
    c2->cd(1);
    gPad->SetGrid();
    h1_lambda->SetLineColor(kBlue);
    h1_lambda->Draw();
    h1_lambda->SetXTitle("Invariant Mass (GeV/c^2)");
    h1_lambda->SetYTitle("Counts");
    h1_lambda->SetLineWidth(3);

    // Pad 2: Invariant mass of Lambda-bar
    c2->cd(2);
    gPad->SetGrid();
    h1_lambda_bar->SetLineColor(kBlue);
    h1_lambda_bar->Draw();
    h1_lambda_bar->SetXTitle("Invariant Mass (GeV/c^2)");
    h1_lambda_bar->SetYTitle("Counts");
    h1_lambda_bar->SetLineWidth(3);

    // Pad 3: Lambda Signal
    c2->cd(3);
    gPad->SetGrid();
    h1_diff_l->SetLineColor(kBlue);
    h1_diff_l->Draw();
    h1_diff_l->SetXTitle("Invariant Mass (GeV/c^2)");
    h1_diff_l->SetYTitle("Counts");
    h1_diff_l->SetLineWidth(3);
    h1_diff_l->GetXaxis()->SetRangeUser(1.11, 1.123);
    // Draw the Gaussian fit
    combinedFit_l->SetLineColor(kRed);
    combinedFit_l->SetLineWidth(3);
    combinedFit_l->Draw("same");

    // Pad 4: Lambda-bar Signal
    c2->cd(4);
    gPad->SetGrid();
    h1_diff_lbar->SetLineColor(kBlue);
    h1_diff_lbar->Draw();
    h1_diff_lbar->SetXTitle("Invariant Mass (GeV/c^2)");
    h1_diff_lbar->SetYTitle("Counts");
    h1_diff_lbar->SetLineWidth(3);
    h1_diff_lbar->GetXaxis()->SetRangeUser(1.11, 1.123);
    combinedFit_lbar->SetLineColor(kRed);
    combinedFit_lbar->SetLineWidth(3);
    combinedFit_lbar->Draw("same");

    c2->Update();
    c2->WaitPrimitive();

    // Cleanup
    delete h1_lambda;
    delete h1_lambda_bar;
    delete h1_unlike_l;
    delete h1_like_l;
    delete h1_unlike_lbar;
    delete h1_like_lbar;
    delete h1_diff_l;
    delete h1_diff_lbar;
    delete combinedFit_l;
    delete combinedFit_lbar;
    delete c2;

    f->Close();
    delete f;
}

int main() {
    test3();
    return 0;
}
