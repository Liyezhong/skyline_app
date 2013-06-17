// ================================================================================================
/*! @file Himalaya/HimalayaMain/Master/Components/Scheduler/Source/SchedulerMainThreadController.cpp
 *
 *  @brief
 *  Implementation of class Scheduler::SchedulerMainThreadController.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-03-06
 *  $Author:    $ Abe Yang
 *
 *  @b Company:
 *
 *       Leica Microsystems Ltd. Shanghai.
 *
 *  (C) Copyright 2013 by Leica Microsystems Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 * ================================================================================================
*/
#include "Scheduler/Include/SchedulerMainThreadController.h"
#include "Scheduler/Commands/Include/CmdRVReqMoveToInitialPosition.h"
#include "Scheduler/Commands/Include/CmdALStartTemperatureControlWithPID.h"
#include "Scheduler/Commands/Include/CmdALFilling.h"
#include "Scheduler/Commands/Include/CmdALDraining.h"
#include "Scheduler/Commands/Include/CmdALPressure.h"
#include "Scheduler/Commands/Include/CmdALVaccum.h"
#include "Scheduler/Commands/Include/CmdALAllStop.h"
#include "Scheduler/Commands/Include/CmdRVStartTemperatureControlWithPID.h"
#include "Scheduler/Commands/Include/CmdRTStartTemperatureControlWithPID.h"
#include "Scheduler/Commands/Include/CmdOvenStartTemperatureControlWithPID.h"
#include "Scheduler/Commands/Include/CmdPerTurnOnMainRelay.h"
#include "Scheduler/Commands/Include/CmdPerTurnOffMainRelay.h"
#include "Scheduler/Commands/Include/CmdRVReqMoveToRVPosition.h"
#include "Scheduler/Commands/Include/CmdIDBottleCheck.h"
#include "Scheduler/Include/SchedulerCommandProcessor.h"
#include "HimalayaDataManager/Include/DataManager.h"
#include "Global/Include/GlobalDefines.h"
#include <Scheduler/Commands/Include/CmdRTLock.h>
#include <Scheduler/Commands/Include/CmdRTUnlock.h>
#include <HimalayaDataContainer/Containers/DashboardStations/Commands/Include/CmdCurrentProgramStepInfor.h>
#include "HimalayaDataContainer/Containers/DashboardStations/Commands/Include/CmdProgramAcknowledge.h"
#include "HimalayaDataContainer/Containers/ReagentStations/Commands/Include/CmdUpdateStationReagentStatus.h"
#include "HimalayaDataContainer/Containers/DashboardStations/Commands/Include/CmdKeepCassetteCount.h"
#include "HimalayaDataContainer/Containers/DashboardStations/Commands/Include/CmdProgramSelected.h"
#include "HimalayaDataContainer/Containers/DashboardStations/Commands/Include/CmdProgramSelectedReply.h"
#include "HimalayaDataContainer/Containers/DashboardStations/Commands/Include/CmdStationSuckDrain.h"
#include "HimalayaDataContainer/Containers/DashboardStations/Commands/Include/CmdProgramAcknowledge.h"
#include "float.h"


