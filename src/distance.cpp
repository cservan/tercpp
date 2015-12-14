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
#include "distance.h"
using namespace std;
namespace word2vecdistance
{

  distance::distance(string filename)
  {
    strcpy(file_name, filename.c_str());
    f = fopen(file_name, "rb");
    if (f == NULL) 
    {
      printf("Word2Vec Model file not found\n");
      exit(-1);
    }
    fscanf(f, "%lld", &words);
    fscanf(f, "%lld", &size);
    vocab = (char *)malloc((long long)words * max_w * sizeof(char));
    for (a = 0; a < N; a++) bestw[a] = (char *)malloc(max_size * sizeof(char));
    M = (float *)malloc((long long)words * (long long)size * sizeof(float));
//     D = (float *)malloc((long long)words * (long long)words * sizeof(float));
//     L = (float *)malloc((long long)words * sizeof(float));
    if (M == NULL) 
    {
      printf("Cannot allocate memory: %lld MB    %lld  %lld\n", (long long)words * size * sizeof(float) / 1048576, words, size);
      exit(-1);
    }
    cerr << "Loading word2vec model...";
//     printf("Allocation of memory: %lld MB    %lld  %lld\n", (long long)words * size * sizeof(float) / 1048576, words, size);
    for (b = 0; b < words; b++) 
    {
      a = 0;
      while (1) 
      {
	vocab[b * max_w + a] = fgetc(f);
	if (feof(f) || (vocab[b * max_w + a] == ' ')) break;
	if ((a < max_w) && (vocab[b * max_w + a] != '\n')) a++;
      }
      vocab[b * max_w + a] = 0;
      for (a = 0; a < size; a++) fread(&M[a + b * size], sizeof(float), 1, f);
      len = 0;
      for (a = 0; a < size; a++) len += M[a + b * size] * M[a + b * size];
      len = sqrt(len);
      for (a = 0; a < size; a++) M[a + b * size] /= len;
    }
    fclose(f);
    
//     mvocab = new multimap < string, int >;
//     for (b = 0; b < words; b++) 
//     {
// // 	pair < string, int > p(string(&vocab[b * max_w]),b);
//  	mvocab->insert(make_pair < string, int > (string(&vocab[b * max_w]),b));
//     }
    
//     for (b = 0; b < words; b++) 
//     {
// 	len = 0;
// 	for (a = 0; a < size; a++) len += M[a + b * size] * M[a + b * size];
// 	L[b] = sqrt(len);
//     }
//     for (b = 0; b < words; b++) 
//     {
// 	for (c = 0; c <= b; c++) 
// 	{
// 	    D[b*c] = 0.0;
// 	    for (a = 0; a < size; a++) 
// 	    {
// 		D[b*c] += (M[a + b * size] / L[b]) * (M[a + c * size] / L[c]);
// 	    }
// 	}
//     }

//     cerr << mvocab->size() <<endl;
    fillHash();
    cerr << "finished!" <<endl;
  }


