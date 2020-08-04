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

  // TH1F *wHisto_s1 = new TH1F("wHisto_s1","wHisto_s1", 1000, 0, 5);
  // TH1F *wHisto_s2 = new TH1F("wHisto_s2","wHisto_s2", 1000, 0, 5);
  // TH1F *wHisto_s3 = new TH1F("wHisto_s3","wHisto_s3", 1000, 0, 5);
  // TH1F *wHisto_s4 = new TH1F("wHisto_s4","wHisto_s4", 1000, 0, 5);
  // TH1F *wHisto_s5 = new TH1F("wHisto_s5","wHisto_s5", 1000, 0, 5);
  // TH1F *wHisto_s6 = new TH1F("wHisto_s6","wHisto_s6", 1000, 0, 5);
  TH1F *wHisto_all = new TH1F("wHisto_all","wHisto_all", 1000, 0, 5);
  TH1F *wHistoC_all = new TH1F("wHistoC_all","wHistoC_all", 240, 0, 1.2);

  TH1F *wHisto_s[6];
  //TH1F *wHistoC_s[6];
  for(int i=0;i<6;i++){
    wHisto_s[i] = new TH1F(Form("wHisto_s%d",i+1),Form("wHisto_s%d",i+1), 1000, 0, 5);
    //wHistoC_s[i] = new TH1F(Form("wHistoC_s%d",i+1),Form("wHistoC_s%d",i+1), 240, 0, 1.2);
  }

  TH1F *elPM_s[6];
  TH1F *elPCal_s[6];
  TH1F *elPCor_s[6];
  TH2F *elPCor2_s[6];
  TH2F *elPCor3_s[6];
  for(int i=0;i<6;i++){
    elPCor_s[i] = new TH1F(Form("elPCor_s%d",i+1),Form("Change in Electron Momentum (Sec %d)",i+1), 80, -1, 1);
    elPCal_s[i] = new TH1F(Form("elPCal_s%d",i+1),Form("Calculated Electron Momentum (Sec %d)",i+1), 240, 0, 12);
    elPM_s[i] = new TH1F(Form("elPM_s%d",i+1),Form("Measured Electron Momentum (Sec %d)",i+1), 240, 0, 12);
    elPCor2_s[i] = new TH2F(Form("elPCor2_s%d",i+1),Form("Change in Electron Momentum vs Electron Momentum (Sec %d)",i+1), 15, 0, 12, 80, -1, 1); 
    elPCor3_s[i] = new TH2F(Form("elPCor3_s%d",i+1),Form("Change in Electron Momentum vs Electron Momentum (Sec %d)",i+1), 15, 0, 12, 80, -1, 1);
  }
  
  

  ///// Making fits for histograms ///////
  
  
  //TF1 *fitT = new TF1("fitT","[p0]*exp(-0.5*((x-[p1])/[p2])*((x-[p1])/[p2]))+[p3]+[p4]*x",0,1.2);
  TF1 *fitT = new TF1("fitT","[p0]*exp(-0.5*((x-[p1])/[p2])*((x-[p1])/[p2]))+[p3]+[p4]*x+[p5]*x*x",0,1.1);
  
  // TF1 *fitg = new TF1("fitg","gaus",0,1.1);
  // TF1 *fitB = new TF1("fitB","[p0]+[p1]*x+[p2]*x*x",0,1.1);
  // //fitg->SetParameter(0,1509);
  // //fitg->SetParLimits(0,1509*0.5,1509*1.5);
  // fitg->SetParameter(1,0.97);
  // fitg->SetParLimits(1,0.97*0.8,0.97*1.2);
  // fitg->SetParameter(2,0.08);
  // fitg->SetParLimits(2,0.08*0.8,0.08*1.2);
  // TF1 *fitTp = new TF1("fitTp","fitg(0) + fitB(3)",0,1.1);  

  // fitT->SetParameter(0,1509);
  // fitT->SetParLimits(0,1509*0.5,1509*1.5);
  fitT->SetParameter(1,0.97);
  fitT->SetParLimits(1,0.97*0.8,0.97*1.2);
  fitT->SetParameter(2,0.08);
  fitT->SetParLimits(2,0.08*0.8,0.08*1.2);

  TF1 *fitG[6][6];
  for(int i=0;i<6;i++){
    for(int n=0;n<6;n++){
      fitG[i][n] = new TF1(Form("fitG_sec_%d_reg_%d",i+1,n+1),"[p0]*exp(-0.5*((x-[p1])/[p2])*((x-[p1])/[p2]))",-1,1);
      fitG[i][n]->SetParameter(1,0);
      fitG[i][n]->SetParLimits(1,-1,1);
      fitG[i][n]->SetParLimits(2,-1,1);
    }
  }
  
  double ePcMax[6];
  double ePmMax[6];
  double ePdMax[6];
  
  double ePdcMax[6][6];
  double ePdmMax[6][6];

  double ePdcProjPeak[6][6];
  double ePdcProjPeakError[6][6];
  
  double ePmcProjPeak[6][6];
  double ePmcProjPeakError[6][6];
  
  auto eCorEqM[6];
  
  ///// Making/Filling Canvases //////////
  
  
  std::cout<<"Making Canvases"<<std::endl;
  

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
    electron_momentum->Draw(Form("el_p_Difference_Sec_%d:el_p_Calculated_Sec_%d>> elPCor3_s%d",i+1,i+1,i+1),"","colz");
    //ePpCor3[i] = new TGraph(1, ePcMax[i], ePdMax[i]);
    //ePpCor3[i]->Draw("same");
  }

  std::cout<<"elecPCor3 Done"<<std::endl;



  TH1D *projyCor2[6][6];
  TCanvas *ProjYelecPCor2[6];
  for(int i=0;i<6;i++){
    ProjYelecPCor2[i] = new TCanvas(Form("ProjYelecPCor2_s%d",i+1),Form("Y Projection of Difference in Momentum (Measured: Sec %d)",i+1),200,10,700,780);
    ProjYelecPCor2[i]->Divide(3,2);
    for(int n=0;n<6;n++){
      if(n==0){
	projyCor2[i][n] = elPCor2_s[i]->ProjectionY(Form("projYM%d_Sec_%d",n+1,i+1),1,8);
	projyCor2[i][n]->SetTitle("Difference in Momentum for Measured Range: < 6.4 GeV");
      }
      if(n>0){
	projyCor2[i][n] = elPCor2_s[i]->ProjectionY(Form("projYM%d_Sec_%d",n+1,i+1),n+8,n+9);
	projyCor2[i][n]->SetTitle(Form("Difference in Momentum for Measured Range: %4.1f to %4.1f GeV", (0.8*n)+6.4, (0.8*n)+7.2));
      }
      projyCor2[i][n]->GetXaxis()->SetTitle("Difference in Momentum (GeV)");
      ProjYelecPCor2[i]->cd(n+1);
      projyCor2[i][n]->Draw();
      ePdmMax[i][n] = projyCor2[i][n]->GetXaxis()->GetBinCenter(projyCor2[i][n]->GetMaximumBin());
      //projyCor2[i][n]->Fit("gaus");
      fitG[i][n]->SetParameter(0,projyCor2[i][n]->GetBinContent(projyCor2[i][n]->GetMaximumBin()));
      fitG[i][n]->SetParLimits(0,(projyCor2[i][n]->GetBinContent(projyCor2[i][n]->GetMaximumBin()))*0.8,(projyCor2[i][n]->GetBinContent(projyCor2[i][n]->GetMaximumBin()))*1.2);
      fitG[i][n]->SetParameter(1,ePdmMax[i][n]);
      fitG[i][n]->SetParLimits(1,ePdmMax[i][n]*0.9,ePdmMax[i][n]*1.1);
      projyCor2[i][n]->Fit(Form("fitG_sec_%d_reg_%d",i+1,n+1));
      gStyle->SetOptFit(11111111);
      ePdmProjPeak[i][n]=fitG[i][n]->GetParameter(1);
      ePdmProjPeak[i][n]=fitG[i][n]->GetParameter(2);
    }
    //    eCorEqM[i] = new TGraphErrors(6,{6.8,
  }

  // TH1D *projyCor2[6][5];
  // TCanvas *ProjYelecPCor2 = new TCanvas("ProjYelecPCor2","Y Projection of Difference in Momentum (Measure)",200,10,700,780);
  // ProjYelecPCor2->Divide(3,2);
  // for(int n=0;n<5;n++){
  //   //if(n<5){                                                                                                                                                                                           
  //   for(int i=0;i<6;i++){
  //     projyCor2[i][n] = elPCor2_s[i]->ProjectionY(Form("projY%d_Sec_%d",n+1,i+1),(4*n)+8,(4*n)+10);
  //     projyCor2[i][n]->SetTitle(Form("Difference in Momentum for Calculated Range:  %d to %d GeV",((2*n)+4),((2*n)+5)));
  //     projyCor2[i][n]->GetXaxis()->SetTitle("Difference in Momentum (GeV)");
  //     projyCor2[i][n]->SetLineColor(39+(2*i));
  //     ePdmMax[i][n] = projyCor2[i][n]->GetXaxis()->GetBinCenter(projyCor2[i][n]->GetMaximumBin());
  //   }
  //   ProjYelecPCor2->cd(n+1);
  //   projyCor2[0][n]->Draw();
  //   projyCor2[1][n]->Draw("same");
  //   projyCor2[2][n]->Draw("same");
  //   projyCor2[3][n]->Draw("same");
  //   projyCor2[4][n]->Draw("same");
  //   projyCor2[5][n]->Draw("same");
  // }

  // TLegend *legendsec2;
  // legendsec2= new TLegend(2,2,2,2);
  // for(int i=0;i<6;i++){
  //   legendsec2->AddEntry(projyCor2[i][0],Form("Sector %d",i+1));	
  // }           
  // ProjYelecPCor2->cd(6);
  // legendsec2->Draw();

  std::cout<<"ProjYelecPCor2 Done"<<std::endl;




  TH1D *projyCor3[6][6];
  TCanvas *ProjYelecPCor3[6];
  for(int i=0;i<6;i++){
    ProjYelecPCor3[i] = new TCanvas(Form("ProjYelecPCor3_s%d",i+1),Form("Y Projection of Difference in Momentum (Cal: Sec %d)",i+1),200,10,700,780);
    ProjYelecPCor3[i]->Divide(3,2);
    for(int n=0;n<6;n++){
      if(n==0){
        projyCor3[i][n] = elPCor3_s[i]->ProjectionY(Form("projYC%d_Sec_%d",n+1,i+1),1,8);
        projyCor3[i][n]->SetTitle("Difference in Momentum for Calculated Range: <6.4 GeV");
      }
      if(n>0){
        projyCor3[i][n] = elPCor3_s[i]->ProjectionY(Form("projYC%d_Sec_%d",n+1,i+1),n+8,n+9);
        projyCor3[i][n]->SetTitle(Form("Difference in Momentum for Calculated Range: %f to %f GeV", (0.8*n)+6.4, (0.8*n)+7.2));
      } 
      // projyCor3[i][n] = elPCor3_s[i]->ProjectionY(Form("projYC%d_Sec_%d",n+1,i+1),(2*n)+8,(2*n)+10);
      // projyCor3[i][n]->SetTitle(Form("Difference in Momentum for Calculated Range: %d to %d GeV", n+4, n+5));
      projyCor3[i][n]->GetXaxis()->SetTitle("Difference in Momentum (GeV)");
      ProjYelecPCor3[i]->cd(n+1);
      projyCor3[i][n]->Draw();
      ePdcMax[i][n] = projyCor3[i][n]->GetXaxis()->GetBinCenter(projyCor3[i][n]->GetMaximumBin());
      //projyCor3[i][n]->Fit("gaus");
      fitG[i][n]->SetParameter(0,projyCor3[i][n]->GetBinContent(projyCor3[i][n]->GetMaximumBin()));
      fitG[i][n]->SetParLimits(0,(projyCor3[i][n]->GetBinContent(projyCor3[i][n]->GetMaximumBin()))*0.9,(projyCor3[i][n]->GetBinContent(projyCor3[i][n]->GetMaximumBin()))*1.1);
      fitG[i][n]->SetParameter(1,ePdcMax[i][n]);
      fitG[i][n]->SetParLimits(1,ePdcMax[i][n]*0.95,ePdcMax[i][n]*1.05);
      projyCor3[i][n]->Fit(Form("fitG_sec_%d_reg_%d",i+1,n+1));
      gStyle->SetOptFit(11111111);
    }
  }

  std::cout<<"ProjYelecPCor3 Done"<<std::endl;




  TCanvas *SFall = new TCanvas("SFall","Sampling Fraction Histogram (All Sectors)",200,10,700,780);
  SFall->cd();
  SF_sec->Draw("Sampling_Fraction_All:Momentum_for_All>> SFH","","colz");

  std::cout<<"SFall Done"<<std::endl;




  TCanvas *SFsecs = new TCanvas("SFsecs","Sampling Fraction Histograms (by sector)",200,10,700,780);
  SFsecs->Divide(3,2);
  for(int i=0;i<6;i++){
    SFsecs->cd(i+1);
    SF_sec->Draw(Form("Sampling_Fraction_Sec_%d:Momentum_for_Sec_%d>> SFHs%d",i+1,i+1,i+1),"","colz");
  }

  std::cout<<"SFsecs Done"<<std::endl;




  TCanvas *wHVsecs = new TCanvas("wHVsecs","wHisto (by sector)",200,10,700,780);
  wHVsecs->Divide(3,2);
  for(int i=0;i<6;i++){
    wHVsecs->cd(i+1);
    w_and_q2->Draw(Form("wHistoV_Sec_%d>> wHisto_s%d",i+1,i+1));
  }

  std::cout<<"wHVsecs Done"<<std::endl;




  TCanvas *wHVall =new TCanvas("wHVall","wHisto",200,10,700,780);
  wHVall->cd();
  w_and_q2->Draw("wHistoV>> wHisto_all");
  //wHisto_all->ShowBackground(50,"same");
  wHisto_all->Fit("fitT","R",0,1.2);
  gStyle->SetOptFit(1111111);    

  std::cout<<"wHVall Done"<<std::endl;



  TCanvas *wHVall2 =new TCanvas("wHVall2","wHisto2",200,10,700,780);
  wHVall2->cd();
  w_and_q2->Draw("wHistoVC>> wHistoC_all");
  //wHisto_all->ShowBackground(50,"same");                                                                                                                                                              
  wHistoC_all->Fit("fitT","R",0,1.1);
  gStyle->SetOptFit(1111111);

  std::cout<<"wHVall2 Done"<<std::endl;




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
  wHVall2->Write();

  outH->mkdir("eMomentum");
  outH->cd("eMomentum");
  elecPcal->Write();
  elecPM->Write();
  elecPCor->Write();
  //elecPCor2->Write();
  //elecPCor3->Write();

  // outH->mkdir("eMomentum/Projections/Measure");
  // outH->cd("eMomentum/Projections/Measure");
  // for(int i=0;i<6;i++){
  //   ProjYelecPCor2[i]->Write();
  // }

  outH->mkdir("eMomentum/Projections");                                                                                                                                                          
  outH->cd("eMomentum/Projections"); 
  elecPCor2->Write();
  elecPCor3->Write();
  //ProjYelecPCor2->Write();


  outH->mkdir("eMomentum/Projections/Measure");
  outH->cd("eMomentum/Projections/Measure");
  for(int i=0;i<6;i++){
    ProjYelecPCor2[i]->Write();
  }

  outH->mkdir("eMomentum/Projections/Cal");
  outH->cd("eMomentum/Projections/Cal");
  for(int i=0;i<6;i++){
    ProjYelecPCor3[i]->Write();
  }

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

