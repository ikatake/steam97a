/*****************************************************
	International Association for Properties of Water and Steam
	実用国際状態式（1997年）IF97

	This is a part of IF97.
	in C language Coded by Takayoshi Tabuchi.
*****************************************************/

/*
1)変換用以外のプログラム中では特別に記述がない時は，
  温度[K], 圧力[MPa], 密度[Kg/m3],エントロピー[kJ/KgK],
  比容積[m3/Kg]
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

//	*******	領域 3	******************************
//	30p
const static	double	r3Ds	=	IF97_Dc;	//[kg/m3]	Release文では、ρ*と記されているもの
const static	double	r3Ts	=	IF97_Tc;	//[K]	Release文では、T*と記されているもの
//	Table 30	30p
const static	int	r3I[41] =	{ -999999,
		0,	0,	0,	0,	0,	0,	0,	0,	1,
	1,	1,	1,	2,	2,	2,	2,	2,	2,	3,
	3,	3,	3,	3,	4,	4,	4,	4,	5,	5,
	5,	6,	6,	6,	7,	8,	9,	9,	10,	10,
	11		};
const static	int	r3J[41] =	{ -999999,
		0,	0,	1,	2,	7,	10,	12,	23,	2,
	6,	15,	17,	0,	2,	6,	7,	22,	26,	0,
	2,	4,	16,	26,	0,	2,	4,	26,	1,	3,
	26,	0,	2,	26,	2,	26,	2,	26,	0,	1,
	26		};
//	0	1,	2	3	4	5	6	7	8	9	

const static	double r3n[41] = 	{-999999.999,
			 0.10658070028513e+1,
			-0.15732845290239e+2,
			 0.20944396974307e+2,
			-0.76867707878716e+1,
			 0.26185947787954e+1,
			-0.28080781148620e+1,
			 0.12053369696517e+1,
			-0.84566812812502e-2,
			-0.12654315477714e+1,
			-0.11524407806681e+1,	//10
			 0.88521043984318,
			-0.64207765181607,
			 0.38493460186671,
			-0.85214708824206,
			 0.48972281541877e+1,
			-0.30502617256965e+1,
			 0.39420536879154e-1,
			 0.12558408424308,
			-0.27999329698710,
			 0.13899799569460e+1,	//20
			-0.20189915023570e+1,
			-0.82147637173963e-2,
			-0.47596035734923,
			 0.43984074473500e-1,
			-0.44476435428739,
			 0.90572070719733,
			 0.70522450087967,
			 0.10770512626332,
			-0.32913623258954,
			-0.50871062041158,		//30
			-0.22175400873096e-1,
			 0.94260751665092e-1,
			 0.16436278447961,
			-0.13503372241348e-1,
			-0.14834345352472e-1,
			 0.57922953628084e-3,
			 0.32308904703711e-2,
			 0.80964802996215e-4,
			-0.16557679795037e-3,
			-0.44923899061815e-4	};	//40

// 31p Table32 φで表されている関数
double	r3Fai(double delta,	double tau)	{
	double	a	=	0.0;		//作業用
	int	i;
	for(i = 2;	i<=40;	i++)	{
		a	+=	r3n[i]
				*	pow(delta,	(double)r3I[i])
					*	pow(tau,	(double)r3J[i]);
	}
	return	r3n[1] * log(delta) + a;
}

// 31p Table32 φδで表されている関数
double	r3Fai_Delta(double delta,	double tau)	{
	double	a	=	0.0;		//作業用
	int	i;
	for(i = 2;	i<=40;	i++)	{
		a	+=	r3n[i]	*	(double)r3I[i]
				*	pow(delta,	(double)(r3I[i] -1)	)
					*	pow(tau,	(double)r3J[i]);
	}
	return	r3n[1]/delta + a;
}

// 31p Table32 φτで表されている関数
double	r3Fai_Tau(double delta,	double tau)	{
	double	a	=	0.0;		//作業用
	int	i;
	for(i = 2;	i<=40;	i++)	{
		a	+=	r3n[i]	
				*	pow(delta,	(double)r3I[i]	)
					*	(double)r3J[i]
						*	pow(tau,	(double)(r3J[i]-1)	);
	}
	return	0.0	+	a;
}

// 31p Table32 φδδで表されている関数
double	r3Fai_DeltaDelta(double delta,	double tau)	{
	double	a	=	0.0;		//作業用
	int	i;
	for(i = 2;	i<=40;	i++)	{
		a	+=	r3n[i]	*	(double)(r3I[i] *(r3I[i] - 1))
				*	pow(delta,	(double)(r3I[i]	- 2))
					*	pow(tau,	(double)r3J[i]	);
	}
	return	-r3n[1]/pow(delta,	2.0)	+	a;
}

// 31p Table32 φττで表されている関数
double	r3Fai_TauTau(double delta,	double tau)	{
	double	a	=	0.0;		//作業用
	int	i;
	for(i = 2;	i<=40;	i++)	{
		a	+=	r3n[i]
				*	pow(delta,	(double)r3I[i])
					*	(double)(r3J[i] *(r3J[i] - 1))
						*	pow(tau,	(double)(r3J[i] - 2)	);
	}
	return	0.0	+	a;
}

// 31p Table32 φδτで表されている関数
double	r3Fai_DeltaTau(double delta,	double tau)	{
	double	a	=	0.0;		//作業用
	int	i;
	for(i = 2;	i<=40;	i++)	{
		a	+=	r3n[i]	*	(double)r3I[i] 
				*	pow(delta,	(double)(r3I[i] - 1))
					*	(double)r3J[i]
						*	pow(tau, (double)(r3J[i] - 1)	);
	}
	return	0.0	+	a;
}

//一定温度での (dP/dv)を比体積と温度から算出する。
//JSME の蒸気表(FORTRAN)ソースコード(dPdV_3)を参考に記述
double r3dt2dPdV(double v,	double t)	{	//return dP/dv [MPa/(m^3/kg)]
			// v [m3/kg] = 1/d 
	double	d	=	1.0 / v;
	double	delta	=	d/r3Ds;	//δ
	double	tau	=	r3Ts / t;	//τ

	double	a	=	r3n[1];		//作業用
	int	i;
	for(i = 2;	i<=40;	i++)	{
		a	+=	r3n[i]	*	(double)r3I[i] 
				*	((double)r3I[i]  + 1.0)
					*	pow(delta,	(double)r3I[i] )
						*	pow(tau, (double)r3J[i] );
	}
	return	(-t * IF97_Rspec * 0.001 * d * d * a);
}

// 31p	領域3	密度、温度→圧力[MPa]
double	r3dt2p(double d,	double t)	{	//d[kg/m3],	t[K]
	double	delta	=	d/r3Ds;	//δ
	double	tau	=	r3Ts/t;		//τ
	double	p;
	p	=	IF97_Rspec * t * d * delta * r3Fai_Delta(delta,	tau);
//	return	p;
	return	p * 0.001;
}

// 31p	領域3	密度、温度→内部エネルギー[KJ/Kg]
double	r3dt2u(double d,	double t)	{	//d[kg/m3],	t[K]
	double	delta	=	d/r3Ds;	//δ
	double	tau	=	r3Ts/t;		//τ
	double	u;
	u	=	IF97_Rspec * t * tau * r3Fai_Tau(delta,	tau);
	return	u;
}

// 31p	領域3	密度、温度→エントロピー[kJ/KgK]
double	r3dt2s(double d,	double t)	{	//d[kg/m3],	t[K]
	double	delta	=	d/r3Ds;	//δ
	double	tau	=	r3Ts/t;		//τ
	double	s;
	s	=	IF97_Rspec
			* (tau * r3Fai_Tau(delta,	tau) - r3Fai(delta,tau)	);
	return	s;
}

// 31p	領域3	密度、温度→エンタルピー[kJ/Kg]
double	r3dt2h(double d,	double t)	{	//d[kg/m3],	t[K]
	double	delta	=	d/r3Ds;	//δ
	double	tau	=	r3Ts/t;		//τ
	double	h;
	double	a;	//for temp.
	a	=	tau * r3Fai_Tau(delta,tau) + delta * r3Fai_Delta(delta,tau);
	h	=	IF97_Rspec * t * a;
	return	h;
}

// 31p	領域3	密度、温度→定積比熱[kJ/KgK]
double	r3dt2Cv(double d,	double t)	{	//d[kg/m3],	t[K]
	double	delta	=	d/r3Ds;	//δ
	double	tau	=	r3Ts/t;		//τ
	double	Cv;
	double	a;	//for temp.
	a	=	(-pow(tau, 2.0) * r3Fai_TauTau(delta,tau));
	Cv	=	IF97_Rspec * a;
	return	Cv;
}

// 31p	領域3	密度、温度→定圧比熱[kJ/KgK]
double	r3dt2Cp(double d,	double t)	{	//d[kg/m3],	t[K]
	double	delta	=	d/r3Ds;	//δ
	double	tau	=	r3Ts/t;		//τ
	double	Cp;
	double	a,b,c;	//for temp.
	a	=	(-pow(tau, 2.0) * r3Fai_TauTau(delta,tau));
	b	=	delta * r3Fai_Delta(delta,tau) - delta * tau *r3Fai_DeltaTau(delta,tau);
	c	=	2.0 * delta * r3Fai_Delta(delta,tau) + pow(delta,2.0) * r3Fai_DeltaDelta(delta,tau);
	Cp	=	IF97_Rspec * (	a + pow(b,2.0)/c	);
	return	Cp;
}

// 31p	領域3	密度、温度→音速[m/s]
double	r3dt2w(double d,	double t)	{	//d[kg/m3],	t[K]
	double	delta	=	d/r3Ds;	//δ
	double	tau	=	r3Ts/t;		//τ
	double	w;
	double	a,b,c,f,e;	//for temp.
	a	=	2.0 * delta * r3Fai_Delta(delta,tau);
	b	=	delta*delta * r3Fai_DeltaDelta(delta,tau);
	c	=	delta * r3Fai_Delta(delta,tau) - delta * tau *r3Fai_DeltaTau(delta,tau);
	f	=	tau*tau * r3Fai_TauTau(delta,tau);
	e	=	a + b - c*c/f;
	if(e < 0.0)	e	=	0.0;
	w	=	sqrt(IF97_Rspec * t * e * 1000.0);
	return	w;
}
 
// 日本機会学会original	領域3	K	等エントロピー指数(断熱指数:isentropic exponent)
//	密度、温度→K(等エントロピー指数)
double	r3dt2k(double d,	double t)	{	//d[kg/m3],	t[K]
	double	delta	=	d/r3Ds;	//δ
	double	tau	=	r3Ts/t;		//τ
	double	k;
	double	b,c,f;	//for temp.
	b	=	delta * r3Fai_DeltaDelta(delta,tau) / r3Fai_Delta(delta,tau);
	c	=	r3Fai_Delta(delta,tau) - tau *r3Fai_DeltaTau(delta,tau);
	f	=	tau*tau * r3Fai_Delta(delta,tau) * r3Fai_TauTau(delta,tau);
	k	=	2.0 + b - delta * c*c/f;
	return	k;
}


//	*******	領域 4	******************************
//for (30) & (31)
const static	double	r4Ps	=	1.0;	//[MPa]	Release文では、p*と記されているもの
const static	double	r4Ts	=	1.0;	//[K]	Release文では、T*と記されているもの
//
const static	double	r4n[11]=	{-999999.999,
							 0.11670521452767e+4,
							-0.72421316703206e+6,
							-0.17073846940092e+2,
							 0.12020824702470e+5,
							-0.32325550322333e+7,
							 0.14915108613530e+2,
							-0.48232657361591e+4,
							 0.40511340542057e+6,
							-0.23855557567849,
							 0.65017534844798e+3	};

//領域4	飽和域の式
//273.15{K] <= t <= 647.096[K]
//33p	(30)式	温度→飽和圧力
double	r4t2p(double	t)	{	//t:温度[K]
	double	pai,	theta,	wA,	wB,	wC;
	if(t/r4Ts == r4n[10])	return	STMOHT_ERROR;
	theta	=	t/r4Ts + r4n[9]/(t/r4Ts - r4n[10]);
	wA	=	pow(theta,2.0) + r4n[1]*theta + r4n[2];
	wB	=	r4n[3]*pow(theta,2.0) + r4n[4]*theta + r4n[5];
	wC	=	r4n[6]*pow(theta,2.0) + r4n[7]*theta + r4n[8];

	pai	=	2.0*wC/( (-wB) + sqrt(pow(wB, 2.0) - 4.0 * wA * wC));
	pai	=	pow(pai,	4.0);

	return	(pai*r4Ps);
}

//領域4	飽和域の式
//611.213[Pa] <= p <= 22.064[MPa]
//34p	(31)式	圧力→飽和温度
double	r4p2t(double	p)	{	//p:温度[MPa]
	double	t2,	beta,	wD,	wE,	wF,	wG;

	beta	=	pow(p/r4Ps, 0.25);
	wE	=	pow(beta, 2.0) + r4n[3]*beta + r4n[6];
	wF	=	r4n[1]*pow(beta, 2.0) + r4n[4]*beta + r4n[7];
	wG	=	r4n[2]*pow(beta, 2.0) + r4n[5]*beta + r4n[8];
	wD	=	2.0*wG/((-wF) - sqrt(pow(wF, 2.0) - 4.0 * wE * wG	)	);

	t2	=	(r4n[10] + wD - sqrt(pow(r4n[10]+wD,2.0)	- 4.0*(r4n[9] + r4n[10]*wD)))
			/2.0;
	return	(t2*r4Ts);
}
