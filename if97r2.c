/*****************************************************
	International Association for Properties of Water and Steam
	実用国際状態式（1997年）IF97

	This is a part of IF97.
	in C language Coded by Takayoshi Tabuchi.
*****************************************************/

/*
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

// 1999/3/9 wrote:
// ここでは、飽和温度おより若干温度が低いにもかかわらず、若干湿りを帯びた
//飽和蒸気状態でいる、準安定蒸気域(Meta-stable-Vaper region)を対象
//とした補助式は記述しない。→ いや、使わなくても記述だけはしておこう。

#include "if97.h"

//	*******	領域2	******************************
//
//****
//	13p
const static	double	r2Ps	=	1.0;	//[MPa]	Release文では、p*と記されているもの
const static	double	r2Ts	=	540.0;	//[K]	Release文では、T*と記されているもの
//	Table 10	13p
const static	int	r2Jo[10] =
	{ -999999,	0,	1,	-5,	-4,	-3,	-2,	-1,	2,	3	};

const static	double r2no[10] = 			//これは、1a,2aなど式(16),(18)用の係数は対象外
			{-999999.999,		
			-0.96927686500217e+1,
			 0.10086655968018e+2,
			-0.56087911283020e-2,
			 0.71452738081455e-1,
			-0.40710498223928,
			 0.14240819171444e+1,
			-0.43839511319450e+1,
			-0.28408632460772,
			 0.21268463753307e-1	};

//Table 11	14p
const static	int r2I[44] =
	{ -999999,	1,	1,	1,	1,	1,	2,	2,	2,	2,
			2,	3,	3,	3,	3,	3,	4,	4,	4,	5,
			6,	6,	6,	7,	7,	7,	8,	8,	9,	10,
			10,	10,	16,	16,	18,	20,	20,	20,	21,	22,
			23,	24,	24,	24		};

const static	int r2J[44] =
	{ -999999,	0,	1,	2,	3,	6,	1,	2,	4,	7,
			36,	0,	1,	3,	6,	35,	1,	2,	3,	7,
			3,	16,	35,	0,	11,	25,	8,	36,	13,	4,
			10,	14,	29,	50,	57,	20,	35,	48,	21,	53,
			39,	26,	40,	58		};

const static	double r2n[44] = 	
		{	-999999.999,
			-0.17731742473213e-2,
			-0.17834862292358e-1,
			-0.45996013696365e-1,
			-0.57581259083432e-1,
			-0.50325278727930e-1,
			-0.33032641670203e-4,
			-0.18948987516315e-3,
			-0.39392777243355e-2,
			-0.43797295650573e-1,
			-0.26674547914087e-4,
			 0.20481737692309e-7,
			 0.43870667284435e-6,
			-0.32277677238570e-4,
			-0.15033924542148e-2,
			-0.40668253562649e-1,
			-0.78847309559367e-9,
			 0.12790717852285e-7,
			 0.48225372718507e-6,
			 0.22922076337661e-5,
			-0.16714766451061e-10,
			-0.21171472321355e-2,
			-0.23895741934104e+2,
			-0.59059564324270e-17,
			-0.12621808899101e-5,
			-0.38946842435739e-1,
			 0.11256211360459e-10,
			-0.82311340897998e+1,
			 0.19809712802088e-7,
			 0.10406965210174e-18,
			-0.10234747095929e-12,
			-0.10018179379511e-8,
			-0.80882908646985e-10,
			 0.10693031879409,
			-0.33662250574171,
			 0.89185845355421e-24,
			 0.30629316876232e-12,
			-0.42002467698208e-5,
			-0.59056029685639e-25,
			 0.37826947613457e-5,
			-0.12768608934681e-14,
			 0.73087610595061e-28,
			 0.55414715350778e-16,
			-0.94369707241210e-6	};

// 16p Table13 γoで表されている関数
double	r2Gam_o(double pai,	double tau)	{
	double	a	=	0.0;	//作業用
	unsigned	i;
	for(i = 1;	i <= 9;	i++){
		a	+=	r2no[i]
				*	pow(tau, (double)r2Jo[i]);
	}
	return	log(pai) + a;
}

// 16p Table13 γoπで表されている関数
double	r2Gam_oPai(double pai)	{
	return	1/pai + 0.0;	//意味なく原本に忠実に書いた。
}

// 16p Table13 γoππで表されている関数
double	r2Gam_oPaiPai(double pai)	{
	return	-1/pow(pai, 2.0) + 0.0;	//意味なく原本に忠実に書いた。
}

// 16p Table13 γoτで表されている関数
double	r2Gam_oTau(double tau)	{
	double	a	=	0.0;	//作業用
	unsigned	i;
	for(i = 1;	i <= 9;	i++){
		a	+=	r2no[i]
				*	(double)r2Jo[i]
					*	pow(tau, (double)(r2Jo[i] - 1));
	}
	return	0.0 + a;	//意味なく原本に忠実に書いた。
}

// 16p Table13 γoττで表されている関数
double	r2Gam_oTauTau(double tau)	{
	double	a	=	0.0;	//作業用
	unsigned	i;
	for(i = 1;	i <= 9;	i++){
		a	+=	r2no[i]
				*	(double)(r2Jo[i]	*	(r2Jo[i] - 1)	)
					*	pow(tau, (double)(r2Jo[i] - 2));
	}
	return	0.0 + a;	//意味なく原本に忠実に書いた。
}

// 16p Table13 γoττで表されている関数
double	r2Gam_oPaiTau(void)	{
	return	0.0 + 0.0;
}

// 16p Table14 γrで表されている関数
double	r2Gam_r(double pai,	double tau)	{
	double	a	=	0.0;	//作業用
	unsigned	i;
	for(i = 1;	i <= 43;	i++){
		a	+=	r2n[i]
				*	pow(pai, (double)r2I[i])
					*	pow(tau - 0.5,	(double)r2J[i]);
	}
	return	a;
}

// 16p Table14 γrπで表されている関数
double	r2Gam_rPai(double pai,	double tau)	{
	double	a	=	0.0;	//作業用
	unsigned	i;
	for(i = 1;	i <= 43;	i++){
		a	+=	r2n[i]	*	(double)r2I[i]
				*	pow(pai, (double)(r2I[i] -1) )
					*	pow(tau - 0.5,	(double)r2J[i]);
	}
	return	a;
}

// 16p Table14 γrππで表されている関数
double	r2Gam_rPaiPai(double pai,	double tau)	{
	double	a	=	0.0;	//作業用
	unsigned	i;
	for(i = 1;	i <= 43;	i++){
		a	+=	r2n[i]	*	(double)(r2I[i]	*	(r2I[i] -1))
				*	pow(pai, (double)(r2I[i] -2) )
					*	pow(tau - 0.5,	(double)r2J[i]);
	}
	return	a;
}

// 16p Table14 γrτで表されている関数
double	r2Gam_rTau(double pai,	double tau)	{
	double	a	=	0.0;	//作業用
	unsigned	i;
	for(i = 1;	i <= 43;	i++){
		a	+=	r2n[i]
				*	pow(pai, (double)r2I[i] )
					*	(double)r2J[i]
						*	pow(tau - 0.5,	(double)(r2J[i] - 1));
	}
	return	a;
}

// 16p Table14 γrττで表されている関数
double	r2Gam_rTauTau(double pai,	double tau)	{
	double	a	=	0.0;	//作業用
	unsigned	i;
	for(i = 1;	i <= 43;	i++){
		a	+=	r2n[i]
				*	pow(pai, (double)r2I[i] )
					*	(double)(r2J[i] * (r2J[i] -1)	)
						*	pow(tau - 0.5,	(double)(r2J[i] - 2));
	}
	return	a;
}

// 16p Table14 γrπτで表されている関数
double	r2Gam_rPaiTau(double pai,	double tau)	{
	double	a	=	0.0;	//作業用
	unsigned	i;
	for(i = 1;	i <= 43;	i++){
		a	+=	r2n[i]	*	(double)r2I[i]
				*	pow(pai, (double)(r2I[i] -1))
					*	(double)r2J[i]
						*	pow(tau - 0.5,	(double)(r2J[i] - 1));
	}
	return	a;
}

// 15p	領域2	比容積[m3/Kg]
double	r2tp2v(double t,	double p)	{	//t[K],	p[MPa]
	double	a;	//作業用
	double	v;
	double	pai	=	p/r2Ps;
	double	tau	=	r2Ts/t;
	a	=	pai * (r2Gam_oPai(pai)	+	r2Gam_rPai(pai,tau)	);
	v	=	IF97_Rspec * t / p * a;
//	return	v;
	return	v * 0.001;
}

// 15p	領域2	内部エネルギー[KJ/Kg]
double	r2tp2u(double t,	double p)	{	//t[K],	p[MPa]
	double	a;	//作業用
	double	u;
	double	pai	=	p/r2Ps;
	double	tau	=	r2Ts/t;
	a	=	tau * (r2Gam_oTau(tau) + r2Gam_rTau(pai,tau))
				- pai * (r2Gam_oPai(pai) + r2Gam_rPai(pai,tau));
	u	=	IF97_Rspec * t * a;
	return	u;
}

// 15p	領域2	比エントロピー[KJ//KgK]
double	r2tp2s(double t,	double p)	{	//t[K],	p[MPa]
	double	a;	//作業
	double	s;
	double	tau	=	r2Ts/t;
	double	pai	=	p/r2Ps;

	a	=	tau * (r2Gam_oTau(tau) + r2Gam_rTau(pai,tau))
				- (r2Gam_o(pai,tau) + r2Gam_r(pai,tau));
	s	=	IF97_Rspec * a;
	return	s;
}

// 15p	領域2	比エンタルピー[KJ//Kg]
double	r2tp2h(double t,	double p)	{	//t[K],	p[MPa]
	double	a;	//作業用
	double	h;
	double	tau	=	r2Ts/t;
	double	pai	=	p/r2Ps;
	a	=	tau * (r2Gam_oTau(tau) + r2Gam_rTau(pai,tau));
	h	=	IF97_Rspec * t * a;
	return	h;
}

// 15p	領域2	Cp	定圧比熱[KJ/KgK]
double	r2tp2Cp(double t,	double p)	{	//t[K],	p[MPa]
	double	a;	//作業用
	double	Cp;
	double	tau	=	r2Ts/t;
	double	pai	=	p/r2Ps;
	a	=	(-pow(tau,	2.0)) * (r2Gam_oTauTau(tau) + r2Gam_rTauTau(pai,tau));
	Cp	=	IF97_Rspec * a;
	return	Cp;
}

// 15p	領域2	Cv	定積比熱[KJ/KgK]
double	r2tp2Cv(double t,	double p)	{	//t[K],	p[MPa]
	double	a,	b;	//作業用
	double	Cv,	Cp;
	double	pai	=	p/r2Ps;
	double	tau	=	r2Ts/t;
	Cp	=	(-pow(tau,	2.0)) * (r2Gam_oTauTau(tau) + r2Gam_rTauTau(pai,tau));
	a	=	(-pow(1.0 + pai * r2Gam_rPai(pai,tau) - tau * pai * r2Gam_rPaiTau(pai,tau),	2.0)	);
	b	=	1.0 - pow(pai, 2.0) * r2Gam_rPaiPai(pai,tau);
	Cv	=	IF97_Rspec * (Cp + a/b);
	return	Cv;
}

// 15p	領域2	w	音速[m/s]
double	r2tp2w(double t,	double p)	{	//t[K],	p[MPa]
	double	w;
	double	a,	b,	c,	d,	w2;	//作業用
	double	tau	=	(r2Ts/t);
	double	pai	=	p/r2Ps;
	double	dGam_rPai	=	r2Gam_rPai(pai,tau);

//	a	=	1.0 + 2.0*pai*dGam_rPai + pow(pai,2.0)*pow(dGam_rPai,2.0);
	a	=	pow(1.0 + pai*dGam_rPai,	2.0);		//Nov./2000 式変形 
	b	=	1.0 - pai*pai*r2Gam_rPaiPai(pai,tau);
	c	=	1.0 + pai*dGam_rPai - tau*pai*r2Gam_rPaiTau(pai,tau);
	d	=	tau*tau*(r2Gam_oTauTau(tau) + r2Gam_rTauTau(pai,tau));
	w2	=	IF97_Rspec * t * a/(b + (c*c)/d) * 1000.0;
	if(w2 < 0.0)	w2 = 0.0;
	w	=	sqrt(	w2	);
	return	w;
}

// 日本機会学会original	領域2	K	等エントロピー指数(断熱指数:isentropic exponent)
double	r2tp2k(double t,	double p)	{	//t[K],	p[MPa]
	double	k;
	double	a,	b,	c,	d,	e;	//作業用
	double	tau	=	(r2Ts/t);
	double	pai	=	p/r2Ps;
	double	dGam_rPai	=	r2Gam_rPai(pai,tau);

	a	=	pow(1.0 + pai*dGam_rPai,	2.0);
	b	=	1.0 - pai*pai*r2Gam_rPaiPai(pai,tau);
	c	=	1.0 + pai*dGam_rPai - tau*pai*r2Gam_rPaiTau(pai,tau);
	d	=	tau*tau*(r2Gam_oTauTau(tau) + r2Gam_rTauTau(pai,tau));
	e	=	pai * (r2Gam_oPai(pai) + r2Gam_rPai(pai,	tau) );
	k	=	a/(b + (c*c)/d) / e;
	return	k;
}

//************************
//	領域2	逆関数
//
//************************
//	for	式(20)&(21)
const static	double	r2B23n[6]	=	{	-999999.999,
		 0.90584278514723e+3,
		-0.67955786399241,
		 0.12809002730136e-3,
		 0.26526571908428e+4,
		 0.45257578905948e+1		};


//領域判定(領域2逆関数専用)
//返り値
//	領域2a	->	1,
//		2b	->	2,
//		2c	->	3	を返す。
//	今から検査しようとする蒸気条件は領域２で
//	あることを前提とする。(領域2に入っているかはCheckしない)
int	WhatIsSubR2ps(double	p,	double	s)	{
	//	p:圧力[MPa]	s:エントロピー[kJ/KgK]
	//int	errNo	=	(int)STMOHT_ERROR;

	if	(p <= IF97_R2AMAX_PRESS	)		return	1;
	else	if	(IF97_R2BC_S	<=	s)	return	2;
	else					return	3;	
}

//領域判定(領域2逆関数専用)
//返り値
//	領域2a	->	1,
//		2b	->	2,
//		2c	->	3	を返す。
//	今から検査しようとする蒸気条件は領域２で
//	あることを前提とする。(領域2に入っているかはCheckしない)
int	WhatIsSubR2ph(double	p,	double	h)	{
	//	p:圧力[MPa]	h:エンタルピー[kJ/Kg]
	//int	errNo	=	(int)STMOHT_ERROR;

	if	(p <= IF97_R2AMAX_PRESS	)		return	1;
	else	if	(r2b23p2h( p)	<=	h)	return	2;
	else					return	3;
}

//	21p	式(20)	領域２の部分
double	r2b23h2p(double h)		{	//h:エンタルピー[kJ/Kg]
	const static	double	Hs	=	1.0;	//	Hs:21pにh*で表されているもの
	const static	double	Ps	=	1.0;	//	Ps:21pにp*で表されているもの
	double	eta	=	h/Hs;
	double	pai;
	pai	=	r2B23n[1] + r2B23n[2]*eta + r2B23n[3]*pow(eta, 2.0);
	return	pai * Ps;
}

//	21p	式(20)	
double	r2b23p2h(double p)		{	//p:圧力[MPa]
	const static	double	Hs	=	1.0;	//	Hs:21pにh*で表されているもの
	const static	double	Ps	=	1.0;	//	Ps:21pにp*で表されているもの
	double	eta;
	double	pai	=	p/Ps;
	eta	=	r2B23n[4] + sqrt((pai - r2B23n[5])/r2B23n[3]);
	return	eta * Hs;
}

//領域2a	圧力、エンタルピー→温度	22p	(22)式
double	r2aph2t(double p,	double h)	{	//p:圧力[MPa],	h:比エンタルピー
	//	22p	Table 20
	const static	int vI[35] =
		{ -999999,	0,	0,	0,	0,	0,	0,	1,	1,	1,
				1,	1,	1,	1,	1,	1,	2,	2,	2,	2,
				2,	2,	2,	2,	3,	3,	4,	4,	4,	5,
				5,	5,	6,	6,	7	};
	const static	int	vJ[35]	=
		{ -999999,	0,	1,	2,	3,	7,	20,	0,	1,	2,
				3,	7,	9,	11,	18,	44,	0,	2,	7,	36,
				38,	40,	42,	44,	24,	44,	12,	32,	44,	32,
				36,	42,	34,	44,	28	};
		
	const static	double	vn[35]	=		{-999999.999,
				 0.10898952318288e+4,
				 0.84951654495535e+3,
				-0.10781748091826e+3,
				 0.33153654801263e+2,
				-0.74232016790248e+1,
				 0.11765048724356e+2,
				 0.18445749355790e+1,
				-0.41792700549624e+1,
				 0.62478196935812e+1,
				-0.17344563108114e+2,
				-0.20058176862096e+3,
				 0.27196065473796e+3,
				-0.45511318285818e+3,
				 0.30919688604755e+4,
				 0.25226640357872e+6,
				-0.61707422868339e-2,
				-0.31078046629583,
				 0.11670873077107e+2,
				 0.12812798404046e+9,
				-0.98554909623276e+9,
				 0.28224546973002e+10,
				-0.35948971410703e+10,
				 0.17227349913197e+10,
				-0.13551334240775e+5,
				 0.12848734664650e+8,
				 0.13865724283226e+1,
				 0.23598832556514e+6,
				-0.13105236545054e+8,
				 0.73999835474766e+4,
				-0.55196697030060e+6,
				 0.37154085996233e+7,
				 0.19127729239660e+5,
				-0.41535164835634e+6,
				-0.62459855192507e+2		};

	const static	double	Ts	=	1.0;	//	Ts:22pにT*で表されているもの
	const static	double	Ps	=	1.0;	//	Ps:22pにp*で表されているもの
	const static	double	Hs	=	2000.0;	//	Hs:22pにh*で表されているもの
	double	eta	=	h/Hs;
	double	pai	=	p/Ps;
	double	theta	=	0.0;
	int	i;
	for(i = 1;i<=34;i++)	{
		theta	+=	vn[i] * pow(pai,(double)vI[i])
					* pow(eta-2.1,	(double)vJ[i]);
	}
	return	theta * Ts;
}

//領域2b	圧力、エンタルピー→温度	23p	(23)式
double	r2bph2t(double p,	double h)	{	//p:圧力[MPa],	h:比エンタルピー
	//	23p	Table 21
	const static	int vI[39] =	{ -999999,
					0,	0,	0,	0,	0,	0,	0,	0,	1,
				1,	1,	1,	1,	1,	1,	1,	2,	2,	2,
				2,	3,	3,	3,	3,	4,	4,	4,	4,	4,
				4,	5,	5,	5,	6,	7,	7,	9,	9	};
	const static	int	vJ[39]	=		{ -999999,
					0,	1,	2,	12,	18,	24,	28,	40,	0,
				2,	6,	12,	18,	24,	28,	40,	2,	8,	18,
				40,	1,	2,	12,	24,	2,	12,	18,	24,	28,
				40,	18,	24,	40,	28,	2,	28,	1,	40	};
		
	const static	double	vn[39]	=		{-999999.999,
				 0.14895041079516e+4,
				 0.74307798314034e+3,
				-0.97708318797837e+2,
				 0.24742464705674e+1,
				-0.63281320016026,
				 0.11385952129658e+1,
				-0.47811863648625,
				 0.85208123431544e-2,
				 0.93747147377932,
				 0.33593118604916e+1,	//10
				 0.33809355601454e+1,
				 0.16844539671904,
				 0.73875745236695,
				-0.47128737436186,
				 0.15020273139707,
				-0.21764114219750e-2,
				-0.21810755324761e-1,
				-0.10829784403677,
				-0.46333324635812e-1,
				 0.71280351959551e-4,	//20
				 0.11032831789999e-3,
				 0.18955248387902e-3,
				 0.30891541160537e-2,
				 0.13555504554949e-2,
				 0.28640237477456e-6,
				-0.10779857357512e-4,
				-0.76462712454814e-4,
				 0.14052392818316e-4,
				-0.31083814331434e-4,
				-0.10302738212103e-5,	//30
				 0.28217281635040e-6,
				 0.12704902271945e-5,
				 0.73803353468292e-7,
				-0.11030139238909e-7,
				-0.81456365207833e-13,
				-0.25180545682962e-10,
				-0.17565233969407e-17,
				 0.86934156344163e-14		};

	const static	double	Ts	=	1.0;	//	Ts:22pにT*で表されているもの
	const static	double	Ps	=	1.0;	//	Ps:22pにp*で表されているもの
	const static	double	Hs	=	2000.0;	//	Hs:22pにh*で表されているもの
	double	eta	=	h/Hs;
	double	pai	=	p/Ps;
	double	theta	=	0.0;
	int	i;
	for(i = 1;i<=38;i++)	{
		theta	+=	vn[i] * pow(pai-2.0,(double)vI[i])
					* pow(eta-2.6,	(double)vJ[i]);
	}
	return	theta * Ts;
}

//領域2c	圧力、エンタルピー→温度	24p	(24)式
double	r2cph2t(double p,	double h)	{	//p:圧力[MPa],	h:比エンタルピー
	//	24p	Table 22
	const static	int vI[24] =	{ -999999,
					-7,	-7,	-6,	-6,	-5,	-5,	-2,	-2,	-1,
				-1,	0,	0,	1,	1,	2,	6,	6,	6,	6,
				6,	6,	6,	6		};
	const static	int	vJ[24]	=	{ -999999,
					0,	4,	0,	2,	0,	2,	0,	1,	0,
				2,	0,	1,	4,	8,	4,	0,	1,	4,	10,
				12,	16,	20,	22		};
//				0,	1,	2,	3,	4,	5,	6,	7,	8,	9
//				0,	1,	2,	3,	4,	5,	6,	7,	8,	9
//				,	,	,	,	,	,	,	,	,	,
		
	const static	double	vn[24]	=		{-999999.999,
				-0.32368398555242e+13,
				 0.73263350902181e+13,
				 0.35825089945447e+12,
				-0.58340131851590e+12,
				-0.10783068217470e+11,
				 0.20825544563171e+11,
				 0.61074783564516e+6,
				 0.85977722535580e+6,
				-0.25745723604170e+5,
				 0.31081088422714e+5,	//10
				 0.12082315865936e+4,
				 0.48219755109255e+3,
				 0.37966001272486e+1,
				-0.10842984880077e+2,
				-0.45364172676660e-1,
				 0.14559115658698e-12,
				 0.11261597407230e-11,
				-0.17804982240686e-10,
				 0.12324579690832e-6,
				-0.11606921130984e-5,		//20
				 0.27846367088554e-4,
				-0.59270038474176e-3,
				 0.12918582991878e-2		};

	const static	double	Ts	=	1.0;	//	Ts:22pにT*で表されているもの
	const static	double	Ps	=	1.0;	//	Ps:22pにp*で表されているもの
	const static	double	Hs	=	2000.0;	//	Hs:22pにh*で表されているもの
	double	eta	=	h/Hs;
	double	pai	=	p/Ps;
	double	theta	=	0.0;
	int	i;
	for(i = 1;	i<=23;	i++)	{
		theta	+=	vn[i] * pow(pai + 25.0,(double)vI[i])
					* pow(eta-1.8,	(double)vJ[i]);
	}
	return	theta * Ts;
}

//領域2a	圧力、エントロピー→温度	26p	(25)式
double	r2aps2t(double p,	double s)	{	//p:圧力[MPa],	s:比エントロピー
	//	26p	Table 25
	const static	double vdI[47] =	{ -999999.999,
				-1.5,	-1.5,	-1.5,	-1.5,	-1.5,	-1.5,	-1.25,	-1.25,	-1.25,
		-1.0,	-1.0,	-1.0,	-1.0,	-1.0,	-1.0,	-0.75,	-0.75,	-0.5,	-0.5,
		-0.5,	-0.5,	-0.25,	-0.25,	-0.25,	-0.25,	0.25,	0.25,	0.25,	0.25,
		0.50,	0.5,	0.5,	0.5,	0.5,	0.5,	0.5,	0.75,	0.75,	0.75,
		0.75,	1.0,	1.0,	1.25,	1.25,	1.5,	1.5		};

	const static	int vJ[47] =	{ -999999,
				-24,	-23,	-19,	-13,	-11,	-10,	-19,	-15,	-6,
		-26,	-21,	-17,	-16,	-9,		-8,		-15,	-14,	-26,	-13,
		-9,		-7,		-27,	-25,	-11,	-6,		1,		4,		8,		11,
		0,		1,		5,		6,		10,		14,		16,		0,		4,		9,
		17,		7,		18,		3,		15,		5,		18		};
		
	const static	double	vn[47]	=		{-999999.999,
				-0.39235983861984e+6,
				 0.51526573827270e+6,
				 0.40482443161048e+5,
				-0.32193790923902e+3,
				 0.96961424218694e+2,
				-0.22867846371773e+2,
				-0.44942914124357e+6,
				-0.50118336020166e+4,
				 0.35684463560015,
				 0.44235335848190e+5,	//10
				-0.13673388811708e+5,
				 0.42163260207864e+6,
				 0.22516925837475e+5,
				 0.47442144865646e+3,
				-0.14931130797647e+3,
				-0.19781126320452e+6,
				-0.23554399470760e+5,
				-0.19070616302076e+5,
				 0.55375669883164e+5,
				 0.38293691437363e+4,	//20
				-0.60391860580567e+3,
				 0.19363102620331e+4,
				 0.42660643698610e+4,
				-0.59780638872718e+4,
				-0.70401463926862e+3,
				 0.33836784107553e+3,
				 0.20862786635187e+2,
				 0.33834172656196e-1,
				-0.43124428414893e-4,
				 0.16653791356412e+3,	//30
				-0.13986292055898e+3,
				-0.78849547999872,
				 0.72132411753872e-1,
				-0.59754839398283e-2,
				-0.12141358953904e-4,
				 0.23227096733871e-6,
				-0.10538463566194e+2,
				 0.20718925496502e+1,
				-0.72193155260427e-1,
				 0.20749887081120e-6,	//40
				-0.18340657911379e-1,
				 0.29036272348696e-6,
				 0.21037527893619,
				 0.25681239729999e-3,
				-0.12799002933781e-1,
				-0.82198102652018e-5	};

	const static	double	Ts	=	1.0;	//	Ts:26pにT*で表されているもの
	const static	double	Ps	=	1.0;	//	Ps:26pにp*で表されているもの
	const static	double	Ss	=	2.0;	//	Ss:26pにs*で表されているもの
	double	sigma	=	s/Ss;
	double	pai	=	p/Ps;
	double	theta	=	0.0;
	int	i;
	for(i = 1;i<=46;i++)	{
		theta	+=	vn[i] * pow(pai,	vdI[i])
					* pow(sigma - 2.0,	(double)vJ[i]);
	}
	return	theta * Ts;
}

//領域2b	圧力、エントロピー→温度	27p	(26)式
double	r2bps2t(double p,	double s)	{	//p:圧力[MPa],	s:比エントロピー
	//	27p	Table 26
	const static	int vI[45] =	{ -999999,
					-6,	-6,	-5,	-5,	-4,	-4,	-4,	-3,	-3,
				-3,	-3,	-2,	-2,	-2,	-2,	-1,	-1,	-1,	-1,
				-1,	0,	0,	0,	0,	0,	0,	0,	1,	1,
				1,	1,	1,	1,	2,	2,	2,	3,	3,	3,
				4,	4,	5,	5,	5		};

	const static	int	vJ[45]	=		{ -999999,
					0,	11,	0,	11,	0,	1,	11,	0,	1,
				11,	12,	0,	1,	6,	10,	0,	1,	5,	8,
				9,	0,	1,	2,	4,	5,	6,	9,	0,	1,
				2,	3,	7,	8,	0,	1,	5,	0,	1,	3,
				0,	1,	0,	1,	2	};
		
	const static	double	vn[45]	=		{-999999.999,
				 0.31687665083497e+6,
				 0.20864175881858e+2,
				-0.39859399803599e+6,
				-0.21816058518877e+2,
				 0.22369785194242e+6,
				-0.27841703445817e+4,
				 0.99207436071480e+1,
				-0.75197512299157e+5,
				 0.29708605951158e+4,
				-0.34406878548526e+1,	//10
				 0.38815564249115,
				 0.17511295085750e+5,
				-0.14237112854449e+4,
				 0.10943803364167e+1,
				 0.89971619308495,
				-0.33759740098958e+4,
				 0.47162885818355e+3,
				-0.19188241993679e+1,
				 0.41078580492196,
				-0.33465378172097,		//20
				 0.13870034777505e+4,
				-0.40663326195838e+3,
				 0.41727347159610e+2,
				 0.21932549434532e+1,
				-0.10320050009077e+1,
				 0.35882943516703,
				 0.52511453726066e-2,
				 0.12838916450705e+2,
				-0.28642437219381e+1,
				 0.56912683664855,		//30
				-0.99962954584931e-1,
				-0.32632037778459e-2,
				 0.23320922576723e-3,
				-0.15334809857450,
				 0.29072288239902e-1,
				 0.37534702741167e-3,
				 0.17296691702411e-2,
				-0.38556050844504e-3,
				-0.35017712292608e-4,
				-0.14566393631492e-4,	//40
				 0.56420857267269e-5,
				 0.41286150074605e-7,
				-0.20684671118824e-7,
				 0.16409393674725e-8	};

	const static	double	Ts	=	1.0;	//	Ts:27pにT*で表されているもの
	const static	double	Ps	=	1.0;	//	Ps:27pにp*で表されているもの
	const static	double	Ss	=	0.7853;	//	Ss:27pにs*で表されているもの
	double	sigma	=	s/Ss;
	double	pai	=	p/Ps;
	double	theta	=	0.0;
	int	i;
	for(i = 1;	i<=44;	i++)	{
		theta	+=	vn[i] * pow(pai,(double)vI[i])
					* pow(10.0-sigma,	(double)vJ[i]);
	}
	return	theta * Ts;
}

//領域2c	圧力、エントロピー→温度	28p	(27)式
double	r2cps2t(double p,	double s)	{	//p:圧力[MPa],	s:比エントロピー
	//	28p	Table 27
	const static	int vI[31] =	{ -999999,
					-2,	-2,	-1,	0,	0,	0,	0,	1,	1,
				1,	1,	2,	2,	2,	3,	3,	3,	4,	4,
				4,	5,	5,	5,	6,	6,	7,	7,	7,	7,
				7	};

	const static	int	vJ[31]	=		{ -999999,
					0,	1,	0,	0,	1,	2,	3,	0,	1,
				3,	4,	0,	1,	2,	0,	1,	5,	0,	1,
				4,	0,	1,	2,	0,	1,	0,	1,	3,	4,
				5	};
		
	const static	double	vn[31]	=		{-999999.999,
				 0.90968501005365e+3,
				 0.24045667088420e+4,
				-0.59162326387130e+3,
				 0.54145404128074e+3,
				-0.27098308411192e+3,
				 0.97976525097926e+3,
				-0.46966772959435e+3,
				 0.14399274604723e+2,
				-0.19104204230429e+2,
				 0.53299167111971e+1,	//10
				-0.21252975375934e+2,
				-0.31147334413760,
				 0.60334840894623,
				-0.42764839702509e-1,
				 0.58185597255259e-2,
				-0.14597008284753e-1,
				 0.56631175631027e-2,
				-0.76155864584577e-4,
				 0.22440342919332e-3,
				-0.12561095013413e-4,	//20
				 0.63323132660934e-6, 
				-0.20541989675375e-5,
				 0.36405370390082e-7,
				-0.29759897789215e-8,
				 0.10136618529763e-7,
				 0.59925719692351e-11,
				-0.20677870105164e-10,
				-0.20874278181886e-10,
				 0.10162166825089e-9,
				-0.16429828281347e-9	};	//30

	const static	double	Ts	=	1.0;	//	Ts:28pにT*で表されているもの
	const static	double	Ps	=	1.0;	//	Ps:28pにp*で表されているもの
	const static	double	Ss	=	2.9251;	//	Ss:28pにs*で表されているもの
	double	sigma	=	s/Ss;
	double	pai	=	p/Ps;
	double	theta	=	0.0;
	int	i;
	for(i = 1;	i<=30;	i++)	{
		theta	+=	vn[i] * pow(pai,(double)vI[i])
					* pow(2.0-sigma,	(double)vJ[i]);
	}
	return	theta * Ts;
}

//***********************************************************
//準安定蒸気域の補助式
//**********************
const static	double r2_no[10] = 			//1a,2aなど式(16),(18)用の係数
			{-999999.999,		
			-0.96937268393049e+1,
			 0.10087275970006e+2,
			-0.56087911283020e-2,
			 0.71452738081455e-1,
			-0.40710498223928,
			 0.14240819171444e+1,
			-0.43839511319450e+1,
			-0.28408632460772,
			 0.21268463753307e-1	};

//以下は、準安定蒸気の理想気体項式と導関数
// 16p Table13 γoで表されている関数
double	r2_Gam_o(double pai,	double tau)	{
	double	a	=	0.0;	//作業用
	unsigned	i;
	for(i = 1;	i <= 9;	i++){
		a	+=	r2_no[i]
				*	pow(tau, (double)r2Jo[i]);
	}
	return	log(pai) + a;
}

// 16p Table13 γoπで表されている関数
double	r2_Gam_oPai(double pai)	{
	return	1/pai + 0.0;	//意味なく原本に忠実に書いた。
}

// 16p Table13 γoππで表されている関数
double	r2_Gam_oPaiPai(double pai)	{
	return	-1/pow(pai, 2.0) + 0.0;	//意味なく原本に忠実に書いた。
}

// 16p Table13 γoτで表されている関数
double	r2_Gam_oTau(double tau)	{
	double	a	=	0.0;	//作業用
	unsigned	i;
	for(i = 1;	i <= 9;	i++){
		a	+=	r2_no[i]
				*	(double)r2Jo[i]
					*	pow(tau, (double)(r2Jo[i] - 1));
	}
	return	0.0 + a;	//意味なく原本に忠実に書いた。
}

// 16p Table13 γoττで表されている関数
double	r2_Gam_oTauTau(double tau)	{
	double	a	=	0.0;	//作業用
	unsigned	i;
	for(i = 1;	i <= 9;	i++){
		a	+=	r2_no[i]
				*	(double)(r2Jo[i]	*	(r2Jo[i] - 1)	)
					*	pow(tau, (double)(r2Jo[i] - 2));
	}
	return	0.0 + a;	//意味なく原本に忠実に書いた。
}

// 16p Table13 γoττで表されている関数
double	r2_Gam_oPaiTau(void)	{
	return	0.0 + 0.0;
}

//Table 16	18p	準安定蒸気域の為の残余項式の係数
const static	int r2_I[14] =
	{ -999999,	1,	1,	1,	1,	2,	2,	2,	3,	3,
			4,	4,	5,	5		};

const static	int r2_J[14] =
	{ -999999,	0,	2,	5,	11,	1,	7,	16,	4,	16,
			7,	10,	9,	10		};

const static	double r2_n[14] = 	
		{	-999999.999,
			-0.73362260186506e-2,
			-0.88223831943146e-1,
			-0.72334555213245e-1,
			-0.40813178534455e-2,
			 0.20097803380207e-2,
			-0.53045921898642e-1,
			-0.76190409086970e-2,
			-0.63498037657313e-2,
			-0.86043093028588e-1,
			 0.75321581522770e-2,
			-0.79238375446139e-2,
			-0.22888160778447e-3,
			-0.26456501482810e-2		};

// 以下準安定蒸気の残余項式と導関数
// 19p Table17 γrで表されている関数
double	r2_Gam_r(double pai,	double tau)	{
	double	a	=	0.0;	//作業用
	unsigned	i;
	for(i = 1;	i <= 13;	i++){
		a	+=	r2_n[i] * pow(pai,(double)r2_I[i])
				*	pow(tau - 0.5,(double)r2_J[i]);
	}
	return	a;
}

// 19p Table17 γrπで表されている関数
double	r2_Gam_rPai(double pai,	double tau)	{
	double	a	=	0.0;	//作業用
	unsigned	i;
	for(i = 1;	i <= 13;	i++){
		a	+=	r2_n[i] * (double)r2_I[i]
				* pow(pai,(double)(r2_I[i]-1))
					*	pow(tau - 0.5,(double)r2_J[i]);
	}
	return	a;
}

// 19p Table17 γrτで表されている関数
double	r2_Gam_rTau(double pai,	double tau)	{
	double	a	=	0.0;	//作業用
	unsigned	i;
	for(i = 1;	i <= 13;	i++){
		a	+=	r2_n[i]	* pow(pai,(double)r2_I[i])
				*	(double)r2_J[i]	*	pow(tau - 0.5,(double)(r2_J[i] - 1));
	}
	return	a;
}

// 19p Table17 γrππで表されている関数
double	r2_Gam_rPaiPai(double pai,	double tau)	{
	double	a	=	0.0;	//作業用
	unsigned	i;
	for(i = 1;	i <= 13;	i++){
		a	+=	r2_n[i]	*(double)(r2_I[i]*(r2_I[i] - 1))
				* pow(pai,(double)(r2_I[i]-2))
					*	pow(tau - 0.5,(double)r2_J[i]);
	}
	return	a;
}

// 19p Table17 γrττで表されている関数
double	r2_Gam_rTauTau(double pai,	double tau)	{
	double	a	=	0.0;	//作業用
	unsigned	i;
	for(i = 1;	i <= 13;	i++){
		a	+=	r2_n[i]	* pow(pai,(double)r2_I[i])
				*(double)(r2_J[i]*(r2_J[i] - 1))
					*	pow(tau - 0.5,(double)(r2_J[i] -2));
	}
	return	a;
}

// 19p Table17 γrπτで表されている関数
double	r2_Gam_rPaiTau(double pai,	double tau)	{
	double	a	=	0.0;	//作業用
	unsigned	i;
	for(i = 1;	i <= 13;	i++){
		a	+=	r2_n[i]	*	(double)r2_I[i]
			* pow(pai,(double)(r2_I[i] - 1))
				*	(double)r2_J[i]
					*	pow(tau - 0.5,(double)(r2_J[i] -1));
	}
	return	a;
}
//	***
// 15p	領域2準安定	比容積[m3/Kg]
double	r2_tp2v(double t,	double p)	{	//t[K],	p[MPa]
	double	a;	//作業用
	double	v;
	double	pai	=	p/r2Ps;
	double	tau	=	r2Ts/t;
	a	=	pai * (r2_Gam_oPai(pai)	+	r2_Gam_rPai(pai,tau)	);
	v	=	IF97_Rspec * t / p * a;
//	return	v;
	return	v * 0.001;
}

// 15p	領域2準安定	内部エネルギー[KJ/Kg]
double	r2_tp2u(double t,	double p)	{	//t[K],	p[MPa]
	double	a;	//作業用
	double	u;
	double	pai	=	p/r2Ps;
	double	tau	=	r2Ts/t;
	a	=	tau * (r2_Gam_oTau(tau) + r2_Gam_rTau(pai,tau))
				- pai * (r2_Gam_oPai(pai) + r2_Gam_rPai(pai,tau));
	u	=	IF97_Rspec * t * a;
	return	u;
}

// 15p	領域2準安定	比エントロピー[KJ//KgK]
double	r2_tp2s(double t,	double p)	{	//t[K],	p[MPa]
	double	a;	//作業
	double	s;
	double	pai	=	p/r2Ps;
	double	tau	=	r2Ts/t;

	a	=	tau * (r2_Gam_oTau(tau) + r2_Gam_rTau(pai,tau))
				- (r2_Gam_o(pai,tau) + r2_Gam_r(pai,tau));
	s	=	IF97_Rspec * a;
	return	s;
}

// 15p	領域2準安定	比エンタルピー[KJ//Kg]
double	r2_tp2h(double t,	double p)	{	//t[K],	p[MPa]
	double	a;	//作業用
	double	h;
	double	pai	=	p/r2Ps;
	double	tau	=	r2Ts/t;
	a	=	tau * (r2_Gam_oTau(tau) + r2_Gam_rTau(pai,tau));
	h	=	IF97_Rspec * t * a;
	return	h;
}

// 15p	領域2準安定	Cp	定圧比熱[KJ/KgK]
double	r2_tp2Cp(double t,	double p)	{	//t[K],	p[MPa]
	double	a;	//作業用
	double	Cp;
	double	pai	=	p/r2Ps;
	double	tau	=	r2Ts/t;
	a	=	(-pow(tau,	2.0)) * (r2_Gam_oTauTau(tau) + r2_Gam_rTauTau(pai,tau));
	Cp	=	IF97_Rspec * a;
	return	Cp;
}

// 15p	領域2準安定	Cv	定積比熱[KJ/KgK]
double	r2_tp2Cv(double t,	double p)	{	//t[K],	p[MPa]
	double	a,	b;	//作業用
	double	Cv,	Cp;
	double	pai	=	p/r2Ps;
	double	tau	=	r2Ts/t;
	Cp	=	(-pow(tau,	2.0)) * (r2_Gam_oTauTau(tau) + r2_Gam_rTauTau(pai,tau));
	a	=	(-pow(1.0 + pai * r2_Gam_rPai(pai,tau) - tau * pai * r2_Gam_rPaiTau(pai,tau),	2.0)	);
	b	=	1.0 - pow(pai, 2.0) * r2_Gam_rPaiPai(pai,	tau);
	Cv	=	IF97_Rspec * (Cp + a/b);
	return	Cv;
}

// 15p	領域2準安定	w	音速[m/s]
double	r2_tp2w(double t,	double p)	{	//t[K],	p[MPa]
	double	w;
	double	a,	b,	c,	d;	//作業用
	double	tau	=	(r2Ts/t);
	double	pai	=	p/r2Ps;
	double	dGam_rPai	=	r2_Gam_rPai(pai,tau);

	a	=	1.0 + 2.0*pai*dGam_rPai + pow(pai,2.0)*pow(dGam_rPai,2.0);
	b	=	1.0 - pow(pai,2.0)*r2_Gam_rPaiPai(pai,tau);
	c	=	1.0 + pai*dGam_rPai - tau*pai*r2_Gam_rPaiTau(pai,tau);
	d	=	pow(tau,2.0)*(r2_Gam_oTauTau(tau) + r2_Gam_rTauTau(pai,tau));

	w	=	sqrt(	IF97_Rspec * t * a/(b + pow(c,2.0)/d)	);
	return	w;
}

