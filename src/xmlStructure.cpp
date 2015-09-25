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
#include "xmlStructure.h"

// The following class defines a hash function for strings


using namespace std;

namespace TERCpp
{

    // tutorial demo program

// ----------------------------------------------------------------------
// STDOUT dump and indenting utility functions
// ----------------------------------------------------------------------
// const unsigned int NUM_INDENTS_PER_SPACE=2;

    xmlStructure::xmlStructure()
    {
        NUM_INDENTS_PER_SPACE = 2;
    }

    const char * xmlStructure::getIndent ( unsigned int numIndents )
    {
        static const char * pINDENT = "                                      + ";
        static const unsigned int LENGTH = strlen ( pINDENT );
        unsigned int n = numIndents * NUM_INDENTS_PER_SPACE;
        if ( n > LENGTH )
            n = LENGTH;

        return &pINDENT[ LENGTH-n ];
    }

// same as getIndent but no "+" at the end
    const char * xmlStructure::getIndentAlt ( unsigned int numIndents )
    {
        static const char * pINDENT = "                                        ";
        static const unsigned int LENGTH = strlen ( pINDENT );
        unsigned int n = numIndents * NUM_INDENTS_PER_SPACE;
        if ( n > LENGTH )
            n = LENGTH;

        return &pINDENT[ LENGTH-n ];
    }

    int xmlStructure::dump_attribs_to_stdout ( TiXmlElement* pElement, unsigned int indent )
    {
        if ( !pElement )
            return 0;

        TiXmlAttribute* pAttrib = pElement->FirstAttribute();
        int i = 0;
        int ival;
        double dval;
        const char* pIndent = getIndent ( indent );
        printf ( "\n" );
        while ( pAttrib )
        {
            printf ( "%s%s: value=[%s]", pIndent, pAttrib->Name(), pAttrib->Value() );

            if ( pAttrib->QueryIntValue ( &ival ) == TIXML_SUCCESS )
                printf ( " int=%d", ival );
            if ( pAttrib->QueryDoubleValue ( &dval ) == TIXML_SUCCESS )
                printf ( " d=%1.1f", dval );
            printf ( "\n" );
            i++;
            pAttrib = pAttrib->Next();
        }
        return i;
    }

    void xmlStructure::dump_to_stdout ( TiXmlNode* pParent, unsigned int indent = 0 )
    {
        if ( !pParent )
            return;
// 	cerr << pParent->Value()<< endl;
        TiXmlNode* pChild;
        TiXmlText* pText;
        int t = pParent->Type();
        printf ( "%s", getIndent ( indent ) );
        int num;
        switch ( t )
        {
        case TiXmlNode::DOCUMENT:
            printf ( "Document" );
            break;

        case TiXmlNode::ELEMENT:
            printf ( "Element [%s]", pParent->Value() );
            num = dump_attribs_to_stdout ( pParent->ToElement(), indent + 1 );
            switch ( num )
            {
            case 0:
                printf ( " (No attributes)" );
                break;
            case 1:
                printf ( "%s1 attribute", getIndentAlt ( indent ) );
                break;
            default:
                printf ( "%s%d attributes", getIndentAlt ( indent ), num );
                break;
            }
            break;

        case TiXmlNode::COMMENT:
            printf ( "Comment: [%s]", pParent->Value() );
            break;

        case TiXmlNode::UNKNOWN:
            printf ( "Unknown" );
            break;

        case TiXmlNode::TEXT:
            pText = pParent->ToText();
            printf ( "Text: [%s]", pText->Value() );
            break;

        case TiXmlNode::DECLARATION:
            printf ( "Declaration" );
            break;
        default:
            break;
        }
        printf ( "\n" );
        for ( pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling() )
        {
            dump_to_stdout ( pChild, indent + 1 );
        }
        if ( pChild == 0 )
        {
            /*	    pChild = pParent->NextSibling();
            	    for ( pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling() )
            	    {
            		dump_to_stdout ( pChild, indent + 1 );
            	    }*/
        }
    }

// load the named file and dump its structure to STDOUT
    void xmlStructure::dump_to_stdout ( const char* pFilename )
    {
        TiXmlDocument doc ( pFilename );
        bool loadOkay = doc.LoadFile ( TIXML_ENCODING_UTF8 );
        if ( loadOkay )
        {
            printf ( "\n%s:\n", pFilename );
            dump_to_stdout ( &doc ); // defined later in the tutorial
        }
        else
        {
            printf ( "Failed to load file \"%s\"\n", pFilename );
        }
    }
// Load the file and dump it into a SGMLDocument.
    SGMLDocument xmlStructure::dump_to_SGMLDocument ( string FileName )
    {

        TiXmlDocument doc ( FileName.c_str() );
        SGMLDocument to_return;
        bool isLoaded = doc.LoadFile ( TIXML_ENCODING_UTF8 );
        if ( isLoaded )
        {
// 	    dump_to_stdout (  &doc );
            copy_to_SGMLDocument ( &to_return, &doc, ( unsigned int ) 0 );
        }
        else
        {
            cerr << "ERROR : xmlStructure::dump_to_SGMLDocument : Failed to load file " << FileName << endl;
            exit ( 0 );
        }
        return to_return;
    }

