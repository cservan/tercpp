/*********************************
 * tercpp: an open-source Translation Edit Rate (TER) scorer tool for Machine Translation.
 *
 * Copyright 2010-2013, Christophe Servan, LIUM, University of Le Mans, France
 * Copyright 2015, Christophe Servan, GETALP-LIG, University of Grenoble, France
 * Contact: christophe.servan@gmail.com
 *
 * The tercpp tool and library are free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by 
 * the Free Software Foundation, either version 2.1 of the licence, or
 * (at your option) any later version.
 *
 * This program and library are distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * **********************************/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <fstream>
#include <iostream>
// #include <stdio.h>
// #include <stdlib.h>
#include <string.h>
#include <vector>
#include "tools.h"
// #include "tercalc.h"
// #include "hashMapStringInfos.h"
#include "hashMapStringInfos.h"
// #include "../src/hashmap.h"
// #include "../src/stringhasher.h"
// #include <boost/algorithm/string.hpp>
// #include <boost/xpressive/xpressive.hpp>
#include "tercalc.h"
#include "stringInfosHasher.h"
#include "sgmlDocument.h"
#include "multiTxtDocument.h"
#include "xmlStructure.h"
#include "tools.h"
#include "multiEvaluation.h"
#include "distance.h"



using namespace TERCpp;
using namespace std;
using namespace HashMapSpace;
// using namespace boost::xpressive;


void usage();

