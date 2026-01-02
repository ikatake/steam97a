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

//	*******	領域1	******************************
const static	double	r1Ps	=	16.53;	//[MPa]	Release文では、p*と記されているもの
const static	double	r1Ts	=	1386.0;	//[K]	Release文では、T*と記されているもの
//	Table 2	7p
const static	int r1I[35] =
	{ -999999,	0,	0,	0,	0,	0,	0,	0,	0,	1,
			1,	1,	1,	1,	1,	2,	2,	2,	2,	2,
			3,	3,	3,	4,	4,	4,	5,	8,	8,	21,
			23,	29,	30,	31,	32		};

const static	int r1J[35] =
	{ -999999,	-2,	-1,	0,	1,	2,	3,	4,	5,	-9,
			-7,	-1,	0,	1,	3,	-3,	0,	1,	3,	17,
			-4,	0,	6,	-5,	-2,	10,	-8,-11,	-6,	-29,
			-31,-38,-39,-40,-41		};

const static	double r1n[35] = {-999999.999,		
		 0.14632971213167e+0,
		-0.84548187169114e+0,
		-0.37563603672040e+1,
		 0.33855169168385e+1,
		-0.95791963387872e+0,
		 0.15772038513228e+0,
		-0.16616417199501e-1,
		 0.81214629983568e-3,
		 0.28319080123804e-3,
		-0.60706301565874e-3,
		-0.18990068218419e-1,
		-0.32529748770505e-1,
		-0.21841717175414e-1,
		-0.52838357969930e-4,
		-0.47184321073267e-3,
		-0.30001780793026e-3,
		 0.47661393906987e-4,
		-0.44141845330846e-5,
		-0.72694996297594e-15,
		-0.31679644845054e-4,
		-0.28270797985312e-5,
		-0.85205128120103e-9,
		-0.22425281908000e-5,
		-0.65171222895601e-6,
		-0.14341729937924e-12,
		-0.40516996860117e-6,
		-0.12734301741641e-8,
		-0.17424871230634e-9,
		-0.68762131295531e-18,
		 0.14478307828521e-19,
		 0.26335781662795e-22,
		-0.11947622640071e-22,
		 0.18228094581404e-23,
		-0.93537087292458e-25	};

// 8p γで表されている関数
double	r1Gam(double pai,	double tau)	{
	double	w	=	0.0;	//作業用
	unsigned	i;
	for(i = 1;	i <= 34;	i++){
		w	+=	r1n[i]
				*	pow(7.1 - pai, (double)r1I[i])
					*	pow(tau - 1.222, (double)r1J[i]);
	}
	return	w;
}

// 8p γπで表されている関数
double	r1GamPai(double pai,	double tau)	{
	double	w	=	0.0;	//作業用
	unsigned	i;
	for(i = 1;	i <= 34;	i++){
		w	+=	(-r1n[i]) * (double)r1I[i]
				*	pow(7.1 - pai, (double)(r1I[i] - 1))
					*	pow(tau - 1.222, (double)r1J[i]);
	}
	return	w;
}

// 8p γτで表されている関数
double	r1GamTau(double pai,	double tau)	{
	double	w	=	0.0;	//作業用
	unsigned	i;
	for(i = 1;	i <= 34;	i++){
		w	+=	r1n[i]
				*	pow(7.1 - pai, (double)r1I[i] )
					*	(double)r1J[i]	*	pow(tau - 1.222, (double)(r1J[i] - 1));
	}
return	w;
}

// 8p γππで表されている関数
double	r1GamPaiPai(double pai,	double tau)	{
	double	w	=	0.0;	//作業用
	unsigned	i;
	for( i = 1;	i <= 34;	i++){
		w	+=	r1n[i] * (double)( r1I[i]*(r1I[i] - 1) )
				*	pow(7.1 - pai, (double)(r1I[i] - 2))
					*	pow(tau - 1.222, (double)r1J[i]);
	}
	return	w;
}

// 8p γττで表されている関数
double	r1GamTauTau(double pai,	double tau)	{
	double	w	=	0.0;	//作業用
	unsigned	i;
	for( i = 1;	i <= 34;	i++){
		w	+=	r1n[i]	*	pow(7.1 - pai, (double)r1I[i])
					* (double)(r1J[i] *	(r1J[i] - 1))
						*	pow(tau - 1.222, (double)(r1J[i] - 2));
	}
	return	w;
}

