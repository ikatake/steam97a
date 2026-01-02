/******************************************************/
/****	Steam97.DLL									**/
/*	���p���ۏ�Ԏ��i1997�N�j						**/
/*	Presented by Takayoshi Tabuchi.					**/
/*****************************************************/

/*	**Notation************************
'99 03/03		Ver 0.1 �쐬�J�n
'99 03/27		Ver 1.0 release
'99 11/13		Ver 1.1	���͒P��Pascal�ɃQ�[�W����ǉ�
'00 09/11		Ver 1.5	���͒P��Kg/cm2(g)�̃o�O�C��
'00 09/19		Ver 2.0	�ESTM_nTps()��STM_nTph()�ɂ����ė̈�R��
						�T�|�[�g�����B�܂�A�S�̈�Ή��ƂȂ����B
						�E�C�t������ɂ���̂�(�O�a���C�����
						����̂�)�𔻒肷��֐���ǉ������B
						STM_IsSat_ps(), STM_IsSat_ph()

****									

1)�ϊ��p�ȊO�̃v���O�������ł�,���ʂɋL�q���Ȃ����́C
  ���x[K], ����[MPa], ���x[Kg/m3],�G���g���s�[[kJ/KgK],
  ��e��[m3/Kg],�G���^���s�[[kJ/Kg] �̒P�ʂł���B

2)���̕ϐ����́C���ꂼ��ȉ��̕���\���Ƃ���D
	t	-	���x[k]	---1990�N���ۉ��x�ڐ���
	p	-	����[MPa]	��Έ�
	v	-	��e��{m3/Kg}
	d	-	���x[Kg/m3]
	h	-	��G���^���s�[[kJ/Kg]
	s	-	��G���g���s�[[kJ/KgK]
	x	-	����x[--](!=�����x)
	w	-	����[m/s]

3) ��L�ȊO�̕ϐ���,���ꂼ��P�ʐݒ�֐��Őݒ肳�ꂽ
  �P�ʂŕ\�킳��Ă�����̂Ƃ���B
  ��--temp [��], Press[mmHg]

*/

#define	_MAKING_STEAM97	//DLL�̃r���h���ɂ́CDll_Dec�� export �錾
#define THIS_DLL_NAME		"Steam97a.dll"

#ifdef _DEBUG
#include	<stdio.h>
#endif

#include	<malloc.h>
#include	<memory.h>
#include	<tchar.h>
#include	<windows.h>
#include	"if97.h"
#include	"if97back.h"
#include	"steam97.h"

/*	prototype declare	����file���ł����g�p���Ȃ��B	*/
//double	d_max(double	*v);	Vre.2 �ō폜
//double	d_min(double	*v);
double	Custom2Kelvine(double	c);
double	Kelvine2Custom(double	t);
double	Custom2MPa(double	c);
double	MPa2Custom(double	p);
double	Custom2KJkgK(double	c);
double	KJkgK2Custom(double	s);
double	wetdry2wet(double	c);
double	wet2wetdry(double	x);
double	HC_Custom2KJkgK(double	c);
double	HC_KJkgK2Custom(double	Cp);
double	Custom2Kgm3(double	c);
double	Kgm32Custom(double	d);

//double	s_r1ps2t(double	p,	double s);	Sep./2000 delleted
//double	s_r2ps2t(double	p,	double s);
//double	s_r1ph2t(double	p,	double h);
//double	s_r2ph2t(double	p,	double h);


/*�P�ʊ��Z�p*/
#define	C_KCAL2J	(4186.8)	/*1[Kcal]=C_KCALTOJ[J]	I.T(���C�\)�J�����[*/
#define	C_NEWTON	(9.80665)	/*1[Kgf]=C_NEWTON[N]			****/
#define	C_ATM2PAS	(101325.0)	/*1[atm]=C_ATMTOPAS[Pa]	'54 ��10��CGPM	*/
#define	C_ATM2MMHG	(760.0)		/*1[atm]=C_ATMTOMMHG[mmHg]	****/
#define	C_CEL2FAH	(1.8)		/*1[��]=C_CELTOFAH[��F]		****/
#define	C_HOUR2SEC	(3600.0)	/*1[Hour]=C_HOURTOSEC[sec]	****/
#define	C_CEL2KEL	(273.15)	/*��<->�j�K�ϊ��p	C_CEL2KEL[K] = 0[��]*/

//�P�ʌn�����Z�b�g
//static	enum	TEMP_UNIT		gSTM_TempU	=	U_CELSIUS;	//���x�P��
static	enum	TEMP_UNIT		gSTM_TempU	=	U_KELVIN;	//���x�P��
static	enum	PRESS_UNIT		gSTM_PressU	=	U_MPASCAL;	//���͒P��
static	enum	ENTHALPY_UNIT	gSTM_H_U	=	U_KJKG;		//	H
static	enum	ENTROPY_UNIT	gSTM_S_U	=	U_KJKGK;		//	S
static	enum	DENSITY_UNIT	gSTM_D_U		=	U_M3KG;		//���x�P��
static	enum	WET_DRY_UNIT	gSTM_WetDryU	=	U_WET;		//����xor�����x
static	enum	HC_UNIT			gSTM_HeatCapU	=	U_HCKJKGK;	//��M�P��

