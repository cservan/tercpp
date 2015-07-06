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
#ifndef __SGML_DOCUMENT_H__
#define __SGML_DOCUMENT_H__

#include "documentStructure.h"
// #include "xmlStructure.h"

#include <iostream>
#include <string>
namespace TERCpp
{

    class SGMLDocument
    {
        public:
            SGMLDocument();
// 		SGMLDocument(string FileName);
// 		SGMLDocument(const std::string &bread, const std::string &cheese, const std::string &meat, const bool pickle):
// 			m_bread(bread), m_cheese(cheese), m_meat(meat), m_pickle(pickle){};
// 		~SGMLDocument(){};

// 		void output()
// 		{
// 			std::cout << "Bread = " << m_bread << ", Cheese = " << m_cheese <<
// 				", Meat = " << m_meat << ", Has Pickle = " << m_pickle << std::endl;
//
// 		}
            void setDocType ( string s );
            void setSetId ( string s );
            void setSrcLang ( string s );
            void setTgtLang ( string s );
            string getDocType();
            string getSetId();
            string getSrcLang();
            string getTgtLang();
// 		xmlStructure getStructure();
            void addDocument ( documentStructure doc );
            documentStructure* getLastDocument();
            documentStructure* getFirstDocument();
	    int getSize();
	    documentStructure* getDocument(string docId);
	    vector <documentStructure> getDocumentCollection(string docId);
	    documentStructure* getDocument(int docId);
	    void setAverageLength();
	    void setParams(param p);
	    string toString();

        private:
            string docType;
            string setId;
            string srcLang;
            string tgtLang;
	    param sgmlDocumentParams;
// 		xmlStructure xmlStruct;
            vector<documentStructure> documents;
// 		std::string m_bread, m_cheese, m_meat;
// 		bool m_pickle;
//
// 	// declare the boost::serialization::access class as a friend of SGMLDocument
// 	friend class boost::serialization::access;
// 	// Create a serialize function for serialization::access to use, I guess you could regard this as a kind of callback function!
// 	template<class archive>
// 	void serialize(archive& ar, const unsigned int version)
// 	{
// 		// Note: As explained in the original tut. the & operator is overridden in boost to use
// 		// << or >> depending on the direction of the data (read/write)
// 		using boost::serialization::make_nvp;
// 		ar & make_nvp("Bread", m_bread);
// 		ar & make_nvp("Cheese", m_cheese);
// 		ar & make_nvp("Meats", m_meat);
// 		ar & make_nvp("HasPickle", m_pickle);
// 		// Also note: strings in the first parameter of make_nvp cannot contain spaces!
// 	}
    };
}
#endif //SANDWICH_DEFINED