    void xmlStructure::copy_to_SGMLDocument ( SGMLDocument* sgmlDoc, TiXmlNode* pParent, unsigned int indent )
    {
        if ( !pParent )
            return;

        TiXmlNode* pChild;
        TiXmlText* pText;
        int t = pParent->Type();
//         printf ( "%s", getIndent ( indent ) );
//         int num;
        string elementValue;
	bool unknownTag=false;
	if ( xmlParams.debugMode )
        {
            cerr << "DEBUG tercpp : xmlStructure::copy_to_SGMLDocument : " << endl << " TiXmlNode: " << t << endl << "END DEBUG" << endl;
            cerr << "DEBUG tercpp : xmlStructure::copy_to_SGMLDocument : " << endl << " indent: " << indent << endl << "END DEBUG" << endl;
        }
        switch ( t )
        {
        case TiXmlNode::DOCUMENT:

            if ( xmlParams.debugMode )
            {
                cerr << "DEBUG tercpp : xmlStructure::copy_to_SGMLDocument : " << endl << " Document" << endl << "END DEBUG" << endl;
            }//                 printf ( "Document" );
            break;

        case TiXmlNode::ELEMENT:
//                 printf ( "Element [%s]", pParent->Value() );
            elementValue = Tools::lowerCase ( pParent->Value() );

            if ( xmlParams.debugMode )
            {
                cerr << "DEBUG tercpp : xmlStructure::copy_to_SGMLDocument : " << endl << " elementValue: " << elementValue << endl << "END DEBUG" << endl;
            }

            if ( ( ( int ) elementValue.compare ( "refset" ) == 0 ) || ( ( int ) elementValue.compare ( "tstset" ) == 0 ) )
            {
                sgmlDoc->setDocType ( elementValue );
            }
            else
                if (( ( int ) elementValue.compare ( "doc" ) == 0 ) || ( ( int ) elementValue.compare ( "DOC" ) == 0 ))
                {
                    documentStructure tmp_doc;
                    sgmlDoc->addDocument ( tmp_doc );
                }
                else
                    if ( ( ( int ) elementValue.compare ( "seg" ) == 0 ) || ( ( int ) elementValue.compare ( "SEG" ) == 0 ) )
                    {
                        segmentStructure tmp_seg;
                        ( sgmlDoc->getLastDocument() )->addSegments ( tmp_seg );
                    }
		    else
		    {
			unknownTag=true;
		    }

            if ( xmlParams.debugMode )
            {
                cerr << "DEBUG tercpp : xmlStructure::copy_to_SGMLDocument : " << endl << " Calling dump_attribs_to_SGMLDocuments with indent :" << indent + 1 << endl << "END DEBUG" << endl;
            }
            if (!unknownTag)
	    {
		dump_attribs_to_SGMLDocuments ( sgmlDoc, pParent->ToElement(), indent + 1 );
	    }
//                 num = dump_attribs_to_stdout ( pParent->ToElement(), indent + 1 );
//                 switch ( num )
//                 {
//                     case 0:
//                         printf ( " (No attributes)" );
//                         break;
//                     case 1:
//                         printf ( "%s1 attribute", getIndentAlt ( indent ) );
//                         break;
//                     default:
//                         printf ( "%s%d attributes", getIndentAlt ( indent ), num );
//                         break;
//                 }
            break;

//             case TiXmlNode::COMMENT:
//                 printf ( "Comment: [%s]", pParent->Value() );
//                 break;
//
//             case TiXmlNode::UNKNOWN:
//                 printf ( "Unknown" );
//                 break;

        case TiXmlNode::TEXT:
            pText = pParent->ToText();
//                 printf ( "Text: [%s]", pText->Value() );
            if ( indent >= 2 )
            {
                documentStructure * l_tmp_doc = sgmlDoc->getLastDocument();
                segmentStructure * l_tmp_seg = l_tmp_doc->getLastSegments();
                string l_text = pText->Value();
                string line_mod = l_text;
                if ( !xmlParams.tercomLike )
                {
                    if ( xmlParams.debugMode )
                    {
                        cerr << "DEBUG xmlStructure::copy_to_SGMLDocument : line NOT tokenized |" << line_mod << "|" << endl << "END DEBUG" << endl;
                    }
                    if ( xmlParams.debugMode )
                    {
                        cerr << "DEBUG tercpp : xmlStructure::copy_to_SGMLDocument : " << endl << "TERCOM AT FALSE " << endl << "END DEBUG" << endl;
                    }

                    line_mod = tokenizePunct ( line_mod );
                }
                if ( !xmlParams.caseOn )
                {
                    if ( xmlParams.debugMode )
                    {
                        cerr << "DEBUG tercpp : xmlStructure::copy_to_SGMLDocument : " << endl << "CASEON AT FALSE " << endl << "END DEBUG" << endl;
                    }
                    line_mod = lowerCase ( line_mod );
                }
                if ( xmlParams.noPunct )
                {
                    if ( xmlParams.debugMode )
                    {
                        cerr << "DEBUG tercpp : xmlStructure::copy_to_SGMLDocument : " << endl << "NOPUNCT AT TRUE " << endl << "END DEBUG" << endl;
                    }
                    if ( !xmlParams.tercomLike )
                    {
                        line_mod = removePunctTercom ( line_mod );
                    }
                    else
                    {
                        line_mod = removePunct ( line_mod );
                    }
                }
                if ( xmlParams.debugMode )
                {
                    cerr << "DEBUG xmlStructure::copy_to_SGMLDocument : line tokenized |" << line_mod << "|" << endl << "END DEBUG" << endl;
                }
                l_tmp_seg->addContent ( line_mod );
            }
            break;

//             case TiXmlNode::DECLARATION:
//                 printf ( "Declaration" );
//                 break;
        default:
            cerr << "DEBUG tercpp : xmlStructure::copy_to_SGMLDocument : " << endl << "Default TiXmlNode: " << t << endl << "END DEBUG" << endl;
            break;
        }
//         printf ( "\n" );
        for ( pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling() )
        {
            copy_to_SGMLDocument ( sgmlDoc, pChild, indent + 1 );
        }
    }