//�E����Z�o�֐��̖����K��
//�@�@���C������v�Z����֐�(���[�e�B���e�B�֐���P�ʐݒ�֐��Ȃǂ�����)�́C
//�@�ȉ��̖����K���ɂ��D
//�@
//�@�@�@�@�@�@�@�@�@�@�Z�o�ړI��\�� T:���x, P:����, H:�G���^���s�[,
//�@�@�@�@�@�@�@�@�@�@�b�@�@�@�@ S:�G���g���s�[, D:���x, X:����x, R:�����M
//�@�@�@�@�@�@�@�@�@�@�b         CP:�舳��M, CV:��ϔ�M  W:����
//�@�@�@�@�@�@�@�@�@�@�b
//�@�@�@�@�@�@�@�@�@�@�b�@��Q������\���D�����̈Ӗ��͑�P�����Ɠ����D
//�@�@�@�@�@�@�@�@�@�@���@��
//�@�@��F�@�r�s�l�Q���r����(��, ��)
//�@�@�@�@�@�@�@�@�@���@���@
//�@�@�@�@�@�@�@�@�@�b�@��P������\���Dt:���x, p:����, h:�G���^���s�[�@
//�@�@�@�@�@�@�@�@�@�b�@�@�@�@�@�@�@�@�@s:�G���g���s�[, d:���x, x:����x
//�@�@�@�@�@�@�@�@�@�b
//�@�@�@�@�@�@�@�@�@�ΏۂƂ���v�Z��\���Dn:������̈�̔C�ӓ_, 
//�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@s:�O�a���C�Ɍ���
//�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@2:�̈�2(*1)�̉ߔM���C�̂�

/////////////////////////////////////////////////////////////////////////////
// Initialization of DLL
int APIENTRY	DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	switch(	dwReason	)	{
		case	DLL_PROCESS_ATTACH:	//When DLL is Loaded, Initialize
			break;
		case	DLL_THREAD_ATTACH:
			break;
		case	DLL_THREAD_DETACH:
			break;
		case	DLL_PROCESS_DETACH://When DLL is released
			break;
		default:
			return	FALSE;
		}
	return TRUE;   // ok
}

//���̂c�k�k�̃o�[�W������Ԃ�
//STM_GetDllVer(void)
#define VERSION_INFO_KEY_ROOT    TEXT("\\StringFileInfo\\")

int	WINAPI	STM_GetDllVer(int	n)	{
	HANDLE	hInst;
	DWORD   dwVerInfoSize;     // Size of version information block
	DWORD   dwVerHnd=0;        // An 'ignored' parameter, always '0'
	TCHAR	szFullPath[256];

    static TCHAR szNull[1] = TEXT("");
    LPVOID  lpInfo;
    UINT	cch;
    TCHAR   key[80];
    TCHAR   lpBuffer[10];

	LPVOID  szBuff;
    HANDLE  hMem;
	int	i,	m;
	TCHAR	seps[]   = " ,\t\n";
	TCHAR*	pToken;

	hInst	=	GetModuleHandle(THIS_DLL_NAME);
    GetModuleFileName(hInst, szFullPath, sizeof(szFullPath));

    // Now lets dive in and pull out the version information:
    dwVerInfoSize = GetFileVersionInfoSize(szFullPath, &dwVerHnd);
    if (dwVerInfoSize == (DWORD)NULL)	return	(int)STMOHT_ERROR;

    hMem = GlobalAlloc(GMEM_MOVEABLE, dwVerInfoSize);
    szBuff  = GlobalLock(hMem);
    GetFileVersionInfo(szFullPath, 0, dwVerInfoSize, szBuff );
    wsprintf(lpBuffer, "%04X", GetUserDefaultLangID());
	strcat(lpBuffer,TEXT("04B0"));
	lstrcpy(key, VERSION_INFO_KEY_ROOT);
	lstrcat(key, lpBuffer);
	lstrcat(key, "\\");
	lstrcat(key, TEXT("ProductVersion"));
    // If version info exists, and the key query is successful, add
    //  the value.  Otherwise, the value for the key is NULL.
    //
    if( !(dwVerInfoSize && VerQueryValue(szBuff, key, &lpInfo, &cch))	)	{
		return	(int)STMOHT_ERROR;
	}

	if(n < 1	|| 4 < n)	m = 1;
	else			m = n;
	pToken = _tcstok( (TCHAR*)lpInfo, seps );	/* �ŏ��̃g�[�N�����擾���܂��B */   
 	for	(i=1;	i<=m && (pToken != NULL);	i++)	{
		if(m == i)	return	_ttoi(pToken);
		pToken = _tcstok( NULL, seps );
	}
	return	(int)STMOHT_ERROR;
}

//*****�P�ʂ��Z�b�g******************
BOOL	WINAPI	STM_SetTempUnit(enum TEMP_UNIT	sw)
{
	switch(sw)	{
		case	U_KELVIN:
		case	U_CELSIUS:
		case	U_FAHRENHEIT:
			gSTM_TempU	=	sw;	//���x�P��
			return	TRUE;
		default:	return	FALSE;
	}
}

BOOL	WINAPI	STM_SetPressUnit(enum PRESS_UNIT	sw)
{
	switch(sw)	{
		case U_PASCAL:
		case U_MPASCAL:
		case U_PASCALG:		//	'99/11/13 appended
		case U_MPASCALG:	//	'99/11/13 appended
		case U_KGCM2A:
		case U_KGCM2G:
		case U_MMHGA:
		case U_MMHGG:
		case U_ATMA:
		case U_ATMG:
		case U_HPASCAL:	gSTM_PressU	=	sw;	//���͒P��
					return	TRUE;
		default:	return	FALSE;
	}
}

BOOL	WINAPI	STM_SetEnthalUnit(enum ENTHALPY_UNIT	sw)
{
	switch(sw)	{
	case	U_JKG:
	case	U_KJKG:
	case	U_KCALKG:
		gSTM_H_U	=	sw;
		return	TRUE;
	default:
		return	FALSE;
	}
}

BOOL	WINAPI	STM_SetEntroUnit(enum ENTROPY_UNIT	sw)
{
	switch(sw)	{
		case	U_JKGK:
		case	U_KJKGK:
		case	U_KCALKGK:
					gSTM_S_U	=	sw;
					return	TRUE;
		default:	return	FALSE;
	}
}