  vector< pair< string, float > > distance::recherche(string s)
  {
    vector< pair< string, float > > to_return;
    float vec[max_size];
    for (a = 0; a < N; a++) bestd[a] = 0;
    for (a = 0; a < N; a++) bestw[a][0] = 0;
//     printf("Enter word or sentence (EXIT to break): ");
    a = 0;
//     while (1) {
//       st1[a] = fgetc(stdin);
//       if ((st1[a] == '\n') || (a >= max_size - 1)) {
//         st1[a] = 0;
//         break;
//       }
//       a++;
//     }
    char st1[max_size];
    strcpy(st1, s.c_str());
//     st1 = s.c_str();
//     if (!strcmp(st1, "EXIT")) break;
    cn = 0;
    b = 0;
    c = 0;
    while (1) {
      st[cn][b] = st1[c];
      b++;
      c++;
      st[cn][b] = 0;
      if (st1[c] == 0) break;
      if (st1[c] == ' ') {
        cn++;
        b = 0;
        c++;
      }
    }
    cn++;
    for (a = 0; a < cn; a++) {
      for (b = 0; b < words; b++) if (!strcmp(&vocab[b * max_w], st[a])) break;
      if (b == words) b = -1;
      bi[a] = b;
//       printf("\nWord: %s  Position in vocabulary: %lld\n", st[a], bi[a]);
      if (b == -1) {
// 	printf("%s: Out of dictionary word!\n",st[a]);
        return to_return;
      }
    }
    if (b == -1) return to_return;
//     printf("\n                                              Word       Cosine distance\n------------------------------------------------------------------------\n");
    for (a = 0; a < size; a++) vec[a] = 0;
    for (b = 0; b < cn; b++) {
      if (bi[b] == -1) continue;
      for (a = 0; a < size; a++) vec[a] += M[a + bi[b] * size];
    }
    len = 0;
    for (a = 0; a < size; a++) len += vec[a] * vec[a];
    len = sqrt(len);
    for (a = 0; a < size; a++) vec[a] /= len;
    for (a = 0; a < N; a++) bestd[a] = -1;
    for (a = 0; a < N; a++) bestw[a][0] = 0;
    for (c = 0; c < words; c++) {
      a = 0;
      for (b = 0; b < cn; b++) if (bi[b] == c) a = 1;
      if (a == 1) continue;
      dist = 0;
      for (a = 0; a < size; a++) dist += vec[a] * M[a + c * size];
      for (a = 0; a < N; a++) {
        if (dist > bestd[a]) {
          for (d = N - 1; d > a; d--) {
            bestd[d] = bestd[d - 1];
            strcpy(bestw[d], bestw[d - 1]);
          }
          bestd[a] = dist;
          strcpy(bestw[a], &vocab[c * max_w]);
          break;
        }
      }
    }
    
//       for (a = 0; a < N && a < 10 ; a++) 
// 	printf("%50s\t\t%f\n", bestw[a], bestd[a]);
    for (a = 0; a < N; a++) 
    {
      pair<string,float> l_p( string(bestw[a]), bestd[a]);
//       printf("%50s\t\t%f\n", bestw[a], bestd[a]);
      to_return.push_back(l_p);
    }
    return to_return;
  }
/*  float distance::getSimilarity(string s1, string s2)
  {
    float vec1[max_size];
    float vec2[max_size];
  //     vector< pair< string, float > > to_return;
      for (a = 0; a < N; a++) bestd[a] = 0;
      for (a = 0; a < N; a++) bestw[a][0] = 0;
//       printf("Enter word or sentence (EXIT to break): ");
      a = 0;
      char st1[max_size];
      char st2[max_size];
      strcpy(st1, s1.c_str());
      strcpy(st2, s2.c_str());
      cn = 0;
      b = 0;
      c = 0;
      int pos1, pos2;
      // supression des espaces 
      while (1) {
	st[cn][b] = st1[c];
	b++;
	c++;
	st[cn][b] = 0;
	if (st1[c] == 0) break;
	if (st1[c] == ' ') {
	  cn++;
	  b = 0;
	  c++;
	}
      }
      cn++;
      // supression des espaces 
      for (a = 0; a < cn; a++) {
	for (b = 0; b < words; b++) if (!strcmp(&vocab[b * max_w], st[a])) break;
	if (b == words) b = -1;
	bi[a] = b;
// 	printf("\nWord: %s  Position in vocabulary: %lld\n", st[a], bi[a]);
	pos1=bi[a];
	if (b == -1) {
// 	  printf("%s: Out of dictionary word!\n",st[a]);
	  return 0.0;
	}
      }
      // Verification de la présence dans le vocab
      cn = 0;
      b = 0;
      c = 0;
      // supression des espaces 
      while (1) {
	st[cn][b] = st2[c];
	b++;
	c++;
	st[cn][b] = 0;
	if (st2[c] == 0) break;
	if (st2[c] == ' ') {
	  cn++;
	  b = 0;
	  c++;
	}
      }
      cn++;
      for (a = 0; a < cn; a++) {
//       cerr << a << " " << cn << endl;
	for (b = 0; b < words; b++) if (!strcmp(&vocab[b * max_w], st[a])) break;
	if (b == words) b = -1;
	bi[a] = b;
	pos2=bi[a];
// 	printf("\nWord: %s  Position in vocabulary: %lld\n", st[a], bi[a]);
	if (b == -1) {
// 	  printf("%s: Out of dictionary word!\n",st[a]);
	  return 0.0;
	}
      }
      
      if (b == -1) return 0.0;
  //     printf("\n                                              Word       Cosine distance\n------------------------------------------------------------------------\n");
      for (a = 0; a < size; a++) vec1[a] = 0;
      for (b = 0; b < cn; b++) {
	if (pos1 == -1) continue;
	for (a = 0; a < size; a++) vec1[a] += M[a + pos1 * size];
      }
      len = 0;
      for (a = 0; a < size; a++) len += vec1[a] * vec1[a];
      len = sqrt(len);
      for (a = 0; a < size; a++) vec1[a] /= len;

      for (a = 0; a < size; a++) vec2[a] = 0;
      for (b = 0; b < cn; b++) {
	if (pos2 == -1) continue;
	for (a = 0; a < size; a++) vec2[a] += M[a + pos2 * size];
      }
      len = 0;
      for (a = 0; a < size; a++) len += vec2[a] * vec2[a];
      len = sqrt(len);
      for (a = 0; a < size; a++) vec2[a] /= len;
      dist = 0;
      for (a = 0; a < size; a++) dist += vec1[a] * vec2[a];
      return dist;

//       for (a = 0; a < N; a++) bestd[a] = -1;
//       for (a = 0; a < N; a++) bestw[a][0] = 0;
//       for (c = 0; c < words; c++) {
// 	a = 0;
// 	for (b = 0; b < cn; b++) if (bi[b] == c) a = 1;
// 	if (a == 1) continue;
// 	dist = 0;
// 	for (a = 0; a < size; a++) dist += vec[a] * M[a + c * size];
// 	for (a = 0; a < N; a++) {
// 	  if (dist > bestd[a]) {
// 	    for (d = N - 1; d > a; d--) {
// 	      bestd[d] = bestd[d - 1];
// 	      strcpy(bestw[d], bestw[d - 1]);
// 	    }
// 	    bestd[a] = dist;
// 	    
// 	    strcpy(bestw[a], &vocab[c * max_w]);
// 	    break;
// 	  }
// 	}
//       }
//       for (a = 0; a < N && a < 2 ; a++) 
//       {
// 	pair<string,float> l_p( string(bestw[a]), bestd[a]);
// 	printf("%50s\t\t%f\n", bestw[a], bestd[a]);
//       }
//       return 1.0;
  }*/
  float distance::getSimilarityTest(string &s1, string &s2)
  {
//       float vec1[max_size];
//       float vec2[max_size];
//       float len1=0;
//       float len2=0;
      char st1[max_w];
      char st2[max_w];
      strcpy(st1, s1.c_str());
      strcpy(st2, s2.c_str());
      int pos1 = -1; 
      int pos2 = -1;
      b = 0;
      for (b = 0; b < words; b++)
      {
	if (!strcmp(&vocab[b * max_w], st1)) 
	{
	  pos1=b;
	  break;
	}
      }
      if (pos1 == -1) 
      {
	return 0.0;
      }
      for (c = 0; c < words; c++)
      {
	if (!strcmp(&vocab[c * max_w], st2)) 
	{
	  pos2=c;
	  break;
	}
      }
      if (pos2 == -1) 
      {
	return 0.0;
      }
      dist = 0;
      for (a = 0; a < size; a++) 
      {
	  dist += M[a + pos1 * size] * M[a + pos2 * size] ;
      }
      return dist;
  }
  float distance::getSimilarity(string &s1, string &s2)
  {
      char st1[max_w];
      char st2[max_w];
      strcpy(st1, s1.c_str());
      strcpy(st2, s2.c_str());
      return getSimilarity(st1,st2);
  }
  float distance::getSimilarityProb(string &s1, string &s2)
  {
      char st1[max_w];
      char st2[max_w];
      strcpy(st1, s1.c_str());
      strcpy(st2, s2.c_str());
      return getSimilarityProb(st1,st2);
  }

