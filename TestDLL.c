//	*******************************************
//	Test for Steam97.DLL routine
//
//

#include	<stdio.h>
#include	"steam97.h"

int main()	{
	float	fpT,	fpP;
	double	t,	p,	d;
	double	h,s,Cp,Cv,w,k;
	double	t2,	t3,	t4;

	printf("Steam97.DLL Product Version %d, %d, %d, %d\n",
		STM_GetDllVer(1),STM_GetDllVer(2),STM_GetDllVer(3),STM_GetDllVer(4)	);

	STM_SetTempUnit(U_CELSIUS);
	STM_SetPressUnit(U_MPASCAL);
	STM_SetHCUnit(U_HCKJKGK);
	STM_SetDensityUnit(U_M3KG);
	STM_SetEnthalUnit(U_KJKG);
	STM_SetEntroUnit(U_KJKGK);
	
	while(1)	{
		d = 0.0;
		h = 0.0;
		s = 0.0;
		Cp = 0.0;

		printf("\nTemp.[Åé] :");
		scanf("%f",	&fpT);
		if(fpT <= 0.0)	return	1;
		t	=	(double)fpT;

		printf("\nPress.[MPa] :");
		scanf("%f",	&fpP);
		printf("\n");
		if(fpP <= 0.0)	return	2;
		p	=	(double)fpP;

		h	=	STM_nHtp(	t,	p);
		s	=	STM_nStp(	t,	p);
		Cp	=	STM_nCPtp(	t,	p);
		Cv	=	STM_nCVtp(	t,	p);
		d	=	STM_nDtp(	t,	p);
		w	=	STM_nWtp(	t,	p);
		k	=	STM_nKtp(	t,	p);

//		printf("h = \t%f\t[KJ/kg]\n",h);
//		printf("s = \t%f\t[kJ/kgK]\n",s);
		printf("w = \t%f\t[m/s]\n",w);
		printf("Cp = \t%f\t[kJ/kgK]\n",Cp);
		printf("Cv = \t%f\t[kJ/kgK]\n",Cv);
		printf("Cp/Cv = \t%f\t[]\n",Cp/Cv);
//		printf("v = \t%f\t[m3/kg]\n",d);
		printf("É» = \t%f\t;\n",k);

		//ãtä÷êî debug
//		t2	=	STM_nTps(p,	s);
//		printf("Back from p s	t = \t%f\t[Åé]\n",	t2);
//		t3	=	STM_nTph(p,	h);
//		printf("Back from p h	t = \t%f\t[Åé]\n",	t3);
	}
	return	0;
}
