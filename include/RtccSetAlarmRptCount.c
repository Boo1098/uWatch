#if defined(__PIC24F__)
#include <p24Fxxxx.h>
#endif
#include "Rtcc.h"


#ifdef _TMR_RTCC_V1

/*********************************************************************
 * Function:        void RtccSetAlarmRptCount(int rptCnt, BOOL dsblAlrm)
 *
 * PreCondition:    rptCnt has to be a value less then 255 
 * Input:           rpt         - value of the desired alarm repeat rate 
 *                  dsblAlrm    - if TRUE, the API can temporarily disable
 *                                the alarm when changing the RPT value 
 * Output:          None
 * Side Effects:    None
 * Overview:        The function sets the RTCC alarm repeat rate.
 * Note:            - rptCnt will be truncated to fit into 8 bit representation.
 *                  - If alarm is enabled, changing the repeat count can be safely made
 *                  only when the sync pulse is unasserted. To avoid waiting for the sync
 *                  pulse, the user can choose to temporarily disable the alarm and then
 *                  re-enable it. This means that the user has the knowledge that an alarm
 *                  event is not imminent.
 ********************************************************************/
void RtccSetAlarmRptCount(UINT8 rptCnt, BOOL dsblAlrm)
{
   BOOL isAlrm=_RtccIsAlrmEnabled();

   if(isAlrm)
   {
       if(dsblAlrm)
       {
           mRtccAlrmDisable();
       }
       else
       {
           mRtccWaitSync();
       }
   }
   
   // update the repeat count value
   ALCFGRPTbits.ARPT=rptCnt;

   // restore previous status
   if(isAlrm && dsblAlrm)
   {
       mRtccAlrmEnable();
   }
   
}

#else
#warning "Does not build on this target"
#endif
