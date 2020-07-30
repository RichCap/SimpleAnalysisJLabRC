# SimpleAnalysisJLabRC
This is the notes on my code for analyzing the hipo file data


Most Recent Changes:
* Attempted to make simpleAnaLC.C run faster during its calculation of SF
* Added more to histobuilderR.C
* simpleAnaLC.C is even less responsible for 2D histograms (moving this responsibility over to histobuilderR.C)

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
