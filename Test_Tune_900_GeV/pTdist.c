// Macro che legge i Tree generati ed estrae le distribuzioni in impulso trasverso per protoni e neutroni
// #define PROTON 2212
// #define ANTIPROTON -2212
// #define NEUTRON 2112
// #define ANTINEUTRON -2112

void pTdist()
{

    // Enable LaTeX in titles
    gROOT->SetStyle("Plain");  // Choose your preferred ROOT style
    gStyle->SetOptTitle(1);    // Turn on title options
    gStyle->SetOptTitle(1111); // Enable option for using LaTeX in titles
    // ########################
    const int kMaxArraySize = 100; // Max array dimension
    TFile *file = new TFile("900GeV0.root", "read");
    file->ls();
    TTree *Tout = (TTree *)file->Get("h76");
    Int_t nentries = Tout->GetEntries();
    // Enabling for reading only the branches I need
    Tout->SetBranchStatus("*", false);
    Tout->SetBranchStatus("ntrack", true);
    Tout->SetBranchStatus("PID", true);
    Tout->SetBranchStatus("pT", true);
    Tout->SetBranchStatus("rap", true);
    // #############################################
    cout << " nentries = " << nentries << endl;
    // Definition of the variables to address the trees
    Int_t ntrack;
    Int_t PID[kMaxArraySize];
    Double_t pT[kMaxArraySize];
    Double_t y[kMaxArraySize];
    // ##############################################
    // Addressing the variables I need with variables in the code
    Tout->SetBranchAddress("ntrack", &ntrack);
    Tout->SetBranchAddress("PID", PID);
    Tout->SetBranchAddress("pT", pT);
    Tout->SetBranchAddress("rap", y);
    //Binning utilizzato dal paper di ALICE
    Double_t bins_pt[25] = {0.35, 0.4, 0.45, 0.5, 0.55, 0.6, 0.65, 0.7, 0.75, 0.8, 0.85, 0.9, 0.95, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 2.0, 2.2, 2.4};
    TH1F *hPt_ap = new TH1F("Pt_ap", "Simulated Antiproton Pythia-tune, |y|<0.5; p_{T} (GeV/c); 1/N_{evt}*d^{2}N/(dydp_{T})", 24, bins_pt);

    for (int i = 0; i < nentries; ++i) // LOOP SUGLI EVENTI
    {
        Tout->GetEntry(i);
        for (int j = 0; j < ntrack; ++j) // LOOP SUL SINGOLO EVENTO          //IN QUESTO LOOP RIEMPIO L'ISTOGRAMMA 2D
        {                                                                    //SELEZIONO SOLO I PROTONI CON |y| < 0.5, PERCHE' HO DEFINITO I LIMITI DI TH2F
            if (PID[j] == -2212 && (TMath::Abs(y[j]) < 0.5))
            {
                hPt_ap->Fill(pT[j]);
            }
        }
    }
    // Normalizzazione
    hPt_ap->Scale(1. / (double)nentries);
    hPt_ap->Scale(1., "width");
    //hPt_ap->Draw("e1 same");
    TFile * fin = TFile::Open("HEPData-ins885104-v1-Table_3.root", "read");
    //now we do the same for the antiprotons in a new canvas where data now is Table3/Graph1D_y2
    TGraphErrors *alice_data_ap = (TGraphErrors*)fin->Get("Table 3/Graph1D_y2");
    alice_data_ap->SetMarkerStyle(20);
    alice_data_ap->SetMarkerSize(0.5);
    alice_data_ap->SetMarkerColor(kRed);
    alice_data_ap->SetLineColor(kRed);
    //alice_data_ap->Draw("same p e1");
    //Build the ratio between simulated and experimental data taking the value of the bins under the
    double ratio_ap = 0;
    double error_ap = 0;
    double x_ap = 0;
    double yh_ap = 0;
    TGraphErrors *gr_ap = new TGraphErrors();
    for (int i = 1; i < 25; i++)
    {
        x_ap = hPt_ap->GetBinCenter(i);
        yh_ap = hPt_ap->GetBinContent(i);
        ratio_ap = yh_ap / alice_data_ap->Eval(x_ap);
        error_ap = hPt_ap->GetBinError(i) / alice_data_ap->Eval(x_ap);
        gr_ap->SetPoint(i - 1, x_ap, ratio_ap);
        gr_ap->SetPointError(i - 1, 0, error_ap);
    }
    //As before we plot the histogram and the data in the same plot, the ratio is plotted in the same canvas below
    TCanvas *c1 = new TCanvas("c2", "c2", 800, 800);
    //set the legend for the plot
    c1->Divide(1, 2);
    c1->cd(1);
    //set the log scale on the y axis
    gPad->SetLogy();
    hPt_ap->Draw("e1");
    alice_data_ap->Draw("same p e1");
    //set the statistics off
    hPt_ap->SetStats(0);

    TLegend *leg_ap = new TLegend(0.6, 0.6, 0.9, 0.9);
    leg_ap->AddEntry(hPt_ap, "Pythia-tune simulated antiprotons", "l");
    leg_ap->AddEntry(alice_data_ap, "ALICE data", "l");
    //make the legend nice
    leg_ap->SetBorderSize(0);
    leg_ap->SetFillColor(0);
    //enlarge the font size
    leg_ap->SetTextSize(0.04);
    leg_ap->Draw();
    //enlarge the font size of the axis
    hPt_ap->GetXaxis()->SetTitleSize(0.04);
    hPt_ap->GetYaxis()->SetTitleSize(0.04);
    // set the title of the plot
    hPt_ap->SetTitle("pp collisions at #sqrt{s}=900 GeV , |y|<0.5");
    c1->cd(2);
    gr_ap->Draw("ap");
    gr_ap->SetMarkerStyle(20);
    gr_ap->SetMarkerSize(0.5);
    gr_ap->SetMarkerColor(kBlack);
    gr_ap->SetLineColor(kBlack);
    //gr->SetTitle("Ratio between simulated and experimental data");
    gr_ap->GetXaxis()->SetTitle("p_{T} (GeV/c)");
    gr_ap->GetYaxis()->SetTitle("Ratio MC/ALICE");
    //gr->SetTitle("Ratio between simulated and experimental data");
    //set the range of the y axis
    gr_ap->GetYaxis()->SetRangeUser(0.8, 2.2);
    //set the range of the x axis
    gr_ap->GetXaxis()->SetRangeUser(0.35, 2.4);
    //draw with no margin with the upper plot
    gr_ap->SetMinimum(0.1);
    gr_ap->SetMaximum(1.5);
    gr_ap->Draw("ap");

    //Save everything in a root file, both the histograms and the graphs
    TFile *fout = new TFile("pTdist.root", "recreate");
    hPt_ap->Write();
    alice_data_ap->Write();
    gr_ap->Write();
    fout->Close();
    fin->Close();
    
}

    