# SimpleAnalysisJLabRC


These are the notes on my code for analyzing the hipo file data


Most Recent Changes:
* elecPCal3 has not been updated along with elecPCal2 (ignore the projections of this histogram until further developments are made)
* q2 and w vs q2 histograms have been added to histobuilderR.C 
* wHisto is now also fit sector-by-sector (additional fits and histograms have been added to do this)
* Fits have been added to elecPCor to find peak change based off gaussian fit
* Range for defining elastic events for momentum correction calculations has been updated to be based on the new wHisto fits
* Momentum corrections now are only applied to elastic events rather than to all events
* Improvements made to the wHisto fits (for elastic events) and to the elecPCor fits
* Removed some unused data from test#.root (specifically the data in "out_tree" and the example histograms that histobuilderR.C is now responsible for making)

Instructions on running:

1) Use ./toConvert.sh to have simpleAnaLC.C begin processing data.
   Note: toConvert.sh uses a hipo file that has been pre-determined. See the code for the hipo files name and location.

2) Wait for simpleAnaLC.C to finish creating a new root file.

3) These root files are named test#.root where the number # is set within simpleAnaLC.C (can be changed).

4) Once you have a root file, run ./toConvert2.sh test#.root to have histobuilderR.C open the file.
   Note 1: toConvert2.sh must be followed by the name of the root file being used when entering it into the command line.
   Note 2: toConvert2.sh and histobuilderR.C are designed to be used in a set directory and with a set test#.root file.
   If they are used in a different directory than they were made in, or if the # changes, these scripts will need to be updated manually.

5) Running ./toConvert2.sh test#.root will create and save a new file based on the data recorded by running toConvert.sh and simpleAnaLC.C

6) Manually update simpleAnaLC.C after running histobuilderR.C in order to provide it with the ranges for each sector's elastic events and the equations for momentum correction (as described below)
   Note 1: The fit equations for each sector and the wHisto ranges are displayed in the commandline after running toConvert2.sh (copy these equations into simpleAnaLC.C)
   Note 2: There are (currently) 2 sets of equations for the momentum corrections. They are based on histograms elecPCor and elecPCor2 (see below)
   	Note 2.1.1: Equations from elecPCor2 are the fit lines predicting the change in momentum as a fuction of the measured momentum
 	Note 2.1.2: These equations most accurately describe momentum change values ranging from 8 to 9.8 GeV (range is dictated by statistical necessity)
	Note 2.2.1: Equations from elecPCor are the peak positions of the calculated change in momentum (determined via a gaussian fit)
	Note 2.2.1: These equation are not functions of the measured momentum but should be accurate for all ranges of momentum
	Note 2.3.1: simpleAnaLC.C can only use one of these equations at a time. Select one of these equations to use and then copy and paste it into the program (location is the same for either eq)
       *Note 2.3.2: If equations from elecPCor are used, set the variable EPCorQ=1. If equations from elecPCor2 are used, set the variable EPCorQ=2 (Important for defining statistical ranges)
   Note 3: wMinRange and wMaxRange range are defined at the beginning of simpleAnaLC.C (should remain fairly constant as long as the same original data file is not changes in toConvert.sh)
   Note 4: This step is required as histobuilderR.C is not able to edit the existing data recorded by simpleAnaLC.C
   Note 5: This step is a Work in Progress and thus may be heavily subject to changes

7) Repeat steps 1-6 (as needed).
