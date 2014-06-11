/****************************************************************************/
/*! \file SchedulerMachine.h
 *
 *  \brief SchedulerStateMachine class definition.
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


#ifndef SCHEDULERMACHINE_H
#define SCHEDULERMACHINE_H

#include <QStateMachine>
#include "DeviceControl/Include/Global/DeviceControlGlobal.h"
#include "ProgramStepStateMachine.h"
#include "RsRvGetOriginalPositionAgain.h"

namespace Scheduler{

class CRsStandby;
class CRsStandbyWithTissue;
class CRcLevelSensorHeatingOvertime;
class CRcRestart;
class CRcReport;
class SchedulerMainThreadController;
class SchedulerCommandProcessorBase;

/****************************************************************************/
/*!
 * \brief Scheduler State Machine Class
 */
/****************************************************************************/
class CSchedulerStateMachine : public QObject
{
    Q_OBJECT
private:
    QStateMachine* mp_SchedulerMachine;			///<  Scheduler state machine 
    QState* mp_InitState;       				///<  Initial state  
    QState* mp_IdleState;						///<  Idle state 
    QState* mp_BusyState;						///<  Busy state
    QState* mp_ErrorState;						///<  Error state 
    QState* mp_ErrorWaitState;                                          ///<  Error State's sub state: error wait state
    QState* mp_ErrorRsStandbyWithTissueState;                           ///<  Error State's sub state: handle RS_STandby_WithTissue related logic
    QState* mp_ErrorRcLevelSensorHeatingOvertimeState;                  ///<  Error State's sub state: handle RC_Levelsensor_Heating_Overtime related logic
    QState* mp_ErrorRcRestart;                                          ///<  Error State's sub state: handle RC_Restart related logic
    CProgramStepStateMachine *mp_ProgramStepStates;						///<  Definition/Declaration of variable mp_ProgramStepStates
    CRsRvGetOriginalPositionAgain *mp_RsRvGetOriginalPositionAgain;		///<  Definition/Declaration of variable mp_RSRvGetOriginalPositionAgain
    CRsStandby *mp_RsStandby;											///<  Definition/Declaration of variable mp_RSStandby
    CRsStandbyWithTissue *mp_RsStandbyWithTissue;						///<  Definition/Declaration of variable mp_RSStandbyWithTissue
    CRcLevelSensorHeatingOvertime *mp_RcLevelSensorHeatingOvertime;     ///<  Definition/Declaration of variable mp_RcLevelSensorHeatingOvertime
    CRcRestart *mp_RcRestart;                                           ///<  Definition/Declaration of variable mp_RcRestart
    CRcReport *mp_RcReport;												///<  Definition/Declaration of variable mp_RcReport
    SchedulerStateMachine_t m_PreviousState;							///<  Definition/Declaration of variable m_PreviousState
    SchedulerStateMachine_t m_CurrentState;								///<  Definition/Declaration of variable m_CurrentState
	SchedulerMainThreadController *mp_SchedulerThreadController;		///<  Definition/Declaration of variable mp_SchedulerThreadController
    SchedulerCommandProcessorBase* mp_SchedulerCommandProcessor;         ///< Pointer of SchedulerMainThreadController's SchedulerCommandProcessor



public:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CSchedulerStateMachine
     *
     *  \param  SchedulerThreadController
     *
     *  \return from void
     */
    /****************************************************************************/
    CSchedulerStateMachine(SchedulerMainThreadController* SchedulerThreadController);
    ~CSchedulerStateMachine();
    /****************************************************************************/
    /*!
     *  \brief  Set Scheduler Command Processor
     *
     *  \param  pSchedCmdProcessor
     *
     *  \return from void
     */
    /****************************************************************************/
     void SetSchedCommandProcessor( Scheduler::SchedulerCommandProcessorBase* pSchedCmdProcessor ) { mp_SchedulerCommandProcessor = pSchedCmdProcessor; }
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function Start
     *
     *  \return from Start
     */
    /****************************************************************************/
    void Start();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function Stop
     *
     *  \return from Stop
     */
    /****************************************************************************/
    void Stop();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SendSchedulerInitComplete
     *
     *  \return from SendSchedulerInitComplete
     */
    /****************************************************************************/
    void SendSchedulerInitComplete();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SendRunSignal
     *
     *  \return from SendRunSignal
     */
    /****************************************************************************/
    void SendRunSignal();
    /****************************************************************************/
    /*!
    *  \brief  Definition/Declaration of function SendRunSignal
    *
    *  \return from SendRunSignal
    */
   /****************************************************************************/
   void SendRunCleaning();
   /****************************************************************************/
   /*!
    *  \brief  Definition/Declaration of function SendRunSelfTest
    *
    *  \return from SendRunSelfTest
    */
   /****************************************************************************/
   void SendRunSelfTest();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SendRunComplete
     *
     *  \return from SendRunComplete
     */
    /****************************************************************************/
    void SendRunComplete();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function SendErrorSignal
     *
     *  \return from SendErrorSignal
     */
    /****************************************************************************/
    void SendErrorSignal();

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function NotifyStInitOK
     *
     *  \return from NotifyStInitOK
     */
    /****************************************************************************/
    void NotifyStInitOK();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function NotifyStTempOK
     *
     *  \return from NotifyStTempOK
     */
    /****************************************************************************/
    void NotifyStTempOK();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function NotifyStCurrentOK
     *
     *  \return from NotifyStCurrentOK
     */
    /****************************************************************************/
    void NotifyStCurrentOK();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function NotifyStVoltageOK
     *
     *  \return from NotifyStVoltageOK
     */
    /****************************************************************************/
    void NotifyStVoltageOK();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function NotifyStRVPositionOK
     *
     *  \return from NotifyStRVPositionOK
     */
    /****************************************************************************/
    void NotifyStRVPositionOK();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function NotifyStPressureOK
     *
     *  \return from NotifyStPressureOK
     */
    /****************************************************************************/
    void NotifyStPressureOK();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function NotifyStSealingOK
     *
     *  \return from NotifyStSealingOK
     */
    /****************************************************************************/
    void NotifyStSealingOK();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function NotifyStGetStationcheckResult
     *
     *  \return from NotifyStGetStationcheckResult
     */
    /****************************************************************************/
    void NotifyStGetStationcheckResult();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function NotifyStStationLeft
     *
     *  \return from NotifyStStationLeft
     */
    /****************************************************************************/
    void NotifyStStationLeft();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function NotifyStStationOK
     *
     *  \return from NotifyStStationOK
     */
    /****************************************************************************/
    void NotifyStStationOK();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function NotifyStDone
     *
     *  \return from NotifyStDone
     */
    /****************************************************************************/
    void NotifyStDone();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function NotifyTempsReady
     *
     *  \return from NotifyTempsReady
     */
    /****************************************************************************/
    void NotifyTempsReady();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function NotifyLevelSensorTempS1Ready
     *
     *  \return from NotifyLevelSensorTempS1Ready
     */
    /****************************************************************************/
    void NotifyLevelSensorTempS1Ready();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function NotifyLevelSensorTempS2Ready
     *
     *  \return from NotifyLevelSensorTempS2Ready
     */
    /****************************************************************************/
    void NotifyLevelSensorTempS2Ready();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function NotifyHitTubeBefore
     *
     *  \return from NotifyHitTubeBefore
     */
    /****************************************************************************/
    void NotifyHitTubeBefore();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function NotifyFillFinished
     *
     *  \return from NotifyFillFinished
     */
    /****************************************************************************/
    void NotifyFillFinished();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function NotifyHitSeal
     *
     *  \return from NotifyHitSeal
     */
    /****************************************************************************/
    void NotifyHitSeal();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function NotifySoakFinished
     *
     *  \return from NotifySoakFinished
     */
    /****************************************************************************/
    void NotifySoakFinished();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function NotifyHitTubeAfter
     *
     *  \return from NotifyHitTubeAfter
     */
    /****************************************************************************/
    void NotifyHitTubeAfter();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function NotifyDrainFinished
     *
     *  \return from NotifyDrainFinished
     */
    /****************************************************************************/
    void NotifyDrainFinished();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function NotifyStepFinished
     *
     *  \return from NotifyStepFinished
     */
    /****************************************************************************/
    void NotifyStepFinished();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function NotifyProgramFinished
     *
     *  \return from NotifyProgramFinished
     */
    /****************************************************************************/
    void NotifyProgramFinished();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function NotifyError
     *
     *  \return from NotifyError
     */
    /****************************************************************************/
    void NotifyError();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function NotifyPause
     *
     *  \param PreviousState = SchedulerStateMachine_t type parameter
     *
     *  \return from NotifyPause
     */
    /****************************************************************************/
    void NotifyPause(SchedulerStateMachine_t PreviousState);

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function NotifyResumeDrain
     *
     *  \return from NotifyResumeDrain
     */
    /****************************************************************************/
    void NotifyResumeDrain();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function NotifyAbort
     *
     *  \return from NotifyAbort
     */
    /****************************************************************************/
    void NotifyAbort();

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function NotifyRsRvMoveToInitPosition
     *
     *  \return from NotifyRsRvMoveToInitPosition
     */
    /****************************************************************************/
    void NotifyRsRvMoveToInitPosition();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function NotifyRsRvMoveToInitPositionFinished
     *
     *  \return from NotifyRsRvMoveToInitPositionFinished
     */
    /****************************************************************************/
    void NotifyRsRvMoveToInitPositionFinished();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function NotifyRcReport
     *
     *  \return from NotifyRcReport
     */
    /****************************************************************************/
    void NotifyRcReport();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function NotifyRsReleasePressure
     *
     *  \return from NotifyRsReleasePressure
     */
    /****************************************************************************/
    void NotifyRsReleasePressure();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function NotifyRsShutdownFailedHeater
     *
     *  \return from NotifyRsShutdownFailedHeater
     */
    /****************************************************************************/
    void NotifyRsShutdownFailedHeater();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function NotifyRsShutdownFailedHeaterFinished
     *
     *  \return from NotifyRsShutdownFailedHeaterFinished
     */
    /****************************************************************************/
    void NotifyRsShutdownFailedHeaterFinished();

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function UpdateCurrentState
     *
     *  \param currentState = SchedulerStateMachine_t type parameter
     *
     *  \return from UpdateCurrentState
     */
    /****************************************************************************/
    void UpdateCurrentState(SchedulerStateMachine_t currentState);

