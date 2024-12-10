#pragma once

//Variable pointeur
typedef struct
{
	int64_t value; // valeur immediate (si bimm == true)
	EAGLE_keywords type; //le type de variable
	bool bimm; //indique si c'est une valeur immediate
	char token1,token2; // indique les token devant et derriere si y'en a


}EAGLE_VARIABLEP;


//Variable
typedef struct
{
	int64_t immediate; // valeur immediate (si bimm == true)
	double dimmediate; // pareil mais en type double
	uint64_t address; //address si c'est une variable
	uint64_t nsize; // pas certain , mais probablement la taille de l'adresse ?
	EAGLE_keywords type,type2; // type de la variable (actuellement le type2 est inutilisé)
	char token1,token2; // indique les token devant et derriere si y'en a
	bool bimm; //indique si c'est une valeur immediate
	bool blabel; //indique si c'est un label
	bool bptr; //indique si c'est un pointeur
	bool ptr2_exist; //indique si un deuxieme pointeur existe

	// pour ptr1 et ptr2 , c'est pour [0x1000,idx] , ptr1 = 0x1000 et idx = ptr2

	EAGLE_VARIABLEP ptr1,ptr2;
	EAGLE_keywords ptr_type; // le type du ptr , si on écrit [var,idx]  , il aura le type du var

}EAGLE_VARIABLE;


typedef struct
{
	std::string item;
	std::string ptr[4];
	EAGLE_keywords ktype;
	int ptype[4];
	int type;
	int scope;
	int line,col,pn;
	char token1,token2;
	bool label;

	char ptoken1[4],ptoken2[4];

}EAGLE_WORDS;
