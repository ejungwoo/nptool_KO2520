#include "stark_common.h"

TObjArray *cvsList = nullptr;
void SaveFigures();

void draw_eloss_qqq5(TString fileName="", TString fileName2="")
{
    bool draw_layers = false;
    bool draw_kinematics = true;
    vector<int> layers = {0,1};
    //vector<int> kinematics = {0,1,2};
    vector<int> kinematics = {0};

    cvsList = new TObjArray();

    gStyle -> SetOptStat(11111111);
    if (fileName.IsNull()) fileName = "data/stark_qqq5.proton_e5_30_a5_30.pana.root";

    if (1) {
        fileName  = "data/stark_qqq5.proton_e5_30_a5_30.pana.root";
        fileName2 = "data/stark_qqq5.2H_e5_30_a5_30.pana.root";
    }
    cout << fileName << endl;
    cout << fileName2 << endl;

    auto tree = new TChain("ana");
    tree -> AddFile(fileName);
    if (!fileName2.IsNull()) tree -> AddFile(fileName2);
    //auto file = new TFile(fileName);
    //auto tree = (TTree*) file -> Get("ana");
    auto numEvents = tree -> GetEntries();

    //TCut cut_common = "mult==2";
    //TCut cut_common = "mult>0";
    TCut cut_common = "";

    cut_common = cut_common && "e1>0&&e2>0";
    cut_common = cut_common && "strip2>1";
    //cut_common = cut_common && "strip1>0&&strip2>0";

    //cut_common = cut_common && "strip2==1";
    //cut_common = cut_common && "strip1==32";
    //cut_common = cut_common && "prim_ke>15&&e2<3&&e2>1";
    //cut_common = cut_common && "strip1==1";
    //cut_common = cut_common && "multi_strip1==false";

    if (draw_layers)
    {
        for (int i : layers)
        {
            TCut cut_layer = TCut("first layer", "e1>0.01");
            if (i==1) cut_layer = TCut("second layer", "e2>0.01");
            cut_layer = cut_layer && cut_common;

            auto cvs_layer = new TCanvas(Form("cvs_layer_%d",i),"",1200,1200);
            cvs_layer -> Divide(2,2);
            cvsList -> Add(cvs_layer);

            auto hist1 = new TH1D(Form("hl%d_phi"  ,i),";#phi (deg)",180,-180,180);
            auto hist2 = new TH1D(Form("hl%d_tta"  ,i),";#theta (deg)",180,0,30);
            auto hist3 = new TH1D(Form("hl%d_strip",i),";strip no.",35,0,35);
            auto hist4 = new TH2D(Form("hl%d_xy"   ,i),";x (mm);y (mm)",200,-100,100,200,-100,100);

            cvs_layer->cd(1)->SetMargin(0.11,0.12,0.1,0.08);; tree -> Draw(Form("180/pi*pos%d.Theta()>>%s",i+1,hist1->GetName()),cut_layer,"");
            cvs_layer->cd(2)->SetMargin(0.11,0.12,0.1,0.08);; tree -> Draw(Form("180/pi*pos%d.Phi()>>%s",i+1,hist2->GetName()),cut_layer,"");
            cvs_layer->cd(3)->SetMargin(0.11,0.12,0.1,0.08);; tree -> Draw(Form("strip%d>>%s",i+1,hist3->GetName()),cut_layer,"");
            cvs_layer->cd(4)->SetMargin(0.11,0.12,0.1,0.08);; tree -> Draw(Form("pos%d.fX:pos%d.fY>>%s",i+1,i+1,hist4->GetName()),cut_layer,"colz");
            draw_qqq5_rstrips(); // stark_common.h

            //LKDrawingGroup(cvs_layer,"hstyle").Update();
        }
    }

    if (draw_kinematics)
    {
        for (auto i : kinematics)
        {
            TCut cut_pid;
            if (i==1) cut_pid = "prim_a==1";
            if (i==2) cut_pid = "prim_a==2";
            TCut cut = cut_common && cut_pid;

            auto cvse = new TCanvas(Form("cvs_kinematics_%d",i),"",2000,1200);
            cvse -> Divide(3,2);
            cvsList -> Add(cvse);

            auto hist1 = new TH2D(Form("hk%d_e1" ,i),";primary KE (Mev);e1 (MeV)",200,0,32,200,0,20);
            auto hist2 = new TH2D(Form("hk%d_e2" ,i),";primary KE (Mev);e2 (MeV)",200,0,32,200,0,20);
            auto hist3 = new TH2D(Form("hk%d_ea" ,i),";primary KE (Mev);e1+e2 (MeV)",200,0,32,200,0,30);
            auto hist4 = new TH2D(Form("hk%d_e12",i),";e1 (MeV);e2 (MeV);",200,0,20,200,0,20);
            auto hist5 = new TH3D(Form("hk%d_3d" ,i),";e1 (MeV);e2 (MeV);primary KE (MeV)",80,0,20,80,0,20,80,0,32);
            auto hist6 = new TH2D(Form("hk%d_angle",i),";#theta (deg);#phi (deg)",180,0,45,180,-180,180);

            cvse->cd(1)->SetMargin(0.11,0.12,0.1,0.08);; tree -> Draw(Form("e1:prim_ke   >>%s",hist1->GetName()),cut,"colz");
            cvse->cd(2)->SetMargin(0.11,0.12,0.1,0.08);; tree -> Draw(Form("e2:prim_ke   >>%s",hist2->GetName()),cut,"colz");
            cvse->cd(3)->SetMargin(0.11,0.12,0.1,0.08);; tree -> Draw(Form("e1+e2:prim_ke>>%s",hist3->GetName()),cut,"colz");
            cvse->cd(4)->SetMargin(0.11,0.12,0.1,0.08);; tree -> Draw(Form("e2:e1        >>%s",hist4->GetName()),cut,"colz");
            cvse->cd(5)->SetMargin(0.11,0.12,0.1,0.08);; tree -> Draw(Form("prim_ke:e2:e1>>%s",hist5->GetName()),cut,"box2");
            cvse->cd(6)->SetMargin(0.11,0.12,0.1,0.08);; tree -> Draw(Form("180/pi*pos1.Phi():180/pi*pos1.Theta()>>%s",hist6->GetName()),cut,"colz");

            //LKDrawingGroup(cvse,"hstyle").Update();
        }
    }

    //SaveFigures();
}


void SaveFigures()
{
    TIter next(cvsList);
    TCanvas *cvs = nullptr;
    while ((cvs = (TCanvas*) next()))
        cvs -> SaveAs(Form("figures/%s.png",cvs->GetName()));
}
