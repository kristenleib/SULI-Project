#include <iostream>
#include <map>

#include "TFile.h"
#include "TTree.h"
#include "TH2I.h"
#include "TH3I.h"
#include "TCanvas.h"

void analyzeCombinations() {
    // Open the ROOT file
    TFile *f = new TFile("output_6.root", "READ");

    // Check if the file was opened successfully
    if (!f || f->IsZombie()) {
        std::cerr << "Error: Could not open file 'output_6.root'." << std::endl;
        return;
    }

    // Retrieve the TTree containing Lambda particle data
    TTree *ntp_Lambda = (TTree*) f->Get("ntp_Lambda");

    // Check if the tree was retrieved successfully
    if (!ntp_Lambda) {
        std::cerr << "Error: Could not retrieve tree 'ntp_Lambda' from file 'output_6.root'." << std::endl;
        f->Close();
        delete f;
        return;
    }

    // Variables to store daughter particle IDs
    Int_t p1_InEventID, p2_InEventID;

    // Set branch addresses for reading from the TTree
    ntp_Lambda->SetBranchAddress("p1_InEventID", &p1_InEventID);
    ntp_Lambda->SetBranchAddress("p2_InEventID", &p2_InEventID);

    // Create a map to count combinations of p1_InEventID and p2_InEventID
    std::map<std::pair<int, int>, int> combinationCount;

    // Loop over all entries in the TTree
    Long64_t nEntries = ntp_Lambda->GetEntries();
    for (Long64_t i = 0; i < nEntries; i++) {
        ntp_Lambda->GetEntry(i);
        // Form a pair representing the combination
        std::pair<int, int> combination(p1_InEventID, p2_InEventID);
        // Increment the count for this combination
        combinationCount[combination]++;
    }

    // Close the ROOT file
    f->Close();
    delete f;

    // Create a 2D histogram to visualize the combination counts
    TH2I *h2_combinations = new TH2I("h2_combinations", "Combinations of p1_InEventID and p2_InEventID",
                                    100, 0, 950, 100, 0, 950);

    // Fill the histogram with the combination counts
    for (const auto& entry : combinationCount) {
        h2_combinations->Fill(entry.first.first, entry.first.second, entry.second);
    }

    // Create a 3D histogram to visualize the combination counts
    TH3I *h3_combinations = new TH3I("h3_combinations", "Combinations of p1_InEventID, p2_InEventID, and counts",
                                     100, 0, 950, 100, 0, 950, 100, 0, 10000);

    // Fill the 3D histogram with the combination counts
    for (const auto& entry : combinationCount) {
        h3_combinations->Fill(entry.first.first, entry.first.second, entry.second);
    }

    // Create a canvas for the 2D histogram
    TCanvas *c1 = new TCanvas("c1", "2D Combination Analysis", 800, 600);
    h2_combinations->Draw("colz"); // "colz" option for a 2D color plot
    h2_combinations->SetXTitle("p1_InEventID");
    h2_combinations->SetYTitle("p2_InEventID");
    h2_combinations->SetTitle("Combinations of p1_InEventID and p2_InEventID");

    // Create a canvas for the 3D histogram
    TCanvas *c2 = new TCanvas("c2", "3D Combination Analysis", 800, 600);
    h3_combinations->Draw(); // Draw the 3D histogram

    // Add labels and titles to the 3D histogram
    h3_combinations->SetXTitle("p1_InEventID");
    h3_combinations->SetYTitle("p2_InEventID");
    h3_combinations->SetZTitle("Counts");
    h3_combinations->SetTitle("Combinations of p1_InEventID, p2_InEventID, and counts");

    // Save the canvases as images (optional)
    c1->SaveAs("2d_combination_plot.png");
    c2->SaveAs("3d_combination_plot.png");

    // Optionally print out the combination counts
    for (const auto& entry : combinationCount) {
        std::cout << "Combination (" << entry.first.first << ", " << entry.first.second
                  << ") count: " << entry.second << std::endl;
    }
}

int main() {
    analyzeCombinations();
    return 0;
}