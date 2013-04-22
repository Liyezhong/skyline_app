/****************************************************************************/
/*! \file SpecialVerifierGroupA.cpp
 *
 *  \brief Implementation file for class CSpecialVerifierGroupA.
 *
 *  $Version:   $ 0.2
 *  $Date:      $ 2011-08-01, 2012-05-03
 *  $Author:    $ F. Toth, Raju
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#include <QDebug>
#include <QFile>

#include "HimalayaDataContainer/SpecialVerifiers/Include/SpecialVerifierGroupA.h"
#include "Global/Include/EventObject.h"
#include "HimalayaDataContainer/Helper/Include/HimalayaDataManagerEventCodes.h"


namespace DataManager {

/****************************************************************************/
/*!
 *  \brief Constructor
 */
/****************************************************************************/
CSpecialVerifierGroupA::CSpecialVerifierGroupA() :
        mp_DProgramList(NULL), mp_DReagentList(NULL),
        mp_DStationList(NULL), m_pDataReagentGroupList(NULL)
{
}

/****************************************************************************/
/*!
 *  \brief  Constructor
 *
 *  \iparam p_DataProgramList = Pointer to a Program list
 *  \iparam p_DataReagentList = Pointer to a Reagent list
 *  \iparam p_DataStationList = Pointer to a Station list
 */
/****************************************************************************/
CSpecialVerifierGroupA::CSpecialVerifierGroupA(CDataProgramList* p_DataProgramList, CDataReagentList* p_DataReagentList,
                                               CDashboardDataStationList* p_DataStationList, CDataReagentGroupList* pDataReagentGroupList)
    : mp_DProgramList(p_DataProgramList),
      mp_DReagentList(p_DataReagentList),
      mp_DStationList(p_DataStationList),
      m_pDataReagentGroupList(pDataReagentGroupList)

{
}


/****************************************************************************/
/*!
 *  \brief  Verifies the data
 *
 *  \iparam p_ContainerBase = Pointer to a Container base
 *
 *  \return Successful (true) or not (false)
 */
/****************************************************************************/
bool CSpecialVerifierGroupA::VerifyData(CDataContainerBase* p_ContainerBase)
{
    bool Result = true;

    if (p_ContainerBase != NULL) {
        // prepare member pointers according to given container type
        // switch given container to temporary one for check
        // and switch back to real data after check again
        CDataContainerBase* p_TempDataItem = NULL;

        // check the containers consists of data or not if not please log it
        // which container is failed. This is for logger
        if (mp_DReagentList == NULL) {
            Global::EventObject::Instance().RaiseEvent(EVENT_DM_GV_REAGENT_IS_NOT_INITIALIZED);
            qDebug() << "CSpecialVerifierGroupA::VerifyData failed. Reagent List is NULL";
            return false;
        }
        if (mp_DProgramList == NULL) {
            Global::EventObject::Instance().RaiseEvent(EVENT_DM_GV_PROGRAM_IS_NOT_INITIALIZED);
            qDebug() << "CSpecialVerifierGroupA::VerifyData failed. Program List is NULL";
            return false;
        }
        if (mp_DStationList == NULL) {
            Global::EventObject::Instance().RaiseEvent(EVENT_DM_GV_STATION_IS_NOT_INITIALIZED);
            qDebug() << "CSpecialVerifierGroupA::VerifyData failed. Station List is NULL";
            return false;
        }

        // copy all the data data to temporary variables
        if (p_ContainerBase->GetDataContainerType() == PROGRAMS) {
            p_TempDataItem = mp_DProgramList;
            mp_DProgramList = static_cast<CDataProgramList*>(p_ContainerBase);
        }
        else if (p_ContainerBase->GetDataContainerType() == REAGENTS) {
            p_TempDataItem = mp_DReagentList;
            mp_DReagentList = static_cast<CDataReagentList*>(p_ContainerBase);
        }
        else if (p_ContainerBase->GetDataContainerType() == STATIONS) {
            p_TempDataItem = mp_DStationList;
            mp_DStationList = static_cast<CDashboardDataStationList*>(p_ContainerBase);
        }
        else {
            Global::EventObject::Instance().RaiseEvent(EVENT_DM_GV_INVALID_CONTAINER_TYPE);
            qDebug() << "CSpecialVerifierGroupA::VerifyData failed. Invalid conatiner type";
            return false;
        }

        if (Result) {
            if (!CheckData()) {
                Result = false;
            }
        }

        // revert all the temporary data to local variables
        if (p_ContainerBase->GetDataContainerType() == REAGENTS) {
            mp_DReagentList = static_cast<CDataReagentList*>(p_TempDataItem);
        }
        else if (p_ContainerBase->GetDataContainerType() == PROGRAMS) {
            mp_DProgramList = static_cast<CDataProgramList*>(p_TempDataItem);
        }
        else if (p_ContainerBase->GetDataContainerType() == STATIONS) {
            mp_DStationList = static_cast<CDashboardDataStationList*>(p_TempDataItem);
        }
    }
    else {
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_GV_BASE_CONTAINER_IS_NOT_INITIALIZED);
        qDebug() << "CSpecialVerifierGroupA::VerifyData failed. Container base is not initialised";;
        Result = false;
    }

    return Result;

}


