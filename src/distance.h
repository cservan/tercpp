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
#ifndef __WORD2VECDISTANCE_H__
#define __WORD2VECDISTANCE_H__

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <malloc.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <stdlib.h>

namespace word2vecdistance
{
const long long max_size = 2000;         // max length of strings
const long long N = 40;                  // number of closest words that will be shown
const long long max_w = 50;              // max length of vocabulary entries
using namespace std;
const int vocab_hash_size = 30000000;

struct vocab_word {
  long long cn;
  int *point;
  char *word, *code, codelen;
};

class distance
{
    private:
      FILE *f;
      char *bestw[N];
      char file_name[max_size], st[100][max_size];
      float dist, len, bestd[N];
      long long words, size, a, b, c, d, cn, bi[100];
      char ch;
      float *M;
      char *vocab;
      float *D;
      float *L;
//       struct vocab_word *vocab;
      int *vocab_hash;
//       long long vocab_size;
//       long long vocab_max_size = 1000;
      
      
    public:
//       multimap < string, int > * mvocab;
      distance(string filename);
      vector < pair < string, float > > recherche(string s);
      float getSimilarity(string s1,string s2);
      float getSimilarityTest(string &s1,string &s2);
      float getSimilarity(char * s1,char * s2);
      float getDistance(string s1,string s2);
      float getDistanceTest(string &s1,string &s2);
      float getDistance(char * s1,char * s2);
      bool strcompare(char * c1, char * c2);
      int getWordHash(char *word);
      int searchVocab(char *word);
      void addWordToHash(char *word, int l_pos);
      void fillHash();
};
}      
#endif

