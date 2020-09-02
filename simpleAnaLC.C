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

int helicity;
double fCup;
vector<int> sectorE;
vector<double> p4_ele_px;
vector<double> p4_ele_py;
vector<double> p4_ele_pz;
vector<double> p4_ele_vx;
vector<double> p4_ele_vy;
vector<double> p4_ele_vz;
vector<double> p4_ele_E;

//Below is MINE
//vector<double> p4_ele_p_Sec[6];

vector<double> el_p_Measured[6];  
vector<double> el_p_Calculated[6];
vector<double> el_p_Difference[6];

vector<double> p4_ele_pR[6];
vector<double> p4_ele_pTh[6];
vector<double> p4_ele_pPh[6];

TH1F *wHisto = new TH1F("wHisto", "wHisto", 1000, 0, 5);
TH1F *q2Histo = new TH1F("q2Histo", "q2Histo", 1000, 0, 10);
TH2F *wq2Histo = new TH2F("wq2Histo", "wq2Histo",  1000, 0, 5,  1000, 0, 10);

vector<double> wHistoV[7];
vector<double> q2HistoV[7];

vector<double> wHistoVC[7];
vector<double> q2HistoVC[7];

vector<double> wHistoVCor[7];
vector<double> q2HistoVCor[7];

vector<double> wHistoVCCor[7];
vector<double> q2HistoVCCor[7];



double kin_W(TLorentzVector ele, float Ebeam){
  TLorentzVector beam(0,0,Ebeam,Ebeam);
  TLorentzVector target(0,0,0,0.93827);
  TLorentzVector fGamma = beam - ele;
  TLorentzVector fCM = fGamma + target;
  return fCM.M();
}


//Below is MINE
//TH2F *ThetaVPHisto = new TH2F("ThetaVPHisto", "Theta versus Momentum Histogram",  1200, 0, 12,  45, 0, 45);
//TH2F *PhiVPHisto = new TH2F("PhiVPHisto", "Phi versus Momentum Histogram",  1200, 0, 12,  390, -30, 360);
//TH2F *ThetaVPhiHisto2 = new TH2F("ThetaVPhiHisto2", "Theta versus Phi Histogram (adjusted)",  390,-30, 360,  45, 0, 45);

TH1F *wHisto2 = new TH1F("wHisto2", "wHisto full with fit", 1000, 0, 5);
TH1F *wHistoCut2 = new TH1F("wHistoCut2", "wHistoCut2", 240, 0, 1.2);
TH2F *BEVPhiHisto2 = new TH2F("BEVPhiHisto2", "Beam Energy versus Phi Histogram (adjusted)",  390,-30, 360,  300, 0, 30);
TH2F *BEVThetaHisto = new TH2F("BEVThetaHisto", "Beam Energy versus Theta Histogram", 90, 0, 45,  300, 0, 30);

//TH2F *SamplingFractionHisto = new TH2F("SamplingFractionHisto", "Sampling Fraction versus Momentum Histogram", 600, 0, 12,  50, 0, 0.5);

//Below was a test of MINE
vector<double> wHisto_RangeAll[10];
//vector<double> wHisto_RangeSecs[10][6];


//Below is for splitting eBeam up by Theta and Sector (search out_tree_BE to comment out of code)
vector<double> BEThetaSector[8][6];


//Below is for tracking the proton data (Cal is for calculated values and M is for values measured values from the detector)
vector<double> proThetaCal[7];
vector<double> proThetaM[7];



double kin_Q2(TLorentzVector ele, float Ebeam){
  TLorentzVector beam(0,0,Ebeam,Ebeam);
  TLorentzVector target(0,0,0,0.93827);
  TLorentzVector fGamma = beam - ele;
  return -fGamma.M2();
}



void fill_output_vector_electron(TLorentzVector el){
  if(el.E() > 0){
    p4_ele_px.push_back(el.Px());
    p4_ele_py.push_back(el.Py());
    p4_ele_pz.push_back(el.Pz());
    p4_ele_E.push_back(el.E());
  }
}



void SetLorentzVector(TLorentzVector &p4,clas12::region_part_ptr rp){
  p4.SetXYZM(rp->par()->getPx(),rp->par()->getPy(),
	     rp->par()->getPz(),p4.M());
}




