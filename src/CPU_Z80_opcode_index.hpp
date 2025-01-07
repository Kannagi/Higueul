/*
 * Copyright (c) 2024 Emmanuel Rousseau
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

// Forward Declarations --------------------------------------------------------
class CPU_Z80;
class Z80Evaluable;

// Types -----------------------------------------------------------------------
typedef std::string (*translator_fn)(CPU_Z80 &cpu, Z80Evaluable &dst,
									 Z80Evaluable &src);

// Z80OpcodeIndex keeps an index of all possible Z80 operations related to
// translator functions.
class Z80OpcodeIndex
{
	public:
		translator_fn get_translator(int64_t key);
		void index(OpType type, OpFlag dst, OpFlag src, translator_fn f);

		uint64_t get_size(void) { return table.size(); }

	private:
		std::map<int64_t, translator_fn> table;
};

int64_t make_z80_opkey(OpType type, Z80Evaluable &dst, Z80Evaluable &src);
uint64_t get_z80_optype_from_key(int64_t key);
std::string explain_key(uint64_t flags);