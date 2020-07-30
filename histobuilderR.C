#include <cstdlib>
#include <iostream>
#include <chrono>
#include <TFile.h>
#include <TTree.h>
#include <TApplication.h>
#include <TROOT.h>
#include <TDatabasePDG.h>
#include <TLorentzVector.h>
#include <TVector3.h>
#include <TH1.h>
#include <TH2.h>
#include <TChain.h>
#include <TCanvas.h>
#include <TBenchmark.h>
#include <bitset>
#include "/w/hallb-scifs17exp/clas12/markov/dataQuality/clasqaDB/srcC/include/QADB.h"

#include "clas12reader.h"

//Below is MINE                                                                                                                                                                                             
#include <TStyle.h>
#include <TLegend.h>
#include <TF1.h>
#include <TGraph.h>

using namespace clas12;

double PI= 3.14159265358979323846;

void histobuilderR(){

  //Preparing Code                                                                                                                                                                         
  int OutFileNum=18;
  
  TString inputFile;
 
  for(Int_t i=1;i<gApplication->Argc();i++){
    TString opt=gApplication->Argv(i);
    if((opt.Contains(".root"))){
      inputFile=opt(5,opt.Sizeof());
      std::cout<<opt<<std::endl<<inputFile<<std::endl<<std::endl;
    }
  }
  
  
  if(inputFile==TString())  {
    std::cout << " *** please provide a file name..." << std::endl;
    exit(0);
  }

  auto start = std::chrono::high_resolution_clock::now();

  gBenchmark->Start("timer");
 
  if(Form("/u/home/richcap/Code_Example/SimpleAnalysisJLabRC/test%d.root",OutFileNum) != inputFile){
    std::cout<<endl<<"You may wish to open this code."<<endl<<"OutFileNum may not match root file."<<endl<<"May result in the incorrect sorting of the PDFs."<<endl;
  }



  //////// Making new out file ////////// 
  TString outputFile = Form("histotest%d.root",OutFileNum);
  TFile *outH;
  outH = new TFile(outputFile, "RECREATE");




  ////////////////////////////////////////


  std::cout<<"Opening Program"<<std::endl;

  TFile *InF=new TFile(inputFile);
  TTree *SF_sec=(TTree*)InF->Get("out_tree_SF_sec"); 
  TTree *w_and_q2=(TTree*)InF->Get("out_tree_w_and_q2");
  TTree *electron_momentum=(TTree*)InF->Get("out_tree_electron_momentum");

  
  //////// Making histograms //////////// 


  TH2F *SFH = new TH2F("SFH", "Sampling Fraction versus Momentum Histogram (Made from vectors)", 600, 0, 12,  50, 0, 0.5);

  TH2F *SFHs1 = new TH2F("SFHs1", "Sampling Fraction versus Momentum Histogram (Sec 1)", 600, 0, 12,  50, 0, 0.5);
  TH2F *SFHs2 = new TH2F("SFHs2", "Sampling Fraction versus Momentum Histogram (Sec 2)", 600, 0, 12,  50, 0, 0.5);
  TH2F *SFHs3 = new TH2F("SFHs3", "Sampling Fraction versus Momentum Histogram (Sec 3)", 600, 0, 12,  50, 0, 0.5);
  TH2F *SFHs4 = new TH2F("SFHs4", "Sampling Fraction versus Momentum Histogram (Sec 4)", 600, 0, 12,  50, 0, 0.5);
  TH2F *SFHs5 = new TH2F("SFHs5", "Sampling Fraction versus Momentum Histogram (Sec 5)", 600, 0, 12,  50, 0, 0.5);
  TH2F *SFHs6 = new TH2F("SFHs6", "Sampling Fraction versus Momentum Histogram (Sec 6)", 600, 0, 12,  50, 0, 0.5);

  TH1F *wHisto_s1 = new TH1F("wHisto_s1","wHisto_s1", 1000, 0, 5);
  TH1F *wHisto_s2 = new TH1F("wHisto_s2","wHisto_s2", 1000, 0, 5);
  TH1F *wHisto_s3 = new TH1F("wHisto_s3","wHisto_s3", 1000, 0, 5);
  TH1F *wHisto_s4 = new TH1F("wHisto_s4","wHisto_s4", 1000, 0, 5);
  TH1F *wHisto_s5 = new TH1F("wHisto_s5","wHisto_s5", 1000, 0, 5);
  TH1F *wHisto_s6 = new TH1F("wHisto_s6","wHisto_s6", 1000, 0, 5);
  TH1F *wHisto_all = new TH1F("wHisto_all","wHisto_all", 1000, 0, 5);

  TH1F *elPM_s[6];
  TH1F *elPCal_s[6];
  TH1F *elPCor_s[6];
  TH2F *elPCor2_s[6];
  for(int i=0;i<6;i++){
    elPCor_s[i] = new TH1F(Form("elPCor_s%d",i+1),Form("Change in Electron Momentum (Sec %d)",i+1), 180, -7, 2);
    elPCal_s[i] = new TH1F(Form("elPCal_s%d",i+1),Form("Calculated Electron Momentum (Sec %d)",i+1), 240, 0, 12);
    elPM_s[i] = new TH1F(Form("elPM_s%d",i+1),Form("Measured Electron Momentum (Sec %d)",i+1), 240, 0, 12);
    elPCor2_s[i] = new TH2F(Form("elPCor2_s%d",i+1),Form("Change in Electron Momentum vs Electron Momentum (Sec %d)",i+1), 24, 0, 12, 130, -11, 2); 
  }



  ///// Making fits for histograms ///////


  // TF1 *fitT = new TF1("fitT","[Amp]*exp(-0.5*((x-[Mean])/[Sigma])*((x-[Mean])/[Sigma]))+[p0]+[p1]*x+[p2]*x*x",0,1.2); 
  // fitT->SetParameter(5,0.9774);
  // fitT->SetParLimits(5,0.9774*0.95,0.9774*1.05);
  // fitT->SetParameter(6,0.08757);
  // fitT->SetParLimits(6,0.08757*0.9,0.08757*1.1);  
  // fitT->SetParLimits(4,0,10000);
  
  TF1 *fitT = new TF1("fitT","[p0]*exp(-0.5*((x-[p1])/[p2])*((x-[p1])/[p2]))+[p3]+[p4]*x+[p5]*x*x",0,1.2);
  fitT->SetParameter(0,1509);
  fitT->SetParLimits(0,1509*0.5,1509*1.5);
  fitT->SetParameter(1,0.9774);
  fitT->SetParLimits(1,0.9774*0.95,0.9774*1.05);
  fitT->SetParameter(2,0.08757);
  fitT->SetParLimits(2,0.08757*0.9,0.08757*1.1);


  ///// Making/Filling Canvases //////////


  std::cout<<"Making Canvases"<<std::endl;

  double ePcMax[6];
  double ePmMax[6];
  double ePdMax[6];

  TCanvas *elecPcal = new TCanvas("elecPcal","Calculated Electron Momentum",200,10,700,780);
  elecPcal->Divide(3,2);
  for(int i=0;i<6;i++){
    elecPcal->cd(i+1);
    electron_momentum->Draw(Form("el_p_Calculated_Sec_%d>> elPCal_s%d",i+1,i+1));
    ePcMax[i] = elPCal_s[i]->GetXaxis()->GetBinCenter(elPCal_s[i]->GetMaximumBin());
    //ePcMax[i] = elPCal_s[i]->ShowPeaks(0.25,"",1);
    //cout<<Form("Peak of elPCal_s%d = ",i+1)<<ePcMax[i]<<endl;
  }

  std::cout<<"elecPcal Done"<<std::endl;

  TCanvas *elecPM = new TCanvas("elecPM","Measured Electron Momentum",200,10,700,780);
  elecPM->Divide(3,2);
  for(int i=0;i<6;i++){
    elecPM->cd(i+1);
    electron_momentum->Draw(Form("el_p_Measured_Sec_%d>> elPM_s%d",i+1,i+1));
    ePmMax[i] = elPM_s[i]->GetXaxis()->GetBinCenter(elPM_s[i]->GetMaximumBin());
  }

  std::cout<<"elecPM Done"<<std::endl;

  TCanvas *elecPCor = new TCanvas("elecPCor","Change in Electron Momentum",200,10,700,780);
  elecPCor->Divide(3,2);
  for(int i=0;i<6;i++){
    elecPCor->cd(i+1);
    electron_momentum->Draw(Form("el_p_Difference_Sec_%d>> elPCor_s%d",i+1,i+1));
    ePdMax[i] = elPCor_s[i]->GetXaxis()->GetBinCenter(elPCor_s[i]->GetMaximumBin());
  }

  std::cout<<"elecPCor Done"<<std::endl;

  //TGraph *ePpCor2[6];

  TCanvas *elecPCor2 = new TCanvas("elecPCor2","Change in Electron Momentum versus Electron Momentum (Measured)",200,10,700,780);
  elecPCor2->Divide(3,2);
  for(int i=0;i<6;i++){
    elecPCor2->cd(i+1);
    electron_momentum->Draw(Form("el_p_Difference_Sec_%d:el_p_Measured_Sec_%d>> elPCor2_s%d",i+1,i+1,i+1),"","colz");
    //ePpCor2[i] = new TGraph(1, ePmMax[i], ePdMax[i]);
    //ePpCor2[i]->Draw("same");
  }

  std::cout<<"elecPCor2 Done"<<std::endl;

  //TGraph *ePpCor3[6];

  TCanvas *elecPCor3 = new TCanvas("elecPCor3","Change in Electron Momentum versus Electron Momentum (Calculated)",200,10,700,780);
  elecPCor3->Divide(3,2);
  for(int i=0;i<6;i++){
    elecPCor3->cd(i+1);
    electron_momentum->Draw(Form("el_p_Difference_Sec_%d:el_p_Calculated_Sec_%d>> elPCor2_s%d",i+1,i+1,i+1),"","colz");
    //ePpCor3[i] = new TGraph(1, ePcMax[i], ePdMax[i]);
    //ePpCor3[i]->Draw("same");
  }

  std::cout<<"elecPCor3 Done"<<std::endl;

  TCanvas *SFall = new TCanvas("SFall","Sampling Fraction Histogram (All Sectors)",200,10,700,780);
  SFall->cd();
  SF_sec->Draw("Sampling_Fraction_All:Momentum_for_All>> SFH","","colz");

  std::cout<<"SFall Done"<<std::endl;

  TCanvas *SFsecs = new TCanvas("SFsecs","Sampling Fraction Histograms (by sector)",200,10,700,780);
  SFsecs->Divide(3,2);
  SFsecs->cd(1);
  SF_sec->Draw("Sampling_Fraction_Sec_1:Momentum_for_Sec_1>> SFHs1","","colz");
  SFsecs->cd(2);
  SF_sec->Draw("Sampling_Fraction_Sec_2:Momentum_for_Sec_2>> SFHs2","","colz");
  SFsecs->cd(3);
  SF_sec->Draw("Sampling_Fraction_Sec_3:Momentum_for_Sec_3>> SFHs3","","colz");
  SFsecs->cd(4);
  SF_sec->Draw("Sampling_Fraction_Sec_4:Momentum_for_Sec_4>> SFHs4","","colz");
  SFsecs->cd(5);
  SF_sec->Draw("Sampling_Fraction_Sec_5:Momentum_for_Sec_5>> SFHs5","","colz");
  SFsecs->cd(6);
  SF_sec->Draw("Sampling_Fraction_Sec_6:Momentum_for_Sec_6>> SFHs6","","colz");

  std::cout<<"SFsecs Done"<<std::endl;

  TCanvas *wHVsecs = new TCanvas("wHVsecs","wHisto (by sector)",200,10,700,780);
  wHVsecs->Divide(3,2);
  wHVsecs->cd(1);
  w_and_q2->Draw("wHistoV_Sec_1>> wHisto_s1");
  wHVsecs->cd(2);
  w_and_q2->Draw("wHistoV_Sec_2>> wHisto_s2");
  wHVsecs->cd(3);
  w_and_q2->Draw("wHistoV_Sec_3>> wHisto_s3");
  wHVsecs->cd(4);
  w_and_q2->Draw("wHistoV_Sec_4>> wHisto_s4");
  wHVsecs->cd(5);
  w_and_q2->Draw("wHistoV_Sec_5>> wHisto_s5");
  wHVsecs->cd(6);
  w_and_q2->Draw("wHistoV_Sec_6>> wHisto_s6");

  std::cout<<"wHVsecs Done"<<std::endl;

  TCanvas *wHVall =new TCanvas("wHVall","wHisto",200,10,700,780);
  wHVall->cd();
  w_and_q2->Draw("wHistoV>> wHisto_all");
  wHisto_all->ShowBackground(50,"same");
  wHisto_all->Fit("fitT","R",0,1.2);
  gStyle->SetOptFit(1111111);    

  std::cout<<"wHVall Done"<<std::endl;

  /////////// Saving Outfile /////////////
  

  std::cout<<"Saving Results"<<std::endl;

  outH->Write();

  outH->mkdir("SF");
  outH->cd("SF");
  SFall->Write();
  SFsecs->Write();

  outH->mkdir("wHistos");
  outH->cd("wHistos");
  wHVsecs->Write();
  wHVall->Write();

  outH->mkdir("eMomentum");
  outH->cd("eMomentum");
  elecPcal->Write();
  elecPM->Write();
  elecPCor->Write();
  elecPCor2->Write();
  elecPCor3->Write();

  outH->Close();  


  ////////// Saving PDFs  ///////////////  

  //SFall->SaveAs("SFall.pdf","pdf");
  //SFsecs->SaveAs("SFsecs.pdf","pdf");



  ////////// Organizing PDFs  /////////////                                                                                                                                                                                      
  //system("ls");
  //system("mkdir PDFs_from_root/");
  //system(Form("mkdir PDFs_from_root/From_test%d",OutFileNum));
  //system(Form("mv *.pdf PDFs_from_root/From_test%d",OutFileNum));
  system("ls -lh");


  //End of code
  auto finish = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = finish - start;
  std::cout << "Elapsed time: " << elapsed.count()<< " s \n";

}

