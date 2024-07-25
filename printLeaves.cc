#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "TLeaf.h"

void printLeaves() {
    // Open the ROOT file
    TFile *f = new TFile("output_6.root", "READ");

    // Check if the file was opened successfully
    if (!f || f->IsZombie()) {
        std::cerr << "Error: Could not open file 'output_6.root'." << std::endl;
        return;
    }

    // Get the TTree object from the file
    TTree *ntp_Lambda = (TTree*) f->Get("ntp_Lambda");

    // Check if the TTree was retrieved successfully
    if (!ntp_Lambda) {
        std::cerr << "Error: Could not retrieve TTree 'ntp_Lambda'." << std::endl;
        f->Close();
        return;
    }

    // Get the list of leaves
    TObjArray *leaves = ntp_Lambda->GetListOfLeaves();
    if (!leaves) {
        std::cerr << "Error: Could not retrieve list of leaves from 'ntp_Lambda'." << std::endl;
        f->Close();
        return;
    }

    // Print the names of all the leaves
    std::cout << "List of leaves in the TTree 'ntp_Lambda':" << std::endl;
    for (int i = 0; i < leaves->GetEntries(); ++i) {
        TLeaf *leaf = (TLeaf*) leaves->At(i);
        if (leaf) {
            std::cout << leaf->GetName() << std::endl;
        }
    }

    // Close the file
    f->Close();
}