//
//  QuadrupedLeg.h
//
//  Created by Sergey on 6/4/15.
//  Copyright (c) 2015 Sergey. All rights reserved.
//
#ifndef __QuadrupedLeg__
#define __QuadrupedLeg__

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define M_PI        3.14159f
#define M_PI_2      M_PI * 0.5f
#define M_PI_4      M_PI * 0.25f

/*
 * Quadruped leg structs and functions.
 */
typedef struct
{
    float x;
    float y;
    float z;
} QVec3;

QVec3 MakeQVec (float x, float y, float z);

typedef struct
{
    //public:
    QVec3 origFootPos;          // Relative to coxa.
    QVec3 curFootPos;           // Relative to coxa.
    
    float coxaLength;
    float femurLength;
    float tibiaLength;
    
    float coxaAngle;
    float coxaFemurAngle;
    float femurTibiaAngle;
} QuadrLeg;

QuadrLeg* quadrLegCreate (float coxaLen, float femurLen, float tibiaLen, QVec3 origPos);
void quadrLegUpdate (QuadrLeg *leg, float dt);
void quadrLegDelete (QuadrLeg *leg);

#ifdef __cplusplus
}
#endif
    
#endif /* defined(__QuadrupedLeg__) */
