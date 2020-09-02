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
#include <TGraphErrors.h>

using namespace clas12;

//double PI= 3.14159265358979323846;

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

  TH2F *SFHs[6];
  for(int i=0;i<6;i++){
    SFHs[i] = new TH2F(Form("SFHs%d",i+1), Form("Sampling Fraction versus Momentum Histogram (Sec %d)",i+1), 600, 0, 12,  50, 0, 0.5);
    SFHs[i]->GetXaxis()->SetTitle("Momentum (GeV)");
    SFHs[i]->GetYaxis()->SetTitle("Sampling Fraction");
  }

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
    elPCor2_s[i] = new TH2F(Form("elPCor2_s%d",i+1),Form("Change in Electron Momentum vs Electron Momentum (Sec %d)",i+1), 45, 3, 12, 30, -0.3, 0.6);
    //elPCor2_s[i] = new TH2F(Form("elPCor2_s%d",i+1),Form("Change in Electron Momentum vs Electron Momentum (Sec %d)",i+1), 15, 0, 12, 52, -0.3, 1); 
    //elPCor3_s[i] = new TH2F(Form("elPCor3_s%d",i+1),Form("Change in Electron Momentum vs Electron Momentum (Sec %d)",i+1), 15, 0, 12, 52, -0.3, 1);
    elPCor3_s[i] = new TH2F(Form("elPCor3_s%d",i+1),Form("Change in Electron Momentum vs Electron Momentum (Sec %d)",i+1), 45, 3, 12, 30, -0.3, 0.6);
  }
  
  //auto testslicefit;//[6];
  // for(int i=0;i<6;i++){
  //   testslicefit[i] = new TH2F(Form("sliceTest_s%d",i+1),Form("sliceTest_s%d",i+1));
  // }

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

  fitT->SetParameter(0,1512);
  fitT->SetParLimits(0,1480,1600);
  fitT->SetParameter(1,0.97);
  fitT->SetParLimits(1,0.96,0.98);
  fitT->SetParameter(2,0.07);
  fitT->SetParLimits(2,0.06,0.09);
  fitT->SetParameter(3,0);
  fitT->SetParLimits(4,-10,80);
  fitT->SetParLimits(5,-100,-20);


  TF1 *fitG[6][6];
  for(int i=0;i<6;i++){
    for(int n=0;n<6;n++){
      fitG[i][n] = new TF1(Form("fitG_sec_%d_reg_%d",i+1,n+1),"[Amp]*exp(-0.5*((x-[peak])/[sigma])*((x-[peak])/[sigma]))",-1,1);
      fitG[i][n]->SetParameter(1,0);
      fitG[i][n]->SetParLimits(1,-1,1);
      fitG[i][n]->SetParameter(2,0.4);
      fitG[i][n]->SetParLimits(2,-1,1);
    }
  }
  
  double ePcMax[6];
  double ePmMax[6];
  double ePdMax[6];
  
  double ePdcMax[6][6];
  double ePdmMax[6][6];

  double ePdcProjPeaks1[6];
  double ePdcProjPeakErrors1[6];
  double ePdcProjPeaks2[6];
  double ePdcProjPeakErrors2[6];
  double ePdcProjPeaks3[6];
  double ePdcProjPeakErrors3[6];
  double ePdcProjPeaks4[6];
  double ePdcProjPeakErrors4[6];
  double ePdcProjPeaks5[6];
  double ePdcProjPeakErrors5[6];
  double ePdcProjPeaks6[6];
  double ePdcProjPeakErrors6[6];

  double ePdmProjPeaks1[6];
  double ePdmProjPeakErrors1[6];
  double ePdmProjPeaks2[6];
  double ePdmProjPeakErrors2[6];
  double ePdmProjPeaks3[6];
  double ePdmProjPeakErrors3[6];
  double ePdmProjPeaks4[6];
  double ePdmProjPeakErrors4[6];
  double ePdmProjPeaks5[6];
  double ePdmProjPeakErrors5[6];
  double ePdmProjPeaks6[6];
  double ePdmProjPeakErrors6[6];
  
  //double XcorBins[]={6.8, 7.6, 8.4, 9.2, 10, 10.8};
  //double XcorBinsError[]={0.4, 0.4, 0.4, 0.4, 0.4, 0.4};
  //double XcorBins[]={8.5, 9.1, 9.3, 9.5, 9.7, 10.3};
  //double XcorBinsError[]={0.5, 0.1, 0.1, 0.1, 0.1, 0.5};
  double XcorBins[]={8.5, 9.1, 9.3, 9.5, 9.7};
  double XcorBinsError[]={0.5, 0.1, 0.1, 0.1, 0.1};

  //auto *eCorEqM[6];

  TF1 *sliceFit[6];
  for(int i=0;i<6;i++){
    sliceFit[i] = new TF1(Form("sliceFit_sec%d",i+1),"[p1]*x+[p0]");
  }


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

  std::cout<<"elecPCor2 Started"<<std::endl;




  //TGraph *ePpCor3[6];

  TCanvas *elecPCor3 = new TCanvas("elecPCor3","Change in Electron Momentum versus Electron Momentum (Calculated)",200,10,700,780);
  elecPCor3->Divide(3,2);
  for(int i=0;i<6;i++){
    elecPCor3->cd(i+1);
    electron_momentum->Draw(Form("el_p_Difference_Sec_%d:el_p_Calculated_Sec_%d>> elPCor3_s%d",i+1,i+1,i+1),"","colz");
    //ePpCor3[i] = new TGraph(1, ePcMax[i], ePdMax[i]);
    //ePpCor3[i]->Draw("same");
  }

  std::cout<<"elecPCor3 Started"<<std::endl;



  TH1D *projyCor2[6][6];
  TCanvas *ProjYelecPCor2[6];
  for(int i=0;i<6;i++){
    ProjYelecPCor2[i] = new TCanvas(Form("ProjYelecPCor2_s%d",i+1),Form("Y Projection of Difference in Momentum (Measured: Sec %d)",i+1),200,10,700,780);
    ProjYelecPCor2[i]->Divide(3,2);
    //for(int n=0;n<6;n++){
    for(int n=0;n<5;n++){
      if(n==0){
	//projyCor2[i][n] = elPCor2_s[i]->ProjectionY(Form("projYM%d_Sec_%d",n+1,i+1),1,8);
	//projyCor2[i][n]->SetTitle("Difference in Momentum for Measured Range: < 6.4 GeV");
        projyCor2[i][n] = elPCor2_s[i]->ProjectionY(Form("projYM%d_Sec_%d",n+1,i+1),26,30);
        projyCor2[i][n]->SetTitle("Difference in Momentum for Measured Range: 8 to 9 GeV");
      }
      //if(n>0){
      if(n>0 && n<5){
	//projyCor2[i][n] = elPCor2_s[i]->ProjectionY(Form("projYM%d_Sec_%d",n+1,i+1),n+8,n+9);
        //projyCor2[i][n]->SetTitle(Form("Difference in Momentum for Measured Range: %4.1f to %4.1f GeV", (0.8*n)+6.4, (0.8*n)+7.2));
	projyCor2[i][n] = elPCor2_s[i]->ProjectionY(Form("projYM%d_Sec_%d",n+1,i+1),n+30,n+30);
	projyCor2[i][n]->SetTitle(Form("Difference in Momentum for Measured Range: %4.1f to %4.1f GeV", (0.2*n)+8.8, (0.2*n)+9));
      }
      if(n==5){
	projyCor2[i][n] = elPCor2_s[i]->ProjectionY(Form("projYM%d_Sec_%d",n+1,i+1),35,40);
	projyCor2[i][n]->SetTitle("Difference in Momentum for Measured Range: 9.8 to 10.8 GeV");
      }
      projyCor2[i][n]->GetXaxis()->SetTitle("Difference in Momentum (GeV)");
      ProjYelecPCor2[i]->cd(n+1);
      projyCor2[i][n]->Draw();
      ePdmMax[i][n] = projyCor2[i][n]->GetXaxis()->GetBinCenter(projyCor2[i][n]->GetMaximumBin());
      fitG[i][n]->SetParameter(0,projyCor2[i][n]->GetBinContent(projyCor2[i][n]->GetMaximumBin()));
      fitG[i][n]->SetParLimits(0,(projyCor2[i][n]->GetBinContent(projyCor2[i][n]->GetMaximumBin()))*0.8,(projyCor2[i][n]->GetBinContent(projyCor2[i][n]->GetMaximumBin()))*1.2);
      fitG[i][n]->SetParameter(1,ePdmMax[i][n]);
      if(ePdmMax[i][n]>0){
	fitG[i][n]->SetParLimits(1,ePdmMax[i][n]*0.8,ePdmMax[i][n]*1.2);
      }
      if(ePdmMax[i][n]<0){
        fitG[i][n]->SetParLimits(1,ePdmMax[i][n]*1.2,ePdmMax[i][n]*0.8);
      } 
      projyCor2[i][n]->Fit(Form("fitG_sec_%d_reg_%d",i+1,n+1));
      gStyle->SetOptFit(11111111);
    }
  }

  std::cout<<"ProjYelecPCor2 Done"<<std::endl;




  for(int n=0;n<6;n++){
    ePdmProjPeaks1[n]=fitG[0][n]->GetParameter(1);
    //ePdmProjPeakErrors1[n]=fitG[0][n]->GetParameter(2);
    ePdmProjPeakErrors1[n]= TMath::Abs(fitG[0][n]->GetParameter(2));
 
    ePdmProjPeaks2[n]=fitG[1][n]->GetParameter(1);
    //ePdmProjPeakErrors2[n]=fitG[1][n]->GetParameter(2);
    ePdmProjPeakErrors2[n]= TMath::Abs(fitG[1][n]->GetParameter(2));

    ePdmProjPeaks3[n]=fitG[2][n]->GetParameter(1);
    //ePdmProjPeakErrors3[n]=fitG[2][n]->GetParameter(2);
    ePdmProjPeakErrors3[n]= TMath::Abs(fitG[2][n]->GetParameter(2));

    ePdmProjPeaks4[n]=fitG[3][n]->GetParameter(1);
    //ePdmProjPeakErrors4[n]=fitG[3][n]->GetParameter(2);
    ePdmProjPeakErrors4[n]= TMath::Abs(fitG[3][n]->GetParameter(2));

    ePdmProjPeaks5[n]=fitG[4][n]->GetParameter(1);
    //ePdmProjPeakErrors5[n]=fitG[4][n]->GetParameter(2);
    ePdmProjPeakErrors5[n]= TMath::Abs(fitG[4][n]->GetParameter(2));

    ePdmProjPeaks6[n]=fitG[5][n]->GetParameter(1);
    //ePdmProjPeakErrors6[n]=fitG[5][n]->GetParameter(2);
    ePdmProjPeakErrors6[n]= TMath::Abs(fitG[5][n]->GetParameter(2));
    
    gStyle->SetOptFit(11111111);
  }
  // auto eCorEqMs1 = new TGraphErrors(6, XcorBins, ePdmProjPeaks1, XcorBinsError, ePdmProjPeakErrors1);    
  // auto eCorEqMs2 = new TGraphErrors(6, XcorBins, ePdmProjPeaks2, XcorBinsError, ePdmProjPeakErrors2);
  // auto eCorEqMs3 = new TGraphErrors(6, XcorBins, ePdmProjPeaks3, XcorBinsError, ePdmProjPeakErrors3);
  // auto eCorEqMs4 = new TGraphErrors(6, XcorBins, ePdmProjPeaks4, XcorBinsError, ePdmProjPeakErrors4);
  // auto eCorEqMs5 = new TGraphErrors(6, XcorBins, ePdmProjPeaks5, XcorBinsError, ePdmProjPeakErrors5);
  // auto eCorEqMs6 = new TGraphErrors(6, XcorBins, ePdmProjPeaks6, XcorBinsError, ePdmProjPeakErrors6);
  auto eCorEqMs1 = new TGraphErrors(5, XcorBins, ePdmProjPeaks1, XcorBinsError, ePdmProjPeakErrors1);
  auto eCorEqMs2 = new TGraphErrors(5, XcorBins, ePdmProjPeaks2, XcorBinsError, ePdmProjPeakErrors2);
  auto eCorEqMs3 = new TGraphErrors(5, XcorBins, ePdmProjPeaks3, XcorBinsError, ePdmProjPeakErrors3);
  auto eCorEqMs4 = new TGraphErrors(5, XcorBins, ePdmProjPeaks4, XcorBinsError, ePdmProjPeakErrors4);
  auto eCorEqMs5 = new TGraphErrors(5, XcorBins, ePdmProjPeaks5, XcorBinsError, ePdmProjPeakErrors5);
  auto eCorEqMs6 = new TGraphErrors(5, XcorBins, ePdmProjPeaks6, XcorBinsError, ePdmProjPeakErrors6);
  for(int i=0;i<6;i++){
    elecPCor2->cd(i+1);
    electron_momentum->Draw(Form("el_p_Difference_Sec_%d:el_p_Measured_Sec_%d>> elPCor2_s%d",i+1,i+1,i+1),"","colz");
    if(i==0){
      eCorEqMs1->SetMarkerColor(2);
      eCorEqMs1->SetMarkerStyle(21);
      //eCorEqMs1->Fit("pol1");
      //eCorEqMs1->Fit("pol2");
      eCorEqMs1->Fit(sliceFit[i]);
      eCorEqMs1->Draw("same");                                                                                                                                                                            
    }
    if(i==1){
      eCorEqMs2->SetMarkerColor(2);
      eCorEqMs2->SetMarkerStyle(21);
      //eCorEqMs2->Fit("pol1");
      eCorEqMs2->Fit(sliceFit[i]);
      //eCorEqMs2->Fit(Form("sliceFit_sec%d",i+1));
      eCorEqMs2->Draw("same");
    }
    if(i==2){
      eCorEqMs3->SetMarkerColor(2);
      eCorEqMs3->SetMarkerStyle(21);
      //eCorEqMs3->Fit("pol1");
      eCorEqMs3->Fit(sliceFit[i]);
      eCorEqMs3->Draw("same");
    }
    if(i==3){
      eCorEqMs4->SetMarkerColor(2);
      eCorEqMs4->SetMarkerStyle(21);
      //eCorEqMs4->Fit("pol1");
      eCorEqMs4->Fit(sliceFit[i]);
      eCorEqMs4->Draw("same");
    }
    if(i==4){
      eCorEqMs5->SetMarkerColor(2);
      eCorEqMs5->SetMarkerStyle(21);
      //eCorEqMs5->Fit("pol1");
      eCorEqMs5->Fit(sliceFit[i]);
      eCorEqMs5->Draw("same");
    }
    if(i==5){
      eCorEqMs6->SetMarkerColor(2);
      eCorEqMs6->SetMarkerStyle(21);
      //eCorEqMs6->Fit("pol1");
      eCorEqMs6->Fit(sliceFit[i]);
      eCorEqMs6->Draw("same");
    }
    gStyle->SetOptFit(11111111);
  }

  std::cout<<"elecPCor2 Done"<<std::endl;




  // TCanvas *TestSliceFit = new TCanvas("SliceTest","Test of Slice Fit", 200,10,700,780);
  // TestSliceFit->Divide(3,2);
  // for(int i=0;i<6;i++){
  //   auto testslicefit = elPCor2_s[i]->FitSlicesY();
  //   TestSliceFit->cd(i+1);
  //   //elPCor2_s[i]->FitSlicesY()->Draw();
  //   testslicefit->Draw();
  // }

  // std::cout<<"TestSliceFit Done"<<std::endl;
  
  
  
  
  
  TCanvas *elecPCor2JP = new TCanvas("elecPCor2JP","Just the peak positions from elecPCor2",200,10,700,780);
  elecPCor2JP->Divide(3,2);
  for(int i=0;i<6;i++){
    elecPCor2JP->cd(i+1);
    if(i==0){
      eCorEqMs1->SetMarkerColor(2);
      eCorEqMs1->SetMarkerStyle(21);
      //eCorEqMs1->Fit("pol1");
      eCorEqMs1->Fit(sliceFit[i]);
      eCorEqMs1->Draw("ALP");
    }
    if(i==1){
      eCorEqMs2->SetMarkerColor(2);
      eCorEqMs2->SetMarkerStyle(21);
      //eCorEqMs2->Fit("pol1");
      eCorEqMs2->Fit(sliceFit[i]);
      eCorEqMs2->Draw("ALP");
    }
    if(i==2){
      eCorEqMs3->SetMarkerColor(2);
      eCorEqMs3->SetMarkerStyle(21);
      //eCorEqMs3->Fit("pol1");
      eCorEqMs3->Fit(sliceFit[i]);
      eCorEqMs3->Draw("ALP");
    }
    if(i==3){
      eCorEqMs4->SetMarkerColor(2);
      eCorEqMs4->SetMarkerStyle(21);
      //eCorEqMs4->Fit("pol1");
      eCorEqMs4->Fit(sliceFit[i]);
      eCorEqMs4->Draw("ALP");
    }
    if(i==4){
      eCorEqMs5->SetMarkerColor(2);
      eCorEqMs5->SetMarkerStyle(21);
      //eCorEqMs5->Fit("pol1");
      eCorEqMs5->Fit(sliceFit[i]);
      eCorEqMs5->Draw("ALP");
    }
    if(i==5){
      eCorEqMs6->SetMarkerColor(2);
      eCorEqMs6->SetMarkerStyle(21);
      //eCorEqMs6->Fit("pol1");
      eCorEqMs6->Fit(sliceFit[i]);
      eCorEqMs6->Draw("ALP");
    }
    gStyle->SetOptFit(11111111);
  }


  std::cout<<"elecPCor2JP Done"<<std::endl;




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
        projyCor3[i][n]->SetTitle(Form("Difference in Momentum for Calculated Range: %4.1f to %4.1f GeV", (0.8*n)+6.4, (0.8*n)+7.2));
      } 
      // projyCor3[i][n] = elPCor3_s[i]->ProjectionY(Form("projYC%d_Sec_%d",n+1,i+1),(2*n)+8,(2*n)+10);
      // projyCor3[i][n]->SetTitle(Form("Difference in Momentum for Calculated Range: %d to %d GeV", n+4, n+5));
      projyCor3[i][n]->GetXaxis()->SetTitle("Difference in Momentum (GeV)");
      ProjYelecPCor3[i]->cd(n+1);
      projyCor3[i][n]->Draw();
      ePdcMax[i][n] = projyCor3[i][n]->GetXaxis()->GetBinCenter(projyCor3[i][n]->GetMaximumBin());
      fitG[i][n]->SetParameter(0,projyCor3[i][n]->GetBinContent(projyCor3[i][n]->GetMaximumBin()));
      fitG[i][n]->SetParLimits(0,(projyCor3[i][n]->GetBinContent(projyCor3[i][n]->GetMaximumBin()))*0.8,(projyCor3[i][n]->GetBinContent(projyCor3[i][n]->GetMaximumBin()))*1.2);
      fitG[i][n]->SetParameter(1,ePdcMax[i][n]);
      if(ePdcMax[i][n]>0){
	fitG[i][n]->SetParLimits(1,ePdcMax[i][n]*0.8,ePdcMax[i][n]*1.2);
      }
      if(ePdcMax[i][n]<0){
        fitG[i][n]->SetParLimits(1,ePdcMax[i][n]*1.2,ePdcMax[i][n]*0.8);
      }
      projyCor3[i][n]->Fit(Form("fitG_sec_%d_reg_%d",i+1,n+1));
      gStyle->SetOptFit(11111111);
    }
  }

  std::cout<<"ProjYelecPCor3 Done"<<std::endl;



  for(int n=0;n<6;n++){
    ePdcProjPeaks1[n]=fitG[0][n]->GetParameter(1);
    ePdcProjPeakErrors1[n]=fitG[0][n]->GetParameter(2);

    ePdcProjPeaks2[n]=fitG[1][n]->GetParameter(1);
    ePdcProjPeakErrors2[n]=fitG[1][n]->GetParameter(2);

    ePdcProjPeaks3[n]=fitG[2][n]->GetParameter(1);
    ePdcProjPeakErrors3[n]=fitG[2][n]->GetParameter(2);

    ePdcProjPeaks4[n]=fitG[3][n]->GetParameter(1);
    ePdcProjPeakErrors4[n]=fitG[3][n]->GetParameter(2);

    ePdcProjPeaks5[n]=fitG[4][n]->GetParameter(1);
    ePdcProjPeakErrors5[n]=fitG[4][n]->GetParameter(2);

    ePdcProjPeaks6[n]=fitG[5][n]->GetParameter(1);
    ePdcProjPeakErrors6[n]=fitG[5][n]->GetParameter(2);
  }
  auto eCorEqCs1 = new TGraphErrors(6, XcorBins, ePdcProjPeaks1, XcorBinsError, ePdcProjPeakErrors1);
  auto eCorEqCs2 = new TGraphErrors(6, XcorBins, ePdcProjPeaks2, XcorBinsError, ePdcProjPeakErrors2);
  auto eCorEqCs3 = new TGraphErrors(6, XcorBins, ePdcProjPeaks3, XcorBinsError, ePdcProjPeakErrors3);
  auto eCorEqCs4 = new TGraphErrors(6, XcorBins, ePdcProjPeaks4, XcorBinsError, ePdcProjPeakErrors4);
  auto eCorEqCs5 = new TGraphErrors(6, XcorBins, ePdcProjPeaks5, XcorBinsError, ePdcProjPeakErrors5);
  auto eCorEqCs6 = new TGraphErrors(6, XcorBins, ePdcProjPeaks6, XcorBinsError, ePdcProjPeakErrors6);
  for(int i=0;i<6;i++){
    elecPCor3->cd(i+1);
    electron_momentum->Draw(Form("el_p_Difference_Sec_%d:el_p_Calculated_Sec_%d>> elPCor3_s%d",i+1,i+1,i+1),"","colz");   
    if(i==0){
      eCorEqCs1->SetMarkerColor(2);
      eCorEqCs1->SetMarkerStyle(21);
      eCorEqCs1->Fit("pol1");
      eCorEqCs1->Draw("same");
    }
    if(i==1){
      eCorEqCs2->SetMarkerColor(2);
      eCorEqCs2->SetMarkerStyle(21);
      eCorEqCs2->Fit("pol1");
      eCorEqCs2->Draw("same");
    }
    if(i==2){
      eCorEqCs3->SetMarkerColor(2);
      eCorEqCs3->SetMarkerStyle(21);
      eCorEqCs3->Fit("pol1");
      eCorEqCs3->Draw("same");
    }
    if(i==3){
      eCorEqCs4->SetMarkerColor(2);
      eCorEqCs4->SetMarkerStyle(21);
      eCorEqCs4->Fit("pol1");
      eCorEqCs4->Draw("same");
    }
    if(i==4){
      eCorEqCs5->SetMarkerColor(2);
      eCorEqCs5->SetMarkerStyle(21);
      eCorEqCs5->Fit("pol1");
      eCorEqCs5->Draw("same");
    }
    if(i==5){
      eCorEqCs6->SetMarkerColor(2);
      eCorEqCs6->SetMarkerStyle(21);
      eCorEqCs6->Fit("pol1");
      eCorEqCs6->Draw("same");
    }
  }

  std::cout<<"elecPCor3 Done"<<std::endl;




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
  elecPCor2JP->Write();
  elecPCor3->Write();
  //TestSliceFit->Write();
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



  ////////// Couting Fit Eqs  ///////////////                                                                                                                                                               
  std::cout << endl << "Equations for the corrected momentum measurements (see elecPCor2): "<<endl;
  for(int i=0;i<6;i++){
    if((sliceFit[i]->GetParameter(1))>0){
      std::cout << Form("Sector %d: ",i+1) << (sliceFit[i]->GetParameter(0)) << "*x+" << (sliceFit[i]->GetParameter(1)) <<endl;
    }
    if((sliceFit[i]->GetParameter(1))<0){
      std::cout << Form("Sector %d: ",i+1) << (sliceFit[i]->GetParameter(0)) << "*x" << (sliceFit[i]->GetParameter(1)) <<endl;
    }
  }
  std::cout << endl;

  //End of code
  auto finish = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = finish - start;
  std::cout << "Elapsed time: " << elapsed.count()<< " s \n";

}