    void xmlStructure::dump_attribs_to_SGMLDocuments ( SGMLDocument * sgmlDoc, TiXmlElement* pElement, unsigned int indent )
    {
        if ( !pElement )
            return;
        if ( xmlParams.debugMode )
        {
            cerr << "DEBUG xmlStructure::dump_attribs_to_SGMLDocuments : indent " << indent << endl << "END DEBUG" << endl;
        }
        TiXmlAttribute* pAttrib = pElement->FirstAttribute();
//         int i = 0;
//         int ival;
//         double dval;
//         const char* pIndent = getIndent ( indent );
//         printf ( "\n" );
        while ( pAttrib )
        {
            string attribut = pAttrib->Name();
            if ( ( attribut.compare ( "setid" ) == 0 ) || ( attribut.compare ( "srclang" ) == 0 ) || ( attribut.compare ( "trglang" ) == 0 ) )
            {
                if ( attribut.compare ( "setid" ) == 0 )
                {
                    sgmlDoc->setSetId ( pAttrib->Value() );
                }
                if ( attribut.compare ( "srclang" ) == 0 )
                {
                    sgmlDoc->setSrcLang ( pAttrib->Value() );
                }
                if ( attribut.compare ( "trglang" ) == 0 )
                {
                    sgmlDoc->setTgtLang ( pAttrib->Value() );
                }
            }
            else
                if ( ( attribut.compare ( "docid" ) == 0 ) || ( attribut.compare ( "sysid" ) == 0 ) )
                {
                    documentStructure * tmp_doc_bis = sgmlDoc->getLastDocument();
                    if ( attribut.compare ( "docid" ) == 0 )
                    {
                        tmp_doc_bis->setDocId ( pAttrib->Value() );
                    }
                    if ( attribut.compare ( "sysid" ) == 0 )
                    {
                        tmp_doc_bis->setSysId ( pAttrib->Value() );
                    }
                }
                else
                    if ( attribut.compare ( "id" ) == 0 )
                    {
                        documentStructure * l_tmp_doc = sgmlDoc->getLastDocument();
                        segmentStructure * l_tmp_seg = l_tmp_doc->getLastSegments();
                        l_tmp_seg->setSegId ( pAttrib->Value() );
                    }
            pAttrib = pAttrib->Next();
        }
//         return i;
    }


//     std::size_t hashValue(std::string key){}

}

