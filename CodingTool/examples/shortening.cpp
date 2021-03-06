/*!
  \file shortening.cpp
  \author Tomislav Nad, Tomislav.Nad@iaik.tugraz.at
  \version 0.9
  \brief Example for finding code words producing a collison.
*/
// Copyright (c) 2010 Graz University of Technology (IAIK) <http://www.iaik.tugraz.at>
//  
// This file is part of the CodingTool.
//
// The CodingTool is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CodingTool is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CodingTool.  If not, see <http://www.gnu.org/licenses/>.

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>


#include "LowWeightSearch.h"
#include "InputHandler.h"
#include "types.h"

using namespace std;

//! Rotate a 32-bit word to the right.
# define ROTR(w,x) ((((w) & 0xFFFFFFFF) >> (x))|((w) << (32 - (x))))
//! Rotate a 32-bit word to the left.
# define ROTL(w,x) (((w) << (x))|(((w) & 0xFFFFFFFF) >> (32 - (x))))

void SHA1ME(uint32_t * m);

/*! \example shortening.cpp
    This is an example how to use code shortening
	to force specific bits of a code word to be zero.

	It reads a matrix from a file and forces the last 32 bits
	to zero. The last word should be zero if the input matrix
	was previously generated by sha1me.cpp.

	\see search.cpp
	\see sha1me.cpp
*/
int main(int argc, const char* argv[]) {

	// create an empty generator matrix
	CodeMatrix oGenerator;
	// create an empty code word
	CodeWord oCodeWord;
	// create parameters
	Parameters oParameters;
	// create an input handler
	InputHandler oInputHandler(oParameters);
	// create the low weight search object
	LowWeightSearch oLowWS;

	string sCMFile = "";
	
	// parse the command line arguments
	// example: ./shortening -i 100 -cm sha1me.cm
	if(oInputHandler.ParseSettings(argc, argv))
		exit(-1);
	
	// get the file name of the code matrix
	sCMFile = oParameters.GetStringParameter(Parameters::CMFILE);

	// read data from the file
	oGenerator.ReadFromFile(sCMFile);

	// force the last 32 bits to zero;
	vector<uint64_t> vForceZero;
	for(uint32_t i = 0; i < 32; i++)
		vForceZero.push_back(oGenerator.GetColumns()-32+i);
	oGenerator = LowWeightSearch::CodeShortening(oGenerator,vForceZero);

	// start the search
	oCodeWord = oLowWS.CanteautChabaud(oGenerator,oParameters);

	// print the code word and the Hamming weight
	oCodeWord.Print64();
	cout << "Hamming weight is " << oCodeWord.GetHammingWeight() << endl;

	// if input matrix was SHA-1 message expansion:
	uint32_t m[60];
	// first 512 bits of the code word are input
	for(uint32_t j = 0; j<16; j++)
		m[j] = oCodeWord.At32(j);

	SHA1ME(m);
	
	// this should output 0
	cout << "last word = " << m[59] << endl;
	exit(1);
}

void SHA1ME(uint32_t * m) {

	// SHA-1 message expansion for the last 60 words
	for(uint32_t j = 16; j<60; j++)
		m[j] = ROTL((m[j-3] ^ m[j-8] ^ m[j-14] ^ m[j-16]),1);
}