// 8p γπτで表されている関数
double	r1GamPaiTau(double pai,	double tau)	{
	double	a	=	0.0;	//作業用
	unsigned	i;
	for( i = 1;	i <= 34;	i++){
		a	+=	(-r1n[i])
				*	(double)r1I[i]	*	pow(7.1 - pai, (double)(r1I[i] - 1))
					* (double)r1J[i]	*	pow(tau - 1.222, (double)(r1J[i] - 1));
	}
	return	a;
}

// 8p	領域1	比容積[m3/Kg]
double	r1tp2v(double t,	double p)	{	//t[K],	p[MPa]
	double	pai	=	p/r1Ps;
	double	tau	=	r1Ts/t;
	double	v;
//	v	=	IF97_Rspec * t / p * pai * r1GamPai(pai,	tau);
	v	=	IF97_Rspec * t /r1Ps * r1GamPai(pai,	tau);
//	return	v;
	return	v * 0.001;
}

// 8p	領域1	内部エネルギー[KJ//Kg]
double	r1tp2u(double t,	double p)	{	//t[K],	p[MPa]
	double	pai	=	p/r1Ps;
	double	tau	=	r1Ts/t;
	double	u;
	u	=	IF97_Rspec * t *(	tau * r1GamTau(pai,tau) - pai * r1GamPai(pai,tau)	);
	return	u;
}

// 8p	領域1	比エントロピー[KJ//KgK]
double	r1tp2s(double t,	double p)	{	//t[K],	p[MPa]
	double	pai	=	p/r1Ps;
	double	tau	=	r1Ts/t;
	double	s;
	s	=	IF97_Rspec*(	tau * r1GamTau(pai,tau) - r1Gam(pai,tau)	);
	return	s;
}

// 8p	領域1	比エンタルピー[KJ//Kg]
double	r1tp2h(double t,	double p)	{	//t[K],	p[MPa]
	double	pai	=	p/r1Ps;
	double	tau	=	r1Ts/t;
	double	h;
	h	=	IF97_Rspec * t * tau * r1GamTau(pai,	tau);
	return	h;
}

// 8p	領域1	Cp	定圧比熱[KJ/KgK]
double	r1tp2Cp(double t,	double p)	{	//t[K],	p[MPa]
	double	pai	=	p/r1Ps;
	double	tau	=	r1Ts/t;
	double	Cp;
	Cp	=	IF97_Rspec * (-pow(tau, 2.0)) * r1GamTauTau(pai,	tau);
	return	Cp;
}

// 8p	領域1	Cv	定積比熱[KJ/KgK]
double	r1tp2Cv(double t,	double p)	{	//t[K],	p[MPa]
	double	pai	=	p/r1Ps;
	double	tau	=	r1Ts/t;
	double	Cv;
	Cv	=	IF97_Rspec 
			* (-pow(tau, 2.0) * r1GamTauTau(pai,	tau)
				+ pow(r1GamPai(pai,	tau) - tau*r1GamPaiTau(pai,	tau),2.0)/r1GamPaiPai(pai,	tau));
	return	Cv;
}

// 8p	領域1	w	音速[m/s]
double	r1tp2w(double t,	double p)	{	//t[K],	p[MPa]
	double	w;
	double	w1,	w2,	w3,	a;	//for tempo.
	double	dGamPai;
	double	pai	=	p/r1Ps;
	double	tau	=	r1Ts/t;
	dGamPai	=	r1GamPai(pai,tau);
	w1	=	dGamPai*dGamPai;
	w2	=	pow(dGamPai - tau * r1GamPaiTau(pai,tau),	2.0);
	w3	=	tau*tau*r1GamTauTau(pai,tau);
	a	=	w1	/	(w2/w3 - r1GamPaiPai(pai,tau));
	if(a < 0.0	)	a = 0.0;
	w	=	sqrt(IF97_Rspec * t * a*1000.0);
	return	w;
}

// 日本機会学会original	領域1	K	等エントロピー指数(isentropic exponent)
double	r1tp2k(double t,	double p)	{	//t[K],	p[MPa]
	double	k;
	double	w1,	w2,	w3;	//for tempo.
	double	dGamPai;
	double	pai	=	p/r1Ps;
	double	tau	=	r1Ts/t;
	dGamPai	=	r1GamPai(pai,tau);
	w1	=	dGamPai / pai;
	w2	=	pow(dGamPai - tau * r1GamPaiTau(pai,tau),	2.0);
	w3	=	tau*tau*r1GamTauTau(pai,tau);
	k	=	w1	/	(w2/w3 - r1GamPaiPai(pai,tau));
	return	k;
}

