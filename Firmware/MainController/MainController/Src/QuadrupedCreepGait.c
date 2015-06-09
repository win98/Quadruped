//
//  QuadrupedCreepGait.c
//
//  Created by Sergey on 6/4/15.
//  Copyright (c) 2015 Sergey. All rights reserved.
//

#include "QuadrupedCreepGait.h"
#include <stdlib.h>
#include <math.h>

#define TIME_UNITS_NUM_IN_LEG_CYCLE     16

#define MIN_STRAIGHT_VELOCITY      -20.0f
#define MAX_STRAIGHT_VELOCITY       20.0f

#define MAX_STEP_SIZE               20.0f
#define STEP_HEIGHT                 3.0f

#define ACTIONS_IN_SEQUENCE     7

uint32_t RFactionsOrderList[ACTIONS_IN_SEQUENCE] = { 0, 1, 2, 3, 4, 5, 6 };
uint32_t RHactionsOrderList[ACTIONS_IN_SEQUENCE] = { 6, 0, 1, 2, 3, 4, 5 };
uint32_t LFactionsOrderList[ACTIONS_IN_SEQUENCE] = { 3, 4, 5, 6, 0, 1, 2 };
uint32_t LHactionsOrderList[ACTIONS_IN_SEQUENCE] = { 1, 2, 3, 4, 5, 6, 0 };

void quadrupedCreepGaitLegCycleFinished (void *target, void *object);
void quadrupedCreepGaitModifyActions (QuadrCreepGait *creepGate, QuadrLegMoveSequence *sequence);


