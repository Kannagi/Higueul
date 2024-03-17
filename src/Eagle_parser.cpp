#include <vector>
#include <iostream>
#include <map>
#include <fstream>
#include <cstring>
#include <string>

#include <stdint.h>

#include "Eagle.hpp"

bool Eagle::isAlphabetic(char c)
{
	return ( (c >= 'a') && (c <= 'z') ) || ( (c >= 'A') && (c <= 'Z') ) || (c == '_');
}


bool Eagle::isWord(char c)
{
	return ( (c >= 'a') && (c <= 'z') ) || ( (c >= 'A') && (c <= 'Z') ) || ( (c >= '0') && (c <= '9') ) || (c == '_') || (c == '.');
}

bool Eagle::isOperator(char c,char c2,int &i)
{
	if(
	( (c == '<') && (c2 == '<') ) || ( (c == '>') && (c2 == '>') ) || ( (c == '<') && (c2 == '=') ) || ( (c == '>') && (c2 == '=') ) ||
	( (c == '+') && (c2 == '=') ) || ( (c == '-') && (c2 == '=') ) || ( (c == '=') && (c2 == '=') ) || ( (c == '!') && (c2 == '=') ) ||
	( (c == '&') && (c2 == '=') ) || ( (c == '|') && (c2 == '=') ) || ( (c == '^') && (c2 == '=') ) || ( (c == '?') && (c2 == '!') ) ||
	( (c == '&') && (c2 == '!') ) || ( (c == '?') && (c2 == '=') )
	)
	{
		i++;
		return true;
	}

	return (c == '+') || (c == '-') || (c == '/') || (c == '*') || (c == '=') || (c == '&') || (c == '|') || (c == '^')  || (c == '<')  || (c == '>')  || (c == '?') || (c == '~');
}

bool Eagle::isNumber(char c)
{
	return ( (c >= '0') && (c <= '9') );
}


int Eagle::isConstantFolding(char c1,int i,EAGLE_WORDS &tword)
{
	if(c1 == '(')
	{
		tword.item.clear();
		tword.type = TYPE_CONSTANT;

		tword.line = this->line;
		tword.col  = this->col;
		EAGLE_DEFINE tdefine;
		std::string word;
		//tword.item += '(';

		i++;
		while(this->filetext[i] != 0)
		{
			char letter = this->filetext[i];
			if(letter == ')')
			{
				if(word.size() != 0)
				{
					tdefine = this->define[word];
					if(tdefine.exist == true)
						word = tdefine.text;


					if(tmacro.exist == true)
					{
						for(int l = 0;l < 8;l++)
						{
							if(this->kmacro[l] == word)
							{
								word = tmacro.arg[l];
								l = 8;
							}
						}
					}

				}

				tword.item += word;
				return i;
			}
			else
			{

				if(isWord(letter))
				{
					word += letter;

				}
				else
				{
					if(word.size() != 0)
					{
						tdefine = this->define[word];
						if(tdefine.exist == true)
							word = tdefine.text;

						if(tmacro.exist == true)
						{
							for(int l = 0;l < 8;l++)
							{
								if(this->kmacro[l] == word)
								{
									word = tmacro.arg[l];
									l = 8;
								}
							}
						}


						tword.item += word + letter;
						word = "";
					}else
						tword.item += letter;
				}

			}

			i++;
		}
	}

	return i;
}

int Eagle::isText(char c1,int i,EAGLE_WORDS &tword)
{
	if(c1 == '"')
	{
		tword.item.clear();
		tword.type = TYPE_DATASTR;

		tword.line = this->line;
		tword.col  = this->col;
		i++;
		while(this->filetext[i] != 0)
		{
			if(this->filetext[i] == '"')
				return i;
			else
				tword.item += this->filetext[i];

			this->col++;
			if(this->filetext[i] == '\n')
			{
				this->line++;
				this->col = 0;
			}


			i++;
		}
	}

	return i;
}

