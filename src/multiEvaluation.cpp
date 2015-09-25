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
#include "multiEvaluation.h"
// #include <boost/bind/bind_template.hpp>


// #include <iostream>
// #include <boost/filesystem/fstream.hpp>
// #include <boost/archive/xml_oarchive.hpp>
// #include <boost/archive/xml_iarchive.hpp>
// #include <boost/serialization/nvp.hpp>

// helper functions to allow us to load and save sandwiches to/from xml

namespace TERCpp
{

    multiEvaluation::multiEvaluation()
    {
        evalParameters.debugMode = false;
        evalParameters.caseOn = false;
        evalParameters.noPunct = false;
        evalParameters.normalize = false;
        evalParameters.tercomLike = false;
        evalParameters.sgmlInputs = false;
        evalParameters.noTxtIds = false;
	evalParameters.delCost = 1.0;
	evalParameters.insCost = 1.0;
	evalParameters.subCost = 1.0;
	evalParameters.shiftCost = 1.0;
	evalParameters.matchCost = 0.0;
// 	referencesTxt=new multiTxtDocument();
// 	hypothesisTxt=new documentStructure();
    }

    multiEvaluation::multiEvaluation ( param p )
    {
        evalParameters.debugMode = false;
        evalParameters.caseOn = false;
        evalParameters.noPunct = false;
        evalParameters.normalize = false;
        evalParameters.tercomLike = false;
        evalParameters.sgmlInputs = false;
        evalParameters.noTxtIds = false;
	evalParameters.delCost = 1.0;
	evalParameters.insCost = 1.0;
	evalParameters.subCost = 1.0;
	evalParameters.shiftCost = 1.0;
	evalParameters.matchCost = 0.0;
	
        evalParameters = Tools::copyParam ( p );
	if (evalParameters.deep)
	{
	    m_distance = new word2vecdistance::distance(evalParameters.W2VModel);
	}
// 	referencesTxt=new multiTxtDocument();
// 	hypothesisTxt=new documentStructure();
    }

    void multiEvaluation::addReferences()
    {
        referencesTxt.loadRefFiles ( evalParameters );
    }
// void multiEvaluation::addReferences(vector< string > vecRefecrences)
// {
//     for (int i=0; i< (int) vecRefecrences.size(); i++)
//     {
//         referencesTxt.loadFile(vecRefecrences.at(i));
//     }
// }


    void multiEvaluation::setHypothesis()
    {
        multiTxtDocument l_multiTxtTmp;
        l_multiTxtTmp.loadHypFile ( evalParameters );
        hypothesisTxt = (*(l_multiTxtTmp.getDocument ( "0" )));
    }
    void multiEvaluation::setParameters ( param p )
    {
        evalParameters = Tools::copyParam ( p );
    }

