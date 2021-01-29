#include "ntc.h"



u16 GetTempreture(u16 ntcy)
{
    u16 i;
    float temp;
	  u16 temp_S=0;
    if(ntcy>=17212)temp=0;
    else if(ntcy<=67)temp=130*10;
    else {
        for(i=1; i<=130; i++)
            if(ntcy>ntc1[i])
            {   temp=10*(i-1);
                temp+=(float)(ntc1[i-1]-ntcy)/(ntc1[i-1]-ntc1[i])*10;
                i=131;
            }
    }
		temp_S = temp -300;
    return temp;
}