QuadrCreepGait* quadrupedCreepGaitCreate (QuadrLeg *RFLeg, QuadrLeg *RHLeg, QuadrLeg *LFLeg, QuadrLeg *LHLeg)
{
    QuadrCreepGait *result = (QuadrCreepGait *)malloc(sizeof(QuadrCreepGait));
    
    result->RFLeg = RFLeg;
    result->RHLeg = RHLeg;
    result->LFLeg = LFLeg;
    result->LHLeg = LHLeg;
    
    result->RFsequence = quadrLegMoveSequenceCreate(ACTIONS_IN_SEQUENCE, true, result, quadrupedCreepGaitLegCycleFinished);
    result->RHsequence = quadrLegMoveSequenceCreate(ACTIONS_IN_SEQUENCE, true, result, quadrupedCreepGaitLegCycleFinished);
    result->LFsequence = quadrLegMoveSequenceCreate(ACTIONS_IN_SEQUENCE, true, result, quadrupedCreepGaitLegCycleFinished);
    result->LHsequence = quadrLegMoveSequenceCreate(ACTIONS_IN_SEQUENCE, true, result, quadrupedCreepGaitLegCycleFinished);
    
    result->step = MakeQVec(0, 0, 0);
    result->timeUinit = 0;
    
    QuadrLegMoveAction *action;
    QuadrLeg *RF = RFLeg;
    QuadrLegMoveSequence *RFsequence = result->RFsequence;
    QuadrLeg *RH = RHLeg;
    QuadrLegMoveSequence *RHsequence = result->RHsequence;
    QuadrLeg *LF = LFLeg;
    QuadrLegMoveSequence *LFsequence = result->LFsequence;
    QuadrLeg *LH = LHLeg;
    QuadrLegMoveSequence *LHsequence = result->LHsequence;
    
    // RF leg.
    action = quadrLegMoveActionCreate(RF, RF->origFootPos, 0);
    quadrLegMoveSequenceAddAction(RFsequence, action);
    action = quadrLegMoveActionCreate(RF, RF->origFootPos, 0);
    quadrLegMoveSequenceAddAction(RFsequence, action);
    action = quadrLegMoveActionCreate(RF, RF->origFootPos, 0);
    quadrLegMoveSequenceAddAction(RFsequence, action);
    action = quadrLegMoveActionCreate(RF, RF->origFootPos, 0);
    quadrLegMoveSequenceAddAction(RFsequence, action);
    action = quadrLegMoveActionCreate(RF, RF->origFootPos, 0);
    quadrLegMoveSequenceAddAction(RFsequence, action);
    action = quadrLegMoveActionCreate(RF, RF->origFootPos, 0);
    quadrLegMoveSequenceAddAction(RFsequence, action);
    action = quadrLegMoveActionCreate(RF, RF->origFootPos, 0);
    quadrLegMoveSequenceAddAction(RFsequence, action);
    
    // RH leg.
    action = quadrLegMoveActionCreate(RH, RH->origFootPos, 0);
    quadrLegMoveSequenceAddAction(RHsequence, action);
    action = quadrLegMoveActionCreate(RH, RH->origFootPos, 0);
    quadrLegMoveSequenceAddAction(RHsequence, action);
    action = quadrLegMoveActionCreate(RH, RH->origFootPos, 0);
    quadrLegMoveSequenceAddAction(RHsequence, action);
    action = quadrLegMoveActionCreate(RH, RH->origFootPos, 0);
    quadrLegMoveSequenceAddAction(RHsequence, action);
    action = quadrLegMoveActionCreate(RH, RH->origFootPos, 0);
    quadrLegMoveSequenceAddAction(RHsequence, action);
    action = quadrLegMoveActionCreate(RH, RH->origFootPos, 0);
    quadrLegMoveSequenceAddAction(RHsequence, action);
    action = quadrLegMoveActionCreate(RH, RH->origFootPos, 0);
    quadrLegMoveSequenceAddAction(RHsequence, action);
    
    // LF leg.
    action = quadrLegMoveActionCreate(LF, LF->origFootPos, 0);
    quadrLegMoveSequenceAddAction(LFsequence, action);
    action = quadrLegMoveActionCreate(LF, LF->origFootPos, 0);
    quadrLegMoveSequenceAddAction(LFsequence, action);
    action = quadrLegMoveActionCreate(LF, LF->origFootPos, 0);
    quadrLegMoveSequenceAddAction(LFsequence, action);
    action = quadrLegMoveActionCreate(LF, LF->origFootPos, 0);
    quadrLegMoveSequenceAddAction(LFsequence, action);
    action = quadrLegMoveActionCreate(LF, LF->origFootPos, 0);
    quadrLegMoveSequenceAddAction(LFsequence, action);
    action = quadrLegMoveActionCreate(LF, LF->origFootPos, 0);
    quadrLegMoveSequenceAddAction(LFsequence, action);
    action = quadrLegMoveActionCreate(LF, LF->origFootPos, 0);
    quadrLegMoveSequenceAddAction(LFsequence, action);
    
    // LH leg.
    action = quadrLegMoveActionCreate(LH, LH->origFootPos, 0);
    quadrLegMoveSequenceAddAction(LHsequence, action);
    action = quadrLegMoveActionCreate(LH, LH->origFootPos, 0);
    quadrLegMoveSequenceAddAction(LHsequence, action);
    action = quadrLegMoveActionCreate(LH, LH->origFootPos, 0);
    quadrLegMoveSequenceAddAction(LHsequence, action);
    action = quadrLegMoveActionCreate(LH, LH->origFootPos, 0);
    quadrLegMoveSequenceAddAction(LHsequence, action);
    action = quadrLegMoveActionCreate(LH, LH->origFootPos, 0);
    quadrLegMoveSequenceAddAction(LHsequence, action);
    action = quadrLegMoveActionCreate(LH, LH->origFootPos, 0);
    quadrLegMoveSequenceAddAction(LHsequence, action);
    action = quadrLegMoveActionCreate(LH, LH->origFootPos, 0);
    quadrLegMoveSequenceAddAction(LHsequence, action);
    
    return result;
}

void quadrupedCreepGaitStart (QuadrCreepGait *creepGait)
{
    quadrLegMoveSequenceStart(creepGait->RFsequence);
    quadrLegMoveSequenceStart(creepGait->RHsequence);
    quadrLegMoveSequenceStart(creepGait->LFsequence);
    quadrLegMoveSequenceStart(creepGait->LHsequence);
}

void quadrupedCreepGaitUpdate (QuadrCreepGait *creepGait, float dt)
{
    quadrLegMoveSequenceUpdate(creepGait->RFsequence, dt);
    quadrLegMoveSequenceUpdate(creepGait->RHsequence, dt);
    quadrLegMoveSequenceUpdate(creepGait->LFsequence, dt);
    quadrLegMoveSequenceUpdate(creepGait->LHsequence, dt);
    
    quadrLegUpdate(creepGait->RFLeg, dt);
    quadrLegUpdate(creepGait->RHLeg, dt);
    quadrLegUpdate(creepGait->LFLeg, dt);
    quadrLegUpdate(creepGait->LHLeg, dt);
}

