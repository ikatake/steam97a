//	*******************************************
//	逆関数 using IF97.c etc.
//		These functions are used in Steam97.c
//		presented by Takayoshi Tabuchi.
//

#include	<stdlib.h>
#include	<math.h>

#include	"if97back.h"


//	*****************	逆関数	****************

/*	Version 1.5以前のもの
//領域3	温度，圧力->密度	密度，温度->圧力の逆
//注意!	
//	IF97_R3MIN_TEMP < t < IF97_Tc,
//		b23t2p(IF97_R3MIN_TEMP) < p < IF97_Pc	を満たす時、
//	t,pの条件を満たす解(d:密度)は、3つある。しかし、
//	この関数は最初に見つけたd:密度のみ返す。
//r3dt2p(double d,	double t)を使用して２分法密度を算出
//	返値:d[kg/m3]
double	r3tp2d(double	t,	double	p)	{	//	t[K],	p[MPa]
	double	d;		//目標のd[kg/m3]
	double	d1,	d2;	//d1とd2の間で答えを探す	d1 < d2
	double	p1_t,	p2_t;	//
	double	pt;		//圧力誤差

	//入力温度チェック
	if(	t < IF97_R3MIN_TEMP	||	b23p2t(IF97_MAX_PRESS) < t)	{
		return	STMOHT_ERROR;
	}

	//入力圧力チェック
	if(p < b23t2p(t)	||	IF97_MAX_PRESS < p)	return	STMOHT_ERROR;

	//温度一定なら圧力が低いほうが密度は薄いと仮定
	d1	=	1.0 /(r2tp2v(t, b23t2p(t)) + V_R23_room);
	d2	=	1.0 /(r1tp2v(IF97_R3MIN_TEMP, IF97_MAX_PRESS) - V_R13_room);
	if(d1 < 0.0 || d2 < 0.0)	return	STMOHT_ERROR;

	//d1～d2で解を探す
	while(1)	{
		d	=	(d1 + d2)/2.0;
		pt	=	r3dt2p(d, t) - p;
		if(	-P_R3_tole<pt && pt<P_R3_tole )	break;	//許容誤差に入った

		p2_t	=	r3dt2p(d2, t) - p;
		if(	(pt< 0.0 && 0.0<p2_t)
				||	(0.0<pt && p2_t<0.0)	){	//d～d2の間に答え
			d1	=	d;
		}
		else	{
			p1_t	=	r3dt2p(d1, t) - p;
			if(	(pt< 0.0 && 0.0<p1_t)
					||	(0.0<pt && p1_t<0.0)	){	//d1～dの間に答え
				d2	=	d;
			}
			else	{	return	STMOHT_ERROR;	}
		}
	}
	return	(d);
}
*/

/*	Version 1.5以前のもの
//領域3	温度，圧力->密度	密度，温度->圧力の逆
//注意!	
//	IF97_R3MIN_TEMP < t < IF97_Tc,
//		b23t2p(IF97_R3MIN_TEMP) < p < IF97_Pc	の範囲では、
//	t,pの条件を満たす解(d:密度)は、3つある。
//	この関数は見つけたd:密度をすべて配列の形で返す。
//		(最後は、負値を終端子として書き込む)
//	但し、Delta_Dの範囲内に隣接して解が複数あった場合は検出できない。
//r3dt2p(double d,	double t)を使用して２分法密度を算出
//	返値:d[kg/m3](最後に見つけた)
double	r3tp2d_3(double	t,	double	p,	double*	ans,	int	k)	{	//	t[K],	p[MPa]
		//	ansは、double ans[4以上]のansを渡すこと
		//	k は、ansに確保している(double)の個数(=必要とする答えの個数 + 1)
	double	d;		//目標のd[kg/m3]
	double	do1,	do2;	//密度の範囲	do1 < do2
	double	d1,	d2;	//d1とd2の間で答えを探す	d1 < d2
	double	p1_t,	p2_t;	//
	double	pt;		//圧力誤差
	long	i;
	int		n = 0;		//答えの数

	//入力温度チェック
	if(	t < IF97_R3MIN_TEMP	||	IF97_Tc < t)	return	STMOHT_ERROR;

	//入力圧力チェック
	if(p < b23t2p(t)	||	IF97_Pc < p)	return	STMOHT_ERROR;

	//温度一定なら圧力が低いほうが密度は薄いと仮定
	do1	=	1.0 /(r2tp2v(t, b23t2p(t)) + V_R23_room);
	do2	=	1.0 /(r1tp2v(IF97_R3MIN_TEMP, IF97_Pc) - V_R13_room);
	if(do1 < 0.0 || do2 < 0.0)	return	STMOHT_ERROR;

	d1	=	0.0;
	for(i = 0;	d1 < do2;	i++	)	{
		d1	=	do1 + (double)i * Delta_D;
		d2	=	d1	+	Delta_D;
		//d1～d2で解を探す
		while(1)	{
			d	=	(d1 + d2)/2.0;
			pt	=	r3dt2p(d, t) - p;
			if(	-P_R3_tole<pt && pt<P_R3_tole )	{	//許容誤差に入った
				ans[n]	=	d;
				n++;
				break;
			}

			p2_t	=	r3dt2p(d2, t) - p;
			if(	(pt< 0.0 && 0.0<p2_t)
					||	(0.0<pt && p2_t<0.0)	){	//d～d2の間に答え
				d1	=	d;
			}
			else	{
				p1_t	=	r3dt2p(d1, t) - p;
				if(	(pt< 0.0 && 0.0<p1_t)
						||	(0.0<pt && p1_t<0.0)	){	//d1～dの間に答え
					d2	=	d;
				}
				else	{	break;	}	//d1～d2に解はない
			}
		}
		if(n >= k)	break;
	}
	ans[n]	=	STMOHT_ERROR;
	return	(ans[n-1]);
}
*/
  
