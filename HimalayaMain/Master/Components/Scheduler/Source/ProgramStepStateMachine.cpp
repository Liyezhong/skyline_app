/****************************************************************************/
/*! \file ProgramStepStateMachine.cpp
 *
 *  \brief CProgramStepStateMachine class definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 02.01.2014
 *   $Author:  $ Quan Zhang
 *
 *  \b Company:
 *
 *       Leica Biosystems SH CN.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#include "../Include/ProgramStepStateMachine.h"
#include "../Include/HimalayaDeviceEventCodes.h"
#include <QHistoryState>

namespace Scheduler
{
/****************************************************************************/
/*!
 *  \brief    Constructor
 *  \iparam   pParentState: Pointer to parent state
 *  \iparam   pErrorState: Pointer to Error state
 */
/****************************************************************************/
CProgramStepStateMachine::CProgramStepStateMachine(QState* pParentState, QState* pErrorState)
{
    if((pParentState)&&(pErrorState))
    {
        m_PreviousState = SM_UNDEF;

        mp_PssmInit = new QState(pParentState);
        mp_PssmPreTest = new QState(pParentState);
        mp_PssmReadyToHeatLevelSensorS1 = new QState(pParentState);
        mp_PssmReadyToHeatLevelSensorS2 = new QState(pParentState);
        mp_PssmReadyToTubeBefore = new QState(pParentState);
        mp_PssmReadyToTubeAfter = new QState(pParentState);
        mp_PssmReadyToSeal = new QState(pParentState);
        mp_PssmReadyToFill = new QState(pParentState);
        mp_PssmReadyToDrain = new QState(pParentState);
        mp_PssmSoak = new QState(pParentState);
        mp_PssmStepFinish = new QState(pParentState);
        mp_PssmError = new QState(pParentState);
        mp_PssmPause = new QState(pParentState);
        mp_PssmPauseDrain = new QState(pParentState);
        mp_PssmAborting = new QState(pParentState);
        mp_PssmAborted = new QState(pParentState);
        mp_PssmProgramFinish = new QFinalState(pParentState);

        pParentState->setInitialState(mp_PssmInit);
        mp_PssmInit->addTransition(this, SIGNAL(PreTestSig()), mp_PssmPreTest);
        mp_PssmPreTest->addTransition(this, SIGNAL(StDone()), mp_PssmInit);
        mp_PssmPreTest->addTransition(this, SIGNAL(Pause()), mp_PssmPause);
        mp_PssmPreTest->addTransition(this, SIGNAL(Error()), pErrorState);

<<<<<<< HEAD
        mp_PssmInit->addTransition(this, SIGNAL(CleaningMoveRVPosSig()), mp_PssmStDone);

        mp_PssmStInit->addTransition(this, SIGNAL(StInitOK()), mp_PssmStTempChecking);
        mp_PssmStTempChecking->addTransition(this, SIGNAL(StTempOK()), mp_PssmStCurrentChecking);
        mp_PssmStCurrentChecking->addTransition(this, SIGNAL(StCurrentOK()), mp_PssmStVoltageChecking);
        mp_PssmStVoltageChecking->addTransition(this, SIGNAL(StVoltageOK()), mp_PssmStRVPositionChecking);
        mp_PssmStRVPositionChecking->addTransition(this, SIGNAL(StRVPositionOK()), mp_PssmStPressureChecking);
        mp_PssmStPressureChecking->addTransition(this, SIGNAL(StPressureOK()), mp_PssmStSealingChecking);
        mp_PssmStSealingChecking->addTransition(this, SIGNAL(StSealingOK()), mp_PssmStStationChecking);
        mp_PssmStStationChecking->addTransition(this, SIGNAL(StGetStationcheckResult()), mp_PssmStStationCheckFinish);
        mp_PssmStStationCheckFinish->addTransition(this, SIGNAL(StStationLeft()), mp_PssmStStationChecking);
        mp_PssmStStationCheckFinish->addTransition(this, SIGNAL(StStationOK()), mp_PssmStDone);

        mp_PssmSelfTest->addTransition(this, SIGNAL(Pause()), mp_PssmPause);
        //mp_PssmPause->addTransition(this, SIGNAL(ResumeToSelftest()), stHistory);
        mp_PssmSelfTest->addTransition(this, SIGNAL(Error()), pErrorState);
        pErrorState->addTransition(this, SIGNAL(ResumeToSelftest()), mp_PssmSelfTest);//verify later
||||||| merged common ancestors
        mp_PssmStInit->addTransition(this, SIGNAL(StInitOK()), mp_PssmStTempChecking);
        mp_PssmStTempChecking->addTransition(this, SIGNAL(StTempOK()), mp_PssmStCurrentChecking);
        mp_PssmStCurrentChecking->addTransition(this, SIGNAL(StCurrentOK()), mp_PssmStVoltageChecking);
        mp_PssmStVoltageChecking->addTransition(this, SIGNAL(StVoltageOK()), mp_PssmStRVPositionChecking);
        mp_PssmStRVPositionChecking->addTransition(this, SIGNAL(StRVPositionOK()), mp_PssmStPressureChecking);
        mp_PssmStPressureChecking->addTransition(this, SIGNAL(StPressureOK()), mp_PssmStSealingChecking);
        mp_PssmStSealingChecking->addTransition(this, SIGNAL(StSealingOK()), mp_PssmStStationChecking);
        mp_PssmStStationChecking->addTransition(this, SIGNAL(StGetStationcheckResult()), mp_PssmStStationCheckFinish);
        mp_PssmStStationCheckFinish->addTransition(this, SIGNAL(StStationLeft()), mp_PssmStStationChecking);
        mp_PssmStStationCheckFinish->addTransition(this, SIGNAL(StStationOK()), mp_PssmStDone);

        mp_PssmSelfTest->addTransition(this, SIGNAL(Pause()), mp_PssmPause);
        //mp_PssmPause->addTransition(this, SIGNAL(ResumeToSelftest()), stHistory);
        mp_PssmSelfTest->addTransition(this, SIGNAL(Error()), pErrorState);
        pErrorState->addTransition(this, SIGNAL(ResumeToSelftest()), mp_PssmSelfTest);//verify later
=======
        pErrorState->addTransition(this, SIGNAL(ResumeToSelftest()), mp_PssmPreTest);//verify later
>>>>>>> Initial version of Refacting Scheduler
        pErrorState->addTransition(this, SIGNAL(ResumeToSoak()), mp_PssmSoak);
        pErrorState->addTransition(this, SIGNAL(ResumeFromErrorToBegin()), mp_PssmInit);
        mp_PssmPreTest->addTransition(this, SIGNAL(Abort()), mp_PssmAborted);

        mp_PssmInit->addTransition(this, SIGNAL(TempsReady()), mp_PssmReadyToHeatLevelSensorS1);
        mp_PssmReadyToHeatLevelSensorS1->addTransition(this, SIGNAL(LevelSensorTempS1Ready()), mp_PssmReadyToHeatLevelSensorS2);
        mp_PssmReadyToHeatLevelSensorS2->addTransition(this, SIGNAL(LevelSensorTempS2Ready()), mp_PssmReadyToFill);
        mp_PssmReadyToFill->addTransition(this, SIGNAL(FillFinished()), mp_PssmReadyToSeal);
        mp_PssmReadyToSeal->addTransition(this, SIGNAL(HitSeal()), mp_PssmSoak);
        mp_PssmSoak->addTransition(this, SIGNAL(SoakFinished()), mp_PssmReadyToTubeAfter);
        mp_PssmReadyToTubeAfter->addTransition(this, SIGNAL(HitTubeAfter()), mp_PssmReadyToDrain);
        mp_PssmReadyToDrain->addTransition(this, SIGNAL(DrainFinished()), mp_PssmReadyToTubeBefore);
        mp_PssmReadyToTubeBefore->addTransition(this, SIGNAL(HitTubeBefore()), mp_PssmStepFinish);
        mp_PssmStepFinish->addTransition(this, SIGNAL(ProgramFinished()), mp_PssmProgramFinish);
        mp_PssmStepFinish->addTransition(this, SIGNAL(StepFinished()), mp_PssmInit);

        mp_PssmInit->addTransition(this, SIGNAL(Error()), pErrorState);
        mp_PssmReadyToHeatLevelSensorS1->addTransition(this, SIGNAL(Error()), pErrorState);
        mp_PssmReadyToHeatLevelSensorS2->addTransition(this, SIGNAL(Error()), pErrorState);
        mp_PssmReadyToTubeBefore->addTransition(this, SIGNAL(Error()), pErrorState);
        mp_PssmReadyToTubeAfter->addTransition(this, SIGNAL(Error()), pErrorState);
        mp_PssmReadyToSeal->addTransition(this, SIGNAL(Error()), pErrorState);
        mp_PssmReadyToFill->addTransition(this, SIGNAL(Error()), pErrorState);
        mp_PssmReadyToDrain->addTransition(this, SIGNAL(Error()), pErrorState);
        mp_PssmSoak->addTransition(this, SIGNAL(Error()), pErrorState);

        mp_PssmInit->addTransition(this, SIGNAL(Pause()), mp_PssmPause);
        mp_PssmReadyToHeatLevelSensorS1->addTransition(this, SIGNAL(Pause()), mp_PssmPause);
        mp_PssmReadyToHeatLevelSensorS2->addTransition(this, SIGNAL(Pause()), mp_PssmPause);
        mp_PssmReadyToTubeBefore->addTransition(this, SIGNAL(Pause()), mp_PssmPause);
        mp_PssmReadyToSeal->addTransition(this, SIGNAL(Pause()), mp_PssmPause);
        mp_PssmReadyToFill->addTransition(this, SIGNAL(Pause()), mp_PssmPause);
        mp_PssmSoak->addTransition(this, SIGNAL(Pause()), mp_PssmPause);
        mp_PssmStepFinish->addTransition(this, SIGNAL(Pause()), mp_PssmPause);

        mp_PssmReadyToTubeAfter->addTransition(this, SIGNAL(Pause()), mp_PssmPauseDrain);
        mp_PssmReadyToDrain->addTransition(this, SIGNAL(Pause()), mp_PssmPauseDrain);

        mp_PssmInit->addTransition(this, SIGNAL(Abort()), mp_PssmAborted);
        mp_PssmReadyToHeatLevelSensorS1->addTransition(this, SIGNAL(Abort()), mp_PssmAborted);
        mp_PssmReadyToHeatLevelSensorS2->addTransition(this, SIGNAL(Abort()), mp_PssmAborted);
        mp_PssmReadyToTubeBefore->addTransition(this, SIGNAL(Abort()), mp_PssmAborted);
        mp_PssmStepFinish->addTransition(this, SIGNAL(Abort()), mp_PssmAborted);
        mp_PssmAborting->addTransition(this, SIGNAL(Abort()), mp_PssmAborted);

        mp_PssmReadyToFill->addTransition(this, SIGNAL(Abort()), mp_PssmAborting);
        mp_PssmReadyToSeal->addTransition(this, SIGNAL(Abort()), mp_PssmAborting);
        mp_PssmSoak->addTransition(this, SIGNAL(Abort()), mp_PssmAborting);
        mp_PssmReadyToTubeAfter->addTransition(this, SIGNAL(Abort()), mp_PssmAborting);
        mp_PssmReadyToDrain->addTransition(this, SIGNAL(Abort()), mp_PssmAborting);


        mp_PssmPause->addTransition(this, SIGNAL(ResumeToInit()), mp_PssmInit);
        mp_PssmPause->addTransition(this, SIGNAL(ResumeToHeatLevelSensorS1()), mp_PssmReadyToHeatLevelSensorS1);
        mp_PssmPause->addTransition(this, SIGNAL(ResumeToHeatLevelSensorS2()), mp_PssmReadyToHeatLevelSensorS2);
        mp_PssmPause->addTransition(this, SIGNAL(ResumeToReadyToFill()), mp_PssmReadyToFill);
        mp_PssmPause->addTransition(this, SIGNAL(ResumeToSoak()), mp_PssmSoak);
        mp_PssmPause->addTransition(this, SIGNAL(ResumeToStepFinished()), mp_PssmStepFinish);
        mp_PssmPause->addTransition(this, SIGNAL(Abort()), mp_PssmAborted);

        mp_PssmPauseDrain->addTransition(this, SIGNAL(ResumeToReadyToTubeAfter()), mp_PssmReadyToTubeAfter);
        mp_PssmPauseDrain->addTransition(this, SIGNAL(Abort()), mp_PssmAborting);

        connect(mp_PssmInit, SIGNAL(entered()), this, SIGNAL(OnInit()));
        connect(mp_PssmReadyToHeatLevelSensorS1, SIGNAL(entered()), this, SIGNAL(OnHeatLevelSensorTempS1()));
        connect(mp_PssmReadyToHeatLevelSensorS2, SIGNAL(entered()), this, SIGNAL(OnHeatLevelSensorTempS2()));
        connect(mp_PssmReadyToTubeBefore, SIGNAL(entered()), this, SIGNAL(OnMoveToTubeBefore()));
        connect(mp_PssmReadyToTubeAfter, SIGNAL(entered()), this, SIGNAL(OnMoveToTubeAfter()));
        connect(mp_PssmReadyToSeal, SIGNAL(entered()), this, SIGNAL(OnMoveToSeal()));
        connect(mp_PssmReadyToFill, SIGNAL(entered()), this, SIGNAL(OnFill()));
        connect(mp_PssmSoak, SIGNAL(entered()), this, SIGNAL(OnSoak()));
        connect(mp_PssmReadyToDrain, SIGNAL(entered()), this, SIGNAL(OnDrain()));
        connect(mp_PssmAborting, SIGNAL(entered()), this, SIGNAL(OnAborting()));
        connect(mp_PssmAborted, SIGNAL(entered()), this, SIGNAL(OnAborted()));
        connect(mp_PssmPause, SIGNAL(entered()), this, SIGNAL(OnPause()));
        connect(mp_PssmPauseDrain, SIGNAL(entered()), this, SIGNAL(OnPauseDrain()));

        connect(mp_PssmPreTest, SIGNAL(exited()), this, SIGNAL(OnStateExited()));

        connect(mp_PssmInit, SIGNAL(exited()), this, SIGNAL(OnStateExited()));
        connect(mp_PssmReadyToHeatLevelSensorS1, SIGNAL(exited()), this, SIGNAL(OnStateExited()));
        connect(mp_PssmReadyToHeatLevelSensorS2, SIGNAL(exited()), this, SIGNAL(OnStateExited()));
        connect(mp_PssmReadyToTubeBefore, SIGNAL(exited()), this, SIGNAL(OnStateExited()));
        connect(mp_PssmReadyToTubeAfter, SIGNAL(exited()), this, SIGNAL(OnStateExited()));
        connect(mp_PssmReadyToSeal, SIGNAL(exited()), this, SIGNAL(OnStateExited()));
        connect(mp_PssmReadyToFill, SIGNAL(exited()), this, SIGNAL(OnStateExited()));
        connect(mp_PssmSoak, SIGNAL(exited()), this, SIGNAL(OnStateExited()));
        connect(mp_PssmReadyToDrain, SIGNAL(exited()), this, SIGNAL(OnStateExited()));
        connect(mp_PssmStepFinish, SIGNAL(exited()), this, SIGNAL(OnStateExited()));
        connect(mp_PssmProgramFinish, SIGNAL(exited()), this, SIGNAL(OnStateExited()));
        connect(mp_PssmError, SIGNAL(exited()), this, SIGNAL(OnStateExited()));
        connect(mp_PssmPause, SIGNAL(exited()), this, SIGNAL(OnStateExited()));
        connect(mp_PssmPauseDrain, SIGNAL(exited()), this, SIGNAL(OnStateExited()));
        connect(mp_PssmAborted, SIGNAL(exited()), this, SIGNAL(OnStateExited()));
        connect(mp_PssmAborting, SIGNAL(exited()), this, SIGNAL(OnStateExited()));

        connect(mp_PssmReadyToFill,SIGNAL(exited()),this, SIGNAL(OnStopFill()));
        connect(mp_PssmReadyToDrain,SIGNAL(exited()),this,SIGNAL(OnStopDrain()));
    }
}