BOOL	WINAPI	STM_SetDensityUnit(enum	DENSITY_UNIT	sw	)
{
	switch(sw)	{
	case	U_M3KG:
	case	U_KGM3:
				gSTM_D_U	=	sw;
				return	TRUE;
	default:	return	FALSE;
	}
}

BOOL	WINAPI	STM_SetWetDryUnit(enum	WET_DRY_UNIT	sw)
{
	switch(sw)	{
		case	U_WET:	/*����x*/
		case	U_DRY:	/*�����x*/
					gSTM_WetDryU	=	sw;	//����xor�����x
					return	TRUE;
		default:		return	FALSE;
	}
}

BOOL	WINAPI	STM_SetHCUnit(enum	HC_UNIT	sw)
{
	switch(sw)	{
		case	U_HCJKGK:
		case	U_HCKJKGK:
		case	U_HCKCALKGK:
			gSTM_HeatCapU	=	sw;
			return	TRUE;
	default:	return	FALSE;
	}
}

//	*********�P�ʕϊ�*******
//���x
double	Custom2Kelvine(double	c){
	switch(gSTM_TempU)	{
		case	U_KELVIN:		return	c;
		case	U_CELSIUS:		return	c + C_CEL2KEL;	/*[��]->[K]	*/
		case	U_FAHRENHEIT:	return	(c+459.67)/C_CEL2FAH;	/*[��F]->[K]	*/
		default:	return	STMOHT_ERROR;
	}
}

double	Kelvine2Custom(double	t){
	switch(gSTM_TempU)	{
		case	U_KELVIN:		return	t;
		case	U_CELSIUS:		return	t - C_CEL2KEL;	/*[K]->[��]*/
		case	U_FAHRENHEIT:	return	t * C_CEL2FAH - 459.67;	/*[K]->[��F]*/
		default:	return	STMOHT_ERROR;
	}
}

//����
double	Custom2MPa(double	c)	{
	switch(gSTM_PressU)	{
		case U_PASCAL:	return	c * 0.000001;		/*N/m2	->MPa*/
		case U_PASCALG:	return	(c + C_ATM2PAS) * 0.000001;	/*	'99/11/13 appended*/
		case U_HPASCAL:	return	c * 0.0001;			/*mmbar	->MPa*/
		case U_MPASCAL:	return	c;					/*M Pa	->MPa*/
		case U_MPASCALG:	return	c + (C_ATM2PAS * 0.000001);	//	'99/11/13 appended
		case U_KGCM2A:	return	c * C_NEWTON * 0.01;	/*Kg/cm2a->MPa*/
		case U_KGCM2G:									/*Kg/cm2g->MPa*/
						return	c * C_NEWTON * 0.01 + C_ATM2PAS*0.000001;//'00/09 updated
		case U_MMHGA:								/*mmHga	->MPa*/
						return	c * C_ATM2PAS / (1000000.0 * C_ATM2MMHG);
		case U_MMHGG:										/*mmHgg	->MPa*/
						return	(c+C_ATM2MMHG) * C_ATM2PAS / (1000000.0 * C_ATM2MMHG);
		case U_ATMA:	return	c * C_ATM2PAS * 0.000001;	/*atma->MPa*/
		case U_ATMG:	return	(c+1.0) * C_ATM2PAS * 0.000001;	/*atmg->MPa*/
		default:		return	STMOHT_ERROR;
	}
}

double	MPa2Custom(double	p)	{
	switch(gSTM_PressU)	{
		case U_PASCAL:	return	p * 1000000.0;		/*MPa -> Pa(N/m2)*/
		case U_PASCALG:	return	p * 1000000.0 - C_ATM2PAS;	//	'99/11/13 appended
		case U_HPASCAL:	return	p * 10000.0;		/*MPa -> mmbar*/
		case U_MPASCAL:	return	p;					/*M Pa	->MPa*/
		case U_MPASCALG:	return	p - (C_ATM2PAS * 0.000001);	//	'99/11/13 appended
		case U_KGCM2A:	return	p * 100.0 / C_NEWTON;	/*MPa -> Kg/cm2a*/
		case U_KGCM2G:									/*MPa -> Kg/cm2g*/ //'00/09 updated
						return	(p - C_ATM2PAS*0.000001) * 100.0 / C_NEWTON ;
		case U_MMHGA:								/*MPa -> mmHga*/
						return	p * 1000000.0 * C_ATM2MMHG / C_ATM2PAS;
		case U_MMHGG:									/*MPa -> mmHgg*/
						return	p * 1000000.0 * C_ATM2MMHG / C_ATM2PAS - C_ATM2MMHG;
		case U_ATMA:	return	p * 1000000.0 / C_ATM2PAS;	/*MPa -> atma*/
		case U_ATMG:	return	p * 1000000.0 / C_ATM2PAS - 1.0;	/*MPa -> atmg*/
		default:		return	STMOHT_ERROR;
	}
}

//Enthalpy
double	Custom2KJkg(double	e)	{
	switch(gSTM_H_U)	{
		case	U_KCALKG:					/*[Kcal/kg] -> [KJ/Kg]*/
					return	e * C_KCAL2J * 0.001;
		case	U_JKG:	return	e * 0.001;	/*	[J/Kg] -> [kJ/Kg]*/
		case	U_KJKG:	return	e ;			/*	[kJ/Kg] -> [kJ/Kg]*/
		default:		return	STMOHT_ERROR;
	}
}

double	KJkg2Custom(double	h)	{
	switch(gSTM_H_U)	{
		case	U_KCALKG:					/*[KJ/Kg] -> [Kcal/kg]*/
					return	h * 1000.0 / C_KCAL2J;
		case	U_JKG:	return	h * 1000.0;	/*	[kJ/Kg] -> [J/Kg]*/
		case	U_KJKG:	return	h ;			/*	[kJ/Kg] -> [kJ/Kg]*/
		default:		return	STMOHT_ERROR;
	}
}

