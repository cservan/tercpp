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
#ifndef __SEGMENTSTRUCTURE_H__
#define __SEGMENTSTRUCTURE_H__


#include <vector>
#include <stdio.h>
#include <string>
#include <sstream>
#include "tools.h"
#include "tercalc.h"


using namespace std;
using namespace Tools;

namespace TERCpp
{
    class segmentStructure
    {
        private:
            string segId;
            vecString content;
            terAlignment evaluation;
            string bestDocId;
    	    float averageLength;

        public:
            segmentStructure();
            segmentStructure ( string id, vecString vecS );
            segmentStructure ( string id, string txt );
	    void setAverageLength(float f);
	    float getAverageLength();
            string getSegId();
            terAlignment getAlignment();
            void setAlignment(terAlignment& l_align);
            void setSegId ( string s );
            void setBestDocId ( string s );
            string getBestDocId();
            void addContent ( vecString vecS );
            void addContent ( string s );
	    int getSize();
// 	  {
// 	    return segId;
// 	  }
            vecString getContent();
// 	  {
// 	    return content;
// 	  }
// 	alignmentStruct();
// 	alignmentStruct (int _start, int _end, int _moveto, int _newloc);
// 	alignmentStruct (int _start, int _end, int _moveto, int _newloc, vector<string> _shifted);
// 	string toString();
// 	int distance() ;
// 	bool leftShift();
// 	int size();
// 	alignmentStruct operator=(alignmentStruct t);
// 	string vectorToString(vector<string> vec);

//   int start;
//   int end;
//   int moveto;
//   int newloc;
            vector<string> nwords; // The words we shifted
            vector<char> alignment ; // for pra_more output
            vector<vecInt> aftershift; // for pra_more output
            // This is used to store the cost of a shift, so we don't have to
            // calculate it multiple times.
            double cost;
            string toString();
    };

}
#endif