//領域3	温度，圧力->密度	密度，温度->圧力の逆
//	Version 2.0以降の関数	日本機会学会発行 蒸気表(IF97)のCD-ROMを参考に
//	2000/09/13 Updated	2分法
//	r3dt2p(d, t)は気液分離域もサポートするが、この関数はサポートしない。
double	r3tp2d(double	t,	double	p)	{	//	t[K],	p[MPa]
	double	d;		//目標のd[kg/m3]
	double	d1,	d2;	//d1とd2の間で答えを探す	d1 < d2		
					//r3dt2p(d, t)はdについて単調増加関数で、d1とd2の間には
					//解が必ずたった一つあると仮定
	double	pt;		//圧力誤差
	int	i;

	//入力温度チェック
	if(	t < IF97_R3MIN_TEMP	||	b23p2t(IF97_MAX_PRESS) < t)	{
		return	STMOHT_ERROR;
	}

	//入力圧力チェック
	if(p < b23t2p(t)	||	IF97_MAX_PRESS < p)	return	STMOHT_ERROR;

	//臨界点温度以上
	if(IF97_Tc <= t)	{
		d1	=	1.0/R3Vmax;
		d2	=	1.0/R3Vmin;
	}	else	{	//臨界点温度 未満
		if(p < r4t2p(t))	{	//蒸気
			d1	=	1.0/R3Vmax;
			d2	=	1.0/r3t2v_g(t);
		}	else	{	//水
			d1	=	1.0/r3t2v_l(t);
			d2	=	1.0/R3Vmin;
		}
	}
	//d1～d2で解を探す
	for(i=0;	i<32000;	i++)	{
		d	=	(d1 + d2)/2.0;
		pt	=	r3dt2p(d, t) - p;
		if(	-P_R3_tole<pt && pt<P_R3_tole )	return	d;	//許容誤差に入った

		if(pt < 0)	{//d～d2の間に答え
			d1	=	d;	
		}
		else	{	//d1～dの間に答え
			d2	=	d;
		}
	}
	return	STMOHT_ERROR;	//解が見つけられなかった。
}

