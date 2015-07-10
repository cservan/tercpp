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
//
// C++ Implementation: tercalc
//
// Description:
//
//
// Author:  <>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "tercalc.h"
using namespace std;
using namespace Tools;
namespace TERCpp
{

    terCalc::terCalc()
    {
        TAILLE_PERMUT_MAX = 20;
        NBR_PERMUT_MAX = 100;
        infinite = 99999.0;
        shift_cost = 1.0;
        insert_cost = 1.0;
        delete_cost = 1.0;
        substitute_cost = 1.0;
        match_cost = 0.0;
        NBR_SEGS_EVALUATED = 0;
        NBR_PERMUTS_CONSID = 0;
        NBR_BS_APPELS = 0;
        TAILLE_BEAM = 10;
        DIST_MAX_PERMUT = 25;
        PRINT_DEBUG = false;
	hypSpans.clear();
	refSpans.clear();
	CALL_TER_ALIGN=0;
	CALL_CALC_PERMUT=0;
	CALL_FIND_BSHIFT=0;
	MAX_LENGTH_SENTENCE=10;
	S = new vector < vector < float > >(MAX_LENGTH_SENTENCE, std::vector<float>(MAX_LENGTH_SENTENCE,0.0));
	D = new vector < vector < float > >(MAX_LENGTH_SENTENCE, std::vector<float>(MAX_LENGTH_SENTENCE,0.0));
	P = new vector < vector < char > >(MAX_LENGTH_SENTENCE, std::vector<char>(MAX_LENGTH_SENTENCE,' '));
	m_deep = false;
	m_distance = NULL;
    }

    terCalc::~terCalc()
    {
	delete(S);
	delete(D);
	delete(P);
// 	delete(m_distance);
    }

    void terCalc::setCosts(param l_p)
    {
	match_cost = l_p.matchCost;
	insert_cost = l_p.insCost;
	delete_cost = l_p.delCost;
	substitute_cost = l_p.subCost;
	shift_cost = l_p.shiftCost;
    }

    terAlignment terCalc::WERCalculation ( vector< string >& hyp , vector< string >& ref )
    {
      
      return minimizeDistanceEdition ( hyp, ref, hypSpans );
      
    }

    terAlignment terCalc::TER ( vector< int >& hyp, vector< int >& ref )
    {
        stringstream s;
        s.str ( "" );
        string stringRef ( "" );
        string stringHyp ( "" );
        for ( vector<int>::iterator l_it = ref.begin(); l_it != ref.end(); l_it++ )
        {
            if ( l_it == ref.begin() )
            {
                s << ( *l_it );
            }
            else
            {
                s << " " << ( *l_it );
            }
        }
        stringRef = s.str();
        s.str ( "" );
        for ( vector<int>::iterator l_itHyp = hyp.begin(); l_itHyp != hyp.end(); l_itHyp++ )
        {
            if ( l_itHyp == hyp.begin() )
            {
                s << ( *l_itHyp );
            }
            else
            {
                s << " " << ( *l_itHyp );
            }
        }
        stringHyp = s.str();
        s.str ( "" );
	vector<string> l_vref=stringToVector ( stringRef , " " );
	vector<string> l_vhyp=stringToVector ( stringHyp , " " );
        return TER ( l_vhyp , l_vref);
    }


    hashMapInfos terCalc::createConcordMots ( vector< string >& hyp, vector< string >& ref )
    {
        hashMap tempHash;
        hashMapInfos retour;
        for ( int i = 0; i < ( int ) hyp.size(); i++ )
        {
            tempHash.addHasher ( hyp.at ( i ), "" );
        }
        bool cor[ref.size() ];
        for ( int i = 0; i < ( int ) ref.size(); i++ )
        {
            if ( tempHash.trouve ( ( string ) ref.at ( i ) ) )
            {
                cor[i] = true;
            }
            else
            {
                cor[i] = false;
            }
        }
        for ( int start = 0; start < ( int ) ref.size(); start++ )
        {
            if ( cor[start] )
            {
                for ( int end = start; ( ( end < ( int ) ref.size() ) && ( end - start <= TAILLE_PERMUT_MAX ) && ( cor[end] ) );end++ )
                {
                    vector<string> ajouter = subVector ( ref, start, end + 1 );
                    string ajouterString = vectorToString ( ajouter );
                    vector<int> values = retour.getValue ( ajouterString );
                    values.push_back ( start );
                    if ( values.size() > 1 )
                    {
                        retour.setValue ( ajouterString, values );
                    }
                    else
                    {
                        retour.addValue ( ajouterString, values );
                    }
                }
            }
        }
//         if (PRINT_DEBUG)
// 	{
//             cerr << "BEGIN DEBUG : terCalc::createConcordMots :" << endl << retour.toString() << endl << "END DEBUG" << endl;
// 	}
        return retour;
    }

    bool terCalc::trouverIntersection ( vecInt& refSpan, vecInt& hypSpan )
    {
        if ( ( refSpan.at ( 1 ) >= hypSpan.at ( 0 ) ) && ( refSpan.at ( 0 ) <= hypSpan.at ( 1 ) ) )
        {
            return true;
        }
        return false;
    }


