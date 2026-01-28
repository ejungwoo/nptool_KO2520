#include "SimulatedTree.C"
#include "LKLogger.h"

void Init();
void End();
void FillHit(int hitType, int type, int detn, int fstn, int bstn, double fre, double bke, double upe, double dwe, TVector3 pos);
void FillTrack(int iTrack, double ke, int a, int z, double tta, double phi);
void FillEvent(int nTracks, int nHits);
void FillGoodEvent(double energy, TVector3 pos);
double ThetaComToLab(double theta_com) { return (180. - theta_com)/2.; }
double ThetaLabToCom(double theta_lab) { return (180. - 2*theta_lab); }

void draw_ko2520(TString fileName="")
{
    bool test = 0;
    Init();

    if (fileName.IsNull())
        //fileName = "data/stark_ko2520_21Na.21Na_elastic.sim.root";
        fileName = "data/stark_ko2520_21Na.21Na_elastic.sim.root";
    cout << fileName << endl;

    auto file = new TFile(fileName);
    auto tree = (TTree*) file -> Get("SimulatedTree");
    auto numEvents = tree -> GetEntries();
    if (test && numEvents>20) numEvents = 20;

    auto file_ereco = new TFile("../sim_qqq5_layers/energy_restoration.root");
    auto graph_ereco = (TGraph2D*) file_ereco -> Get("energy_restoration");

    const int max_detectors = 50;

    auto t = new SimulatedTree(tree);
    for (auto iEvent=0; iEvent<numEvents; ++iEvent)
    {
        t -> GetEntry(iEvent);
        if (test) cout << endl;
        if (test) e_test << "Event " << iEvent << endl;

        auto nTracks= t->TrackInfo -> GetParticleMultiplicity();
        for (auto iTrack=0; iTrack<nTracks; ++iTrack)
        {
            auto ke  = t->TrackInfo -> fTI_Kinetic_Energy[iTrack];
            auto a   = t->TrackInfo -> fTI_A[iTrack];
            auto z   = t->TrackInfo -> fTI_Z[iTrack];
            auto tta = t->TrackInfo -> fTI_Theta[iTrack];
            auto phi = t->TrackInfo -> fTI_Phi[iTrack];
            FillTrack(iTrack,ke,a,z,tta,phi);

            if (test) {
                TString name  = t->TrackInfo -> fTI_Particle_Name[iTrack];
                e_test << iTrack << ") name=" << name << ", a=" << a << ", z=" << z << ", ke=" << ke << ", tta=" << tta << endl;
            }
        }

        auto nHits = t->STARK -> GetMult();
        if (test) e_test << "mult_STARK=" << nHits << endl;
        for (auto iHit=0; iHit<nHits; ++iHit)
        {
            int    type = t->STARK -> GetType(iHit);
            int    detn = t->STARK -> GetDetN(iHit);
            int    fstn = t->STARK -> GetFStN(iHit);
            int    bstn = t->STARK -> GetBStN(iHit);
            double fre  = t->STARK -> GetFrE (iHit);
            double bke  = t->STARK -> GetBkE (iHit);
            double upe  = t->STARK -> GetUpE (iHit);
            double dwe  = t->STARK -> GetDwE (iHit);
            TVector3 pos= t->STARK -> GetPos (iHit);

            int hitType = 0;
            FillHit(hitType, type, detn, fstn, bstn, fre, bke, upe, dwe, pos);
        }

        FillEvent(nTracks, nHits);

        int goodEvent = -1;
        double energyGood;
        TVector3 posGood;

        if (nHits==1)
        {
            int    type = t->STARK -> GetType(0);
            int    detn = t->STARK -> GetDetN(0);
            int    fstn = t->STARK -> GetFStN(0);
            int    bstn = t->STARK -> GetBStN(0);
            double fre  = t->STARK -> GetFrE (0);
            TVector3 pos= t->STARK -> GetPos (0);
            if (detn>24 && detn<=40)
            {
                if (bstn<4) {
                    energyGood = fre;
                    posGood = pos;
                    goodEvent = 0;
                }
            }

        }
        else if (nHits==2)
        {
            int      type0 = t->STARK -> GetType(0);
            int      detn0 = t->STARK -> GetDetN(0);
            int      fstn0 = t->STARK -> GetFStN(0);
            int      bstn0 = t->STARK -> GetBStN(0);
            double   fre0  = t->STARK -> GetFrE (0);
            TVector3 pos0  = t->STARK -> GetPos (0);
            int      type1 = t->STARK -> GetType(1);
            int      detn1 = t->STARK -> GetDetN(1);
            int      fstn1 = t->STARK -> GetFStN(1);
            int      bstn1 = t->STARK -> GetBStN(1);
            double   fre1  = t->STARK -> GetFrE (1);
            TVector3 pos1  = t->STARK -> GetPos (1);
            energyGood = fre0 + fre1;

            auto detector_number_difference = (detn1 - detn0);
            if (detector_number_difference<0) {
                auto pos2 = pos0;
                pos0 = pos1;
                pos1 = pos2;
                auto fstn2 = fstn0;
                fstn0 = fstn1;
                fstn1 = fstn2;
                auto detn2 = detn0;
                detn0 = detn1;
                detn1 = detn2;
            }
            if (abs(detector_number_difference)==12 && detn0<=24 && detn1<=24)
            {
                if (bstn0<4)
                {
                    posGood = pos0;
                    goodEvent = 1;
                }
                //if (energyGood < ThetaLabToCom(posGood.Theta()*180/pi)*(20./123) && ThetaLabToCom(posGood.Theta()*180/pi)>116) {
                //    lk_debug << energyGood << " " << fstn0 << " " << bstn0 << " " << fstn1 << " " << bstn1 << endl;
                //    goodEvent = 1;
                //}

            }
            else if (type0==2 && type1==2)
            {
                posGood = pos0;
                if (detn0>detn1)
                {
                    auto detn9 = detn0;
                    detn0 = detn1;
                    detn1 = detn9;
                }
                double energyGood1 = energyGood;
                //energyGood = graph_ereco -> Interpolate(fre0,fre1);
                if (detn0<=42 && detn1>42)
                    goodEvent = 2;
            }
        }

        if (goodEvent>=0)
        //if (goodEvent==1)
            FillGoodEvent(energyGood, posGood);
    }

    End();
}

