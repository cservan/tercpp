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
/*
 * Generic hashmap manipulation functions
 */
#ifndef __HASHMAP_H__
#define __HASHMAP_H__
#include <boost/functional/hash.hpp>
#include "stringHasher.h"
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <locale>

using namespace std;

namespace HashMapSpace
{
    class hashMap
    {
        private:
            vector<stringHasher> m_hasher;

        public:
//     ~hashMap();
            long hashValue ( string key );
            int trouve ( long searchKey );
            int trouve ( string key );
            void addHasher ( string key, string value );
            stringHasher getHasher ( string key );
            string getValue ( string key );
            string searchValue ( string key );
            void setValue ( string key , string value );
            void printHash();
            vector<stringHasher> getHashMap();
            string printStringHash();
            string printStringHash2();
            string printStringHashForLexicon();
    };


}


#endif
