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

#include "if97.h"

//	*******	領域5	******************************
//	36p
const static	double	r5Ps	=	1.0;	//[MPa]	Release文では、p*と記されているもの
const static	double	r5Ts	=	1000.0;	//[K]	Release文では、T*と記されているもの
//	Table 37	36p
const static	int r5oJ[7] =	{ -999999,
				0,	1,	-3,	-2,	-1,	2	};

const static	double r5on[7] = {-999999.999,		
		-0.13179983674201e+2,
		 0.68540841634434e+1,
		-0.24805148933466e-1,
		 0.36901534980333,
		-0.31161318213925e+1,
		-0.32961626538917	};

// 38p γoで表されている関数
double	r5Gam_o(double pai,	double tau)	{
	double	a	=	0.0;	//作業用
	unsigned	i;
	for(i = 1;	i <= 6;	i++){
		a	+=	r5on[i] * pow(tau, (double)r5oJ[i]);
	}
	return	log(pai) + a;
}

// 38p γoπで表されている関数
double	r5Gam_oPai(double pai)	{
	return	1.0/pai + 0.0;	//無意味に原本忠実
}

// 38p γoππで表されている関数
double	r5Gam_oPaiPai(double pai)	{
	return	-1.0/pow(pai,	2.0) + 0.0;	//無意味に原本忠実
}

// 38p γoτで表されている関数
double	r5Gam_oTau(	double tau)	{
	double	a	=	0.0;	//作業用
	unsigned	i;
	for(i = 1;	i <= 6;	i++){
		a	+=	r5on[i] * r5oJ[i] * pow(tau, (double)(r5oJ[i] - 1));
	}
	return	0.0	+	a;	//無意味に原本忠実
}

// 38p γoττで表されている関数
double	r5Gam_oTauTau(double tau)	{
	double	a	=	0.0;	//作業用
	unsigned	i;
	for(i = 1;	i <= 6;	i++){
		a	+=	r5on[i] * (double)(r5oJ[i] * (r5oJ[i] - 1)) *pow(tau, (double)(r5oJ[i] - 2));
	}
	return	0.0	+	a;	//無意味に原本忠実
}

// 38p γoπτで表されている関数
double	r5Gam_oPaiTau()	{
	return	0.0	+	0.0;	//無意味に原本忠実
}

///領域5 残余項
const static	int r5I[6] =	{ -999999,
				1,	1,	1,	2,	3		};

const static	int r5J[6] =	{ -999999,
				0,	1,	3,	9,	3		};

const static	double r5n[6] = {-999999.999,		
		-0.12563183589592e-3,
		 0.21774678714571e-2,
		-0.45942820899910e-2,
		-0.39724828359569e-5,
		0.12919228289784e-6};

// 38p γrで表されている関数
double	r5Gam_r(double pai,	double tau)	{
	double	a	=	0.0;	//作業用
	unsigned	i;
	for(i = 1;	i <= 5;	i++){
		a	+=	r5n[i] * pow(pai, (double)r5I[i]) * pow(tau, (double)r5J[i]);
	}
	return	a;
}

// 38p γrπで表されている関数
double	r5Gam_rPai(double pai,	double tau)	{
	double	a	=	0.0;	//作業用
	unsigned	i;
	for(i = 1;	i <= 5;	i++){
		a	+=	r5n[i]
				* (double)r5I[i] * pow(pai, (double)(r5I[i] - 1))
					* pow(tau, (double)r5J[i]);
	}
	return	a;
}

// 38p γrππで表されている関数
double	r5Gam_rPaiPai(double pai,	double tau)	{
	double	a	=	0.0;	//作業用
	unsigned	i;
	for(i = 1;	i <= 5;	i++){
		a	+=	r5n[i]
				* (double)(r5I[i] * (r5I[i] - 1)) * pow(pai, (double)(r5I[i] - 2))
					* pow(tau, (double)r5J[i]);
	}
	return	a;
}

// 38p γrπτで表されている関数
double	r5Gam_rPaiTau(double pai,	double tau)	{
	double	a	=	0.0;	//作業用
	unsigned	i;
	for(i = 1;	i <= 5;	i++){
		a	+=	r5n[i]
				* (double)r5I[i] * pow(pai, (double)(r5I[i] - 1))
					* (double)r5J[i] * pow(tau, (double)(r5J[i] - 1));
	}
	return	a;
}

// 38p γrτで表されている関数
double	r5Gam_rTau(double pai,	double tau)	{
	double	a	=	0.0;	//作業用
	unsigned	i;
	for(i = 1;	i <= 5;	i++){
		a	+=	r5n[i]
				* pow(pai, (double)r5I[i])
					* (double)r5J[i] * pow(tau, (double)(r5J[i] - 1));
	}
	return	a;
}

// 38p γrττで表されている関数
double	r5Gam_rTauTau(double pai,	double tau)	{
	double	a	=	0.0;	//作業用
	unsigned	i;
	for(i = 1;	i <= 5;	i++){
		a	+=	r5n[i]
				* pow(pai, (double)r5I[i])
					* (double)(r5J[i] * (r5J[i] - 1)) * pow(tau, (double)(r5J[i] - 2));
	}
	return	a;
}

