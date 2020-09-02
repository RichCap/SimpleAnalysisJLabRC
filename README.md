# SimpleAnalysisJLabRC
This is the notes on my code for analyzing the hipo file data


Most Recent Changes:
* histobuilderR.C plots the projections' peaks with error bars onto the change in momentum versus momomentum graphs (Improved binning)
* histobuilderR.C now also gives line fits for the above projections' peaks 
* Initial test of adding the above equations into simpleAnaLC.C has begun
* Developed histobuilderR.C's ability to make better fits
* Changed range of elastic scattering cut in simpleAnaLC.C to w<1.1 instead of w<1.2 (peak positions described above are better for analysis in this range)
* elecPCal3 has not been updated along with elecPCal2 (ignore the projections of this histogram until further developments are made)

Instructions on running:
1) Use ./toConvert.sh to have simpleAnaLC.C begin processing data.
Note: toConvert.sh uses a hipo file that has been pre-determined. See the code for the hipo files name and location.
2) Wait for simpleAnaLC.C to finish creating a new root file.
3) These root files are named test#.root where the number # is set within simpleAnaLC.C (can be changed).
4) Once you have a root file, run ./toConvert2.sh test#.root to have histobuilderR.C open the file.
Note 1: toConvert2.sh must be followed by the name of the root file being used when entering it into the command line.
Note 2: toConvert2.sh and histobuilderR.C are designed to be used in a set directory and with a set test#.root file.
If they are used in a different directory than they were made in, or if the # changes, these scripts will need to be updated manually.
5) Running ./toConvert2.sh test#.root will create and save new PDFs based on the data recorded by running toConvert.sh and simpleAnaLC.C
(./toConvert2.sh and histobuilderR.C are currently works in progress)
6) Manually update simpleAnaLC.C after running histobuilderR.C in order to provide it with the equations for momentum correction (as described above).
Note 1: The fit equations for each sector are displayed in the commandline after running toConvert2.sh (input these equations into simpleAnaLC.C)
Note 2: The equations are (currently) the fit lines predicting the change in momentum as a fuction of the measured momentum
Note 3: The equations describe momentum change values ranging from 8 to 9.8 GeV (range is dictated by statistical necessity)
Note 4: This step is required as histobuilderR.C is not able to edit the existing data recorded by simpleAnaLC.C 
Note 5: This step is a Work in Progress and thus may be heavily subject to changes