    terAlignment terCalc::minimizeDistanceEdition ( vector< string >& hyp, vector< string >& ref, vector< vecInt >& curHypSpans )
    {
        float current_best = infinite;
        float last_best = infinite;
        int first_good = 0;
        int current_first_good = 0;
        int last_good = -1;
        int cur_last_good = 0;
        int last_peak = 0;
        int cur_last_peak = 0;
        int i=0;
	int j=0;
	int ref_size=0 ; 
	ref_size=( int ) ref.size();
	int hyp_size=0;
	hyp_size=( int ) hyp.size();
        float cost, icost, dcost;
        float score;
	delete(S);
	delete(D);
	delete(P);
	S = new vector < vector < float > >(ref_size+1, std::vector<float>(hyp_size+1,-1.0));
	D = new vector < vector < float > >(ref_size+1, std::vector<float>(hyp_size+1,-1.0));
	P = new vector < vector < char > >(ref_size+1, std::vector<char>(hyp_size+1,'0'));


	
        NBR_BS_APPELS++;
// 	cerr << "Appels : " << NBR_BS_APPELS << endl;
	
//         for ( i = 0; i <= ref_size; i++ )
//         {
//             for ( j = 0; j <= hyp_size; j++ )
//             {
//                 S->at(i).at(j) = -1.0;
//                 P->at(i).at(j) = '0';
//             }
//         }
        S->at(0).at(0) = 0.0;
        D->at(0).at(0) = 0.0;
// 	cerr << "REF "<< vectorToString(ref) << endl << "HYP "<< vectorToString(hyp) <<endl;
        for ( j = 0; j <= hyp_size; j++ )
        {
            last_best = current_best;
            current_best = infinite;
            first_good = current_first_good;
            current_first_good = -1;
            last_good = cur_last_good;
            cur_last_good = -1;
            last_peak = cur_last_peak;
            cur_last_peak = 0;
	    float l_distance = 0.0;
	    float l_erreur = 1.0;
// 	    cerr << m_deep << endl;
// 	    cerr << "=========================" << endl;
	    
            for ( i = first_good; i <= ref_size; i++ )
            {
                if ( i > last_good )
                {
                    break;
                }
                if ( S->at(i).at(j) < 0 )
                {
                    continue;
                }
                score = S->at(i).at(j);
                if ( ( j < hyp_size ) && ( score > last_best + TAILLE_BEAM ) )
                {
                    continue;
                }
                if ( current_first_good == -1 )
                {
                    current_first_good = i ;
                }
                if ( ( i < ref_size ) && ( j < hyp_size ) )
                {
		    if (m_deep)
		    {
			l_distance = m_distance->getDistance(ref.at ( i ), hyp.at ( j ));
			l_erreur = 1-l_distance; 
// 			cerr << ref.at ( i ) << "\t" << hyp.at ( j ) << "\t" << l_distance << "\t" << l_erreur <<endl;
		    }
//                     if ( ( int ) refSpans.size() ==  0 || ( int ) hypSpans.size() ==  0 || trouverIntersection ( refSpans.at ( i ), curHypSpans.at ( j ) ) || (m_deep && l_erreur < 0.5) )
                    if ( ( int ) refSpans.size() ==  0 || ( int ) hypSpans.size() ==  0 || trouverIntersection ( refSpans.at ( i ), curHypSpans.at ( j ) ) )
                    {
                        if ( ( int ) ( ref.at ( i ).compare ( hyp.at ( j ) ) ) == 0 )
                        {
                            cost = match_cost + score;
                            if ( ( S->at(i+1).at(j+1) == -1 ) || ( cost < S->at(i+1).at(j+1) ) )
                            {
				S->at(i+1).at(j+1) = cost;
				D->at(i+1).at(j+1) = cost;
				P->at(i+1).at(j+1) = 'A';
                            }
                            if ( cost < current_best )
                            {
                                current_best = cost;
                            }
                            if ( current_best == cost )
                            {
                                cur_last_peak = i + 1;
                            }
                        }
                        else
                        {
                            cost = substitute_cost + score;
                            if ( ( S->at(i+1).at(j+1) < 0 ) || ( cost < S->at(i+1).at(j+1) ) )
                            {
                                S->at(i+1).at(j+1) = cost;
				if (m_deep)
				{
				    cost = l_erreur + score;
				}
                                D->at(i+1).at(j+1) = cost;
				P->at(i+1).at(j+1) = 'S';
                                if ( cost < current_best )
                                {
                                    current_best = cost;
                                }
                                if ( current_best == cost )
                                {
                                    cur_last_peak = i + 1 ;
                                }
                            }
                        }
                    }
                }
                cur_last_good = i + 1;
                if ( j < hyp_size )
                {
                    icost = score + insert_cost;
                    if ( ( S->at(i).at(j+1) < 0 ) || ( S->at(i).at(j+1) > icost ) )
                    {
                        S->at(i).at(j+1) = icost;
                        D->at(i).at(j+1) = icost;
			P->at(i).at(j+1) = 'I';
                        if ( ( cur_last_peak <  i ) && ( current_best ==  icost ) )
                        {
                            cur_last_peak = i;
                        }
                    }
                }
                if ( i < ref_size )
                {
                    dcost =  score + delete_cost;
                    if ( ( S->at(i+1).at(j) < 0.0 ) || ( S->at(i+1).at(j) > dcost ) )
                    {
                        S->at(i+1).at(j) = dcost;
                        D->at(i+1).at(j) = dcost;
                        P->at(i+1).at(j) = 'D';
                        if ( i >= last_good )
                        {
                            last_good = i + 1 ;
                        }
                    }
                }
            }
        }


        int tracelength = 0;
        i = ref.size();
        j = hyp.size();
        while ( ( i > 0 ) || ( j > 0 ) )
        {
            tracelength++;
            if ( P->at(i).at(j) == 'A' )
            {
                i--;
                j--;
            }
            else
                if ( P->at(i).at(j) == 'S' )
                {
                    i--;
                    j--;
                }
                else
                    if ( P->at(i).at(j) == 'D' )
                    {
                        i--;
                    }
                    else
                        if ( P->at(i).at(j) == 'I' )
                        {
                            j--;
                        }
                        else
                        {
                            cerr << "ERROR : terCalc::minimizeDistanceEdition : Invalid path : " << P->at(i).at(j) << endl;
                            exit ( -1 );
                        }
        }
        vector<char> path ( tracelength );
        i = ref.size();
        j = hyp.size();
        while ( ( i > 0 ) || ( j > 0 ) )
        {
            path[--tracelength] = P->at(i).at(j);
            if ( P->at(i).at(j) == 'A' )
            {
                i--;
                j--;
            }
            else
                if ( P->at(i).at(j) == 'S' )
                {
                    i--;
                    j--;
                }
                else
                    if ( P->at(i).at(j) == 'D' )
                    {
                        i--;
                    }
                    else
                        if ( P->at(i).at(j) == 'I' )
                        {
                            j--;
                        }
        }
        terAlignment to_return;
        to_return.numWords = ref_size;
        to_return.alignment = path;
        to_return.numEdits = S->at(ref_size).at(hyp_size);
        to_return.deepNumEdits = D->at(ref_size).at(hyp_size);
	to_return.hyp = hyp;
	to_return.ref = ref;
	to_return.averageWords = ref_size;
        if ( PRINT_DEBUG )
        {
            cerr << "BEGIN DEBUG : terCalc::minimizeDistanceEdition : to_return :" << endl << to_return.toString() << endl ; 
	    cerr << "P:" <<endl;
	    i = 0;
	    j = 0;
	    while ( ( i < (int)ref.size() ) )
	    {
		cerr<< i<< "\t";
		j = 0;
		while ( ( j < (int)hyp.size() ))
		{
		    cerr << P->at(i).at(j)<< "\t";
		    j++;
		}
		i++;
		cerr << endl;
	    }
	    cerr << "S:" <<endl;
	    i = 0;
	    j = 0;
	    while ( ( i < (int)ref.size() ) )
	    {
		cerr<< i<< "\t";
		j = 0;
		while ( ( j < (int)hyp.size() ))
		{
		    cerr << S->at(i).at(j)<< "\t";
		    j++;
		}
		i++;
		cerr << endl;
	    }
	    cerr << "END DEBUG" << endl;
	    
        }
        return to_return;

    }
    void terCalc::minimizeDistanceEdition ( vector< string >& hyp, vector< string >& ref, vector< vecInt >& curHypSpans, terAlignment* to_return )
    {
        float current_best = infinite;
        float last_best = infinite;
        int first_good = 0;
        int current_first_good = 0;
        int last_good = -1;
        int cur_last_good = 0;
        int last_peak = 0;
        int cur_last_peak = 0;
        int i=0;
	int j=0;
	int ref_size=0 ; 
	ref_size=( int ) ref.size();
	int hyp_size=0;
	hyp_size=( int ) hyp.size();
        float cost, icost, dcost;
        float score;
	delete(S);
	delete(D);
	delete(P);
	S = new vector < vector < float > >(ref_size+1, std::vector<float>(hyp_size+1,-1.0));
	D = new vector < vector < float > >(ref_size+1, std::vector<float>(hyp_size+1,-1.0));
	P = new vector < vector < char > >(ref_size+1, std::vector<char>(hyp_size+1,'0'));
	
        NBR_BS_APPELS++;
// 	cerr << "Appels : " << NBR_BS_APPELS << endl;
	
//         for ( i = 0; i <= ref_size; i++ )
//         {
//             for ( j = 0; j <= hyp_size; j++ )
//             {
//                S->at(i).at(j) = -1.0;
//                P->at(i).at(j) = '0';
//             }
//         }
        S->at(0).at(0) = 0.0;
        D->at(0).at(0) = 0.0;
        for ( j = 0; j <= hyp_size; j++ )
        {
            last_best = current_best;
            current_best = infinite;
            first_good = current_first_good;
            current_first_good = -1;
            last_good = cur_last_good;
            cur_last_good = -1;
            last_peak = cur_last_peak;
            cur_last_peak = 0;
            for ( i = first_good; i <= ref_size; i++ )
            {
                if ( i > last_good )
                {
                    break;
                }
                if (S->at(i).at(j) < 0 )
                {
                    continue;
                }
                score = S->at(i).at(j);
                if ( ( j < hyp_size ) && ( score > last_best + TAILLE_BEAM ) )
                {
                    continue;
                }
                if ( current_first_good == -1 )
                {
                    current_first_good = i ;
                }
                if ( ( i < ref_size ) && ( j < hyp_size ) )
                {
                    if ( ( int ) refSpans.size() ==  0 || ( int ) hypSpans.size() ==  0 || trouverIntersection ( refSpans.at ( i ), curHypSpans.at ( j ) ) )
                    {
                        if ( ( int ) ( ref.at ( i ).compare ( hyp.at ( j ) ) ) == 0 )
                        {
                            cost = match_cost + score;
                            if ( ( S->at(i+1).at(j+1) == -1 ) || ( cost < S->at(i+1).at(j+1) ) )
                            {
                                S->at(i+1).at(j+1) = cost;
                                D->at(i+1).at(j+1) = cost;
                                P->at(i+1).at(j+1) = 'A';
                            }
                            if ( cost < current_best )
                            {
                                current_best = cost;
                            }
                            if ( current_best == cost )
                            {
                                cur_last_peak = i + 1;
                            }
                        }
                        else
                        {
                            cost = substitute_cost + score;
                            if ( ( S->at(i+1).at(j+1) < 0 ) || ( cost < S->at(i+1).at(j+1) ) )
                            {
                                S->at(i+1).at(j+1) = cost;
                                D->at(i+1).at(j+1) = cost;
                                P->at(i+1).at(j+1) = 'S';
                                if ( cost < current_best )
                                {
                                    current_best = cost;
                                }
                                if ( current_best == cost )
                                {
                                    cur_last_peak = i + 1 ;
                                }
                            }
                        }
                    }
                }
                cur_last_good = i + 1;
                if ( j < hyp_size )
                {
                    icost = score + insert_cost;
                    if ( ( S->at(i).at(j+1) < 0 ) || ( S->at(i).at(j+1) > icost ) )
                    {
                        S->at(i).at(j+1) = icost;
                        D->at(i).at(j+1) = icost;
                        P->at(i).at(j+1) = 'I';
                        if ( ( cur_last_peak <  i ) && ( current_best ==  icost ) )
                        {
                            cur_last_peak = i;
                        }
                    }
                }
                if ( i < ref_size )
                {
                    dcost =  score + delete_cost;
                    if ( ( S->at(i+1).at(j) < 0.0 ) || ( S->at(i+1).at(j) > dcost ) )
                    {
                        S->at(i+1).at(j) = dcost;
                        D->at(i+1).at(j) = dcost;
                        P->at(i+1).at(j) = 'D';
                        if ( i >= last_good )
                        {
                            last_good = i + 1 ;
                        }
                    }
                }
            }
        }


        int tracelength = 0;
        i = ref_size;;
        j = hyp_size;
        while ( ( i > 0 ) || ( j > 0 ) )
        {
            tracelength++;
            if (P->at(i).at(j) == 'A' )
            {
                i--;
                j--;
            }
            else
                if (P->at(i).at(j) == 'S' )
                {
                    i--;
                    j--;
                }
                else
                    if (P->at(i).at(j) == 'D' )
                    {
                        i--;
                    }
                    else
                        if (P->at(i).at(j) == 'I' )
                        {
                            j--;
                        }
                        else
                        {
                            cerr << "ERROR : terCalc::minimizeDistanceEdition : Invalid path : " <<P->at(i).at(j) << endl;
                            exit ( -1 );
                        }
        }
        vector<char> path ( tracelength );
        i = ref_size;
        j = hyp_size;
        while ( ( i > 0 ) || ( j > 0 ) )
        {
            path[--tracelength] =P->at(i).at(j);
            if (P->at(i).at(j) == 'A' )
            {
                i--;
                j--;
            }
            else
                if (P->at(i).at(j) == 'S' )
                {
                    i--;
                    j--;
                }
                else
                    if (P->at(i).at(j) == 'D' )
                    {
                        i--;
                    }
                    else
                        if (P->at(i).at(j) == 'I' )
                        {
                            j--;
                        }
        }
//         terAlignment to_return;
        to_return->numWords = ref_size;
        to_return->alignment = path;
        to_return->numEdits = S->at(ref_size).at(hyp_size);
        to_return->deepNumEdits = D->at(ref_size).at(hyp_size);
	to_return->hyp = hyp;
	to_return->ref = ref;
	to_return->averageWords = ref_size;
        if ( PRINT_DEBUG )
        {
            cerr << "BEGIN DEBUG : terCalc::minimizeDistanceEdition : to_return :" << endl << to_return->toString() << endl << "END DEBUG" << endl;
        }
//         return to_return;

    }

    
    terAlignment terCalc::TER ( vector<string>& hyp, vector<string>& ref )
    {
        hashMapInfos rloc = createConcordMots ( hyp, ref );
        terAlignment cur_align = minimizeDistanceEdition ( hyp, ref, hypSpans );
        vector<string> cur = hyp;
        cur_align.hyp = hyp;
        cur_align.ref = ref;
        cur_align.aftershift = hyp;
        float edits = 0;
//         int numshifts = 0;

        vector<terShift> * allshifts=new vector<terShift>(0);
	bestShiftStruct * returns=new bestShiftStruct();

//     cerr << "Initial Alignment:" << endl << cur_align.toString() <<endl;
        if ( PRINT_DEBUG )
        {
            cerr << "BEGIN DEBUG : terCalc::TER : cur_align :" << endl << cur_align.toString() << endl << "END DEBUG" << endl;
        }
        while ( true )
        {
            rloc = createConcordMots ( hyp, ref );
	    
            returns=findBestShift ( cur, hyp, ref, rloc, cur_align );
//             cerr << "****************************************************************** " <<  returns->getEmpty() << endl;
            if ( returns->getEmpty())
            {
                break;
            }
            terShift bestShift = (*(returns->m_best_shift));
            cur_align = (*(returns->m_best_align));
            edits += bestShift.cost;
            bestShift.alignment = cur_align.alignment;
            bestShift.aftershift = cur_align.aftershift;
            allshifts->push_back ( bestShift );
// update shifts
            cur = cur_align.aftershift;
            delete(returns);
        }
        if ( PRINT_DEBUG )
        {
            cerr << "BEGIN DEBUG : terCalc::TER : Final to return :" << endl << cur_align.toString() << endl << "END DEBUG" << endl;
        }
        terAlignment to_return;
        to_return = cur_align;
        to_return.allshifts = (*(allshifts));
        to_return.numEdits += edits;
        NBR_SEGS_EVALUATED++;
        return to_return;
    }
    bestShiftStruct * terCalc::findBestShift ( vector<string>& cur, vector<string>& hyp, vector<string>& ref, hashMapInfos& rloc, terAlignment& med_align )
    {
	CALL_FIND_BSHIFT++;
	if ( PRINT_DEBUG )
        {
            cerr << "BEGIN DEBUG : terCalc::findBestShift CALL OF THE FUNCTION" << endl;
	    cerr << "END DEBUG " << endl;
	}
// 	cerr << "CALL_FIND_BSHIFT " << CALL_FIND_BSHIFT <<endl;
//        to_return->m_empty = new bool(false);
        bool anygain = false;
        vector <bool> * herr = new vector<bool>(( int ) hyp.size() + 1 );
        vector <bool> * rerr = new vector<bool>( ( int ) ref.size() + 1 ); 
        vector <int> * ralign = new vector<int>( ( int ) ref.size() + 1 ); 
	int l_i,i,j,s;
	for (i = 0 ; i< ( int ) hyp.size() + 1 ; i++)
	{
	  herr->at(i)=false;
	}
	for (i = 0 ; i< ( int ) ref.size() + 1 ; i++)
	{
	  rerr->at(i)=false;
	  ralign->at(i)=-1;
	}
        calculateTerAlignment ( med_align, herr, rerr, ralign );
        vector<vecTerShift> * poss_shifts = new vector< vector<terShift> >(0) ;
	terAlignment * cur_best_align = new terAlignment();
        terShift * cur_best_shift = new terShift();
        float cur_best_shift_cost = 0.0;
	vector<string> shiftarr;
	vector<vecInt> curHypSpans;
	terShift * curshift = new terShift();
	alignmentStruct shiftReturns;
	terAlignment * curalign = new terAlignment() ;
	
	
        if ( PRINT_DEBUG )
        {
            cerr << "BEGIN DEBUG : terCalc::findBestShift (after the calculateTerAlignment call) :" << endl;
	    cerr << "indices : ";
	    for (l_i=0; l_i < ( int ) ref.size() ; l_i++)
	    {
		cerr << l_i << "\t";
	    }
	    cerr << endl;
	    cerr << "hyp : \t"<<vectorToString(hyp ,"\t") << endl;
	    cerr << "cur : \t"<<vectorToString(cur ,"\t") << endl;
	    cerr << "ref : \t"<<vectorToString(ref ,"\t") << endl;
	    cerr << "herr   : "<<vectorToString(herr,"\t",( int ) hyp.size()) << " | " << ( int ) hyp.size() <<endl;
	    cerr << "rerr   : "<<vectorToString(rerr,"\t",( int ) ref.size()) << " | " << ( int ) ref.size() <<endl;
	    cerr << "ralign : "<< vectorToString(ralign,"\t",( int ) ref.size()) << " | " << ( int ) ref.size() << endl;
	    cerr << "Alignt  : "<< vectorToString(med_align.alignment,"\t",( int ) med_align.alignment.size()) << " | " << ( int ) ref.size() << endl;
	    cerr << "END DEBUG " << endl;
        }
        poss_shifts = calculerPermutations ( cur, ref, rloc, med_align, herr, rerr, ralign  );
        float curerr = med_align.numEdits;
        if ( PRINT_DEBUG )
        {
            cerr << "BEGIN DEBUG : terCalc::findBestShift :" << endl;
            cerr << "Possible Shifts:" << endl;
            for ( i = ( int ) poss_shifts->size() - 1; i >= 0; i-- )
            {
                for ( j = 0; j < ( int ) ( poss_shifts->at ( i ) ).size(); j++ )
                {
                    cerr << " [" << i << "] " << ( ( poss_shifts->at ( i ) ).at ( j ) ).toString() << endl;
                }
            }
            cerr << endl;
            cerr << "END DEBUG " << endl;
        }
// 	exit(0);
        cur_best_align->set(med_align);
        for ( i = ( int ) poss_shifts->size() - 1; i >= 0; i-- )
        {
            if ( PRINT_DEBUG )
            {
                cerr << "BEGIN DEBUG : terCalc::findBestShift :" << endl;
                cerr << "Considering shift of length " << i << " (" << ( poss_shifts->at ( i ) ).size() << ")" << endl;
                cerr << "END DEBUG " << endl;
            }
            /* Consider shifts of length i+1 */
            float curfix = curerr - ( cur_best_shift_cost + cur_best_align->numEdits );
            float maxfix = ( 2 * ( 1 + i ) );
	    if ( ( curfix > maxfix ) || ( ( cur_best_shift_cost != 0 ) && ( curfix == maxfix ) ) )
	    {
		break;
	    }
	    else
	    {
		for ( s = 0; s < ( int ) ( poss_shifts->at ( i ) ).size(); s++ )
		{
		    curfix = curerr - ( cur_best_shift_cost + cur_best_align->numEdits );
		    if ( ( curfix > maxfix ) || ( ( cur_best_shift_cost != 0 ) && ( curfix == maxfix ) ) )
		    {
			break;
		    }
		    else
		    {
			curshift->set(( poss_shifts->at ( i ) ).at ( s ));
			if ( PRINT_DEBUG )
			{
			    cerr << "BEGIN DEBUG : terCalc::findBestShift :" << endl;
			    cerr << "cur : "<< join(" ",cur) << endl;
			    cerr << "shift size : "<< i << endl;
			    cerr << "shift number : "<< s << endl;
			    cerr << "size of shift size  : "<< ( int ) ( poss_shifts->at ( i ) ).size() << endl;
			    cerr << "curshift : "<< curshift->toString() << endl;
			
			}
// 			alignmentStruct shiftReturns;
			shiftReturns.set(permuter ( cur, curshift ));
			shiftarr = shiftReturns.nwords;
			curHypSpans = shiftReturns.aftershift;

			if ( PRINT_DEBUG )
			{
			    cerr << "shiftarr : "<< join(" ",shiftarr) << endl;
			    cerr << "curHypSpans size : "<< (int)curHypSpans.size() << endl;
			    cerr << "END DEBUG " << endl;
			}
// 			terAlignment tmp=minimizeDistanceEdition ( shiftarr, ref, curHypSpans );
			minimizeDistanceEdition ( shiftarr, ref, curHypSpans, curalign );
// 			curalign->set(tmp);

			curalign->hyp = hyp;
			curalign->ref = ref;
			curalign->aftershift = shiftarr;


			float gain = ( cur_best_align->numEdits + cur_best_shift_cost ) - ( curalign->numEdits + curshift->cost );

			if ( PRINT_DEBUG )
			{
			    cerr << "BEGIN DEBUG : terCalc::findBestShift :" << endl;
			    cerr << "Gain for " << curshift->toString() << " is " << gain << ". (result: [" << curalign->join ( " ", shiftarr ) << "]" << endl;
			    cerr << "Details of gains : gain = ( cur_best_align->numEdits + cur_best_shift_cost ) - ( curalign->numEdits + curshift->cost )"<<endl;
			    cerr << "Details of gains : gain = ("<<cur_best_align->numEdits << "+" << cur_best_shift_cost << ") - (" << curalign->numEdits << "+" <<  curshift->cost << ")"<<endl;
			    cerr << "" << curalign->toString() << "\n" << endl;
			    cerr << "END DEBUG " << endl;
			}

			if ( ( gain > 0 ) || ( ( cur_best_shift_cost == 0 ) && ( gain == 0 ) ) )
			{
			    anygain = true;
			    cur_best_shift->set(curshift);
			    cur_best_shift_cost = curshift->cost;
			    cur_best_align->set(curalign);
			    if ( PRINT_DEBUG )
			    {
				cerr << "BEGIN DEBUG : terCalc::findBestShift :" << endl;
				cerr << "Tmp Choosing shift: " << cur_best_shift->toString() << " gives:\n" << cur_best_align->toString()  << "\n" << endl;
				cerr << "END DEBUG " << endl;
			    }
			}
		    }
		}
	    }
        }
	bestShiftStruct * to_return=new bestShiftStruct();
        if ( anygain )
        {
            to_return->setEmpty(false);
	    if ( PRINT_DEBUG )
	    {
		cerr << "BEGIN DEBUG : terCalc::findBestShift :" << endl;
		cerr << "Final shift chosen : " << cur_best_shift->toString() << " gives:\n" << cur_best_align->toString()  << "\n" << endl;
		cerr << "END DEBUG " << endl;
	    }
            to_return->m_best_shift->set(cur_best_shift);
// 	    terAlignment tmp=cur_best_align;
// 	    cur_best_align->toString();
// 	    to_return.m_best_align.toString();
// 	    if ((int)cur_best_align->alignment.size() == 0)
// 	    {
// 		to_return.m_best_align = cur_best_align;
// 	    }
// 	    else
// 	    {
// 		cerr << "Warning: cur_best_align->alignment.size() = 0 !!!"<<endl;
// 		
// 	    }
	    to_return->m_best_align->set(cur_best_align);
// 	    to_return.m_best_align.toString();
        }
        else
        {
            to_return->setEmpty(true);
        }
// // 		cerr << to_return->toString() << endl;
		delete(poss_shifts);
		delete(cur_best_align);
        delete(cur_best_shift);
		delete(curshift);
		delete(curalign) ;
        return to_return;
    }

