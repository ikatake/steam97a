/*****************************************************
	International Association for Properties of Water and Steam
	実用国際状態式（1997年）IF97
	in C language Coded by Takayoshi Tabuchi.
*****************************************************/

/***Notation************************
'99 03/03		Ver 0.1 作成開始
				Releaseを正確にわかりやすく
				記述することを最優先して書く。
				コメントのPageや式番号は 
				Sep. 1997 IAPWS release原文のもの
'99 03/27		Ver 1.0 release	(w:音速の計算がおかしいが無視。)

****

1)変換用以外のプログラム中では特別に記述がない時は，
  温度[K], 圧力[MPa], 密度[Kg/m3],エントロピー[kJ/KgK],
  比容積[m3/Kg], 
2)次の変数名は，それぞれ以下の物を表すとする．
	t	-	温度[k]	---1990年国際温度目盛り
	p	-	圧力[MPa]	絶対圧
	v	-	比容積{m3/Kg}
	d	-	密度[Kg/m3]
	h	-	比エンタルピー[kJ/Kg]
	s	-	比エントロピー[kJ/KgK]
	w	-	音速[m/s]

3)このFileでは、自関数の防衛(関数内で0ゼロ除算など
  のなエラーを出さないようにする)目的以外のための
  エラーチェックは行わない。

4)関数内で引数のチェックなどをしてエラーを返すときは
  ヘッダーFileで宣言している STMOHT_ERROR を返値として返す。
*/

#include "if97.h"

/////////////////////////////////////


//for (5) & (6)
const static	double	B23n[6]=	{-999999.999,
							 0.34805185628969e+3,
							-0.11671859879975e+1,
							 0.10192970039326e-2,
							 0.57254459862746e+3,
							 0.13918839778870e+2};

//領域2と3の境界線
//5p	式(5)	350℃ < t < 590℃,	16.5295MPa < p < 100MPa
double	b23t2p(double t)	{	//t:絶対温度[K]	返り値:圧力[MPa]
	double	Ps	=	1.0;	//[MPa]	Release文では、p*と記されているもの
	double	Ts	=	1.0;	//[K]	Release文では、T*と記されているもの

	double	theta	=	t/Ts;
	double	pai;
	pai	=	B23n[1] + B23n[2]*theta + B23n[3]*pow(theta,2.0);
	return(pai*Ps);
}

//6p	式(6)		式(7)の逆関数
//	350℃ < t < 590℃,	16.5295MPa < p < 100MPa
double	b23p2t(double p)	{	//p:圧力[MPa]	返り値:温度[K]
	double	Ps	=	1.0;	//[MPa]	Release文では、p*と記されているもの
	double	Ts	=	1.0;	//[K]	Release文では、T*と記されているもの
	double	pai	=	p/Ps;
	double	theta;
	theta	=	B23n[4] + sqrt( (pai - B23n[5])/B23n[3]);
	return(theta*Ts);
}

//	*******
//領域判定(Normal全領域)	温度t, 圧力pから
int	WhatIsRegion(double	t,	double	p)	{	//返り値 領域の数字
	//t:温度[K]	p:圧力[MPa]
	double	sP;		//飽和蒸気圧力

	if(	(IF97_MIN_TEMP <= t && t <= IF97_R5MIN_TEMP) 
			&& (IF97_MIN_PRESS < p && p <= IF97_MAX_PRESS)	)	{
		if(t <= IF97_R3MIN_TEMP)	{
			sP	=	r4t2p(t);
			if( p == sP )	return	4;	//領域4
			else	if(sP < p)		return	1;	//領域1
			else			return	2;	//領域2
		}
		else	if(t< IF97_Tc)	{
			sP	=	r4t2p(t);
			if( p == sP )	return	4;	//領域4
			else	if(	b23t2p(t) < p)	return	3;	//領域3
			else	return	2;			//領域2
		}
		else	{
			if(b23t2p(t) < p)	return	3;		//領域3
			else	return	2;			//領域2
		}
	}
	else	if(	(IF97_R5MIN_TEMP < t && t <= IF97_MAX_TEMP)
					&&	(IF97_MIN_PRESS < p && p <= IF97_R5MAX_PRESS)	)	{
		return	5;	//領域5
	}
	else	{	//領域外
		return	(int)STMOHT_ERROR;	
	}
}