/****************************************************************************/
/*!
 *  \brief  Checks the data with other containers (Like Program list,
 *          Station list and Reagent list)
 *
 *  \return Successful (true) or not (false)
 */
/****************************************************************************/
bool CSpecialVerifierGroupA::CheckData()
{
    bool Result = true;
    // check required to avoid lint warning
    if (mp_DReagentList && mp_DProgramList) {
        // check each program has a valid reagent ID or not
        for (qint32 Counter = 0; Counter < mp_DProgramList->GetReagentIDList().count(); Counter++) {
            // check the Program Reagent ID exists in ReagentID List
            if (!(mp_DReagentList->ReagentExists(mp_DProgramList->GetReagentIDList().at(Counter)) ||
                  (mp_DProgramList->GetReagentIDList().at(Counter) == "-1"))) {            
                m_ErrorsHash.insert(EVENT_DM_GV_REAGENTID_EXIST_IN_PROG_NOT_IN_REAGENT_LIST,
                                    Global::tTranslatableStringList() << mp_DProgramList->GetReagentIDList().at(Counter));
                Global::EventObject::Instance().RaiseEvent(EVENT_DM_GV_REAGENTID_EXIST_IN_PROG_NOT_IN_REAGENT_LIST,
                                                           Global::tTranslatableStringList() << mp_DProgramList->GetReagentIDList().at(Counter),
                                                           Global::GUI_MSG_BOX);
                Result = false;
                break;
            }
        }
        CProgram* p_Program = NULL;
        CProgramStep *Previous_ProgramStep = NULL;
        QString Previous_ReagentGroupID;
        QString Previous_ReagentID;
        CProgramStep *Current_ProgramStep = NULL;
        QString Current_ReagentGroupID;
        QString Current_ReagentID;
        qint32 temperature;
        for (qint32 I = 0; I < mp_DProgramList->GetNumberOfPrograms(); I++) {
            p_Program = mp_DProgramList->GetProgram(I);
            if (p_Program) {
                for (qint32 X = 0; X < p_Program->GetNumberOfSteps(); X++) {
                    Current_ProgramStep = const_cast<CProgramStep *>(p_Program->GetProgramStep(X));
                    if (Current_ProgramStep != NULL) {
                        //check temperature
                        Current_ReagentID = Current_ProgramStep->GetReagentID();
                        if(Current_ReagentID.isEmpty()){
                            continue;
                        }
                        Current_ReagentGroupID = mp_DReagentList->GetReagent(Current_ReagentID)->GetGroupID();
                        bool ok = false;
                        temperature = Current_ProgramStep->GetTemperature().toInt(&ok);
                        if(ok){
                            // paraffin
                            CReagentGroup* pReagentGroup = m_pDataReagentGroupList->GetReagentGroup(Current_ReagentGroupID);
                            if(pReagentGroup->IsParraffin()){
                                if(temperature < STEP_PARAFFIN_TEMP_MIN || temperature > STEP_PARAFFIN_TEMP_MAX){
                                    // error
                                    m_ErrorsHash.insert(VENT_DM_PROG_STEP_TEMP_EXCEED_LIMIT,
                                                        Global::tTranslatableStringList() << Current_ProgramStep->GetStepID()
                                                        << p_Program->GetName() << QString::number(STEP_PARAFFIN_TEMP_MIN)
                                                        << QString::number(STEP_PARAFFIN_TEMP_MAX));
                                    Global::EventObject::Instance().RaiseEvent(EVENT_DM_PARAFFIN_TEMP_OUT_OF_RANGE,
                                                                               Global::tTranslatableStringList() << Current_ProgramStep->GetStepID()
                                                                               << p_Program->GetName() << QString::number(STEP_PARAFFIN_TEMP_MIN)
                                                                               << QString::number(STEP_PARAFFIN_TEMP_MAX),
                                                                               Global::GUI_MSG_BOX);
                                    Result = false;

                                }
                            }
                            //reagents
                            else
                            {
                                if(temperature < STEP_REAGENT_TEMP_MIN || temperature > STEP_REAGENT_TEMP_MAX){
                                    // error
                                    m_ErrorsHash.insert(VENT_DM_PROG_STEP_TEMP_EXCEED_LIMIT,
                                                        Global::tTranslatableStringList() << Current_ProgramStep->GetStepID()
                                                        << p_Program->GetName() << QString::number(STEP_REAGENT_TEMP_MIN)
                                                        << QString::number(STEP_REAGENT_TEMP_MAX));
                                    Global::EventObject::Instance().RaiseEvent(EVENT_DM_PARAFFIN_TEMP_OUT_OF_RANGE,
                                                                               Global::tTranslatableStringList() << Current_ProgramStep->GetStepID()
                                                                               << p_Program->GetName() << QString::number(STEP_REAGENT_TEMP_MIN)
                                                                               << QString::number(STEP_REAGENT_TEMP_MAX),
                                                                               Global::GUI_MSG_BOX);
                                    Result = false;
                                }
                            }
                        }
                        //check reagent group Compatible
                        if(X > 0){
                            Previous_ProgramStep = const_cast<CProgramStep *>(p_Program->GetProgramStep(X - 1));
                            if(Previous_ProgramStep){
                                Previous_ReagentID = Previous_ProgramStep->GetReagentID();
                                Previous_ReagentGroupID = mp_DReagentList->GetReagent(Previous_ReagentID)->GetGroupID();
                                if(!IsCompatible(Current_ReagentGroupID,Previous_ReagentGroupID)){
                                    //error
                                    m_ErrorsHash.insert(EVENT_DM_INCOMPATIBLE_STEP_REAGENT_GROUP,
                                                        Global::tTranslatableStringList() <<p_Program->GetName()
                                                        << Current_ProgramStep->GetStepID() << Previous_ProgramStep->GetStepID());
                                    Global::EventObject::Instance().RaiseEvent(EVENT_DM_INCOMPATIBLE_STEP_REAGENT_GROUP,
                                                                               Global::tTranslatableStringList() <<p_Program->GetName()
                                                                               << Current_ProgramStep->GetStepID() << Previous_ProgramStep->GetStepID(),
                                                                               Global::GUI_MSG_BOX);
                                    Result = false;
                                }
                            }
                        }// end if for reagent group compatible check
                    }
                }// end of loop for step
            }
        }//end of loop for program
    }    

    // if everything goes well then Special verifier verified all the data
    return Result;
}

 bool CSpecialVerifierGroupA::IsCompatible(const QString& currentReagentGroupID, const QString& PreviousReagentGroupID)
 {
    if (currentReagentGroupID.isEmpty()||PreviousReagentGroupID.isEmpty())
        return true;

    if(!m_pDataReagentGroupList)
        return false;

     int i = m_pDataReagentGroupList->GetReagentGroupIndex(PreviousReagentGroupID);
     int j = m_pDataReagentGroupList->GetReagentGroupIndex(currentReagentGroupID);

     if (i > j)//swap
     {
         int temp = i;
         i = j;
         j= temp;
     }

    int arr[8][8]={1, 1, 0, 0, 0, 0, 0, 1,
                 0, 1, 1, 1, 0, 0, 0, 1,//water
                 0, 0, 1, 1, 0, 0, 0, 1,//dehydrating,diluted
                 0, 0, 0, 1, 1, 0, 0, 1,
                 0, 0, 0, 0, 1, 1, 1, 0, //clearing
                 0, 0, 0, 0, 0, 1, 1, 0,
                 0, 0, 0, 0, 0, 0, 1, 1,
                 0, 0, 0, 0, 0, 0, 0, 1};

    if (1 == arr[i][j])
       return true;
    else
        return false;
 }

}  // namespace DataManager

