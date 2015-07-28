/*********************************
 * tercpp: an open-source Translation Edit Rate (TER) scorer tool for Machine Translation.
 *
 * Copyright 2010-2013, Christophe Servan, LIUM, University of Le Mans, France
 * Copyright 2015, Christophe Servan, GETALP-LIG, University of Grenoble, France
 * Contact: christophe.servan@gmail.com
 *
 * The tercpp tool and library are free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by 
 * the Free Software Foundation, either version 3 of the licence, or
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
    p.deep = false;
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

// string scoreString (vector<float> numEdits, vector<float> numWords)
// {
//     vector<float>::iterator editsIt=numEdits.begin();
//     vector<float>::iterator wordsIt=numWords.begin();
//     if (numWords.size()!=numEdits.size())
//     {
//         cerr << "ERROR : tercpp:score, diffrent size of hyp and ref" <<endl;
//         exit(0);
//     }
// 
//     double editsCount=0.0;
//     double wordsCount=0.0;
//     while (editsIt!=numEdits.end())
//     {
//         editsCount+=(*editsIt);
//         wordsCount+=(*wordsIt);
//         editsIt++;
//         wordsIt++;
//     }
//     stringstream output;
// 
//     if ( ( wordsCount<= 0.0 ) && ( editsCount > 0.0 ) )
//     {
//         output<<  1.0 << " (" << editsCount << "/"<< wordsCount << ")"<<endl;
//     }
//     else
//         if ( wordsCount <= 0.0 )
//         {
//             output<<  0.0 << " (" << editsCount << "/"<< wordsCount << ")"<<endl;
//         }
//         else
//         {
// //       return editsCount/wordsCount;
//             output<<  editsCount/wordsCount << " (" << editsCount << "/"<< wordsCount << ")"<<endl;
//         }
//     return output.str();
// }
// 
// 
// void scoreTXTFiles(param l_params)
// {
//     ifstream fichierHyp ( l_params.hypothesisFile.c_str(), ios::in );
//     ifstream fichierRef ( l_params.referenceFile.c_str(), ios::in );
//     string line;
//     string Ref;
//     string Hyp;
// //     vector<string> vecRef;
// //     vector<string> vecHyp;
//     hashMapStringInfos hashReferences;
//     hashMapStringInfos hashHypothesis;
//     int tot_ins = 0;
//     int tot_del = 0;
//     int tot_sub = 0;
//     int tot_sft = 0;
//     int tot_wsf = 0;
//     float tot_err = 0;
//     float tot_wds = 0;
// 
//     if ( fichierHyp )
//     {
// 	int l_ids=1;
// 	stringstream l_stream;
//         while (getline ( fichierHyp, line ))
//         {
//             string l_key;
//             string line_mod;
// 	    l_stream.str("");
// 	    if (l_params.noTxtIds)
// 	    {
// 	      l_stream << l_ids;
// 	      l_key=l_stream.str();
// 	      line_mod=line;
// 	      l_ids++;
// 	    }
// 	    else
// 	    {
// 	      l_key=line.substr(line.rfind("("),line.size()-1);
// 	      line_mod=line.substr(0,line.rfind("(")-1);
// 	    }
// 
//             line_mod=tokenizePunct(line_mod);
//             if (!l_params.caseOn)
//             {
//                 line_mod=lowerCase(line_mod);
//             }
//             if (l_params.noPunct)
//             {
//                 line_mod=removePunct(line_mod);
//             }
// 
//             if (l_params.debugMode)
//             {
//                 cerr <<"DEBUG tercpp scoreTXTFiles : Hypothesis tokenized |"<< line_mod <<"|"<<endl<<"END DEBUG"<<endl;
//             }
//             vector<string> vecHyp = stringToVector(line_mod," ");
// // 	  string l_key;
// // 	  hashHypothesis.addValue(l_key,vecHyp);
// // 	  l_key=(string)vecHyp.at((int)vecHyp.size()-1);
// // 	  vecHyp.pop_back();
//             if (l_params.debugMode)
//             {
//                 cerr <<"DEBUG tercpp scoreTXTFiles : hypothesis "<< l_key << "|" << vectorToString(vecHyp)<<"|"<<endl<<"Vector Size : "<< vecHyp.size()<<endl <<"Line length : "<<(int)line_mod.length() <<endl<<"END DEBUG"<<endl;
//             }
//             hashHypothesis.addValue(l_key,vecHyp);
//         }
// //         Ref=line;
// //         getline ( fichierHyp, line );
// //         Hyp=line;
//         fichierHyp.close();  // on ferme le fichier
//     }
//     else  // sinon
//     {
//         cerr << "ERROR : scoreTXTFiles : can't open hypothesis file : " + l_params.hypothesisFile +" !" << endl;
//         exit(0);
//     }
//     if ( fichierRef )
//     {
// 	int l_ids=1;
// 	stringstream l_stream;
//         while (getline ( fichierRef, line ))
//         {
//             string l_key;
//             string line_mod;
// 	    l_stream.str("");
// 	    if (l_params.noTxtIds)
// 	    {
// 	      l_stream << l_ids;
// 	      l_key=l_stream.str();
// 	      line_mod=line;
// 	      l_ids++;
// 	    }
// 	    else
// 	    {
// 	      l_key=line.substr(line.rfind("("),line.size()-1);
// 	      line_mod=line.substr(0,line.rfind("(")-1);
// 	    }
//             if (!l_params.tercomLike)
//             {
//                 line_mod=tokenizePunct(line_mod);
//             }
//             if (!l_params.caseOn)
//             {
//                 line_mod=lowerCase(line_mod);
//             }
//             if (l_params.noPunct)
//             {
//                 if (!l_params.tercomLike)
//                 {
//                     line_mod=removePunctTercom(line_mod);
//                 }
//                 else
//                 {
//                     line_mod=removePunct(line_mod);
//                 }
//             }
//             if (l_params.debugMode)
//             {
//                 cerr <<"DEBUG tercpp scoreTXTFiles : Reference tokenized |"<< line_mod <<"|"<<endl<<"END DEBUG"<<endl;
//             }
//             vector<string> vecRef = stringToVector(line_mod," ");
// 
// // 	  l_key=vecRef.at((int)vecRef.size()-1);
// // 	  vecRef.pop_back();
// 
//             if (l_params.debugMode)
//             {
//                 cerr <<"DEBUG tercpp scoreTXTFiles : Reference  "<< l_key << "|" << vectorToString(vecRef)<<"|"<< endl<<"Vector Size : "<<vecRef.size()<< endl<<"Line length : "<<(int)line_mod.length()<<endl<<"END DEBUG"<<endl;
//             }
//             hashReferences.addValue(l_key,vecRef);
//         }
// //         Ref=line;
// //         getline ( fichierRef, line );
// //         Ref=line;
//         fichierRef.close();  // on ferme le fichier
//     }
//     else  // sinon
//     {
//         cerr << "ERROR : scoreTXTFiles : can't open references file : " + l_params.referenceFile +" !" << endl;
//         exit(0);
//     }
//     vector<float> editsResults;
//     vector<float> wordsResults;
//     vector<stringInfosHasher> setOfHypothesis=hashHypothesis.getHashMap();
//     ofstream outputSum("output.sum.log");
//     outputSum << "Hypothesis File: " + l_params.hypothesisFile + "\nReference File: " + l_params.referenceFile + "\n" + "Ave-Reference File: " <<endl;
//     char outputCharBuffer[200];
//     sprintf(outputCharBuffer, "%19s | %4s | %4s | %4s | %4s | %4s | %6s | %8s | %8s","Sent Id", "Ins", "Del", "Sub", "Shft", "WdSh", "NumEr", "NumWd", "TER");
//     outputSum<< outputCharBuffer<<endl;
//     outputSum<< "-------------------------------------------------------------------------------------"<<endl;
// //     terCalc myEvaluation;
// //      terCalc myEvaluation();
//     for (vector<stringInfosHasher>::iterator phrasesHypothesis = setOfHypothesis.begin(); phrasesHypothesis!=setOfHypothesis.end(); phrasesHypothesis++)
//     {
// //        cout << "processing " << (*phrasesHypothesis).getKey()<<endl;
//         terCalc evaluation;
// // 	terCalc.setCost(p);
// 
//         evaluation.setDebugMode ( false );
//         string l_id=(*phrasesHypothesis).getKey();
//         if (!hashReferences.trouve(l_id))
//         {
//             cerr << "ERROR tercpp : main : Evaluation : ID "+ l_id +" not found in references"<<endl;
//             exit(0);
//         }
//         else
//         {
// 	    vector<string> l_vhyp=(*phrasesHypothesis).getValue();
// 	    vector<string> l_vref=hashReferences.getValue(l_id);
//             terAlignment result=evaluation.TER (l_vhyp, l_vref);
//             editsResults.push_back(result.numEdits);
//             wordsResults.push_back(result.numWords);
//             result.scoreDetails();
//             tot_ins += result.numIns;
//             tot_del += result.numDel;
//             tot_sub += result.numSub;
//             tot_sft += result.numSft;
//             tot_wsf += result.numWsf;
//             tot_err += result.numEdits;
//             tot_wds += result.numWords;
// 
//             char outputCharBufferTmp[200];
//             sprintf(outputCharBufferTmp, "%19s | %4d | %4d | %4d | %4d | %4d | %6.1f | %8.3f | %8.3f",((l_id.substr(1,l_id.size()-2))+":1").c_str(), result.numIns, result.numDel, result.numSub, result.numSft, result.numWsf, result.numEdits, result.numWords, result.score()*100.0);
//             outputSum<< outputCharBufferTmp<<endl;
// 
//             if (l_params.debugMode)
//             {
//                 cerr <<"DEBUG tercpp : scoreTXTFiles : Evaluation "<<endl<< result.toString() <<endl<<"END DEBUG"<<endl;
//             }
//         }
//     }
//     cout << "Total TER: " << scoreString(editsResults,wordsResults);
//     char outputCharBufferTmp[200];
//     outputSum<< "-------------------------------------------------------------------------------------"<<endl;
//     sprintf(outputCharBufferTmp, "%19s | %4d | %4d | %4d | %4d | %4d | %6.1f | %8.3f | %8.3f", "TOTAL", tot_ins, tot_del, tot_sub, tot_sft, tot_wsf, tot_err, tot_wds, tot_err*100.0/tot_wds);
//     outputSum<< outputCharBufferTmp<<endl;
//     outputSum.close();
// 
// }
// 
// 
// 
// 
// 
// void scoreMultiTXTFiles(param l_params)
// {
//     ifstream fichierHyp ( l_params.hypothesisFile.c_str(), ios::in );
//     ifstream fichierRef ( l_params.referenceFile.c_str(), ios::in );
//     string line;
//     string Ref;
//     string Hyp;
// //     vector<string> vecRef;
// //     vector<string> vecHyp;
//     hashMapStringInfos hashReferences;
//     hashMapStringInfos hashHypothesis;
//     int tot_ins = 0;
//     int tot_del = 0;
//     int tot_sub = 0;
//     int tot_sft = 0;
//     int tot_wsf = 0;
//     float tot_err = 0;
//     float tot_wds = 0;
// 
//     if ( fichierHyp )
//     {
// 	int l_ids=1;
// 	stringstream l_stream;
//         while (getline ( fichierHyp, line ))
//         {
//             string l_key;
//             string line_mod;
// 	    l_stream.str("");
// 	    if (l_params.noTxtIds)
// 	    {
// 	      l_stream << l_ids;
// 	      l_key=l_stream.str();
// 	      line_mod=line;
// 	      l_ids++;
// 	    }
// 	    else
// 	    {
// 	      l_key=line.substr(line.rfind("("),line.size()-1);
// 	      line_mod=line.substr(0,line.rfind("(")-1);
// 	    }
// 
//             line_mod=tokenizePunct(line_mod);
//             if (!l_params.caseOn)
//             {
//                 line_mod=lowerCase(line_mod);
//             }
//             if (l_params.noPunct)
//             {
//                 line_mod=removePunct(line_mod);
//             }
// 
//             if (l_params.debugMode)
//             {
//                 cerr <<"DEBUG tercpp scoreTXTFiles : Hypothesis tokenized |"<< line_mod <<"|"<<endl<<"END DEBUG"<<endl;
//             }
//             vector<string> vecHyp = stringToVector(line_mod," ");
// // 	  string l_key;
// // 	  hashHypothesis.addValue(l_key,vecHyp);
// // 	  l_key=(string)vecHyp.at((int)vecHyp.size()-1);
// // 	  vecHyp.pop_back();
//             if (l_params.debugMode)
//             {
//                 cerr <<"DEBUG tercpp scoreTXTFiles : hypothesis "<< l_key << "|" << vectorToString(vecHyp)<<"|"<<endl<<"Vector Size : "<< vecHyp.size()<<endl <<"Line length : "<<(int)line_mod.length() <<endl<<"END DEBUG"<<endl;
//             }
//             hashHypothesis.addValue(l_key,vecHyp);
//         }
// //         Ref=line;
// //         getline ( fichierHyp, line );
// //         Hyp=line;
//         fichierHyp.close();  // on ferme le fichier
//     }
//     else  // sinon
//     {
//         cerr << "ERROR : scoreTXTFiles : can't open hypothesis file : " + l_params.hypothesisFile +" !" << endl;
//         exit(0);
//     }
//     if ( fichierRef )
//     {
// 	int l_ids=1;
// 	stringstream l_stream;
//         while (getline ( fichierRef, line ))
//         {
//             string l_key;
//             string line_mod;
// 	    l_stream.str("");
// 	    if (l_params.noTxtIds)
// 	    {
// 	      l_stream << l_ids;
// 	      l_key=l_stream.str();
// 	      line_mod=line;
// 	      l_ids++;
// 	    }
// 	    else
// 	    {
// 	      l_key=line.substr(line.rfind("("),line.size()-1);
// 	      line_mod=line.substr(0,line.rfind("(")-1);
// 	    }
//             if (!l_params.tercomLike)
//             {
//                 line_mod=tokenizePunct(line_mod);
//             }
//             if (!l_params.caseOn)
//             {
//                 line_mod=lowerCase(line_mod);
//             }
//             if (l_params.noPunct)
//             {
//                 if (!l_params.tercomLike)
//                 {
//                     line_mod=removePunctTercom(line_mod);
//                 }
//                 else
//                 {
//                     line_mod=removePunct(line_mod);
//                 }
//             }
//             if (l_params.debugMode)
//             {
//                 cerr <<"DEBUG tercpp scoreTXTFiles : Reference tokenized |"<< line_mod <<"|"<<endl<<"END DEBUG"<<endl;
//             }
//             vector<string> vecRef = stringToVector(line_mod," ");
// 
// // 	  l_key=vecRef.at((int)vecRef.size()-1);
// // 	  vecRef.pop_back();
// 
//             if (l_params.debugMode)
//             {
//                 cerr <<"DEBUG tercpp scoreTXTFiles : Reference  "<< l_key << "|" << vectorToString(vecRef)<<"|"<< endl<<"Vector Size : "<<vecRef.size()<< endl<<"Line length : "<<(int)line_mod.length()<<endl<<"END DEBUG"<<endl;
//             }
//             hashReferences.addValue(l_key,vecRef);
//         }
// //         Ref=line;
// //         getline ( fichierRef, line );
// //         Ref=line;
//         fichierRef.close();  // on ferme le fichier
//     }
//     else  // sinon
//     {
//         cerr << "ERROR : scoreMultiTXTFiles : can't open references file : " + l_params.referenceFile +" !" << endl;
//         exit(0);
//     }
//     vector<float> editsResults;
//     vector<float> wordsResults;
//     vector<stringInfosHasher> setOfHypothesis=hashHypothesis.getHashMap();
//     ofstream outputSum("output.sum.log");
//     outputSum << "Hypothesis File: " + l_params.hypothesisFile + "\nReference File: " + l_params.referenceFile + "\n" + "Ave-Reference File: " <<endl;
//     char outputCharBuffer[200];
//     sprintf(outputCharBuffer, "%19s | %4s | %4s | %4s | %4s | %4s | %6s | %8s | %8s","Sent Id", "Ins", "Del", "Sub", "Shft", "WdSh", "NumEr", "NumWd", "TER");
//     outputSum<< outputCharBuffer<<endl;
//     outputSum<< "-------------------------------------------------------------------------------------"<<endl;
// //     terCalc myEvaluation;
// //      terCalc myEvaluation();
//     for (vector<stringInfosHasher>::iterator phrasesHypothesis = setOfHypothesis.begin(); phrasesHypothesis!=setOfHypothesis.end(); phrasesHypothesis++)
//     {
// //        cout << "processing " << (*phrasesHypothesis).getKey()<<endl;
//         terCalc evaluation;
// //	terCalc.setCost(p);
// 
//         evaluation.setDebugMode ( false );
//         string l_id=(*phrasesHypothesis).getKey();
//         if (!hashReferences.trouve(l_id))
//         {
//             cerr << "ERROR tercpp : main : Evaluation : ID "+ l_id +" not found in references"<<endl;
//             exit(0);
//         }
//         else
//         {
// 	    vector<string> l_vhyp=(*phrasesHypothesis).getValue();
// 	    vector<string> l_vref=hashReferences.getValue(l_id);
//             terAlignment result=evaluation.TER ( l_vhyp,l_vref );
//             editsResults.push_back(result.numEdits);
//             wordsResults.push_back(result.numWords);
//             result.scoreDetails();
//             tot_ins += result.numIns;
//             tot_del += result.numDel;
//             tot_sub += result.numSub;
//             tot_sft += result.numSft;
//             tot_wsf += result.numWsf;
//             tot_err += result.numEdits;
//             tot_wds += result.numWords;
// 
//             char outputCharBufferTmp[200];
//             sprintf(outputCharBufferTmp, "%19s | %4d | %4d | %4d | %4d | %4d | %6.1f | %8.3f | %8.3f",((l_id.substr(1,l_id.size()-2))+":1").c_str(), result.numIns, result.numDel, result.numSub, result.numSft, result.numWsf, result.numEdits, result.numWords, result.score()*100.0);
//             outputSum<< outputCharBufferTmp<<endl;
// 
//             if (l_params.debugMode)
//             {
//                 cerr <<"DEBUG tercpp : scoreTXTFiles : Evaluation "<<endl<< result.toString() <<endl<<"END DEBUG"<<endl;
//             }
//         }
//     }
//     cout << "Total TER: " << scoreString(editsResults,wordsResults);
//     char outputCharBufferTmp[200];
//     outputSum<< "-------------------------------------------------------------------------------------"<<endl;
//     sprintf(outputCharBufferTmp, "%19s | %4d | %4d | %4d | %4d | %4d | %6.1f | %8.3f | %8.3f", "TOTAL", tot_ins, tot_del, tot_sub, tot_sft, tot_wsf, tot_err, tot_wds, tot_err*100.0/tot_wds);
//     outputSum<< outputCharBufferTmp<<endl;
//     outputSum.close();
// 
// }
// 
// 
// 
// 
// void scoreSGMLFiles(param l_params)
// {
// //     ifstream fichierHyp ( l_params.hypothesisFile.c_str(), ios::in );
// //     ifstream fichierRef ( l_params.referenceFile.c_str(), ios::in );
// //     SGMLDocument sgmlHyp=xmlStructure::dump_to_SGMLDocument(l_params.hypothesisFile.c_str());
//       SGMLDocument sgmlHyp;
//       SGMLDocument sgmlRef;
//       xmlStructure refStruct;
//       xmlStructure hypStruct;
//       sgmlRef=refStruct.dump_to_SGMLDocument(l_params.referenceFile);
//       sgmlHyp=hypStruct.dump_to_SGMLDocument(l_params.hypothesisFile);
// 
// //     SGMLDocument sgmlHyp(l_params.hypothesisFile);
// //     SGMLDocument sgmlRef=xmlStructure::dump_to_SGMLDocument(l_params.referenceFile.c_str());;
//     
// //     string line;
// //     string Ref;
// //     string Hyp;
// //     vector<string> vecRef;
// //     vector<string> vecHyp;
// /*    hashMapStringInfos hashReferences;
//     hashMapStringInfos hashHypothesis;
//     int tot_ins = 0;
//     int tot_del = 0;
//     int tot_sub = 0;
//     int tot_sft = 0;
//     int tot_wsf = 0;
//     float tot_err = 0;
//     float tot_wds = 0;
// 
// 
//     if ( fichierHyp )
//     {
//         while (getline ( fichierHyp, line ))
//         {
//             string l_key;
//             string line_mod;
//             l_key=line.substr(line.rfind("("),line.size()-1);
//             line_mod=line.substr(0,line.rfind("(")-1);
// 
//             line_mod=tokenizePunct(line_mod);
//             if (!l_params.caseOn)
//             {
//                 line_mod=lowerCase(line_mod);
//             }
//             if (l_params.noPunct)
//             {
//                 line_mod=removePunct(line_mod);
//             }
// 
//             if (l_params.debugMode)
//             {
//                 cerr <<"DEBUG main : Hypothesis tokenized |"<< line_mod <<"|"<<endl<<"END DEBUG"<<endl;
//             }
//             vector<string> vecHyp = stringToVector(line_mod," ");
// // 	  string l_key;
// // 	  hashHypothesis.addValue(l_key,vecHyp);
// // 	  l_key=(string)vecHyp.at((int)vecHyp.size()-1);
// // 	  vecHyp.pop_back();
//             if (l_params.debugMode)
//             {
//                 cerr <<"DEBUG main : hypothesis "<< l_key << "|" << vectorToString(vecHyp)<<"|"<<endl<<"Vector Size : "<< vecHyp.size()<<endl <<"Line length : "<<(int)line_mod.length() <<endl<<"END DEBUG"<<endl;
//             }
//             hashHypothesis.addValue(l_key,vecHyp);
//         }
// //         Ref=line;
// //         getline ( fichierHyp, line );
// //         Hyp=line;
//         fichierHyp.close();  // on ferme le fichier
//     }
//     else  // sinon
//     {
//         cerr << "ERROR : main : can't open hypothesis file : " + l_params.hypothesisFile +" !" << endl;
//         exit(0);
//     }
//     if ( fichierRef )
//     {
//         while (getline ( fichierRef, line ))
//         {
//             string l_key;
//             string line_mod;
//             l_key=line.substr(line.rfind("("),line.size()-1);
//             line_mod=line.substr(0,line.rfind("(")-1);
//             if (!l_params.tercomLike)
//             {
//                 line_mod=tokenizePunct(line_mod);
//             }
//             if (!l_params.caseOn)
//             {
//                 line_mod=lowerCase(line_mod);
//             }
//             if (l_params.noPunct)
//             {
//                 if (!l_params.tercomLike)
//                 {
//                     line_mod=removePunctTercom(line_mod);
//                 }
//                 else
//                 {
//                     line_mod=removePunct(line_mod);
//                 }
//             }
//             if (l_params.debugMode)
//             {
//                 cerr <<"DEBUG main : Reference tokenized |"<< line_mod <<"|"<<endl<<"END DEBUG"<<endl;
//             }
//             vector<string> vecRef = stringToVector(line_mod," ");
// 
// // 	  l_key=vecRef.at((int)vecRef.size()-1);
// // 	  vecRef.pop_back();
// 
//             if (l_params.debugMode)
//             {
//                 cerr <<"DEBUG main : Reference  "<< l_key << "|" << vectorToString(vecRef)<<"|"<< endl<<"Vector Size : "<<vecRef.size()<< endl<<"Line length : "<<(int)line_mod.length()<<endl<<"END DEBUG"<<endl;
//             }
//             hashReferences.addValue(l_key,vecRef);
//         }
// //         Ref=line;
// //         getline ( fichierRef, line );
// //         Ref=line;
//         fichierRef.close();  // on ferme le fichier
//     }
//     else  // sinon
//     {
//         cerr << "ERROR : main : can't open references file : " + l_params.referenceFile +" !" << endl;
//         exit(0);
//     }
//     vector<float> editsResults;
//     vector<float> wordsResults;
//     vector<stringInfosHasher> setOfHypothesis=hashHypothesis.getHashMap();
//     ofstream outputSum("output.sum.log");
//     outputSum << "Hypothesis File: " + l_params.hypothesisFile + "\nReference File: " + l_params.referenceFile + "\n" + "Ave-Reference File: " <<endl;
//     char outputCharBuffer[200];
//     sprintf(outputCharBuffer, "%19s | %4s | %4s | %4s | %4s | %4s | %6s | %8s | %8s","Sent Id", "Ins", "Del", "Sub", "Shft", "WdSh", "NumEr", "NumWd", "TER");
//     outputSum<< outputCharBuffer<<endl;
//     outputSum<< "-------------------------------------------------------------------------------------"<<endl;
// //     terCalc myEvaluation;
// //      terCalc myEvaluation();
//     for (vector<stringInfosHasher>::iterator phrasesHypothesis = setOfHypothesis.begin(); phrasesHypothesis!=setOfHypothesis.end(); phrasesHypothesis++)
//     {
//         cout << "processing " << (*phrasesHypothesis).getKey()<<endl;
//         terCalc evaluation;
// 
//         evaluation.setDebugMode ( false );
//         string l_id=(*phrasesHypothesis).getKey();
//         if (!hashReferences.trouve(l_id))
//         {
//             cerr << "ERROR tercpp : main : Evaluation : ID "+ l_id +" not found in references"<<endl;
//             exit(0);
//         }
//         else
//         {
//             terAlignment result=evaluation.TER ( (*phrasesHypothesis).getValue(),hashReferences.getValue(l_id) );
//             editsResults.push_back(result.numEdits);
//             wordsResults.push_back(result.numWords);
//             result.scoreDetails();
//             tot_ins += result.numIns;
//             tot_del += result.numDel;
//             tot_sub += result.numSub;
//             tot_sft += result.numSft;
//             tot_wsf += result.numWsf;
//             tot_err += result.numEdits;
//             tot_wds += result.numWords;
// 
//             char outputCharBufferTmp[200];
//             sprintf(outputCharBufferTmp, "%19s | %4d | %4d | %4d | %4d | %4d | %6.1f | %8.3f | %8.3f",((l_id.substr(1,l_id.size()-2))+":1").c_str(), result.numIns, result.numDel, result.numSub, result.numSft, result.numWsf, result.numEdits, result.numWords, result.score()*100.0);
//             outputSum<< outputCharBufferTmp<<endl;
// 
//             if (l_params.debugMode)
//             {
//                 cerr <<"DEBUG tercpp : main : Evaluation "<<endl<< result.toString() <<endl<<"END DEBUG"<<endl;
//             }
//         }
//     }
//     cout << "Total TER: " << scoreString(editsResults,wordsResults);
//     char outputCharBufferTmp[200];
//     outputSum<< "-------------------------------------------------------------------------------------"<<endl;
//     sprintf(outputCharBufferTmp, "%19s | %4d | %4d | %4d | %4d | %4d | %6.1f | %8.3f | %8.3f", "TOTAL", tot_ins, tot_del, tot_sub, tot_sft, tot_wsf, tot_err, tot_wds, tot_err*100.0/tot_wds);
//     outputSum<< outputCharBufferTmp<<endl;
//     outputSum.close();
// */
// }



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