/****************************************************************************/
/*!
 *  \brief    Destructor
 */
/****************************************************************************/
CProgramStepStateMachine::~CProgramStepStateMachine()
{
    delete  mp_PssmInit;
    delete  mp_PssmPreTest;
    delete  mp_PssmReadyToHeatLevelSensorS1;
    delete  mp_PssmReadyToHeatLevelSensorS2;
    delete  mp_PssmReadyToTubeBefore;
    delete  mp_PssmReadyToTubeAfter;
    delete  mp_PssmReadyToSeal;
    delete  mp_PssmReadyToFill;
    delete  mp_PssmReadyToDrain;
    delete  mp_PssmSoak;
    delete  mp_PssmStepFinish;
    delete  mp_PssmProgramFinish;
    delete  mp_PssmError;
    delete  mp_PssmPause;
    delete  mp_PssmPauseDrain;
    delete  mp_PssmAborted;
    delete  mp_PssmAborting;
}

/****************************************************************************/
/*!
 *  \brief   Return the current state of the state machine
 *  \iparam statesList = A list contains the current state, usually get from
 *                       the State Machine current states belogs to
 *
 *  \return The current state of the state machine
 */
/****************************************************************************/
SchedulerStateMachine_t CProgramStepStateMachine::GetCurrentState(QSet<QAbstractState*> statesList)
{
    if(statesList.contains(mp_PssmInit))
    {
        return PSSM_INIT;
    }
    else if(statesList.contains(mp_PssmPreTest))
    {
        return PSSM_PRETEST;
    }
    else if(statesList.contains(mp_PssmReadyToTubeBefore))
    {
        return PSSM_INIT;
    }
    else if(statesList.contains(mp_PssmReadyToTubeAfter))
    {
        return PSSM_DRAINING;
    }
    else if(statesList.contains(mp_PssmReadyToFill))
    {
        return PSSM_FILLING;
    }
    else if(statesList.contains(mp_PssmReadyToSeal))
    {
        return PSSM_RV_MOVE_TO_SEAL;
    }
    else if(statesList.contains(mp_PssmSoak))
    {
        return PSSM_PROCESSING;
    }
    else if(statesList.contains(mp_PssmReadyToDrain))
    {
        return PSSM_DRAINING;
    }
    else if(statesList.contains(mp_PssmStepFinish))
    {
        return PSSM_STEP_FINISH;
    }
    else if(statesList.contains(mp_PssmProgramFinish))
    {
        return PSSM_PROGRAM_FINISH;
    }
    else if(statesList.contains(mp_PssmPause))
    {
        return PSSM_PAUSE;
    }
    else if(statesList.contains(mp_PssmPauseDrain))
    {
        return PSSM_PAUSE_DRAIN;
    }
    else if(statesList.contains(mp_PssmAborting))
    {
        return PSSM_ABORTING;
    }
    else if(statesList.contains(mp_PssmAborted))
    {
        return PSSM_ABORTED;
    }
    else
    {
        return SM_UNDEF;
    }
}


}
