/****************************************************************************/
/*! \file LevelSensorDetectingTest.cpp
 *
 *  \brief Implementation of Retort Level Sensor Detecting test.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2014-10-16
 *   $Author:  $ Dixiong Li
 *
 *  \b Company:
 *
 *       Leica Biosystems R&D Center Shanghai.
 *
 *  (C) Copyright 2010 by LBS R&D Center Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#include "Diagnostics/Include/Retort/LevelSensorDetectingTest.h"
#include "Diagnostics/Include/SelectBottleNReagentDialog.h"
#include "Diagnostics/Include/LevelSensorHeatingDialog.h"
#include "Diagnostics/Include/ServiceDeviceProcess/ServiceDeviceProcess.h"

#include <QDebug>

namespace Diagnostics {

namespace Retort {

CLevelSensorDetectingTest::CLevelSensorDetectingTest(CDiagnosticMessageDlg* p_MessageDlg, QWidget* p_Parent)
    : CTestBase(p_Parent),
      mp_MessageDlg(p_MessageDlg),
      m_MessageTitle("Level Sensor Detection Test")
{
}

CLevelSensorDetectingTest::~CLevelSensorDetectingTest(void)
{
}

int CLevelSensorDetectingTest::Run(void)
{
    qDebug() << "Level sensor detecting test starts!";

    QString Text;

    if (ShowConfirmDlg(1) == 0 || ShowConfirmDlg(2) == 0) {
        return RETURN_ERR_FAIL;
    }

    Text = "Please enter the bottle number of the freshest process xylene";
    CSelectBottleNReagentDialog* p_SelectDlg = new CSelectBottleNReagentDialog(11, mp_Parent);
    p_SelectDlg->SetRadioButtonVisible(false);
    p_SelectDlg->SetTitle(m_MessageTitle);
    p_SelectDlg->SetLableText(Text);

    if (p_SelectDlg->exec() == 0) {
        delete p_SelectDlg;
        return RETURN_ERR_FAIL;
    }

    int BottleNumber = p_SelectDlg->GetBottleNumber();
    delete p_SelectDlg;

    ServiceDeviceProcess* p_DevProc = ServiceDeviceProcess::Instance();

    if (!TestRVInitialize()) {
         mp_MessageDlg->ShowRVMoveFailedDlg(m_MessageTitle);
         return RETURN_ERR_FAIL;
    }

    if (!TestRVMovePosition(true, BottleNumber)) {
        mp_MessageDlg->ShowRVMoveFailedDlg(m_MessageTitle);
        return RETURN_ERR_FAIL;
    }

    int HeatingRetValue = LevelSensorHeating(true);
    if (HeatingRetValue != RETURN_OK) {
        return HeatingRetValue;
    }

    Text = QString("Filling retort from the bottle %1").arg(BottleNumber);
    mp_MessageDlg->ShowWaitingDialog(m_MessageTitle, Text);
    int SuckRet = p_DevProc->PumpSucking();
    mp_MessageDlg->HideWaitingDialog();

    if (!TestDraining(SuckRet, BottleNumber)) {
        return RETURN_ERR_FAIL;
    }

    if (!TestRVMovePosition(true, 13)) {
        mp_MessageDlg->ShowRVMoveFailedDlg(m_MessageTitle);
        return RETURN_ERR_FAIL;
    }

    HeatingRetValue = LevelSensorHeating(false);
    if (HeatingRetValue != RETURN_OK) {
        return HeatingRetValue;
    }

    Text = QString("Filling retort from the bottle 13 cleaning alcohol.");
    mp_MessageDlg->ShowWaitingDialog(m_MessageTitle, Text);
    SuckRet = p_DevProc->PumpSucking(5);
    mp_MessageDlg->HideWaitingDialog();

    if (!TestDraining(SuckRet, 13)) {
        return RETURN_ERR_FAIL;
    }

    if (!TestRVInitialize()) {
         mp_MessageDlg->ShowRVMoveFailedDlg(m_MessageTitle);
         return RETURN_ERR_FAIL;
    }

    ShowFinishDlg(4);

    return RETURN_OK;
}

bool CLevelSensorDetectingTest::TestRVInitialize()
{
    QString Text = "Rotary Valve is initializing";
    mp_MessageDlg->ShowWaitingDialog(m_MessageTitle, Text);
    int ret = ServiceDeviceProcess::Instance()->RVInitialize();
    mp_MessageDlg->HideWaitingDialog();

    return ret == (int)RETURN_OK;
}

bool CLevelSensorDetectingTest::TestRVMovePosition(bool TubeFlag, int Position)
{
    QString Text;
    if (TubeFlag) {
        Text = QString("Rotary Valve is moving to tube position %1").arg(Position);

        if (Position == 13) {
            Text += " cleaning alcohol";
        }
    }
    else {
        Text = QString("Rotary Valve is moving to sealing position %1").arg(Position);
    }
    mp_MessageDlg->ShowWaitingDialog(m_MessageTitle, Text);
    int ret = ServiceDeviceProcess::Instance()->RVMovePosition(TubeFlag, Position);
    mp_MessageDlg->HideWaitingDialog();

    return ret == (int)RETURN_OK;
}

bool CLevelSensorDetectingTest::TestDraining(int RetCode, int Positon)
{
    (void)ServiceDeviceProcess::Instance()->LSStopHeating();

    if (!TestRVMovePosition(false, Positon)) {
        mp_MessageDlg->ShowRVMoveFailedDlg(m_MessageTitle);
        return false;
    }

     QString Text = "Releasing pressure...";
     mp_MessageDlg->ShowWaitingDialog(m_MessageTitle, Text);
    (void)ServiceDeviceProcess::Instance()->PumpReleasePressure();
    mp_MessageDlg->HideWaitingDialog();

    int Ret = ShowConfirmDlg(3);

    Text = "Close the lid and rotate the handle to the closed position.";
    mp_MessageDlg->ShowMessage(m_MessageTitle, Text, RETURN_OK);

    if (!TestRVMovePosition(true, Positon)) {
        mp_MessageDlg->ShowRVMoveFailedDlg(m_MessageTitle);
        return false;
    }

    Text = QString("Start draining to the bottle %1").arg(Positon);
    if (Positon == 13) {
        Text += " cleaning alcohol.";
    }
    mp_MessageDlg->ShowWaitingDialog(m_MessageTitle, Text);
    (void)ServiceDeviceProcess::Instance()->PumpDraining();
    mp_MessageDlg->HideWaitingDialog();

    if (RetCode == 1) {  //sucking success.
        if (Ret == 0) {
            ShowFinishDlg(2);
            return false;
        }
        else {
            return true;
        }
    }
    else {
        if (Ret == 0) {
            ShowFinishDlg(3);
        }
        else {
            ShowFinishDlg(2);
        }
        return false;
    }
}

int CLevelSensorDetectingTest::LevelSensorHeating(bool TempFlag)
{
    CLevelSensorHeatingDialog* p_HeatingDlg = new CLevelSensorHeatingDialog(mp_Parent);
    p_HeatingDlg->SetTitle(m_MessageTitle);

    bool HeatingRet = p_HeatingDlg->StartHeating(TempFlag);
    int RetValue = RETURN_OK;

    if (p_HeatingDlg->result() == 0) {
        HeatingRet = false;
        RetValue = RETURN_ABORT;
    }
    else if (!HeatingRet) {
        ShowFinishDlg(1);
        RetValue = RETURN_ERR_FAIL;
    }

    delete p_HeatingDlg;

    return RetValue;
}

int CLevelSensorDetectingTest::ShowConfirmDlg(int StepNum)
{
    QString Text;
    CDiagnosticMessageDlg::BUTTON_TYPE BtnType = CDiagnosticMessageDlg::OK_ABORT;
    switch (StepNum) {
    case 1:
        Text = "For this test, make sure to use only fresh cleaning "
                "alcohol in bottle 13. If necessary exchange it.";
        break;
    case 2:
        Text = "Make sure the retort is empty and dry. (If not, use "
                "the \"Diagnostic_Retort_Drain Reagent\" function first). "
                "Then close the lid and rotate the handle to the closed position.";
        break;
    case 3:
        Text = "Please open the retort lid and confirm liquid level; Does the "
                "liquid cover the level sensor?";
        BtnType = CDiagnosticMessageDlg::YES_NO;
        break;
    default:
        qDebug()<<"CLevelSensorDectetingTest: show confirm message error StepNum:"<<StepNum;
    }

    return mp_MessageDlg->ShowConfirmMessage(m_MessageTitle, Text, BtnType);
}

void CLevelSensorDetectingTest::ShowFinishDlg(int RetNum)
{
    QString Text;
    ErrorCode_t Ret = RETURN_ERR_FAIL;

    switch (RetNum) {
    case 1:
        Text = "Retort Level Sensor Heating Test failed.<br> "
                "Target temperature was not reached in time. Clean level sensor, "
                "repeat this test. If still failed, exchange the level sensor.";
        break;
    case 2:
        Text = " Retort Level Sensor Detection Test failed.<br> "
                "Clean level sensor, repeat this test. If still failed, exchange the level sensor.";
        break;
    case 3:
        Text = " Retort Level Sensor Detection Test failed.<br> "
                "Liquid level did not reach the sensor. Check for insufficient "
                "reagent level in bottle. Check for blockage reagent line. "
                "Check for air leakage. Resolve the problem and repeat level sensor detection test.";
        break;
    case 4:
        Text = "Level sensor detection test successful.";
        Ret = RETURN_OK;
        break;
    default:
        qDebug()<<"CLevelSensorDectetingTest: show finish message error RetNum:"<<RetNum;
    }

    mp_MessageDlg->ShowMessage(m_MessageTitle, Text, Ret);
}

} // namespace Retort

} // namespace Diagnostics