  float distance::getSimilarity(char * st1, char * st2)
  {
      int pos1 = -1; 
      int pos2 = -1;
      b = 0;
      pos1 = searchVocab(st1);
      if (pos1 == -1) 
      {
	return 0.0;
      }
      pos2 = searchVocab(st2);
      if (pos2 == -1) 
      {
	return 0.0;
      }
      dist = 0;
      if (pos1 == pos2)
      {
	  return 1.0;
      }
      for (a = 0; a < size; a++) 
      {
	  dist += M[a + pos1 * size] * M[a + pos2 * size] ;
      }
      return dist;
  }
  float distance::getSimilarityProb(char * st1, char * st2)
  {
      return (1.0+getSimilarity(st1,st2))/2.0;
  }
  float distance::getDistance(char * st1, char * st2)
  {
      return (1.0-getSimilarity(st1,st2));
  }
  float distance::getDistance(string& st1, string& st2)
  {
      return (1.0-getSimilarity(st1,st2));
  }
 
  float distance::getDistanceProb(char * st1, char * st2)
  {
      return (1.0-getSimilarityProb(st1,st2));
  }
  float distance::getDistanceProb(string& st1, string& st2)
  {
      return (1.0-getSimilarityProb(st1,st2));
  }
 
  
  bool distance::strcompare(char* c1, char* c2)
  {
      int l1 = strlen(c1);
      if (l1 != (int)strlen(c2))
      {
	  return false;
      }
      for (int i = 0; i < l1; i++)
      {
	  if (c1[i] != c2[i]) return false;
      }
      return true;
  }
        /* Returns hash value of a word */
  int distance::getWordHash(char *word) 
  {
      unsigned long long a, hash = 0;
      for (a = 0; a < strlen(word); a++) hash = hash * 257 + word[a];
      hash = hash % vocab_hash_size;
      return hash;
  }