LKDrawingGroup *fTop;
TH2D *fHistAA[4];
TH1D *fHistTTA[4];
TH1D* fHistDetn;
TH2D* fHistDetn2;
TH2D* fHistDetn3;
TH2D* fHistDetn4;
TH1D *fHistNHits;
TH1D *fHistNTracks;
TH2D *fHistEnergyTheta;
TH1D *fHistTheta;

void Init()
{
    fTop = new LKDrawingGroup("draw_ko2520");
    auto group3 = fTop -> CreateGroup("good_event",true);
    auto group1 = fTop -> CreateGroup("track",true);
    auto group2 = fTop -> CreateGroup("hit",true);
    auto group0 = fTop -> CreateGroup("event",true);

    //for (auto i : {0,1,2,3}) {
    for (auto i : {0,1}) {
        fHistTTA[i] = new TH1D(Form("fHistTTA_%d",i),Form("track info (%s);theta (deg);yield",TString((i==0?"proton":"all")).Data()),2*180, 0,180);
        group1 -> CreateDrawing() -> Add(fHistTTA[i]);
    }
    //for (auto i : {0,1,2,3}) {
    for (auto i : {0,1}) {
        fHistAA[i] = new TH2D(Form("fHistAA_%d",i),Form("track info (%s);theta (deg);phi (deg)",TString((i==0?"proton":"all")).Data()),180, 0,180,180,-180,180);
        auto draw = group1 -> CreateDrawing();
        draw -> Add(fHistAA[i]);
        draw -> SetOptStat(1111111);
    }

    auto drawET = group3 -> CreateDrawing();
    drawET -> SetStatCorner(1);

    fHistDetn        = new TH1D("fHistDetn",       "hit info;detn",50,0,50); group2 -> CreateDrawing() -> Add(fHistDetn);
    fHistDetn2       = new TH2D("fHistDetn2",      "hit info;detn;type",50,0,50,4,0,4); group2 -> CreateDrawing() -> Add(fHistDetn2);
    fHistDetn3       = new TH2D("fHistDetn3",      "hit info;detn;stripf",50,0,50,40,0,40); group2 -> CreateDrawing() -> Add(fHistDetn3);
    fHistDetn4       = new TH2D("fHistDetn4",      "hit info;detn;stripb",50,0,50,40,0,40); group2 -> CreateDrawing() -> Add(fHistDetn4);
    fHistEnergyTheta = new TH2D("fHistEnergyTheta","hit info;theta_{CM};e",180,0,180,150,0,30); drawET -> Add(fHistEnergyTheta);
    fHistTheta       = new TH1D("fHistTheta",      "hit info;theta_{CM}",180,0,180); group3 -> CreateDrawing() -> Add(fHistTheta);

    fHistNHits = new TH1D("nHits","event info",10,0,10);
    group0 -> CreateDrawing() -> Add(fHistNHits);
    fHistNTracks = new TH1D("nTracks","event info",10,0,10);
    group0 -> CreateDrawing() -> Add(fHistNTracks);
}

void FillTrack(int iTrack, double ke, int a, int z, double tta, double phi)
{
    if (iTrack==2) {
        fHistTTA[0] -> Fill(ThetaLabToCom(tta));
        fHistAA [0] -> Fill(ThetaLabToCom(tta),phi);
    }
    if (ThetaLabToCom(tta)<160) {
        fHistTTA[1] -> Fill(ThetaLabToCom(tta));
        fHistAA [1] -> Fill(ThetaLabToCom(tta),phi);
    }
}

void FillHit(int hitType, int type, int detn, int fstn, int bstn, double fre, double bke, double upe, double dwe, TVector3 pos)
{
    fHistDetn -> Fill(detn);
    fHistDetn2 -> Fill(detn,type);
    fHistDetn3 -> Fill(detn,fstn);
    fHistDetn4 -> Fill(detn,bstn);
}

void FillEvent(int nTracks, int nHits)
{
    fHistNHits -> Fill(nHits);
    fHistNTracks -> Fill(nTracks);
}

void FillGoodEvent(double energy, TVector3 pos)
{
    fHistEnergyTheta -> Fill(ThetaLabToCom(pos.Theta()*180/pi),energy);
    fHistTheta -> Fill(ThetaLabToCom(pos.Theta()*180/pi));
}

void End()
{
    fTop -> Draw("viewer");
}
