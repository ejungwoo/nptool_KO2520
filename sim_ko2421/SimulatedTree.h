//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Jul 16 09:45:30 2025 by ROOT version 6.22/06
// from TTree SimulatedTree/Data created / analysed with the nptool package
// found on file: data/stark_qqq5.sim.root
//////////////////////////////////////////////////////////

#ifndef SimulatedTree_h
#define SimulatedTree_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include "TInteractionCoordinates.h"
#include "TSTARKData.h"
#include "TSTARKRaw.h"
#include "TInitialConditions.h"
#include "TReactionConditions.h"
#include "TTrackInfo.h"

class SimulatedTree {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   TInteractionCoordinates *InteractionCoordinates;
   TSTARKData      *STARK;
   TSTARKRaw       *Raw;
   TInitialConditions *InitialConditions;
   TReactionConditions *ReactionConditions;
   TTrackInfo      *TrackInfo;
   Int_t           Run;

   // List of branches
   TBranch        *b_InteractionCoordinates;   //!
   TBranch        *b_STARK;   //!
   TBranch        *b_Raw;   //!
   TBranch        *b_InitialConditions;   //!
   TBranch        *b_ReactionConditions;   //!
   TBranch        *b_TrackInfo;   //!
   TBranch        *b_Run;   //!

   SimulatedTree(TTree *tree=0);
   virtual ~SimulatedTree();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef SimulatedTree_cxx
SimulatedTree::SimulatedTree(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("data/stark_qqq5.sim.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("data/stark_qqq5.sim.root");
      }
      f->GetObject("SimulatedTree",tree);

   }
   Init(tree);
}

SimulatedTree::~SimulatedTree()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t SimulatedTree::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t SimulatedTree::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void SimulatedTree::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   InteractionCoordinates = 0;
   STARK = 0;
   Raw = 0;
   InitialConditions = 0;
   ReactionConditions = 0;
   TrackInfo = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("InteractionCoordinates", &InteractionCoordinates, &b_InteractionCoordinates);
   fChain->SetBranchAddress("STARK", &STARK, &b_STARK);
   fChain->SetBranchAddress("Raw", &Raw, &b_Raw);
   fChain->SetBranchAddress("InitialConditions", &InitialConditions, &b_InitialConditions);
   fChain->SetBranchAddress("ReactionConditions", &ReactionConditions, &b_ReactionConditions);
   fChain->SetBranchAddress("TrackInfo", &TrackInfo, &b_TrackInfo);
   fChain->SetBranchAddress("Run", &Run, &b_Run);
   Notify();
}

Bool_t SimulatedTree::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void SimulatedTree::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t SimulatedTree::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef SimulatedTree_cxx
