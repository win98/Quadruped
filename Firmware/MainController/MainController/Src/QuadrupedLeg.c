//
//  QuadrupedLeg.c
//
//  Created by Sergey on 6/4/15.
//  Copyright (c) 2015 Sergey. All rights reserved.
//

#include "QuadrupedLeg.h"
#include <stdlib.h>
#include <math.h>

QVec3 MakeQVec (float x, float y, float z)
{
    QVec3 p;
    
    p.x = x;
    p.y = y;
    p.z = z;
    
    return p;
}

QuadrLeg* quadrLegCreate (float coxaLen, float femurLen, float tibiaLen, QVec3 origPos)
{
    QuadrLeg *leg = (QuadrLeg *)malloc(sizeof(QuadrLeg));
    
    leg->coxaLength = coxaLen;
    leg->femurLength = femurLen;
    leg->tibiaLength = tibiaLen;
    
    leg->origFootPos = origPos;
    leg->curFootPos = origPos;
    
    return leg;
}

void quadrLegUpdate (QuadrLeg *leg, float dt)
{
    // Update angles here.
    // Coxa angle.
    float x = leg->curFootPos.x;
    float y = leg->curFootPos.z;
    float angle = atan2f(y, x);
    leg->coxaAngle = angle;
    
    // Coxa-femur angle.
    x = leg->curFootPos.x;
    y = leg->curFootPos.y;
    float L1 = powf(x * x + y * y, 0.5f);
    x = L1 - leg->coxaLength;
    y = leg->curFootPos.y;
    float L = powf(x * x + y * y, 0.5f);
    float zOffset = fabsf(leg->curFootPos.y);
    float a1 = acosf(zOffset / L);
    float quadTib = leg->tibiaLength * leg->tibiaLength;
    float quadFem = leg->femurLength * leg->femurLength;
    float cosa2 = (quadTib - quadFem - L * L) / (-2.0f * leg->femurLength * L);
    float a2 = acosf(cosa2);
    leg->coxaFemurAngle = a1 + a2;
    
    // Femur-tibia angle.
    float cosb = (L * L - quadTib - quadFem) / (-2.0f * leg->tibiaLength * leg->femurLength);
    leg->femurTibiaAngle = acosf(cosb);
}

void quadrLegDelete (QuadrLeg *leg)
{
    free(leg);
}