    /****************************************************************************/
    /*!
     *  \brief Enter to Rs_Standby_WithTissue sub state machine 
     *
     *  \return void
     */
    /****************************************************************************/
    void EnterRsStandByWithTissue();

    /****************************************************************************/
    /*!
     *  \brief Enter to Rc_Levelsensor_Heating_Overtime sub state machine
     *
     *  \return void
     */
    /****************************************************************************/
    void EnterRcLevelsensorHeatingOvertime();

    /****************************************************************************/
    /*!
     *  \brief Enter to Rc_Restart sub state machine
     *
     *  \return void
     */
    /****************************************************************************/
    void EnterRcRestart();

    /****************************************************************************/
    /*!
     *  \brief Handle the whole work flow for Rs_Standby_WithTissue 
     *
     *  \param flag - indicate if the execution succeeds or not
     *
     *  \return void
     */
    /****************************************************************************/
    void HandleRsStandByWithTissueWorkFlow(bool flag);

    /****************************************************************************/
    /*!
     *  \brief Handle the whole work flow for Rc_Levelsensor_Heating_Overtime
     *
     *  \param flag - indicate if the execution succeeds or not
     *
     *  \return void
     */
    /****************************************************************************/
    void HandleRcLevelSensorHeatingOvertimeWorkFlow(bool flag);