//Entropy
double	Custom2KJkgK(double	c)	{
	switch(gSTM_S_U)	{
		case	U_KCALKGK:						/*[Kcal/kgK] -> [KJ/KgK]*/
							return	c * C_KCAL2J * 0.001;
		case	U_JKGK:		return	c * 0.001;	/*	[J/KgK] -> [kJ/KgK]*/
		case	U_KJKGK:	return	c ;			/*	[kJ/KgK] -> [kJ/KgK]*/
		default:		return	STMOHT_ERROR;
	}
}

double	KJkgK2Custom(double	s)	{
	switch(gSTM_S_U)	{
		case	U_KCALKGK:						/* [KJ/KgK] -> [Kcal/kgK] */
							return	s * 1000.0 / C_KCAL2J;
		case	U_JKGK:		return	s * 1000.0;	/*	[kJ/KgK] -> [J/KgK]*/
		case	U_KJKGK:	return	s ;			/*	[kJ/KgK] -> [kJ/KgK]*/
		default:		return	STMOHT_ERROR;
	}
}

//����xor�����x�� ����x�ɕϊ�����.
double	wetdry2wet(double	c)	{
	switch(gSTM_WetDryU)	{
		case	U_WET:	//����x
				return	c;
		case	U_DRY:	//�����x
				return	1.0 - c;
		default:		return	STMOHT_ERROR;
	}
}

//����xor�����x�� ����x�ɕϊ�����.
double	wet2wetdry(double	x)	{
	switch(gSTM_WetDryU)	{
		case	U_WET:	//����x
				return	x;
		case	U_DRY:	//�����x
				return	1.0 - x;
		default:		return	STMOHT_ERROR;
	}
}

//��M
double	HC_Custom2KJkgK(double	c)	{
	switch(gSTM_HeatCapU)	{
		case	U_HCKCALKGK:						/*[Kcal/kgK] -> [KJ/KgK]*/
							return	c * C_KCAL2J * 0.001;
		case	U_HCJKGK:	return	c * 0.001;	/*	[J/KgK] -> [kJ/KgK]*/
		case	U_HCKJKGK:	return	c ;			/*	[kJ/KgK] -> [kJ/KgK]*/
		default:		return	STMOHT_ERROR;
	}
}

double	HC_KJkgK2Custom(double	Cp)	{
	switch(gSTM_HeatCapU)	{
		case	U_HCKCALKGK:						/* [KJ/KgK] -> [Kcal/kgK] */
							return	Cp * 1000.0 / C_KCAL2J;
		case	U_HCJKGK:	return	Cp * 1000.0;	/*	[kJ/KgK] -> [J/KgK]*/
		case	U_HCKJKGK:	return	Cp ;			/*	[kJ/KgK] -> [kJ/KgK]*/
		default:		return	STMOHT_ERROR;
	}
}

//���xor��e��
double	Custom2Kgm3(double	c)	{
	switch(gSTM_D_U)	{
		case	U_M3KG:	
			if(c == 0.0)	{	return	STMOHT_ERROR;	}
			return	1.0 / c;
		case	U_KGM3:
			return	c;
		default:		return	STMOHT_ERROR;
	}
}

double	Kgm32Custom(double	d)	{
	switch(gSTM_D_U)	{
		case	U_M3KG:	
			if(d == 0.0)	{	return	STMOHT_ERROR;	}
			return	1.0 / d;
		case	U_KGM3:
			return	d;
		default:		return	STMOHT_ERROR;
	}
}

//	***********	Uttility	**************************
//�����\�ō����x
double	WINAPI	STM_GetMaxTemp(void)	{
	return		Kelvine2Custom(IF97_MAX_TEMP);
}

//�����\�Œቷ�x
double	WINAPI	STM_GetMinTemp(void)	{
	return		Kelvine2Custom(IF97_MIN_TEMP);
}

//�����\�ō�����
double	WINAPI	STM_GetMaxPress(void)	{
	return		MPa2Custom(IF97_MAX_PRESS);
}

//�����\�Œሳ��
double	WINAPI	STM_GetMinPress(void)	{
	return		MPa2Custom(IF97_MIN_PRESS);
}
//	*************	�O�a���C�ł̐���v�Z	**********************
//�O�a����(���x������)
double	WINAPI	STM_sPt(double	temp)	{
	double	t,	p;
	t	=	Custom2Kelvine(temp);
	if(t<IF97_MIN_TEMP || IF97_Tc<t)	return	STMOHT_ERROR;
	p	=	r4t2p(t);
	if(p <= 0.0)	return	STMOHT_ERROR;
	else	return	MPa2Custom(	p);
}
//�O�a���x(���́����x)
double	WINAPI	STM_sTp(double	press)	{
	double	t,	p;
	p	=	Custom2MPa(	press);
	if(p < r4t2p(IF97_MIN_TEMP) || IF97_Pc<p)	return	STMOHT_ERROR;
	t	=	r4p2t(p);
	if(t < 0.0)	return	STMOHT_ERROR;
	else	return	Kelvine2Custom(t);
}

//�����M(���x�������M[�G���^���s�[�Ɠ����P�ʂŕԂ��B])
double	WINAPI	STM_sRt(double	temp)	{
	double	t,	p;
	double	h_w,	h_s,	r;
	t	=	Custom2Kelvine(temp);
	if(t < IF97_MIN_TEMP || IF97_Tc <= t)	return	STMOHT_ERROR;
	p	=	r4t2p(t);	//�O�a���C��
	if(p <= 0.0)	return	STMOHT_ERROR;
	
	if(t <= IF97_R3MIN_TEMP)	{
		h_w	=	r1tp2h(t,p);	//��
		h_s	=	r2tp2h(t,p);	//���C
		r	=	h_s - h_w;		//	h_s > h_w	�̂͂�
	}
	else	{	//�O�a���̏�������̈�3�Ŗ��x�̍���T��
		h_w	=	r3dt2h( r3t2d_l(t),	t);
		h_s	=	r3dt2h( r3t2d_g(t),	t);
		r	=	h_s - h_w;		//	h_s > h_w	�̂͂�
	}
	return	KJkg2Custom(r);
}