//	領域3
//	r3dt2s( d,	t)とr3tp2d(	t,	p)を使用して温度を逆演算
double	r3ps2t(double	p,	double	s)	{
	//	p[MPa],	s[KJ/kgK]
	double	t;		//目標のt[K]
	double	t1,	t2;	//t1とt2の間で答えを探す	t1 < t2
	//	r3dt2s(r3tp2d(	t,	p),	t)はtについて単調増加関数で、
	//	t1とt2の間には解が必ずたった一つあると仮定
	double	d;
	double	ds;		//エントロピー誤差
	int	i;

	//入力圧力チェック
	if(p < b23t2p(IF97_R3MIN_TEMP)	||	IF97_MAX_PRESS < p)	return	STMOHT_ERROR;

	t1	=	IF97_R3MIN_TEMP;
	t2	=	b23p2t(p);

	//t1～t2で解を探す
	for(i=0;	i<32000;	i++)	{
		t	=	(t1 + t2)/2.0;
		d	=	r3tp2d(	t,	p);
		if(d	<	0.0	)	return	d;	//r3tp2d(	t,	p)でError
		ds	=	r3dt2s(d,	t)	-	s;
		if(	-S_R3_tole<ds && ds<S_R3_tole )	return	t;	//許容誤差に入った

		if(ds < 0)	{//t～t2の間に答え
			t1	=	t;	
		}
		else	{	//t1～tの間に答え
			t2	=	t;
		}
	}
	return	STMOHT_ERROR;	//解が見つけられなかった。
}

//	領域3
//	r3dt2h( d,	t)とr3tp2d(	t,	p)を使用して温度を逆演算
double	r3ph2t(double	p,	double	h)	{
	//	p[MPa],	h[KJ/kg]
	double	t;		//目標のt[K]
	double	t1,	t2;	//t1とt2の間で答えを探す	t1 < t2
	//	r3dt2h(r3tp2d(	t,	p),	t)はtについて単調増加関数で、
	//	t1とt2の間には解が必ずたった一つあると仮定
	double	d;
	double	dh;		//エントロピー誤差
	int	i;

	//入力圧力チェック
	if(p < b23t2p(IF97_R3MIN_TEMP)	||	IF97_MAX_PRESS < p)	return	STMOHT_ERROR;

	t1	=	IF97_R3MIN_TEMP;
	t2	=	b23p2t(p);

	//t1～t2で解を探す
	for(i=0;	i<32000;	i++)	{
		t	=	(t1 + t2)/2.0;
		d	=	r3tp2d(	t,	p);
		if(d	<	0.0	)	return	d;	//r3tp2d(	t,	p)でError
		dh	=	r3dt2h(d,	t)	-	h;
		if(	-H_R3_tole<dh && dh<H_R3_tole )	return	t;	//許容誤差に入った

		if(dh < 0)	{//t～t2の間に答え
			t1	=	t;	
		}
		else	{	//t1～tの間に答え
			t2	=	t;
		}
	}
	return	STMOHT_ERROR;	//解が見つけられなかった。
}

//温度(t)[K]から領域3の液相側スピノーダル線上の比体積を計算	(2分法)
//	但し、挟み込みは (R3_Nspin)回しか繰り返さない。
//返値 v[m3/kg]
double	r3t2v_l(double	t)	{
	double	vm;
	double	v1	=	R3Vcrt;
	double	v2	=	R3Vmin;
	int	i;
	for(i=1; i<=R3_Nspin;	i++)	{
		vm	=	(v1 +v2) * 0.5;
		if(	r3dt2dPdV(vm, t)	>=	0.0	)	{
			v1	=	vm;
		}
		else	{
			v2	=	vm;
		}
	}
	return	vm;
}

//温度(t)[K]から領域3の気相側スピノーダル線上の比体積を計算	(2分法)
//	但し、挟み込みは (R3_Nspin)回しか繰り返さない。
//返値 v[m3/kg]
double	r3t2v_g(double	t)	{
	double	vm;
	double	v1	=	R3Vmax;
	double	v2	=	R3Vcrt;
	int	i;
	for(i=1; i<=R3_Nspin;	i++)	{
		vm	=	(v1 +v2) * 0.5;
		if(	r3dt2dPdV(vm, t)	<	0.0	)	{
			v1	=	vm;
		}
		else	{
			v2	=	vm;
		}
	}
	return	vm;
}
//領域3	飽和水の密度を温度から算出	温度→密度
//	r3dt2p(d, t)を使用して2分法で解く
double	r3t2d_l(double	t)	{	//	t[K]
	double	d;		//目標のd[kg/m3]
	double	d1,	d2;	//d1とd2の間で答えを探す	d1 < d2		
					//r3dt2p(d, t)はdについて単調増加関数で、d1とd2の間には
					//解が必ずたった一つあると仮定
	double	p;
	double	pt;		//圧力誤差
	int	i;

	//入力温度チェック
	if(	t < IF97_R3MIN_TEMP	||	IF97_Tc < t)	{
		return	STMOHT_ERROR;
	}

	p = r4t2p(t);
	d1	=	1.0/r3t2v_l(t);
	d2	=	1.0/R3Vmin;
	//d1～d2で解を探す
	for(i=0;	i<32000;	i++)	{
		d	=	(d1 + d2)/2.0;
		pt	=	r3dt2p(d, t) - p;
		if(	-P_R3_tole<pt && pt<P_R3_tole )	return	d;	//許容誤差に入った

		if(pt < 0)	{//d2～dの間に答え
			d1	=	d;	
		}
		else	{	//d1～dの間に答え
			d2	=	d;
		}
	}
	return	STMOHT_ERROR;	//解が見つけられなかった。
}