void simpleAnaLC(){
  float eBeam = 10.646;
  float mProton = 0.98327;
  auto db=TDatabasePDG::Instance();

  //Below is MINE
  int OutFileNum=18;

  TString inputFile;
  TString outputFile = Form("test%d.root",OutFileNum);


  //Below is MINE
  //TH1F *wHisto_Range[11][6];
  TH1F *BE1DHisto[6];
  //TH2F *ThetaVPSec[6];
  //TH1F *BE1DHistoS[6];
  TH1F *BE1DThetaHisto[8];
  //TH2F *SFHistoSec[6];


  // for(int s=0; s<11; s++){
  //   for(int n=0; n<6;n++){
  //     wHisto_Range[s][n] = new TH1F(Form("wHisto_Range%d_sec%d", s + 1,n+1), Form("wHisto_Range%d_sec%d", s + 1,n+1), 50, 0, 5);
  //     //wHisto_RangeV.push_back(wHisto_Range[s][n]);
  //     // BE1DHistoS[n] = new TH1F(Form("BE1DHisto_Sec_%d_Small",n+1), Form("BE1DHisto_Sec_%d_Small",n+1), 150, 0, 15);
  //   }
  // }

  for(int n=0; n<6;n++){
    BE1DHisto[n] = new TH1F(Form("BE1DHisto_Sec_%d",n+1), Form("BE1DHisto_Sec_%d",n+1), 300, 0, 20);
    //ThetaVPSec[n] = new TH2F(Form("ThetaVPSec%d",n+1), Form("Theta versus Momentum Sector %d",n+1),  1200, 0, 12,  90, 0, 45);
    BE1DThetaHisto[n] = new TH1F(Form("BE1DThetaHisto_Range_%d",n+1), Form("BE1DThetaHisto_Range_%d",n+1), 300, 0, 20);
    //SFHistoSec[n]= new TH2F(Form("SF_Sec_%d",n+1), Form("SF vs Momentum (Sector %d)",n+1), 600, 0, 12,  50, 0, 0.5);
  }
  
  BE1DThetaHisto[6] = new TH1F("BE1DThetaHisto_Range_7", "BE1DThetaHisto_Range_7", 300, 0, 20);
  BE1DThetaHisto[7] = new TH1F("BE1DThetaHisto_Range_8", "BE1DThetaHisto_Range_8", 300, 0, 20);
  

  // TH1F *wHisto_Range[10];
  // for(int s=0; s<10; s++){
  //   wHisto_Range[s] = new TH1F(Form("wHisto_Range%d", s + 1), Form("wHisto_Range%d", s + 1), 1000, 0, 5);
  // }




  //Below is a test of the tree histograms for Sampling Fraction
  vector<double> SFvector[7];
  vector<double> SFmomentumVector[7];
  vector<double> SFvectorCor[7];
  vector<double> SFmomentumVectorCor[7];
  

  for(Int_t i=1;i<gApplication->Argc();i++){
    TString opt=gApplication->Argv(i);
    if((opt.Contains(".hipo"))){
      inputFile=opt(5,opt.Sizeof());
    }
  }

  
  if(inputFile==TString())  {
    std::cout << " *** please provide a file name..." << std::endl;
    exit(0);
  }



  /////////////////////////////////////

   
  cout<<"Analysing hipo file "<<inputFile<<endl;


  TLorentzVector beam(0, 0, 10.646, 10.646);
  TLorentzVector el;
  TLorentzVector elmc;
  TLorentzVector target(0,0,0,db->GetParticle(2212)->Mass());  
  TLorentzVector q;
  TLorentzVector pro;
  TLorentzVector elCor;


  auto start = std::chrono::high_resolution_clock::now();

  gBenchmark->Start("timer");
  int counter=0;
  /////////////////////////////////////
  
  std::cout << " reading file example program (HIPO) "  << __cplusplus << std::endl;

  hipo::reader  reader;
  cout<<"Analysing hipo file "<<inputFile<<endl;

  reader.open(inputFile);

  hipo::dictionary factory;
  reader.readDictionary(factory);
  TFile *out;

  out = new TFile(outputFile, "RECREATE");
 
  TTree out_tree("out_tree","out_tree");
  out_tree.Branch("helicity", &helicity);
  out_tree.Branch("fCup", &fCup);
  
  //electrons
  out_tree.Branch("sectorE", &sectorE);
  out_tree.Branch("p4_ele_px", &p4_ele_px);
  out_tree.Branch("p4_ele_py", &p4_ele_py);
  out_tree.Branch("p4_ele_pz", &p4_ele_pz);
  out_tree.Branch("p4_ele_E", &p4_ele_E);

  out_tree.Branch("p4_ele_vx", &p4_ele_vx);
  out_tree.Branch("p4_ele_vy", &p4_ele_vy);
  out_tree.Branch("p4_ele_vz", &p4_ele_vz);


  TTree out_tree_electron_momentum("out_tree_electron_momentum","out_tree_electron_momentum");
  for(int i=0;i<6;i++){
    out_tree_electron_momentum.Branch(Form("el_p_Measured_Sec_%d",i+1),&el_p_Measured[i]);
    out_tree_electron_momentum.Branch(Form("el_p_Calculated_Sec_%d",i+1),&el_p_Calculated[i]);
    out_tree_electron_momentum.Branch(Form("el_p_Difference_Sec_%d",i+1),&el_p_Difference[i]);
  }


  TTree out_tree_p_Polar("out_tree_p_Polar","out_tree_p_Polar");
  for(int i=0;i<6;i++){
    out_tree_p_Polar.Branch(Form("p4_ele_pR_Sec%d",i+1), &p4_ele_pR[i]);
    out_tree_p_Polar.Branch(Form("p4_ele_pTh_Sec%d",i+1), &p4_ele_pTh[i]);
    out_tree_p_Polar.Branch(Form("p4_ele_pPh_Sec%d",i+1), &p4_ele_pPh[i]);
  }


  TTree out_tree_wHisto_Range_All("out_tree_wHisto_Range_All","out_tree_wHisto_Range_All");
  for(int i=0;i<10;i++){
    out_tree_wHisto_Range_All.Branch(Form("wHisto_Range_to_%d_GeV",i+2),&wHisto_RangeAll[i]);
  }


  //Below is a test of the tree histograms for Sampling Fraction
  TTree out_tree_SF_sec("out_tree_SF_sec","out_tree_SF_sec");
  for(int i=0;i<6;i++){
    out_tree_SF_sec.Branch(Form("Sampling_Fraction_Sec_%d",i+1),&SFvector[i]);
    out_tree_SF_sec.Branch(Form("Momentum_for_Sec_%d",i+1),&SFmomentumVector[i]);
    //Below is the corrected trees
    out_tree_SF_sec.Branch(Form("Sampling_Fraction_Sec_%d_Corrected",i+1),&SFvectorCor[i]);
    out_tree_SF_sec.Branch(Form("Momentum_for_Sec_%d_Corrected",i+1),&SFmomentumVectorCor[i]);
  }
  out_tree_SF_sec.Branch("Sampling_Fraction_All",&SFvector[6]);
  out_tree_SF_sec.Branch("Momentum_for_All",&SFmomentumVector[6]);
  out_tree_SF_sec.Branch(Form("Sampling_Fraction_Sec_%d_Corrected",i+1),&SFvectorCor[6]);
  out_tree_SF_sec.Branch(Form("Momentum_for_Sec_%d_Corrected",i+1),&SFmomentumVectorCor[6]);

  //Below is to contain all the beam energy data
  TTree out_tree_BE("out_tree_BE","out_tree_BE");
  for(int n=0;n<8;n++){
    for(int i=0;i<6;i++){
      if(n<4){
	out_tree_BE.Branch(Form("BE_Sec_%d_Theta_%d_to_%d",i+1,n+6,n+7),&BEThetaSector[n][i]);
      }
      if(n==4){
	out_tree_BE.Branch(Form("BE_Sec_%d_Theta_10_to_12.5",i+1),&BEThetaSector[n][i]);
      }
      if(n==5){
	out_tree_BE.Branch(Form("BE_Sec_%d_Theta_12.5_to_15",i+1),&BEThetaSector[n][i]);
      }
      if(n==6){
	out_tree_BE.Branch(Form("BE_Sec_%d_Theta_15_to_17.5",i+1),&BEThetaSector[n][i]);
      }
      if(n==7){
	out_tree_BE.Branch(Form("BE_Sec_%d_Theta_17.5_to_20",i+1),&BEThetaSector[n][i]);
      }
    }
  }


  //Below is to contain the proton angle data
  TTree out_tree_Pro_Theta("out_tree_Pro_Theta","out_tree_Pro_Theta");
  for(int i=0;i<6;i++){
    out_tree_Pro_Theta.Branch(Form("Pro_Theta_Cal_Sec_%d",i+1),&proThetaCal[i]);
    out_tree_Pro_Theta.Branch(Form("Pro_Theta_Measured_Sec_%d",i+1),&proThetaM[i]);
  }
  out_tree_Pro_Theta.Branch("Pro_Theta_Cal_All",&proThetaCal[6]);
  out_tree_Pro_Theta.Branch("Pro_Theta_Measured_All",&proThetaM[6]);



  TTree out_tree_w_and_q2("out_tree_w_and_q2","out_tree_w_and_q2");
  for(int i=0;i<6;i++){
    out_tree_w_and_q2.Branch(Form("wHistoV_Sec_%d",i+1),&wHistoV[i]);
    out_tree_w_and_q2.Branch(Form("q2HistoV_Sec_%d",i+1),&q2HistoV[i]);
    out_tree_w_and_q2.Branch(Form("wHistoVC_Sec_%d",i+1),&wHistoVC[i]);
    out_tree_w_and_q2.Branch(Form("q2HistoVC_Sec_%d",i+1),&q2HistoVC[i]);

    out_tree_w_and_q2.Branch(Form("wHistoV_Sec_%d_Cor",i+1),&wHistoVCor[i]);
    out_tree_w_and_q2.Branch(Form("q2HistoV_Sec_%d_Cor",i+1),&q2HistoVCor[i]);
    out_tree_w_and_q2.Branch(Form("wHistoVC_Sec_%d_Cor",i+1),&wHistoVCCor[i]);
    out_tree_w_and_q2.Branch(Form("q2HistoVC_Sec_%d_Cor",i+1),&q2HistoVCCor[i]);
  }
  out_tree_w_and_q2.Branch("wHistoV",&wHistoV[6]);
  out_tree_w_and_q2.Branch("q2HistoV",&q2HistoV[6]);
  out_tree_w_and_q2.Branch("wHistoVC",&wHistoVC[6]);
  out_tree_w_and_q2.Branch("q2HistoVC",&q2HistoVC[6]);

  out_tree_w_and_q2.Branch("wHistoVCor",&wHistoVCor[6]);
  out_tree_w_and_q2.Branch("q2HistoVCor",&q2HistoVCor[6]);
  out_tree_w_and_q2.Branch("wHistoVCCor",&wHistoVCCor[6]);
  out_tree_w_and_q2.Branch("q2HistoVCCor",&q2HistoVCCor[6]);
  //End of TTree creation


  hipo::event event;
  hipo::bank PART(factory.getSchema("REC::Particle"));
  hipo::bank PartCalorimeter(factory.getSchema("REC::Calorimeter"));
  hipo::bank EVENT(factory.getSchema("REC::Event"));
  hipo::bank Traj(factory.getSchema("REC::Traj"));
  hipo::bank RConfig(factory.getSchema("RUN::config"));

 
  QADB * qa = new QADB("/w/hallb-scifs17exp/clas12/markov/dataQuality/clasqaDB/srcC/qaTree.merged.json");

  //Below is for tests when I want to cout a limited amount of information
  int limitCO=0;
  int limitCO2=0;
  int limitCO3=0;
  int elCount=0;
  int tCount=0;
  int proSearch=2;
  int proCount=0;
  int BEThetaRegion=0;
  int ProOelQ1=0;
  int ProOelQ2=0;
  //if ProOelQ2 is 1, then eBeam will be determined by the equation with the proton. Otherwise, only electron data will be used

  //Below is for the corrected momentum value
  double elecPchange=0;
  double elecPCorrected=0;

  while(reader.next()==true){
    p4_ele_px.clear();
    p4_ele_py.clear();
    p4_ele_pz.clear();
    p4_ele_vx.clear();
    p4_ele_vy.clear();
    p4_ele_vz.clear();
    p4_ele_E.clear();
    sectorE.clear();


    //Below is MINE
    for(int i=0;i<6;i++){
      el_p_Measured[i].clear();
      el_p_Calculated[i].clear();
      el_p_Difference[i].clear();

      p4_ele_pR[i].clear();
      p4_ele_pTh[i].clear();
      p4_ele_pPh[i].clear();
      SFvector[i].clear();
      SFmomentumVector[i].clear();
      proThetaCal[i].clear();
      proThetaM[i].clear();
      wHistoV[i].clear();
      q2HistoV[i].clear();
      wHistoVC[i].clear();
      q2HistoVC[i].clear();
      SFvectorCor[i].clear();
      SFmomentumVectorCor[i].clear();
      wHistoVCor[i].clear();
      q2HistoVCor[i].clear();
      wHistoVCCor[i].clear();
      q2HistoVCCor[i].clear();
    }
    SFvector[6].clear();
    SFmomentumVector[6].clear();
    proThetaCal[6].clear();
    proThetaM[6].clear();
    wHistoV[6].clear();
    q2HistoV[6].clear();
    wHistoVC[6].clear();
    q2HistoVC[6].clear();
    SFvectorCor[6].clear();
    SFmomentumVectorCor[6].clear();
    wHistoVCor[6].clear();
    q2HistoVCor[6].clear();
    wHistoVCCor[6].clear();
    q2HistoVCCor[6].clear();

    for(int i=0;i<10;i++){
      wHisto_RangeAll[i].clear();
    }

    //Clearing out_tree_BE
    for(int n=0;n<8;n++){
      for(int i=0;i<6;i++){
	BEThetaSector[n][i].clear();
      }
    }

    counter++;

    reader.read(event);
    event.getStructure(PART);
    event.getStructure(EVENT);
    event.getStructure(PartCalorimeter);
    event.getStructure(Traj);
    event.getStructure(RConfig);



    //Finding the electron
    int pid = PART.getInt("pid",0);
    int charge = PART.getShort("charge",0);
    int id = PART.getInt("pid",0);
    float startTime = EVENT.getFloat("startTime", 0);
    int status = PART.getInt("status", 0);
    int PCALSize = PartCalorimeter.getSize();
    int partSize = PART.getSize();


    //Finding the proton
    int pidP = PART.getInt("pid",proSearch);
    int chargeP = PART.getShort("charge",proSearch);
    int idP = PART.getInt("pid",proSearch);
    int statusP = PART.getInt("status", proSearch);

    for(int i=0;i<partSize;i++){
      proSearch=i;
      pidP = PART.getInt("pid",proSearch);
      chargeP = PART.getShort("charge",proSearch);
      idP = PART.getInt("pid",proSearch);
      statusP = PART.getInt("status", proSearch);
      if(idP == 2212 && statusP > 999 && statusP < 5000 && partSize > 0 && chargeP == 1 && PART.getFloat("vz", proSearch) > -10 && PART.getFloat("vz", proSearch) < 5 && startTime > 0){
    	proCount=proCount+1;
    	i=partSize+1;
      }
    }
    if(id == 11 && status < -1999 && status > -4000 && partSize > 0 && charge == -1 && PART.getFloat("vz",0) > -10 && PART.getFloat("vz",0) < 5 && startTime > 0){
      elCount=elCount+1;
    }
    if(id == 11 && status < -1999 && status > -4000 && partSize > 0 && charge == -1 && idP == 2212 && statusP > 999 && statusP < 5000 && partSize > 0 && chargeP == 1 && PART.getFloat("vz",0) > -10 && PART.getFloat("vz",0) < 5 && startTime > 0 && PART.getFloat("vz",proSearch) > -10 && PART.getFloat("vz",proSearch) < 5){
      tCount=tCount+1;
    }

    //Displaying data
    if(limitCO==500000){
      limitCO=0;
      limitCO2=1;
      cout<<endl<<"ProCount= "<<proCount<<endl;
      cout<<"proSearch= "<<proSearch<<endl;
      cout<<"chargeP= "<<chargeP<<endl;
      cout<<"idP= "<<idP<<endl;
      cout<<"statusP= "<<statusP<<endl<<endl;
      cout<<"Elect: "<<endl<<"elCount= "<<elCount<<endl<<"pid= "<<pid<<endl<<"id= "<<id<<endl<<"status= "<<status<<endl<<endl;
      cout<<"Total: "<<tCount<<endl<<"Num pro missed= "<<proCount-tCount<<endl<<"Num of el missed= "<<elCount-tCount<<endl<<endl;
      cout<<"PCALSize= "<<PCALSize<<endl;
      cout<<"partSize= "<<partSize<<endl<<endl;
      cout<<endl;
    }
    //limitCO=limitCO+1;
    //Comment line above to stop couting info


    //Below is MINE 
    int secNum = PartCalorimeter.getInt("sector",0);
    int TestRangeNum=0;

    int nEvent = RConfig.getInt("event", 0);
    int runNum = RConfig.getInt("run", 0);
    if(qa->Query(runNum,nEvent)) {
      if(qa->Golden()) {
       	if (id == 11 && status < -1999 && status > -4000 && partSize > 0 && charge == -1 && PART.getFloat("vz",0) > -10 && PART.getFloat("vz",0) < 5 && startTime > 0){
       	  el.SetXYZM(PART.getFloat("px",0), PART.getFloat("py",0), PART.getFloat("pz",0), db->GetParticle(11)->Mass());
	  
	  double pred_e_beam_from_angles = ((db->GetParticle(2212)->Mass())*el.P())/((db->GetParticle(2212)->Mass())-el.P()+(el.P()*TMath::Cos(el.Theta())));
	 
	  //Below is for momentum correction using histotest.root
	  if(secNum==1){
	    //Sec 1 equation
	    elecPchange = -0.270603*el.P()+0.025182;
	  }
          if(secNum==2){
            //Sec 2 equation
	    elecPchange = 0.00706641*el.P()-0.00466279;
          }
          if(secNum==3){
            //Sec 3 equation
	    elecPchange = -0.310751*el.P()+0.035505;
          }
          if(secNum==4){
            //Sec 4 equation
	    elecPchange = -0.346706*el.P()+0.0335085;
          }
          if(secNum==5){
            //Sec 5 equation
	    elecPchange = -0.371143*el.P()+0.0365463;
          }
          if(secNum==6){
            //Sec 6 equation
	    elecPchange = 0.00534595*el.P()-0.00283626;
          }

	  elecPCorrected = el.P() + elecPchange;
	  elCor.SetXYZM(elecPCorrected*TMath::Sin(el.Theta())*TMath::Cos(el.Phi()), elecPCorrected*TMath::Sin(el.Theta())*TMath::Sin(el.Phi()), elecPCorrected*TMath::Cos(el.Theta()), db->GetParticle(11)->Mass());

	  double pred_e_beam_from_angles_cor1 = ((db->GetParticle(2212)->Mass())*elecPCorrected)/((db->GetParticle(2212)->Mass())-elecPCorrected+(elecPCorrected*TMath::Cos(el.Theta())));

	  ProOelQ1=1;


	  //if (el.P() > 1.5){
	  //Below is used to calculate the beam energy using the proton angle
	  if(ProOelQ2==1){       	      
	    if(idP == 2212 && statusP > 999 && statusP < 5000 && partSize > 0 && chargeP == 1 && PART.getFloat("vz",proSearch) > -10 && PART.getFloat("vz",proSearch) < 5){
	      pro.SetXYZM(PART.getFloat("px",proSearch), PART.getFloat("py",proSearch), PART.getFloat("pz",proSearch), db->GetParticle(2212)->Mass());
	      double a0= 1 - 1/(TMath::Cos(el.Theta())-TMath::Sin(el.Theta())/TMath::Tan(-1*pro.Theta()));
	      double a1= TMath::Sin(el.Theta())/TMath::Sin(el.Theta()+pro.Theta());
	      pred_e_beam_from_angles = 2 * (db->GetParticle(2212)->Mass()) * a0/(TMath::Power(a1,2)-TMath::Power(a0,2));
	    }
	    else{
	      ProOelQ1=0;
	    }
	  }
	  //End of the beam energy calculations via proton angle

	  //Below is for storing proton angle values
	  if(idP == 2212 && statusP > 999 && statusP < 5000 && partSize > 0 && chargeP == 1 && PART.getFloat("vz",proSearch) > -10 && PART.getFloat("vz",proSearch) < 5){
	    pro.SetXYZM(PART.getFloat("px",proSearch), PART.getFloat("py",proSearch), PART.getFloat("pz",proSearch), db->GetParticle(2212)->Mass());
	    proThetaM[secNum-1].push_back(pro.Theta()*180/PI);
	    proThetaM[6].push_back(pro.Theta()*180/PI);
	  }
	  double proTHETA = TMath::ATan((db->GetParticle(2212)->Mass())/(((db->GetParticle(2212)->Mass())+pred_e_beam_from_angles)*TMath::Tan(el.Theta()/2)));
	  proThetaCal[secNum-1].push_back(proTHETA*180/PI);
	  proThetaCal[6].push_back(proTHETA*180/PI);
	  //End of storing proton angle values           

	  //if (el.P() > 1.5){
	  //Below is for storing beam energy values
	  if (el.P() > 0){  
	    if(ProOelQ1==1){
	      if(kin_W(el, eBeam)<1.2){
		if((el.Phi()*180/PI)<0 && secNum>1){
		  BEVPhiHisto2->Fill(360+(el.Phi()*180/PI),pred_e_beam_from_angles);
		}
		else{
		  BEVPhiHisto2->Fill((el.Phi()*180/PI),pred_e_beam_from_angles);
		}
		
		
		BEVThetaHisto->Fill((el.Theta()*180/PI),pred_e_beam_from_angles);
		
		BE1DHisto[secNum-1]->Fill(pred_e_beam_from_angles);
		
		if(((180/PI)*el.Theta()) >= 6 && ((180/PI)*el.Theta()) < 7){
		  BEThetaRegion=0;
		}
		if(((180/PI)*el.Theta()) >= 7 && ((180/PI)*el.Theta()) < 8){
		  BEThetaRegion=1;
		}
		if(((180/PI)*el.Theta()) >= 8 && ((180/PI)*el.Theta()) < 9){
		  BEThetaRegion=2;
		}
		if(((180/PI)*el.Theta()) >= 9 && ((180/PI)*el.Theta()) < 10){
		  BEThetaRegion=3;
		} 
		if(((180/PI)*el.Theta()) >= 10 && ((180/PI)*el.Theta()) < 12.5){
		  BEThetaRegion=4;
		} 
		if(((180/PI)*el.Theta()) >= 12.5 && ((180/PI)*el.Theta()) < 15){
		  BEThetaRegion=5;
		} 
		if(((180/PI)*el.Theta()) >= 15 && ((180/PI)*el.Theta()) < 17.5){
		  BEThetaRegion=6;
		}
		if(((180/PI)*el.Theta()) >= 17.5 && ((180/PI)*el.Theta()) <= 20){
		  BEThetaRegion=7;
		} 
		if(((180/PI)*el.Theta()) < 6 || ((180/PI)*el.Theta()) > 20){
		  BEThetaRegion=8;
		}
		
		if(BEThetaRegion != 8){
		  BE1DThetaHisto[BEThetaRegion]->Fill(pred_e_beam_from_angles);
		  BEThetaSector[BEThetaRegion][secNum-1].push_back(pred_e_beam_from_angles);//For out_tree_BE
		}
		

		//The if statement below couts information about the beam energy
		if(limitCO2==1){
		  limitCO2=0;
		  cout<<endl;
		  cout<<"E from eq1= "<<(((db->GetParticle(2212)->Mass())*el.P())/((db->GetParticle(2212)->Mass())-el.P()+(el.P()*TMath::Cos(el.Theta()))))<<endl;
		  cout<<"kin_W(el, eBeam)= "<<kin_W(el, eBeam)<<endl;
                  cout<<"kin_W(el, pred_e_beam_from_angles)= "<<kin_W(el, pred_e_beam_from_angles)<<endl;
		  cout<<"el.Theta()= "<<(el.Theta()*180/PI)<<endl;
                   if(ProOelQ2==1){
		    cout<<"E from eq2 (with proton)= "<<((db->GetParticle(2212)->Mass())*(TMath::Power(TMath::Tan(el.Theta()/2),-1)*TMath::Power(TMath::Tan(pro.Theta()),-1)-1))<<endl;
		    cout<<"pro.Theta()= "<<(pro.Theta()*180/PI)<<endl;
		  }
		  cout<<endl<<endl;
	
		}//End of cout 
	      
	      }//End kin_W(el, eBeam) cut

	    }//End of ProOelQ1

	  }//End of el.P() cut

	  if (el.P() > 1.5){
       	    fill_output_vector_electron(el);
       	    sectorE.push_back(PartCalorimeter.getInt("sector", 0));                            
       	    p4_ele_vx.push_back(PART.getFloat("vx",0));
       	    p4_ele_vy.push_back(PART.getFloat("vy",0));
       	    p4_ele_vz.push_back(PART.getFloat("vz",0));
       	    fCup = EVENT.getFloat("beamCharge", 0);
       	    helicity = EVENT.getInt("helicity", 0);
       	    wHisto->Fill(kin_W(el, eBeam));
       	    q2Histo->Fill(kin_Q2(el, eBeam));
       	    wq2Histo->Fill(kin_W(el, eBeam), kin_Q2(el, eBeam));
       	    
	    wHistoV[secNum-1].push_back(kin_W(el, eBeam));
	    wHistoV[6].push_back(kin_W(el, eBeam));
	    q2HistoV[secNum-1].push_back(kin_Q2(el, eBeam));       	    
            q2HistoV[6].push_back(kin_Q2(el, eBeam));

	    //Below is corrected w and q2 histograms
            wHistoVCor[secNum-1].push_back(kin_W(elCor, eBeam));
            wHistoVCor[6].push_back(kin_W(elCor, eBeam));
            q2HistoVCor[secNum-1].push_back(kin_Q2(elCor, eBeam));
            q2HistoVCor[6].push_back(kin_Q2(elCor, eBeam));

	    //Below is the w and q2 with cut
	    if(kin_W(el, eBeam)<1.2){
	      wHistoVC[secNum-1].push_back(kin_W(el, eBeam));
	      wHistoVC[6].push_back(kin_W(el, eBeam));
	      q2HistoVC[secNum-1].push_back(kin_Q2(el, eBeam));
	      q2HistoVC[6].push_back(kin_Q2(el, eBeam));
	    }

            //Below is the w and q2 with cut (for elCor)
            if(kin_W(elCor, eBeam)<1.2){
              wHistoVCCor[secNum-1].push_back(kin_W(elCor, eBeam));
              wHistoVCCor[6].push_back(kin_W(elCor, eBeam));
              q2HistoVCCor[secNum-1].push_back(kin_Q2(elCor, eBeam));
              q2HistoVCCor[6].push_back(kin_Q2(elCor, eBeam));
            }

       	    //Below is MINE
	    p4_ele_pR[secNum-1].push_back(el.P());
       	    p4_ele_pTh[secNum-1].push_back(el.Theta()*180/PI);
       	    p4_ele_pPh[secNum-1].push_back(el.Phi()*180/PI);
       	    
       	    if(kin_W(el, eBeam)<1.2){
       	      wHistoCut2->Fill(kin_W(el, eBeam));
       	    }
            wHisto2->Fill(kin_W(el,eBeam));
	    
	    //electron momentum calculation
	    double elPcal;
	    elPcal=eBeam/(1+((2*eBeam*TMath::Sin(el.Theta()/2)*TMath::Sin(el.Theta()/2))/(db->GetParticle(2212)->Mass())));

       	    if(el.E() > 0){
	      if(kin_W(el, eBeam)<1.1){
		//p4_ele_p_Sec[secNum-1].push_back(el.P());
		el_p_Measured[secNum-1].push_back(el.P());
		el_p_Calculated[secNum-1].push_back(elPcal);
		el_p_Difference[secNum-1].push_back(el.P()-elPcal);
	      }
	    }

	    //Below is commented to remove 2D histograms
       	    // ThetaVPHisto->Fill(el.P(),el.Theta()*180/PI);
       	    
       	    // if((el.Phi()*180/PI)<0 && secNum>1){
       	    //   ThetaVPhiHisto2->Fill(360+(el.Phi()*180/PI),el.Theta()*180/PI);
       	    //   PhiVPHisto->Fill(el.P(),360+el.Phi()*180/PI);
       	    // }
       	    // else{
       	    //   ThetaVPhiHisto2->Fill((el.Phi()*180/PI),el.Theta()*180/PI);
       	    //   PhiVPHisto->Fill(el.P(),el.Phi()*180/PI);
       	    // }
       	      
       	    // ThetaVPSec[secNum-1]->Fill(el.P(),el.Theta()*180/PI);
       	    
      	    //Below is for BE Theta Ranges
       	    if(kin_Q2(el, eBeam)>=1 && kin_Q2(el, eBeam)<2){
	      //       	      wHisto_Range[0][secNum-1]->Fill(kin_W(el, eBeam));//Range:1-2 GeV
       	      //wHisto_RangeV2.push_back(kin_W(el, eBeam));
      	      TestRangeNum=0;
      	    }
      	    if(kin_Q2(el, eBeam)>=2 && kin_Q2(el, eBeam)<3){
	      //      	      wHisto_Range[1][secNum-1]->Fill(kin_W(el, eBeam));//Range:2-3 GeV
       	      //wHisto_RangeV3.push_back(kin_W(el, eBeam));
       	      TestRangeNum=1;                                                                                                                                      
      	    }
       	    if(kin_Q2(el, eBeam)>=3 && kin_Q2(el, eBeam)<4){
	      //      	      wHisto_Range[2][secNum-1]->Fill(kin_W(el, eBeam));//Range:3-4 GeV                                                                                               
      	      //wHisto_RangeV4.push_back(kin_W(el, eBeam));
       	      TestRangeNum=2;
      	    }
      	    if(kin_Q2(el, eBeam)>=4 && kin_Q2(el, eBeam)<5){
	      //      	      wHisto_Range[3][secNum-1]->Fill(kin_W(el, eBeam));//Range:4-5 GeV
      	      //wHisto_RangeV5.push_back(kin_W(el, eBeam));
       	      TestRangeNum=3;
      	    }
       	      if(kin_Q2(el, eBeam)>=5 && kin_Q2(el, eBeam)<6){
		//       	      wHisto_Range[4][secNum-1]->Fill(kin_W(el, eBeam));//Range:5-6 GeV                                                                                                    
      	      //wHisto_RangeV6.push_back(kin_W(el, eBeam));
       	      TestRangeNum=4;
      	    }
              if(kin_Q2(el, eBeam)>=6 && kin_Q2(el, eBeam)<7){
		//       	      wHisto_Range[5][secNum-1]->Fill(kin_W(el, eBeam));//Range:6-7 GeV                                                                                                    
       	      //wHisto_RangeV7.push_back(kin_W(el, eBeam));
      	      TestRangeNum=5;
      	    }
      	    if(kin_Q2(el, eBeam)>=7 && kin_Q2(el, eBeam)<8){
	      //       	      wHisto_Range[6][secNum-1]->Fill(kin_W(el, eBeam));//Range:7-8 GeV                                                                                                           
       	      //wHisto_RangeV8.push_back(kin_W(el, eBeam));
       	      TestRangeNum=6;
      	    }
      	    if(kin_Q2(el, eBeam)>=8 && kin_Q2(el, eBeam)<9){
	      //      	      wHisto_Range[7][secNum-1]->Fill(kin_W(el, eBeam));//Range:8-9 GeV
       	      //wHisto_RangeV9.push_back(kin_W(el, eBeam));
               TestRangeNum=7;
      	      }
       	    if(kin_Q2(el, eBeam)>=9 && kin_Q2(el, eBeam)<10){
	      //       	      wHisto_Range[8][secNum-1]->Fill(kin_W(el, eBeam));//Range:9-10 GeV
       	      //wHisto_RangeV10.push_back(kin_W(el, eBeam));
	      TestRangeNum=8;
      	    }
       	    if(kin_Q2(el, eBeam)>=10){
	      //       		wHisto_Range[9][secNum-1]->Fill(kin_W(el, eBeam));//Range:10+ GeV                                                                                                         
	      //wHisto_RangeV11.push_back(kin_W(el, eBeam));
	      TestRangeNum=9;
       	    }
       	    wHisto_RangeAll[TestRangeNum].push_back(kin_W(el, eBeam));
	    //      	    wHisto_Range[10][secNum-1]->Fill(kin_W(el, eBeam));//Range: All
     	    //wHisto_RangeSecs[TestRangeNum][secNum-1].push_back(kin_W(el, eBeam));
          
	  }
	  if(el.P()>0){
	    //Below is for Sampling Fraction Histogram
	    double ECE=0;
	    double ECOE=0;
	    double PCALE=0;
	    double SFt=0;
	    double SFtCor=0;

	    for(int t=0;t<PCALSize;t++){
	      float energy = PartCalorimeter.getFloat("energy",t);
	      int layer= PartCalorimeter.getInt("layer", t);
	      int pindex = PartCalorimeter.getInt("pindex", t);
	      int detector = PartCalorimeter.getInt("detector", t);

              float energy2 = PartCalorimeter.getFloat("energy",PCALSize-t);
              int layer2 = PartCalorimeter.getInt("layer",PCALSize-t);
              int pindex2 = PartCalorimeter.getInt("pindex",PCALSize-t);
              int detector2 = PartCalorimeter.getInt("detector",PCALSize-t);

              float energy3 = PartCalorimeter.getFloat("energy",TMath::Nint(PCALSize/2)-t);
              int layer3 = PartCalorimeter.getInt("layer",TMath::Nint(PCALSize/2)-t);
              int pindex3 = PartCalorimeter.getInt("pindex",TMath::Nint(PCALSize/2)-t);
              int detector3 = PartCalorimeter.getInt("detector",TMath::Nint(PCALSize/2)-t);

              float energy4 = PartCalorimeter.getFloat("energy",TMath::Nint(PCALSize/2)+t);
              int layer4 = PartCalorimeter.getInt("layer",TMath::Nint(PCALSize/2)+t);
              int pindex4 = PartCalorimeter.getInt("pindex",TMath::Nint(PCALSize/2)+t);
              int detector4 = PartCalorimeter.getInt("detector",TMath::Nint(PCALSize/2)+t);

	      if (pindex == 0 && detector == 7){
		if (layer == 1){
		  PCALE=energy;
		}
		if(layer == 4){
		  ECE=energy;
		}
		if(layer == 7){
		  ECOE=energy;
		}
	      }

              if (pindex2 == 0 && detector2 == 7){
                if (layer2 == 1){
                  PCALE=energy2;
                }
                if(layer2 == 4){
                  ECE=energy2;
		}
                if(layer2 == 7){
                  ECOE=energy2;
                }
              }

	      if (pindex3 == 0 && detector3 == 7){
                if (layer3 == 1){
                  PCALE=energy3;
                }
                if(layer3 == 4){
                  ECE=energy3;
                }
                if(layer3 == 7){
                  ECOE=energy3;
                }
              }

	      if (pindex4 == 0 && detector4 == 7){
                if (layer4 == 1){
                  PCALE=energy4;
                }
                if(layer4 == 4){
                  ECE=energy4;
                }
                if(layer4 == 7){
                  ECOE=energy4;
                }
              }

              if(t >= (PCALSize-t)){
                t=PCALSize+1;
              }

	      if((t >= (TMath::Nint(PCALSize/2)-t)) && ((PCALSize-t) <= (TMath::Nint(PCALSize/2)+t))){
		t=PCALSize+1;
	      }

	      if(PCALE>0 && ECE>0 && ECOE>0){
		t=PCALSize+1;
		//This line ends this loop once all energies have been set
	      }
	    }
	    SFt=(PCALE+ECE+ECOE)/el.P();
	    SFtCor=(PCALE+ECE+ECOE)/elecPCorrected;
	    //SamplingFractionHisto->Fill(el.P(),SFt);
	    //SFHistoSec[secNum-1]->Fill(el.P(),SFt);
	    //if(SFt<=1 && SFt>=0 && el.P()>0){
	    SFvector[secNum-1].push_back(SFt);
	    SFvector[6].push_back(SFt);
	    SFmomentumVector[secNum-1].push_back(el.P());
	    SFmomentumVector[6].push_back(el.P());
            SFvectorCor[secNum-1].push_back(SFtCor);
            SFvectorCor[6].push_back(SFtCor);
            SFmomentumVectorCor[secNum-1].push_back(elecPCorrected);
            SFmomentumVectorCor[6].push_back(elecPCorrected);
	    //}
	    //End of Sampling Fraction loop

	  }
    	}
	//} 
	out_tree.Fill();
	//Below is MINE
	//out_tree_p_Sectors.Fill();
	out_tree_electron_momentum.Fill();
	out_tree_p_Polar.Fill();
	//out_tree_wHisto_Range.Fill();
	out_tree_wHisto_Range_All.Fill();
	//out_tree_wHisto_Range_Secs.Fill();
	out_tree_SF_sec.Fill();
	out_tree_BE.Fill();
	out_tree_Pro_Theta.Fill();
	out_tree_w_and_q2.Fill();
      }
    }
    else {
      if(limitCO3!=1000){
	limitCO3=limitCO3+1;
      }
      if(limitCO3==1000){
	limitCO3=0;
	cout <<"DEFECTO"<<endl;
	cout <<" file " << qa->GetFilenum() <<endl;
      }
    }
    if (counter%10000 == 0) cout << counter <<endl;
  }
  //}
  
  // //Below are the canvases to be saved as PDFs
  // TCanvas *W_Range = new TCanvas("W_Range","w in different ranges of Q2 for all sectors",200,10,1000,980);
  // W_Range->Divide(4,3);   
  // TLegend *legendWR;
  // legendWR= new TLegend(2,2,2,2);
  // for(int n=0;n<12;n++){
  //   W_Range->cd(n+1);
  //   if(n<11){
  //     for(int i=0;i<6;i++){
  // 	if(n!=9){
  // 	  wHisto_Range[n][i]->SetTitle(Form("w_from_%d_to_%d_GeV", n + 1, n + 2));
  // 	}
  // 	if(n==9){
  // 	  wHisto_Range[n][i]->SetTitle("w_from_10_to_11+_GeV");
  // 	}
  // 	if(n==10){
  // 	  wHisto_Range[n][i]->SetTitle("w_from_all_ranges");
  // 	}
  // 	wHisto_Range[n][i]->SetLineColor(39+(2*i));
  //     }
      
  //     wHisto_Range[n][0]->Draw();
  //     wHisto_Range[n][1]->Draw("SAME");
  //     wHisto_Range[n][2]->Draw("SAME");
  //     wHisto_Range[n][3]->Draw("SAME");
  //     wHisto_Range[n][4]->Draw("SAME");
  //     wHisto_Range[n][5]->Draw("SAME");
  //   }
  //   if(n==11){
  //     for(int i=0;i<6;i++){
  // 	legendWR->AddEntry(wHisto_Range[0][i],Form("Sector %d",i+1));
  //     }
  //     legendWR->Draw();
  //   }
  // }


  // //Below is my tests for background fits                                                                                                                                                                  
  // // TF1 *Cutf1 = new TF1("Cutf1","[0]*x+[1]",0,0.8);                                                                                                                                                     
  // // //TF1 *Cutf2 = new TF1("Cutf2","([0]*x)+([1]*sin([2]*x))",0,1.2);                                                                                                                                    
  // // //TF1 *Cutf2 = new TF1("Cutf2","([0]*x)+([1]*sin([2]*x))",0,0.8);                                                                                                                                    
  // // TF1 *Cutf2 = new TF1("Cutf2","(([1]*sin([2]*x))",0,0.8);                                                                                                                                           
  // // TF1 *Cutf3 = new TF1("Cutf3","([0]*x*x)+([1]*x)+[2]",0,0.8);                                                                                                                                         
  // // //Cutf2->SetParameter(0,64.3);                                                                                                                                                                     
  // // //Cutf2->SetParameter(1,2);                                                                                                                                      
  // // //Cutf2->SetParameter(2,20*PI);                                                                                                                                                                       

  // // //Setting Colors                                                                                                                                                                                      
  // // Cutf1->SetLineColor(kBlue);                                                                                                                                                                           
  // // Cutf2->SetLineColor(kPink);     
  // // Cutf3->SetLineColor(kGreen);                                                                                                                                                                          

  // // TF1 *fitT = new TF1("fitT","[amp]*gaus(x,[Mean],[Sigma])+[p0]+[p1]*x+[p2]*x*x",0,1.2);
  // // fitT->SetParameter(1,0.9774);
  // // fitT->SetParLimits(1,0.9774*0.8,0.9774*1.2);
  // // fitT->SetParameter(2,0.08757);
  // // fitT->SetParLimits(2,0.08757*0.8,0.08757*1.2);

  // TF1 *fitT = new TF1("fitT","[p0]*exp(-0.5*((x-[p1])/[p2])*((x-[p1])/[p2]))+[p3]+[p4]*x+[p5]*x*x",0,1.2);
  // fitT->SetParameter(1,0.9774);
  // fitT->SetParLimits(1,0.9774*0.8,0.9774*1.2);
  // fitT->SetParameter(2,0.08757);
  // fitT->SetParLimits(2,0.08757*0.8,0.08757*1.2);
  


  // //Below is applying the fits for the electron scattering                                                                                                                                                 
  // wHistoCut2->Fit("fitT");
  // //gStyle->SetOptFit(1111111);
  
  // // wHistoCut2->Fit("gaus","R",0,0.8, 1.08); 
  // // wHistoCut2->Fit("Cutf1","R+");                                                                                                                                                                       
  // // wHistoCut2->Fit("Cutf2","R++");
  // // wHistoCut2->Fit("Cutf3","R+++"); 
  // // gStyle->SetOptFit(1011);                                                                                                                                                                              

  // //For the full histogram                                                                                                                                                                                 
  // wHisto2->Fit("fitT");
  // //gStyle->SetOptFit(1111111);
  
  // // wHisto2->Fit("gaus","R",0,0.8, 1.08);                                                                                                                                                                 
  // // wHisto2->Fit("Cutf1","R+");                                                                                                                                                                           
  // // wHisto2->Fit("Cutf2","R++");                                                                                                                                                                          
  // // wHisto2->Fit("Cutf3","R+++");                                                                                                                                                                         
  // // gStyle->SetOptFit(1011);       
  

  // TCanvas *BEVAngle = new TCanvas("BEVAngle","Beam Energy versus angles",200,10,700,780);
  // BEVAngle->Divide(2);
  // BEVAngle->cd(1);
  // BEVPhiHisto2->GetYaxis()->SetTitle("Beam Energy (GeV)");
  // BEVPhiHisto2->GetXaxis()->SetTitle("Phi (degrees)");
  // BEVPhiHisto2->Draw("colz"); 
  // BEVAngle->cd(2);  
  // BEVThetaHisto->GetYaxis()->SetTitle("Beam Energy (GeV)");
  // BEVThetaHisto->GetXaxis()->SetTitle("Theta (degrees)");
  // BEVThetaHisto->Draw("colz");


  // TCanvas *BESecs = new TCanvas("BESecs","Beam Energy by sector",200,10,700,780);
  // BESecs->Divide(3,2);
  // for(int i=0;i<6;i++){
  //   BESecs->cd(i+1);
  //   BE1DHisto[i]->SetTitle(Form("Beam Energy for Sector %d",i+1));
  //   BE1DHisto[i]->Fit("gaus");
  //   gStyle->SetOptFit(1011);
  //   BE1DHisto[i]->Draw();  
  // }


  // TCanvas *ThetaPhiP = new TCanvas("ThetaPhiP","2D Theta, Phi, and P Histograms",200,10,780,700);
  // ThetaPhiP->Divide(3);
  // ThetaPhiP->cd(1);
  // ThetaVPhiHisto2->GetYaxis()->SetTitle("Theta (degrees)");
  // ThetaVPhiHisto2->GetXaxis()->SetTitle("Phi (degrees)");
  // ThetaVPhiHisto2->Draw("colz");
  // ThetaPhiP->cd(2);
  // ThetaVPHisto->GetYaxis()->SetTitle("Theta (degrees)");
  // ThetaVPHisto->GetXaxis()->SetTitle("Momentum (GeV)");
  // ThetaVPHisto->Draw("colz");
  // ThetaPhiP->cd(3);
  // PhiVPHisto->GetYaxis()->SetTitle("Phi (degrees)");
  // PhiVPHisto->GetXaxis()->SetTitle("Momentum (GeV)");
  // PhiVPHisto->Draw("colz");


  // TCanvas *ThetaPSecs = new TCanvas("ThetaPSecs","Theta and Momentum Histograms by Sector",200,10,700,780);
  // ThetaPSecs->Divide(3,2);
  // for(int i=0;i<6;i++){
  //   ThetaPSecs->cd(i+1);
  //   ThetaVPSec[i]->GetYaxis()->SetTitle("Theta (degrees)");
  //   ThetaVPSec[i]->GetXaxis()->SetTitle("Momentum (GeV)");
  //   ThetaVPSec[i]->SetTitle(Form("Theta vs Momentum for Sector %d",i+1));
  //   ThetaVPSec[i]->Draw("colz");
  // }


  // TCanvas *BETheta = new TCanvas("BETheta","Beam Energy by angle Theta",200,10,700,780);
  // BETheta->Divide(4,2);
  // for(int i=0;i<8;i++){
  //   BETheta->cd(i+1);
  //   if(i<4){
  //     BE1DThetaHisto[i]->SetTitle(Form("Beam Energy for Theta %d to %d degrees",i+6,i+7));
  //   }
  //   if(i==4){
  //     BE1DThetaHisto[i]->SetTitle("Beam Energy for Theta 10 to 12.5 degrees");
  //   }
  //   if(i==5){
  //     BE1DThetaHisto[i]->SetTitle("Beam Energy for Theta 12.5 to 15 degrees");
  //   }
  //   if(i==6){
  //     BE1DThetaHisto[i]->SetTitle("Beam Energy for Theta 15 to 17.5 degrees");
  //   }
  //   if(i==7){
  //     BE1DThetaHisto[i]->SetTitle("Beam Energy for Theta 17.5 to 20 degrees");
  //   }
  //   BE1DThetaHisto[i]->Fit("gaus");
  //   gStyle->SetOptFit(1011);
  //   BE1DThetaHisto[i]->Draw();
  // }

  // // TCanvas *SFSecs = new TCanvas("SFSecs","Sampling Fraction Histograms by Sector",200,10,700,780);
  // // SFSecs->Divide(3,2);
  // // for(int i=0;i<6;i++){
  // //   SFSecs->cd(i+1);
  // //   SFHistoSec[i]->GetYaxis()->SetTitle("Sampling Fraction");
  // //   SFHistoSec[i]->GetXaxis()->SetTitle("Momentum (GeV)");
  // //   SFHistoSec[i]->SetTitle(Form("Sampling Fraction vs Momentum for Sector %d",i+1));
  // //   SFHistoSec[i]->Draw("colz");
  // // }


  // // TCanvas *SFAll = new TCanvas("SFAll","Sampling Fraction Histogram",200,10,700,780);
  // // SFAll->Divide(1,2);
  // // SFAll->cd(1);
  // // SamplingFractionHisto->GetYaxis()->SetTitle("Sampling Fraction");
  // // SamplingFractionHisto->GetXaxis()->SetTitle("Momentum (GeV)");
  // // SamplingFractionHisto->SetTitle("Sampling Fraction vs Momentum for All Sectors");
  // // SamplingFractionHisto->Draw("colz");

  // // SFAll->cd(2);
  // // out_tree_SF_sec.Draw("Sampling_Fraction_All:Momentum_for_All","","colz");

  // //Below is a test of working with vectors/trees to make 2D histograms
  // TH2F *SFH = new TH2F("SFH", "Sampling Fraction versus Momentum Histogram (Made from vectors)", 600, 0, 12,  50, 0, 0.5);
  // // TH2F *SFHsec[6];
  // // for(int i=0;i<6;i++){
  // //   SFHsec[i]= new TH2F(Form("SFHsec_%d_(vec)",i+1), Form("SF vs Momentum (Sector %d) (Made from vectors)",i+1), 600, 0, 12,  50, 0, 0.5);
  // //   SFHsec[i]->GetYaxis()->SetTitle("Sampling Fraction");
  // //   SFHsec[i]->GetXaxis()->SetTitle("Momentum (GeV)");
  // //   //SFHsec[i]->SetTitle(Form("Sampling Fraction vs Momentum for Sector %d",i+1));
  // //   //out_tree_SF_sec.Draw(Form("Sampling_Fraction_Sec_%d:Momentum_for_Sec_%d>> SFHsec_%d_(vec)",i+1,i+1,i+1),"","colz");
  // // }
  // //out_tree_SF_sec.Branch("Sampling_Fraction_2DAll",&SFH);
  // out_tree_SF_sec.Draw("Sampling_Fraction_All:Momentum_for_All>> SFH","","colz");
  // // out_tree_SF_sec.Draw("Sampling_Fraction_Sec_1:Momentum_for_Sec_1>> SFHsec_1_(vec)","","colz");
  // // out_tree_SF_sec.Draw("Sampling_Fraction_Sec_2:Momentum_for_Sec_2>> SFHsec_2_(vec)","","colz");
  // // out_tree_SF_sec.Draw("Sampling_Fraction_Sec_3:Momentum_for_Sec_3>> SFHsec_3_(vec)","","colz");
  // // out_tree_SF_sec.Draw("Sampling_Fraction_Sec_4:Momentum_for_Sec_4>> SFHsec_4_(vec)","","colz");
  // // out_tree_SF_sec.Draw("Sampling_Fraction_Sec_5:Momentum_for_Sec_5>> SFHsec_5_(vec)","","colz");
  // // out_tree_SF_sec.Draw("Sampling_Fraction_Sec_6:Momentum_for_Sec_6>> SFHsec_6_(vec)","","colz");


  // // out_tree_SF_sec.Draw("Sampling_Fraction_Sec_1:Momentum_for_Sec_1>> SFHsec[0]","","colz");
  // // out_tree_SF_sec.Draw("Sampling_Fraction_Sec_2:Momentum_for_Sec_2>> SFHsec[1]","","colz");
  // // out_tree_SF_sec.Draw("Sampling_Fraction_Sec_3:Momentum_for_Sec_3>> SFHsec[2]","","colz");
  // // out_tree_SF_sec.Draw("Sampling_Fraction_Sec_4:Momentum_for_Sec_4>> SFHsec[3]","","colz");
  // // out_tree_SF_sec.Draw("Sampling_Fraction_Sec_5:Momentum_for_Sec_5>> SFHsec[4]","","colz");
  // // out_tree_SF_sec.Draw("Sampling_Fraction_Sec_6:Momentum_for_Sec_6>> SFHsec[5]","","colz");


  out->Write();
  gBenchmark->Stop("timer");
  gBenchmark->Print("timer");
  out->mkdir("Example");
  out->cd("Example");
  wHisto->Write();
  q2Histo->Write();
  wq2Histo->Write();

  // //Below is MINE
  // out->mkdir("Theta, Phi, and P");
  // out->cd("Theta, Phi, and P");
  // ThetaVPhiHisto2->Write();
  // ThetaVPHisto->Write();
  // PhiVPHisto->Write();


  // for(int i=0;i<6;i++){
  //   ThetaVPSec[i]->Write();
  // }

  out->mkdir("w and q2 analysis");
  out->cd("w and q2 analysis");
  wHistoCut2->Write();
  wHisto2->Write();
  //W_Range->Write();
  BEVPhiHisto2->Write();
  BEVThetaHisto->Write();
  for(int i=0;i<6;i++){
    BE1DHisto[i]->Write();
    BE1DThetaHisto[i]->Write();
  }

  //out->mkdir("Sampling Fraction");
  //out->cd("Sampling Fraction");
  //out_tree_SF_sec.Draw("Sampling_Fraction_All:Momentum_for_All","","colz");
  //SamplingFractionHisto->Write();
  //SFH->Write();
  //out_tree_SF_sec.Draw("Sampling_Fraction_All:Momentum_for_All>> SFH","","colz");
  //out_tree_SF_sec->Draw("Sampling_Fraction_All:Momentum_for_All>> SFH","","colz");
  // for(int i=0;i<6;i++){
  //   SFHistoSec[i]->Write();
  //   //SFHsec[i]->Write();
  //   //out_tree_SF_sec.Draw(Form("Sampling_Fraction_Sec_%d:Momentum_for_Sec_%d>> SFHsec[%d]",i+1,i+1,i),"","colz");
  //   //out_tree_SF_sec->Draw(Form("Sampling_Fraction_Sec_%d:Momentum_for_Sec_%d>> SFHsec[%d]",i+1,i+1,i),"","colz");
  // }

  out->Close();



  // //Saving PDFs
  // W_Range->SaveAs("w_in_different_Q2_Ranges.pdf","pdf");
  // BEVAngle->SaveAs("BEVAngle.pdf","pdf");
  // BESecs->SaveAs("BESecs.pdf","pdf");
  // ThetaPhiP->SaveAs("ThetaPhiP.pdf","pdf");
  // ThetaPSecs->SaveAs("ThetaPSecs.pdf","pdf");
  // BETheta->SaveAs("BETheta.pdf","pdf");
  // //SFSecs->SaveAs("SFSecs.pdf","pdf");
  // //SFAll->SaveAs("SFAll.pdf","pdf");

  auto finish = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = finish - start;
  std::cout << "Elapsed time: " << elapsed.count()<< " s events = "<<counter<< " \n";

  // //Organizing PDFs
  // system("ls");
  // system("mkdir PDFs_from_root/");
  // system(Form("mkdir PDFs_from_root/From_test%d",OutFileNum));
  // system(Form("mv *.pdf PDFs_from_root/From_test%d",OutFileNum));
  // system("ls");

}

