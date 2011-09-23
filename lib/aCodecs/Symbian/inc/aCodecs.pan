/*
* ============================================================================
*  Name     : aCodecs.pan
*  Part of  : aCodecs
*  Created  : 20.04.2005 by 
*  Description:
*     aCodecs.pan - panic codes
*  Version  :
*  Copyright: 
* ============================================================================
*/


#ifndef __ACODECS_PAN__
#define __ACODECS_PAN__


//  Data Types

enum TaCodecsPanic
    {
    EaCodecsNullPointer
    };


//  Function Prototypes

GLREF_C void Panic(TaCodecsPanic aPanic);


#endif  // __ACODECS_PAN__

// End of file
