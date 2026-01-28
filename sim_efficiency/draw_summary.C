void draw_summary(TString fileName="")
{
    //gStyle -> SetOptStat(11111111);
    gStyle -> SetOptStat("e");

    TString fileName2;
    //if (fileName.IsNull()) fileName = "data/stark_ko2421_efficiency.proton_e10_10_a0_90.ana.root";
    //if (fileName.IsNull()) fileName = "data/stark_atomx_0.proton_e15_15_a0_180.ana.root";
    if (fileName.IsNull()) {
        fileName = "data/stark_efficiency_ko2421.proton_e15_15_a0_90_z0.ana.root";
        fileName2 = "data/stark_efficiency_ko2421.proton_e15_15_a0_90_z0.sim.root";
    }
    else {
        fileName2 = fileName;
        fileName2.ReplaceAll(".ana.root",".sim.root");
    }
    cout << fileName << endl;
    cout << fileName2 << endl;

    TString anaName = fileName;
    anaName = anaName(anaName.Index("/")+1,anaName.Sizeof());
    anaName = anaName(0,anaName.Index(".ana.root"));
    anaName.ReplaceAll(".","_");

    auto tree = new TChain("PhysicsTree");
    tree -> AddFile(fileName);
    //tree -> Print("toponly");

    TCanvas *cvs = nullptr;
    //cvs = new TCanvas("cvs","",2500,1300);
    //cvs -> Divide(5,3);

    auto draw_ft = [cvs,tree](int i, TString expression, TCut cut="", TString option="")
    {
        TString htag = expression;
        if (htag.Index(":")>0||htag.Index("(")>0||htag.Index("[")>0) htag = Form("%d",i);
        TString expressFull = expression.Data();
        if (expressFull.Index(">>")<0) expressFull = Form("%s>>hist_%s",expression.Data(),htag.Data());
        Long64_t entries;
        if (cvs!=nullptr) {
            cvs -> cd(i);
            entries = tree -> Draw(expressFull,cut,option);
        }
        else {
            option = option + " goff";
            entries = tree -> Draw(expressFull,cut,option);
        }
        cout << expressFull << " " << cut.GetTitle() << " " << option << " # " << entries << endl;
    };

    if (fileName2.IsNull()==false) {
        auto tree2 = new TChain("SimulatedTree");
        tree2 -> AddFile(fileName2);
        tree2 -> Draw("TrackInfo.fTI_Theta>>hist_sim_theta(150,0,90)","","goff");
    }

    TCut cut = "";
    //TCut cut = "(groupN<200)*(groupdE[0][1]>0&&groupdE[0][2]>0)||(groupN<300&&groupN>=200)*1";

    int icvs = 1;
    draw_ft(icvs++,"nhit"  , cut);
    draw_ft(icvs++,"detN"  , cut);
    draw_ft(icvs++,"fStrN" , cut);
    draw_ft(icvs++,"bStrN" , cut);
    draw_ft(icvs++,"uppE"  , cut);
    draw_ft(icvs++,"dwnE"  , cut);
    draw_ft(icvs++,"nGroup:groupN", cut,"colz");
    draw_ft(icvs++,"groupE", cut);
    draw_ft(icvs++,"hPos.Theta()*180/pi>>hist_ana_theta(150,0,90)", cut);
    draw_ft(icvs++,"sumE:hPos.Theta()*180/pi",cut,"colz");
    draw_ft(icvs++,"groupE:hPos.Theta()*180/pi>>hist_groupGT(200,0,90,200,0,25)",cut&&"groupE>0","colz");
    draw_ft(icvs++,"hPos.Phi():hPos.Theta()*180/pi",cut,"colz");
    draw_ft(icvs++,"groupdE[0][1]",cut&&"groupdE[0][1]>0");
    draw_ft(icvs++,"groupdE[0][2]",cut&&"groupdE[0][2]>0");
    draw_ft(icvs++,"groupdE[0][2]:groupdE[0][1]",cut&&"groupdE[0][1]>0&&groupdE[0][2]>0","colz");

    auto hist1 = (TH1D*) (gDirectory->FindObject("hist_nhit"));
    auto hist2 = (TH1D*) (gDirectory->FindObject("hist_detN"));
    double total = hist1 -> GetEntries();
    double collected = hist2 -> GetEntries();
    double efficiency = collected/total;
    hist1 -> SetTitle(Form("<%s> %s",fileName.Data(),hist1->GetTitle()));
    hist2 -> SetTitle(Form("%s  [efficiency = %.3f]",hist2->GetTitle(),efficiency));

    auto cvs2 = new TCanvas("cvs2","",1000,800);
    cvs2 -> Divide(2,2);
    auto hist3 = (TH1D*) (gDirectory->FindObject("hist_ana_theta"));
    auto hist4 = (TH1D*) (gDirectory->FindObject("hist_sim_theta"));
    cvs2 -> cd(1); hist3 -> Draw();
    cvs2 -> cd(2); hist4 -> Draw();
    cvs2 -> cd(3);
    auto hist5 = (TH1D*) hist3 -> Clone("h_efficiency");
    hist5 -> Divide(hist4);
    hist5 -> Draw();

#ifdef LILAK_VERSION
    if (cvs!=nullptr) LKDrawingGroup(cvs,"hstyle").Update();
    if (cvs2!=nullptr) LKDrawingGroup(cvs2,"hstyle").Update();
#endif
    if (cvs!=nullptr) cvs -> SaveAs(Form("figures/summary_%s.png",anaName.Data()));
    if (cvs2!=nullptr) cvs2 -> SaveAs(Form("figures/summary2_%s.png",anaName.Data()));
}