/*	Sep. /2000 dellete
double	d_max(double	v[])	{
	double	x = STMOHT_ERROR;
	int	i;
	for(i = 0;	i<32700;	i++){
		if(v[i] < 0.0)	break;
		if(	x	<	v[i])	x	=	v[i];
	}
	return	x;
}

double	d_min(double	v[])	{
	double	x = 999999e+6;
	int	i;
	for(i = 0;	i<32700;	i++){
		if(v[i] < 0.0)	break;
		if(	v[i] < x)	x	=	v[i];
	}
	return	x;
}
*/

//�O�a���C�̃G���^���s�[(�����x�Ǝ���x)
double	WINAPI	STM_sHtx(double	temp,	double x0)	{
	double	t,	p,	x;
	double	h_d,	h_w;	//�G���^���s�[	�������C�^�O�a��
	double	h;	//����

	t	=	Custom2Kelvine(temp);
	if(t < IF97_MIN_TEMP || IF97_Tc <= t)	return	STMOHT_ERROR;

	if	(x0 < 0.0 || 1.0 < x0)	return	STMOHT_ERROR;
	x	=	wetdry2wet(x0);		//����xor�����x�� ����x�ɕϊ�����.
	if(x < 0.0)	return	STMOHT_ERROR;
	
	p	=	r4t2p(t);

	if( IF97_R3MIN_TEMP < t )	{	//�̈�3
		h_w	=	r3dt2h( r3t2d_l(t),	t);
		h_d	=	r3dt2h( r3t2d_g(t),	t);
		h	=	h_w*x + h_d*(1.0 - x);
	}
	else	{
		h_w	=	r1tp2h( t,	p);
		h_d	=	r2tp2h( t,	p);
		h	=	h_w*x + h_d*(1.0 - x);
	}
	return	KJkg2Custom(h);
}

//�O�a���C�̖��x(�����x�Ǝ���x)
double	WINAPI	STM_sDtx(double	temp,	double x0)	{
	double	t,	p,	x;
	double	d_d,	d_w;	//���x	�������C�^�O�a��
	double	d;				//����

	t	=	Custom2Kelvine(temp);
	if(t < IF97_MIN_TEMP || IF97_Tc <= t)	return	STMOHT_ERROR;

	if	(x0 < 0.0 || 1.0 < x0)	return	STMOHT_ERROR;
	x	=	wetdry2wet(x0);		//����xor�����x�� ����x�ɕϊ�����.
	if(x < 0.0)	return	STMOHT_ERROR;
	
	p	=	r4t2p(t);

	if( IF97_R3MIN_TEMP < t )	{	//�̈�3
		d_w	=	r3t2d_l(t);
		d_d	=	r3t2d_g(t);
		d	=	d_w*x + d_d*(1.0 - x);
	}
	else	{
		d_w	=	1.0 / r1tp2v( t,	p);
		d_d	=	1.0 / r2tp2v( t,	p);
		d	=	d_w*x + d_d*(1.0 - x);
	}
	return	Kgm32Custom(	d);
}


//�O�a���C�̃G���^���s�[(�����͂ƃG���g���s�[)
double	WINAPI	STM_sHps(double	press,	double entro)	{
	double	t,	p,	x;
	double	s,	s_w,	s_s;	//entropy
	double	h,	h_w,	h_s;	//enthalpy

	p	=	Custom2MPa(	press);
	if(p < r4t2p(IF97_MIN_TEMP) || IF97_Pc<p)	return	STMOHT_ERROR;

	s	=	Custom2KJkgK(entro);
	if(s < 0.0)	return	STMOHT_ERROR;

	t	=	r4p2t(p);
	if(t < 0.0)	return	STMOHT_ERROR;

	if( t <= IF97_R3MIN_TEMP )	{	//�̈�1,2�̊�
		s_w	=	r1tp2s(t,	p);	//��
		s_s	=	r2tp2s(t,	p);	//���C
		if(s < s_w || s_s < s )	return	STMOHT_ERROR;
		h_w	=	r1tp2h(t,	p);	//��
		h_s	=	r2tp2h(t,	p);	//��
	}
	else	{		//�̈�3�̒�
		double	d_s,	d_w;	//���x	���C�^�O�a��
		d_w	=	r3t2d_l(t);
		d_s	=	r3t2d_g(t);
		s_w	=	r3dt2s(d_w,	t);
		s_s	=	r3dt2s(d_s,	t);
		if(s < s_w || s_s < s )	return	STMOHT_ERROR;
		h_w	=	r3dt2h(d_w,	t);
		h_s	=	r3dt2h(d_s,	t);
	}
	x	=	(s_s - s)/(s_s - s_w);	//����x
	h	=	h_w*x + h_s*(1.0 - x);
	return	KJkg2Custom(	h);
}

//�O�a���C�̎���x(�����͂ƃG���g���s�[)
double	WINAPI	STM_sXps(double	press,	double entro)	{
	double	t,	p,	x;
	double	s,	s_w,	s_s;	//entropy

	p	=	Custom2MPa(	press);
	if(p < r4t2p(IF97_MIN_TEMP) || IF97_Pc<p)	return	STMOHT_ERROR;

	s	=	Custom2KJkgK(entro);
	if(s < 0.0)	return	STMOHT_ERROR;

	t	=	r4p2t(p);
	if(t < 0.0)	return	STMOHT_ERROR;

	if( t <= IF97_R3MIN_TEMP )	{	//�̈�1,2�̊�
		s_w	=	r1tp2s(t,	p);	//��
		s_s	=	r2tp2s(t,	p);	//���C
		if(s < s_w || s_s < s )	return	STMOHT_ERROR;
	}
	else	{		//�̈�3�̒�
		double	d_s,	d_w;	//���x	���C�^�O�a��
		d_w	=	r3t2d_l(t);
		d_s	=	r3t2d_g(t);
		s_w	=	r3dt2s(d_w,	t);
		s_s	=	r3dt2s(d_s,	t);
		if(s < s_w || s_s < s )	return	STMOHT_ERROR;
	}
	x	=	(s_s - s)/(s_s - s_w);	//����x
	return	wet2wetdry(	x);
}

