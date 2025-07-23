void draw_summary(TString fileName="")
{
    gStyle -> SetOptStat(11111111);
    if (fileName.IsNull()) fileName = "data/stark_ko2421_efficiency.proton_e10_10_a0_90.ana.root";
    cout << fileName << endl;

    TString anaName = fileName;
    anaName = anaName(anaName.Index("/")+1,anaName.Sizeof());
    anaName = anaName(0,anaName.Index(".ana.root"));
    anaName.ReplaceAll(".","_");

    auto tree = new TChain("PhysicsTree");
    tree -> AddFile(fileName);
    auto numEvents = tree -> GetEntries();

    auto cvs = new TCanvas("cvs","",1800,1200);
    cvs -> Divide(2,2);
    cvs -> cd(1); tree -> Draw("nhit>>hist_nhit","","text hist");
    cvs -> cd(2); tree -> Draw("180/pi*hPos.Theta()>>hist_theta(180,0,120)","","");
    cvs -> cd(3); tree -> Draw("detN>>hist_detN(100,0,100)","","colz");
    cvs -> cd(4); tree -> Draw("sumE>>hist_sumE(100,0,20)","","colz");

    auto hist1 = (TH1D*) (gDirectory->FindObject("hist_nhit"));
    auto hist2 = (TH1D*) (gDirectory->FindObject("hist_detN"));
    double total = hist1 -> GetEntries();
    double collected = hist2 -> GetEntries();
    double efficiency = collected/total;
    hist1 -> SetTitle(Form("<%s> %s",fileName.Data(),hist1->GetTitle()));
    hist2 -> SetTitle(Form("%s  [efficiency = %.3f]",hist2->GetTitle(),efficiency));

#ifndef LILAK_VERSION
    LKDrawingGroup(cvs,"hstyle").Update();
#endif
    cvs -> SaveAs(Form("figures/summary_%s.png",anaName.Data()));
}
