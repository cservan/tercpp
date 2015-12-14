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
#include "segmentStructure.h"

using namespace std;
namespace TERCpp
{
    vecString& segmentStructure::getContent()
    {
        return content;
    }
    string segmentStructure::getSegId()
    {
        return segId;
    }
    string segmentStructure::toString()
    {
//         return vectorToString(content);
	stringstream s;
	s << "\tSeg Id: "<<segId << endl;
	s << "\t\tContent: "<< vectorToString(content," ")<< endl;
	s << "\t\tAverage length: "<< averageLength<< endl;
        return s.str();
    }
    void segmentStructure::addContent ( vecString vecS )
    {
        content = vecS;
	averageLength=0.0;
    }
    void segmentStructure::setSegId ( string s )
    {
        segId = s;
    }
    segmentStructure::segmentStructure ( string id, vecString vecS )
    {
        segId = id;
        content = vecS;
	averageLength=0.0;
    }
    segmentStructure::segmentStructure ( string id, vecString vecS, string docId )
    {
        segId = id;
        content = vecS;
	averageLength=0.0;
	bestDocId = docId;
    }
    segmentStructure::segmentStructure ( string id, string txt )
    {
        segId = id;
        content = stringToVector ( txt, " " );
	averageLength=0.0;
    }
    void segmentStructure::addContent ( string s )
    {
        content = stringToVector ( s, " " );
	averageLength=0.0;
    }
    segmentStructure::segmentStructure()
    {
        segId = "";
    }
    terAlignment segmentStructure::getAlignment()
    {
        return evaluation;
    }
    void segmentStructure::setAlignment ( terAlignment& l_align )
    {
	evaluation = l_align;
    }

    string segmentStructure::getBestDocId()
    {
        return bestDocId;
    }
    void segmentStructure::setBestDocId ( string s )
    {
        bestDocId = s;
    }
    float segmentStructure::getAverageLength()
    {
	return averageLength;
    }
    void segmentStructure::setAverageLength(float f)
    {
	averageLength=f;
    }
    int segmentStructure::getSize()
    {
	return (int)content.size();
    }




}