    /****************************************************************************/
    /*!
     *  \brief Handle the whole work flow for Rc_Restart
     *
     *  \param flag - indicate if the execution succeeds or not
     *
     *  \return void
     */
    /****************************************************************************/
    void HandleRcRestart(bool flag);

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetCurrentState
     *
     *  \return from GetCurrentState
     */
    /****************************************************************************/
    SchedulerStateMachine_t GetCurrentState();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetPreviousState
     *
     *  \return from GetPreviousState
     */
    /****************************************************************************/
    SchedulerStateMachine_t GetPreviousState();

private slots:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot OnstateChanged()
     */
    /****************************************************************************/
    void OnStateChanged();
    /****************************************************************************/
    /*!
     *  \brief	Slot to stop Retort temperature control 
     *  \param	Retort temperature sensor type (RT_SIDE or RT_BOTTOM) 
     *  \return	void
     */
    /****************************************************************************/
    void OnRsRTStopTempCtrl(DeviceControl::RTTempCtrlType_t type);

    /****************************************************************************/
    /*!
     *  \brief	Slot to stop the failed heating element or ASB 
     *  \param	void
     *  \return	void
     */
    /****************************************************************************/
    void OnRsShutdownFailedHeater(); 

    /****************************************************************************/
    /*!
     *  \brief	Slot to release the pressure
     *  \param	void
     *  \return	void
     */
    /****************************************************************************/
    void OnRsReleasePressure(); 