    void terCalc::calculateTerAlignment ( terAlignment& align, vector<bool>* herr, vector<bool>* rerr, vector<int>* ralign )
    {
        int hpos = -1;
        int rpos = -1;
	CALL_TER_ALIGN++;
// 	cerr << "CALL_TER_ALIGN " << CALL_TER_ALIGN << endl;
        if ( PRINT_DEBUG )
        {

            cerr << "BEGIN DEBUG : terCalc::calculateTerAlignment : " << endl << align.toString() << endl;
            cerr << "END DEBUG " << endl;
        }
//         cerr << (int)herr->size() <<endl;
//         cerr << (int)rerr->size() <<endl;
//         cerr << ( int ) align.alignment.size() <<endl;
//         for ( int i = 0; i < ( int ) align.alignment.size(); i++ )
//         {
//                 herr->at(i) = false;
//                 rerr->at(i) = false;
//                 ralign->at(i) = -1;
// 	}
	char sym_prev = ' ';
	bool l_findDS = false;
        for ( int i = 0; i < ( int ) align.alignment.size(); i++ )
        {
            char sym = align.alignment.at(i);
            if ( sym == 'A' )
            {
                hpos++;
                rpos++;
//                 herr->at(hpos) = false;
//                 rerr->at(rpos) = false;
                ralign->at(rpos) = hpos;
		if (l_findDS)
		{
		    l_findDS=false;
		}
            }
            else
                if ( sym == 'S' )
                {
                    hpos++;
                    rpos++;
                    herr->at(hpos) = true;
                    rerr->at(rpos) = true;
                    ralign->at(rpos) = hpos;
		    if (sym_prev == 'D') 
		    {
			l_findDS = true;
		    }
		    if (l_findDS)
		    {
			herr->at(hpos+1) = true;
			ralign->at(rpos) = hpos+1;
		    }
                }
                else
                    if ( sym == 'I' )
                    {
                        hpos++;
                        herr->at(hpos) = true;
                    }
                    else
                        if ( sym == 'D' )
                        {
// 			    hpos++;
                            rpos++;
                            rerr->at(rpos) = true;
                            rerr->at(rpos+1) = true;
			    ralign->at(rpos) = hpos+1;
// 			    herr->at(hpos+1) = true;
                        }
                        else
                        {
                            cerr << "ERROR : terCalc::calculateTerAlignment : Invalid mini align sequence " << sym << " at pos " << i << endl;
                            exit ( -1 );
                        }
	    sym_prev=sym;
        }
    }

