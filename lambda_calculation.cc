#include <TLorentzVector.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TROOT.h>
#include <iostream>
#include <cmath>

// Function to compute the invariant mass of two particles
double invariantMass(const TLorentzVector &p1, const TLorentzVector &p2) {
    TLorentzVector p = p1 + p2;
    return p.M();
}

void lambda_calculation() {
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
    Int_t pair_charge;

    ntp_Lambda->SetBranchAddress("p1_pt", &p1_pt);
    ntp_Lambda->SetBranchAddress("p1_eta", &p1_eta);
    ntp_Lambda->SetBranchAddress("p1_phi", &p1_phi);
    ntp_Lambda->SetBranchAddress("p2_pt", &p2_pt);
    ntp_Lambda->SetBranchAddress("p2_eta", &p2_eta);
    ntp_Lambda->SetBranchAddress("p2_phi", &p2_phi);
    ntp_Lambda->SetBranchAddress("pair_charge", &pair_charge);

    TH1F *h1_unlike = new TH1F("h1_unlike", "Invariant Mass of Unlike-Sign Pairs (GeV/c^2)", 1000, 1, 1.4);
    TH1F *h1_like = new TH1F("h1_like", "Invariant Mass of Like-Sign Pairs (GeV/c^2)", 1000, 1, 1.4);
    TH1F *h1 = new TH1F("h1", "Invariant Mass (GeV/c^2)", 1000, 1, 1.4);

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

        h1->Fill(invariant_mass);

        if (pair_charge == 0) {
            h1_unlike->Fill(invariant_mass);
        } else {
            h1_like->Fill(invariant_mass);
        }
    }

    // Create a histogram for the difference data
    TH1F *h1_diff = new TH1F(*h1_unlike); // Clone the unlike-sign pairs histogram

    // Subtract histograms
    h1_diff->Add(h1_like, -1);

    // Determine the range of the peak for fitting
    double peakStart = 1.1;
    double peakEnd = 1.13;

    // Define a combined fit function: polynomial background + Gaussian peak
    TF1 *combinedFit = new TF1("combinedFit", "[0] + [1]*x + [2]*x*x + gaus(3)", peakStart, peakEnd);
    combinedFit->SetParameters(1, 0, 0, h1_diff->GetMaximum(), 1.115, 0.001);

    // Fit the combined function to the data
    h1_diff->Fit(combinedFit, "R");

    // Extract Gaussian parameters
    double constant = combinedFit->GetParameter(3);
    double mean = combinedFit->GetParameter(4);
    double sigma = combinedFit->GetParameter(5);

    // Calculate the FWHM
    double fwhm = 2.35482 * sigma;
    std::cout << "FWHM of the Gaussian fit on h1_diff: " << fwhm << " GeV/c^2" << std::endl;

    // Integrate the bins in the defined range
    int bin_start = h1_diff->FindBin(peakStart);
    int bin_end = h1_diff->FindBin(peakEnd);
    double bin_integral = h1_diff->Integral(bin_start, bin_end);
    std::cout << "Integral of the bins in h1_diff from " << peakStart << " to " << peakEnd << " GeV/c^2: " << bin_integral << std::endl;

    // Integrate the Gaussian fit in the same range
    double fit_integral = combinedFit->Integral(peakStart, peakEnd) - (combinedFit->GetParameter(0) * (peakEnd - peakStart) + combinedFit->GetParameter(1) * (peakEnd*peakEnd - peakStart*peakStart) / 2 + combinedFit->GetParameter(2) * (peakEnd*peakEnd*peakEnd - peakStart*peakStart*peakStart) / 3);
    std::cout << "Integral of the Gaussian fit in h1_diff from " << peakStart << " to " << peakEnd << " GeV/c^2: " << fit_integral << std::endl;

    // Delete any existing canvas with the same name
    if (gROOT->FindObject("c1")) {
        delete gROOT->FindObject("c1");
    }

    TCanvas *c1 = new TCanvas("c1", "Lambda Calculation", 800, 800);
    c1->Divide(2,1);

    // Pad 1: Both like-sign and unlike-sign pairs on the same histogram
    c1->cd(1);
    c1->GetPad(1)->SetGrid();
    h1->SetLineColor(kBlue-3);
    h1->Draw();
    h1->SetXTitle("Invariant Mass (GeV/c^2)");
    h1->SetLineWidth(2);

    // Draw the Gaussian fit on h1 using the parameters from the fit on h1_diff
    // TF1 *gausFitOnH1 = new TF1("gausFitOnH1", "gaus", peakStart, peakEnd);
    // gausFitOnH1->SetParameters(combinedFit->GetParameter(3), combinedFit->GetParameter(4), combinedFit->GetParameter(5));
    // gausFitOnH1->SetLineColor(kRed);
    // gausFitOnH1->SetLineWidth(3);
    // gausFitOnH1->Draw("same");

    // Pad 2: Invariant mass difference (unlike - like)
    c1->cd(2);
    c1->GetPad(2)->SetGrid();
    h1_diff->SetLineColor(kBlue-3);
    h1_diff->Draw();
    h1_diff->SetXTitle("Invariant Mass Difference (GeV/c^2)");
    h1_diff->SetLineWidth(2);

    // Draw the combined fit
    combinedFit->SetLineColor(kRed);
    combinedFit->SetLineWidth(2);
    combinedFit->Draw("same");

    c1->Update();
    c1->WaitPrimitive();

    f->Close();
}

int main() {
    lambda_calculation();
    return 0;
}