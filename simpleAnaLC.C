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

#include <TStyle.h>
#include <TLegend.h>
#include <TF1.h>

using namespace clas12;

double PI= 3.14159265358979323846;

//Below are the elastic peak ranges used for the momentum correction calculations (from histobuilderR.C)
//Sec 1
double wMinRange1 = 0.808545;
double wMaxRange1 = 1.16771;
//Sec 2
double wMinRange2 = 0.818762;
double wMaxRange2 = 1.16717;
//Sec 3
double wMinRange3 = 0.726834;
double wMaxRange3 = 1.16683;
//Sec 4
double wMinRange4 = 0.8194;
double wMaxRange4 = 1.15807;
//Sec 5
double wMinRange5 = 0.811186;
double wMaxRange5 = 1.13611;
//Sec 6
double wMinRange6 = 0.795192;
double wMaxRange6 = 1.15301;


// int helicity;
// double fCup;
// vector<int> sectorE;
// vector<double> p4_ele_px;
// vector<double> p4_ele_py;
// vector<double> p4_ele_pz;
// vector<double> p4_ele_vx;
// vector<double> p4_ele_vy;
// vector<double> p4_ele_vz;
// vector<double> p4_ele_E;

vector<double> el_p_Measured[6];  
vector<double> el_p_Calculated[6];
vector<double> el_p_Difference[6];

vector<double> el_p_Calculated2[6];
vector<double> el_p_Difference2[6];

//Below was a test that I don't think will work as intended
//vector<double> wHistoVCal[6];

vector<double> p4_ele_pR[6];
vector<double> p4_ele_pTh[6];
vector<double> p4_ele_pPh[6];

// TH1F *wHisto = new TH1F("wHisto", "wHisto", 1000, 0, 5);
// TH1F *q2Histo = new TH1F("q2Histo", "q2Histo", 1000, 0, 10);
// TH2F *wq2Histo = new TH2F("wq2Histo", "wq2Histo",  1000, 0, 5,  1000, 0, 10);

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

TH1F *wHisto2 = new TH1F("wHisto2", "wHisto full with fit", 1000, 0, 5);
TH1F *wHistoCut2 = new TH1F("wHistoCut2", "wHistoCut2", 240, 0, 1.2);
TH2F *BEVPhiHisto2 = new TH2F("BEVPhiHisto2", "Beam Energy versus Phi Histogram (adjusted)",  390,-30, 360,  300, 0, 30);
TH2F *BEVThetaHisto = new TH2F("BEVThetaHisto", "Beam Energy versus Theta Histogram", 90, 0, 45,  300, 0, 30);

//Below was a test of MINE
vector<double> wHisto_RangeAll[10];


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



// void fill_output_vector_electron(TLorentzVector el){
//   if(el.E() > 0){
//     p4_ele_px.push_back(el.Px());
//     p4_ele_py.push_back(el.Py());
//     p4_ele_pz.push_back(el.Pz());
//     p4_ele_E.push_back(el.E());
//   }
// }



void SetLorentzVector(TLorentzVector &p4,clas12::region_part_ptr rp){
  p4.SetXYZM(rp->par()->getPx(),rp->par()->getPy(),
	     rp->par()->getPz(),p4.M());
}




