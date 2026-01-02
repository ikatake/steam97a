/******************************************************/
/****	Stm97.DLL	用 header file					**/
/*	実用国際状態式（1997年）						**/
/*****************************************************/

/*
   This header file is for Steam97.c
*/

#include	<stdlib.h>
#include	<math.h>
#include	<windows.h>

#ifndef	_STEAM97_DEFINED

#undef	Dll_Dec	//reMove

//	DLLのビルド時には，Dll_Decは export 宣言
#ifdef	_MAKING_STEAM97
#define	Dll_Dec  __declspec( dllexport )
#else
#define	Dll_Dec  __declspec( dllimport )
#endif	//	_MAKING

/*温度単位*/
enum	TEMP_UNIT	{
	U_KELVIN	=	1,	/*ケルビン°*/
	U_CELSIUS,			/*摂氏		*/
	U_FAHRENHEIT		/*華氏		*/
	};

/*圧力単位*/
enum	PRESS_UNIT	{
	U_PASCAL	=	11,	/*N/m2(Pa) 	パスカル*/
	U_MPASCAL,			/*10^6 N/m2(M Pa)  	パスカル*/
	U_KGCM2A,			/*Kg/cm2a	*/
	U_KGCM2G,			/*Kg/cm2g	ゲージ圧*/
	U_MMHGA,			/*mmHg a	絶対圧*/
	U_MMHGG,			/*mmHg g	ゲージ圧*/
	U_ATMA,				/*atm	a	*/
	U_ATMG,				/*atm	g	ゲージ圧*/
	U_HPASCAL,			/*1mmbar(ミリバール)=1[hPa]=100[Pa]*/
	/*	以下 '99/11/13 appended*/
	U_PASCALG,			/*N/m2(Pa) 	パスカル のゲージ圧*/
	U_MPASCALG			/*10^6 N/m2(M Pa) ゲージ圧*/
	};

/*エンタルピ単位*/
enum	ENTHALPY_UNIT	{
	U_JKG	=	31,		/*J/Kg		エンタルピ単位*/
	U_KJKG,				/*kJ/Kg		エンタルピ単位*/
	U_KCALKG			/*Kcal/Kg	エンタルピ単位*/
	};

/*エントロピ単位*/
enum	ENTROPY_UNIT	{
	U_JKGK	=	51,		/*	J/KgK		エントロピ単位*/
	U_KJKGK,			/*	kJ/KgK		エントロピ単位*/
	U_KCALKGK			/*	Kcal/KgK	エントロピ単位*/
	};

/*密度単位*/
enum	DENSITY_UNIT		{
	U_M3KG	=	71,		/*m3/Kg		比容積単位*/
	U_KGM3				/*Kg/m3		密度（比重）単位*/
	};

/*乾き度or湿り度*/
enum	WET_DRY_UNIT		{
	U_WET	=	91,		/*湿り度*/
	U_DRY				/*乾き度*/
	};

/*比熱単位	CpもCvも*/
enum	HC_UNIT		{
	U_HCJKGK	=	101,	/*	J/KgK		比熱単位*/
	U_HCKJKGK,				/*	kJ/KgK		比熱単位*/
	U_HCKCALKGK				/*	Kcal/KgK	比熱単位*/
	};

//Function declare
//プロトタイプ
#ifdef	__cplusplus
extern "C"	{
#endif	//__cplusplus
Dll_Dec	int		WINAPI	STM_GetDllVer(int	n);

Dll_Dec	BOOL	WINAPI	STM_SetTempUnit(enum TEMP_UNIT	sw);
Dll_Dec	BOOL	WINAPI	STM_SetPressUnit(enum PRESS_UNIT	sw);
Dll_Dec	BOOL	WINAPI	STM_SetEnthalUnit(enum ENTHALPY_UNIT	sw);
Dll_Dec	BOOL	WINAPI	STM_SetEntroUnit(enum ENTROPY_UNIT	sw);
Dll_Dec	BOOL	WINAPI	STM_SetDensityUnit(enum	DENSITY_UNIT	sw	);
Dll_Dec	BOOL	WINAPI	STM_SetWetDryUnit(enum	WET_DRY_UNIT	sw);
Dll_Dec	BOOL	WINAPI	STM_SetHCUnit(enum	HC_UNIT	sw);

Dll_Dec	double	WINAPI	STM_GetMaxTemp(void);
Dll_Dec	double	WINAPI	STM_GetMinTemp(void);
Dll_Dec	double	WINAPI	STM_GetMaxPress(void);
Dll_Dec	double	WINAPI	STM_GetMinPress(void);

Dll_Dec	double	WINAPI	STM_sPt(double	temp);
Dll_Dec	double	WINAPI	STM_sTp(double	press);
Dll_Dec	double	WINAPI	STM_sRt(double	temp);
Dll_Dec	double	WINAPI	STM_sHtx(double	temp,	double x0);
Dll_Dec	double	WINAPI	STM_sDtx(double	temp,	double x0);
Dll_Dec	double	WINAPI	STM_sHps(double	press,	double entro);
Dll_Dec	double	WINAPI	STM_sXps(double	press,	double entro);
Dll_Dec	double	WINAPI	STM_sStx(double	temp,	double x0);
Dll_Dec	double	WINAPI	STM_sXph(double	press,	double enthal);

Dll_Dec	double	WINAPI	STM_nDtp(double	temp,	double	press);
Dll_Dec	double	WINAPI	STM_nHtp(double	temp,	double	press);
Dll_Dec	double	WINAPI	STM_nStp(double	temp,	double	press);
Dll_Dec	double	WINAPI	STM_nCPtp(double	temp,	double	press);
Dll_Dec	double	WINAPI	STM_nCVtp(double	temp,	double	press);
Dll_Dec	double	WINAPI	STM_nTps(double	press,	double entro);
Dll_Dec	double	WINAPI	STM_nTph(double	press,	double enthal);
//	Sep./2000 append
Dll_Dec	BOOL	WINAPI	STM_IsSat_ps(double press,	double entro);
Dll_Dec	BOOL	WINAPI	STM_IsSat_ph(double press,	double enthal);
// Nov./2000 append
Dll_Dec	double	WINAPI	STM_nWtp(double	temp,	double	press);
Dll_Dec	double	WINAPI	STM_nKtp(double	temp,	double	press);

#ifdef	__cplusplus
}
#endif	//__cplusplus



#ifndef	STMOHT_ERROR
#define	STMOHT_ERROR	(-999999.999)
#endif	//STMOHT_ERROR

#define _STEAM97_DEFINED
#endif	//_STEAM97_DEFINED
//  end