void readCommandLineArguments ( unsigned int argc, char *argv[] , param & p)
{
    p.debugMode=false;
    p.debugLevel=-1;
    p.caseOn=false;
    p.noPunct=false;
    p.normalize=false;
    p.tercomLike=false;
    p.sgmlInputs=false;
    p.noTxtIds=false;
    p.verbose=false;
    p.count_verbose=false;
    p.printAlignments = false;
    p.printAlignmentsTrack = false;
    p.WER=false;
    p.referenceFile="";     // path to the resources
    p.hypothesisFile="";     // path to the configuration files
    p.outputFileExtension="";
    p.outputFileName="";
    p.matchCost=0.0;
    p.insCost=1.0;
    p.delCost=1.0;
    p.subCost=1.0;
    p.shiftCost=1.0;
    p.W2VModel = "";
    p.W2VModelcpp = "";
    p.BivecModelcpp = "";
    p.deep = false;
    p.deepcpp = false;
    p.deeper = false;
    p.threshold = 0.9;
    p.threads = 1;

    string s ( "" );
    string infos ("");
    for ( unsigned int i = 1; i < argc; i++ )
    {
	s= argv[i];
        // Command line string
        if (i < argc-1)
        {
            infos = argv[i+1];
        }

        // Configuration files
        if ( s.compare ( "-r" ) == 0 )
        {
            p.referenceFile = infos;
        }
        else if ( s.compare ( "--debugLevel" ) == 0 )
        {
            p.debugLevel = atoi(infos.c_str());
        }
        else if ( s.compare ( "-h" ) == 0 )
        {
            p.hypothesisFile = infos;
        }
        else if ( s.compare ( "-o" ) == 0 )
        {
            p.outputFileExtension = infos;
        }
        else if ( s.compare ( "-M" ) == 0 )
        {
            p.matchCost = atof(infos.c_str());
        }
        else if ( s.compare ( "-D" ) == 0 )
        {
            p.delCost = atof(infos.c_str());
        }
        else if ( s.compare ( "-S" ) == 0 )
        {
            p.subCost = atof(infos.c_str());
        }
        else if ( s.compare ( "-I" ) == 0 )
        {
            p.insCost = atof(infos.c_str());
        }
        else if ( s.compare ( "-T" ) == 0 )
        {
            p.shiftCost = atof(infos.c_str());
        }
        else if ( s.compare ( "--threads" ) == 0 )
        {
            p.threads = atoi(infos.c_str());
        }
        else if ( s.compare ( "-w2v" ) == 0 )
        {
            p.W2VModel = infos;
	    p.deep = true;
	    p.deepcpp = false;
        }
        else if ( s.compare ( "-w2vcpp" ) == 0 )
        {
            p.W2VModelcpp = infos;
	    p.deep = false;
	    p.deepcpp = true;
        }
        else if ( s.compare ( "-biveccpp" ) == 0 )
        {
            p.BivecModelcpp = infos;
// 	    p.deepcpp = true;
        }
        else if ( s.compare ( "--deeper" ) == 0 )
        {
	    p.deeper = true;
        }
        else if ( s.compare ( "--threshold" ) == 0 )
        {
	    p.threshold = atof(infos.c_str());
        }
        else if ( s.compare ( "-n" ) == 0 )
        {
            p.outputFileName = infos;
        }
        else if ( s.compare ( "--debugMode" ) == 0 )
        {
            p.debugMode = true;
        }
        else if ( s.compare ( "--countVerbose" ) == 0 )
        {
            p.count_verbose = true;
	    p.verbose = true;
        }
        else if ( s.compare ( "--noTxtIds" ) == 0 )
        {
            p.noTxtIds = true;
        }
        else if ( s.compare ( "--sgml" ) == 0 )
        {
            p.sgmlInputs = true;
        }
        else if ( s.compare ( "--verbose" ) == 0 )
        {
            p.verbose = true;
        }
        else if ( s.compare ( "-s" ) == 0 )
        {
            p.caseOn = true;
        }
        else if ( s.compare ( "-P" ) == 0 )
        {
            p.noPunct = true;
        }
        else if ( s.compare ( "-N" ) == 0 )
        {
            p.normalize = true;
        }
        else if ( s.compare ( "--tercom" ) == 0 )
        {
            p.tercomLike = true;
        }
        else if ( s.compare ( "--printAlignments" ) == 0 )
        {
            p.printAlignments = true;
        }        
        else if ( s.compare ( "--printTrackAlignments" ) == 0 )
        {
            p.printAlignments = true;
            p.printAlignmentsTrack = true;
        }        
        else if ( s.compare ( "--WER" ) == 0 )
        {
            p.WER=true;
        }
        else if ( s.compare ( "--help" ) == 0 )
        {
            usage();
        }
        else if ( s.find ( "-" ) == 0 )
        {
	    cerr << "ERROR : tercpp: unknown option :" << s <<endl;
            usage();
        }
    }
    if (p.outputFileName.length() == 0)
    {
       p.outputFileName=p.hypothesisFile;
    }
}

double score(vector<float> numEdits, vector<float> numWords)
{
    vector<float>::iterator editsIt=numEdits.begin();
    vector<float>::iterator wordsIt=numWords.begin();
    if (numWords.size()!=numEdits.size())
    {
        cerr << "ERROR : tercpp:score, diffrent size of hyp and ref" <<endl;
        exit(0);
    }

    double editsCount=0.0;
    double wordsCount=0.0;
    while (editsIt!=numEdits.end())
    {
        editsCount+=(*editsIt);
        wordsCount+=(*wordsIt);
        editsIt++;
        wordsIt++;
    }
    if ( ( wordsCount<= 0.0 ) && ( editsCount > 0.0 ) )
    {
        return 1.0;
    }
    else
        if ( wordsCount <= 0.0 )
        {
            return 0.0;
        }
        else
        {
            return editsCount/wordsCount;
        }
}