    vector<vecTerShift> * terCalc::calculerPermutations ( vector< string >& hyp, vector< string >& ref, hashMapInfos& rloc, TERCpp::terAlignment& align, vector<bool>* herr, vector<bool>* rerr, vector<int>* ralign )
    {
        vector<vecTerShift> * allshifts = new vector<vecTerShift>(0);
// 	to_return.clear();
	CALL_CALC_PERMUT++;
// 	cerr << "CALL_CALC_PERMUT " << CALL_CALC_PERMUT << endl;
        if ( ( TAILLE_PERMUT_MAX <= 0 ) || ( DIST_MAX_PERMUT <= 0 ) )
        {
            return allshifts;
        }
        allshifts = new vector<vecTerShift>( TAILLE_PERMUT_MAX + 1 );
	int start=0;
	int end=0;
	bool ok = false;
	vector<int> mtiVec(0);
	vector<int>::iterator mti;
	int moveto=0;
	vector<string> cand(0);
	bool any_herr = false;
	bool any_rerr = false;
	int i=0;
	int l_nbr_permuts=0;
// 	for (i=0; i< (int)ref.size() +1 ; i++) {cerr << " " << ralign[i] ;} cerr <<endl;
	vector<int> movetoitVec(0);
	string subVectorHypString="";
	terShift * topush;
        for ( start = 0; start < ( int ) hyp.size(); start++ )
        {
            subVectorHypString = vectorToString ( subVector ( hyp, start, start + 1 ) );
// 	    if ( PRINT_DEBUG )
// 	    {
// 		cerr << "CANDIDATE 00: " <<  subVectorHypString << endl;
// 	    }
	    
            if ( ! rloc.trouve ( subVectorHypString ) )
            {
                continue;
            }

            ok = false;
            mtiVec = rloc.getValue ( subVectorHypString );
            mti = mtiVec.begin();
// 	    if ( PRINT_DEBUG )
// 	    {
// 		cerr << "CANDIDATE 01: " <<   vectorToString ( mtiVec ) << endl;
// 	    }
	    
            while ( mti != mtiVec.end() && ( ! ok ) )
            {
                moveto = ( *mti );
                mti++;
		if ( ( start != ralign->at(moveto) ) && ( ( ralign->at(moveto) - start ) <= DIST_MAX_PERMUT ) && ( ( start - ralign->at(moveto) - 1 ) <= DIST_MAX_PERMUT ) )
                {
                    ok = true;
                }
            }
            if ( ! ok )
            {
                continue;
            }
            ok = true;
	    any_herr = false;
//             for ( end = start; ( ok && ( end < ( int ) hyp.size() ) && ( end < start + TAILLE_PERMUT_MAX ) ); end++ )
            for ( end = start; ( end < ( int ) hyp.size() ) && ( end < start + TAILLE_PERMUT_MAX ) ; end++ )
            {
                /* check if cand is good if so, add it */
                cand = subVector ( hyp, start, end + 1 );
// 		if ( PRINT_DEBUG )
// 		{
// // 		    cerr << "BEGIN DEBUG : terCalc::calculerPermutations : " << endl ; 
// 		    cerr << "CANDIDATE " << ( string ) vectorToString ( cand )  <<" PLACED : " << ( string ) vectorToString ( movetoitVec," ") << endl;
// // 		    cerr << endl << "END DEBUG" << endl;
// 		}
                ok = false;
                if ( ! ( rloc.trouve ( vectorToString ( cand ) ) ) )
                {
                    continue;
                }


                for ( i = 0; ( ( i <= ( end - start ) ) && ( ! any_herr ) ); i++ )
                {
                    if ( herr->at(start+i) )
                    {
                        any_herr = true;
                    }
                }
                if ( any_herr == false )
                {
                    ok = true;
                    continue;
                }

                movetoitVec = rloc.getValue ( ( string ) vectorToString ( cand ) );

// 		cerr << "CANDIDATE " << ( string ) vectorToString ( cand )  <<" PLACED : " << ( string ) vectorToString ( movetoitVec," ") << endl;
// 		if ( PRINT_DEBUG )
// 		{
// // 		    cerr << "BEGIN DEBUG : terCalc::calculerPermutations : " << endl ; 
// 		    cerr << "CANDIDATE " << ( string ) vectorToString ( cand )  <<" PLACED : " << ( string ) vectorToString ( movetoitVec," ") << endl;
// // 		    cerr << endl << "END DEBUG" << endl;
// 		}
		vector<int>::iterator movetoit;
                movetoit = movetoitVec.begin();
                while ( movetoit != movetoitVec.end() )
                {
                    moveto = ( *movetoit );
                    movetoit++;
// 		    if ( PRINT_DEBUG )
// 		    {
// 			cerr << "on demarre"<< endl;
// 		    }
//                     if ( ! ( ( ralign->at(moveto) != start ) && ( ( ralign->at(moveto) < start ) || ( ralign->at(moveto) > end ) ) && ( ( ralign->at(moveto) - start ) <= DIST_MAX_PERMUT ) && ( ( start - ralign->at(moveto) - 1 ) <= DIST_MAX_PERMUT ) ) )
//                     {
//                         continue;
//                     }
                    ok = true;
		    if ( PRINT_DEBUG )
// 		    {
// 			cerr << "on continue"<< endl;
// 		    }

                    /* check to see if there are any errors in either string
                    (only move if this is the case!)
                    */

                    any_rerr = false;
                    for ( int i = 0; ( i <= end - start ) && ( ! any_rerr ); i++ )
                    {
                        if ( rerr->at(moveto+i) )
                        {
                            any_rerr = true;
                        }
                    }
                    if ( ! any_rerr )
                    {
                        continue;
                    }
                    for ( int roff = -1; roff <= ( end - start ); roff++ )
                    {
			topush = new terShift();
                        bool topushNull = true;
                        if ( ( roff == -1 ) && ( moveto == 0 ) )
                        {
                            if ( PRINT_DEBUG )
                            {

                                cerr << "BEGIN DEBUG : terCalc::calculerPermutations 01 : " << endl << "Consider making " << start << "..." << end << " (" << vectorToString(cand," ")<< ") moveto: " << moveto << " roff: " << roff << " ralign[mt+roff]: -1" << endl << "END DEBUG" << endl;
                            }
//                             terShift t01 ( start, end, -1, -1 );
//                             topush = t01;
			    topush->start=start;
			    topush->end=end;
			    topush->moveto=-1;
			    topush->newloc=-1;
			    topush->roff=roff;
                            topushNull = false;
                        }
                        else
			    if  ( ( start != ralign->at(moveto+roff) ) && ( ( roff == 0 ) || ( ralign->at(moveto+roff) != ralign->at(moveto) ) ) )
                            {
                                int newloc = ralign->at(moveto+roff);
                                if ( PRINT_DEBUG )
                                {

                                    cerr << "BEGIN DEBUG : terCalc::calculerPermutations 02 : " << endl << "Consider making " << start << "..." << end << " (" << vectorToString(cand," ")<< ") moveto: " << moveto << " roff: " << roff << " ralign[mt+roff]: " << newloc << endl << "END DEBUG" << endl;
                                }
//                                 terShift t02 ( start, end, moveto + roff, newloc );
//                                 topush = t02;
				topush->start=start;
				topush->end=end;
				topush->moveto=moveto + roff;
// 				topush->moveto=newloc - ( end - start );
				topush->newloc=newloc;
				topush->roff=roff;
                                topushNull = false;
                            }
                        if ( !topushNull )
                        {
                            topush->shifted = cand;
                            topush->cost  = shift_cost;
                            l_nbr_permuts++;
                            if ( PRINT_DEBUG )
                            {

                                cerr << "BEGIN DEBUG : terCalc::calculerPermutations 02 : " << endl;
                                cerr << "start : " << start << endl;
                                cerr << "end : " << end << endl;
                                cerr << "end - start : " << end - start << endl;
                                cerr << "nbr Permutations added: " << l_nbr_permuts << endl;
				cerr << "nbr Permutations MAX : " << NBR_PERMUT_MAX + 1<< endl;
                                cerr << "END DEBUG " << endl;
                            }
                            if (l_nbr_permuts < NBR_PERMUT_MAX + 1)
			    {
				( allshifts->at ( end - start ) ).push_back ( (*(topush)) );
			    }
// 			    else
// 			    {
// 				break;
// 			    }
                        }
                        delete(topush);
                    }
                }
            }
        }
//         to_return.clear();
//         for ( int i = 0; i < TAILLE_PERMUT_MAX + 1; i++ )
//         {
//             to_return.push_back ( ( vecTerShift ) allshifts.at ( i ) );
//         }
        if ( PRINT_DEBUG )
        {
            cerr << "BEGIN DEBUG : terCalc::calculerPermutations :" << endl;
            cerr << "End of the procedure possibles possible shifts available:" << endl;
            for ( int i = ( int ) allshifts->size() - 1; i >= 0; i-- )
            {
                for ( int j = 0; j < ( int ) ( allshifts->at ( i ) ).size(); j++ )
                {
                    cerr << " [" << i << "] " << ( ( allshifts->at ( i ) ).at ( j ) ).toString() << endl;
                }
            }
            cerr << endl;
            cerr << "END DEBUG " << endl;
        }

        return allshifts;
    }