//�O�a���C�̃G���g���s�[(�����x�Ǝ���x)
double	WINAPI	STM_sStx(double	temp,	double x0)	{
	double	t,	p,	x;
	double	s_d,	s_w;	//�G���^���s�[	�������C�^�O�a��
	double	s;	//����

	t	=	Custom2Kelvine(temp);
	if(t < IF97_MIN_TEMP || IF97_Tc <= t)	return	STMOHT_ERROR;

	if	(x0 < 0.0 || 1.0 < x0)	return	STMOHT_ERROR;
	x	=	wetdry2wet(x0);		//����xor�����x�� ����x�ɕϊ�����.
	if(x < 0.0)	return	STMOHT_ERROR;
	
	p	=	r4t2p(t);

	if( IF97_R3MIN_TEMP < t )	{	//�̈�3
		s_w	=	r3dt2s( r3t2d_l(t),	t);
		s_d	=	r3dt2s( r3t2d_g(t),	t);
	}
	else	{
		s_w	=	r1tp2s( t,	p);
		s_d	=	r2tp2s( t,	p);
	}
	s	=	s_w*x + s_d*(1.0 - x);
	return	KJkgK2Custom(s);
}

//�O�a���C�̎���x(�����͂ƃG���^���s�[)
double	WINAPI	STM_sXph(double	press,	double enthal)	{
	double	t,	p,	x;
	double	h,	h_w,	h_s;	//enthalpy

	p	=	Custom2MPa(	press);
	if(p < r4t2p(IF97_MIN_TEMP) || IF97_Pc<p)	return	STMOHT_ERROR;

	h	=	Custom2KJkg(enthal);
	if(h < 0.0)	return	STMOHT_ERROR;

	t	=	r4p2t(p);
	if(t < 0.0)	return	STMOHT_ERROR;

	if( t <= IF97_R3MIN_TEMP )	{	//�̈�1,2�̊�
		h_w	=	r1tp2h(t,	p);	//��
		h_s	=	r2tp2h(t,	p);	//���C
		if( h < h_w || h_s < h )	return	STMOHT_ERROR;
	}
	else	{		//�̈�3�̒�
		double	d_s,	d_w;	//���x	���C�^�O�a��
		d_w	=	r3t2d_l(t);
		d_s	=	r3t2d_g(t);
		h_w	=	r3dt2h(d_w,	t);
		h_s	=	r3dt2h(d_s,	t);
		if( h < h_w || h_s < h )	return	STMOHT_ERROR;
	}
	x	=	(h_s - h)/(h_s - h_w);	//����x
	return	wet2wetdry(	x);
}


//	*************	�C�ӓ_�ł̐���v�Z	**********************
//�̈�3�̗ՊE�_���x�ȉ��̗̈�ł́A���x�ƈ��͂����Ԃ���ӂ�
//����ł��Ȃ����Ƃɒ��ӁB

//Density(	<-	t,p)
double	WINAPI	STM_nDtp(double	temp,	double	press)	{
	int	nRegion;
	double	d,	t,	p;
	t	=	Custom2Kelvine(temp);
	p	=	Custom2MPa(press);

	nRegion	=	WhatIsRegion(t,	p);
	if	(nRegion <= 0)	return	STMOHT_ERROR;
	
	switch(nRegion)	{
		case	1:	d = 1.0 / r1tp2v(	t,	p);
					break;
		case	4:	//�O�a���C�́A�̈�2�Ƃ��ď���
		case	2:	d = 1.0 / r2tp2v(	t,	p);
					break;
		case	3:	d = r3tp2d(	t,	p);
					break;
		case	5:	d = 1.0 / r5tp2v(	t,	p);
					break;
		default:	return	STMOHT_ERROR;
	}
	return	Kgm32Custom(d);
}

//Enthalpy(	<-	t,p)
double	WINAPI	STM_nHtp(double	temp,	double	press)	{
	int	nRegion;
	double	h,	t,	p,	d;
	t	=	Custom2Kelvine(temp);
	p	=	Custom2MPa(press);

	nRegion	=	WhatIsRegion(t,	p);
	if	(nRegion <= 0)	return	STMOHT_ERROR;
	
	switch(nRegion)	{
		case	1:	h = r1tp2h(	t,	p);
					break;
		case	4:	//�O�a���C�́A�̈�2�Ƃ��ď���
		case	2:	h = r2tp2h(	t,	p);
					break;
		case	3:	d = r3tp2d(	t,	p);
					h = r3dt2h(	d,	t);
					break;
		case	5:	h = r5tp2h(	t,	p);
					break;
		default:	return	STMOHT_ERROR;
	}
	return	KJkg2Custom(h);
}

//Entropy(	<-	t,p)
double	WINAPI	STM_nStp(double	temp,	double	press)	{
	int	nRegion;
	double	s,	t,	p,	d;
	t	=	Custom2Kelvine(temp);
	p	=	Custom2MPa(press);

	nRegion	=	WhatIsRegion(t,	p);
	if	(nRegion <= 0)	return	STMOHT_ERROR;

	switch(nRegion)	{
		case	1:	s = r1tp2s(	t,	p);
					break;
		case	4:	//�O�a���C�́A�̈�2�Ƃ��ď���
		case	2:	s = r2tp2s(	t,	p);
					break;
		case	3:	d = r3tp2d(	t,	p);
					s = r3dt2s(	d,	t);
					break;
		case	5:	s = r5tp2s(	t,	p);
					break;
		default:	return	STMOHT_ERROR;
	}
	return	KJkgK2Custom(s);
}

