#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TH3F.h>
#include <TCanvas.h>
#include <iostream>
#include <cmath>
#include <vector>

// Define a structure for Four Vector components (px, py, pz, E)
struct FourVector {
    double px;
    double py;
    double pz;
    double E;
};

// Function to compute the invariant mass of two particles using FourVectors
double invariantMass(const FourVector &p1, const FourVector &p2) {
    double m2 = pow(p1.E + p2.E, 2) - pow(p1.px + p2.px, 2) - pow(p1.py + p2.py, 2) - pow(p1.pz + p2.pz, 2);
    return sqrt(m2);
}

void l_pairs4() {
    // Open file
    TFile *f = new TFile("output_6.root", "READ");

    if (!f || f->IsZombie()) {
        std::cerr << "Error: Could not open file 'output_6.root'." << std::endl;
        return;
    }

    // Retrieve TTree
    TTree *ntp_Lambda = dynamic_cast<TTree*>(f->Get("ntp_Lambda"));

    if (!ntp_Lambda) {
        std::cerr << "Error: Could not retrieve tree 'ntp_Lambda' from file 'output_6.root'." << std::endl;
        return;
    }

    // Number of entries in the TTree
    int nEntries = ntp_Lambda->GetEntries();

    // Proton and pion masses
    float mass_p1 = 0.938272;  // Proton mass
    float mass_p2 = 0.139570;  // Pion mass

    // Variables for TTree branches
    float p1_pt, p1_eta, p1_phi;
    float p2_pt, p2_eta, p2_phi;
    Int_t pair_charge, p1_ch;

    // Set TTree branch addresses
    ntp_Lambda->SetBranchAddress("p1_pt", &p1_pt);
    ntp_Lambda->SetBranchAddress("p1_eta", &p1_eta);
    ntp_Lambda->SetBranchAddress("p1_phi", &p1_phi);
    ntp_Lambda->SetBranchAddress("p2_pt", &p2_pt);
    ntp_Lambda->SetBranchAddress("p2_eta", &p2_eta);
    ntp_Lambda->SetBranchAddress("p2_phi", &p2_phi);
    ntp_Lambda->SetBranchAddress("pair_charge", &pair_charge);
    ntp_Lambda->SetBranchAddress("p1_ch", &p1_ch);

    // Vectors to store FourVectors
    std::vector<FourVector> lambdas;
    std::vector<FourVector> lambda_bars;
    lambdas.reserve(nEntries); // Reserve space for efficiency
    lambda_bars.reserve(nEntries);

    // Loop over entries in the TTree
    for (int i = 0; i < nEntries; ++i) {
        ntp_Lambda->GetEntry(i);

        // Compute FourVectors for p1 and p2
        FourVector p1, p2;
        p1.px = p1_pt * cos(p1_phi);
        p1.py = p1_pt * sin(p1_phi);
        p1.pz = p1_pt * sinh(p1_eta);
        p1.E = sqrt(p1.px * p1.px + p1.py * p1.py + p1.pz * p1.pz + mass_p1 * mass_p1);

        p2.px = p2_pt * cos(p2_phi);
        p2.py = p2_pt * sin(p2_phi);
        p2.pz = p2_pt * sinh(p2_eta);
        p2.E = sqrt(p2.px * p2.px + p2.py * p2.py + p2.pz * p2.pz + mass_p2 * mass_p2);

        // Calculate invariant mass
        double mass = invariantMass(p1, p2);

        // Determine the type of lambda and store in respective vectors
        if (p1_ch > 0) {
            lambdas.push_back(p1);
        } else if (p1_ch < 0) {
            lambda_bars.push_back(p1);
        }

        // Print progress every 1000 entries
        if (i % 1000 == 0) {
            std::cout << "Processed " << i << " / " << nEntries << " entries." << std::endl;
        }
    }

    std::cout << "Finished processing entries." << std::endl;

    // Create TH3F histograms
    TH3F *h3_lambda_lambda = new TH3F("h3_lambda_lambda", "Lambda-Lambda Invariant Mass", 3, 0, 5, 3, 0, 5, 3, 0, 5);
    TH3F *h3_lambda_lambda_bar = new TH3F("h3_lambda_lambda_bar", "Lambda-Lambda-bar Invariant Mass", 3, 0, 5, 3, 0, 5, 3, 0, 5);
    TH3F *h3_lambda_bar_lambda_bar = new TH3F("h3_lambda_bar_lambda_bar", "Lambda-bar-Lambda-bar Invariant Mass", 3, 0, 5, 3, 0, 5, 3, 0, 5);

    // Fill histograms for Lambda-Lambda
    for (size_t i = 0; i < lambdas.size(); ++i) {
        for (size_t j = i + 1; j < lambdas.size(); ++j) {
            double mass = invariantMass(lambdas[i], lambdas[j]);
            h3_lambda_lambda->Fill(lambdas[i].px, lambdas[i].py, lambdas[i].pz, mass);
        }
        // Print progress every 100 iterations
        if (i % 100 == 0) {
            std::cout << "Filling Lambda-Lambda histogram: " << i << " / " << lambdas.size() << std::endl;
        }
    }

    // Fill histograms for Lambda-Lambda-bar
    for (size_t i = 0; i < lambdas.size(); ++i) {
        for (size_t j = 0; j < lambda_bars.size(); ++j) {
            double mass = invariantMass(lambdas[i], lambda_bars[j]);
                h3_lambda_lambda_bar->Fill(lambdas[i].px, lambda_bars[j].py, lambda_bars[j].pz, mass);
            }
        // Print progress every 100 iterations
        if (i % 100 == 0) {
            std::cout << "Filling Lambda-Lambda-bar histogram: " << i << " / " << lambdas.size() << std::endl;
        }
    }

    // Fill histograms for Lambda-bar-Lambda-bar
    for (size_t i = 0; i < lambda_bars.size(); ++i) {
        for (size_t j = i + 1; j < lambda_bars.size(); ++j) {
            double mass = invariantMass(lambda_bars[i], lambda_bars[j]);
            h3_lambda_bar_lambda_bar->Fill(lambda_bars[i].px, lambda_bars[i].py, lambda_bars[i].pz, mass);
        }
        // Print progress every 100 iterations
        if (i % 100 == 0) {
            std::cout << "Filling Lambda-bar-Lambda-bar histogram: " << i << " / " << lambda_bars.size() << std::endl;
        }
    }

    // Create TCanvas and draw histograms
    TCanvas *c1 = new TCanvas("c1", "3D Histograms", 1200, 400);
    c1->Divide(3, 1);

    c1->cd(1);
    h3_lambda_lambda->Draw("BOX2");

    c1->cd(2);
    h3_lambda_lambda_bar->Draw("BOX2");

    c1->cd(3);
    h3_lambda_bar_lambda_bar->Draw("BOX2");

    // Cleanup
    delete h3_lambda_lambda;
    delete h3_lambda_lambda_bar;
    delete h3_lambda_bar_lambda_bar;
    delete c1;
    delete f;

    std::cout << "Finished drawing histograms." << std::endl;
}