int Eagle::isComment(char c1,char c2,int i)
{

	if( (c1 == '/') && (c2 == '/') )
	{
		while(this->filetext[i] != 0)
		{
			if(this->filetext[i] == '\n')
			{

				this->col = 0;
				return i;
			}


			i++;
		}
	}

	if( (c1 == '/') && (c2 == '*') )
	{
		while(this->filetext[i] != 0)
		{
			if( (this->filetext[i] == '*') && (this->filetext[i+1] == '/') )
				return i+2;

			this->col++;
			if(this->filetext[i] == '\n')
			{
				this->line++;
				this->col = 0;
			}

			i++;
		}
	}

	return i;
}

void Eagle::parser_word()
{
	std::vector<EAGLE_WORDS> words;
	EAGLE_WORDS tword,tword2,ptword;
	tword2.type = TYPE_UNK;
	int scope = 0;

	std::string word = "";

	bool label = false,endinst = false,bptr = false;

	int i = 0,iptr = 0;
	int preproc = 0;
	int parg = 0;
	std::string dword = "";
	EAGLE_DEFINE tdefine,tdefine2;
	tdefine.exist = true;
	char token = 0;
	tmacro.exist = false;

	this->line = 1;
	this->col = 0;
	this->instructions.clear();

	while(this->filetext[i] != 0)
	{

		i = isComment(this->filetext[i],this->filetext[i+1],i);

		if(preproc == -1)
		{
			label_macro:
			tmacro = tdefine;
			tmacro.exist = true;
			i-=1;
			this->filetext.insert(i,tdefine2.text);
			preproc = 6;
		}

		if(preproc == -1)
		{
			label_define:
			i-=1;
			this->filetext.insert(i,tdefine2.text);
			if(preproc != 6)
				preproc = 0;
		}

		if( (preproc == 0) || (preproc == 6) )
		{
			i = isText(this->filetext[i],i,tword2);

			if(tword2.type == TYPE_DATASTR)
			{
				tword2.scope = scope;
				if(bptr == false)
					words.push_back(tword2);
				tword2.type = TYPE_UNK;
			}

			i = isConstantFolding(this->filetext[i],i,tword2);

			if(tword2.type == TYPE_CONSTANT)
			{
				tword2.scope = scope;
				if(bptr == false)
					words.push_back(tword2);
				tword2.type = TYPE_UNK;
			}
		}

		char letter  = this->filetext[i];
		char letter2 = this->filetext[i+1];
		i++;

		//Define/Macro content
		if( (preproc == 2) || (preproc == 4))
			tdefine.text += letter;

		if(isWord(letter))
		{
			word += letter;
		}
		else
		{
			if(word.size() != 0)
			{

				//define/macro == word
				if(preproc == 6)
				{
					for(int l = 0;l < 8;l++)
					{
						if(this->kmacro[l] == word)
						{
							word = tdefine.arg[l];
							l = 8;
						}
					}
				}

				if(preproc == 5)
				{/*
					if(token == '#')
						word = token + word;

					if(token == '$')
						word = token + word;

					if(token == '@')
						word = token + word;

					if(letter == ':')
						word +=  letter;
*/
					tdefine.arg[parg] = word;

					parg++;
					parg &=7;
				}

				if( (preproc == 0) || (preproc == 6) )
				{
					tdefine2 = this->define[word];
					if(tdefine2.exist == true)
					{
						if(tdefine2.macro == false)
						{
							word = "";
							goto label_define;
						}else
						{
							preproc = 5;
							parg = 0;
						}
					}
				}

				tword.item = word;
				if(isNumber(word[0]))
				{
					tword.type = TYPE_NUMBER;
				}
				else
				{
					tword.type = TYPE_WORD;


					if(letter == ':')
					{
						tword.type = TYPE_LABEL;
						endinst = true;
						label = true;
					}
				}

				if(preproc == 3)
				{
					dword = word;
					preproc = 4;
					tdefine.text = "";
					tdefine.macro = true;
				}

				if(preproc == 1)
				{
					dword = word;
					preproc = 2;
					tdefine.text = "";
					tdefine.macro = false;
				}
				//----

				EAGLE_keywords tmp = this->keywords[word];
				if( (tmp >= EAGLE_keywords::DEFINE) && (tmp <= EAGLE_keywords::ORG))
					endinst = true;

				if( (tmp >= EAGLE_keywords::IF) && (tmp <= EAGLE_keywords::RETURN))
					endinst = true;

				if(tmp == EAGLE_keywords::DEFINE)
					preproc = 1;

				if(tmp == EAGLE_keywords::MACRO)
					preproc = 3;

				if(tmp == EAGLE_keywords::END)
				{
					tword.type = TYPE_END;
					endinst = true;
				}

				if(bptr == true)
				{
					ptword.ptr[iptr] = word;
					ptword.ptype[iptr] = tword.type;
					ptword.ptoken1[iptr] = token;
					ptword.ptoken2[iptr] = letter;
					iptr++;
					iptr &= 3;
					ptword.pn = iptr;

					if( (tmp >= EAGLE_keywords::UINT8) && (tmp <= EAGLE_keywords::VOID))
						ptword.ktype = tmp;

				}

				tword.line = this->line;
				tword.col  = this->col-tword.item.size();

				tword.scope = scope;
				tword.label = label;
				label = false;
				tword.token1 = token;
				tword.token2 = letter;

				if(tmp == EAGLE_keywords::ENDMACRO)
				{
					tmacro.exist = false;
					if(preproc == 4)
					{
						this->define[dword] = tdefine;
						preproc = 0;
					}

					if(preproc == 6)
					{
						preproc = 0;
						this->line--;
					}
				}else
				{
					if( (preproc == 0) || (preproc == 6) )
					{
						if(bptr == false)
							words.push_back(tword);
					}

				}

			}

			token = letter;

			if( (preproc == 0) || (preproc == 6) )
			if(this->isOperator(letter,letter2,i))
			{
				word = letter;
				word += letter2;
				tword.item = word;
				tword.type = TYPE_OPERATOR;
				tword.line = this->line;
				tword.col  = this->col-1;

				tword.scope = scope;
				if(bptr == false)
					words.push_back(tword);
			}

			if( (letter == ';') || ((letter == '\n') && (endinst == true)) )
			{
				if(words.size() != 0)
				{
					this->instructions.push_back(words);
					words.clear();
				}

				endinst = false;
			}

			if(letter == '[')
			{
				ptword.line = this->line;
				ptword.col  = this->col;
				ptword.type = TYPE_PTR;
				ptword.scope = scope;
				ptword.label = false;
				iptr = 0;

				ptword.ptype[0] = TYPE_UNK;
				ptword.ptype[1] = TYPE_UNK;
				ptword.ptype[2] = TYPE_UNK;
				ptword.ptype[3] = TYPE_UNK;

				ptword.ktype = EAGLE_keywords::UINT8;
				ptword.pn = 0;

				bptr = true;
			}

			if(letter == ']')
			{
				ptword.item = ".ptr";
				words.push_back(ptword);
				bptr = false;
			}

			if( (preproc != 2) && (preproc != 4) )
			{
				if(letter == '{')
					scope++;


				if(letter == '}')
				{
					scope--;
					if(scope >= 0)
					{
						if(words.size() != 0)
						{
							this->instructions.push_back(words);
							words.clear();
						}

						tword.item = ".end";
						tword.type = TYPE_END;
						tword.line = this->line;
						tword.col  = this->col;

						tword.scope = scope;
						tword.label = label;
						words.push_back(tword);
						this->instructions.push_back(words);
						words.clear();

						endinst = false;
					}else
					{
						std::cout << "error line " << this->line  << ": extra singleton }\n" ;
						this->error++;
					}
				}
			}




			word = "";
		}


		this->col++;


		if(letter == '\n')
		{
			if(preproc == 5)
			{
				word = "";
				goto label_macro;
			}

			if(preproc != 6)
			{
				this->line++;
				this->col = 0;
			}
			//define/macro declare
			if(preproc == 2)
			{
				tdefine.text.erase(tdefine.text.size() - 1);
				this->define[dword] = tdefine;
				preproc = 0;
				word = "";
			}
		}



	}

}