    /****************************************************************************/
    /*!
     *  \brief	Slot to restart Level sensor temperature control in case of
     *          RC_Levelsensor_Heating_Overtime
     *  \param	void
     *  \return	void
     */
    /****************************************************************************/
    void OnRestartLevelSensorTempControl();

    /****************************************************************************/
    /*!
     *  \brief  Slot to react for program resuming
     *
     *  \param  void
     *
     *  \return void
     */
    /****************************************************************************/
    void OnNotifyResume();

    /****************************************************************************/
    /*!
     *  \brief	Slot to handle the actions when all the tasks are done.
     *  \param	bool flag to indicate if the whole execution succeeds or not
     *  \return	void
     */
    /****************************************************************************/
    void OnTasksDone(bool flag);
signals:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal SchedulerInitComplete
     */
    /****************************************************************************/
    void SchedulerInitComplete();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal RunSignal
     */
    /****************************************************************************/
    void RunSignal();
    /****************************************************************************/
    /*!
    *  \brief  Definition/Declaration of signal RunCleaning
    */
   /****************************************************************************/
   void RunCleaning();
   /****************************************************************************/
   /*!
    *  \brief  Definition/Declaration of signal RunSelfTest
    */
   /****************************************************************************/
   void RunSelfTest();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal RunComplete
     */
    /****************************************************************************/
    void RunComplete();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal RecoverToPause
     */
    /****************************************************************************/
    void RecoverToPause();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal ErrorSignal
     */
    /****************************************************************************/
    void ErrorSignal();

    /****************************************************************************/
    /*!
     *  \brief signal to enter RS_StandBy_WithTissue state 
     */
    /****************************************************************************/
    void SigEnterRsStandByWithTissue();

    /****************************************************************************/
    /*!
     *  \brief signal to enter RC_Levelsensor_Heating_Overtime state
     */
    /****************************************************************************/
    void SigEnterRcLevelsensorHeatingOvertime();

    /****************************************************************************/
    /*!
     *  \brief signal to enter RC_Restart
     */
    /****************************************************************************/
    void SigEnterRcRestart();