using namespace DataManager;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//                                                                                 Project Specific
namespace Scheduler {

SchedulerMainThreadController::SchedulerMainThreadController(
                Global::gSourceType TheHeartBeatSource
                )
        : Threads::ThreadController(TheHeartBeatSource, "SchedulerMainThread")
        , m_TickTimer(this)
        , m_SchedulerCommandProcessorThread(NULL)
        , m_SchedulerCommandProcessor(NULL)
        , m_SchedulerMachine(new SchedulerMachine())
        , mp_IDeviceProcessing(NULL)
        , mp_DataManager(NULL)
        , m_CurProgramStepIndex(-1)
        , m_CurProgramID("")
        , m_NewProgramID("")
        , mp_ProgramStepStateMachine(new ProgramStepStateMachine())
        , mp_SelfTestStateMachine(new SelfTestStateMachine())
        , m_CurReagnetName("")
        , m_PauseToBeProcessed(false)
        , m_ProcessCassetteCount(0)
{
    memset(&m_TimeStamps, 0, sizeof(m_TimeStamps));
}

SchedulerMainThreadController::~SchedulerMainThreadController()
{
    delete m_SchedulerCommandProcessorThread;
    m_SchedulerCommandProcessorThread = NULL;
    delete m_SchedulerCommandProcessor;
    m_SchedulerCommandProcessor = NULL;
    delete m_SchedulerMachine;
    m_SchedulerMachine = NULL;
    delete mp_ProgramStepStateMachine;
    mp_ProgramStepStateMachine = NULL;
    delete mp_SelfTestStateMachine;
    mp_SelfTestStateMachine = NULL;
}

void SchedulerMainThreadController::RegisterCommands()
{
    // register commands
    RegisterCommandForProcessing<HimalayaErrorHandler::CmdRaiseAlarm,
                    SchedulerMainThreadController>(&SchedulerMainThreadController::OnRaiseAlarmLocalRemote, this);

    RegisterCommandForProcessing<MsgClasses::CmdProgramAction,
                    SchedulerMainThreadController>(&SchedulerMainThreadController::OnProgramAction, this);

    RegisterCommandForProcessing<MsgClasses::CmdRetortLock,
                    SchedulerMainThreadController>(&SchedulerMainThreadController::OnRetortLock, this);

    RegisterCommandForProcessing<NetCommands::CmdSystemAction,
                    SchedulerMainThreadController>(&SchedulerMainThreadController::OnActionCommandReceived, this);

    RegisterCommandForProcessing<MsgClasses::CmdKeepCassetteCount,
                    SchedulerMainThreadController>(&SchedulerMainThreadController::OnKeepCassetteCount, this);

    RegisterCommandForProcessing<MsgClasses::CmdProgramSelected,
                    SchedulerMainThreadController>(&SchedulerMainThreadController::OnProgramSelected, this);

}

void SchedulerMainThreadController::CreateAndInitializeObjects()
{

    //create the SchedulerCommandProcessor thread
    m_SchedulerCommandProcessorThread = new QThread();
    m_SchedulerCommandProcessor = new SchedulerCommandProcessor(this);
    //CONNECTSIGNALSLOT(&m_SchedulerCommandProcessor, timeout(), m_SchedulerCommandProcessor, run());

    m_SchedulerCommandProcessor->moveToThread(m_SchedulerCommandProcessorThread);
    CONNECTSIGNALSLOT(m_SchedulerCommandProcessorThread, started(), m_SchedulerCommandProcessor, run());
    m_SchedulerCommandProcessorThread->start();

    //timer setting
    CONNECTSIGNALSLOT(&m_TickTimer, timeout(), this, OnTickTimer());
    CONNECTSIGNALSLOT(m_SchedulerCommandProcessor, DCLConfigurationFinished(ReturnCode_t, IDeviceProcessing*),
                      this,OnDCLConfigurationFinished(ReturnCode_t, IDeviceProcessing*))

    m_TickTimer.setInterval(500);

    CONNECTSIGNALSLOT(mp_ProgramStepStateMachine, OnInit(), this, StepStart());
    CONNECTSIGNALSLOT(mp_ProgramStepStateMachine, OnHeatLevelSensorTempS1(), this, HeatLevelSensor());
    CONNECTSIGNALSLOT(mp_ProgramStepStateMachine, OnHeatLevelSensorTempS2(), this, HeatLevelSensor());
    CONNECTSIGNALSLOT(mp_ProgramStepStateMachine, OnMoveToTubeBefore(), this, MoveRV());
    CONNECTSIGNALSLOT(mp_ProgramStepStateMachine, OnMoveToTubeAfter(), this, MoveRV());
    CONNECTSIGNALSLOT(mp_ProgramStepStateMachine, OnMoveToSeal(), this, MoveRV());
    CONNECTSIGNALSLOT(mp_ProgramStepStateMachine, OnFill(), this, Fill());
    CONNECTSIGNALSLOT(mp_ProgramStepStateMachine, OnSoak(), this, Soak());
    CONNECTSIGNALSLOT(mp_ProgramStepStateMachine, OnDrain(), this, Drain());
    CONNECTSIGNALSLOT(mp_ProgramStepStateMachine, OnAborting(), this, Abort());

    CONNECTSIGNALSLOT(mp_ProgramStepStateMachine, OnStopFill(), this, StopFill());
    CONNECTSIGNALSLOT(mp_ProgramStepStateMachine, OnStopDrain(), this, StopDrain());
    CONNECTSIGNALSLOT(mp_ProgramStepStateMachine, OnPause(), this, Pause());
    CONNECTSIGNALSLOT(mp_ProgramStepStateMachine, OnPauseDrain(), this, Pause());


    //command queue reset
    m_SchedulerCmdQueue.clear();
    m_DeviceControlCmdQueue.clear();

    // now register commands
    RegisterCommands();

    //InitializeDevice();

    m_SchedulerMachine->Start();
    //for debug
    qDebug() << "Current state of Scheduler is: " << m_SchedulerMachine->GetCurrentState();
}

void SchedulerMainThreadController::CleanupAndDestroyObjects()
{
}

void SchedulerMainThreadController::OnGoReceived()
{
   // m_TickTimer.start();
}

void SchedulerMainThreadController::OnStopReceived()
{
    m_TickTimer.stop();
    m_SchedulerCommandProcessorThread->quit();
    m_SchedulerCommandProcessorThread->wait();
}

void SchedulerMainThreadController::OnPowerFail()
{
    DEBUGWHEREAMI;

    //! @todo: Implement Power Fail Handling
}

void SchedulerMainThreadController::OnTickTimer()
{
    ControlCommandType_t newControllerCmd = PeekNonDeviceCommand();
    SchedulerCommandShPtr_t cmd;
    ReturnCode_t retCode = DCL_ERR_UNDEFINED;
    if(PopDeviceControlCmdQueue(cmd))
    {
        if(!(cmd->GetResult(retCode)))
        {
            retCode = DCL_ERR_UNDEFINED;
        }
    }
    SchedulerState_t currentState = m_SchedulerMachine->GetCurrentState();
    switch(currentState)
    {
    case INIT_STATE:
        //refuse any main controller request if there is any
        qDebug()<<"Scheduler main controller state: INIT";
        break;
    case IDLE_STATE:
        qDebug()<<"Scheduler main controller state: IDLE";
        HardwareMonitor(mp_IDeviceProcessing, "IDLE");
        HandleIdleState(newControllerCmd);
        break;
    case RUN_STATE:
        qDebug()<<"Scheduler main controller state: RUN";
        HardwareMonitor(mp_IDeviceProcessing, m_CurProgramID);
        HandleRunState(newControllerCmd, retCode);
        break;
    case ERROR_STATE:
        qDebug()<<"Scheduler main controller state: ERROR";
        //refuse any main controller request if there is any
        break;
    default:
        qDebug()<<"Scheduler main controller gets unexpected state: " << currentState;
    }
}

void SchedulerMainThreadController::HandleIdleState(ControlCommandType_t ctrlCmd)
{
    switch (ctrlCmd)
    {
    case CTRL_CMD_START:
        m_UsedStationIDs.clear();
        m_CurProgramID = m_NewProgramID;
        m_NewProgramID = "";
        m_CurProgramStepIndex = -1;
        this->GetNextProgramStepInformation(m_CurProgramID, m_CurProgramStepInfo);
        if(m_CurProgramStepIndex != -1)
        {
            //send command to main controller to tell program is starting
            MsgClasses::CmdProgramAcknowledge* commandPtrFinish(new MsgClasses::CmdProgramAcknowledge(5000,DataManager::PROGRAM_RUN_BEGIN));
            Q_ASSERT(commandPtrFinish);
            Global::tRefType fRef = GetNewCommandRef();
            SendCommand(fRef, Global::CommandShPtr_t(commandPtrFinish));

            qDebug() << "Start step: " << m_CurProgramID;
            mp_ProgramStepStateMachine->Start();
            mp_SelfTestStateMachine->Start();
            m_SchedulerMachine->SendRunSignal();
            PrepareProgramStationList(m_CurProgramID);
            //send command to main controller to tell the left time
            quint32 leftSeconds = GetCurrentProgramStepNeededTime(m_CurProgramID);
            QTime leftTime(0,0,0);
            leftTime = leftTime.addSecs(leftSeconds);
            MsgClasses::CmdCurrentProgramStepInfor* commandPtr(new MsgClasses::CmdCurrentProgramStepInfor(5000, m_CurReagnetName, m_CurProgramStepIndex, leftTime));
            Q_ASSERT(commandPtr);
            Global::tRefType Ref = GetNewCommandRef();
            SendCommand(Ref, Global::CommandShPtr_t(commandPtr));
            DequeueNonDeviceCommand();
        }
        break;
    case CTRL_CMD_LOCK_RETORT:
        break;
    case CTRL_CMD_UNLOCK_RETORT:
        break;
    default:
          DequeueNonDeviceCommand();
    }
}

void SchedulerMainThreadController::UpdateStationReagentStatus()
{
    MsgClasses::CmdUpdateStationReagentStatus* commandPtr = NULL;
    DataManager::CHimalayaUserSettings* pUserSetting = mp_DataManager->GetUserSettings();
    if (m_CurProgramID.at(0) != 'C')//process cleaning reagent
    {
        Global::RMSOptions_t rmsMode = pUserSetting->GetModeRMSCleaning();
        if (rmsMode == DataManager::RMS_CYCLES)
        {
            commandPtr = new MsgClasses::CmdUpdateStationReagentStatus(5000, m_UsedStationIDs, 0);
        }
    }
    else //process reagent
    {
        Global::RMSOptions_t rmsMode = pUserSetting->GetModeRMSProcessing();
        if (rmsMode == DataManager::RMS_CYCLES)
        {
            commandPtr = new MsgClasses::CmdUpdateStationReagentStatus(5000, m_UsedStationIDs, 0);
        }
        else
        {
            commandPtr = new MsgClasses::CmdUpdateStationReagentStatus(5000, m_UsedStationIDs, m_ProcessCassetteCount);//toDo: 100, should get the actual number
        }
    }

    if(commandPtr)
    {
        Global::tRefType Ref = GetNewCommandRef();
        SendCommand(Ref, Global::CommandShPtr_t(commandPtr));
    }
}

void SchedulerMainThreadController::HandleRunState(ControlCommandType_t ctrlCmd, ReturnCode_t retCode)
{

    if(CTRL_CMD_ABORT == ctrlCmd)
    {
        qDebug() << "Scheduler received command: ABORT";
        mp_ProgramStepStateMachine->NotifyAbort();
        DequeueNonDeviceCommand();
        //tell main controller scheduler is starting to abort
        MsgClasses::CmdProgramAcknowledge* commandPtrAbortBegin(new MsgClasses::CmdProgramAcknowledge(5000,DataManager::PROGRAM_ABORT_BEGIN));
        Q_ASSERT(commandPtrAbortBegin);
        Global::tRefType fRef = GetNewCommandRef();
        SendCommand(fRef, Global::CommandShPtr_t(commandPtrAbortBegin));
    }
    else
    {
        ProgramStepStateMachine_t stepState = mp_ProgramStepStateMachine->GetCurrentState();
        qDebug() << "Scheduler step statemachine: "<<stepState;
        if(PSSM_INIT == stepState)
        {
            if(CTRL_CMD_PAUSE == ctrlCmd)
            {
                AllStop();
                mp_ProgramStepStateMachine->NotifyPause(PSSM_INIT);
                DequeueNonDeviceCommand();
            }
            else if(SelfTest(retCode))
            {
                if(CheckStepTemperature())
                {
                    mp_ProgramStepStateMachine->NotifyTempsReady();
                }
            }
        }
        else if(PSSM_READY_TO_HEAT_LEVEL_SENSOR_S1 == stepState)
        {
            //todo: get data here
            if(CTRL_CMD_PAUSE == ctrlCmd)
            {
                mp_ProgramStepStateMachine->NotifyPause(PSSM_READY_TO_HEAT_LEVEL_SENSOR_S1);
                DequeueNonDeviceCommand();
            }
            else if(CheckLevelSensorTemperature(85))
            {
                mp_ProgramStepStateMachine->NotifyLevelSensorTempS1Ready();
            }
        }
        else if(PSSM_READY_TO_HEAT_LEVEL_SENSOR_S2 == stepState)
        {
            //todo: get data here
            if(CTRL_CMD_PAUSE == ctrlCmd)
            {
                mp_ProgramStepStateMachine->NotifyPause(PSSM_READY_TO_HEAT_LEVEL_SENSOR_S2);
                DequeueNonDeviceCommand();
            }
            else if(CheckLevelSensorTemperature(85))
            {
                mp_ProgramStepStateMachine->NotifyLevelSensorTempS2Ready();
            }
        }
        else if(PSSM_READY_TO_TUBE_BEFORE == stepState)
        {
            // get current step tube position here
            RVPosition_t targetPos = GetRVTubePositionByStationID(m_CurProgramStepInfo.stationID);
            if(m_PositionRV == targetPos)
            {
                if((CTRL_CMD_PAUSE == ctrlCmd)||(m_PauseToBeProcessed))
                {
                    mp_ProgramStepStateMachine->NotifyPause(PSSM_READY_TO_TUBE_BEFORE);
                    m_PauseToBeProcessed = false;
                    DequeueNonDeviceCommand();
                }
                else
                {
                    mp_ProgramStepStateMachine->NotifyHitTubeBefore();
                }
            }
            else
            {
                if(CTRL_CMD_PAUSE == ctrlCmd)
                {
                    m_PauseToBeProcessed = true;
                }
            }
        }
        else if(PSSM_READY_TO_FILL == stepState)
        {
            if(CTRL_CMD_PAUSE == ctrlCmd)
            {
                qDebug() << "Scheduler step: READY_TO_FILL is abort to PAUSE";
                AllStop();
                mp_ProgramStepStateMachine->NotifyPause(PSSM_READY_TO_FILL);
                DequeueNonDeviceCommand();
            }
            else if(DCL_ERR_DEV_AL_FILL_SUCCESS == retCode)
            {
                qDebug() << "Scheduler step: READY_TO_FILL received FILL_SUCCESS, go to next state now.";
                mp_ProgramStepStateMachine->NotifyFillFinished();
            }
            else if(retCode != DCL_ERR_UNDEFINED)
            {
                //todo: error handling here
            }
        }
        else if(PSSM_READY_TO_SEAL == stepState)
        {
            RVPosition_t targetPos = GetRVSealPositionByStationID(m_CurProgramStepInfo.stationID);
            if(m_PositionRV == targetPos)
            {
                if((CTRL_CMD_PAUSE == ctrlCmd)||(m_PauseToBeProcessed))
                {
                    mp_ProgramStepStateMachine->NotifyPause(PSSM_READY_TO_SEAL);
                    m_PauseToBeProcessed = false;
                    DequeueNonDeviceCommand();
                }
                else
                {
                    mp_ProgramStepStateMachine->NotifyHitSeal();
                }
            }
            else
            {
                if(CTRL_CMD_PAUSE == ctrlCmd)
                {
                    m_PauseToBeProcessed = true;
                }
            }
        }
        else if(PSSM_SOAK == stepState)
        {
            static qint64 lastPVTime = 0;
            static bool completionNotifierSent = false;
            if(CTRL_CMD_PAUSE == ctrlCmd)
            {
                if(m_CurProgramStepInfo.isPressure || m_CurProgramStepInfo.isVacuum)
                {
                    AllStop();
                    lastPVTime = 0;
                }
                mp_ProgramStepStateMachine->NotifyPause(PSSM_SOAK);
                DequeueNonDeviceCommand();
            }
            else
            {
                qint64 now = QDateTime::currentDateTime().toMSecsSinceEpoch();
                //todo: 1/10 the time
                qint32 period = m_CurProgramStepInfo.durationInSeconds * 1000;
                //qint32 period = m_CurProgramStepInfo.durationInSeconds * 20;
                if((now - m_TimeStamps.CurStepSoakStartTime ) > (period))
                {
                    if(IsLastStep(m_CurProgramStepIndex, m_CurProgramID))
                    {
                         //this is last step, need to notice user
                         if(!completionNotifierSent)
                         {
                             MsgClasses::CmdProgramAcknowledge* commandPtrFinish(new MsgClasses::CmdProgramAcknowledge(5000,DataManager::PROGRAM_WILL_COMPLETE));
                             Q_ASSERT(commandPtrFinish);
                             Global::tRefType fRef = GetNewCommandRef();
                             SendCommand(fRef, Global::CommandShPtr_t(commandPtrFinish));
                             completionNotifierSent = true;
                         }
                         if(CTRL_CMD_DRAIN == ctrlCmd)
                         {
                             mp_ProgramStepStateMachine->NotifySoakFinished();
                             m_TimeStamps.CurStepSoakStartTime = 0;
                             completionNotifierSent = false;
                         }
                    }
                    else
                    {
                        mp_ProgramStepStateMachine->NotifySoakFinished();
                        m_TimeStamps.CurStepSoakStartTime = 0;
                    }
                }
                else
                {
                    if(m_CurProgramStepInfo.isPressure && m_CurProgramStepInfo.isVacuum)
                    {
                        // P/V take turns in 1 minute
                        if((now - lastPVTime)>60000)
                        {
                            if(((now - m_TimeStamps.CurStepSoakStartTime)/60000)%2 == 0)
                            {
                                Pressure();
                            }
                            else
                            {
                                Vaccum();
                            }
                            lastPVTime = now;
                        }
                    }
                }
            }
        }
        else if(PSSM_READY_TO_TUBE_AFTER == stepState)
        {

            // get current step tube position here
            RVPosition_t targetPos = GetRVTubePositionByStationID(m_CurProgramStepInfo.stationID);
            if(m_PositionRV == targetPos)
            {
                if((CTRL_CMD_PAUSE == ctrlCmd)||(m_PauseToBeProcessed))
                {
                    RVPosition_t sealPos = GetRVSealPositionByStationID(m_CurProgramStepInfo.stationID);
                    if(RV_UNDEF != sealPos)
                    {
                        CmdRVReqMoveToRVPosition* cmd = new CmdRVReqMoveToRVPosition(500, mp_IDeviceProcessing, this);
                        cmd->SetRVPosition(sealPos);
                        m_SchedulerCommandProcessor->pushCmd(cmd);
                    }
                    else
                    {
                        //todo: error handling
                        qDebug()<<"Get invalid RV position: " << m_CurProgramStepInfo.stationID;
                    }
                }
                else
                {
                    mp_ProgramStepStateMachine->NotifyHitTubeAfter();
                }
            }
            else
            {
                if(CTRL_CMD_PAUSE == ctrlCmd)
                {
                    m_PauseToBeProcessed = true;
                }
                if(m_PauseToBeProcessed)
                {
                    RVPosition_t sealPos = GetRVSealPositionByStationID(m_CurProgramStepInfo.stationID);
                    if(m_PositionRV == sealPos)
                    {
                        mp_ProgramStepStateMachine->NotifyPause(PSSM_READY_TO_TUBE_AFTER);
                        m_PauseToBeProcessed = false;
                        DequeueNonDeviceCommand();
                    }
                }
            }
        }
        else if(PSSM_READY_TO_DRAIN == stepState)
        {
            if((CTRL_CMD_PAUSE == ctrlCmd)||(m_PauseToBeProcessed))
            {
                if(m_PauseToBeProcessed)
                {
                    RVPosition_t sealPos = GetRVSealPositionByStationID(m_CurProgramStepInfo.stationID);
                    if(m_PositionRV == sealPos)
                    {
                        //release pressure
                        AllStop();
                        m_PauseToBeProcessed = false;
                        mp_ProgramStepStateMachine->NotifyPause(PSSM_READY_TO_DRAIN);
                        DequeueNonDeviceCommand();
                    }
                }
                else
                {
                    m_PauseToBeProcessed = true;
                    RVPosition_t sealPos = GetRVSealPositionByStationID(m_CurProgramStepInfo.stationID);
                    if(RV_UNDEF != sealPos)
                    {
                        CmdRVReqMoveToRVPosition* cmd = new CmdRVReqMoveToRVPosition(500, mp_IDeviceProcessing, this);
                        cmd->SetRVPosition(sealPos);
                        m_SchedulerCommandProcessor->pushCmd(cmd);
                    }
                    else
                    {
                        //todo: error handling
                        qDebug()<<"Get invalid RV position: " << m_CurProgramStepInfo.stationID;
                    }
                }
            }
            else if(DCL_ERR_DEV_AL_DRAIN_SUCCESS == retCode)
            {
                mp_ProgramStepStateMachine->NotifyDrainFinished();
            }
            else if(DCL_ERR_UNDEFINED != retCode)
            {
                //todo: error handling here
            }
        }
        else if(PSSM_FINISH == stepState)
        {
            //todo: start next program step or finish all program
            m_UsedStationIDs.append(m_CurProgramStepInfo.stationID);
            qDebug()<< "step finished!";
            this->GetNextProgramStepInformation(m_CurProgramID, m_CurProgramStepInfo);
            if(m_CurProgramStepIndex != -1)
            {
                //start next step
                qDebug() << "Start step: " << m_CurProgramID;
                mp_ProgramStepStateMachine->Start();
                //send command to main controller to tell the left time
                quint32 leftSeconds = GetCurrentProgramStepNeededTime(m_CurProgramID);
                QTime leftTime(0,0,0);
                leftTime = leftTime.addSecs(leftSeconds);
                MsgClasses::CmdCurrentProgramStepInfor* commandPtr(new MsgClasses::CmdCurrentProgramStepInfor(5000, m_CurReagnetName, m_CurProgramStepIndex, leftTime));
                Q_ASSERT(commandPtr);
                Global::tRefType Ref = GetNewCommandRef();
                SendCommand(Ref, Global::CommandShPtr_t(commandPtr));
            }
            else
            {
                //program finished
                qDebug() << "Program finished!";
                m_SchedulerMachine->SendRunComplete();
                mp_ProgramStepStateMachine->Stop();
                //todo: tell main controller that program is complete
                UpdateStationReagentStatus();

                //send command to main controller to tell the left time
                QTime leftTime(0,0,0);
                MsgClasses::CmdCurrentProgramStepInfor* commandPtr(new MsgClasses::CmdCurrentProgramStepInfor(5000, "", m_CurProgramStepIndex, leftTime));
                Q_ASSERT(commandPtr);
                Global::tRefType Ref = GetNewCommandRef();
                SendCommand(Ref, Global::CommandShPtr_t(commandPtr));

                //send command to main controller to tell program finished
                MsgClasses::CmdProgramAcknowledge* commandPtrFinish(new MsgClasses::CmdProgramAcknowledge(5000,DataManager::PROGRAM_RUN_FINISHED));
                Q_ASSERT(commandPtrFinish);
                Ref = GetNewCommandRef();
                SendCommand(Ref, Global::CommandShPtr_t(commandPtrFinish));
            }
        }
        else if(PSSM_PAUSE == stepState)
        {
            if(CTRL_CMD_START == ctrlCmd)
            {
                // resume the program
                mp_ProgramStepStateMachine->NotifyResume();
                DequeueNonDeviceCommand();
                // tell the main controller the program is resuming
                MsgClasses::CmdProgramAcknowledge* commandPtrFinish(new MsgClasses::CmdProgramAcknowledge(5000,DataManager::PROGRAM_RUN_BEGIN));
                Q_ASSERT(commandPtrFinish);
                Global::tRefType fRef = GetNewCommandRef();
                SendCommand(fRef, Global::CommandShPtr_t(commandPtrFinish));
            }
        }
        else if(PSSM_PAUSE_DRAIN == stepState)
        {
            if(CTRL_CMD_START == ctrlCmd)
            {
                mp_ProgramStepStateMachine->NotifyResumeDrain();
                DequeueNonDeviceCommand();
                // tell the main controller the program is resuming
                MsgClasses::CmdProgramAcknowledge* commandPtrFinish(new MsgClasses::CmdProgramAcknowledge(5000,DataManager::PROGRAM_RUN_BEGIN));
                Q_ASSERT(commandPtrFinish);
                Global::tRefType fRef = GetNewCommandRef();
                SendCommand(fRef, Global::CommandShPtr_t(commandPtrFinish));
            }
        }
        else if(PSSM_ABORTING == stepState)
        {
            if(DCL_ERR_DEV_RV_REF_MOVE_OK == retCode)
            {
                this->Drain();
            }
            else if(DCL_ERR_DEV_AL_DRAIN_SUCCESS == retCode)
            {
                mp_ProgramStepStateMachine->NotifyAbort();
            }
#if 0
          qDebug()<<"unexpected ret code: "<<retCode;
#endif
        }
        else if(PSSM_ABORTED == stepState)
        {
            //program finished
            AllStop();
            qDebug() << "Program aborted!";
            m_SchedulerMachine->SendRunComplete();
            mp_ProgramStepStateMachine->Stop();
            mp_SelfTestStateMachine->Stop();
            // tell the main controller the program has been aborted
            MsgClasses::CmdProgramAcknowledge* commandPtrAbortFinish(new MsgClasses::CmdProgramAcknowledge(5000,DataManager::PROGRAM_ABORT_FINISHED));
            Q_ASSERT(commandPtrAbortFinish);
            Global::tRefType fRef = GetNewCommandRef();
            SendCommand(fRef, Global::CommandShPtr_t(commandPtrAbortFinish));

            UpdateStationReagentStatus();

            //send command to main controller to tell the left time
            QTime leftTime(0,0,0);
            MsgClasses::CmdCurrentProgramStepInfor* commandPtr(new MsgClasses::CmdCurrentProgramStepInfor(5000, "", m_CurProgramStepIndex, leftTime));
            Q_ASSERT(commandPtr);
            Global::tRefType Ref = GetNewCommandRef();
            SendCommand(Ref, Global::CommandShPtr_t(commandPtr));
        }
    }
#if 0
    //work around to avoid continus START cmd
    if(CTRL_CMD_START == ctrlCmd)
    {
        if((PSSM_PAUSE != stepState)&&(PSSM_PAUSE_DRAIN != stepState))
        {
            DequeueNonDeviceCommand();
        }
    }
#endif
}

ControlCommandType_t SchedulerMainThreadController::PeekNonDeviceCommand()
{

    if(m_SchedulerCmdQueue.isEmpty())
        return CTRL_CMD_NONE;

    Global::CommandShPtr_t pt = m_SchedulerCmdQueue.head();
    MsgClasses::CmdProgramAction* pCmdProgramAction = dynamic_cast<MsgClasses::CmdProgramAction*>(pt.GetPointerToUserData());
    if(pCmdProgramAction)
    {
        if (pCmdProgramAction->ProgramActionType() == DataManager::PROGRAM_START)
        {
            m_NewProgramID = pCmdProgramAction->GetProgramID();
            return CTRL_CMD_START;
        }
        if (pCmdProgramAction->ProgramActionType() == DataManager::PROGRAM_PAUSE)
        {
            return CTRL_CMD_PAUSE;
        }
        if (pCmdProgramAction->ProgramActionType() == DataManager::PROGRAM_ABORT)
        {
            return CTRL_CMD_ABORT;
        }
        if (pCmdProgramAction->ProgramActionType() == DataManager::PROGRAM_DRAIN)
        {
            return CTRL_CMD_DRAIN;
        }
    }

    MsgClasses::CmdRetortLock* pCmdRetortLock = dynamic_cast<MsgClasses::CmdRetortLock*>(pt.GetPointerToUserData());
    if (pCmdRetortLock)
    {
        if(pCmdRetortLock->IsLock())
        {
            return CTRL_CMD_LOCK_RETORT;
        }
        else
        {
            return CTRL_CMD_UNLOCK_RETORT;
        }
    }

    HimalayaErrorHandler::CmdRaiseAlarm* pCmdRaiseAlarm = dynamic_cast<HimalayaErrorHandler::CmdRaiseAlarm*>(pt.GetPointerToUserData());
    if (pCmdRaiseAlarm)
    {
        if(pCmdRaiseAlarm->m_localAlarm)
        {

        }
        else
        {

        }
        //todo temp. return set remote alarm, update later
        return CTRL_CMD_SET_REMOTE_ALARM;
    }
    return CTRL_CMD_UNKNOWN;

}//When end of this function, the command in m_SchedulerCmdQueue will be destrory by share pointer (CommandShPtr_t) mechanism

void SchedulerMainThreadController::DequeueNonDeviceCommand()
{
        if(!m_SchedulerCmdQueue.isEmpty())
        {
            m_SchedulerCmdQueue.dequeue();
        }
}

bool SchedulerMainThreadController::GetNextProgramStepInformation(const QString& ProgramID,
                                                                  ProgramStepInfor& programStepInfor)
{
    if (!mp_DataManager)
    {
        Q_ASSERT(false);
        return false;//need error handling
    }

    CDataProgramList* pDataProgramList = mp_DataManager->GetProgramList();
    if (!pDataProgramList)
    {
        Q_ASSERT(false);
        return false;
    }

    CProgram* pProgram = const_cast<CProgram*>(pDataProgramList->GetProgram(ProgramID));
    ListOfIDs_t* stepIDs = pProgram->OrderedListOfStepIDs();

    int stepSize = stepIDs->count();
    bool isLastStep = false;

    int nextProgramStepIndex(-1);
    if (-1 == m_CurProgramStepIndex)
    {
        nextProgramStepIndex = 0;
    }
    else
    {
        int nextOne = m_CurProgramStepIndex ;
        ++nextOne;
        nextProgramStepIndex = nextOne;
        if (stepSize == nextOne + 1)
            isLastStep = true;
    }

    const CProgramStep* pProgramStep = pProgram->GetProgramStep(nextProgramStepIndex);
    if (pProgramStep)
    {
        QString reagentID = pProgramStep->GetReagentID();
        programStepInfor.stationID  = this->GetStationIDFromProgramStep(nextProgramStepIndex);
        programStepInfor.durationInSeconds = pProgramStep->GetDurationInSeconds();
        programStepInfor.temperature = pProgramStep->GetTemperature().toInt();
        programStepInfor.isPressure = (pProgramStep->GetPressure() == "On");
        programStepInfor.isVacuum = (pProgramStep->GetVacuum() == "On");
        m_CurProgramStepIndex = nextProgramStepIndex;
        m_CurReagnetName = GetReagentName(reagentID);
    }
    else
    {
        m_CurProgramStepIndex = -1;
    }
    return true;
}

QString SchedulerMainThreadController::GetStationIDFromProgramStep(int ProgramStepIndex)
{
    return m_StationList.at(ProgramStepIndex);
}

QString SchedulerMainThreadController::SelectStationFromReagentID(const QString& ReagentID, ListOfIDs_t& unusedStationIDs,
                                                                 QList<StationUseRecord_t>& usedStations, bool IsLastStep)
{
    if (!mp_DataManager)
        return "";

    DataManager::CHimalayaUserSettings* pUserSetting = mp_DataManager->GetUserSettings();
    Global::RMSOptions_t rmsMode = Global::RMS_UNDEFINED;
    if (this->IsCleaningReagent(ReagentID))
    {
        rmsMode = pUserSetting->GetModeRMSCleaning();
    }
    else
    {
        rmsMode = pUserSetting->GetModeRMSProcessing();
    }

    if (CDataReagentList* pReagentList =  mp_DataManager->GetReagentList())
    {
        const CReagent* pReagent = pReagentList->GetReagent(ReagentID);
        return this->SelectStationByReagent(pReagent,  unusedStationIDs,
                                          usedStations, IsLastStep, rmsMode);
    }

    return "";
}


const QString& SchedulerMainThreadController::SelectStationByReagent(const CReagent* pReagent,
                                                                    ListOfIDs_t& unusedStationIDs,
                                                                    QList<StationUseRecord_t>& usedStations,
                                                                    bool bFindNewestOne,
                                                                    Global::RMSOptions_t rmsMode) const
{
    Q_UNUSED(bFindNewestOne);
    Q_UNUSED(usedStations);
    Q_UNUSED(rmsMode);

    if (!pReagent)
        return "";

    CDashboardDataStationList* pDashboardDataStationList = mp_DataManager->GetStationList();
    if (!pDashboardDataStationList)
        return "";

    CDataReagentGroupList* pDataReagentGroupList = mp_DataManager->GetReagentGroupList();
    if (!pDataReagentGroupList)
        return "";

    const QString& ReagentID = pReagent->GetReagentID();
    bool isCleaningReagent = pDataReagentGroupList->GetReagentGroup(pReagent->GetGroupID())->IsCleaningReagentGroup();

    for (int i = 0; i < unusedStationIDs.count(); i++)
    {
        CDashboardStation* pDashboardStation = pDashboardDataStationList->GetDashboardStation(unusedStationIDs.at(i));
        if (pDashboardStation->GetDashboardReagentID() == ReagentID)
        {
                if (!isCleaningReagent)
                    unusedStationIDs.removeAt(i);
                return pDashboardStation->GetDashboardStationID();
        }
    }
    return "";
}

bool SchedulerMainThreadController::PrepareProgramStationList(const QString& ProgramID)
{
    if (!mp_DataManager)
    {
        Q_ASSERT(false);
        return false;//need error handling
    }

    CDataProgramList* pDataProgramList = mp_DataManager->GetProgramList();
    if (!pDataProgramList)
    {
        Q_ASSERT(false);
        return false;
    }

    CDashboardDataStationList* pDashboardDataStationList = mp_DataManager->GetStationList();
    if (!pDashboardDataStationList)
    {
        Q_ASSERT(false);
        return false;
    }

    CProgram* pProgram = const_cast<CProgram*>(pDataProgramList->GetProgram(ProgramID));

    ListOfIDs_t* stepIDs = pProgram->OrderedListOfStepIDs();
    bool isLastStep = false;

    m_ProgramStationList.clear();
    m_StationList.clear();

    ListOfIDs_t unusedStationIDs = pDashboardDataStationList->GetOrderedListOfDashboardStationIDs();
    QList<StationUseRecord_t> usedStations;

    for (int i = 0; i < stepIDs->count(); i++)
    {
        const CProgramStep* pProgramStep = pProgram->GetProgramStep(i);
        ProgramStationInfo_t stationInfo;
        isLastStep = (i == (stepIDs->count() - 1));
        QString reagentID = pProgramStep->GetReagentID();
        stationInfo.ReagentGroupID =GetReagentGroupID(reagentID);
        stationInfo.StationID = this->SelectStationFromReagentID(reagentID, unusedStationIDs, usedStations, isLastStep);
        m_ProgramStationList.enqueue(stationInfo);
        m_StationList.push_back(stationInfo.StationID);
    }

    return true;
}

quint32 SchedulerMainThreadController::GetLeftProgramStepsNeededTime(const QString& ProgramID, int SpecifiedStepIndex)
{
    quint32 leftTime = 0;
    if (!mp_DataManager)
    {
        Q_ASSERT(false);
        return 0;//error handling
    }

    CDataProgramList* pDataProgramList = mp_DataManager->GetProgramList();
    if (!pDataProgramList)
    {
        Q_ASSERT(false);
        return 0;
    }

    CProgram* pProgram = const_cast<CProgram*>(pDataProgramList->GetProgram(ProgramID));
    ListOfIDs_t* stepIDs = pProgram->OrderedListOfStepIDs();
    int index = SpecifiedStepIndex;
    if (-1 == index)
    {
        int programStepIDIndex(-1);
        if (-1 == m_CurProgramStepIndex)
        {
            programStepIDIndex = 0;
        }
        else
        {
            programStepIDIndex = m_CurProgramStepIndex ;
        }
        index = programStepIDIndex;
    }

    for (int i = index; i < stepIDs->count(); i++)
    {
        const CProgramStep* pProgramStep = pProgram->GetProgramStep(i);
        quint32 soakTime = pProgramStep->GetDurationInSeconds();
        leftTime += soakTime;
        bool isPressure = (pProgramStep->GetPressure() == "On");
        bool isVaccum = (pProgramStep->GetVacuum() == "On");
        if (isPressure ^ isVaccum)
        {
            leftTime += 30;//suppose pressure or vaccum need 30 seconds to build up
        }
        else if(isPressure && isVaccum)
        {
            leftTime += 30 * (soakTime / 60); //suppose P/V take turns in 1 min, and each takes 30 seconds to build up
        }
        leftTime += 5 +  2 * 2; //suppose RV need 5 seconds to move to the target station, and movment between tube and its seal need 2 seconds.
        leftTime += 60; //suppose need 60 seconds to fill
        leftTime += 40; //suppose need 40 seconds to drain
        leftTime += 20; //suppose need 20 seconds to heat level sensor
    }
    return leftTime;
}

quint32 SchedulerMainThreadController::GetCurrentProgramStepNeededTime(const QString& ProgramID)
{
    quint32 leftTime = 0;
    if (!mp_DataManager)
    {
        Q_ASSERT(false);
        return 0;//error handling
    }

    CDataProgramList* pDataProgramList = mp_DataManager->GetProgramList();
    if (!pDataProgramList)
    {
        Q_ASSERT(false);
        return 0;
    }

    CProgram* pProgram = const_cast<CProgram*>(pDataProgramList->GetProgram(ProgramID));
    ListOfIDs_t* stepIDs = pProgram->OrderedListOfStepIDs();

    int programStepIDIndex(-1);
    if (-1 == m_CurProgramStepIndex)
    {
        programStepIDIndex = 0;
    }
    else
    {
        programStepIDIndex = m_CurProgramStepIndex ;
    }

    const CProgramStep* pProgramStep = pProgram->GetProgramStep(programStepIDIndex);
    if (pProgramStep)
    {
        quint32 soakTime = pProgramStep->GetDurationInSeconds();
        leftTime += soakTime;
        bool isPressure = (pProgramStep->GetPressure() == "On");
        bool isVaccum = (pProgramStep->GetVacuum() == "On");
        if (isPressure ^ isVaccum)
        {
            leftTime += 30;//suppose pressure or vaccum need 30 seconds to build up
        }
        else if(isPressure && isVaccum)
        {
            leftTime += 30 * (soakTime / 60); //suppose P/V take turns in 1 min, and each takes 30 seconds to build up
        }
        leftTime += 5 +  2 * 2; //suppose RV need 5 seconds to move to the target station, and movment between tube and its seal need 2 seconds.
        leftTime += 60; //suppose need 60 seconds to fill
        leftTime += 40; //suppose need 40 seconds to drain
        leftTime += 20; //suppose need 20 seconds to heat level sensor
    }
    return leftTime;
}

void SchedulerMainThreadController::OnRaiseAlarmLocalRemote(Global::tRefType Ref,
                                                            const HimalayaErrorHandler::CmdRaiseAlarm &Cmd)
{
    Q_UNUSED(Ref);
    m_Mutex.lock();
    m_SchedulerCmdQueue.enqueue(Global::CommandShPtr_t(new HimalayaErrorHandler::CmdRaiseAlarm(Cmd.m_localAlarm)));
    m_Mutex.unlock();
    this->SendAcknowledgeOK(Ref);
}

//client-->master
void SchedulerMainThreadController::OnProgramAction(Global::tRefType Ref,
                                                    const MsgClasses::CmdProgramAction &Cmd)
{
    m_Mutex.lock();
    m_SchedulerCmdQueue.enqueue(Global::CommandShPtr_t(new MsgClasses::CmdProgramAction(Cmd.GetTimeout(), Cmd.GetProgramID(), Cmd.ProgramActionType(),
                                                                                        Cmd.ProgramEndDateTime())));
    m_Mutex.unlock();
    this->SendAcknowledgeOK(Ref);
}

//client-->master
void SchedulerMainThreadController::OnRetortLock(Global::tRefType Ref, const MsgClasses::CmdRetortLock &Cmd)
{
    m_Mutex.lock();
    m_SchedulerCmdQueue.enqueue(Global::CommandShPtr_t(new MsgClasses::CmdRetortLock(Cmd.GetTimeout(), Cmd.IsLock())));
    m_Mutex.unlock();
    this->SendAcknowledgeOK(Ref);
}

//response or recovery
void SchedulerMainThreadController::OnActionCommandReceived(Global::tRefType Ref, const NetCommands::CmdSystemAction & Cmd)
{
    m_Mutex.lock();
    NetCommands::CmdSystemAction *p_CmdSystemAction = new NetCommands::CmdSystemAction();
    p_CmdSystemAction->SetAction(Cmd.GetAction());
    m_SchedulerCmdQueue.enqueue(Global::CommandShPtr_t(p_CmdSystemAction));
    m_Mutex.unlock();
}

void SchedulerMainThreadController::OnKeepCassetteCount(Global::tRefType Ref, const MsgClasses::CmdKeepCassetteCount & Cmd)
{
    m_ProcessCassetteCount = Cmd.CassetteCount();
    this->SendAcknowledgeOK(Ref);
}

void SchedulerMainThreadController::OnProgramSelected(Global::tRefType Ref, const MsgClasses::CmdProgramSelected & Cmd)
{
    this->SendAcknowledgeOK(Ref);

    this->PrepareProgramStationList(Cmd.GetProgramID());

    unsigned int timeProposed = GetLeftProgramStepsNeededTime(Cmd.GetProgramID(), 0);
    unsigned int paraffinMeltCostedtime = this->GetOvenHeatingTime();
    unsigned int costedTimeBeforeParaffin = 0;
    if (-1 != Cmd.ParaffinStepIndex())//has Paraffin
    {
        costedTimeBeforeParaffin = timeProposed - GetLeftProgramStepsNeededTime(Cmd.GetProgramID(), Cmd.ParaffinStepIndex());
    }

    //send back the proposed program end time
    MsgClasses::CmdProgramSelectedReply* commandPtr(new MsgClasses::CmdProgramSelectedReply(5000, timeProposed,
                                                                                paraffinMeltCostedtime,
                                                                                costedTimeBeforeParaffin,
                                                                                m_StationList));
    Q_ASSERT(commandPtr);
    SendCommand(Ref, Global::CommandShPtr_t(commandPtr));

}


bool SchedulerMainThreadController::IsCleaningReagent(const QString& ReagentID)
{
    if (!mp_DataManager)
    {
        return false;
        Q_ASSERT(0);
    }

    if (CDataReagentList* pReagentList =  mp_DataManager->GetReagentList())
    {
        const CReagent* pReagent = pReagentList->GetReagent(ReagentID);
        if (CReagentGroup*  reagentGroup = mp_DataManager->GetReagentGroupList()->GetReagentGroup(pReagent->GetGroupID()))
        {
            return reagentGroup->IsCleaningReagentGroup();
        }
    }
    return false;
}

QString SchedulerMainThreadController::GetReagentName(const QString& ReagentID)
{
    if (!mp_DataManager)
        return "";

    if (CDataReagentList* pReagentList =  mp_DataManager->GetReagentList())
    {
        const CReagent* pReagent = pReagentList->GetReagent(ReagentID);
        if (pReagent)
            return pReagent->GetReagentName();
    }

    return "";
}

QString SchedulerMainThreadController::GetReagentGroupID(const QString& ReagentID)
{
    if (!mp_DataManager)
        return "";

    if (CDataReagentList* pReagentList =  mp_DataManager->GetReagentList())
    {
        const CReagent* pReagent = pReagentList->GetReagent(ReagentID);
        if (pReagent)
            return pReagent->GetGroupID();
    }

    return "";
}

void SchedulerMainThreadController::OnDCLConfigurationFinished(ReturnCode_t RetCode, IDeviceProcessing* pIDP)
{
    if(RetCode == DCL_ERR_FCT_CALL_SUCCESS)
    {
        mp_IDeviceProcessing = pIDP;
        m_SchedulerCommandProcessor->pushCmd(new CmdRVReqMoveToInitialPosition(500, mp_IDeviceProcessing, this));
        SchedulerCommandShPtr_t resRVInitPos;
        while(!PopDeviceControlCmdQueue(resRVInitPos));
        ReturnCode_t retCode;
        resRVInitPos->GetResult(retCode);
        if(DCL_ERR_DEV_RV_MOVE_TO_INIT_POS_SUCCESS != retCode)
        {
            //todo: error handling
            qDebug()<<"Failed move to initial position, return code: " << retCode;
            goto ERROR;
        }
#if 1
    //hardware not ready yet
        m_SchedulerCommandProcessor->pushCmd(new CmdPerTurnOnMainRelay(500, mp_IDeviceProcessing, this));
        SchedulerCommandShPtr_t resPerTurnOnRelay;
        while(!PopDeviceControlCmdQueue(resPerTurnOnRelay));
        resPerTurnOnRelay->GetResult(retCode);
        if(DCL_ERR_FCT_CALL_SUCCESS != retCode)
        {
            //todo: error handling
            qDebug()<<"Failed turn on main relay, return code: " << retCode;
            goto ERROR;
        }
        CmdRTStartTemperatureControlWithPID* cmdHeatRTSide = new CmdRTStartTemperatureControlWithPID(500, mp_IDeviceProcessing, this);
        cmdHeatRTSide->SetType(RT_SIDE);
        //todo: get temperature here
        cmdHeatRTSide->SetNominalTemperature(90);
        cmdHeatRTSide->SetSlopeTempChange(10);
        cmdHeatRTSide->SetMaxTemperature(120);
        cmdHeatRTSide->SetControllerGain(1212);
        cmdHeatRTSide->SetResetTime(1000);
        cmdHeatRTSide->SetDerivativeTime(80);
        m_SchedulerCommandProcessor->pushCmd(cmdHeatRTSide);
        SchedulerCommandShPtr_t resHeatRtSide;
        while(!PopDeviceControlCmdQueue(resHeatRtSide));
        resHeatRtSide->GetResult(retCode);
        if(DCL_ERR_FCT_CALL_SUCCESS != retCode)
        {
            //todo: error handling
            qDebug()<<"Failed to heat Retort side, return code: " << retCode;
            goto ERROR;
        }

        CmdRTStartTemperatureControlWithPID* cmdHeatRTBot = new CmdRTStartTemperatureControlWithPID(500, mp_IDeviceProcessing, this);
        cmdHeatRTBot->SetType(RT_BOTTOM);
        //todo: get temperature here
        cmdHeatRTBot->SetNominalTemperature(90);
        cmdHeatRTBot->SetSlopeTempChange(10);
        cmdHeatRTBot->SetMaxTemperature(120);
        cmdHeatRTBot->SetControllerGain(1212);
        cmdHeatRTBot->SetResetTime(1000);
        cmdHeatRTBot->SetDerivativeTime(80);
        m_SchedulerCommandProcessor->pushCmd(cmdHeatRTBot);
        SchedulerCommandShPtr_t resHeatRtBot;
        while(!PopDeviceControlCmdQueue(resHeatRtBot));
        resHeatRtBot->GetResult(retCode);
        if(DCL_ERR_FCT_CALL_SUCCESS != retCode)
        {
            //todo: error handling
            qDebug()<<"Failed to heat Retort bottom, return code: " << retCode;
            goto ERROR;
        }


        CmdRVStartTemperatureControlWithPID* cmdHeatRV = new CmdRVStartTemperatureControlWithPID(500, mp_IDeviceProcessing, this);
        //todo: get temperature here
        cmdHeatRV->SetNominalTemperature(90);
        cmdHeatRV->SetSlopeTempChange(10);
        cmdHeatRV->SetMaxTemperature(120);
        cmdHeatRV->SetControllerGain(1212);
        cmdHeatRV->SetResetTime(1000);
        cmdHeatRV->SetDerivativeTime(80);
        m_SchedulerCommandProcessor->pushCmd(cmdHeatRV);
        SchedulerCommandShPtr_t resHeatRV;
        while(!PopDeviceControlCmdQueue(resHeatRV));
        resHeatRV->GetResult(retCode);
        if(DCL_ERR_FCT_CALL_SUCCESS != retCode)
        {
            //todo: error handling
            qDebug()<<"Failed to heat Rotary valve, return code: " << retCode;
            goto ERROR;
        }

        CmdOvenStartTemperatureControlWithPID* cmdHeatOvenBot = new CmdOvenStartTemperatureControlWithPID(500, mp_IDeviceProcessing, this);
        cmdHeatOvenBot->SetType(OVEN_BOTTOM);
        //todo: get temperature here
        cmdHeatOvenBot->SetNominalTemperature(90);
        cmdHeatOvenBot->SetSlopeTempChange(10);
        cmdHeatOvenBot->SetMaxTemperature(120);
        cmdHeatOvenBot->SetControllerGain(1212);
        cmdHeatOvenBot->SetResetTime(1000);
        cmdHeatOvenBot->SetDerivativeTime(80);
        m_SchedulerCommandProcessor->pushCmd(cmdHeatOvenBot);
        SchedulerCommandShPtr_t resHeatOvenBot;
        while(!PopDeviceControlCmdQueue(resHeatOvenBot));
        resHeatOvenBot->GetResult(retCode);
        if(DCL_ERR_FCT_CALL_SUCCESS != retCode)
        {
            //todo: error handling
            qDebug()<<"Failed to heat oven bottom, return code: " << retCode;
            goto ERROR;
        }

        CmdOvenStartTemperatureControlWithPID* cmdHeatOvenTop = new CmdOvenStartTemperatureControlWithPID(500, mp_IDeviceProcessing, this);
        cmdHeatOvenTop->SetType(OVEN_TOP);
        //todo: get temperature here
        cmdHeatOvenTop->SetNominalTemperature(90);
        cmdHeatOvenTop->SetSlopeTempChange(10);
        cmdHeatOvenTop->SetMaxTemperature(120);
        cmdHeatOvenTop->SetControllerGain(1212);
        cmdHeatOvenTop->SetResetTime(1000);
        cmdHeatOvenTop->SetDerivativeTime(80);
        m_SchedulerCommandProcessor->pushCmd(cmdHeatOvenTop);
        SchedulerCommandShPtr_t resHeatOvenTop;
        while(!PopDeviceControlCmdQueue(resHeatOvenTop));
        resHeatOvenTop->GetResult(retCode);
        if(DCL_ERR_FCT_CALL_SUCCESS != retCode)
        {
            //todo: error handling
            qDebug()<<"Failed to heat oven top, return code: " << retCode;
            goto ERROR;
        }
        m_TimeStamps.OvenStartHeatingTime = QDateTime::currentDateTime().toMSecsSinceEpoch();

        CmdALStartTemperatureControlWithPID* cmdHeatALTube1  = new CmdALStartTemperatureControlWithPID(500, mp_IDeviceProcessing, this);
        cmdHeatALTube1->SetType(AL_LEVELSENSOR);
        //todo: get temperature here
        cmdHeatALTube1->SetNominalTemperature(90);
        cmdHeatALTube1->SetSlopeTempChange(10);
        cmdHeatALTube1->SetMaxTemperature(120);
        cmdHeatALTube1->SetControllerGain(200);
        cmdHeatALTube1->SetResetTime(1000);
        cmdHeatALTube1->SetDerivativeTime(0);
        m_SchedulerCommandProcessor->pushCmd(cmdHeatALTube1);
        SchedulerCommandShPtr_t resHeatALTube1;
        while(!PopDeviceControlCmdQueue(resHeatALTube1));
        resHeatALTube1->GetResult(retCode);
        if(DCL_ERR_FCT_CALL_SUCCESS != retCode)
        {
            //todo: error handling
            qDebug()<<"Failed to heat tube 1, return code: " << retCode;
            goto ERROR;
        }

        CmdALStartTemperatureControlWithPID* cmdHeatALTube2  = new CmdALStartTemperatureControlWithPID(500, mp_IDeviceProcessing, this);
        cmdHeatALTube2->SetType(AL_LEVELSENSOR);
        //todo: get temperature here
        cmdHeatALTube2->SetNominalTemperature(90);
        cmdHeatALTube2->SetSlopeTempChange(10);
        cmdHeatALTube2->SetMaxTemperature(120);
        cmdHeatALTube2->SetControllerGain(200);
        cmdHeatALTube2->SetResetTime(1000);
        cmdHeatALTube2->SetDerivativeTime(0);
        m_SchedulerCommandProcessor->pushCmd(cmdHeatALTube2);
        SchedulerCommandShPtr_t resHeatALTube2;
        while(!PopDeviceControlCmdQueue(resHeatALTube2));
        resHeatALTube2->GetResult(retCode);
        if(DCL_ERR_FCT_CALL_SUCCESS != retCode)
        {
            //todo: error handling
            qDebug()<<"Failed to heat tube 2, return code: " << retCode;
            goto ERROR;
        }
        bool ok;
        qreal pressureDrift = mp_DataManager->GetProgramSettings()->GetParameterValue("LA", "Base", "PressureDrift", ok);
        if(ok)
        {
#if 1
            pressureDrift= mp_IDeviceProcessing->ALGetRecentPressure(0);
            if(UNDEFINED_VALUE != pressureDrift)
            {
                mp_DataManager->GetProgramSettings()->SetParameterValue("LA", "Base", "PressureDrift", pressureDrift);
            }
            mp_IDeviceProcessing->ALSetPressureDrift(pressureDrift);
#else
            if(UNDEFINED_VALUE == pressureDrift)
            {
                 pressureDrift= mp_IDeviceProcessing->ALGetRecentPressure(0);
                 if(UNDEFINED_VALUE != pressureDrift)
                 {
                     mp_DataManager->GetProgramSettings()->SetParameterValue("LA", "Base", "PressureDrift", pressureDrift);
                 }
            }
            else
            {
            }
            mp_IDeviceProcessing->ALSetPressureDrift(pressureDrift);
#endif
        }

#endif
        // set state machine "init" to "idle" (David)
        m_SchedulerMachine->SendSchedulerInitComplete();
        //for debug
        qDebug() << "Current state of Scheduler is: " << m_SchedulerMachine->GetCurrentState();

        //send command to main controller to tell init complete
        MsgClasses::CmdProgramAcknowledge* commandPtr(new MsgClasses::CmdProgramAcknowledge(5000, DataManager::PROGRAM_READY));
        Q_ASSERT(commandPtr);
        Global::tRefType Ref = GetNewCommandRef();
        SendCommand(Ref, Global::CommandShPtr_t(commandPtr));
    }
    else
    {
ERROR:
        if(RetCode == DCL_ERR_TIMEOUT)
        {
            qDebug()<<"Some devices are not found during DCL's initialization period.";
        }
        //error happend
        // set state machine "init" to "error" (David)
        m_SchedulerMachine->SendErrorSignal();
        //for debug
        qDebug() << "Error while init, Current state of Scheduler is: " << m_SchedulerMachine->GetCurrentState();
    }
    m_TickTimer.start();
}

void SchedulerMainThreadController::HardwareMonitor(IDeviceProcessing* pIDP, const QString& StepID)
{
    if(pIDP)
    {
       // if(StepID == "IDLE")
        {
            qreal PressureAL= pIDP->ALGetRecentPressure(0);
            qreal TempALLevelSensor= pIDP->ALGetRecentTemperature(AL_LEVELSENSOR, 0);
            qreal TempALTube1= pIDP->ALGetRecentTemperature(AL_TUBE1,0);
            qreal TempALTube2= pIDP->ALGetRecentTemperature(AL_TUBE2,0);
            qreal TempRV1 = pIDP->RVGetRecentTemperature(0);
            qreal TempRV2 = pIDP->RVGetRecentTemperature(1);
            RVPosition_t PositionRV = pIDP->RVReqActRVPosition();
            qreal TempRTBottom= pIDP->RTGetRecentTemperature(RT_BOTTOM,0);
            qreal TempRTSide= pIDP->RTGetRecentTemperature(RT_SIDE,0);
            qreal TempOvenBottom= pIDP->OvenGetRecentTemperature(OVEN_BOTTOM,0);
            qreal TempOvenTop= pIDP->OvenGetRecentTemperature(OVEN_TOP,0);
            if(PressureAL != UNDEFINED_VALUE)
            {
                m_PressureAL = PressureAL;
            }
            if(TempALLevelSensor != UNDEFINED_VALUE)
            {
                m_TempALLevelSensor = TempALLevelSensor;
            }
            if(TempALTube1 != UNDEFINED_VALUE)
            {
                m_TempALTube1 = TempALTube1;
            }
            if(TempALTube2 != UNDEFINED_VALUE)
            {
                m_TempALTube2 = TempALTube2;
            }
            if(TempRV1 != UNDEFINED_VALUE)
            {
                m_TempRV1 = TempRV1;
            }
            if(TempRV2 != UNDEFINED_VALUE)
            {
                m_TempRV2 = TempRV2;
            }
            if(TempRTBottom != UNDEFINED_VALUE)
            {
                m_TempRTBottom = TempRTBottom;
            }
            if(TempRTSide!= UNDEFINED_VALUE)
            {
                m_TempRTSide = TempRTSide;
            }
            if(TempOvenBottom != UNDEFINED_VALUE)
            {
                m_TempOvenBottom = TempOvenBottom;
            }
            if(TempOvenTop != UNDEFINED_VALUE)
            {
                m_TempOvenTop = TempOvenTop;
            }
            m_PositionRV = PositionRV;
#if 0
            qDebug()<<"Rotary valve's position is" << PositionRV;
            qDebug()<<"Air liquid system pressure is" << PressureAL;
            qDebug()<<"Air liquid system level sensor's temp is" << TempALLevelSensor;
            qDebug()<<"Air liquid system tube1's temp is" << TempALTube1;
            qDebug()<<"Air liquid system tube2's temp is" << TempALTube2;
            qDebug()<<"Rotary valve's temp1 is" << TempRV1;
            qDebug()<<"Rotary valve's temp2 is" << TempRV2;
            qDebug()<<"Retort bottom temp is" << TempRTBottom;
            qDebug()<<"Retort side temp is" << TempRTSide;
            qDebug()<<"Oven bottom temp is" << TempOvenBottom;
            qDebug()<<"Oven top temp is" << TempOvenTop;
#endif
        }
    }
}

void SchedulerMainThreadController::DataManager(DataManager::CDataManager *p_DataManager)
{
    mp_DataManager = p_DataManager;
}

bool SchedulerMainThreadController::PopDeviceControlCmdQueue(Scheduler::SchedulerCommandShPtr_t& PtrCmd)
{
    bool ret = false;
    m_MutexDeviceControlCmdQueue.lock();
    if(!m_DeviceControlCmdQueue.isEmpty())
    {
        PtrCmd = m_DeviceControlCmdQueue.dequeue();
        ret = true;
    }
    m_MutexDeviceControlCmdQueue.unlock();
    return ret;
}

void SchedulerMainThreadController::PushDeviceControlCmdQueue(Scheduler::SchedulerCommandShPtr_t CmdPtr)
{
    m_MutexDeviceControlCmdQueue.lock();
    m_DeviceControlCmdQueue.push_back(CmdPtr);
    m_MutexDeviceControlCmdQueue.unlock();
}

void SchedulerMainThreadController::StepStart()
{

}

void SchedulerMainThreadController::HeatLevelSensor()
{
    ProgramStepStateMachine_t stepState = mp_ProgramStepStateMachine->GetCurrentState();
    if(PSSM_READY_TO_HEAT_LEVEL_SENSOR_S1 == stepState)
    {
        CmdALStartTemperatureControlWithPID* cmd  = new CmdALStartTemperatureControlWithPID(500, mp_IDeviceProcessing, this);
        cmd->SetType(AL_LEVELSENSOR);
        //todo: get temperature here
        cmd->SetNominalTemperature(90);
        cmd->SetSlopeTempChange(10);
        cmd->SetMaxTemperature(120);
        cmd->SetControllerGain(1212);
        cmd->SetResetTime(1000);
        cmd->SetDerivativeTime(80);
        m_SchedulerCommandProcessor->pushCmd(cmd);
    }
    else if(PSSM_READY_TO_HEAT_LEVEL_SENSOR_S2 == stepState)
    {
        CmdALStartTemperatureControlWithPID* cmd  = new CmdALStartTemperatureControlWithPID(500, mp_IDeviceProcessing, this);
        cmd->SetType(AL_LEVELSENSOR);
        //todo: get temperature here
        cmd->SetNominalTemperature(90);
        cmd->SetSlopeTempChange(10);
        cmd->SetMaxTemperature(120);
        cmd->SetControllerGain(200);
        cmd->SetResetTime(1000);
        cmd->SetDerivativeTime(0);
        m_SchedulerCommandProcessor->pushCmd(cmd);
    }
}

void SchedulerMainThreadController::MoveRV()
{
    ProgramStepStateMachine_t stepState = mp_ProgramStepStateMachine->GetCurrentState();
    CmdRVReqMoveToRVPosition* cmd = new CmdRVReqMoveToRVPosition(500, mp_IDeviceProcessing, this);

    if(PSSM_READY_TO_TUBE_BEFORE == stepState)
    {
        //get target position here
        RVPosition_t targetPos = GetRVTubePositionByStationID(m_CurProgramStepInfo.stationID);
        if(RV_UNDEF != targetPos)
        {
            cmd->SetRVPosition(targetPos);
            m_SchedulerCommandProcessor->pushCmd(cmd);
        }
        else
        {
           //todo: error handling
           qDebug()<<"Get invalid RV position: " << m_CurProgramStepInfo.stationID;
        }
    }
    else if(PSSM_READY_TO_TUBE_AFTER == stepState)
    {
        //get target position here
        RVPosition_t targetPos = GetRVTubePositionByStationID(m_CurProgramStepInfo.stationID);
        if(RV_UNDEF != targetPos)
        {
            cmd->SetRVPosition(targetPos);
            m_SchedulerCommandProcessor->pushCmd(cmd);
        }
        else
        {
           //todo: error handling
           qDebug()<<"Get invalid RV position: " << m_CurProgramStepInfo.stationID;
        }
    }
    else if(PSSM_READY_TO_SEAL == stepState)
    {
        //get target position here
        RVPosition_t targetPos = GetRVSealPositionByStationID(m_CurProgramStepInfo.stationID);
        if(RV_UNDEF != targetPos)
        {
            cmd->SetRVPosition(targetPos);
            m_SchedulerCommandProcessor->pushCmd(cmd);
        }
        else
        {
           //todo: error handling
           qDebug()<<"Get invalid RV position: " << m_CurProgramStepInfo.stationID;
        }
    }
}

void SchedulerMainThreadController::Fill()
{
    CmdALFilling* cmd  = new CmdALFilling(500, mp_IDeviceProcessing, this);
    //todo: get delay time here
    cmd->SetDelayTime(2000);
    m_SchedulerCommandProcessor->pushCmd(cmd);

    // acknowledge to gui
    MsgClasses::CmdStationSuckDrain* commandPtr(new MsgClasses::CmdStationSuckDrain(5000,m_CurProgramStepInfo.stationID , true, true));
    Q_ASSERT(commandPtr);
    Global::tRefType Ref = GetNewCommandRef();
    SendCommand(Ref, Global::CommandShPtr_t(commandPtr));
}

void SchedulerMainThreadController::StopFill()
{
    // acknowledge to gui
    MsgClasses::CmdStationSuckDrain* commandPtr(new MsgClasses::CmdStationSuckDrain(5000,m_CurProgramStepInfo.stationID , false, true));
    Q_ASSERT(commandPtr);
    Global::tRefType Ref = GetNewCommandRef();
    SendCommand(Ref, Global::CommandShPtr_t(commandPtr));
}

void SchedulerMainThreadController::Soak()
{
    if(m_TimeStamps.CurStepSoakStartTime == 0)
    {
        m_TimeStamps.CurStepSoakStartTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
        qDebug() << "Start to soak, start time stamp is:" << m_TimeStamps.CurStepSoakStartTime ;
    }
    if(m_CurProgramStepInfo.isPressure ^ m_CurProgramStepInfo.isVacuum)
    {
        if(m_CurProgramStepInfo.isPressure)
        {
            Pressure();
        }
        else
        {
            Vaccum();
        }
    }
}

void SchedulerMainThreadController::Drain()
{
    CmdALDraining* cmd  = new CmdALDraining(500, mp_IDeviceProcessing, this);
    //todo: get delay time here
    cmd->SetDelayTime(2000);
    m_SchedulerCommandProcessor->pushCmd(cmd);

    // acknowledge to gui
    MsgClasses::CmdStationSuckDrain* commandPtr(new MsgClasses::CmdStationSuckDrain(5000,m_CurProgramStepInfo.stationID , true, false));
    Q_ASSERT(commandPtr);
    Global::tRefType Ref = GetNewCommandRef();
    SendCommand(Ref, Global::CommandShPtr_t(commandPtr));
}

void SchedulerMainThreadController::StopDrain()
{
    // acknowledge to gui
    MsgClasses::CmdStationSuckDrain* commandPtr(new MsgClasses::CmdStationSuckDrain(5000,m_CurProgramStepInfo.stationID , false, false));
    Q_ASSERT(commandPtr);
    Global::tRefType Ref = GetNewCommandRef();
    SendCommand(Ref, Global::CommandShPtr_t(commandPtr));
}

void SchedulerMainThreadController::Pressure()
{
    m_SchedulerCommandProcessor->pushCmd(new CmdALPressure(500, mp_IDeviceProcessing, this));
}

void SchedulerMainThreadController::Vaccum()
{
    m_SchedulerCommandProcessor->pushCmd(new CmdALVaccum(500, mp_IDeviceProcessing, this));
}

void SchedulerMainThreadController::AllStop()
{
    mp_IDeviceProcessing->ALBreakAllOperation();
    m_SchedulerCommandProcessor->pushCmd(new CmdALAllStop(500, mp_IDeviceProcessing, this));
}

void SchedulerMainThreadController::Pause()
{
    //send command to main controller to tell program is actually pasued
    MsgClasses::CmdProgramAcknowledge* commandPtrPauseFinish(new MsgClasses::CmdProgramAcknowledge(5000,DataManager::PROGRAM_PAUSE_FINISHED));
    Q_ASSERT(commandPtrPauseFinish);
    Global::tRefType fRef = GetNewCommandRef();
    SendCommand(fRef, Global::CommandShPtr_t(commandPtrPauseFinish));
}

bool SchedulerMainThreadController::CheckStepTemperature()
{
    //todo: remove this when hardware is ready
    return true;
    qint32 targetTemp = m_CurProgramStepInfo.temperature;
    return ((targetTemp < m_TempRTBottom)&&(targetTemp < m_TempRTSide));
}

bool SchedulerMainThreadController::CheckLevelSensorTemperature(qreal targetTemperature)
{
    // get target temperature here
    return (m_TempALLevelSensor > targetTemperature);
}

void SchedulerMainThreadController::Abort()
{
    RVPosition_t targetPos = GetRVTubePositionByStationID(m_CurProgramStepInfo.stationID);
    if(RV_UNDEF != targetPos)
    {
        if((m_PositionRV != targetPos))
        {
            CmdRVReqMoveToRVPosition* cmd = new CmdRVReqMoveToRVPosition(500, mp_IDeviceProcessing, this);
            cmd->SetRVPosition(targetPos);
            m_SchedulerCommandProcessor->pushCmd(cmd);
        }
        else
        {
            if(mp_ProgramStepStateMachine->GetPreviousState() != PSSM_READY_TO_DRAIN)
            {
                this->Drain();
            }
            else
            {
                qDebug()<<"Already in draining process, abort will happen when draining finished.";
            }
        }
    }
}


RVPosition_t SchedulerMainThreadController::GetRVTubePositionByStationID(const QString stationID)
{
    RVPosition_t ret = RV_UNDEF;
    if(!stationID.isEmpty())
    {
        bool ok;
        int position = stationID.right(stationID.length() - 1).toInt(&ok, 10);
        if(ok)
        {
            ret = (RVPosition_t)(position * 2 - 1);
        }
    }
    return ret;
}

RVPosition_t SchedulerMainThreadController::GetRVSealPositionByStationID(const QString stationID)
{
    RVPosition_t ret = RV_UNDEF;
    if(!stationID.isEmpty())
    {
        bool ok;
        int position = stationID.right(stationID.length() - 1).toInt(&ok, 10);
        if(ok)
        {
            ret = (RVPosition_t)(position * 2);
        }
    }
    return ret;
}

bool SchedulerMainThreadController::SelfTest(ReturnCode_t RetCode)
{
    bool retValue = false;
    SelfTestStateMachine_t selfTestState = mp_SelfTestStateMachine->GetCurrentState();
    if(SELF_TEST_INIT == selfTestState)
    {
        bool ok;
        bool goon;
        // check oven heat time
        qreal parameter = mp_DataManager->GetProgramSettings()->GetParameterValue( "Oven", "oven heating",  "HeatingOvertime", ok);
        if(ok)
        {
             goon |= (GetOvenHeatingTime() > parameter);
        }

        // check temperature
        parameter = mp_DataManager->GetProgramSettings()->GetParameterValue( "Rotary Valve", "Heating",  "TemperatureOverrange", ok);
        if(ok)
        {
             goon |= (m_TempRV1 < parameter);
             goon |= (m_TempRV2 < parameter);
        }
        parameter = mp_DataManager->GetProgramSettings()->GetParameterValue( "Oven", "oven heating",  "TemperatureOverrange", ok);
        if(ok)
        {
             goon |= (m_TempOvenTop < parameter);
             goon |= (m_TempOvenBottom < parameter);
        }
        parameter = mp_DataManager->GetProgramSettings()->GetParameterValue( "Retort", "Retort heating",  "TemperatureOverrange", ok);
        if(ok)
        {
             goon |= (m_TempRTBottom < parameter);
             goon |= (m_TempRTSide < parameter);
        }
        parameter = mp_DataManager->GetProgramSettings()->GetParameterValue( "Retort", "level sensor",  "TemperatureOverrange", ok);
        if(ok)
        {
             goon |= (m_TempALLevelSensor < parameter);
        }
        parameter = mp_DataManager->GetProgramSettings()->GetParameterValue( "LA", "HeatingOfRetortToRVTube",  "TemperatureOverrange", ok);
        if(ok)
        {
             goon |= (m_TempALTube1 < parameter);
        }
        parameter = mp_DataManager->GetProgramSettings()->GetParameterValue( "LA", "HeatingOfRetortToRVWaxTrap",  "TemperatureOverrange", ok);
        if(ok)
        {
             goon |= (m_TempALTube2 < parameter);
        }
        // check pressure
        parameter = mp_DataManager->GetProgramSettings()->GetParameterValue( "LA", "Release",  "AmbientThresholdPressure", ok);
        if(ok)
        {
             goon |= (m_PressureAL < parameter);
             goon |= (m_PressureAL > (parameter * (-1)));
        }
        // check Rotary valve's position
        goon |= (m_PositionRV != RV_UNDEF);

        if(goon)
        {
            mp_SelfTestStateMachine->NotifyTempsReady();
        }
        else
        {
            //todo: raise error
        }
    }
    else if(SELF_TEST_TEMP_READY == selfTestState)
    {
        ProgramStationInfo_t stationInfo = m_ProgramStationList.dequeue();
        RVPosition_t tubePos = GetRVTubePositionByStationID(stationInfo.StationID);
        QString reagentGrpId = stationInfo.ReagentGroupID;

        CmdIDBottleCheck* cmd  = new CmdIDBottleCheck(500, mp_IDeviceProcessing, this);
        //todo: get delay time here
        cmd->SetReagentGrpID(reagentGrpId);
        cmd->SetTubePos(tubePos);
        m_SchedulerCommandProcessor->pushCmd(cmd);
        mp_SelfTestStateMachine->NotifyCheckStation();

    }
    else if(SELF_TEST_BOTTLE_CHECKING == selfTestState)
    {
        if( DCL_ERR_DEV_BOTTLE_CHECK_OK == RetCode)
        {
            mp_SelfTestStateMachine->NotifyGotCheckStationResult();
        }
        else if( DCL_ERR_DEV_BOTTLE_CHECK_NOT_FULL == RetCode)
        {
            mp_SelfTestStateMachine->NotifyGotCheckStationResult();
        }
        else if( DCL_ERR_DEV_BOTTLE_CHECK_BLOCKAGE == RetCode)
        {
            mp_SelfTestStateMachine->NotifyGotCheckStationResult();
        }
        else if(DCL_ERR_DEV_BOTTLE_CHECK_EMPTY == RetCode)
        {
            mp_SelfTestStateMachine->NotifyGotCheckStationResult();
        }
        else if(DCL_ERR_DEV_BOTTLE_CHECK_ERROR == RetCode)
        {
            mp_SelfTestStateMachine->NotifyGotCheckStationResult();
        }
        else if(DCL_ERR_DEV_BOTTLE_CHECK_TIMEOUT == RetCode)
        {
            mp_SelfTestStateMachine->NotifyGotCheckStationResult();
        }
        else if(DCL_ERR_UNDEFINED != RetCode)
        {
            qDebug() << "Unexpected ret code: "<< RetCode;
        }
    }
    else if(SELF_TEST_BOTTLE_CHECK_FINISH == selfTestState)
    {
        if(m_ProgramStationList.size() > 0)
        {
            ProgramStationInfo_t stationInfo = m_ProgramStationList.dequeue();
            RVPosition_t tubePos = GetRVTubePositionByStationID(stationInfo.StationID);
            QString reagentGrpId = stationInfo.ReagentGroupID;

            CmdIDBottleCheck* cmd  = new CmdIDBottleCheck(500, mp_IDeviceProcessing, this);
            cmd->SetReagentGrpID(reagentGrpId);
            cmd->SetTubePos(tubePos);
            m_SchedulerCommandProcessor->pushCmd(cmd);
            mp_SelfTestStateMachine->NotifyCheckStation();
        }
        else
        {
            mp_SelfTestStateMachine->NotifyCheckStaionFinished();
        }
    }
    else if(SELF_TEST_FINISH == selfTestState)
    {
        retValue = true;
    }
    else
    {
        //should not get here
        qDebug()<<"error when run Selftest state machine";
    }

    return retValue;
}

qint64 SchedulerMainThreadController::GetOvenHeatingTime()
{
    if(m_TimeStamps.OvenStartHeatingTime != 0)
    {
        return  (QDateTime::currentDateTime().toMSecsSinceEpoch() - m_TimeStamps.OvenStartHeatingTime)/1000;
    }
    else
    {
        return 0;
    }
}

bool SchedulerMainThreadController::IsLastStep(int currentStepIndex,const QString& currentProgramID)
{
    if (!mp_DataManager)
    {
        Q_ASSERT(false);
        return false;//need error handling
    }

    CDataProgramList* pDataProgramList = mp_DataManager->GetProgramList();
    if (!pDataProgramList)
    {
        Q_ASSERT(false);
        return false;
    }

    CProgram* pProgram = const_cast<CProgram*>(pDataProgramList->GetProgram(currentProgramID));
    ListOfIDs_t* stepIDs = pProgram->OrderedListOfStepIDs();
    return (stepIDs->count() == (currentStepIndex + 1));
}
}