//Heat Capacity(	<-	t,p)	�舳��M
double	WINAPI	STM_nCPtp(double	temp,	double	press)	{
	int	nRegion;
	double	Cp,	t,	p,	d;
	t	=	Custom2Kelvine(temp);
	p	=	Custom2MPa(press);

	nRegion	=	WhatIsRegion(t,	p);
	if	(nRegion <= 0)	return	STMOHT_ERROR;

	switch(nRegion)	{
		case	1:	Cp = r1tp2Cp(	t,	p);
					break;
		case	4:	//�O�a���C����́A�̈�2�Ƃ��ď���
		case	2:	Cp = r2tp2Cp(	t,	p);
					break;
		case	3:	d = r3tp2d(	t,	p);
					Cp = r3dt2Cp(	d,	t);
					break;
		case	5:	Cp = r5tp2Cp(	t,	p);
					break;
		default:	return	STMOHT_ERROR;
	}
	return	HC_KJkgK2Custom(	Cp);
}

//Heat Capacity(	<-	t,p)	��ϔ�M
double	WINAPI	STM_nCVtp(double	temp,	double	press)	{
	int	nRegion;
	double	Cv,	t,	p,	d;
	t	=	Custom2Kelvine(temp);
	p	=	Custom2MPa(press);

	nRegion	=	WhatIsRegion(t,	p);
	if	(nRegion <= 0)	return	STMOHT_ERROR;

	switch(nRegion)	{
		case	1:	Cv = r1tp2Cv(	t,	p);
					break;
		case	4:	//�O�a���C����́A�̈�2�Ƃ��ď���
		case	2:	Cv = r2tp2Cv(	t,	p);
					break;
		case	3:	d = r3tp2d(	t,	p);
					Cv = r3dt2Cv(	d,	t);
					break;
		case	5:	Cv = r5tp2Cv(	t,	p);
					break;
		default:	return	STMOHT_ERROR;
	}
	return	HC_KJkgK2Custom(	Cv);
}

//���x�v�Z( <- ���́A�G���g���s�[)
double	WINAPI	STM_nTps(double	press,	double entro)	{
	double	t,	p;
	double	s;	//entropy
	int	nRegion;
	//	�̈�1				:	1
	//	�̈�2				:	2 
	//	350���ȉ��̎�����C	:	12
	//	�̈�5				:	5
	//	�̈�3(�ՊE���ȏ�)	:	30
	//	�̈�3(�t��)			:	31
	//	�̈�3(���C)			:	32
	//	350���ȏ�̎�����C	:	33
	//	�̈�O				:(int)STMOHT_ERROR;	
	int		r;	//r:1	->	2a
				//r:2	->	2b
				//r:3	->	2c

	p	=	Custom2MPa(	press);
	if(p <= IF97_MIN_PRESS || IF97_MAX_PRESS <= p)	return	STMOHT_ERROR;

	s	=	Custom2KJkgK(entro);
	if(s < 0.0)	return	STMOHT_ERROR;

	nRegion	=	WhatIsRegPS(p,	s);
	if(nRegion < 0)	return	STMOHT_ERROR;
	switch(nRegion)	{
		case	1:	t	=	r1ps2t(p,s);
					if(t > 0.0)	return	Kelvine2Custom(	t);
					else	break;
		case	2:	r	=	WhatIsSubR2ps(p,	s);
					switch(r)	{
					case	1:	t	=	r2aps2t( p,	s);
								break;
					case	2:	t	=	r2bps2t( p,	s);
								break;
					case	3:	t	=	r2cps2t( p,	s);
								break;
					default:	return	STMOHT_ERROR;
					}
					if(t > 0.0)	return	Kelvine2Custom(	t);
					else	break;
		case	5:	t	=	r5sp2t(	s,	p);	//in if97back.c
					if(t > 0.0)	return	Kelvine2Custom(	t);
					else	break;
		case	12:
		case	33:	t	=	r4p2t(p);	//p�����̖O�a���x��?
					if(t > 0.0)	return	Kelvine2Custom(	t);
					else	break;
		case	30:	
		case	31:	
		case	32:	t	=	r3ps2t(p,s);
					if(t > 0.0)	return	Kelvine2Custom(	t);
					else	break;
		default:	return	STMOHT_ERROR;
	}
	//�ǂ����� �v�Zerror
	return	STMOHT_ERROR;
}

//���x�v�Z( <- ���́A�G���^���s�[)
double	WINAPI	STM_nTph(double	press,	double enthal)	{
	double	t,	p;
	double	h;	//enthalpy
	int		nRegion;	//�̈�
	//	�̈�1				:	1
	//	�̈�2				:	2 
	//	350���ȉ��̎�����C	:	12
	//	�̈�5				:	5
	//	�̈�3(�ՊE���ȏ�)	:	30
	//	�̈�3(�t��)			:	31
	//	�̈�3(���C)			:	32
	//	350���ȏ�̎�����C	:	33
	//	�̈�O				:(int)STMOHT_ERROR;	
	int		r;	//r:1	->	2a
				//r:2	->	2b
				//r:3	->	2c

	p	=	Custom2MPa(	press);
	h	=	Custom2KJkg(enthal);

	nRegion	=	WhatIsRegPH(p,	h);
	if(nRegion < 0)	return	STMOHT_ERROR;
	switch(nRegion)	{
		case	1:	t	=	r1ph2t(p,h);
					if(t > 0.0)	return	Kelvine2Custom(	t);
					else	break;
		case	2:	r	=	WhatIsSubR2ph(p,	h);
					switch(r)	{
					case	1:	t	=	r2aph2t( p,	h);
								break;
					case	2:	t	=	r2bph2t( p,	h);
								break;
					case	3:	t	=	r2cph2t( p,	h);
								break;
					default:	return	STMOHT_ERROR;
					}
					if(t > 0.0)	return	Kelvine2Custom(	t);
					else	break;
		case	5:	t	=	r5hp2t(	h,	p);	//in if97back.c
					if(t > 0.0)	return	Kelvine2Custom(	t);
					else	break;
		case	12:
		case	33:	t	=	r4p2t(p);	//p�����̖O�a���x��?
					if(t > 0.0)	return	Kelvine2Custom(	t);
					else	break;
		case	30:	
		case	31:	
		case	32:	t	=	r3ph2t(p,	h);
					if(t > 0.0)	return	Kelvine2Custom(	t);
					else	break;
		default:	return	STMOHT_ERROR;
	}
	//�ǂ����� �v�Zerror
	return	STMOHT_ERROR;
}


