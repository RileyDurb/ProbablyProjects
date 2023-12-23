#pragma once

// Include all node headers in this file

// Example Control Flow Nodes
#include "ControlFlow/C_ParallelSequencer.h"
#include "ControlFlow/C_RandomSelector.h"
#include "ControlFlow/C_Selector.h"
#include "ControlFlow/C_Sequencer.h"

// Student Control Flow Nodes


// Example Decorator Nodes
#include "Decorator/D_Delay.h"
#include "Decorator/D_InvertedRepeater.h"
#include "Decorator/D_RepeatFourTimes.h"

// Student Decorator Nodes
#include "Decorator/D_HasPartner.h"
#include "Decorator/D_RepeatThreeTimes.h"
#include "Decorator/D_ShouldDoRUN.h"
#include "Decorator/D_RPSDone.h"
#include "Decorator/D_HasChampion.h"
#include "Decorator/D_SupportedChampionWon.h"


// Example Leaf Nodes
#include "Leaf/L_CheckMouseClick.h"
#include "Leaf/L_Idle.h"
#include "Leaf/L_MoveToFurthestAgent.h"
#include "Leaf/L_MoveToMouseClick.h"
#include "Leaf/L_MoveToRandomPosition.h"
#include "Leaf/L_PlaySound.h"

// Student Leaf Nodes
#include "Leaf/L_CheckRangeBattleStart.h"
#include "Leaf/L_CheckDelay.h"
#include "Leaf/L_PlayRPS.h"
#include "Leaf/L_RPSShake.h"
#include "Leaf/L_ChooseRPS.h"
#include "Leaf/L_RUN.h"
#include "Leaf/L_ChasePartner.h"
#include "Leaf/L_RPSRoundDelay.h"
#include "Leaf/L_SetHandPosition.h"
#include "Leaf/L_CheckRangeForChampion.h"
#include "Leaf/L_ApplyCurrVelocity.h"
#include "Leaf/L_Celebrate.h"
#include "Leaf/L_ClearChampion.h"
#include "Leaf/L_ScardeyRUN.h"
#include "Leaf/L_SetFindRange.h"
#include "Leaf/L_CheckRangeForFight.h"
#include "Leaf/L_PatrolToNextCorner.h"
#include "Leaf/L_MoveToParentLocation.h"
#include "Leaf/L_TargetSupportedChampion.h"
#include "Leaf/L_StopAgentsAroundMe.h"