    void multiEvaluation::launchTxtEvaluation()
    {
	if (evalParameters.debugMode)
	{
		    cerr <<"DEBUG tercpp : multiEvaluation::launchTxtEvaluation : before testing references and hypothesis size  "<<endl<<"END DEBUG"<<endl;
	}
	  
        if ( referencesTxt.getSize() == 0 )
        {
            cerr << "ERROR : multiEvaluation::launchTxtEvaluation : there is no references" << endl;
            exit ( 0 );
        }
        if ( hypothesisTxt.getSize() == 0 )
        {
            cerr << "ERROR : multiEvaluation::launchTxtEvaluation : there is no hypothesis" << endl;
            exit ( 0 );
        }
	if (evalParameters.debugMode)
	{
		    cerr <<"DEBUG tercpp : multiEvaluation::launchTxtEvaluation : testing references and hypothesis size  "<<endl<<" number of references : "<<  referencesTxt.getSize()<<endl; 
		    vector <string> s =referencesTxt.getListDocuments();
		    cerr << " avaiable ids : ";
		    for (vector <string>::iterator iterS=s.begin(); iterS!=s.end(); iterS++)
		    {
			cerr << " " << (*iterS);
		    }
		    cerr << endl;
		    for (vector <string>::iterator iterSBis=s.begin(); iterSBis!=s.end(); iterSBis++)
		    {
			cerr << " reference : "+(*iterSBis)+";  size : "<<  (referencesTxt.getDocument((*iterSBis)))->getSize() << endl;
		    }
		    cerr << " hypothesis size : "<<  hypothesisTxt.getSize() << endl<<"END DEBUG"<<endl;
	}
	  
        int incDocRefences = 0;
        stringstream l_stream;
        vector<float> editsResults;
        vector<float> deepEditsResults;
        vector<float> wordsResults;
        int tot_ins = 0;
        int tot_del = 0;
        int tot_sub = 0;
        int tot_sft = 0;
        int tot_wsf = 0;
        float tot_err = 0;
        float tot_deeperr = 0;
        float tot_wds = 0;
//         vector<stringInfosHasher> setOfHypothesis = hashHypothesis.getHashMap();
	ofstream outputAlignments;
        if (evalParameters.printAlignments)
	{
	    outputAlignments.open( ( evalParameters.outputFileName + ".alignments" ).c_str() );
	}
	ofstream outputSum ( ( evalParameters.outputFileName + ".output.sum.log" ).c_str() );
        outputSum << "Hypothesis File: " + evalParameters.outputFileName + "\nReference File: " + evalParameters.referenceFile + "\n" + "Ave-Reference File: " << endl;
        char outputCharBuffer[200];
        if (evalParameters.WER)
	{
	    if (evalParameters.deep)
	    {
		if (evalParameters.deeper)
		{
		    sprintf ( outputCharBuffer, "%19s | %4s | %4s | %4s | %4s | %4s | %6s | %8s | %8s | %8s | %8s", "Sent Id", "Ins", "Del", "Sub", "Shft", "WdSh", "D-NumEr", "NumEr", "AvNumWd", "D-WER", "WER");
		}
		else
		{
		    sprintf ( outputCharBuffer, "%19s | %4s | %4s | %4s | %4s | %4s | %6s | %8s | %8s | %8s | %8s", "Sent Id", "Ins", "Del", "Sub", "Shft", "WdSh", "NumEr", "D-NumEr", "AvNumWd", "WER", "D-WER");
		}
	    }
	    else
	    {
		sprintf ( outputCharBuffer, "%19s | %4s | %4s | %4s | %4s | %4s | %6s | %8s | %8s", "Sent Id", "Ins", "Del", "Sub", "Shft", "WdSh", "NumEr", "AvNumWd", "WER");
	    }
	}
	else
	{
	    if (evalParameters.deep)
	    {
		if (evalParameters.deeper)
		{
		    sprintf ( outputCharBuffer, "%19s | %4s | %4s | %4s | %4s | %4s | %6s | %8s | %8s | %8s | %8s", "Sent Id", "Ins", "Del", "Sub", "Shft", "WdSh", "D-NumEr", "NumEr", "AvNumWd", "D-WER", "TER");
		}
		else
		{
		    sprintf ( outputCharBuffer, "%19s | %4s | %4s | %4s | %4s | %4s | %6s | %8s | %8s | %8s | %8s", "Sent Id", "Ins", "Del", "Sub", "Shft", "WdSh", "NumEr", "D-NumEr", "AvNumWd", "WER", "D-TER");
		}
	    }
	    else
	    {
		sprintf ( outputCharBuffer, "%19s | %4s | %4s | %4s | %4s | %4s | %6s | %8s | %8s", "Sent Id", "Ins", "Del", "Sub", "Shft", "WdSh", "NumEr", "AvNumWd", "TER");
	    }
	}
        outputSum << outputCharBuffer << endl;
        outputSum << "--------------------------------------------------------------------------------------------------------------" << endl;
	vector <string> referenceList =referencesTxt.getListDocuments();
	for (vector <string>::iterator referenceListIter=referenceList.begin(); referenceListIter!=referenceList.end(); referenceListIter++)
	{
// 	    cerr << " " << (*referenceListIter);
            documentStructure l_reference = (*(referencesTxt.getDocument ( (*referenceListIter) )));
            evaluate ( l_reference, hypothesisTxt );
//             evaluate ( l_reference);
	}

//         for ( incDocRefences = 0; incDocRefences < referencesTxt.getSize();incDocRefences++ )
//         {
//             l_stream.str ( "" );
//             l_stream << incDocRefences;
//         }
        for ( vector<segmentStructure>::iterator segHypIt = hypothesisTxt.getSegments()->begin(); segHypIt != hypothesisTxt.getSegments()->end(); segHypIt++ )
        {
            terAlignment l_result = segHypIt->getAlignment();
	    string bestDocId = segHypIt->getBestDocId();
	    string l_id=segHypIt->getSegId();
            editsResults.push_back(l_result.numEdits);
            deepEditsResults.push_back(l_result.deepNumEdits);
            wordsResults.push_back(l_result.averageWords);
            l_result.scoreDetails();
            tot_ins += l_result.numIns;
            tot_del += l_result.numDel;
            tot_sub += l_result.numSub;
            tot_sft += l_result.numSft;
            tot_wsf += l_result.numWsf;
            tot_err += l_result.numEdits;
            tot_deeperr += l_result.deepNumEdits;
            tot_wds += l_result.averageWords;

            char outputCharBufferTmp[200];
	    if (evalParameters.deep)
	    {
		sprintf(outputCharBufferTmp, "%19s | %4d | %4d | %4d | %4d | %4d | %6.1f | %8.3f | %8.3f | %8.3f | %8.3f",(l_id+":"+bestDocId).c_str(), l_result.numIns, l_result.numDel, l_result.numSub, l_result.numSft, l_result.numWsf, l_result.numEdits, l_result.deepNumEdits, l_result.averageWords, l_result.scoreAv()*100.0, l_result.deepScoreAv()*100.0);
		outputSum<< outputCharBufferTmp<<endl;
	    }
	    else
	    {
		sprintf(outputCharBufferTmp, "%19s | %4d | %4d | %4d | %4d | %4d | %6.1f | %8.3f | %8.3f",(l_id+":"+bestDocId).c_str(), l_result.numIns, l_result.numDel, l_result.numSub, l_result.numSft, l_result.numWsf, l_result.numEdits, l_result.averageWords, l_result.scoreAv()*100.0);
		outputSum<< outputCharBufferTmp<<endl;
	    }

            if (evalParameters.debugMode)
            {
                cerr <<"DEBUG tercpp : multiEvaluation::launchTxtEvaluation : Evaluation "<<endl<< l_result.toString() <<endl<<"END DEBUG"<<endl;
            }
	    if (evalParameters.printAlignments)
	    {
		
		outputAlignments << l_id<<endl;
		if (evalParameters.WER)
		{
		    outputAlignments << "REF:" << "\t"<<vectorToString(l_result.ref," ")<<endl;
		    outputAlignments << "HYP:" << "\t"<<vectorToString(l_result.hyp," ")<<endl;
		}
		else
		{
		    outputAlignments << "REF:            " << "\t"<<vectorToString(l_result.ref," ")<<endl;
		    outputAlignments << "HYP:            " << "\t"<<vectorToString(l_result.hyp," ")<<endl;
		    outputAlignments << "HYP AFTER SHIFT:" << "\t"<<vectorToString(l_result.aftershift," ")<<endl;
		}
		outputAlignments << "ALIG:"<< "\t"<<l_result.printAlignments();//<<endl;
		if (evalParameters.printAlignmentsTrack)
		{
		    outputAlignments << endl << l_result.printAllShiftsTrack();
 		}
		else
		{
		    if (! evalParameters.WER)
		    {
			outputAlignments << " ||| " << l_result.printAllShifts();
		    }
		}
// 		outputAlignments << "ALIG:"<< "\t"<<l_result.toString()<<"\t"<<endl;
		outputAlignments << endl;
		if (evalParameters.deep)
		{  
		    outputAlignments << "Score: "<< l_result.deepScoreAv()*100 << " ( "<< l_result.deepNumEdits << " / " <<  l_result.averageWords<< " )" << endl << endl;
		}
		else
		{
		    outputAlignments << "Score: "<< l_result.scoreAv()*100 << " ( "<< l_result.numEdits << " / " <<  l_result.averageWords<< " )" << endl << endl;
		}
	    }

        }
        if (evalParameters.verbose) { cerr << "Ok!" <<endl;  }

	if (evalParameters.WER)
	{
	    if (evalParameters.deeper)
	    {
		cout << "Total WER:\t" << scoreTER ( deepEditsResults, wordsResults )<< "\t\tTotal D-WER:\t" << scoreTER ( editsResults, wordsResults );
	    }
	    else
	    {
		cout << "Total WER:\t" << scoreTER ( editsResults, wordsResults );
		if (evalParameters.deep)
		{
		    cout << "\t\tTotal D-WER:\t" << scoreTER ( deepEditsResults, wordsResults );
		}
	    }
	    cout << endl;
	}
	else
	{
	    if (evalParameters.deeper)
	    {
		cout << "Total TER:\t" << scoreTER ( deepEditsResults, wordsResults )<< "\t\tTotal D-TER:\t" << scoreTER ( editsResults, wordsResults );
	    }
	    else
	    {
		cout << "Total TER:\t" << scoreTER ( editsResults, wordsResults );
		if (evalParameters.deep)
		{
		    cout << "\t\tTotal D-TER:\t" << scoreTER ( deepEditsResults, wordsResults );
		}
	    }
	    cout << endl;
	}
	char outputCharBufferTmp[200];
        outputSum << "--------------------------------------------------------------------------------------------------------------" << endl;
	if (evalParameters.deep)
	{
	    sprintf ( outputCharBufferTmp, "%19s | %4d | %4d | %4d | %4d | %4d | %6.1f | %8.3f | %8.3f | %8.3f | %8.3f", "TOTAL", tot_ins, tot_del, tot_sub, tot_sft, tot_wsf, tot_err, tot_deeperr, tot_wds, tot_err*100.0 / tot_wds , tot_deeperr*100.0 / tot_wds );
	}
	else
	{
	    sprintf ( outputCharBufferTmp, "%19s | %4d | %4d | %4d | %4d | %4d | %6.1f | %8.3f | %8.3f", "TOTAL", tot_ins, tot_del, tot_sub, tot_sft, tot_wsf, tot_err, tot_wds, tot_err*100.0 / tot_wds );
	}
        outputSum << outputCharBufferTmp << endl;
        outputSum.close();

    }
    void multiEvaluation::evaluate ( documentStructure& docStructReference, documentStructure& docStructhypothesis )
    {
	if (docStructReference.getSize() != docStructhypothesis.getSize())
	{
		cerr << "ERROR :  multiEvaluation::evaluate size of hypothesis and size of reference differ : " << endl << "Hypothesis size: " << docStructhypothesis.getSize() << endl << "Reference size: "<< docStructReference.getSize() <<endl << "Exit Here !!!" <<endl;
		cerr << "Hyp name : " << docStructhypothesis.getFileName() <<endl;
		cerr << "Ref name : " << docStructReference.getFileName() <<endl;
		exit(1);
	}
	if (evalParameters.debugMode)
	{
		    cerr <<"DEBUG tercpp : multiEvaluation::evaluate : launching evaluate on  "<<endl<<" references size : "<<  docStructReference.getSize() << endl << " hypothesis size : "<<  docStructhypothesis.getSize() << endl<<"END DEBUG"<<endl;
	}
	if (evalParameters.debugMode)
	{
		    cerr <<"DEBUG tercpp : multiEvaluation::evaluate : testing hypothesis "<<endl;
		    cerr <<" segId : "<<  docStructhypothesis.getSegments()->at(0).getSegId() << endl<<"END DEBUG"<<endl;
	}
	int l_cpt=0;
	vector<string> l_vhyp;
	vector<string> l_vref;
// 	float l_seuil=0.6;
	
	if (evalParameters.threads == 1)
	{
	    terAlignment l_result;
	    terCalc * l_evalTER;
	    for ( vector<segmentStructure>::iterator segHypIt = docStructhypothesis.getSegments()->begin(); segHypIt != docStructhypothesis.getSegments()->end(); segHypIt++ )
	    {
    // 	  cerr << "************************************************************************************************************************************************************************************** 1 " << (docStructhypothesis.getSegments()->at(0)).toString()<<endl;
    // 	  cerr << ".";
		l_evalTER = new terCalc();
		l_evalTER->setDebugMode(evalParameters.debugMode);
		l_evalTER->setCosts(evalParameters);
		if (evalParameters.deep)
		{
		      l_evalTER->setDeep(true);
		      l_evalTER->m_deeper = evalParameters.deeper;
		      l_evalTER->m_threshold = evalParameters.threshold;
// 		      cerr << l_evalTER->m_deeper  << endl;
// 		      l_evalTER->setW2VModel(m_distance);
		}
    // 	  cerr << "************************************************************************************************************************************************************************************** 2"<<endl;
    // 	  (*segHypIt).getSegId() ;
    // 	  cerr << "************************************************************************************************************************************************************************************** 3"<<endl;
		segmentStructure * l_segRef = docStructReference.getSegment ( segHypIt->getSegId() );
    // 	  cerr << "************************************************************************************************************************************************************************************** 4"<<endl;
    // 	    exit(0);
    // 	    l_seuil=0.3;
		l_vhyp=segHypIt->getContent();
		l_vref=l_segRef->getContent();
    // 	    l_seuil=(float)l_vref.size()/100.0;
    // 	    if ((evalParameters.WER) || (((float)l_vhyp.size() / (float)l_vref.size()) < l_seuil)) 
		if (evalParameters.WER) 
		{
		    if (evalParameters.deep)
		    {
			l_result = l_evalTER->WERCalculation ( l_vhyp, l_vref, (*m_distance));
		    }
		    else
		    {
			l_result = l_evalTER->WERCalculation ( l_vhyp, l_vref);
		    }
		}
		else
		{
		    if (evalParameters.deep)
		    {
			l_result = l_evalTER->TER ( l_vhyp, l_vref, (*m_distance));
		    }
		    else
		    {
			l_result = l_evalTER->TER ( l_vhyp, l_vref);
		    }
			  
		}
    // 	    terAlignment l_result = l_evalTER->TER ( segHypIt->getContent(), l_segRef->getContent());
		delete l_evalTER;
		l_result.averageWords = l_segRef->getAverageLength();
		if (l_result.averageWords==0.0)
		{
		    cerr << "ERROR : tercpp : multiEvaluation::evaluate : averageWords is equal to zero" <<endl;
		    exit(0);
		}
		l_segRef->setAlignment ( l_result );
		if (evalParameters.debugMode)
		{
			    cerr <<"DEBUG tercpp : multiEvaluation::evaluate : testing   "<<endl<<"reference : "<<  l_segRef->getSegId() <<endl;
			    cerr << "Hypothesis : "<< vectorToString(l_vhyp)<<endl;
			    cerr << "Reference : "<<	vectorToString(l_vref)<<endl;
    // 			cerr << "BestDoc Id : "<<  l_segRef->getBestDocId() <<endl;
			    cerr << "numEdits : "<< l_result.numEdits  <<endl;
			    cerr << "deepNumEdits : "<< l_result.deepNumEdits <<endl;
			    cerr << "averageWords : "<< l_result.averageWords  <<endl;
			    cerr << "score : "<<  l_result.scoreAv()  <<endl;
			    cerr << "deep score : "<<  l_result.deepScoreAv()  <<endl;
			    cerr << "terAlignment.toString :" << l_result.toString()<<endl;
			    cerr << "END DEBUG"<<endl<<endl;
		}
		if ((segHypIt->getAlignment().numWords == 0) && (segHypIt->getAlignment().numEdits == 0 ))
		{
		    segHypIt->setAlignment ( l_result );
		    segHypIt->setBestDocId ( docStructReference.getDocId() );
		}
		else if ( l_result.scoreAv() < segHypIt->getAlignment().scoreAv() )
		{
		    segHypIt->setAlignment ( l_result );
		    segHypIt->setBestDocId ( docStructReference.getDocId() );
		}
		if (evalParameters.debugMode)
		{
			    cerr << "DEBUG tercpp : multiEvaluation::evaluate : testing   "<<endl;
			    cerr << "hypothesis : "<<  segHypIt->getSegId() <<endl;
			    cerr << "hypothesis score : "<<  segHypIt->getAlignment().scoreAv() <<endl;
    // 			cerr << "BestDoc Id : "<<  segHypIt->getBestDocId() <<endl;
			    cerr << "new score : "<<  l_result.scoreAv()  <<endl;
			    cerr << "new deep score : "<< l_result.deepScoreAv() <<endl;
			    cerr << "new BestDoc Id : "<< docStructReference.getDocId()  <<endl;
			    cerr << "Best Alignements : "<< l_result.printAlignments() <<endl;
			    cerr << "END DEBUG"<<endl<<endl;
		}
		if (evalParameters.count_verbose) 
		{
		    l_cpt++; cerr << l_cpt<< endl; 
		}
		else
		if (evalParameters.verbose) 
		{ 
		    cerr << ".";   
		}
	    }
	}
	else
	{
    	    vector<terAlignment> lv_results(evalParameters.threads);
// 	    terCalc * l_evalTER;
	    int lv_cpt=0;
	    boost::thread_group * tgroup;
	    tgroup = new boost::thread_group();
	    for ( vector<segmentStructure>::iterator segHypIt = docStructhypothesis.getSegments()->begin(); segHypIt != docStructhypothesis.getSegments()->end(); segHypIt++ )
	    {
      // 	  cerr << "************************************************************************************************************************************************************************************** 1 " << (docStructhypothesis.getSegments()->at(0)).toString()<<endl;
		segmentStructure * l_segRef = docStructReference.getSegment ( segHypIt->getSegId() );
		segmentStructure * l_segHyp = docStructhypothesis.getSegment ( segHypIt->getSegId() );
		boost::thread *t;

// 		subEvaluate(l_segRef , l_segHyp);
		t = new boost::thread(boost::bind(&multiEvaluation::subEvaluate,this,l_segRef , l_segHyp));
		tgroup->add_thread(t);
		lv_cpt++;
		if (lv_cpt % evalParameters.threads == 0)
		{
// 		    cerr << "JOIN...";
		    tgroup->join_all();
// 		    cerr << "FINISHED!" << endl;
		    lv_cpt = 0;
// 		    delete(tgroup);
// 		    tgroup = new boost::thread_group();

		}
      // 	  cerr << ".";
		if (evalParameters.count_verbose) 
		{
		    l_cpt++; cerr << l_cpt<< endl; 
		}
		else
		if (evalParameters.verbose) 
		{ 
		    cerr << ".";   
		}
	    }
// 	    cerr << "JOIN...";
	    tgroup->join_all();
// 	    cerr << "FINISHED!" << endl;
	}
// 	cerr << "FINI" <<endl;
	if (evalParameters.debugMode)
	{
		    cerr <<"DEBUG tercpp : multiEvaluation::evaluate :    "<<endl<<"End of function"<<endl<<"END DEBUG"<<endl;
	}
// 	for (incSegHypothesis=0; incSegHypothesis< getSize();incSegHypothesis++)
// 	{
// 	  docStructhypothesis->getSegments()
// 	}
    }
    void multiEvaluation::subEvaluate(segmentStructure* segStructReference, segmentStructure* segStructHypothesis)
    {
	vector<string> l_vhyp;
	vector<string> l_vref;
	terAlignment l_result;
	terCalc * l_evalTER = new terCalc();
	l_evalTER->setDebugMode(evalParameters.debugMode);
	l_evalTER->setCosts(evalParameters);
	if (evalParameters.deep)
	{
	      l_evalTER->setDeep(true);
// 	      l_evalTER->setW2VModel(m_distance);
	      l_evalTER->m_deeper = evalParameters.deeper;
	      l_evalTER->m_threshold = evalParameters.threshold;
// 	      cerr << l_evalTER->m_deeper  << endl;
	      
	}
	l_vhyp=segStructHypothesis->getContent();
	l_vref=segStructReference->getContent();
	if (evalParameters.WER) 
	{
	    if (evalParameters.deep)
	    {
		l_result = l_evalTER->WERCalculation ( l_vhyp, l_vref, (*m_distance));
	    }
	    else
	    {
		l_result = l_evalTER->WERCalculation ( l_vhyp, l_vref);
	    }
	}
	else
	{
	    if (evalParameters.deep)
	    {
		l_result = l_evalTER->TER ( l_vhyp, l_vref, (*m_distance));
	    }
	    else
	    {
		l_result = l_evalTER->TER ( l_vhyp, l_vref);
	    }
		  
	}
	delete l_evalTER;
	l_result.averageWords = segStructReference->getAverageLength();
	if (l_result.averageWords==0.0)
	{
	    cerr << "ERROR : tercpp : multiEvaluation::evaluate : averageWords is equal to zero" <<endl;
	    exit(0);
	}
	segStructReference->setAlignment ( l_result );
	if (evalParameters.debugMode)
	{
	      cerr <<"DEBUG tercpp : multiEvaluation::evaluate : testing   "<<endl<<"reference : "<<  segStructReference->getSegId() <<endl;
	      cerr << "Hypothesis : "<< vectorToString(l_vhyp)<<endl;
	      cerr << "Reference : "<<	vectorToString(l_vref)<<endl;
	      cerr << "numEdits : "<< l_result.numEdits  <<endl;
	      cerr << "deepNumEdits : "<< l_result.deepNumEdits <<endl;
	      cerr << "averageWords : "<< l_result.averageWords  <<endl;
	      cerr << "score : "<<  l_result.scoreAv()  <<endl;
	      cerr << "deep score : "<<  l_result.deepScoreAv()  <<endl;
	      cerr << "terAlignment.toString :" << l_result.toString()<<endl;
	      cerr << "END DEBUG"<<endl<<endl;
	}
	if ((segStructHypothesis->getAlignment().numWords == 0) && (segStructHypothesis->getAlignment().numEdits == 0 ))
	{
	    segStructHypothesis->setAlignment ( l_result );
	    segStructHypothesis->setBestDocId ( segStructReference->getBestDocId());
	}
	else if ( l_result.scoreAv() < segStructHypothesis->getAlignment().scoreAv() )
	{
	    segStructHypothesis->setAlignment ( l_result );
	    segStructHypothesis->setBestDocId ( segStructReference->getBestDocId());
	}
	if (evalParameters.debugMode)
	{
	      cerr << "DEBUG tercpp : multiEvaluation::evaluate : testing   "<<endl;
	      cerr << "hypothesis : "<<  segStructHypothesis->getSegId() <<endl;
	      cerr << "hypothesis score : "<<  segStructHypothesis->getAlignment().scoreAv() <<endl;
// 			cerr << "BestDoc Id : "<<  segStructHypothesis->getBestDocId() <<endl;
	      cerr << "new score : "<<  l_result.scoreAv()  <<endl;
	      cerr << "new deep score : "<< l_result.deepScoreAv() <<endl;
	      cerr << "new BestDoc Id : "<< segStructReference->getBestDocId() <<endl;
	      cerr << "Best Alignements : "<< l_result.printAlignments() <<endl;
	      cerr << "END DEBUG"<<endl<<endl;
	}
    }

