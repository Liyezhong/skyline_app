// ================================================================================================
/*! @file Himalaya/HimalayaMain/Master/Components/Scheduler/Source/HeatingStrategy.cpp
 *
 *  @brief
 *  Implementation of class Scheduler::HeatingStrategy.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 04.03.2014 
 *  $Author:    $ Songtao Yu 
 *
 *  @b Company:
 *
 *       Leica Microsystems Ltd. Shanghai.
 *
 *  (C) Copyright 2014 by Leica Microsystems Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 * ================================================================================================
*/
#include "Scheduler/Include/HeatingStrategy.h"
#include "Scheduler/Include/SchedulerMainThreadController.h"
#include "Scheduler/Include/SchedulerCommandProcessor.h"
#include "HimalayaDataManager/Include/DataManager.h"
#include "Scheduler/Commands/Include/CmdRVStartTemperatureControlWithPID.h"
#include "Scheduler/Commands/Include/CmdOvenStartTemperatureControlWithPID.h"
#include "Scheduler/Commands/Include/CmdRTStartTemperatureControlWithPID.h"
#include "Scheduler/Commands/Include/CmdALStartTemperatureControlWithPID.h"

namespace Scheduler{
HeatingStrategy::HeatingStrategy(SchedulerMainThreadController* schedController,
                                SchedulerCommandProcessorBase* SchedCmdProcessor,
                                DataManager::CDataManager* DataManager)
								:mp_SchedulerController(schedController),
								mp_SchedulerCommandProcessor(SchedCmdProcessor),
                                mp_DataManager(DataManager)
{
    m_CurScenario = 0;
    this->ConstructHeatingSensorList();
}
DeviceControl::ReturnCode_t HeatingStrategy::RunHeatingStrategy(const HardwareMonitor_t& strctHWMonitor, qint32 scenario)
{
    ReturnCode_t retCode = DCL_ERR_FCT_CALL_SUCCESS;

    /********************************************************************************
     *
    Firstly, check if current temperature exceeds max temperature for each sensor
    *
    ********************************************************************************/
    //For Level Sensor
    if (false == this->CheckSensorCurrentTemperature(m_RTLevelSensor, strctHWMonitor.TempALLevelSensor))
    {
        return DCL_ERR_DEV_RETORT_LEVELSENSOR_TEMPERATURE_OVERRANGE;
    }
    //For Retort Top Sensor
    if (false == this->CheckSensorCurrentTemperature(m_RTTop, strctHWMonitor.TempRTSide))
    {
        return DCL_ERR_DEV_RETORT_TSENSOR1_TEMPERATURE_OVERANGE;
    }
    //For Retort Bottom Sensor
    if (false == this->CheckSensorCurrentTemperature(m_RTBottom, strctHWMonitor.TempRTBottom1))
    {
        return DCL_ERR_DEV_RETORT_TSENSOR2_TEMPERATURE_OVERRANGE;
    }
    //For Oven Top
    if (false == this->CheckSensorCurrentTemperature(m_OvenTop, strctHWMonitor.TempOvenTop))
    {
        return DCL_ERR_DEV_WAXBATH_TSENSORUP_HEATING_OUTOFTARGETRANGE;
    }
    //For Oven Bottom
    if (false == this->CheckSensorCurrentTemperature(m_OvenBottom, strctHWMonitor.TempOvenBottom1))
    {
        return DCL_ERR_DEV_WAXBATH_TSENSORDOWN1_HEATING_OUTOFTARGETRANGE;
    }
    //For Rotary Valve Rod
    if (false == this->CheckSensorCurrentTemperature(m_RVRod, strctHWMonitor.TempRV1))
    {
        return DCL_ERR_DEV_RV_HEATING_TEMPSENSOR1_OUTOFRANGE;
    }

    //For LA RV Tube
    if (false == this->CheckSensorCurrentTemperature(m_LARVTube, strctHWMonitor.TempALTube1))
    {
        return DCL_ERR_DEV_LA_TUBEHEATING_TSENSOR1_OUTOFRANGE;
    }

    //For LA Wax Trap
    if (false == this->CheckSensorCurrentTemperature(m_LAWaxTrap, strctHWMonitor.TempALTube2))
    {
        return DCL_ERR_DEV_LA_TUBEHEATING_TSENSOR2_OUTOFRANGE;
    }

    /***************************************************
     *
    Set temperature for each sensor
    *
    ***************************************************/
    bool scenariochanged = false; //used for Level Sensor
    if (scenario != m_CurScenario)
    {
        m_CurScenario = scenario;
        scenariochanged = true;
        //For RTTop
        retCode = StartRTTemperatureControl(m_RTTop, RT_SIDE);
        if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
        {
            return retCode;
        }
        //For RTBottom
        retCode = StartRTTemperatureControl(m_RTBottom, RT_BOTTOM);
        if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
        {
            return retCode;
        }

        //For RVRod
        retCode = StartRVTemperatureControl(m_RVRod);
        if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
        {
            return retCode;
        }

        //For RVOutlet
        StartRVOutletHeatingOTCalculation();
    }

    // For Level Sensor
    retCode = this->StartLevelSensorTemperatureControl(strctHWMonitor, scenariochanged);
    if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
    {
        return retCode;
    }

    // For Oven Top
    retCode = StartOvenTemperatureControl(m_OvenTop, OVEN_TOP);
    if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
    {
        return retCode;
    }
    //For Oven Bottom
    retCode = StartOvenTemperatureControl(m_OvenBottom, OVEN_BOTTOM);
    if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
    {
        return retCode;
    }
    //For LA RVTube
    retCode = StartLATemperatureControl(m_LARVTube, AL_TUBE1);
    if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
    {
        return retCode;
    }
    //For LA WaxTrap
    retCode = StartLATemperatureControl(m_LAWaxTrap, AL_TUBE2);
    if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
    {
        return retCode;
    }

    /***********************************************************
     *
    Check temperature difference of two Retort bottom sensors
    !!!!!!!uncomment return code when simulator is ready.
    *
    ***********************************************************/
    if (false == m_RTBottom.curModuleId.isEmpty() &&
            -1!= m_RTBottom.functionModuleList[m_RTBottom.curModuleId].ScenarioList.indexOf(m_CurScenario))
    {
        if (std::abs(strctHWMonitor.TempRTBottom1 - strctHWMonitor.TempRTBottom2) >= m_RTBottom.TemperatureDiffList[m_RTBottom.curModuleId])
        {
            mp_SchedulerController->LogDebug(QString("The temperature difference is: %1f").arg(std::abs(strctHWMonitor.TempRTBottom1 - strctHWMonitor.TempRTBottom2)));
            mp_SchedulerController->LogDebug(QString("Temperature Bottm1 is: %1 and Bottom2 is: %2").arg(strctHWMonitor.TempRTBottom1).arg(strctHWMonitor.TempRTBottom2));
            mp_SchedulerController->LogDebug(QString("Allowed temperature difference is: %1").arg(m_RTBottom.TemperatureDiffList[m_RTBottom.curModuleId]));
            return DCL_ERR_DEV_RETORT_TSENSOR1_TO_2_SELFCALIBRATION_FAILED;
        }
    }

    /*******************************************************
     *
    Check Heating Overtime
    *
    ******************************************************/
    // For Level Sensor
    if (false == this->CheckSensorHeatingOverTime(m_RTLevelSensor, strctHWMonitor.TempALLevelSensor))
    {
        return DCL_ERR_DEV_RETORT_LEVELSENSOR_HEATING_OVERTIME;
    }
    // For Retort Top
    if (false == this->CheckSensorHeatingOverTime(m_RTTop, strctHWMonitor.TempRTSide))
    {
        return DCL_ERR_DEV_RETORT_SIDTOP_SIDEMID_HEATING_ELEMENT_FAILED;
    }
    // For Retort Bottom
    if (false == this->CheckSensorHeatingOverTime(m_RTBottom, strctHWMonitor.TempRTBottom1))
    {
        return DCL_ERR_DEV_RETORT_BOTTOM_SIDELOW_HEATING_ELEMENT_FAILED;
    }
    //For Oven Top
    if (false == this->CheckSensorHeatingOverTime(m_OvenTop, strctHWMonitor.TempOvenTop))
    {
        return DCL_ERR_DEV_WAXBATH_TSENSORUP_HEATING_ABNORMAL;
    }
    //For Oven Bottom
    if (false == this->CheckSensorHeatingOverTime(m_OvenBottom, strctHWMonitor.TempOvenBottom1))
    {
        return DCL_ERR_DEV_WAXBATH_TSENSORUP_HEATING_ABNORMAL;
    }
    //For RV Outlet, and RV Rod is NOT needed to check Heating overtime.
    if (false == this->CheckRVOutletHeatingOverTime(strctHWMonitor.TempRV2))
    {
        return DCL_ERR_DEV_RV_HEATING_TEMPSENSOR2_NOTREACHTARGET;
    }
    //For LA RVTube
    if (false == this->CheckSensorHeatingOverTime(m_LARVTube, strctHWMonitor.TempALTube1))
    {
        return DCL_ERR_DEV_LA_TUBEHEATING_TUBE1_NOTREACHTARGETTEMP;
    }
    //For LA WaxTrap
    if (false == this->CheckSensorHeatingOverTime(m_LAWaxTrap, strctHWMonitor.TempALTube2))
    {
        return DCL_ERR_DEV_LA_TUBEHEATING_TUBE2_NOTREACHTARGETTEMP;
    }

    return DCL_ERR_FCT_CALL_SUCCESS;
}

bool HeatingStrategy::CheckSensorCurrentTemperature(const HeatingSensor& heatingSensor, qreal HWTemp)
{
    if (false == heatingSensor.curModuleId.isEmpty())
    {
        return true;
    }

    //For Scenarios NON-related sensors(Oven and LA)
    if (1 == heatingSensor.functionModuleList[heatingSensor.curModuleId].ScenarioList.size()
            && 0 == heatingSensor.functionModuleList[heatingSensor.curModuleId].ScenarioList.at(0))
    {
        if (heatingSensor.functionModuleList[heatingSensor.curModuleId].MaxTemperature <HWTemp)
        {
            return false;
        }
    }
    //For Scenarios related Sensors (Retort and Rotary valve)
    if (-1 != heatingSensor.functionModuleList[heatingSensor.curModuleId].ScenarioList.indexOf(m_CurScenario))
    {
        if (heatingSensor.functionModuleList[heatingSensor.curModuleId].MaxTemperature <HWTemp)
        {
            return false;
        }
    }

    return true;
}

DeviceControl::ReturnCode_t HeatingStrategy::StartLevelSensorTemperatureControl(const HardwareMonitor_t& strctHWMonitor, bool ScenarioChanged)
{
    ReturnCode_t retCode = DCL_ERR_FCT_CALL_SUCCESS;

    //For LevelSensor
    QMap<QString, FunctionModule>::iterator iter = m_RTLevelSensor.functionModuleList.begin();
    for (; iter!=m_RTLevelSensor.functionModuleList.end(); ++iter)
    {
        // Firstly, check if scenario changed. If yes, set the flag
        if (true == ScenarioChanged)
        {
            m_RTLevelSensor.StartTempFlagList[iter->Id] = false;
        }
        // check if the StartTemperaturePID has been set or not
        if (true == m_RTLevelSensor.StartTempFlagList[iter->Id])
        {
            continue;
        }
        // Current(new) scenario belongs to the specific scenario list
        if (iter->ScenarioList.indexOf(m_CurScenario) != -1)
        {
            // Check for High or Low speed
            if (m_RTLevelSensor.ExchangePIDTempList[iter->Id] >= strctHWMonitor.TempALLevelSensor)
            {
                if ("High" == m_RTLevelSensor.CurrentSpeedList[iter->Id])
                {
                    break;
                }
                else
                {
                    continue;
                }
            }
            else if (m_RTLevelSensor.ExchangePIDTempList[iter->Id] < strctHWMonitor.TempALLevelSensor)
            {
                if ("Low" == m_RTLevelSensor.CurrentSpeedList[iter->Id])
                {
                    break;
                }
                else
                {
                    continue;
                }
            }

        }
    }

    // Found out the level sensor's function module
    if (iter != m_RTLevelSensor.functionModuleList.end())
    {
        CmdALStartTemperatureControlWithPID* pHeatingCmd  = new CmdALStartTemperatureControlWithPID(500, mp_SchedulerController);
        pHeatingCmd->SetType(AL_LEVELSENSOR);
        pHeatingCmd->SetNominalTemperature(iter->TemperatureOffset);
        pHeatingCmd->SetSlopeTempChange(iter->SlopTempChange);
        pHeatingCmd->SetMaxTemperature(iter->MaxTemperature);
        pHeatingCmd->SetControllerGain(iter->ControllerGain);
        pHeatingCmd->SetResetTime(iter->ResetTime);
        pHeatingCmd->SetDerivativeTime(iter->DerivativeTime);
        mp_SchedulerCommandProcessor->pushCmd(pHeatingCmd);
        SchedulerCommandShPtr_t pResHeatingCmd;
        while (!mp_SchedulerController->PopDeviceControlCmdQueue(pResHeatingCmd, pHeatingCmd->GetName()));
        pResHeatingCmd->GetResult(retCode);
        if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
        {
            return retCode;
        }
        else
        {
            m_RTLevelSensor.heatingStartTime = QDateTime::currentMSecsSinceEpoch();
            m_RTLevelSensor.curModuleId = iter->Id;
            iter->OTTargetTemperature = iter->TemperatureOffset;
            m_RTLevelSensor.StartTempFlagList[iter->Id] = true;

            //Just for debug
            mp_SchedulerController->LogDebug(QString("targ temp is %1").arg(iter->TemperatureOffset));
            mp_SchedulerController->LogDebug(QString("SlopTempChange is %1").arg(iter->SlopTempChange));
            mp_SchedulerController->LogDebug(QString("MaxTemperature is %1").arg(iter->MaxTemperature));
            mp_SchedulerController->LogDebug(QString("ControllerGain is %1").arg(iter->ControllerGain));
            mp_SchedulerController->LogDebug(QString("ResetTime is %1").arg(iter->ResetTime));
            mp_SchedulerController->LogDebug(QString("DerivativeTime is %1").arg(iter->DerivativeTime));

            mp_SchedulerController->LogDebug(QString("LevelSensor - Scenario is %1").arg(m_CurScenario));
            mp_SchedulerController->LogDebug(QString("LevelSensor - current sequence is: %1").arg(iter->Id));
        }
    }

    // The current scenario is NOT related to Level Sensor's ones.
    return DCL_ERR_FCT_CALL_SUCCESS;
}

DeviceControl::ReturnCode_t HeatingStrategy::StartRTTemperatureControl(HeatingSensor& heatingSensor, RTTempCtrlType_t RTType)
{
    ReturnCode_t retCode = DCL_ERR_FCT_CALL_SUCCESS;

    QMap<QString, FunctionModule>::iterator iter = heatingSensor.functionModuleList.begin();
    for (; iter!=heatingSensor.functionModuleList.end(); ++iter)
    {
        // Current(new) scenario belongs to the specific scenario list
        if (iter->ScenarioList.indexOf(m_CurScenario) != -1)
        {
            break;
        }
    }

    // Found out the heating sensor's function module
    if (iter != heatingSensor.functionModuleList.end())
    {
		// Get the userInput temperature
		qreal userInputTemp = 0.0;
        // make sure program ID is NOT empty and current Program Step Index is NOT -1
        if (false == mp_SchedulerController->GetCurProgramID().isEmpty() && -1 != mp_SchedulerController->GetCurProgramStepIndex())
        {
            userInputTemp = mp_DataManager->GetProgramList()->GetProgram(mp_SchedulerController->GetCurProgramID())
                ->GetProgramStep(mp_SchedulerController->GetCurProgramStepIndex())->GetTemperature().toDouble();
        }   
		else
		{
			return DCL_ERR_FCT_CALL_SUCCESS;
		}
        if (qFuzzyCompare(userInputTemp+1, 0.0+1))
        {   
            return DCL_ERR_FCT_CALL_SUCCESS;
        }  
        CmdRTStartTemperatureControlWithPID* pHeatingCmd  = new CmdRTStartTemperatureControlWithPID(500, mp_SchedulerController);
        pHeatingCmd->SetType(RTType);
        pHeatingCmd->SetNominalTemperature(iter->TemperatureOffset+userInputTemp);
        pHeatingCmd->SetSlopeTempChange(iter->SlopTempChange);
        pHeatingCmd->SetMaxTemperature(iter->MaxTemperature);
        pHeatingCmd->SetControllerGain(iter->ControllerGain);
        pHeatingCmd->SetResetTime(iter->ResetTime);
        pHeatingCmd->SetDerivativeTime(iter->DerivativeTime);
        mp_SchedulerCommandProcessor->pushCmd(pHeatingCmd);
        SchedulerCommandShPtr_t pResHeatingCmd;
        while (!mp_SchedulerController->PopDeviceControlCmdQueue(pResHeatingCmd, pHeatingCmd->GetName()));
        pResHeatingCmd->GetResult(retCode);
        if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
        {
            return retCode;
        }
        else
        {
            heatingSensor.heatingStartTime = QDateTime::currentMSecsSinceEpoch();
            heatingSensor.curModuleId = iter->Id;
            iter->OTTargetTemperature = iter->TemperatureOffset+userInputTemp;
            return DCL_ERR_FCT_CALL_SUCCESS;
        }
    }

    // The current scenario is NOT related to Level Sensor's ones.
    return DCL_ERR_FCT_CALL_SUCCESS;
}

DeviceControl::ReturnCode_t HeatingStrategy::StartOvenTemperatureControl(OvenSensor& heatingSensor, OVENTempCtrlType_t OvenType)
{
    ReturnCode_t retCode = DCL_ERR_FCT_CALL_SUCCESS;

    //Firstly, get the Parrifin melting point (user input)
    qreal userInputMeltingPoint = mp_DataManager->GetUserSettings()->GetTemperatureParaffinBath();

    QMap<QString, FunctionModule>::iterator iter = heatingSensor.functionModuleList.begin();
    for(; iter!=heatingSensor.functionModuleList.end(); ++iter)
    {
        QPair<qreal,qreal> meltingRange = heatingSensor.ParaffinTempRangeList[iter->Id];
        QPair<qreal,qreal> timeRange = heatingSensor.TimeRangeList[iter->Id];

        // Get Time Elapse
        qint64 now = QDateTime::currentMSecsSinceEpoch();
        qreal timeElapse = 0.0;
        if (0 != heatingSensor.heatingStartTime)
        {
            timeElapse = (now - heatingSensor.heatingStartTime)/1000;
        }

        if (meltingRange.first<=userInputMeltingPoint && meltingRange.second>=userInputMeltingPoint && timeRange.first<=timeElapse && timeRange.second>=timeElapse)
        {
            // We only need set the temperature, so we have a check list for this
            if (false == heatingSensor.StartTempFlagList[iter->Id])
            {
                break;
            }
            else
            {
                continue;
            }
        }
    }

    // Found out the heating sensor's function module
    if (iter != heatingSensor.functionModuleList.end())
    {

        CmdOvenStartTemperatureControlWithPID* pHeatingCmd  = new CmdOvenStartTemperatureControlWithPID(500, mp_SchedulerController);
        pHeatingCmd->SetType(OvenType);
        pHeatingCmd->SetNominalTemperature(iter->TemperatureOffset+userInputMeltingPoint);
        pHeatingCmd->SetSlopeTempChange(iter->SlopTempChange);
        pHeatingCmd->SetMaxTemperature(iter->MaxTemperature);
        pHeatingCmd->SetControllerGain(iter->ControllerGain);
        pHeatingCmd->SetResetTime(iter->ResetTime);
        pHeatingCmd->SetDerivativeTime(iter->DerivativeTime);
        mp_SchedulerCommandProcessor->pushCmd(pHeatingCmd);
        SchedulerCommandShPtr_t pResHeatingCmd;
        while (!mp_SchedulerController->PopDeviceControlCmdQueue(pResHeatingCmd, pHeatingCmd->GetName()));
        pResHeatingCmd->GetResult(retCode);
        if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
        {
            return retCode;
        }
        else
        {
            heatingSensor.heatingStartTime = QDateTime::currentMSecsSinceEpoch();
            heatingSensor.curModuleId = iter->Id;
            iter->OTTargetTemperature = heatingSensor.OTTempOffsetList[iter->Id]+userInputMeltingPoint;
            heatingSensor.StartTempFlagList[iter->Id] = true;
            return DCL_ERR_FCT_CALL_SUCCESS;
        }
    }

    // The current scenario is NOT related to Level Sensor's ones.
    return DCL_ERR_FCT_CALL_SUCCESS;
}

DeviceControl::ReturnCode_t HeatingStrategy::StartRVTemperatureControl(RVSensor& heatingSensor)
{
    ReturnCode_t retCode = DCL_ERR_FCT_CALL_SUCCESS;

    //Firstly, get the Parrifin melting point (user input)
    qreal userInputMeltingPoint = mp_DataManager->GetUserSettings()->GetTemperatureParaffinBath();

    QMap<QString, FunctionModule>::iterator iter = heatingSensor.functionModuleList.begin();
    for (; iter!=heatingSensor.functionModuleList.end(); ++iter)
    {
        // Current(new) scenario belongs to the specific scenario list
        if (iter->ScenarioList.indexOf(m_CurScenario) != -1)
        {
            break;
        }
    }

    // Found out the heating sensor's function module
    if (iter != heatingSensor.functionModuleList.end())
    {

        CmdRVStartTemperatureControlWithPID* pHeatingCmd  = new CmdRVStartTemperatureControlWithPID(500, mp_SchedulerController);
        if (true == heatingSensor.UserInputFlagList[iter->Id])
        {
            pHeatingCmd->SetNominalTemperature(iter->TemperatureOffset+userInputMeltingPoint);
        }
        else
        {
            pHeatingCmd->SetNominalTemperature(iter->TemperatureOffset);
        }
        pHeatingCmd->SetSlopeTempChange(iter->SlopTempChange);
        pHeatingCmd->SetMaxTemperature(iter->MaxTemperature);
        pHeatingCmd->SetControllerGain(iter->ControllerGain);
        pHeatingCmd->SetResetTime(iter->ResetTime);
        pHeatingCmd->SetDerivativeTime(iter->DerivativeTime);
        mp_SchedulerCommandProcessor->pushCmd(pHeatingCmd);
        SchedulerCommandShPtr_t pResHeatingCmd;
        while (!mp_SchedulerController->PopDeviceControlCmdQueue(pResHeatingCmd, pHeatingCmd->GetName()));
        pResHeatingCmd->GetResult(retCode);
        if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
        {
            return retCode;
        }
        else
        {
            heatingSensor.heatingStartTime = QDateTime::currentMSecsSinceEpoch();
            heatingSensor.curModuleId = iter->Id;
            return DCL_ERR_FCT_CALL_SUCCESS;
        }
    }

    // The current scenario is NOT related to Level Sensor's ones.
    return DCL_ERR_FCT_CALL_SUCCESS;
}

DeviceControl::ReturnCode_t HeatingStrategy::StartLATemperatureControl(LASensor& heatingSensor,ALTempCtrlType_t LAType)
{
    ReturnCode_t retCode = DCL_ERR_FCT_CALL_SUCCESS;

    QMap<QString, FunctionModule>::iterator iter = heatingSensor.functionModuleList.begin();
    for(; iter!=heatingSensor.functionModuleList.end(); ++iter)
    {
        // We only need set the temperature, so we have a check list for this
        if (false == heatingSensor.StartTempFlagList[iter->Id])
        {
            break;
        }
        else
        {
            continue;
        }
    }

    // Found out the heating sensor's function module
    if (iter != heatingSensor.functionModuleList.end())
    {

        CmdALStartTemperatureControlWithPID* pHeatingCmd  = new CmdALStartTemperatureControlWithPID(500, mp_SchedulerController);
        pHeatingCmd->SetType(LAType);
        pHeatingCmd->SetNominalTemperature(iter->TemperatureOffset);
        pHeatingCmd->SetSlopeTempChange(iter->SlopTempChange);
        pHeatingCmd->SetMaxTemperature(iter->MaxTemperature);
        pHeatingCmd->SetControllerGain(iter->ControllerGain);
        pHeatingCmd->SetResetTime(iter->ResetTime);
        pHeatingCmd->SetDerivativeTime(iter->DerivativeTime);
        mp_SchedulerCommandProcessor->pushCmd(pHeatingCmd);
        SchedulerCommandShPtr_t pResHeatingCmd;
        while (!mp_SchedulerController->PopDeviceControlCmdQueue(pResHeatingCmd, pHeatingCmd->GetName()));
        pResHeatingCmd->GetResult(retCode);
        if (DCL_ERR_FCT_CALL_SUCCESS != retCode)
        {
            return retCode;
        }
        else
        {
            heatingSensor.StartTempFlagList[iter->Id] = true;
            heatingSensor.heatingStartTime = QDateTime::currentMSecsSinceEpoch();
            heatingSensor.curModuleId = iter->Id;
            iter->OTTargetTemperature = iter->TemperatureOffset;
            return DCL_ERR_FCT_CALL_SUCCESS;
        }
    }

    // The current scenario is NOT related to Level Sensor's ones.
    return DCL_ERR_FCT_CALL_SUCCESS;
}

void HeatingStrategy::StartRVOutletHeatingOTCalculation()
{
    // Current(new) scenario belongs to the specific scenario list
    int index = m_RVOutlet.ScenarioList.indexOf(m_CurScenario);
    if ( -1 == index)
    {
        m_RVOutlet.needCheckOT = false;
        return;
    }
    m_RVOutlet.heatingStartTime = QDateTime::currentMSecsSinceEpoch();
    m_RVOutlet.needCheckOT = true;

}

bool HeatingStrategy::ConstructHeatingSensorList()
{
    //For Retort Level Sensor
    m_RTLevelSensor.devName = "Retort";
    m_RTLevelSensor.sensorName = "LevelSensor";
    m_RTLevelSensor.heatingStartTime = 0;
    m_RTLevelSensor.curModuleId = "";
    QStringList sequenceList = {"11", "12", "21", "22"};
    if (false == this->ConstructHeatingSensor(m_RTLevelSensor, sequenceList))
    {
        return false;
    }
    // Add Current Speed (low or high) list and PID exchanging Temperature list
    QStringList::const_iterator iter = sequenceList.begin();
    for(; iter!=sequenceList.end(); ++iter)
    {
        DataManager::FunctionKey_t funcKey;
        funcKey.key = "Heating";
        funcKey.name = m_RTLevelSensor.sensorName;
        funcKey.sequence = *iter;
        QString curSpeed = mp_DataManager->GetProgramSettings()->GetParameterStrValue(m_RTLevelSensor.devName, funcKey, "CurrentSPeed");
        m_RTLevelSensor.CurrentSpeedList.insert(*iter, curSpeed);
        bool ok = false;
        qreal exchangePIDtmp = mp_DataManager->GetProgramSettings()->GetParameterValue(m_RTLevelSensor.devName, funcKey, "ExchangePIDTemp", ok);
        if (false == ok)
        {
            return false;
        }
        m_RTLevelSensor.ExchangePIDTempList.insert(*iter, exchangePIDtmp);

        //Flag list for checking if StartTemp command has been sent out
        m_RTLevelSensor.StartTempFlagList.insert(*iter, false);
    }


    // For Retort Top
    m_RTTop.devName = "Retort";
    m_RTTop.sensorName = "RTTop";
    m_RTTop.heatingStartTime = 0;
    m_RTTop.curModuleId = "";
    sequenceList = {"1", "2", "3"};
    if (false == this->ConstructHeatingSensor(m_RTTop, sequenceList))
    {
        return false;
    }

    // For Retort Bottom
    m_RTBottom.devName = "Retort";
    m_RTBottom.sensorName = "RTBottom";
    m_RTBottom.heatingStartTime = 0;
    m_RTBottom.curModuleId = "";
    sequenceList = {"1", "2", "3"};
    if (false == this->ConstructHeatingSensor(m_RTBottom, sequenceList))
    {
        return false;
    }
    // Add Temperature difference for each sequence
    iter = sequenceList.begin();
    for (; iter!= sequenceList.end(); ++iter)
    {
        DataManager::FunctionKey_t funcKey;
        funcKey.key = "Heating";
        funcKey.name = m_RTBottom.sensorName;
        funcKey.sequence = *iter;
        bool ok = false;
        qreal tempDiff = mp_DataManager->GetProgramSettings()->GetParameterValue(m_RTBottom.devName, funcKey, "TempDifference", ok);
        if (false == ok)
        {
            return false;
        }
        m_RTBottom.TemperatureDiffList.insert(*iter, tempDiff);
    }

    //For Oven Top
    m_OvenTop.devName = "Oven";
    m_OvenTop.sensorName = "OvenTop";
    m_OvenTop.heatingStartTime = 0;
    m_OvenTop.curModuleId = "";
    sequenceList = {"1", "2", "3", "4"};
    if (false == this->ConstructHeatingSensor(m_OvenTop, sequenceList))
    {
        return false;
    }
    //Add other attributes for Oven Top
    iter = sequenceList.begin();
    for (; iter != sequenceList.end(); ++iter)
    {
        DataManager::FunctionKey_t funcKey;
        funcKey.key = "Heating";
        funcKey.name = m_OvenTop.sensorName;
        funcKey.sequence = *iter;
        bool ok = false;

        //OverTimeTempOffset
        qreal OTTempOffset = mp_DataManager->GetProgramSettings()->GetParameterValue(m_OvenTop.devName, funcKey, "OverTimeTempOffset", ok);
        if (false == ok)
        {
            return false;
        }
        m_OvenTop.OTTempOffsetList.insert(*iter, OTTempOffset);

        //ParaffinTempRange
        QString paraffinTempRange = mp_DataManager->GetProgramSettings()->GetParameterStrValue(m_OvenTop.devName, funcKey, "ParaffinTempRange");
        QStringList  strList = paraffinTempRange.split(",");
        QPair<qreal,qreal> tempPair;
        tempPair.first = strList[0].toDouble(&ok);
        tempPair.second = strList[1].toDouble(&ok);
        m_OvenTop.ParaffinTempRangeList.insert(*iter, tempPair);

        //TimeRange
        QString timeRange = mp_DataManager->GetProgramSettings()->GetParameterStrValue(m_OvenTop.devName, funcKey, "TimeRange");
        strList = timeRange.split(",");
        QPair<qreal, qreal> timePair;
        timePair.first = strList[0].toDouble(&ok);
        timePair.second = strList[1].toDouble(&ok);
        m_OvenTop.TimeRangeList.insert(*iter,timePair);

        //Flag list for checking if StartTemp command has been sent out
        m_OvenTop.StartTempFlagList.insert(*iter, false);
    }

    //For Oven Bottom
    m_OvenBottom.devName = "Oven";
    m_OvenBottom.sensorName = "OvenBottom";
    m_OvenBottom.heatingStartTime = 0;
    m_OvenBottom.curModuleId = "";
    sequenceList = {"1", "2", "3", "4"};
    if (false == this->ConstructHeatingSensor(m_OvenBottom, sequenceList))
    {
        return false;
    }
    //Add other attributes for Oven Top
    iter = sequenceList.begin();
    for (; iter != sequenceList.end(); ++iter)
    {
        DataManager::FunctionKey_t funcKey;
        funcKey.key = "Heating";
        funcKey.name = m_OvenBottom.sensorName;
        funcKey.sequence = *iter;
        bool ok = false;

        //OverTimeTempOffset
        qreal OTTempOffset = mp_DataManager->GetProgramSettings()->GetParameterValue(m_OvenBottom.devName, funcKey, "OverTimeTempOffset", ok);
        if (false == ok)
        {
            return false;
        }
        m_OvenBottom.OTTempOffsetList.insert(*iter, OTTempOffset);

        //ParaffinTempRange
        QString paraffinTempRange = mp_DataManager->GetProgramSettings()->GetParameterStrValue(m_OvenBottom.devName, funcKey, "ParaffinTempRange");
        QStringList  strList = paraffinTempRange.split(",");
        QPair<qreal,qreal> tempPair;
        tempPair.first = strList[0].toDouble(&ok);
        tempPair.second = strList[1].toDouble(&ok);
        m_OvenBottom.ParaffinTempRangeList.insert(*iter, tempPair);

        //TimeRange
        QString timeRange = mp_DataManager->GetProgramSettings()->GetParameterStrValue(m_OvenBottom.devName, funcKey, "TimeRange");
        strList = timeRange.split(",");
        QPair<qreal, qreal> timePair;
        timePair.first = strList[0].toDouble(&ok);
        timePair.second = strList[1].toDouble(&ok);
        m_OvenBottom.TimeRangeList.insert(*iter,timePair);

        //Flag list for checking if StartTemp command has been sent out
        m_OvenBottom.StartTempFlagList.insert(*iter, false);
    }

    //For Rotary Valve Rod
    m_RVRod.devName = "Rotary Valve";
    m_RVRod.sensorName = "RVRod";
    m_RVRod.heatingStartTime = 0;
    m_RVRod.curModuleId = "";
    sequenceList = {"1", "2", "3", "4"};
    if (false == this->ConstructHeatingSensor(m_RVRod, sequenceList))
    {
        return false;
    }
    //Add other attributes for Rotary Valve Rod
    iter = sequenceList.begin();
    for (; iter != sequenceList.end(); ++iter)
    {
       //For checking if user input temperature is needed
        DataManager::FunctionKey_t funcKey;
        funcKey.key = "Heating";
        funcKey.name = m_RVRod.sensorName;
        funcKey.sequence = *iter;
        QString userInput = mp_DataManager->GetProgramSettings()->GetParameterStrValue(m_RVRod.devName, funcKey, "UserInput");
        if ("false" == userInput)
        {
            m_RVRod.UserInputFlagList.insert(*iter, false);
        }
        else
        {
            m_RVRod.UserInputFlagList.insert(*iter, true);
        }
    }

    //For Rotary Valve Outlet
    m_RVOutlet.devName = "Rotary Valve";
    m_RVOutlet.sensorName = "RVOutlet";
    m_RVOutlet.heatingStartTime = 0;
    m_RVOutlet.needCheckOT = false;

    DataManager::FunctionKey_t funcKey;
    funcKey.key = "Heating";
    funcKey.name = m_RVOutlet.sensorName;
    funcKey.sequence = "1";
    bool ok = false;
    qreal heatingOvertime = mp_DataManager->GetProgramSettings()->GetParameterValue(m_RVOutlet.devName, funcKey, "HeatingOverTime", ok);
    if (false == ok)
    {
        return false;
    }
    m_RVOutlet.HeatingOverTime = heatingOvertime;
    QString scenarioList = mp_DataManager->GetProgramSettings()->GetParameterStrValue(m_RVOutlet.devName, funcKey, "ScenarioList");
    QStringList strList = scenarioList.split(",");
    for (int i=0; i<strList.size(); ++i)
    {
        m_RVOutlet.ScenarioList.push_back(strList.at(i).toInt(&ok));
    }

    //For LA RV Tube
    m_LARVTube.devName = "LA";
    m_LARVTube.sensorName = "RVTube";
    m_LARVTube.heatingStartTime = 0;
    m_LARVTube.curModuleId = "";
    sequenceList = {"1"};
    if (false == this->ConstructHeatingSensor(m_LARVTube, sequenceList))
    {
        return false;
    }
    m_LARVTube.StartTempFlagList.insert("1", false);
    //For LA Wax Trap
    m_LAWaxTrap.devName = "LA";
    m_LAWaxTrap.sensorName = "WaxTrap";
    m_LAWaxTrap.heatingStartTime = 0;
    m_LAWaxTrap.curModuleId = "";
    sequenceList = {"1"};
    if (false == this->ConstructHeatingSensor(m_LAWaxTrap, sequenceList))
    {
        return false;
    }
    m_LAWaxTrap.StartTempFlagList.insert("1", false);
    return true;
}

bool HeatingStrategy::CheckSensorHeatingOverTime(const HeatingSensor& heatingSensor, qreal HWTemp)
{
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    if (false== heatingSensor.curModuleId.isEmpty() &&
            now-heatingSensor.heatingStartTime >= heatingSensor.functionModuleList[heatingSensor.curModuleId].HeatingOverTime*1000)
    {
        if (HWTemp < heatingSensor.functionModuleList[heatingSensor.curModuleId].OTTargetTemperature)
        {
            mp_SchedulerController->LogDebug(QString("Time elapse is %1 seconds").arg((now-heatingSensor.heatingStartTime)/1000));
            mp_SchedulerController->LogDebug(QString("current HW temperature is: %1").arg(HWTemp));
            return false;
        }
    }
    return true;
}

bool HeatingStrategy::CheckRVOutletHeatingOverTime(qreal HWTemp)
{
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    if (true == m_RVOutlet.needCheckOT &&
            now-m_RVOutlet.heatingStartTime >= m_RVOutlet.HeatingOverTime*1000)
    {
        if (HWTemp < mp_DataManager->GetUserSettings()->GetTemperatureParaffinBath())
        {
            return false;
        }
    }
    return true;
}

bool HeatingStrategy::ConstructHeatingSensor(HeatingSensor& heatingSensor, const QStringList& sequenceList)
{
   DataManager::FunctionKey_t funcKey;
   funcKey.key = "Heating";
   funcKey.name = heatingSensor.sensorName;

   QStringList::const_iterator seqIter = sequenceList.begin();
   for (; seqIter!=sequenceList.end(); ++seqIter)
   {
       FunctionModule funcModule;
       funcModule.Id = *seqIter;

       // Firstly, get the scenario list
       funcKey.sequence = *seqIter;
       QString strScenarioList = mp_DataManager->GetProgramSettings()->GetParameterStrValue(heatingSensor.devName, funcKey, "ScenarioList");
       QStringList  strList = strScenarioList.split(",");
       QStringList::const_iterator strIter = strList.begin();
       qint32 scenario = 0;
       bool ok = false;
       for (; strIter != strList.end(); ++strIter)
       {
           scenario = (*strIter).toInt(&ok);
           if (false == ok)
           {
               return false;
           }
           funcModule.ScenarioList.push_back(scenario);
       }

       // Get the others
       qreal temperatureOffset = mp_DataManager->GetProgramSettings()->GetParameterValue(heatingSensor.devName, funcKey, "NominalTemperature", ok);
       if (false == ok)
       {
           return false;
       }
       funcModule.TemperatureOffset = temperatureOffset;

       qreal maxTemperature= mp_DataManager->GetProgramSettings()->GetParameterValue(heatingSensor.devName, funcKey, "MaxTemperature", ok);
       if (false == ok)
       {
           return false;
       }
       funcModule.MaxTemperature = maxTemperature;

       qreal heatingOverTime= mp_DataManager->GetProgramSettings()->GetParameterValue(heatingSensor.devName, funcKey, "HeatingOverTime", ok);
       if (false == ok)
       {
           return false;
       }
       funcModule.HeatingOverTime = heatingOverTime;

       qreal slopTempChange= mp_DataManager->GetProgramSettings()->GetParameterValue(heatingSensor.devName, funcKey, "SlopeTempChange", ok);
       if (false == ok)
       {
           return false;
       }
       funcModule.SlopTempChange = slopTempChange;

       qreal controllerGain= mp_DataManager->GetProgramSettings()->GetParameterValue(heatingSensor.devName, funcKey, "ControllerGain", ok);
       if (false == ok)
       {
           return false;
       }
       funcModule.ControllerGain = controllerGain;

       qreal resetTime= mp_DataManager->GetProgramSettings()->GetParameterValue(heatingSensor.devName, funcKey, "ResetTime", ok);
       if (false == ok)
       {
           return false;
       }
       funcModule.ResetTime = resetTime;

       qreal derivativeTime= mp_DataManager->GetProgramSettings()->GetParameterValue(heatingSensor.devName, funcKey, "DerivativeTime", ok);
       if (false == ok)
       {
           return false;
       }
       funcModule.DerivativeTime = derivativeTime;
       funcModule.OTTargetTemperature = 0.0; //Initialize target temperature to zero

       heatingSensor.functionModuleList.insert(*seqIter, funcModule);
   }

   return true;
}
}// end of namespace Scheduler
