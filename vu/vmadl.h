#include "vu.h"

void VMADL(int vd, int vs, int vt, int element)
{
    register unsigned int product;
    register int i, j;

    if (element == 0x0) /* if (element >> 1 == 00) */
    {
        for (i = 0; i < 8; i++)
        {
            product = (unsigned short)VR[vs].s[i] * (unsigned short)VR[vt].s[i];
            product >>= 16;
            VACC[i].DW += product;
        }
    }
    else if ((element & 0xE) == 02) /* scalar quarter */
    {
        for (i = 0; i < 8; i++)
        {
            j = (i & 0xE) | (element & 01);
            product = (unsigned short)VR[vs].s[i] * (unsigned short)VR[vt].s[j];
            product >>= 16;
            VACC[i].DW += product;
        }
    }
    else if ((element & 0xC) == 04) /* scalar half */
    {
        for (i = 0; i < 8; i++)
        {
            j = (i & 0xC) | (element & 03);
            product = (unsigned short)VR[vs].s[i] * (unsigned short)VR[vt].s[j];
            product >>= 16;
            VACC[i].DW += product;
        }
    }
    else /* if ((element & 0b1000) == 0b1000) /* scalar whole */
    {
        const register unsigned short m = (unsigned short)VR[vt].s[element % 8];

        for (i = 0; i < 8; i++)
        {
            product = (unsigned short)VR[vs].s[i] * m;
            product >>= 16;
            VACC[i].DW += product;
        }
    }
    for (i = 0; i < 8; i++) /* Signed-clamp bits 15..0 of ACC to dest. VR. */
        if (VACC[i].DW & 0x800000000000) /* acc < 0 */
            if (~VACC[i].DW & ~0x00007FFFFFFF) /* short underflow */
                VR[vd].s[i] = 0x0000;
            else
                VR[vd].s[i] = VACC[i].s[LO];
        else
            if (VACC[i].DW & ~0x00007FFFFFFF) /* short overflow */
                VR[vd].s[i] = 0xFFFF;
            else
                VR[vd].s[i] = VACC[i].s[LO];
    return;
}
