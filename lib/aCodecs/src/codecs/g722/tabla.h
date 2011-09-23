# ifndef TABLA_H 
# define TABLA_H

/*tabla 14/g.722; niveles de decision del cuantificador L;
representacion S 2 1 ... -9 */

const short q6[30] =
{	
		0,
		35,
		72,
		110,
		150,
		190,
		233,
		276,
		323,
		370,
		422,
		473,
		530,
		587,
		650,
		714,
		786,
		858,
		940,
		1023,
		1121,
		1219,
		1339,
		1458,
		1612,
		1765,
		1980,
		2195,
		2557,
		2919
};

/* En la tabla 14/G722. Son las salidas del cuantificador	*/
/* inverso de cuatro bits. S 2 1...-9 */

const short qq4[9]=
{
	0,
	0,
	150,
	323,
	530,
	786,
	1121,
	1612,
	2557
};

/* salidas del cuantificador inverso de 5 bits*/
const short qq5[16]=
{
	0,
	35,
	110,
	190,
	276,
	370,
	473,
	587,
	714,
	858,
	1023,
	1219,
	1458,
	1765,
	2195,
	2919
};

/* salidas del cuantificador inverso de 6 bits*/

const short qq6[31]=
{
	0,
	17,
	54,
	91,
	130,
	170,
	211,
	254,
	300,
	347,
	396,
	447,
	501,
	558,
	618,
	682,
	750,
	822,
	899,
	982,
	1072,
	1170,
	1279,
	1399,
	1535,
	1689,
	1873,
	2088,
	2376,
	2738,
	3101
};

/* En la tabla 17/G722. Son los intervalos de cuantificacion que */
/* se corresponden a las palabras codificadas de cuatro bits, que*/
/* actuan como indices del array */

const unsigned char iL4 [16]=
{
	0,
	7,
	6,
	5,
	4,
	3,
	2,
	1,
	7,
	6,
	5,
	4,
	3,
	2,
	1,
	0
};

/* tabla 18/g722; intervalos de cuantificación para palabras de 
6 bits*/

const unsigned char iL6[64]=
{
	1,
	1,
	1,
	1,
	30,
	29,
	28,
	27,
	26,
	25,
	24,
	23,
	22,
	21,
	20,
	19,
	18,
	17,
	16,
	15,
	14,
	13,
	12,
	11,
	10,
	9,
	8,
	7,
	6,
	5,
	4,
	3,
	30,
	29,
	28,
	27,
	26,
	25,
	24,
	23,
	22,
	21,
	20,
	19,
	18,
	17,
	16,
	15,
	14,
	13,
	12,
	11,
	10,
	9,
	8,
	7,
	6,
	5,
	4,
	3,
	2,
	1,
	2,
	1
};

/* tabla 19/g722; intervalos de cuantificacion para palabras de 5 bits*/

const unsigned char iL5[32]=
{
	1,
	1,
	15,
	14,
	13,
	12,
	11,
	10,
	9,
	8,
	7,
	6,
	5,
	4,
	3,
	2,
	15,
	14,
	13,
	12,
	11,
	10,
	9,
	8,
	7,
	6,
	5,
	4,
	3,
	2,
	1,
	1
};

/* En la tabla 14/G722. Son los multiplicadores del factor de 	*/
/* escala logaritmico. S 0 -1...-11 */

const short wL[9]=
{
	0,
	-60,
	-30,
	58,
	172,
	334,
	538,
	1198,
	3042
};

/* Es la tabla 15/G722 de conversion logaritmo-lineal de 32 	*/
/* entradas. S 0...-16 */

const short iLB [32]=
{
	2048,
	2093,
	2139,
	2186,
	2233,
	2282,
	2332,
	2383,
	2435,
	2489,
	2543,
	2599,
	2656,
	2714,
	2774,
	2834,
	2896,
	2960,
	3025,
	3091,
	3158,
	3228,
	3298,
	3371,
	3444,
	3520,
	3597,
	3676,
	3756,
	3838,
	3922,
	4008
};

/* Tabla 21/G722, conversion a los intervalos de cuantificacion*/

const short iH2[4]=
{
	2,
	1,
	2,
	1
};

/* Tablas 14/G722,*/
const short qq2[3] =
{
	0,
	1616,
	7408
};

const short wH[3]=
{
	0,
	-214,
	798
};

#endif
