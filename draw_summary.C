void draw_summary(TString fileName="")
{
    gStyle -> SetOptStat(11111111);
    if (fileName.IsNull()) fileName = "data/stark_ko2421.ana.root";
    cout << fileName << endl;

    auto tree = new TChain("PhysicsTree");
    tree -> AddFile(fileName);
    tree -> Print();
    auto numEvents = tree -> GetEntries();

    auto cvs = new TCanvas("cvs","",1800,1200);
    cvs -> Divide(3,2);
    cvs -> cd(1); tree -> Draw("nhit>>hist_nhit");
    cvs -> cd(2); tree -> Draw("180/pi*hPos.Theta()>>hist_theta(180,0,90)","","");
    cvs -> cd(3); tree -> Draw("detN","","colz");
    cvs -> cd(4); tree -> Draw("sumE","","colz");
    cvs -> cd(5); tree -> Draw("180/pi*hPos.Phi():hPos.z()","","colz");
    cvs -> cd(6); tree -> Draw("180/pi*sPos.Phi():sPos.z()","","colz");

    LKDrawingGroup(cvs,"hstyle").Update();
}
