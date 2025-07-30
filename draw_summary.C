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
    //tree -> Print("toponly");

    auto cvs = new TCanvas("cvs","",2500,1300);
    cvs -> Divide(5,3);

    auto draw_ft = [cvs,tree](int i, TString expression, TCut cut="", TString option="")
    {
        TString htag = expression;
        if (htag.Index(":")>0||htag.Index("(")>0||htag.Index("[")>0) htag = Form("%d",i);
        TString expressFull = expression.Data();
        if (expressFull.Index(">>")<0) expressFull = Form("%s>>hist_%s",expression.Data(),htag.Data());
        cvs -> cd(i);
        auto entries = tree -> Draw(expressFull,cut,option);
        cout << expressFull << " " << cut.GetTitle() << " " << option << " # " << entries << endl;
    };

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
    draw_ft(icvs++,"hPos.Theta()*180/pi", cut);
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

#ifdef LILAK_VERSION
    LKDrawingGroup(cvs,"hstyle").Update();
#endif
    cvs -> SaveAs(Form("figures/summary_%s.png",anaName.Data()));
}
