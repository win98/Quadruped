//
//  QuadrupedCreepGait.h
//
//  Created by Sergey on 6/4/15.
//  Copyright (c) 2015 Sergey. All rights reserved.
//
//http://blog.oscarliang.net/inverse-kinematics-and-trigonometry-basics/
//http://blog.oscarliang.net/quadruped-robot-gait-study/


#ifndef __QuadrupedCreepGait__
#define __QuadrupedCreepGait__

#include <stdio.h>
#include "QuadrupedLeg.h"
#include "QuadrupedLegAction.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MIN_STRAIGHT_VELOCITY      -20.0f
#define MAX_STRAIGHT_VELOCITY       20.0f

#define MIN_STEP_SIZE               5.0f
#define MAX_STEP_SIZE               20.0f
#define STEP_HEIGHT                 5.0f

typedef struct
{
    //private:
    float angularVelocity;
    float straightVelocity;     // cm/sec.
    float lateralVelocity;      // cm/sec.
    
    bool reverseStraight;
    
    QuadrLeg *RFLeg;
    QuadrLeg *RHLeg;
    QuadrLeg *LFLeg;
    QuadrLeg *LHLeg;
    
    QuadrLegMoveSequence *RFsequence;
    QuadrLegMoveSequence *RHsequence;
    QuadrLegMoveSequence *LFsequence;
    QuadrLegMoveSequence *LHsequence;
  
    QVec3 step;
    float timeUinit;
} QuadrCreepGait;
    
QuadrCreepGait* quadrupedCreepGaitCreate (QuadrLeg *RFLeg, QuadrLeg *RHLeg, QuadrLeg *LFLeg, QuadrLeg *LHLeg);
void quadrupedCreepGaitStart (QuadrCreepGait *creepGait);
void quadrupedCreepGaitUpdate (QuadrCreepGait *creepGait, float dt);
void quadrupedCreepGaitSetAngularVelocity (QuadrCreepGait *creepGait, float angVel);
void quadrupedCreepGaitSetStraightVelocity (QuadrCreepGait *creepGait, float strVel);
void quadrupedCreepGaitSetLateralVelocity (QuadrCreepGait *creepGait, float latVel);
void quadrupedCreepGaitSetBodyLevel (QuadrCreepGait *creepGait, float level);
    
#ifdef __cplusplus
}
#endif

#endif /* defined(__QuadrupedCreepGait__) */
