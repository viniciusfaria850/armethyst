/* ----------------------------------------------------------------------------

    (EN) armethyst - A simple ARM Simulator written in C++ for Computer Architecture
    teaching purposes. Free software licensed under the MIT License (see license
    below).

    (PT) armethyst - Um simulador ARM simples escrito em C++ para o ensino de
    Arquitetura de Computadores. Software livre licenciado pela MIT License
    (veja a licen�a, em ingl�s, abaixo).

    (EN) MIT LICENSE:

    Copyright 2020 Andr� Vital Sa�de

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.

   ----------------------------------------------------------------------------
*/

#include "BasicMemory.h"

#include <iostream>
#include <iomanip>

using namespace std;

BasicMemory::BasicMemory(int size)
{
	data = new char[size];
}

BasicMemory::~BasicMemory()
{
	delete[] data;
}

/**
 * L� uma instru��o de 32 bits considerando um endere�amento em bytes.
 *
 * BasicMemory.cpp implementa a arquitetura de Von Neumman, com apenas uma
 * mem�ria, que armazena instru��es e dados.
 */
uint32_t BasicMemory::readInstruction32(uint64_t address)
{
	return ((uint32_t*)data)[address >> 2];
}

/**
 * L� um dado de 32 bits considerando um endere�amento em bytes.
 */
uint32_t BasicMemory::readData32(uint64_t address)
{
	return ((uint32_t*)data)[address >> 2];
}

/**
 * L� um dado de 64 bits considerando um endere�amento em bytes.
 */
uint64_t BasicMemory::readData64(uint64_t address)
{
	return ((uint64_t*)data)[address >> 3];
}

/**
 * Escreve uma instru��o de 32 bits considerando um
 * endere�amento em bytes.
 */
void BasicMemory::writeInstruction32(uint64_t address, uint32_t value)
{
	((uint32_t*)data)[address >> 2] = value;
}

/**
 * Escreve um dado (value) de 32 bits considerando um endere�amento em bytes.
 */
void BasicMemory::writeData32(uint64_t address, uint32_t value)
{
	((uint32_t*)data)[address >> 2] = value;
}

/**
 * Escreve um dado (value) de 64 bits considerando um endere�amento em bytes.
 */
void BasicMemory::writeData64(uint64_t address, uint64_t value)
{
	((uint64_t*)data)[address >> 3] = value;
}

/**
 * carrega arquivo bin�rio na mem�ria
 */
void BasicMemory::loadBinary(string filename)
{
    streampos size;

    ifstream file(filename, ios::in|ios::binary|ios::ate);
    if (file.is_open())
    {
        fileSize = file.tellg();
        file.seekg (0, ios::beg);
        file.read (data, fileSize);
        file.close();
    }
    else {
        cout << "Unable to open file " << filename << endl;
		cout << "Aborting... " << endl;
		exit(1);
    }

}


/**
 * Escreve arquivo binario em um arquivo leg�vel
 */
#define LINE_SIZE 4
void BasicMemory::writeBinaryAsText (string basename) {
    string filename = "txt_" + basename + ".txt";
    ofstream ofp;
    int i,j;

    cout << "Gerado arquivo " << filename << endl << endl;
    ofp.open(filename);

    ofp << uppercase << hex;

    // caption
    ofp << "ADDR    ";
    for (j=0; j<LINE_SIZE; j++) {
        ofp << "ADDR+" << setfill('0') << setw(2) << 4*j << "  ";
    }
    ofp << endl << "----------------------------------------------------------------------------" << endl;


    // binary
    i=0;
    for (i = 0; i < fileSize / 4; i+=LINE_SIZE) {
        ofp << setw(4) << 4*i << "    ";
        for (j=0; j<LINE_SIZE; j++) {
            ofp << setw(8) << ((unsigned int *)data)[i+j] << " ";
        }
        ofp << endl;
    }
    ofp.close();
}