void simpleAnaLC(){
  float eBeam = 10.646;
  float mProton = 0.98327;
  auto db=TDatabasePDG::Instance();

  //Below determines output file name
  int OutFileNum=19;

  TString inputFile;
  TString outputFile = Form("test%d.root",OutFileNum);


  TH1F *BE1DHisto[6];
  TH1F *BE1DThetaHisto[8];

  for(int n=0; n<6;n++){
    BE1DHisto[n] = new TH1F(Form("BE1DHisto_Sec_%d",n+1), Form("BE1DHisto_Sec_%d",n+1), 300, 0, 20);
    BE1DThetaHisto[n] = new TH1F(Form("BE1DThetaHisto_Range_%d",n+1), Form("BE1DThetaHisto_Range_%d",n+1), 300, 0, 20);
  }
  
  BE1DThetaHisto[6] = new TH1F("BE1DThetaHisto_Range_7", "BE1DThetaHisto_Range_7", 300, 0, 20);
  BE1DThetaHisto[7] = new TH1F("BE1DThetaHisto_Range_8", "BE1DThetaHisto_Range_8", 300, 0, 20);


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
 
  // TTree out_tree("out_tree","out_tree");
  // out_tree.Branch("helicity", &helicity);
  // out_tree.Branch("fCup", &fCup);
  
  // //electrons
  // out_tree.Branch("sectorE", &sectorE);
  // out_tree.Branch("p4_ele_px", &p4_ele_px);
  // out_tree.Branch("p4_ele_py", &p4_ele_py);
  // out_tree.Branch("p4_ele_pz", &p4_ele_pz);
  // out_tree.Branch("p4_ele_E", &p4_ele_E);

  // out_tree.Branch("p4_ele_vx", &p4_ele_vx);
  // out_tree.Branch("p4_ele_vy", &p4_ele_vy);
  // out_tree.Branch("p4_ele_vz", &p4_ele_vz);


  TTree out_tree_electron_momentum("out_tree_electron_momentum","out_tree_electron_momentum");
  for(int i=0;i<6;i++){
    out_tree_electron_momentum.Branch(Form("el_p_Measured_Sec_%d",i+1),&el_p_Measured[i]);
    out_tree_electron_momentum.Branch(Form("el_p_Calculated_Sec_%d",i+1),&el_p_Calculated[i]);
    out_tree_electron_momentum.Branch(Form("el_p_Difference_Sec_%d",i+1),&el_p_Difference[i]);

    out_tree_electron_momentum.Branch(Form("el_p_Calculated2_Sec_%d",i+1),&el_p_Calculated2[i]);
    out_tree_electron_momentum.Branch(Form("el_p_Difference2_Sec_%d",i+1),&el_p_Difference2[i]);
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
  TTree out_tree_SF_sec_Cor("out_tree_SF_sec_Cor","out_tree_SF_sec_Cor");
  for(int i=0;i<6;i++){
    out_tree_SF_sec.Branch(Form("Sampling_Fraction_Sec_%d",i+1),&SFvector[i]);
    out_tree_SF_sec.Branch(Form("Momentum_for_Sec_%d",i+1),&SFmomentumVector[i]);
    //Below is the corrected trees
    out_tree_SF_sec_Cor.Branch(Form("Sampling_Fraction_Sec_%d_Corrected",i+1),&SFvectorCor[i]);
    out_tree_SF_sec_Cor.Branch(Form("Momentum_for_Sec_%d_Corrected",i+1),&SFmomentumVectorCor[i]);
  }
  out_tree_SF_sec.Branch("Sampling_Fraction_All",&SFvector[6]);
  out_tree_SF_sec.Branch("Momentum_for_All",&SFmomentumVector[6]);
  out_tree_SF_sec_Cor.Branch("Sampling_Fraction_Corrected",&SFvectorCor[6]);
  out_tree_SF_sec_Cor.Branch("Momentum_Corrected_SF",&SFmomentumVectorCor[6]);

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
  TTree out_tree_w_and_q2_Cor("out_tree_w_and_q2_Cor","out_tree_w_and_q2_Cor");
  for(int i=0;i<6;i++){
    out_tree_w_and_q2.Branch(Form("wHistoV_Sec_%d",i+1),&wHistoV[i]);
    out_tree_w_and_q2.Branch(Form("q2HistoV_Sec_%d",i+1),&q2HistoV[i]);
    out_tree_w_and_q2.Branch(Form("wHistoVC_Sec_%d",i+1),&wHistoVC[i]);
    out_tree_w_and_q2.Branch(Form("q2HistoVC_Sec_%d",i+1),&q2HistoVC[i]);
    //out_tree_w_and_q2.Branch(Form("wHistoVCal_Sec_%d",i+1),&wHistoVCal[i]);

    out_tree_w_and_q2_Cor.Branch(Form("wHistoV_Sec_%d_Cor",i+1),&wHistoVCor[i]);
    out_tree_w_and_q2_Cor.Branch(Form("q2HistoV_Sec_%d_Cor",i+1),&q2HistoVCor[i]);
    out_tree_w_and_q2_Cor.Branch(Form("wHistoVC_Sec_%d_Cor",i+1),&wHistoVCCor[i]);
    out_tree_w_and_q2_Cor.Branch(Form("q2HistoVC_Sec_%d_Cor",i+1),&q2HistoVCCor[i]);
  }
  out_tree_w_and_q2.Branch("wHistoV",&wHistoV[6]);
  out_tree_w_and_q2.Branch("q2HistoV",&q2HistoV[6]);
  out_tree_w_and_q2.Branch("wHistoVC",&wHistoVC[6]);
  out_tree_w_and_q2.Branch("q2HistoVC",&q2HistoVC[6]);

  out_tree_w_and_q2_Cor.Branch("wHistoVCor",&wHistoVCor[6]);
  out_tree_w_and_q2_Cor.Branch("q2HistoVCor",&q2HistoVCor[6]);
  out_tree_w_and_q2_Cor.Branch("wHistoVCCor",&wHistoVCCor[6]);
  out_tree_w_and_q2_Cor.Branch("q2HistoVCCor",&q2HistoVCCor[6]);
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
    // p4_ele_px.clear();
    // p4_ele_py.clear();
    // p4_ele_pz.clear();
    // p4_ele_vx.clear();
    // p4_ele_vy.clear();
    // p4_ele_vz.clear();
    // p4_ele_E.clear();
    // sectorE.clear();

    for(int i=0;i<6;i++){
      el_p_Measured[i].clear();
      el_p_Calculated[i].clear();
      el_p_Difference[i].clear();

      el_p_Calculated2[i].clear();
      el_p_Difference2[i].clear();

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
      //wHistoVCal[i].clear();
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


    int secNum = PartCalorimeter.getInt("sector",0);
    int TestRangeNum=0;

    int nEvent = RConfig.getInt("event", 0);
    int runNum = RConfig.getInt("run", 0);
    if(qa->Query(runNum,nEvent)) {
      if(qa->Golden()) {
       	if (id == 11 && status < -1999 && status > -4000 && partSize > 0 && charge == -1 && PART.getFloat("vz",0) > -10 && PART.getFloat("vz",0) < 5 && startTime > 0){
       	  el.SetXYZM(PART.getFloat("px",0), PART.getFloat("py",0), PART.getFloat("pz",0), db->GetParticle(11)->Mass());
	  
	  double pred_e_beam_from_angles = ((db->GetParticle(2212)->Mass())*el.P())/((db->GetParticle(2212)->Mass())-el.P()+(el.P()*TMath::Cos(el.Theta())));
	 
	  int EPCorQ=1;//If equations from elecPCor are used, let this be equal to 1. If equations from elecPCor2 are used, let it be equal to 2.

	  //Below is for momentum correction using histotest.root
	  if((el.P()>8 && el.P()<9.8) || EPCorQ==1){
	    if(secNum==1){
	      if(kin_W(el, eBeam) >= wMinRange1 && kin_W(el, eBeam) <= wMaxRange1){
		//Sec 1 equation
		elecPchange = -0.226514*el.P()+0.0200081;
		//elecPchange = -0.0375;
	      }
	      if(kin_W(el, eBeam) <  wMinRange1 || kin_W(el, eBeam) > wMaxRange1){
		elecPchange = 0;//Do NOT change this value
	      }
	    }
	    if(secNum==2){
	      if(kin_W(el, eBeam) >= wMinRange2 && kin_W(el, eBeam) <= wMaxRange2){
		//Sec 2 equation
		elecPchange = -0.119186*el.P()+0.00747421;
		//elecPchange = -0.0375;
	      }
              if(kin_W(el, eBeam) <  wMinRange2 || kin_W(el, eBeam) > wMaxRange2){
                elecPchange = 0;//Do NOT change this value
              }
	    }
	    if(secNum==3){
	      if(kin_W(el, eBeam) >= wMinRange3 && kin_W(el, eBeam) <= wMaxRange3){
		//Sec 3 equation
		elecPchange = -1.32002*el.P()+0.14146;
		//elecPchange = 0.03;
	      }
              if(kin_W(el, eBeam) <  wMinRange3 || kin_W(el, eBeam) > wMaxRange3){
                elecPchange = 0;//Do NOT change this value 
              }
	    }
	    if(secNum==4){
	      if(kin_W(el, eBeam) >= wMinRange4 && kin_W(el, eBeam) <= wMaxRange4){
		//Sec 4 equation
		elecPchange = -0.460126*el.P()+0.0456117;
		//elecPchange = -0.0375;
	      }
	      if(kin_W(el, eBeam) <  wMinRange4 || kin_W(el, eBeam) > wMaxRange4){
                elecPchange = 0;//Do NOT change this value                                                                                                                                                
              }
	    }
	    if(secNum==5){
	      if(kin_W(el, eBeam) >= wMinRange5 && kin_W(el, eBeam) <= wMaxRange5){
		//Sec 5 equation
		elecPchange = -0.34675*el.P()+0.0336453;
		//elecPchange = -0.0125;
	      }
	      if(kin_W(el, eBeam) <  wMinRange5 || kin_W(el, eBeam) > wMaxRange5){
                elecPchange = 0;//Do NOT change this value                                                                                                                                                 
              }
	    }
	    if(secNum==6){
	      if(kin_W(el, eBeam) >= wMinRange6 && kin_W(el, eBeam) <= wMaxRange6){
		//Sec 6 equation
		elecPchange = 0.0988662*el.P()-0.012884;
		//elecPchange = -0.0125;
	      }
	      if(kin_W(el, eBeam) <  wMinRange6 || kin_W(el, eBeam) > wMaxRange6){
                elecPchange = 0;//Do NOT change this value                                                                                                                                                 
              }
	    }
	  }

	  if((el.P()<8 || el.P()>9.8) && EPCorQ==2){
	    elecPchange = 0;
	  }

	  elecPCorrected = el.P() - elecPchange;
	  elCor.SetXYZM(elecPCorrected*TMath::Sin(el.Theta())*TMath::Cos(el.Phi()), elecPCorrected*TMath::Sin(el.Theta())*TMath::Sin(el.Phi()), elecPCorrected*TMath::Cos(el.Theta()), db->GetParticle(11)->Mass());

	  double pred_e_beam_from_angles_cor1 = ((db->GetParticle(2212)->Mass())*elecPCorrected)/((db->GetParticle(2212)->Mass())-elecPCorrected+(elecPCorrected*TMath::Cos(el.Theta())));

	  ProOelQ1=1;


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
       	    // fill_output_vector_electron(el);
       	    // sectorE.push_back(PartCalorimeter.getInt("sector", 0));                            
       	    // p4_ele_vx.push_back(PART.getFloat("vx",0));
       	    // p4_ele_vy.push_back(PART.getFloat("vy",0));
       	    // p4_ele_vz.push_back(PART.getFloat("vz",0));
       	    // fCup = EVENT.getFloat("beamCharge", 0);
       	    // helicity = EVENT.getInt("helicity", 0);
       	    // wHisto->Fill(kin_W(el, eBeam));
       	    // q2Histo->Fill(kin_Q2(el, eBeam));
       	    // wq2Histo->Fill(kin_W(el, eBeam), kin_Q2(el, eBeam));
       	    
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
	    if(kin_W(el, eBeam)<1.18){
	      wHistoVC[secNum-1].push_back(kin_W(el, eBeam));
	      wHistoVC[6].push_back(kin_W(el, eBeam));
	      q2HistoVC[secNum-1].push_back(kin_Q2(el, eBeam));
	      q2HistoVC[6].push_back(kin_Q2(el, eBeam));
	    }

            //Below is the w and q2 with cut (for elCor)
            if(kin_W(elCor, eBeam)<1.18){
              wHistoVCCor[secNum-1].push_back(kin_W(elCor, eBeam));
              wHistoVCCor[6].push_back(kin_W(elCor, eBeam));
              q2HistoVCCor[secNum-1].push_back(kin_Q2(elCor, eBeam));
              q2HistoVCCor[6].push_back(kin_Q2(elCor, eBeam));
           }

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
	      if(secNum == 1){
		if(kin_W(el, eBeam) >= wMinRange1 && kin_W(el, eBeam) <= wMaxRange1){
		  el_p_Measured[secNum-1].push_back(el.P());
		  el_p_Calculated[secNum-1].push_back(elPcal);
		  el_p_Difference[secNum-1].push_back(el.P()-elPcal); 

		  el_p_Calculated2[secNum-1].push_back(elCor.P());
		  el_p_Difference2[secNum-1].push_back(el.P()-elCor.P());
		}
	      }
	      
              if(secNum == 2){
                if(kin_W(el, eBeam) >= wMinRange2 && kin_W(el, eBeam) <= wMaxRange2){
                  el_p_Measured[secNum-1].push_back(el.P());
                  el_p_Calculated[secNum-1].push_back(elPcal);
                  el_p_Difference[secNum-1].push_back(el.P()-elPcal);

                  el_p_Calculated2[secNum-1].push_back(elCor.P());
                  el_p_Difference2[secNum-1].push_back(el.P()-elCor.P());
		}
	      } 

              if(secNum == 3){
                if(kin_W(el, eBeam) >= wMinRange3 && kin_W(el, eBeam) <= wMaxRange3){
                  el_p_Measured[secNum-1].push_back(el.P());
                  el_p_Calculated[secNum-1].push_back(elPcal);
                  el_p_Difference[secNum-1].push_back(el.P()-elPcal);

                  el_p_Calculated2[secNum-1].push_back(elCor.P());
                  el_p_Difference2[secNum-1].push_back(el.P()-elCor.P());
		}
	      } 

              if(secNum == 4){
                if(kin_W(el, eBeam) >= wMinRange4 && kin_W(el, eBeam) <= wMaxRange4){
                  el_p_Measured[secNum-1].push_back(el.P());
                  el_p_Calculated[secNum-1].push_back(elPcal);
                  el_p_Difference[secNum-1].push_back(el.P()-elPcal);

                  el_p_Calculated2[secNum-1].push_back(elCor.P());
                  el_p_Difference2[secNum-1].push_back(el.P()-elCor.P());
		}
	      } 

              if(secNum == 5){
                if(kin_W(el, eBeam) >= wMinRange5 && kin_W(el, eBeam) <= wMaxRange5){
                  el_p_Measured[secNum-1].push_back(el.P());
                  el_p_Calculated[secNum-1].push_back(elPcal);
                  el_p_Difference[secNum-1].push_back(el.P()-elPcal);

                  el_p_Calculated2[secNum-1].push_back(elCor.P());
                  el_p_Difference2[secNum-1].push_back(el.P()-elCor.P());
		}
	      } 

              if(secNum == 6){
                if(kin_W(el, eBeam) >= wMinRange6 && kin_W(el, eBeam) <= wMaxRange6){
                  el_p_Measured[secNum-1].push_back(el.P());
                  el_p_Calculated[secNum-1].push_back(elPcal);
                  el_p_Difference[secNum-1].push_back(el.P()-elPcal);

                  el_p_Calculated2[secNum-1].push_back(elCor.P());
                  el_p_Difference2[secNum-1].push_back(el.P()-elCor.P());
		}
	      } 
	      // if(kin_W(el, eBeam)>0.7){
	      // 	if(secNum != 3){
	      // 	  if(kin_W(el, eBeam)<1.12){//Was 1.1 as of 9/9/2020
	      // 	    el_p_Measured[secNum-1].push_back(el.P());
	      // 	    el_p_Calculated[secNum-1].push_back(elPcal);
	      // 	    el_p_Difference[secNum-1].push_back(el.P()-elPcal);
	      // 	  }
	      // 	}
	      // 	if(secNum == 3){
	      // 	  if(kin_W(el, eBeam)<1.05){
	      // 	    el_p_Measured[secNum-1].push_back(el.P());
	      // 	    el_p_Calculated[secNum-1].push_back(elPcal);
	      // 	    el_p_Difference[secNum-1].push_back(el.P()-elPcal);
	      // 	  }
	      // 	} 
	      // }
	    }
	
	    //Below is for BE Theta Ranges
       	    if(kin_Q2(el, eBeam)>=1 && kin_Q2(el, eBeam)<2){
	      //Range:1-2 GeV
       	       TestRangeNum=0;
      	    }
      	    if(kin_Q2(el, eBeam)>=2 && kin_Q2(el, eBeam)<3){
	      //Range:2-3 GeV
       	      TestRangeNum=1;                                                                                                                                      
      	    }
       	    if(kin_Q2(el, eBeam)>=3 && kin_Q2(el, eBeam)<4){
	      //Range:3-4 GeV                                                                                              
	      TestRangeNum=2;
      	    }
      	    if(kin_Q2(el, eBeam)>=4 && kin_Q2(el, eBeam)<5){
	      //Range:4-5 GeV
      	      TestRangeNum=3;
      	    }
	    if(kin_Q2(el, eBeam)>=5 && kin_Q2(el, eBeam)<6){
	      //Range:5-6 GeV                                                                                                    
      	      TestRangeNum=4;
      	    }
	    if(kin_Q2(el, eBeam)>=6 && kin_Q2(el, eBeam)<7){
	      //Range:6-7 GeV                                                                                                    
       	      TestRangeNum=5;
      	    }
      	    if(kin_Q2(el, eBeam)>=7 && kin_Q2(el, eBeam)<8){
	      //Range:7-8 GeV                                                                                                           
       	      TestRangeNum=6;
      	    }
      	    if(kin_Q2(el, eBeam)>=8 && kin_Q2(el, eBeam)<9){
	      //Range:8-9 GeV
	      TestRangeNum=7;
	    }
       	    if(kin_Q2(el, eBeam)>=9 && kin_Q2(el, eBeam)<10){
	      //Range:9-10 GeV
       	      TestRangeNum=8;
      	    }
       	    if(kin_Q2(el, eBeam)>=10){
	      //Range:10+ GeV                                                                                                         
	      TestRangeNum=9;
       	    }
       	    wHisto_RangeAll[TestRangeNum].push_back(kin_W(el, eBeam));//Range: All
     	    
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
	    SFvector[secNum-1].push_back(SFt);
	    SFvector[6].push_back(SFt);
	    SFmomentumVector[secNum-1].push_back(el.P());
	    SFmomentumVector[6].push_back(el.P());
            SFvectorCor[secNum-1].push_back(SFtCor);
            SFvectorCor[6].push_back(SFtCor);
            SFmomentumVectorCor[secNum-1].push_back(elecPCorrected);
            SFmomentumVectorCor[6].push_back(elecPCorrected);
	    //End of Sampling Fraction loop

	  }
    	}
       
	//out_tree.Fill();
	out_tree_electron_momentum.Fill();
	out_tree_p_Polar.Fill();
	out_tree_wHisto_Range_All.Fill();
	out_tree_SF_sec.Fill();
	out_tree_SF_sec_Cor.Fill();
	out_tree_BE.Fill();
	out_tree_Pro_Theta.Fill();
	out_tree_w_and_q2.Fill();
        out_tree_w_and_q2_Cor.Fill();
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


  out->Write();
  gBenchmark->Stop("timer");
  gBenchmark->Print("timer");
  // out->mkdir("Example");
  // out->cd("Example");
  // wHisto->Write();
  // q2Histo->Write();
  // wq2Histo->Write();

  out->mkdir("w and q2 analysis");
  out->cd("w and q2 analysis");
  wHistoCut2->Write();
  wHisto2->Write();
  BEVPhiHisto2->Write();
  BEVThetaHisto->Write();
  for(int i=0;i<6;i++){
    BE1DHisto[i]->Write();
    BE1DThetaHisto[i]->Write();
  }

  out->Close();



  auto finish = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = finish - start;
  std::cout << "Elapsed time: " << elapsed.count()<< " s events = "<<counter<< " \n";

}

