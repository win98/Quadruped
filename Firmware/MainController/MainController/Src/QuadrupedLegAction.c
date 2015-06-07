//
//  QuadrupedLegAction.c
//
//  Created by Sergey on 6/4/15.
//  Copyright (c) 2015 Sergey. All rights reserved.
//

#include "QuadrupedLegAction.h"
#include <stdlib.h>

/*
 * Quadruped leg move action functions.
 */

QuadrLegMoveAction* quadrLegMoveActionCreate (QuadrLeg *leg, QVec3 destPos, float duration)
{
    QuadrLegMoveAction *action = (QuadrLegMoveAction *)malloc(sizeof(QuadrLegMoveAction));
    
    action->leg = leg;
    action->destPosition = destPos;
    action->duration = duration;
    action->stopped = true;
    
    return action;
}

void quadrLegMoveActionModify (QuadrLegMoveAction* action, QVec3 destPos, float duration)
{
    action->destPosition = destPos;
    action->duration = duration;
}

void quadrLegMoveActionStart (QuadrLegMoveAction* action)
{
    action->posIncrement = MakeQVec((action->destPosition.x - action->leg->curFootPos.x) / action->duration,
                                    (action->destPosition.y - action->leg->curFootPos.y) / action->duration,
                                    (action->destPosition.z - action->leg->curFootPos.z) / action->duration);
    action->accTime = 0.0f;
    action->stopped = false;
}

void quadrLegMoveActionUpdate (QuadrLegMoveAction* action, float dt)
{
    if (!action->stopped)
    {
        action->accTime += dt;
        if (action->accTime >= action->duration)
        {
            quadrLegMoveActionStop(action);
            return;
        }
        
        action->leg->curFootPos.x += action->posIncrement.x * dt;
        action->leg->curFootPos.y += action->posIncrement.y * dt;
        action->leg->curFootPos.z += action->posIncrement.z * dt;
    }
}

void quadrLegMoveActionStop (QuadrLegMoveAction* action)
{
    action->leg->curFootPos = action->destPosition;
    action->stopped = true;
}

void quadrLegMoveActionDelete (QuadrLegMoveAction *action)
{
    free(action);
}



/*
 * Quadruped leg move actions sequence functions.
 */

QuadrLegMoveSequence* quadrLegMoveSequenceCreate (uint32_t actionsCount, bool loop, void *target, void(*callback)(void*, void*))
{
    QuadrLegMoveSequence *sequence = (QuadrLegMoveSequence *)malloc(sizeof(QuadrLegMoveSequence));
    sequence->actions = (QuadrLegMoveAction **)malloc(sizeof(QuadrLegMoveAction*) * actionsCount);
    sequence->idx = 0;
    sequence->actionsCount = 0;
    sequence->loop = loop;
    sequence->stopped = true;
    sequence->target = target;
    sequence->cycleFinishedCallback = callback;
    
    return sequence;
}

void quadrLegMoveSequenceAddAction (QuadrLegMoveSequence *sequence, QuadrLegMoveAction *action)
{
    sequence->actions[sequence->actionsCount++] = action;
}

void quadrLegMoveSequenceStart (QuadrLegMoveSequence *sequence)
{
    sequence->stopped = false;
    sequence->idx = 0;
    sequence->current = sequence->actions[sequence->idx];
    quadrLegMoveActionStart(sequence->current);
}

void quadrLegMoveSequenceUpdate (QuadrLegMoveSequence *sequence, float dt)
{
    if (!sequence->stopped)
    {
        quadrLegMoveActionUpdate(sequence->current, dt);
        if (sequence->current->stopped)
        {
            sequence->idx++;
            
            if (sequence->idx >= sequence->actionsCount)
            {
                // Invoke callback.
                sequence->cycleFinishedCallback(sequence->target, sequence);
                
                if (!sequence->loop)
                {
                    quadrLegMoveSequenceStop(sequence);
                    
                    return;
                }
                else
                {
                    sequence->idx = 0;
                }
            }
            
            sequence->current = sequence->actions[sequence->idx];
            quadrLegMoveActionStart(sequence->current);
        }
    }
}

void quadrLegMoveSequenceStop (QuadrLegMoveSequence *sequence)
{
    sequence->stopped = true;
}