//�C�t������ɂ��邩�ǂ����𔻒�
// �O�a���C����ł���΁ATRUE(1), ������΁AFALSE(0)
//�G���[��FALSE�Ƃ��Ĉ���
BOOL	WINAPI	STM_IsSat_ps(double press,	double entro)	{
	double	p;
	double	s;	//entropy
	int	nRegion;
	//	�̈�1				:	1
	//	�̈�2				:	2 
	//	350���ȉ��̎�����C	:	12
	//	�̈�5				:	5
	//	�̈�3(�ՊE���ȏ�)	:	30
	//	�̈�3(�t��)			:	31
	//	�̈�3(���C)			:	32
	//	350���ȏ�̎�����C	:	33
	//	�̈�O				:(int)STMOHT_ERROR;	

	p	=	Custom2MPa(	press);
	if(p <= IF97_MIN_PRESS || IF97_MAX_PRESS <= p)	return	FALSE;

	s	=	Custom2KJkgK(entro);
	if(s < 0.0)	return	FALSE;

	nRegion	=	WhatIsRegPS(p,	s);
	if(nRegion < 0)	return	FALSE;
	switch(nRegion)	{
		case	12:
		case	33:	return	TRUE;

		case	1:	
		case	2:
		case	5:
		case	30:	
		case	31:	
		case	32:	return	FALSE;
		default:	return	FALSE;
	}
	//�ǂ����� �v�Zerror
	return	FALSE;
}

//�C�t������ɂ��邩�ǂ����𔻒�
// �O�a���C����ł���΁ATRUE(1), ������΁AFALSE(0)
//�G���[��FALSE�Ƃ��Ĉ���
BOOL	WINAPI	STM_IsSat_ph(double press,	double enthal)	{
	double	p;
	double	h;	//enthalpy
	int	nRegion;
	//	�̈�1				:	1
	//	�̈�2				:	2 
	//	350���ȉ��̎�����C	:	12
	//	�̈�5				:	5
	//	�̈�3(�ՊE���ȏ�)	:	30
	//	�̈�3(�t��)			:	31
	//	�̈�3(���C)			:	32
	//	350���ȏ�̎�����C	:	33
	//	�̈�O				:(int)STMOHT_ERROR;	

	p	=	Custom2MPa(	press);
	if(p <= IF97_MIN_PRESS || IF97_MAX_PRESS <= p)	return	FALSE;

	h	=	Custom2KJkg(enthal);
	if(h < 0.0)	return	FALSE;

	nRegion	=	WhatIsRegPH(p,	h);
	if(nRegion < 0)	return	FALSE;
	switch(nRegion)	{
		case	12:
		case	33:	return	TRUE;

		case	1:	
		case	2:
		case	5:
		case	30:	
		case	31:	
		case	32:	return	FALSE;
		default:	return	FALSE;
	}
	//�ǂ����� �v�Zerror
	return	FALSE;
}



//Speed of Sound(	<-	t,p)	����[m/s]
double	WINAPI	STM_nWtp(double	temp,	double	press)	{
	int	nRegion;
	double	w,	t,	p,	d;
	t	=	Custom2Kelvine(temp);
	p	=	Custom2MPa(press);

	nRegion	=	WhatIsRegion(t,	p);
	if	(nRegion <= 0)	return	STMOHT_ERROR;

	switch(nRegion)	{
		case	1:	w = r1tp2w(	t,	p);
					break;
		case	4:	//�O�a���C����́A�̈�2�Ƃ��ď���
		case	2:	w = r2tp2w(	t,	p);
					break;
		case	3:	d = r3tp2d(	t,	p);
					w = r3dt2w(	d,	t);
					break;
		case	5:	w = r5tp2w(	t,	p);	//JSME�ł�,�̈�5�͔͈͊O
					break;
		default:	return	STMOHT_ERROR;
	}
	return	w;
}

//���G���g���s�[�w��( <- t, p)  (�܂��͒f�M�w��, (Cp/Cv) = �� ��M��Ɣ����ɈႤ)
//	�w���Ȃ̂ŒP�ʂ͂Ȃ�
Dll_Dec	double	WINAPI	STM_nKtp(double	temp,	double	press)	{
	int	nRegion;
	double	k,	t,	p,	d;
	t	=	Custom2Kelvine(temp);
	p	=	Custom2MPa(press);

	nRegion	=	WhatIsRegion(t,	p);
	if	(nRegion <= 0)	return	STMOHT_ERROR;

	switch(nRegion)	{
		case	1:	k = r1tp2k(	t,	p);
					break;
		case	4:	//�O�a���C����́A�̈�2�Ƃ��ď���
		case	2:	k = r2tp2k(	t,	p);
					break;
		case	3:	d = r3tp2d(	t,	p);
					k = r3dt2k(	d,	t);
					break;
		case	5:	k = r5tp2k(	t,	p);	//JSME�ł�,�̈�5�͔͈͊O
					break;
		default:	return	STMOHT_ERROR;
	}
	return	k;
}
