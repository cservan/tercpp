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
#include "documentStructure.h"

using namespace std;
namespace TERCpp
{

    string documentStructure::toString()
    {
        stringstream s;
// 	s << "nword : " << vectorToString(nwords)<<endl;
// 	s << "alignment" << vectorToString(alignment)<<endl;
// 	s << "afterShift" << vectorToString(alignment)<<endl;
//         s << "Nothing to be printed" << endl;
	s << "\tFileName: "<< m_fileName << endl;
	s << "\tDoc Id  : "<< docId << endl;
	s << "\tSys Id  : "<< sysId << endl;
	int i=0;
	for (i=0; i<(int)seg.size(); i++)
	{
	    s << "\t"<< seg.at(i).toString();
	}
        return s.str();
    }

    string documentStructure::getDocId()
    {
        return docId;
    }
    string documentStructure::getFileName()
    {
        return m_fileName;
    }
    void documentStructure::setFileName(string f_fileName)
    {        
        m_fileName = f_fileName;
    }

    vector< segmentStructure >* documentStructure::getSegments()
    {
        return &seg;
    }

    string documentStructure::getSysId()
    {
        return sysId;
    }
//     float documentStructure::getAverageLength()
//     {
// 	return averageLength;
//     }
//     void documentStructure::setAverageLength(float f)
//     {
// 	averageLength=f;
//     }

    void documentStructure::addSegments ( segmentStructure s )
    {
        seg.push_back ( s );
    }
    void documentStructure::addSegments ( string id, string text )
    {
        segmentStructure tmp_seg ( id, text );
        seg.push_back ( tmp_seg );
    }
    segmentStructure* documentStructure::getLastSegments()
    {
        return & seg.at ( ( int ) seg.size() - 1 );
    }
    void documentStructure::setDocId ( string s )
    {
        docId = s;
    }
    void documentStructure::setSysId ( string s )
    {
        sysId = s;
    }

    segmentStructure* documentStructure::getSegment ( string id )
    {
        for ( int i = 0; i < ( int ) seg.size(); i++ )
        {
            if ( id.compare ( seg.at ( i ).getSegId() ) == 0 )
            {
                return & ( seg.at ( i ) );
            }
        }
        cerr << "ERROR : documentStructure::getSegment : Segment " << id << " does not exist" <<endl;
        cerr << "Segment size " << seg.size()<< endl;
	for (int i=0;i<(int)seg.size(); i++)
	{
	  cerr <<seg.at(i).getSegId()<<endl;
	}
	exit(0);
    }
    int documentStructure::getSize()
    {
        return ( int ) seg.size();
    }


// 	documentStructure::documentStructure()
// 	{
// // 		vector<string> ref;
// // 		vector<string> hyp;
// // 		vector<string> aftershift;
//
// 		//   documentStructure[] allshifts = null;
//
// 		numEdits=0;
// 		numWords=0;
// 		bestRef="";
//
// 		numIns=0;
// 		numDel=0;
// 		numSub=0;
// 		numSft=0;
// 		numWsf=0;
// 	}
// 	documentStructure::documentStructure ()
// 	{
// 		start = 0;
// 		end = 0;
// 		moveto = 0;
// 		newloc = 0;
// 		cost=1.0;
// 	}
// 	documentStructure::documentStructure (int _start, int _end, int _moveto, int _newloc)
// 	{
// 		start = _start;
// 		end = _end;
// 		moveto = _moveto;
// 		newloc = _newloc;
// 		cost=1.0;
// 	}

// 	documentStructure::documentStructure (int _start, int _end, int _moveto, int _newloc, vector<string> _shifted)
// 	{
// 		start = _start;
// 		end = _end;
// 		moveto = _moveto;
// 		newloc = _newloc;
// 		shifted = _shifted;
// 		cost=1.0;
// 	}
// 	string documentStructure::vectorToString(vector<string> vec)
// 	{
// 		string retour("");
// 		for (vector<string>::iterator vecIter=vec.begin();vecIter!=vec.end(); vecIter++)
// 		{
// 			retour+=(*vecIter)+"\t";
// 		}
// 		return retour;
// 	}

// 	string documentStructure::toString()
// 	{
// 		stringstream s;
// 		s.str("");
// 		s << "[" << start << ", " << end << ", " << moveto << "/" << newloc << "]";
// 		if ((int)shifted.size() > 0)
// 		{
// 			s << " (" << vectorToString(shifted) << ")";
// 		}
// 		return s.str();
// 	}

    /* The distance of the shift. */
// 	int documentStructure::distance()
// 	{
// 		if (moveto < start)
// 		{
// 			return start - moveto;
// 		}
// 		else if (moveto > end)
// 		{
// 			return moveto - end;
// 		}
// 		else
// 		{
// 			return moveto - start;
// 		}
// 	}
//
// 	bool documentStructure::leftShift()
// 	{
// 		return (moveto < start);
// 	}
//
// 	int documentStructure::size()
// 	{
// 		return (end - start) + 1;
// 	}
// 	documentStructure documentStructure::operator=(documentStructure t)
// 	{
//
// 		return t;
// 	}


}
