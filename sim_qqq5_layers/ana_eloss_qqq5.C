#include "stark_common.h"

TObjArray *cvsList = nullptr;
void SaveFigures();

void ana_eloss_qqq5(TString fileName="", TString fileName2="")
{
    bool draw_layers = false;
    bool draw_kinematics = false;
    bool draw_projection = true;
    vector<int> layers = {0,1};
    //vector<int> kinematics = {0,1,2};
    vector<int> kinematics = {1};

    cvsList = new TObjArray();

    gStyle -> SetOptStat(11111111);
    if (fileName.IsNull()) fileName = "data/stark_qqq5.proton_e5_30_a5_30_z0.pana.root";

    if (1) {
        fileName  = "data/stark_qqq5.proton_e5_30_a5_30_z0.pana.root";
        fileName2 = "data/stark_qqq5.2H_e5_30_a5_30_z0.pana.root";
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

    //cut_common = cut_common && "e1>0&&e2>0";
    cut_common = cut_common && "strip2>1";
    //cut_common = cut_common && "strip1>0&&strip2>0";

    //cut_common = cut_common && "strip2==1";
    //cut_common = cut_common && "strip1==32";
    //cut_common = cut_common && "prim_ke>15&&e2<3&&e2>1";
    //cut_common = cut_common && "strip1==1";
    //cut_common = cut_common && "multi_strip1==false";

    if (draw_layers)
    {
        for (int iKinematic : layers)
        {
            TCut cut = TCut("first layer", "e1>0");
            if (iKinematic==1) cut = TCut("second layer", "e2>0");
            cut = cut && cut_common;

            auto cvs_layer = new TCanvas(Form("cvs_layer_%d",iKinematic),"",1200,1200);
            cvs_layer -> Divide(2,2);
            cvsList -> Add(cvs_layer);

            auto hist1 = new TH1D(Form("hl%d_phi"  ,iKinematic),TString(cut.GetTitle())+";#phi (deg)",180,-180,180);
            auto hist2 = new TH1D(Form("hl%d_tta"  ,iKinematic),TString(cut.GetTitle())+";#theta (deg)",180,0,30);
            auto hist3 = new TH1D(Form("hl%d_strip",iKinematic),TString(cut.GetTitle())+";strip no.",40,0,40);
            auto hist4 = new TH2D(Form("hl%d_xy"   ,iKinematic),TString(cut.GetTitle())+";x (mm);y (mm)",200,-100,100,200,-100,100);

            cvs_layer->cd(1)->SetMargin(0.11,0.12,0.1,0.08);; tree -> Draw(Form("180/pi*pos%d.Phi()>>%s",iKinematic+1,hist1->GetName()),cut,"");
            cvs_layer->cd(2)->SetMargin(0.11,0.12,0.1,0.08);; tree -> Draw(Form("180/pi*pos%d.Theta()>>%s",iKinematic+1,hist2->GetName()),cut,"");
            cvs_layer->cd(3)->SetMargin(0.11,0.12,0.1,0.08);; tree -> Draw(Form("strip%d>>%s",iKinematic+1,hist3->GetName()),cut,"");
            cvs_layer->cd(4)->SetMargin(0.11,0.12,0.1,0.08);; tree -> Draw(Form("pos%d.fX:pos%d.fY>>%s",iKinematic+1,iKinematic+1,hist4->GetName()),cut,"colz");
            draw_qqq5_rstrips(); // stark_common.h

#ifndef LILAK_VERSION
            LKDrawingGroup group(cvs_layer,"hstyle");
            group.GetDrawing(0) -> SetStatCorner(2);
            group.GetDrawing(1) -> SetStatCorner(1);
            group.GetDrawing(2) -> SetStatCorner(0);
            group.GetDrawing(3) -> SetStatCorner(0);
            group.Update();
#endif
        }
    }

    auto graph_xyz = new TGraph2D();
    graph_xyz -> SetLineColor(kRed);
    graph_xyz -> SetMarkerStyle(24);
    if (draw_projection)
    {
        for (auto iKinematic : {1})
        {
            TCut cut = cut_common && "e1>0&&e2>0";
            cut = cut && Form("prim_a==%d",iKinematic);

            //int numProj = 40;
            int numProj = 80;
            double primE_range1 = 12;
            double primE_range2 = 30;
            double dPrimE = (primE_range2-primE_range1)/numProj;
            auto cvs_reco = new TCanvas(Form("cvs_reco_%d",iKinematic),"",3000,1800);
            if (numProj==20)  cvs_reco -> Divide(5,4,0.001,0.001);
            else if (numProj==40) cvs_reco -> Divide(7,6,0.001,0.001);
            else if (numProj==80) cvs_reco -> Divide(10,8,0.001,0.001);
            cvsList -> Add(cvs_reco);
            for (auto iProj=0; iProj<numProj; ++iProj)
            {
                //cvs_reco -> cd(iProj+1) -> SetMargin(0.11,0.12,0.1,0.08);
                double z1 = primE_range1+ iProj   *dPrimE;
                double z2 = primE_range1+(iProj+1)*dPrimE;
                cvs_reco -> cd(iProj+1) -> SetMargin(0.02,0.02,0.02,0.02);
                TCut cut2 = cut && Form("prim_ke>=%f&&prim_ke<%f",z1,z2);
                //tree -> Draw(Form("e2:e1>>hist_primE_iz_%d(80,0,20,80,0,20)",iProj),cut2,"col");
                auto hist = new TH2D(Form("hist_primE_iz_%d",iProj),"",80,0,20,80,0,20);
                hist -> SetStats(0);
                tree -> Draw(Form("e2:e1>>%s",hist->GetName()),cut2,"col");
                double x = hist -> GetMean(1);
                double y = hist -> GetMean(2);
                hist -> GetXaxis() -> SetRangeUser(x-2.5,x+2.5);
                hist -> GetYaxis() -> SetRangeUser(y-2.5,y+2.5);
                auto graph = new TGraph();
                graph -> SetPoint(0,x,y);
                graph -> SetMarkerStyle(24);
                graph -> SetMarkerSize(4);
                graph -> SetMarkerColor(kRed);
                graph -> Draw("samep");
                graph_xyz -> SetPoint(graph_xyz->GetN(),x,y,0.5*(z1+z2));
            }

            cvs_reco -> SaveAs("figures/pp.png");

            TString fileERecoName = "energy_restoration.root";
            cout << fileERecoName << endl;
            auto file_ereco = new TFile(fileERecoName,"recreate");
            graph_xyz -> SetName("energy_restoration");
            graph_xyz -> Write();
        }
    }

    if (draw_kinematics)
    {
        for (auto iKinematic : kinematics)
        {
            TCut cut = cut_common && "e1>0&&e2>0";
            cut = cut && Form("prim_a==%d",iKinematic);

            auto cvse = new TCanvas(Form("cvs_kinematics_%d",iKinematic),"",2000,1800);
            cvse -> Divide(3,3);
            cvsList -> Add(cvse);

            auto numPrimE = 80;
            auto numSingleE = 80;
            double primE2 = 32.;
            auto hist1 = new TH2D(Form("hk%d_e1" ,iKinematic),TString(cut.GetTitle())+";primary KE (Mev);e1 (MeV)",200,0,32,200,0,20);
            auto hist2 = new TH2D(Form("hk%d_e2" ,iKinematic),TString(cut.GetTitle())+";primary KE (Mev);e2 (MeV)",200,0,32,200,0,20);
            auto hist3 = new TH2D(Form("hk%d_ea" ,iKinematic),TString(cut.GetTitle())+";primary KE (Mev);e1+e2 (MeV)",200,0,32,200,0,30);
            auto hist4 = new TH2D(Form("hk%d_e12",iKinematic),TString(cut.GetTitle())+";e1 (MeV);e2 (MeV);",200,0,20,200,0,20);
            auto hist5 = new TH3D(Form("hk%d_3d" ,iKinematic),TString(cut.GetTitle())+";e1 (MeV);e2 (MeV);primary KE (MeV)",numSingleE,0,20,numSingleE,0,20,numPrimE,0,primE2);
            auto hist6 = new TH2D(Form("hk%d_tp" ,iKinematic),TString(cut.GetTitle())+";#theta (deg);#phi (deg)",180,0,45,180,-180,180);
            auto hist7 = new TH2D(Form("hk%d_3d1",iKinematic),TString(cut.GetTitle())+";e1 (MeV);primary KE (MeV)",numSingleE,0,20,numSingleE,0,32);
            auto hist8 = new TH2D(Form("hk%d_3d2",iKinematic),TString(cut.GetTitle())+";e2 (MeV);primary KE (MeV)",numSingleE,0,20,numSingleE,0,32);
            hist5 -> SetStats(0);

            cvse->cd(1)->SetMargin(0.11,0.12,0.1,0.08); tree -> Draw(Form("e1:prim_ke   >>%s",hist1->GetName()),cut,"colz");
            cvse->cd(2)->SetMargin(0.11,0.12,0.1,0.08); tree -> Draw(Form("e2:prim_ke   >>%s",hist2->GetName()),cut,"colz");
            cvse->cd(3)->SetMargin(0.11,0.12,0.1,0.08); tree -> Draw(Form("e1+e2:prim_ke>>%s",hist3->GetName()),cut,"colz");
            cvse->cd(4)->SetMargin(0.11,0.12,0.1,0.08); tree -> Draw(Form("e2:e1        >>%s",hist4->GetName()),cut,"colz");
            cvse->cd(5)->SetMargin(0.11,0.12,0.1,0.08); tree -> Draw(Form("180/pi*pos1.Phi():180/pi*pos1.Theta()>>%s",hist6->GetName()),cut,"colz");
            cvse->cd(6)->SetMargin(0.11,0.12,0.1,0.08); tree -> Draw(Form("prim_ke:e2:e1>>%s",hist5->GetName()),cut,"box2");
            graph_xyz -> Draw("sameline");
            cvse->cd(7)->SetMargin(0.11,0.12,0.1,0.08); tree -> Draw(Form("prim_ke:e1>>%s",hist7->GetName()),cut,"colz");
            cvse->cd(8)->SetMargin(0.11,0.12,0.1,0.08); tree -> Draw(Form("prim_ke:e2>>%s",hist8->GetName()),cut,"colz");

#ifndef LILAK_VERSION
            LKDrawingGroup group(cvse,"hstyle");
            group.GetDrawing(0) -> SetStatCorner(1);
            group.GetDrawing(1) -> SetStatCorner(1);
            group.GetDrawing(2) -> SetStatCorner(1);
            group.GetDrawing(3) -> SetStatCorner(0);
            group.GetDrawing(4) -> SetOptStat(-1);
            group.GetDrawing(5) -> SetStatCorner(0);
            group.Update();
#endif
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
