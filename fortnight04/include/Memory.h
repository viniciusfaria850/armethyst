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

#pragma once

#include <string>

class Memory
{
	public:
		virtual void loadBinary(std::string filename) = 0;
		virtual void writeBinaryAsText (std::string basename) = 0;

		/**
		 * L� uma instru��o de 32 bits considerando um endere�amento
		 * em bytes.
		 */
		virtual uint32_t readInstruction32(uint64_t address) = 0;

		/**
		 * L� um dado de 32 bits considerando um endere�amento em bytes.
		 */
		virtual uint32_t readData32(uint64_t address) = 0;
		
		/**
		 * L� um dado de 64 bits considerando um endere�amento em bytes.
		 */
		virtual uint64_t readData64(uint64_t address) = 0;
		
		/**
		 * Escreve uma instru��o de 32 bits considerando um
		 * endere�amento em bytes.
		 */
		virtual void writeInstruction32(uint64_t address, uint32_t value) = 0;

		/**
		 * Escreve um dado (value) de 32 bits considerando um endere�amento em bytes.
		 */
		virtual void writeData32(uint64_t address, uint32_t value) = 0;

		/**
		 * Escreve um dado (value) de 64 bits considerando um endere�amento em bytes.
		 */
		virtual void writeData64(uint64_t address, uint64_t value) = 0;


};

