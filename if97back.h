/******************************************************/
/****	IF97逆関数	用 header file					**/
/*	実用国際状態式（1997年）を使用した逆関数		**/
/*****************************************************/

/*
   This header file is for if97back.c
*/

#include	<stdlib.h>
#include	<math.h>
#include "if97.h"

#ifdef _DEBUG
#include	<stdio.h>
#endif

#ifndef	_IF97BACK_DEFINED

#define	V_R13_room	(0.4e-5)	//比容積	領域1-3の不連続量
#define	V_R23_room	(1.8e-5)	//比容積	領域2-3の不連続量

//２分法で算出する場合の許容誤差
#define	P_R3_tole	(1.0e-5)	//in r3tp2d(double	t,	p), r3t2d_l(t), r3t2d_g(t)
#define	S_R3_tole	(1.0e-5)	//in r3ps2t(p ,s)
#define	H_R3_tole	(1.0e-5)	//in r3ph2t(p ,h)
//#define	Delta_D		(5.0)		//t,p->d 算出用刻み値（領域3）[kg/m3]	in r3tp2d_3()
#define	S_R5_tole	(1.0e-5)	//in r5sp2t(double	s,	p)
#define	H_R5_tole	(1.0e-5)	//in r5hp2t(double	h,	p)

//領域3で2分法を使用する際の固定値
#define	R3Vmin		(0.0013)	//比体積(v)[m3/kg]の最小値
#define	R3Vmax		(0.0089)	//比体積(v)[m3/kg]の最大値
#define	R3Vcrt		(1.0/(IF97_Dc))	//臨界点での比体積(v)[m3/kg]
#define	R3_Nspin	(30)		//スピノーダル線上の比体積を挟み込むときの繰り返し演算回数

//	Proto type declare
double	r3tp2d(double	t,	double	p);
//double	r3tp2d_3(double	t,	double	p,	double*	ans,	int	n);	//Version 1.5以前に使用
double	r5sp2t(double	s,	double	p);
double	r5hp2t(double	h,	double	p);
//	Sep./2000 append
double	r3t2v_l(double	t);
double	r3t2v_g(double	t);
double	r3t2d_l(double	t);
double	r3t2d_g(double	t);
double	r3ps2t(double	p,	double	s);
double	r3ph2t(double	p,	double	h);

extern int	WhatIsRegPH(double	p,	double	h);
extern int	WhatIsRegPS(double	p,	double	s);


#define _IF97BACK_DEFINED
#endif	//_IF97BACK_DEFINED
//  end