    alignmentStruct terCalc::permuter ( vector< string >& words, TERCpp::terShift& s )
    {
        return permuter ( words, s.start, s.end, s.newloc );
    }
    alignmentStruct terCalc::permuter ( vector< string >& words, TERCpp::terShift* s )
    {
        return permuter ( words, s->start, s->end, s->newloc );
    }


    alignmentStruct terCalc::permuter ( vector< string >& words, int start, int end, int newloc )
    {
        int c = 0;
        vector<string> nwords ( words );
        vector<vecInt> spans ( ( int ) hypSpans.size() );
        alignmentStruct to_return;
        if ( PRINT_DEBUG )
        {

            if ( ( int ) hypSpans.size() > 0 )
            {
                cerr << "BEGIN DEBUG : terCalc::permuter :" << endl << "word length: " << ( int ) words.size() << " span length: " << ( int ) hypSpans.size() << endl ;
            }
            else
            {
                cerr << "BEGIN DEBUG : terCalc::permuter :" << endl << "word length: " << ( int ) words.size() << " span length: null" << endl ;
            }
            cerr << "BEGIN DEBUG : terCalc::permuter :" << endl << join(" ",words) << " start: " << start << " end: " << end << " newloc "<< newloc << endl << "END DEBUG " << endl;
        }
        if (newloc >=  ( int ) words.size())
	{
	    if ( PRINT_DEBUG )
	    {
		cerr << "WARNING: Relocation over the size of the hypothesis, replacing at the end of it."<<endl;
	    }
	    newloc =  ( int ) words.size()-1;
	}
	  
// 		}

        if ( newloc == -1 )
        {
            for ( int i = start; i <= end;i++ )
            {
                nwords.at ( c++ ) = words.at ( i );
                if ( ( int ) hypSpans.size() > 0 )
                {
                    spans.at ( c - 1 ) = hypSpans.at ( i );
                }
            }
            for ( int i = 0; i <= start - 1;i++ )
            {
                nwords.at ( c++ ) = words.at ( i );
                if ( ( int ) hypSpans.size() > 0 )
                {
                    spans.at ( c - 1 ) = hypSpans.at ( i );
                }
            }
            for ( int i = end + 1; i < ( int ) words.size();i++ )
            {
                nwords.at ( c++ ) = words.at ( i );
                if ( ( int ) hypSpans.size() > 0 )
                {
                    spans.at ( c - 1 ) = hypSpans.at ( i );
                }
            }
        }
        else
        {
            if ( newloc < start )
            {
		
                for ( int i = 0; i < newloc; i++ )
                {
                    nwords.at ( c++ ) = words.at ( i );
                    if ( ( int ) hypSpans.size() > 0 )
                    {
                        spans.at ( c - 1 ) = hypSpans.at ( i );
                    }
                }
                for ( int i = start; i <= end;i++ )
                {
                    nwords.at ( c++ ) = words.at ( i );
                    if ( ( int ) hypSpans.size() > 0 )
                    {
                        spans.at ( c - 1 ) = hypSpans.at ( i );
                    }
                }
                for ( int i = newloc ; i < start ;i++ )
                {
                    nwords.at ( c++ ) = words.at ( i );
                    if ( ( int ) hypSpans.size() > 0 )
                    {
                        spans.at ( c - 1 ) = hypSpans.at ( i );
                    }
                }
                for ( int i = end + 1; i < ( int ) words.size();i++ )
                {
                    nwords.at ( c++ ) = words.at ( i );
                    if ( ( int ) hypSpans.size() > 0 )
                    {
                        spans.at ( c - 1 ) = hypSpans.at ( i );
                    }
                }
            }
            else
            {
                if ( newloc > end )
                {
                    for ( int i = 0; i <= start - 1; i++ )
                    {
                        nwords.at ( c++ ) = words.at ( i );
                        if ( ( int ) hypSpans.size() > 0 )
                        {
                            spans.at ( c - 1 ) = hypSpans.at ( i );
                        }
                    }
                    for ( int i = end + 1; i <= newloc;i++ )
                    {
                        nwords.at ( c++ ) = words.at ( i );
                        if ( ( int ) hypSpans.size() > 0 )
                        {
                            spans.at ( c - 1 ) = hypSpans.at ( i );
                        }
                    }
                    for ( int i = start; i <= end;i++ )
                    {
                        nwords.at ( c++ ) = words.at ( i );
                        if ( ( int ) hypSpans.size() > 0 )
                        {
                            spans.at ( c - 1 ) = hypSpans.at ( i );
                        }
                    }
                    for ( int i = newloc + 1; i < ( int ) words.size();i++ )
                    {
                        nwords.at ( c++ ) = words.at ( i );
                        if ( ( int ) hypSpans.size() > 0 )
                        {
                            spans.at ( c - 1 ) = hypSpans.at ( i );
                        }
                    }
                }
                else
                {
                    // we are moving inside of ourselves
                    for ( int i = 0; i <= start - 1; i++ )
                    {
                        nwords.at ( c++ ) = words.at ( i );
                        if ( ( int ) hypSpans.size() > 0 )
                        {
                            spans.at ( c - 1 ) = hypSpans.at ( i );
                        }
                    }
                    for ( int i = end + 1; ( i < ( int ) words.size() ) && ( i <= ( end + ( newloc - start ) ) ); i++ )
                    {
                        nwords.at ( c++ ) = words.at ( i );
                        if ( ( int ) hypSpans.size() > 0 )
                        {
                            spans.at ( c - 1 ) = hypSpans.at ( i );
                        }
                    }
                    for ( int i = start; i <= end;i++ )
                    {
                        nwords.at ( c++ ) = words.at ( i );
                        if ( ( int ) hypSpans.size() > 0 )
                        {
                            spans.at ( c - 1 ) = hypSpans.at ( i );
                        }
                    }
                    for ( int i = ( end + ( newloc - start ) + 1 ); i < ( int ) words.size();i++ )
                    {
                        nwords.at ( c++ ) = words.at ( i );
                        if ( ( int ) hypSpans.size() > 0 )
                        {
                            spans.at ( c - 1 ) = hypSpans.at ( i );
                        }
                    }
                }
            }
        }
        NBR_PERMUTS_CONSID++;
	
        if ( PRINT_DEBUG )
        {
	    cerr << "nwords" << join(" ",nwords) << endl;
// 	    cerr << "spans" << spans. << endl;
	}
	
        to_return.nwords = nwords;
        to_return.aftershift = spans;
        return to_return;
    }
    void terCalc::setDebugMode ( bool b )
    {
        PRINT_DEBUG = b;
    }
    void terCalc::setW2VModel(string filename)
    {
	m_distance = new word2vecdistance::distance(filename);
    }
    void terCalc::setW2VModel(word2vecdistance::distance* f_distance)
    {
	m_distance = f_distance;
    }
    bool terCalc::getDeep()
    {
	return m_deep;
    }
    void terCalc::setDeep(bool l_b)
    {
	m_deep = l_b;
    }
}