// 37p	領域5	比容積[m3/kg]
double	r5tp2v(double t,	double p)	{	//t[K],	p[MPa]
	double	pai	=	p/r5Ps;
	double	tau	=	r5Ts/t;
	double	a;
	double	v;
	a	=	pai * (r5Gam_oPai(pai) + r5Gam_rPai(pai,tau));
	v	=	IF97_Rspec * t / p * a;
//	return	v;
	return	v * 0.001;
}

// 37p	領域5	内部エネルギー[KJ//Kg]
double	r5tp2u(double t,	double p)	{	//t[K],	p[MPa]
	double	pai	=	p/r5Ps;
	double	tau	=	r5Ts/t;
	double	a;	//for temp
	double	u;
	a	=	tau * (r5Gam_oTau(tau) + r5Gam_rTau(pai,tau))
				- pai * (r5Gam_oPai(pai) + r5Gam_rPai(pai,tau));
	u	=	IF97_Rspec * t * a;
	return	u;
}


// 37p	領域5	比エントロピー[KJ//KgK]
double	r5tp2s(double t,	double p)	{	//t[K],	p[MPa]
	double	pai	=	p/r5Ps;
	double	tau	=	r5Ts/t;
	double	a;	//for temp
	double	s;
	a	=	tau * (r5Gam_oTau(tau) + r5Gam_rTau(pai,tau))
				- (r5Gam_o(pai,tau) + r5Gam_r(pai,tau));
	s	=	IF97_Rspec * a;
	return	s;
}

// 37p	領域5	比エンタルピー[KJ//Kg]
double	r5tp2h(double t,	double p)	{	//t[K],	p[MPa]
	double	pai	=	p/r5Ps;
	double	tau	=	r5Ts/t;
	double	a;
	double	h;
	a	=	tau * (r5Gam_oTau(tau) + r5Gam_rTau(pai,tau));
	h	=	IF97_Rspec * t * a;
	return	h;
}

// 37p	領域5	Cp	定圧比熱[KJ/KgK]
double	r5tp2Cp(double t,	double p)	{	//t[K],	p[MPa]
	double	pai	=	p/r5Ps;
	double	tau	=	r5Ts/t;
	double	a;
	double	Cp;
	a	=	(-pow(tau,2.0)) * (r5Gam_oTauTau(tau) + r5Gam_rTauTau(pai,tau)	);
	Cp	=	IF97_Rspec * a;
	return	Cp;
}

// 37p	領域5	Cv	定積比熱[KJ/KgK]
double	r5tp2Cv(double t,	double p)	{	//t[K],	p[MPa]
	double	pai	=	p/r5Ps;
	double	tau	=	r5Ts/t;
	double	a,b,c;	//for temp
	double	Cv;
	a	=	(-pow(tau,2.0)) * (r5Gam_oTauTau(tau) + r5Gam_rTauTau(pai,tau)	);
	b	=	1.0 + pai*r5Gam_rPai(pai,tau) - tau * pai * r5Gam_rPaiTau(pai,tau);
	c	=	1.0 - pow(pai,2.0) * r5Gam_rPaiPai(pai,tau);
	Cv	=	IF97_Rspec 	* (	a - pow(b,2.0)/c	);
	return	Cv;
}

// 37p	領域5	w	音速[m/s]
double	r5tp2w(double t,	double p)	{	//t[K],	p[MPa]
	double	w;
	double	a,	b,	c,	d,	w2;	//作業用
	double	tau	=	(r5Ts/t);
	double	pai	=	p/r5Ps;
	double	dGam_rPai	=	r5Gam_rPai(pai,tau);

//	a	=	1.0 + 2.0*pai*dGam_rPai + pow(pai,2.0)*pow(dGam_rPai,2.0);
	a	=	pow(1.0 + pai*dGam_rPai,	2.0);		//Nov./2000 式変形 
	b	=	1.0 - pai*pai*r5Gam_rPaiPai(pai,tau);
	c	=	1.0 + pai*dGam_rPai - tau*pai*r5Gam_rPaiTau(pai,tau);
	d	=	tau*tau*(r5Gam_oTauTau(tau) + r5Gam_rTauTau(pai,tau));
	w2	=	IF97_Rspec * t * a/(b + (c*c)/d) * 1000.0;
	if(w2 < 0.0)	w2 = 0.0;
	w	=	sqrt(	w2	);
	return	w;
}

// 日本機会学会original	領域5	K	等エントロピー指数(断熱指数:isentropic exponent)
double	r5tp2k(double t,	double p)	{	//t[K],	p[MPa]
	double	k;
	double	a,	b,	c,	d,	e;	//作業用
	double	tau	=	(r5Ts/t);
	double	pai	=	p/r5Ps;
	double	dGam_rPai	=	r5Gam_rPai(pai,tau);

	a	=	pow(1.0 + pai*dGam_rPai,	2.0);
	b	=	1.0 - pai*pai*r5Gam_rPaiPai(pai,tau);
	c	=	1.0 + pai*dGam_rPai - tau*pai*r5Gam_rPaiTau(pai,tau);
	d	=	tau*tau*(r5Gam_oTauTau(tau) + r5Gam_rTauTau(pai,tau));
	e	=	pai * (r5Gam_oPai(pai) + r5Gam_rPai(pai,	tau) );
	k	=	a/(b + (c*c)/d) / e;
	return	k;
}
