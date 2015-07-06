#include "distance.h"
using namespace std;
namespace word2vecdistance
{

  distance::distance(string filename)
  {
    strcpy(file_name, filename.c_str());
    f = fopen(file_name, "rb");
    if (f == NULL) {
      printf("Input file not found\n");
      exit(-1);
    }
    fscanf(f, "%lld", &words);
    fscanf(f, "%lld", &size);
    vocab = (char *)malloc((long long)words * max_w * sizeof(char));
    for (a = 0; a < N; a++) bestw[a] = (char *)malloc(max_size * sizeof(char));
    M = (float *)malloc((long long)words * (long long)size * sizeof(float));
    if (M == NULL) {
      printf("Cannot allocate memory: %lld MB    %lld  %lld\n", (long long)words * size * sizeof(float) / 1048576, words, size);
      exit(-1);
    }
    printf("Allocation of memory: %lld MB    %lld  %lld\n", (long long)words * size * sizeof(float) / 1048576, words, size);
    for (b = 0; b < words; b++) {
      a = 0;
      while (1) {
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
	printf("%s: Out of dictionary word!\n",st[a]);
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
  float distance::getDistance(string s1, string s2)
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
	  printf("%s: Out of dictionary word!\n",st[a]);
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
	  printf("%s: Out of dictionary word!\n",st[a]);
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
  }
}