    string multiEvaluation::scoreTER ( vector<float> numEdits, vector<float> numWords )
    {
        vector<float>::iterator editsIt = numEdits.begin();
        vector<float>::iterator wordsIt = numWords.begin();
        if ( numWords.size() != numEdits.size() )
        {
            cerr << "ERROR : tercpp:score, diffrent size of hyp and ref" << endl;
            exit ( 0 );
        }

        double editsCount = 0.0;
        double wordsCount = 0.0;
        while ( editsIt != numEdits.end() )
        {
            editsCount += ( *editsIt );
            wordsCount += ( *wordsIt );
            editsIt++;
            wordsIt++;
        }
        stringstream output;

        if ( ( wordsCount <= 0.0 ) && ( editsCount > 0.0 ) )
        {
            output <<  1.0 << "\t(" << editsCount << "/" << wordsCount << ")";
        }
        else
            if ( wordsCount <= 0.0 )
            {
                output <<  0.0 << "\t(" << editsCount << "/" << wordsCount << ")" ;
            }
            else
            {
//       return editsCount/wordsCount;
                output <<  editsCount / wordsCount << "\t(" << editsCount << "/" << wordsCount << ")";
            }
        return output.str();
    }

    void multiEvaluation::launchSGMLEvaluation()
    {
	if (evalParameters.debugMode)
	{
		    cerr <<"DEBUG tercpp : multiEvaluation::launchSGMLEvaluation : before testing references and hypothesis size  "<<endl<<"END DEBUG"<<endl;
	}
	  
        if ( referencesSGML.getSize() == 0 )
        {
            cerr << "ERROR : multiEvaluation::launchSGMLEvaluation : there is no references" << endl;
            exit ( 0 );
        }
        if ( hypothesisSGML.getSize() == 0 )
        {
            cerr << "ERROR : multiEvaluation::launchSGMLEvaluation : there is no hypothesis" << endl;
            exit ( 0 );
        }
	if (evalParameters.debugMode)
	{
		    cerr <<"DEBUG tercpp : multiEvaluation::launchSGMLEvaluation : testing references and hypothesis size  "<<endl<<" references size : "<<  referencesSGML.getSize() << endl << " hypothesis size : "<<  hypothesisSGML.getSize() << endl<<"END DEBUG"<<endl;
	}
	  
        int incDocRefences = 0;
        int incDocHypothesis = 0;
        stringstream l_stream;
        vector<float> editsResults;
        vector<float> deepEditsResults;
        vector<float> wordsResults;
        int tot_ins = 0;
        int tot_del = 0;
        int tot_sub = 0;
        int tot_sft = 0;
        int tot_wsf = 0;
        float tot_err = 0;
        float tot_deeperr = 0;
        float tot_wds = 0;
//         vector<stringInfosHasher> setOfHypothesis = hashHypothesis.getHashMap();
        ofstream outputSum ( ( evalParameters.outputFileName + ".output.sum.log" ).c_str() );
        outputSum << "Hypothesis File: " + evalParameters.outputFileName + "\nReference File: " + evalParameters.referenceFile + "\n" + "Ave-Reference File: " << endl;
        char outputCharBuffer[200];
	ofstream outputAlignments;
	if (evalParameters.printAlignments)
	{
	    outputAlignments.open( ( evalParameters.outputFileName + ".alignments" ).c_str() );
	}
        if (evalParameters.WER)
	{
	    if (evalParameters.deep)
	    {
		if (evalParameters.deeper)
		{
		    sprintf ( outputCharBuffer, "%19s | %4s | %4s | %4s | %4s | %4s | %6s | %8s | %8s | %8s | %8s", "Sent Id", "Ins", "Del", "Sub", "Shft", "WdSh", "D-NumEr", "NumEr", "AvNumWd", "D-WER", "WER");
		}
		else
		{
		    sprintf ( outputCharBuffer, "%19s | %4s | %4s | %4s | %4s | %4s | %6s | %8s | %8s | %8s | %8s", "Sent Id", "Ins", "Del", "Sub", "Shft", "WdSh", "NumEr", "D-NumEr", "AvNumWd", "WER", "D-WER");
		}
	    }
	    else
	    {
		sprintf ( outputCharBuffer, "%19s | %4s | %4s | %4s | %4s | %4s | %6s | %8s | %8s", "Sent Id", "Ins", "Del", "Sub", "Shft", "WdSh", "NumEr", "AvNumWd", "WER");
	    }
	}
	else
	{
	    if (evalParameters.deep)
	    {
		if (evalParameters.deeper)
		{
		    sprintf ( outputCharBuffer, "%19s | %4s | %4s | %4s | %4s | %4s | %6s | %8s | %8s | %8s | %8s", "Sent Id", "Ins", "Del", "Sub", "Shft", "WdSh", "D-NumEr", "NumEr", "AvNumWd", "D-WER", "TER");
		}
		else
		{
		    sprintf ( outputCharBuffer, "%19s | %4s | %4s | %4s | %4s | %4s | %6s | %8s | %8s | %8s | %8s", "Sent Id", "Ins", "Del", "Sub", "Shft", "WdSh", "NumEr", "D-NumEr", "AvNumWd", "WER", "D-TER");
		}
	    }
	    else
	    {
		sprintf ( outputCharBuffer, "%19s | %4s | %4s | %4s | %4s | %4s | %6s | %8s | %8s", "Sent Id", "Ins", "Del", "Sub", "Shft", "WdSh", "NumEr", "AvNumWd", "TER");
	    }
	}
        outputSum << outputCharBuffer << endl;
        outputSum << "--------------------------------------------------------------------------------------------------------------" << endl;
        for ( incDocHypothesis = 0; incDocHypothesis < hypothesisSGML.getSize();incDocHypothesis++ )
        {
	    documentStructure l_hypothesis = (*(hypothesisSGML.getDocument ( incDocHypothesis)));
	    vector<documentStructure> l_reference = (referencesSGML.getDocumentCollection ( l_hypothesis.getDocId() ));
	    
	    for ( incDocRefences = 0; incDocRefences < (int)l_reference.size();incDocRefences++ )
	    {
		
// 		cerr << "******************************************************** DEB : "<< incDocRefences <<endl;
		documentStructure l_l_references = l_reference.at(incDocRefences);
		evaluate ( l_l_references, l_hypothesis );
// 		cerr << "******************************************************** FIN : "<< incDocRefences <<endl;
	    }
	    for ( vector<segmentStructure>::iterator segHypIt = l_hypothesis.getSegments()->begin(); segHypIt != l_hypothesis.getSegments()->end(); segHypIt++ )
	    {
		terAlignment l_result = segHypIt->getAlignment();
		string bestDocId = segHypIt->getBestDocId();
		string l_id=segHypIt->getSegId();
		editsResults.push_back(l_result.numEdits);
		deepEditsResults.push_back(l_result.deepNumEdits);
		wordsResults.push_back(l_result.averageWords);
		l_result.scoreDetails();
		tot_ins += l_result.numIns;
		tot_del += l_result.numDel;
		tot_sub += l_result.numSub;
		tot_sft += l_result.numSft;
		tot_wsf += l_result.numWsf;
		tot_err += l_result.numEdits;
		tot_deeperr += l_result.deepNumEdits;
		tot_wds += l_result.averageWords;

		char outputCharBufferTmp[200];
		if (evalParameters.deep)
		{
		    sprintf(outputCharBufferTmp, "%19s | %4d | %4d | %4d | %4d | %4d | %6.1f | %8.3f | %8.3f | %8.3f | %8.3f",(l_id+":"+bestDocId).c_str(), l_result.numIns, l_result.numDel, l_result.numSub, l_result.numSft, l_result.numWsf, l_result.numEdits, l_result.deepNumEdits, l_result.averageWords, l_result.scoreAv()*100.0, l_result.deepScoreAv()*100.0);
		    outputSum<< outputCharBufferTmp<<endl;
		}
		else
		{
		    sprintf(outputCharBufferTmp, "%19s | %4d | %4d | %4d | %4d | %4d | %6.1f | %8.3f | %8.3f",(l_id+":"+bestDocId).c_str(), l_result.numIns, l_result.numDel, l_result.numSub, l_result.numSft, l_result.numWsf, l_result.numEdits, l_result.averageWords, l_result.scoreAv()*100.0);
		    outputSum<< outputCharBufferTmp<<endl;
		}
		if (evalParameters.printAlignments)
		{
		    
		    outputAlignments << l_id<<endl;
		    if (evalParameters.WER)
		    {
			outputAlignments << "REF:" << "\t"<<vectorToString(l_result.ref," ")<<endl;
			outputAlignments << "HYP:"<< "\t"<<vectorToString(l_result.hyp," ")<<endl;
		    }
		    else
		    {
			outputAlignments << "REF:" << "\t"<<vectorToString(l_result.ref," ")<<endl;
			outputAlignments << "HYP:"<< "\t"<<vectorToString(l_result.hyp," ")<<endl;
			outputAlignments << "HYP AFTER SHIFT:"<< "\t"<<vectorToString(l_result.aftershift," ")<<endl;
		    }
		    outputAlignments << "ALIG:"<< "\t"<<l_result.printAlignments();//<<endl;
		    if (evalParameters.printAlignmentsTrack)
		    {
			outputAlignments << endl << l_result.printAllShiftsTrack();
		    }
		    else
		    {
			if (! evalParameters.WER)
			{
			    outputAlignments << " ||| " << l_result.printAllShifts();
			}
		    }
    // 		outputAlignments << "ALIG:"<< "\t"<<l_result.toString()<<"\t"<<endl;
		    outputAlignments << endl;
		    if (evalParameters.deep)
		    {  
			outputAlignments << "Score: "<< l_result.deepScoreAv()*100 << " ( "<< l_result.deepNumEdits << " / " <<  l_result.averageWords<< " )" << endl << endl;
		    }
		    else
		    {
			outputAlignments << "Score: "<< l_result.scoreAv()*100 << " ( "<< l_result.numEdits << " / " <<  l_result.averageWords<< " )" << endl << endl;
		    }
// 		    if (evalParameters.printAlignmentsTrack)
// 		    {
// 			outputAlignments << endl << l_result.printAllShiftsTrack();
// 		    }
// 		    else
// 		    {
// 			if (! evalParameters.WER)
// 			{
// 			    outputAlignments << " ||| " << l_result.printAllShifts();
// 			}
// 		    }
//     // 		outputAlignments << "ALIG:"<< "\t"<<l_result.toString()<<"\t"<<endl;
// 		    outputAlignments << endl;
		}
// 		if (evalParameters.debugMode)
// 		{
// 		    cerr <<"DEBUG tercpp : multiEvaluation::launchSGMLEvaluation : Evaluation "<<endl<< l_result.toString() <<endl<<"END DEBUG"<<endl;
// 		}

	    }
	    
	}
//         for ( incDocRefences = 0; incDocRefences < referencesSGML.getSize();incDocRefences++ )
//         {
//             l_stream.str ( "" );
//             l_stream << incDocRefences;
//             documentStructure l_reference = (*(referencesSGML.getDocument ( l_stream.str() )));
//             evaluate ( l_reference, hypothesisSGML );
//         }
        if (evalParameters.verbose) { cerr << "Ok!" <<endl;  }

	if (evalParameters.WER)
	{
	    if (evalParameters.deeper)
	    {
		cout << "Total WER:\t" << scoreTER ( deepEditsResults, wordsResults )<< "\t\tTotal D-WER:\t" << scoreTER ( editsResults, wordsResults );
	    }
	    else
	    {
		cout << "Total WER:\t" << scoreTER ( editsResults, wordsResults );
		if (evalParameters.deep)
		{
		    cout << "\t\tTotal D-WER:\t" << scoreTER ( deepEditsResults, wordsResults );
		}
	    }
	    cout << endl;
	}
	else
	{
	    if (evalParameters.deeper)
	    {
		cout << "Total TER:\t" << scoreTER ( deepEditsResults, wordsResults )<< "\t\tTotal D-TER:\t" << scoreTER ( editsResults, wordsResults );
	    }
	    else
	    {
		cout << "Total TER:\t" << scoreTER ( editsResults, wordsResults );
		if (evalParameters.deep)
		{
		    cout << "\t\tTotal D-TER:\t" << scoreTER ( deepEditsResults, wordsResults );
		}
	    }
	    cout << endl;
	}
	char outputCharBufferTmp[200];
        outputSum << "--------------------------------------------------------------------------------------------------------------" << endl;
	if (evalParameters.deep)
	{
	    sprintf ( outputCharBufferTmp, "%19s | %4d | %4d | %4d | %4d | %4d | %6.1f | %8.3f | %8.3f | %8.3f | %8.3f", "TOTAL", tot_ins, tot_del, tot_sub, tot_sft, tot_wsf, tot_err, tot_deeperr, tot_wds, tot_err*100.0 / tot_wds , tot_deeperr*100.0 / tot_wds );
	}
	else
	{
	    sprintf ( outputCharBufferTmp, "%19s | %4d | %4d | %4d | %4d | %4d | %6.1f | %8.3f | %8.3f", "TOTAL", tot_ins, tot_del, tot_sub, tot_sft, tot_wsf, tot_err, tot_wds, tot_err*100.0 / tot_wds );
	}
        outputSum << outputCharBufferTmp << endl;
        outputSum.close();


    }
void multiEvaluation::addSGMLReferences()
{
      if (evalParameters.debugMode)
      {
	  cerr <<"DEBUG tercpp : multiEvaluation::addSGMLReferences "<<endl<<"END DEBUG"<<endl;
      }
      xmlStructure refStruct;
      referencesSGML.setParams(evalParameters);
      refStruct.xmlParams=copyParam(evalParameters);
      referencesSGML=refStruct.dump_to_SGMLDocument(evalParameters.referenceFile);
      referencesSGML.setAverageLength();
      if (evalParameters.debugMode)
      {
	  cerr <<"DEBUG tercpp : multiEvaluation::addSGMLReferences Reference Content :"<<endl;
	  cerr << referencesSGML.toString()<<endl<<"END DEBUG"<<endl;
      }
      
}
void multiEvaluation::setSGMLHypothesis()
{
      if (evalParameters.debugMode)
      {
	  cerr <<"DEBUG tercpp : multiEvaluation::setSGMLHypothesis "<<endl<<"END DEBUG"<<endl;
      }
//       SGMLDocument sgmlHyp;
      xmlStructure hypStruct;
      hypStruct.xmlParams=copyParam(evalParameters);
      hypStruct.xmlParams.tercomLike=false;
      hypothesisSGML.setParams(evalParameters);
      hypothesisSGML=hypStruct.dump_to_SGMLDocument(evalParameters.hypothesisFile);
      if (evalParameters.debugMode)
      {
	  cerr <<"DEBUG tercpp : multiEvaluation::setSGMLHypothesis Hypothesis Content :"<<endl;
	  cerr << hypothesisSGML.toString()<<endl<<"END DEBUG"<<endl;
	  cerr <<"DEBUG tercpp : LOAD FINISHED "<<endl<<"END DEBUG"<<endl;
      }
//       hypothesisSGML=(*(sgmlHyp.getFirstDocument()));
}

}
