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
#ifndef __MULTIEVAL_DOCUMENT_H__
#define __MULTIEVAL_DOCUMENT_H__

#include "multiTxtDocument.h"
#include "tools.h"
#include <iostream>
#include <string>
#include "xmlStructure.h"
#include "sgmlDocument.h"
#include "distance.h"
using namespace Tools;
namespace TERCpp
{

    class multiEvaluation
    {
        public:
            multiEvaluation();
            multiEvaluation(param p );
//     void addReferences(string s);
//     void addReferences(vector<string> vecRefecrences);
//     void addReferences(documentStructure doc);
//     void setHypothesis(string s);
//     void setHypothesis(documentStructure doc);
            void addReferences();
            void setHypothesis();
            void addSGMLReferences();
            void setSGMLHypothesis();
            void setParameters ( param p );
            void launchTxtEvaluation();
            void launchSGMLEvaluation();
            void evaluate ( documentStructure & docStructReference, documentStructure & docStructhypothesis );
            string scoreTER ( vector<float> numEdits, vector<float> numWords );
	    
        private:
            param evalParameters;
            multiTxtDocument referencesTxt;
            documentStructure hypothesisTxt;
	    SGMLDocument referencesSGML;
            SGMLDocument hypothesisSGML;
	    word2vecdistance::distance * m_distance;


    };
}
#endif //SANDWICH_DEFINED