void quadrupedCreepGaitSetAngularVelocity (QuadrCreepGait *creepGait, float angVel)
{
    
}

void quadrupedCreepGaitSetStraightVelocity (QuadrCreepGait *creepGait, float strVel)
{
    if (strVel > MAX_STRAIGHT_VELOCITY && strVel < MIN_STRAIGHT_VELOCITY)
    {
        return;
    }
    
    float stepY = strVel == 0.0f ? 0 : STEP_HEIGHT;
    // New step.
    creepGait->step = MakeQVec(creepGait->step.x, stepY, MAX_STEP_SIZE / MAX_STRAIGHT_VELOCITY * strVel);
    // New time unit.
    creepGait->timeUinit = strVel == 0.0f ? 0 : (creepGait->step.z / strVel) / TIME_UNITS_NUM_IN_LEG_CYCLE;
}

void quadrupedCreepGaitSetLateralVelocity (QuadrCreepGait *creepGait, float latVel)
{
    
}

void quadrupedCreepGaitSetBodyLevel (QuadrCreepGait *creepGait, float level)
{
    
}

void quadrupedCreepGaitModifyActions (QuadrCreepGait *creepGate, QuadrLegMoveSequence *sequence)
{
    float STEP_Y = creepGate->step.y;
    float HALF_STEP_Z = creepGate->step.z * 0.5f;
    
    QuadrLeg *leg;
    uint32_t *actionsOrderList;
    
    if (sequence == creepGate->RFsequence)
    {
        leg = creepGate->RFLeg;
        actionsOrderList = RFactionsOrderList;
    }
    else if (sequence == creepGate->RHsequence)
    {
        leg = creepGate->RHLeg;
        actionsOrderList = RHactionsOrderList;
    }
    else if (sequence == creepGate->LFsequence)
    {
        leg = creepGate->LFLeg;
        actionsOrderList = LFactionsOrderList;
    }
    else if (sequence == creepGate->LHsequence)
    {
        leg = creepGate->LHLeg;
        actionsOrderList = LHactionsOrderList;
    }
    else
    {
        return;
    }
    
    
    float x = leg->origFootPos.x;
    float y = leg->origFootPos.y;
    float z = leg->origFootPos.z;
    
    float tu = creepGate->timeUinit;
    
    uint32_t i = 0;
    uint32_t idx;
    QuadrLegMoveAction *action;
    
    idx = actionsOrderList[i++];
    action = sequence->actions[idx];
    quadrLegMoveActionModify(action, MakeQVec(  x,                      y,                  z - HALF_STEP_Z),               tu * 4);
    idx = actionsOrderList[i++];
    action = sequence->actions[idx];
    quadrLegMoveActionModify(action, MakeQVec(  x,                      y + STEP_Y,         z - HALF_STEP_Z),               tu);
    idx = actionsOrderList[i++];
    action = sequence->actions[idx];
    quadrLegMoveActionModify(action, MakeQVec(  x,                      y + STEP_Y,         z + HALF_STEP_Z * 0.25f),       tu * 2);
    idx = actionsOrderList[i++];
    action = sequence->actions[idx];
    quadrLegMoveActionModify(action, MakeQVec(  x,                      y,                  z + HALF_STEP_Z),               tu);
    idx = actionsOrderList[i++];
    action = sequence->actions[idx];
    quadrLegMoveActionModify(action, MakeQVec(  x,                      y,                  z + HALF_STEP_Z),               tu);
    idx = actionsOrderList[i++];
    action = sequence->actions[idx];
    quadrLegMoveActionModify(action, MakeQVec(  x,                      y,                  z + HALF_STEP_Z * 0.4f),        tu * 3);
    idx = actionsOrderList[i++];
    action = sequence->actions[idx];
    quadrLegMoveActionModify(action, MakeQVec(  x,                      y,                  z - HALF_STEP_Z * 0.27f),       tu * 4);
}

void quadrupedCreepGaitLegCycleFinished (void *target, void *object)
{
    QuadrLegMoveSequence *sequence = (QuadrLegMoveSequence *)object;
    QuadrCreepGait *creepGate = (QuadrCreepGait *)target;
    
    quadrupedCreepGaitModifyActions(creepGate, sequence);
}