void usage()
{
// 	cerr<<"tercpp [-N] [-s] [-P] -r ref -h hyp [-a alter_ref] [-b beam_width] [-S trans_span_prefix] [-o out_format -n out_pefix] [-d max_shift_distance] [-M match_cost] [-D delete_cost] [-B substitute_cost] [-I insert_cost] [-T shift_cost]"<<endl;
    cerr<<"Usage : "<<endl<<"\ttercpp [--tercom] [--debugMode] [--debugLevel 0-3] [--noTxtIds] [--printAlignments] [--printTrackAlignments]  [-s] [-P] [--WER] -r ref[,ref2,ref3...] -h hyp [-D delete_cost] [-B substitute_cost] [-I insert_cost] [-T shift_cost] :\n\n\t\t --tercom \t: to use the tercom standart normalization\n\t\t --noTxtIds \t: you don't have to add ids at the end of sentences \n\t\t --debugMode \t: print debug messages \n\t\t --debugLevel \t: print debug messages regarding the level\n\t\t -s \t\t: to be case sensitive \n\t\t -P \t\t: do not take account of punctuation\n\t\t --printAlignments/printTrackAlignments\t: print alignments information into a separate file\n\t\t --WER\t\t: do a simple WER calculation\n\t\t --help \t: print this help message.\n "<<endl;
    exit(0);
// 	System.exit(1);

}



int main ( int argc, char *argv[] )
{
    param myParams;
    readCommandLineArguments ( argc,argv, myParams);
//     cerr << Tools::printParams(myParams) << endl; 
//     exit(0);
    if (((int)myParams.hypothesisFile.size()==0) || ((int)myParams.referenceFile.size()==0))
    {
        cerr << "ERROR : main : hypothesis file or reference file is not set !" << endl;
        usage();
//         return 0;
    }
    multiEvaluation l_eval(myParams);
//     cerr << Tools::printParams(myParams) << endl;
    if (!myParams.sgmlInputs)
    {
//       if ((int)myParams.referenceFile.find(",")>-1)
//       {
      if (myParams.verbose) { cerr << "Loading references..."; }
	l_eval.addReferences();
      if (myParams.verbose) { cerr << "Ok!" << endl << "Loading hypothesis..."; }
	l_eval.setHypothesis();
      if (myParams.verbose) { cerr << "Ok!" << endl << "Evaluation..."; }
	l_eval.launchTxtEvaluation();
//       if (myParams.verbose) { cerr << "Ok!" <<endl;  }
//       }
//       else
//       {
// 	scoreTXTFiles(myParams);
//       }
    }
    else
    {
      if (myParams.verbose) { cerr << "Loading references..."; }
	l_eval.addSGMLReferences();
      if (myParams.verbose) { cerr << "Ok!" << endl << "Loading hypothesis..."; }
	l_eval.setSGMLHypothesis();
      if (myParams.verbose) { cerr << "Ok!" << endl << "Evaluation..."; }
	l_eval.launchSGMLEvaluation();
//       if (myParams.verbose) { cerr << "Ok!" << endl; }
//       scoreSGMLFiles(myParams);
    }

//     cerr << myParams.hypothesisFile << endl <<myParams.referenceFile <<endl;
//     exit(0);
//     boost::split ( vecRef,Ref,boost::is_any_of ( " " ) );
//     boost::split ( vecHyp,Hyp,boost::is_any_of ( " " ) );
//     for ( vector<string>::iterator it=vecRef.begin(); it != vecRef.end(); it++ )
//     {
//         cerr << ( *it ) << "\t";
//     }
//     cerr << endl;
//     for ( vector<string>::iterator it2=vecHyp.begin(); it2 != vecHyp.end(); it2++ )
//     {
//         cerr << ( *it2 ) << "\t";
//     }
//     cerr << endl;
//     terCalc test;
//     test.setDebugMode ( false );
//     size_t * hashRef=test.hashVec ( vecRef );
//     size_t * hashHyp=test.hashVec ( vecHyp );
// 	cout << "R�sultat WER : " << test.WERCalculation(vecRef,vecHyp)<< endl;
//     terAlignment result=test.TER ( vecHyp,vecRef );
//     cout << "resultat : "<<endl<<result.toString() <<endl;
    return EXIT_SUCCESS;
}