    /****************************************************************************/
    /*!
     *  \brief signal to enter RC_Levelsensor_Heating_Overtime state
     */
    /****************************************************************************/
    void sigEnterRcLevelsensorHeatingOvertime();

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigStInitOK
     */
    /****************************************************************************/
    void sigStInitOK();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigStTempOK
     */
    /****************************************************************************/
    void sigStTempOK();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigStCurrentOK
     */
    /****************************************************************************/
    void sigStCurrentOK();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigStVoltageOK
     */
    /****************************************************************************/
    void sigStVoltageOK();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigStRVPositionOK
     */
    /****************************************************************************/
    void sigStRVPositionOK();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigStPressureOK
     */
    /****************************************************************************/
    void sigStPressureOK();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigStSealingOK
     */
    /****************************************************************************/
    void sigStSealingOK();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigStGetStationcheckResult
     */
    /****************************************************************************/
    void sigStGetStationcheckResult();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigStStationLeft
     */
    /****************************************************************************/
    void sigStStationLeft();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigStStationOK
     */
    /****************************************************************************/
    void sigStStationOK();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigStDone
     */
    /****************************************************************************/
    void sigStDone();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigTempsReady
     */
    /****************************************************************************/
    void sigTempsReady();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigLevelSensorTempS1Ready
     */
    /****************************************************************************/
    void sigLevelSensorTempS1Ready();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigLevelSensorTempS2Ready
     */
    /****************************************************************************/
    void sigLevelSensorTempS2Ready();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigHitTubeBefore
     */
    /****************************************************************************/
    void sigHitTubeBefore();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigFillFinished
     */
    /****************************************************************************/
    void sigFillFinished();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigHitSeal
     */
    /****************************************************************************/
    void sigHitSeal();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigSoakFinished
     */
    /****************************************************************************/
    void sigSoakFinished();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigHitTubeAfter
     */
    /****************************************************************************/
    void sigHitTubeAfter();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigDrainFinished
     */
    /****************************************************************************/
    void sigDrainFinished();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigStepFinished
     */
    /****************************************************************************/
    void sigStepFinished();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigProgramFinished
     */
    /****************************************************************************/
    void sigProgramFinished();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigError
     */
    /****************************************************************************/
    void sigError();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigPause
     */
    /****************************************************************************/
    void sigPause();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigResumeToSelftest
     */
    /****************************************************************************/
    void sigResumeToSelftest();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigResumeToInit
     */
    /****************************************************************************/
    void sigResumeToInit();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigResumeFromErrorToBegin
     */
    /****************************************************************************/
    void sigResumeFromErrorToBegin();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigResumeToHeatLevelSensorS1
     */
    /****************************************************************************/
    void sigResumeToHeatLevelSensorS1();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigResumeToHeatLevelSensorS2
     */
    /****************************************************************************/
    void sigResumeToHeatLevelSensorS2();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigResumeToReadyToFill
     */
    /****************************************************************************/
    void sigResumeToReadyToFill();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigResumeToSoak
     */
    /****************************************************************************/
    void sigResumeToSoak();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigResumeToStepFinished
     */
    /****************************************************************************/
    void sigResumeToStepFinished();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigResumeToReadyToTubeAfter
     */
    /****************************************************************************/
    void sigResumeToReadyToTubeAfter();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigAbort
     */
    /****************************************************************************/
    void sigAbort();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigRsRvMoveToInitPosition
     */
    /****************************************************************************/
    void sigRsRvMoveToInitPosition();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigRsRvMoveToInitPositionFinished
     */
    /****************************************************************************/
    void sigRsRvMoveToInitPositionFinished();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigRcReport
     */
    /****************************************************************************/
    void sigRcReport();

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigReleasePressure
     */
    /****************************************************************************/
    void sigReleasePressure();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigShutdownFailedHeater
     */
    /****************************************************************************/
    void sigShutdownFailedHeater();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigShutdownFailedHeaterFinished
     */
    /****************************************************************************/
    void sigShutdownFailedHeaterFinished();

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigReleasePressureAtRsStandByWithTissue
     */
    /****************************************************************************/
    void sigReleasePressureAtRsStandByWithTissue();

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigShutdownFailedHeaterAtRsStandByWithTissue
     */
    /****************************************************************************/
    void sigShutdownFailedHeaterAtRsStandByWithTissue();

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigShutdownFailedHeaterFinishedAtRsStandByWithTissue
     */
    /****************************************************************************/
    void sigShutdownFailedHeaterFinishedAtRsStandByWithTissue();

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigRTBottomStopTempCtrlAtRsStandByWithTissue
     */
    /****************************************************************************/
    void sigRTBottomStopTempCtrlAtRsStandByWithTissue();

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigRTTopStopTempCtrlAtRsStandByWithTissue
     */
    /****************************************************************************/
    void sigRTTopStopTempCtrlAtRsStandByWithTissue();

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigOnInit
     */
    /****************************************************************************/
    void sigOnInit();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigOnHeatLevelSensorTempS1
     */
    /****************************************************************************/
    void sigOnHeatLevelSensorTempS1();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigOnHeatLevelSensorTempS2
     */
    /****************************************************************************/
    void sigOnHeatLevelSensorTempS2();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigOnMoveToTubeBefore
     */
    /****************************************************************************/
    void sigOnMoveToTubeBefore();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigOnMoveToTubeAfter
     */
    /****************************************************************************/
    void sigOnMoveToTubeAfter();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigOnMoveToSeal
     */
    /****************************************************************************/
    void sigOnMoveToSeal();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigOnFill
     */
    /****************************************************************************/
    void sigOnFill();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigOnStopFill
     */
    /****************************************************************************/
    void sigOnStopFill();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigOnSoak
     */
    /****************************************************************************/
    void sigOnSoak();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigOnDrain
     */
    /****************************************************************************/
    void sigOnDrain();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigOnStopDrain
     */
    /****************************************************************************/
    void sigOnStopDrain();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigOnAborting
     */
    /****************************************************************************/
    void sigOnAborting();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigOnAborted
     */
    /****************************************************************************/
    void sigOnAborted();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigOnPause
     */
    /****************************************************************************/
    void sigOnPause();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigOnPauseDrain
     */
    /****************************************************************************/
    void sigOnPauseDrain();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigOnRsRvMoveToInitPosition
     */
    /****************************************************************************/
    void sigOnRsRvMoveToInitPosition();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigOnRcReport
     */
    /****************************************************************************/
    void sigOnRcReport();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigOnRsReleasePressure
     */
    /****************************************************************************/
    void sigOnRsReleasePressure();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigOnRsShutdownFailedHeater
     */
    /****************************************************************************/
    void sigOnRsShutdownFailedHeater();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigOnRVPositionCheck
     */
    /****************************************************************************/
    void sigOnRVPositionCheck();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigOnSealingCheck
     */
    /****************************************************************************/
    void sigOnSealingCheck();

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigOnRsReleasePressureAtRsStandByWithTissue
     */
    /****************************************************************************/
    void sigOnRsReleasePressureAtRsStandByWithTissue();

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigOnRsShutdownFailedHeaterAtRsStandByWithTissue
     */
    /****************************************************************************/
    void sigOnRsShutdownFailedHeaterAtRsStandByWithTissue();

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigOnRsRTBottomStopTempCtrlAtRsStandByWithTissue
     */
    /****************************************************************************/
    void sigOnRsRTBottomStopTempCtrlAtRsStandByWithTissue();

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of signal sigOnRsRTTopStopTempCtrlAtRsStandByWithTissue
     */
    /****************************************************************************/
    void sigOnRsRTTopStopTempCtrlAtRsStandByWithTissue();

    void sigStateChange();
};
}

#endif // SCHEDULERMACHINE_H
