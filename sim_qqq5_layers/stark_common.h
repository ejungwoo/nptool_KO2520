void draw_qqq5_rstrips()
{
    const double QQQ5_SiActiveInR  = 25.25;
    const double QQQ5_SiActiveOutR = 81.95;
    const int    QQQ5_NRStrip = 32 ;

    for (auto stripNo=0; stripNo<QQQ5_NRStrip+1; stripNo++)
    {
        double r = QQQ5_SiActiveInR + stripNo * (QQQ5_SiActiveOutR - QQQ5_SiActiveInR) / QQQ5_NRStrip;
        auto graph = new TGraph();
        for (auto i=0; i<=100; ++i)
        {
            double phi = TMath::Pi()*2/100*i;
            TVector3 pos(r,0,0);
            pos.SetPhi(phi);
            graph -> SetPoint(graph->GetN(),pos.X(),pos.Y());
        }
        graph -> Draw("samel");
    }
}

#ifndef jw_cout
#include <string>
#define jw_cout std::cout<<"\033[0;32m"<<Form("+%d %s # \033[0m",__LINE__,std::string(__FILE__).c_str())
#endif

