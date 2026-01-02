/******************************************************/
/****	IF97	用 header file						**/
/*	実用国際状態式（1997年）						**/
/*****************************************************/

/*
   This header file is for if97r1.c,if97r2.c,if97r3.c,if97r4.c,if97.c
*/

#include	<stdlib.h>
#include	<math.h>

#ifdef _DEBUG
#include	<stdio.h>
#endif

#ifndef	STMOHT_ERROR
#define	STMOHT_ERROR	(-99999.999)
#endif	//STMOHT_ERROR

#ifndef	_IF97_DEFINED

/////////////////////////////////////////////////////////////////////////////
// Constant Values
#define	IF97_Rspec	(0.461526)		//[kJ/KgK]	普通水のガス定数()
#define	IF97_Tc		(647.096)		//[K]	臨界点温度
#define	IF97_Pc		(22.064)		//[MPa]	臨界点圧力
#define	IF97_Dc		(322.0)			//[kg/m3]	臨界点密度
////
#define	IF97_Tt		(273.16)		//[K]	三重点温度
#define	IF97_Pt		(611.657e-6)	//[MPa]	三重点圧力
//
#define	IF97_MAX_TEMP	(2273.15)	//処理可能最高温度[K]
#define	IF97_MIN_TEMP	(273.15)	//処理可能最低温度[K]
#define	IF97_MAX_PRESS	(100.0)		//処理可能最高圧力[MPa]
#define	IF97_MIN_PRESS	(0.0)		//処理可能最低圧力[MPa]
#define	IF97_R3MIN_TEMP	(623.15)	//領域3の最低温度
#define	IF97_R5MIN_TEMP	(1073.15)	//領域5の最低温度
#define	IF97_R5MAX_PRESS	(10.0)	//領域5の最高圧力

#define	IF97_R2BC_S		(5.85)		//領域2の逆関数T(p,s)用領域分割s[kJ/kgK]
#define	IF97_R2AMAX_PRESS	(4.0)	//領域2aの最大圧力[MPa]

//
/////////////////////////////////////


//****	prototype declare	**
//
//Region1
extern	double	r1tp2v(double t,	double p);
extern	double	r1tp2u(double t,	double p);
extern	double	r1tp2s(double t,	double p);
extern	double	r1tp2h(double t,	double p);
extern	double	r1tp2Cp(double t,	double p);
extern	double	r1tp2Cv(double t,	double p);
extern	double	r1tp2w(double t,	double p);
extern	double	r1ph2t(double p,	double h);
extern	double	r1ps2t(double p,	double s);
//'00/Nov append
extern	double	r1tp2k(double t,	double p);

//Region2
extern double	r2tp2v(double t,	double p);
extern double	r2tp2u(double t,	double p);
extern double	r2tp2s(double t,	double p);
extern double	r2tp2h(double t,	double p);
extern double	r2tp2Cp(double t,	double p);
extern double	r2tp2Cv(double t,	double p);
extern double	r2tp2w(double t,	double p);
extern int	WhatIsSubR2ps(double	p,	double	s);
extern int	WhatIsSubR2ph(double	p,	double	h);
extern double	r2b23h2p(double h);
extern double	r2b23p2h(double p);
extern double	r2aph2t(double p,	double h);
extern double	r2bph2t(double p,	double h);
extern double	r2cph2t(double p,	double h);
extern double	r2aps2t(double p,	double s);
extern double	r2bps2t(double p,	double s);
extern double	r2cps2t(double p,	double s);
extern double	r2_tp2v(double t,	double p);
extern double	r2_tp2u(double t,	double p);
extern double	r2_tp2s(double t,	double p);
extern double	r2_tp2h(double t,	double p);
extern double	r2_tp2Cp(double t,	double p);
extern double	r2_tp2Cv(double t,	double p);
extern double	r2_tp2w(double t,	double p);
//'00/Nov append
extern	double	r2tp2k(double t,	double p);

//Region3
extern double	r3dt2p(double d,	double t);
extern double	r3dt2u(double d,	double t);
extern double	r3dt2s(double d,	double t);
extern double	r3dt2h(double d,	double t);
extern double	r3dt2Cv(double d,	double t);
extern double	r3dt2Cp(double d,	double t);
extern double	r3dt2w(double d,	double t);
//'00/02/17 appended
extern double r3dt2dPdV(double v,	double t);
//'00/Nov append
extern	double	r3dt2k(double d,	double t);


//Region4
extern double	r4t2p(double	t);
extern double	r4p2t(double	p);

//Region5
extern	double	r5tp2v(double t,	double p);
extern	double	r5tp2u(double t,	double p);
extern	double	r5tp2s(double t,	double p);
extern	double	r5tp2h(double t,	double p);
extern	double	r5tp2Cp(double t,	double p);
extern	double	r5tp2Cv(double t,	double p);
extern	double	r5tp2w(double t,	double p);
//'00/Nov append
extern	double	r5tp2k(double t,	double p);

//	Other
extern double	b23t2p(double t);
extern double	b23p2t(double p);
extern int	WhatIsRegion(double	t,	double	p);

#define _IF97_DEFINED
#endif	//_IF97_DEFINED
//  end