//領域3	飽和蒸気の密度を温度から算出	温度→密度
//	r3dt2p(d, t)を使用して2分法で解く
double	r3t2d_g(double	t)	{	//	t[K]
	double	d;		//目標のd[kg/m3]
	double	d1,	d2;	//d1とd2の間で答えを探す	d1 < d2		
					//r3dt2p(d, t)はdについて単調増加関数で、d1とd2の間には
					//解が必ずたった一つあると仮定
	double	p;
	double	pt;		//圧力誤差
	int	i;

	//入力温度チェック
	if(	t < IF97_R3MIN_TEMP	||	IF97_Tc < t)	{
		return	STMOHT_ERROR;
	}

	p = r4t2p(t);
	d1	=	1.0/R3Vmax;
	d2	=	1.0/r3t2v_g(t);
	//d1～d2で解を探す
	for(i=0;	i<32000;	i++)	{
		d	=	(d1 + d2)/2.0;
		pt	=	r3dt2p(d, t) - p;
		if(	-P_R3_tole<pt && pt<P_R3_tole )	return	d;	//許容誤差に入った

		if(pt < 0)	{//d2～dの間に答え
			d1	=	d;	
		}
		else	{	//d1～dの間に答え
			d2	=	d;
		}
	}
	return	STMOHT_ERROR;	//解が見つけられなかった。
}

//領域5	エントロピー，圧力->温度	温度，圧力->エントロピーの逆
//r5tp2s(double t,	double p)を使用して２分法密度を算出
//	返値:t[K]
double	r5sp2t(double	s,	double	p)	{	//	s[kJ/kgK],	p[MPa]
	double	t;		//目標のt[K]
	double	t1,	t2;	//t1とt2の間で答えを探す	t1 < t2
	double	s1_t,	s2_t;	//
	double	st;		//エントロピー誤差

	/*入力圧力チェック*/
	if(	p <= IF97_MIN_PRESS	||	IF97_R5MAX_PRESS < p)	{
		return	STMOHT_ERROR;
	}

	//圧力一定なら温度が低いほうがエントロピーは小さいと仮定
	if( s < r5tp2s(IF97_R5MIN_TEMP, p) || r5tp2s(IF97_MAX_TEMP, p) < s)	{
		return	STMOHT_ERROR;	//入力エントロピーエラー
	}

	t1	=	IF97_R5MIN_TEMP;
	t2	=	IF97_MAX_TEMP;
	/*t1～t2で解を探す*/
	while(1)	{
		t	=	(t1 + t2)/2.0;
		st	=	r5tp2s(t, p) - s;
		if(	-S_R5_tole<st && st<S_R5_tole )	break;	//許容誤差に入った

		s2_t	=	r5tp2s(t2, p) - s;
		if(	(st< 0.0 && 0.0<s2_t)
				||	(0.0<st && s2_t<0.0)	){	//t～t2の間に答え
			t1	=	t;
		}
		else	{
			s1_t	=	r5tp2s(t1, p) - s;
			if(	(st< 0.0 && 0.0<s1_t)
					||	(0.0<st && s1_t<0.0)	){	//t1～tの間に答え
				t2	=	t;
			}
			else	{	return	STMOHT_ERROR;	}
		}
	}
	return	(t);
}

