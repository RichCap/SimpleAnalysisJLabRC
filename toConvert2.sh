#!/bin/csh -f
#source /group/clas12/packages/setup.csh
#module avail
#module load coatjava/6.5.3
#module load clas12root/1.4
#module load root/6.14.04
#module load groovy/2.5.6
source /group/clas12/packages/setup.csh
#module avail
module load clas12/pro
#module list

echo "Now loading file: "$1

#Rfile=$(ls -A /u/home/richcap/Code_Example/SimpleAnalysisJLabRC/$1)
#echo $Rfile

#if [[ $? != 0]]; then
#    echo "Failed"
#elif [[ $Rfile ]]; then
    #echo "File found"
    clas12root -q histobuilderR.C+ --in=/u/home/richcap/Code_Example/SimpleAnalysisJLabRC/$1

#/cache/clas12/rg-a/production/recon/fall2018/torus-1/pass1/v0/dst/train/skim4/skim4_005382.hipo
# /cache/clas12/rg-a/production/recon/fall2018/torus-1/pass1/v0/dst/recon/005038/rec_clas_005038.evio.00485-00489.hipo


#This file is to run the second script which uses the root file to make histograms
