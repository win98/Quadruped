//
//  QuadrupedLegAction.h
//
//  Created by Sergey on 6/4/15.
//  Copyright (c) 2015 Sergey. All rights reserved.
//

#ifndef __QuadrupedLegAction__
#define __QuadrupedLegAction__

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "QuadrupedLeg.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Quadruped leg move action struct and functions.
 */
typedef struct
{
    //public:
    QuadrLeg *leg;
    QVec3 destPosition;    
    
    float duration;
    bool stopped;
    
    //private:
    float accTime;
    QVec3 posIncrement;
} QuadrLegMoveAction;

QuadrLegMoveAction* quadrLegMoveActionCreate (QuadrLeg *leg, QVec3 destPos, float duration);
void quadrLegMoveActionModify (QuadrLegMoveAction* action, QVec3 destPos, float duration);
void quadrLegMoveActionStart (QuadrLegMoveAction* action);
void quadrLegMoveActionUpdate (QuadrLegMoveAction* action, float dt);
void quadrLegMoveActionStop (QuadrLegMoveAction* action);
void quadrLegMoveActionDelete (QuadrLegMoveAction *action);



/*
 * Quadruped leg move actions sequence structs and functions.
 */
typedef struct
{
    //public:
    uint32_t actionsCount;
    bool loop;
    QuadrLegMoveAction **actions;
    
    //private:
    QuadrLegMoveAction *current;
    uint32_t idx;
    bool stopped;
    
    void *target;
    void (*cycleFinishedCallback) (void *target, void *sequenceObject);
} QuadrLegMoveSequence;

QuadrLegMoveSequence* quadrLegMoveSequenceCreate (uint32_t actionsCount, bool loop, void *target, void(*callback)(void*, void*));
void quadrLegMoveSequenceAddAction (QuadrLegMoveSequence *sequence, QuadrLegMoveAction *action);
void quadrLegMoveSequenceStart (QuadrLegMoveSequence *sequence);
void quadrLegMoveSequenceUpdate (QuadrLegMoveSequence *sequence, float dt);
void quadrLegMoveSequenceStop (QuadrLegMoveSequence *sequence);

#ifdef __cplusplus
}
#endif
    
#endif /* defined(__QuadrupedLegAction__) */