//	*** 領域1	逆関数
const static	double	rev1Ps	=	1.0;	//[MPa]	原本では、p*と記されているもの
const static	double	rev1Ts	=	1.0;	//[K]	原本では、T*と記されているもの
const static	double	rev1Hs	=	2500.0;	//[KJ/Kg]	原本では、h*と記されているもの
const static	double	rev1Ss	=	1.0;	//[KJ/KgK]	原本では、s*と記されているもの

// 8p	領域1	圧力{MPa},比エンタルピー[KJ//Kg] → 温度[K]
double	r1ph2t(double p,	double h)	{	//p:圧力[MPa],	h:比エンタルピー
	//	10p	Table 6
	const static	int v1I[21] =
		{ -999999,	0,	0,	0,	0,	0,	0,	1,	1,	1,
				1,	1,	1,	1,	2,	2,	3,	3,	4,	5,
				6	};

	const static	int v1J[21] =
		{ -999999,	0,	1,	2,	6,	22,	32,	0,	1,	2,
				3,	4,	10,	32,	10,	32,	10,	32,	32,	32,
				32		};

	const static	double v1n[21] = {-999999.999,		
			-0.23872489924521e+3,
			 0.40421188637945e+3,
			 0.11349746881718e+3,
			-0.58457616048039e+1,
			-0.15285482413140e-3,
			-0.10866707695377e-5,
			-0.13391744872602e+2,
			 0.43211039183559e+2,
			-0.54010067170506e+2,
			 0.30535892203916e+2,
			-0.65964749423638e+1,
			 0.93965400878363e-2,
			 0.11573647505340e-6,
			-0.25858641282073e-4,
			-0.40644363084799e-8,
			 0.66456186191635e-7,
			 0.80670734103027e-10,
			-0.93477771213947e-12,
			 0.58265442020601e-14,
			-0.15020185953503e-16	};	
	
	double	eta	=	h/rev1Hs;	//η
	double	pai	=	p/rev1Ps;	//π
	double	theta	=	0.0;
	int	i;
	for(i = 1;	i<=20;	i++ )	{
		theta	+=	v1n[i] * pow(pai,(double)v1I[i]) * pow(eta + 1.0,(double)v1J[i]);
	}
	return	theta*rev1Ts;
}

// 8p	領域1	圧力{MPa},比エントロピー[KJ//KgK] → 温度[K]
double	r1ps2t(double p,	double s)	{	//p:圧力[MPa],	s:比エントロピー
	//	11p	Table 8
	const static	int v1I[21] =
		{ -999999,	0,	0,	0,	0,	0,	0,	1,	1,	1,
				1,	1,	1,	2,	2,	2,	2,	2,	3,	3,
				4	};

	const static	int v1J[21] =
		{ -999999,	0,	1,	2,	3,	11,	31,	0,	1,	2,
				3,	12,	31,	0,	1,	2,	9,	31,	10,	32,
				32		};

	const static	double v1n[21] = {-999999.999,		
			 0.17478268058307e+3,
			 0.34806930892873e+2,
			 0.65292584978455e+1,
			 0.33039981775489,
			-0.19281382923196e-6,
			-0.24909197244573e-22,
			-0.26107636489332,
			 0.22592965981586,
			-0.64256463395226e-1,
			 0.78876289270526e-2,
			 0.35672110607366e-9,
			 0.17332496994895e-23,
			 0.56608900654837e-3,
			-0.32635483139717e-3,
			 0.44778286690632e-4,
			-0.51322156908507e-9,
			-0.42522657042207e-25,
			 0.26400441360689e-12,
			 0.78124600459723e-28,
			-0.30732199903668e-30	};
	double	sigma	=	s/rev1Ss;	//σ
	double	pai	=	p/rev1Ps;	//π
	double	theta	=	0.0;
	int	i;
	for(i = 1;	i<=20;	i++ )	{
		theta	+=	v1n[i] * pow(pai,(double)v1I[i]) * pow(sigma + 2.0,(double)v1J[i]);
	}
	return	theta*rev1Ts;
}


