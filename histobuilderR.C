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

using namespace clas12;

double PI= 3.14159265358979323846;

void histobuilderR(){

  //Below is MINE                                                                                                                                                                         
  int OutFileNum=17;
  
  TString inputFile;
  //TString outputFile = Form("test%d.root",OutFileNum);
  
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


  std::cout<<"Opening Program"<<std::endl;
  
  TFile *f=new TFile(inputFile);
  TTree *SF_sec=(TTree*)f->Get("out_tree_SF_sec"); 

  TH2F *SFH = new TH2F("SFH", "Sampling Fraction versus Momentum Histogram (Made from vectors)", 600, 0, 12,  50, 0, 0.5);


  TCanvas *SFall = new TCanvas("SFall","Sampling Fraction Histogram (All Sectors)",200,10,700,780);
  SFall->cd();
  SF_sec->Draw("Sampling_Fraction_All:Momentum_for_All>> SFH","","colz");

  TH2F *SFHs1 = new TH2F("SFHs1", "Sampling Fraction versus Momentum Histogram (Sec 1)", 600, 0, 12,  50, 0, 0.5);
  TH2F *SFHs2 = new TH2F("SFHs2", "Sampling Fraction versus Momentum Histogram (Sec 2)", 600, 0, 12,  50, 0, 0.5);
  TH2F *SFHs3 = new TH2F("SFHs3", "Sampling Fraction versus Momentum Histogram (Sec 3)", 600, 0, 12,  50, 0, 0.5);
  TH2F *SFHs4 = new TH2F("SFHs4", "Sampling Fraction versus Momentum Histogram (Sec 4)", 600, 0, 12,  50, 0, 0.5);
  TH2F *SFHs5 = new TH2F("SFHs5", "Sampling Fraction versus Momentum Histogram (Sec 5)", 600, 0, 12,  50, 0, 0.5);
  TH2F *SFHs6 = new TH2F("SFHs6", "Sampling Fraction versus Momentum Histogram (Sec 6)", 600, 0, 12,  50, 0, 0.5);

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

  SFall->SaveAs("SFall.pdf","pdf");
  SFsecs->SaveAs("SFsecs.pdf","pdf");


  //Organizing PDFs                                                                                                                                                                                        
  system("ls");
  system("mkdir PDFs_from_root/");
  system(Form("mkdir PDFs_from_root/From_test%d",OutFileNum));
  system(Form("mv *.pdf PDFs_from_root/From_test%d",OutFileNum));
  system("ls");


  auto finish = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = finish - start;
  std::cout << "Elapsed time: " << elapsed.count()<< " s \n";

}

