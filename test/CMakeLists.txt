#################################
# tercpp: an open-source Translation Edit Rate (TER) scorer tool for Machine Translation.
#
# Copyright 2010-2013, Christophe Servan, LIUM, University of Le Mans, France
# Copyright 2015, Christophe Servan, GETALP-LIG, University of Grenoble, France
# Contact: christophe.servan@gmail.com
#
# The tercpp tool and library are free software: you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as published by 
# the Free Software Foundation, either version 2.1 of the licence, or
# (at your option) any later version.
#
# This program and library are distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
# for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this library; if not, write to the Free Software Foundation,
# Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
###################################
PROJECT(tercpp)


include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src)

SET(tercpp_SRCS
   main.cpp
   ${CMAKE_SOURCE_DIR}/src/tools.cpp
   ${CMAKE_SOURCE_DIR}/src/bestShiftStruct.cpp
   ${CMAKE_SOURCE_DIR}/src/xmlStructure.cpp
   ${CMAKE_SOURCE_DIR}/src/alignmentStruct.cpp
   ${CMAKE_SOURCE_DIR}/src/terAlignment.cpp
   ${CMAKE_SOURCE_DIR}/src/stringHasher.cpp
   ${CMAKE_SOURCE_DIR}/src/stringInfosHasher.cpp
   ${CMAKE_SOURCE_DIR}/src/infosHasher.cpp
   ${CMAKE_SOURCE_DIR}/src/hashMap.cpp
   ${CMAKE_SOURCE_DIR}/src/tercalc.cpp
   ${CMAKE_SOURCE_DIR}/src/hashMapInfos.cpp
   ${CMAKE_SOURCE_DIR}/src/hashMapStringInfos.cpp
   ${CMAKE_SOURCE_DIR}/src/terShift.cpp
   ${CMAKE_SOURCE_DIR}/src/segmentStructure.cpp
   ${CMAKE_SOURCE_DIR}/src/tinystr.cpp  
   ${CMAKE_SOURCE_DIR}/src/tinyxml.cpp  
   ${CMAKE_SOURCE_DIR}/src/tinyxmlerror.cpp  
   ${CMAKE_SOURCE_DIR}/src/tinyxmlparser.cpp
   ${CMAKE_SOURCE_DIR}/src/sgmlDocument.cpp
   ${CMAKE_SOURCE_DIR}/src/multiTxtDocument.cpp
   ${CMAKE_SOURCE_DIR}/src/documentStructure.cpp
   ${CMAKE_SOURCE_DIR}/src/multiEvaluation.cpp
   ${CMAKE_SOURCE_DIR}/src/distance.cpp
   ${CMAKE_SOURCE_DIR}/src/bivec.cpp
   ${CMAKE_SOURCE_DIR}/src/word2vec.cpp
)

add_executable(tercpp.0.9.1-bivec ${tercpp_SRCS})
target_link_libraries(tercpp.0.9.1-bivec boost_regex boost_system boost_filesystem boost_iostreams boost_thread boost_serialization rt)
#target_link_libraries(tercpp.0.7.1 tcmalloc)
# target_link_libraries(tercpp tercalc xmlStructure documentStructure multiEvaluation)

install(TARGETS tercpp.0.9.1-bivec DESTINATION bin)

