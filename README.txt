tercpp Version 0.7.1


- predicate :
	* boost library
	* cmake
	* autoconf
	* automake

- to compile and install :

modify the cbuild.sh and set the -DCMAKE_INSTALL_PREFIX= to the directory you want the tercpp and library be installed (i.e. : /usr).
Then run ./cbuild.sh to install


tercpp [--tercom] [--sgml] [--debugMode] [--noTxtIds] [--printAlignments] [-s|-c] [-P] -r ref[,ref2...] -h hyp :

                 --tercom       : to use the tercom standart normalization
                 --noTxtIds     : you don't have to add ids at the end of sentences
                 --sgml         : to score with sgml files (incompatible with --noTxtIds and plain text files)
                 --debugMode    : print debug messages
                 -s or -c       : to be case sensitive
                 -P             : do not take account of punctuation
                 --help         : print this help message.
		 --printAlignments : print all the final alignements in a separate output file
		 --verbose	: print progress of TER calculation

the ref and hyp file format with ids is like this :
==================
my sentence (1)
other sentence (2)
...
==================
if you don't want to add some ids use the switch --noTxtIds.

you can submit multiple reference files by separating them with comma.


at last, to use it simply run tercpp

Enjoy !!!

Christophe
(christophe.servan@gmail.com)


NB:
To reference tercpp in your publications, please cite this article:

@article{servanPBML2011,        
  title={Optimising multiple metrics with mert},
  author={Servan, Christophe and Schwenk, Holger}, 
  journal={The Prague Bulletin of Mathematical Linguistics},
  volume={96},
  number={1},
  pages={109--117},
  year={2011},
  publisher={Versita}
}

This code is based on the Snover's algorithm provided at http://www.cs.umd.edu/~snover/tercom

References : 
Matthew Snover, Bonnie Dorr, Richard Schwartz, Linnea Micciulla and John Makhoul, "A Study of Translation Edit Rate with Targeted Human Annotation," Proceedings of Association for Machine Translation in the Americas, 2006. 
Matthew Snover, Bonnie J. Dorr, Richard Schwartz, John Makhoul, Linnea Micciulla and Ralph Weischedel, "A Study of Translation Error Rate with Targeted Human Annotation," LAMP-TR-126, CS-TR-4755, UMIACS-TR-2005-58, University of Maryland, College Park, MD July, 2005.