  /* Returns position of a word in the vocabulary; if the word is not found, 
  * returns -1 */
  int distance::searchVocab(char *word) 
  {
      unsigned int hash = getWordHash(word);
      int l_b;
      while (1) 
      {
	  if (vocab_hash[hash] == -1) return -1;
// 	  if (!strcmp(word, vocab[vocab_hash[hash]].word)) return vocab_hash[hash];
	  l_b = vocab_hash[hash];
	  if (!strcmp(&vocab[l_b * max_w], word)) return vocab_hash[hash];
	  hash = (hash + 1) % vocab_hash_size;
      }
      return -1;
  }
  /* Adds a word to the vocabulary */
  void distance::addWordToHash(char *word, int l_pos) {    
    unsigned int hash = getWordHash(word);
    while (vocab_hash[hash] != -1) hash = (hash + 1) % vocab_hash_size;
    vocab_hash[hash] = l_pos;
  }
  void distance::fillHash()
  {
      int c;
      vocab_hash = (int*)calloc(vocab_hash_size, sizeof(int));
      for (c = 0; c < vocab_hash_size; c++)
      {
	  vocab_hash[c] = -1;
      }
      for (c = 0; c < words; c++)
      {
	 addWordToHash(&vocab[c * max_w], c);
      }
  }

}