//領域5	エンタルピー，圧力->温度	温度，圧力->エンタルピーの逆
//r5tp2h(double t,	double p)を使用して２分法密度を算出
//	返値:t[K]
double	r5hp2t(double	h,	double	p)	{	//	h[kJ/kg],	p[MPa]
	double	t;		//目標のt[K]
	double	t1,	t2;	//t1とt2の間で答えを探す	t1 < t2
	double	h1_t,	h2_t;	//
	double	ht;		//エントロピー誤差

	/*入力圧力チェック*/
	if(	p <= IF97_MIN_PRESS	||	IF97_R5MAX_PRESS < p)	{
		return	STMOHT_ERROR;
	}

	//圧力一定なら温度が低いほうがエンタルピーは小さいと仮定
	if( h < r5tp2h(IF97_R5MIN_TEMP, p) || r5tp2h(IF97_MAX_TEMP, p) < h)	{
		return	STMOHT_ERROR;	//入力エントロピーエラー
	}

	t1	=	IF97_R5MIN_TEMP;
	t2	=	IF97_MAX_TEMP;
	/*t1～t2で解を探す*/
	while(1)	{
		t	=	(t1 + t2)/2.0;
		ht	=	r5tp2h(t, p) - h;
		if(	-S_R5_tole<ht && ht<S_R5_tole )	break;	//許容誤差に入った

		h2_t	=	r5tp2h(t2, p) - h;
		if(	(ht< 0.0 && 0.0<h2_t)
				||	(0.0<ht && h2_t<0.0)	){	//t～t2の間に答え
			t1	=	t;
		}
		else	{
			h1_t	=	r5tp2h(t1, p) - h;
			if(	(ht< 0.0 && 0.0<h1_t)
					||	(0.0<ht && h1_t<0.0)	){	//t1～tの間に答え
				t2	=	t;
			}
			else	{	return	STMOHT_ERROR;	}
		}
	}
	return	(t);
}

//領域判定(Normal全領域)  圧力、エンタルピー から
//この関数は、JSME 蒸気表CD-ROM内のコードを参考にして作成	2000/01/24 関数追加
int	WhatIsRegPH(double	p,	double	h)	{	//入力	p:圧力[MPa] h:エンタルピー[kJ/kg]
	//返り値 領域の数字(以下)
	//	領域1				:	1
	//	領域2				:	2 
	//	350℃以下の湿り蒸気	:	12
	//	領域5				:	5
	//	領域3(臨界圧以上)	:	30
	//	領域3(液体)			:	31
	//	領域3(蒸気)			:	32
	//	350℃以上の湿り蒸気	:	33
	//	領域外				:(int)STMOHT_ERROR;	
	double	hTest;		//Testのための試験エンタルピー
	double	tTest;		//温度[K]
	double	p1;			//圧力[M Pa]
	double	d1, d2;		//密度[Kg/m3]

	if	(	p <= IF97_MIN_PRESS	)	return	(int)STMOHT_ERROR;	
	if(	p < IF97_R5MAX_PRESS)		{	//領域5の最高圧力以下
		//領域5から外れているか？(領域5のhが一番大きい)
		hTest	=	r5tp2h(IF97_MAX_TEMP, p);
		if(h > hTest)	{	return	(int)STMOHT_ERROR;	}
		else	{
			hTest	=	r2tp2h(IF97_R5MIN_TEMP, p);
			if(h >= hTest)	{	return	5;	}	//r2のhより大きいならr5
			else	{		//10MPa以下だから臨界圧以下
				tTest	=	r4p2t(p);	//p相当の飽和温度は?
				hTest	=	r2tp2h(tTest, p);	//飽和蒸気(乾き)のh?
				if(h >= hTest)	return	2;	//領域2
				hTest	=	r1tp2h(tTest, p);
				if(h > hTest)	return	12;	//領域12(湿り蒸気)
				hTest	=	r1tp2h(IF97_MIN_TEMP, p);
				if(h >= hTest)	return	1;
				else	return	(int)STMOHT_ERROR;	
			}
		}
	}
	else	if(	p <= IF97_MAX_PRESS)	{
		p1	=	r4t2p(IF97_R3MIN_TEMP);	//領域3最低温度での飽和蒸気圧
		if(p <= p1 )	{	//IF97_R5MAX_PRESS <= p <  p1
			//温度350℃の飽和圧力以下の領域
			hTest	=	r2tp2h(IF97_R5MIN_TEMP, p);
			if(h > hTest)	return	(int)STMOHT_ERROR;
			tTest	=	r4p2t(p);	//p相当の飽和温度は?
			hTest	=	r2tp2h(tTest, p);	//飽和蒸気(乾き)のh?
			if(h >= hTest)	return	2;
			hTest	=	r1tp2h(tTest, p);
			if(h > hTest)	return	12;	//領域12(湿り蒸気)
			hTest	=	r1tp2h(IF97_MIN_TEMP, p);
			if(h >= hTest)	return	1;
			else	return	(int)STMOHT_ERROR;	
		}
		else	if(p < IF97_Pc)	{	//臨界圧未満
			hTest	=	r2tp2h(IF97_R5MIN_TEMP, p);
			if(h > hTest)	return	(int)STMOHT_ERROR;
			tTest	=	b23p2t(p);	//領域2と3の境界線上
			hTest	=	r2tp2h(tTest, p);
			if(h >= hTest)	return	2;
			tTest	=	r4p2t(p);		//p相当の飽和温度は?
			d1		=	r3t2d_g(tTest);	//領域3 飽和蒸気の密度
			hTest	=	r3dt2h(d1,tTest);	//飽和蒸気 h
			if(h > hTest)	return 32;	//領域3 蒸気
			d2		=	r3t2d_l(tTest);	//領域3 飽和水の密度
			hTest	=	r3dt2h(d2,tTest);	//飽和水 h
			if(h >= hTest)	return 33;	//領域3 350℃以上の湿り蒸気
			hTest	=	r1tp2h(IF97_R3MIN_TEMP,	p);	//領域1と3の境界
			if(h > hTest)	return 31;	//領域3 水
			hTest	=	r1tp2h(IF97_MIN_TEMP,	p);	//領域1 温度下限
			if(h >= hTest)	return 1;	//領域1 
			else		return	(int)STMOHT_ERROR;	
		}
		else	{	//臨界圧以上
			hTest	=	r2tp2h(IF97_R5MIN_TEMP, p);	//領域2 温度上限
			if(h > hTest)	return	(int)STMOHT_ERROR;
			tTest	=	b23p2t(p);	//領域2と3の境界線上
			hTest	=	r2tp2h(tTest, p);
			if(h >= hTest)	return	2;
			hTest	=	r1tp2h(IF97_R3MIN_TEMP,	p);	//領域1と3の境界
			if(h > hTest)	return 30;	//領域3 臨界圧以上
			hTest	=	r1tp2h(IF97_MIN_TEMP,	p);	//領域1 温度下限
			if(h >= hTest)	return 1;	//領域1 
			else		return	(int)STMOHT_ERROR;	
		}
	}
	else	{	//領域外
		return	(int)STMOHT_ERROR;	
	}
}

