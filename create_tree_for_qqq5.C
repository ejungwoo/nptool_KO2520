#include "SimulatedTree.C"
#include "LKLogger.h"

void create_tree_for_qqq5(TString fileName="")
{
    bool verbose = false;

    if (fileName.IsNull())
        fileName = "data/stark_qqq5.sim.root";
    cout << fileName << endl;

    auto file = new TFile(fileName);
    auto ascii1 = (TAsciiFile*) file->Get("G4RandomFinalState");
    auto ascii2 = (TAsciiFile*) file->Get("EventGenerator");
    auto ascii3 = (TAsciiFile*) file->Get("DetectorConfiguration");
    auto tree = (TTree*) file -> Get("SimulatedTree");
    auto numEvents = tree -> GetEntries();
    if (verbose && numEvents>1) numEvents = 10;

    TString fileNameQ = fileName;
    fileNameQ.ReplaceAll(".sim.root",".pana.root");
    auto fileQ = new TFile(fileNameQ,"recreate");
    auto treeQ = new TTree("ana","create_tree_for_qqq5");

    int      b_ti_mult;
    double   b_ti_ke;
    int      b_ti_a;
    int      b_ti_z;
    double   b_ti_tta;
    double   b_ti_phi;
    int      b_multSTARK;
    TVector3 b_pos1;
    TVector3 b_pos2;
    double   b_e1;
    double   b_e2;
    int      b_strip1;
    int      b_strip2;
    bool     b_multi_strip1;
    bool     b_multi_strip2;

    treeQ -> Branch("prim_mult"   , &b_ti_mult);
    treeQ -> Branch("prim_ke"     , &b_ti_ke);
    treeQ -> Branch("prim_a"      , &b_ti_a);
    treeQ -> Branch("prim_z"      , &b_ti_z);
    treeQ -> Branch("prim_tta"    , &b_ti_tta);
    treeQ -> Branch("prim_phi"    , &b_ti_phi);
    treeQ -> Branch("mult"        , &b_multSTARK);
    treeQ -> Branch("pos1"        , &b_pos1);
    treeQ -> Branch("pos2"        , &b_pos2);
    treeQ -> Branch("e1"          , &b_e1);
    treeQ -> Branch("e2"          , &b_e2);
    treeQ -> Branch("strip1"      , &b_strip1);
    treeQ -> Branch("strip2"      , &b_strip2);
    treeQ -> Branch("multi_strip1", &b_multi_strip1);
    treeQ -> Branch("multi_strip2", &b_multi_strip2);

    //treeQ -> SetBranchAddress("",&);

    auto t = new SimulatedTree(tree);

    for (auto iEvent=0; iEvent<numEvents; ++iEvent)
    {
        b_ti_mult = 0;
        b_ti_ke = 0;
        b_ti_a = -1;
        b_ti_z = -1;
        b_ti_tta = 0;
        b_ti_phi = 0;

        t->GetEntry(iEvent);
        if (verbose) cout << endl;
        if (verbose) cout << "Event " << iEvent << endl;

        b_ti_mult= t->TrackInfo -> GetParticleMultiplicity();
        b_ti_ke  = t->TrackInfo -> fTI_Kinetic_Energy[0];
        b_ti_a   = t->TrackInfo -> fTI_A[0];
        b_ti_z   = t->TrackInfo -> fTI_Z[0];
        TString ti_name  = t->TrackInfo -> fTI_Particle_Name[0];
        b_ti_tta = t->TrackInfo -> fTI_Theta[0];
        b_ti_phi = t->TrackInfo -> fTI_Phi[0];
        if (verbose) cout << "name=" << ti_name << " a=" << b_ti_a << " z=" << b_ti_z << endl;
        if (verbose) cout << "ke=" << b_ti_ke << " tta=" << b_ti_tta << endl;

        b_multSTARK = t->STARK -> GetMult();
        if (verbose) cout << "mult_STARK=" << b_multSTARK << endl;
        //
        bool fired_det[8] = {0};
        double eloss[8] = {0};
        b_pos1 = TVector3();
        b_pos2 = TVector3();
        b_strip1 = -1;
        b_strip2 = -1;
        b_multi_strip1 = false;
        b_multi_strip2 = false;
        b_e1 = 0;
        b_e2 = 0;
        if (b_multSTARK==0)
            treeQ -> Fill();
        else
        {
            for (auto iSTARK=0; iSTARK<b_multSTARK; ++iSTARK)
            {
                //Int_t    type = t->STARK -> GetType(iSTARK);
                Int_t    detn = t->STARK -> GetDetN(iSTARK);
                Int_t    fstn = t->STARK -> GetFStN(iSTARK);
                Int_t    bstn = t->STARK -> GetBStN(iSTARK);
                Double_t fre  = t->STARK -> GetFrE (iSTARK);
                Double_t bke  = t->STARK -> GetBkE (iSTARK);
                Double_t upe  = t->STARK -> GetUpE (iSTARK);
                Double_t dwe  = t->STARK -> GetDwE (iSTARK);
                //Double_t t    = t->STARK -> GetT   (iSTARK);
                TVector3 pos = t->STARK -> GetPos (iSTARK);
                if (detn>=1&&detn<=8)
                {
                    fired_det[detn-1] = true;
                    eloss[detn-1] += fre;
                    if (detn>=1&&detn<=4)
                    {
                        if (b_strip1<0) {
                            b_strip1 = fstn;
                            b_pos1 = pos;
                        }
                        else if (b_strip1!=fstn)
                            b_multi_strip1 = true;
                    }
                    else {
                        if (b_strip2<0) {
                            b_strip2 = fstn;
                            b_pos2 = pos;
                        }
                        else if (b_strip2!=fstn)
                            b_multi_strip2 = true;
                    }
                }
                if (verbose) cout << "  " << iSTARK << ") id=" << detn << " st=" << fstn << " f=" << fre << " b=" << bke << " u=" << upe << " d=" << dwe << endl;
            }

            bool fired_layer1 = false;
            for (auto detn : {1,2,3,4})
            {
                if (fired_det[detn-1])
                {
                    fired_layer1 = true;
                    b_e1 = eloss[detn-1];
                    b_e2 = eloss[detn-1+4];
                    if (verbose) cout << "  >> " << detn << " " << b_e1 << " " << b_e2 << endl;
                    break;
                }
            }
            if (fired_layer1==false)
            for (auto detn : {5,6,7,8})
            {
                if (fired_det[detn-1])
                {
                    b_e2 = eloss[detn-1];
                    b_e1 = 0;
                    if (verbose) cout << "  >> " << detn << " " << b_e1 << " " << b_e2 << endl;
                    break;
                }
            }
            treeQ -> Fill();
        }
    }
    fileQ -> cd();
    treeQ -> Write();
    //treeQ -> Scan("*");
    cout << fileQ -> GetName() << endl;
}
