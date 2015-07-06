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
/*
 * Generic hashmap manipulation functions
 */
#ifndef __XMLSTRUCTURE_H__
#define __XMLSTRUCTURE_H__

#include "sgmlDocument.h"
#include "documentStructure.h"
#include "stdio.h"
#include <iostream>
#include <string>
#include "tinyxml.h"

using namespace std;

namespace TERCpp
{
    class xmlStructure
    {
        private:
	  unsigned int NUM_INDENTS_PER_SPACE;
//             void dump_attribs_to_SGMLDocuments ( SGMLDocument* arg1, const TiXmlElement* arg2 );
          void dump_attribs_to_SGMLDocuments ( SGMLDocument* sgmlDoc, TiXmlElement* pElement, unsigned int indent );
        public:
	  xmlStructure();
	  const char * getIndent( unsigned int numIndents );
	  const char * getIndentAlt( unsigned int numIndents );
	  int dump_attribs_to_stdout(TiXmlElement* pElement, unsigned int indent);
	  void dump_to_stdout( TiXmlNode* pParent, unsigned int indent );
	  void dump_to_stdout(const char* pFilename);
	  void copy_to_SGMLDocument(SGMLDocument* sgmlDoc ,TiXmlNode* pParent, unsigned int indent );
	  SGMLDocument dump_to_SGMLDocument(string FileName);
	  
    };


}


#endif