//領域判定(Normal全領域)  圧力、エントロピー から
//この関数は、JSME 蒸気表CD-ROM内のコードを参考にして作成	2000/01/24 関数追加
int	WhatIsRegPS(double	p,	double	s)	{	//入力	p:圧力[MPa] s:エントロピー[kJ/kgK]
	//返り値 領域の数字(以下)
	//	領域1				:	1
	//	領域2				:	2 
	//	350℃以下の湿り蒸気	:	12
	//	領域5				:	5
	//	領域3(臨界圧以上)	:	30
	//	領域3(液体)			:	31
	//	領域3(蒸気)			:	32
	//	350℃以上の湿り蒸気	:	33
	//	領域外				:(int)STMOHT_ERROR;	
	double	sTest;		//Testのための試験エンタルピー
	double	tTest;		//温度[K]
	double	p1;			//圧力[M Pa]
	double	d1, d2;		//密度[Kg/m3]

	if	(	p <= IF97_MIN_PRESS	)	return	(int)STMOHT_ERROR;	
	if(	p < IF97_R5MAX_PRESS)		{	//領域5の最高圧力以下
		//領域5から外れているか？(領域5のsが一番大きい)
		sTest	=	r5tp2s(IF97_MAX_TEMP, p);
		if(s > sTest)	{	return	(int)STMOHT_ERROR;	}
		else	{
			sTest	=	r2tp2s(IF97_R5MIN_TEMP, p);
			if(s >= sTest)	{	return	5;	}	//r2のsより大きいならr5
			else	{		//10MPa以下だから臨界圧以下
				tTest	=	r4p2t(p);	//p相当の飽和温度は?
				sTest	=	r2tp2s(tTest, p);	//飽和蒸気(乾き)のs?
				if(s >= sTest)	return	2;	//領域2
				sTest	=	r1tp2s(tTest, p);
				if(s > sTest)	return	12;	//領域12(湿り蒸気)
				sTest	=	r1tp2s(IF97_MIN_TEMP, p);
				if(s >= sTest)	return	1;
				else	return	(int)STMOHT_ERROR;	
			}
		}
	}
	else	if(	p <= IF97_MAX_PRESS)	{
		p1	=	r4t2p(IF97_R3MIN_TEMP);	//領域3最低温度での飽和蒸気圧
		if(p <= p1 )	{	//IF97_R5MAX_PRESS <= p <  p1
			//温度350℃の飽和圧力以下の領域
			sTest	=	r2tp2s(IF97_R5MIN_TEMP, p);
			if(s > sTest)	return	(int)STMOHT_ERROR;
			tTest	=	r4p2t(p);	//p相当の飽和温度は?
			sTest	=	r2tp2s(tTest, p);	//飽和蒸気(乾き)のs?
			if(s >= sTest)	return	2;
			sTest	=	r1tp2s(tTest, p);
			if(s > sTest)	return	12;	//領域12(湿り蒸気)
			sTest	=	r1tp2s(IF97_MIN_TEMP, p);
			if(s >= sTest)	return	1;
			else	return	(int)STMOHT_ERROR;	
		}
		else	if(p < IF97_Pc)	{	//臨界圧未満
			sTest	=	r2tp2s(IF97_R5MIN_TEMP, p);
			if(s > sTest)	return	(int)STMOHT_ERROR;
			tTest	=	b23p2t(p);	//領域2と3の境界線上
			sTest	=	r2tp2s(tTest, p);
			if(s >= sTest)	return	2;
			tTest	=	r4p2t(p);		//p相当の飽和温度は?
			d1		=	r3t2d_g(tTest);	//領域3 飽和蒸気の密度
			sTest	=	r3dt2s(d1,tTest);	//飽和蒸気 s
			if(s > sTest)	return 32;	//領域3 蒸気
			d2		=	r3t2d_l(tTest);	//領域3 飽和水の密度
			sTest	=	r3dt2s(d2,tTest);	//飽和水 s
			if(s >= sTest)	return 33;	//領域3 350℃以上の湿り蒸気
			sTest	=	r1tp2s(IF97_R3MIN_TEMP,	p);	//領域1と3の境界
			if(s > sTest)	return 31;	//領域3 水
			sTest	=	r1tp2s(IF97_MIN_TEMP,	p);	//領域1 温度下限
			if(s >= sTest)	return 1;	//領域1 
			else		return	(int)STMOHT_ERROR;	
		}
		else	{	//臨界圧以上
			sTest	=	r2tp2s(IF97_R5MIN_TEMP, p);	//領域2 温度上限
			if(s > sTest)	return	(int)STMOHT_ERROR;
			tTest	=	b23p2t(p);	//領域2と3の境界線上
			sTest	=	r2tp2s(tTest, p);
			if(s >= sTest)	return	2;
			sTest	=	r1tp2s(IF97_R3MIN_TEMP,	p);	//領域1と3の境界
			if(s > sTest)	return 30;	//領域3 臨界圧以上
			sTest	=	r1tp2s(IF97_MIN_TEMP,	p);	//領域1 温度下限
			if(s >= sTest)	return 1;	//領域1 
			else		return	(int)STMOHT_ERROR;	
		}
	}
	else	{	//領域外
		return	(int)STMOHT_ERROR;	
	}
}


//	*******		まだ、テスト中	*******************
//領域3	温度，エントロピー->密度	密度，温度->エントロピーの逆
//注意!	
//	IF97_R3MIN_TEMP < t < IF97_Tc,
//		b23t2p(IF97_R3MIN_TEMP) < p < IF97_Pc	の範囲では、
//	t,pの条件を満たす解(d:密度)は、3つある。	かもしれない。
//	この関数は見つけたd:密度をすべて配列の形で返す。
//		(最後は、負値を終端子として書き込む)
//	但し、Delta_Dの範囲内に隣接して解が複数あった場合は検出できない。
//r3dt2s(double d,	double t)を使用して２分法密度を算出
//	返値:s[kJ/kgK](最後に見つけた)
//double	r3ts2d_3(double	t,	double	s,	double*	ans,	int	k)	{	//	t[K],	s[kJ/kgK]
		//	ansは、double ans[4以上]のansを渡すこと
		//	k は、ansに確保している(double)の個数(=必要とする答えの個数 + 1)



