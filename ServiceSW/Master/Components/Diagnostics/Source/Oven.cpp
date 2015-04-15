/****************************************************************************/
/*! \file Diagnostics/Source/Oven.cpp
 *
 *  \brief Implementation of Oven test.
 *
 *   $Version: $ 1.0
 *   $Date:    $ 2014-10-10
 *   $Author:  $ Arthur Li
 *
 *  \b Company:
 *
 *       Leica Biosystems R&D Center Shanghai.
 *
 *  (C) Copyright 2014 by LBS R&D Center Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/
#include <QDebug>
#include "Diagnostics/Include/Oven/OvenHeatingTestEmpty.h"
#include "Diagnostics/Include/Oven/CoverSensorTest.h"
#include "Global/Include/Utils.h"
#include "Main/Include/HimalayaServiceEventCodes.h"
#include "Diagnostics/Include/Oven.h"
#include "ui_Oven.h"

namespace Diagnostics {
// lint -e578
COven::COven(QWidget *p_Parent)
    : QWidget(p_Parent)
    , ui(new Ui::COven)
{
    ui->setupUi(this);
    dlg = new CDiagnosticMessageDlg(this);
    (void)connect(ui->testHeatingEmpty,
                 SIGNAL(clicked()),
                 this,
                 SLOT(StartHeatingTestEmpty()) );

    (void)connect(ui->testCoverSensor,
                  SIGNAL(clicked()),
                  this,
                  SLOT(StartCoverSensorTest()) );
}

COven::~COven()
{
    try {
        delete dlg;
        delete ui;
    } catch (...) {

    }
}

void COven::LogResult(QString &TestName, ErrorCode_t RetError)
{
    if (RetError == RETURN_OK)
        Global::EventObject::Instance().RaiseEvent(EVENT_COMMON_ID, Global::tTranslatableStringList() << QString("%1 is successful.").arg(TestName));
    else if (RetError == RETURN_ABORT)
        Global::EventObject::Instance().RaiseEvent(EVENT_COMMON_ID, Global::tTranslatableStringList() << QString("%1 is aborted.").arg(TestName));
    else
        Global::EventObject::Instance().RaiseEvent(EVENT_COMMON_ID, Global::tTranslatableStringList() << QString("%1 is failed.").arg(TestName));

}


void COven::StartHeatingTestEmpty(void)
{
    QString TestName = QString("Paraffin Oven %1").arg(ui->testHeatingEmpty->text());
    Global::EventObject::Instance().RaiseEvent(EVENT_COMMON_ID, Global::tTranslatableStringList() << QString("%1 is requested.").arg(TestName));


    emit SetGUITabEnable(false);
    Oven::CHeatingTestEmpty test(dlg);

    ErrorCode_t ret = (ErrorCode_t)test.Run();

    LogResult(TestName, ret);

    emit SetGUITabEnable(true);
}

void COven::StartCoverSensorTest(void)
{
    QString TestName = QString("Paraffin Oven %1").arg(ui->testCoverSensor->text());
    Global::EventObject::Instance().RaiseEvent(EVENT_COMMON_ID, Global::tTranslatableStringList() << QString("%1 is requested.").arg(TestName));


    emit SetGUITabEnable(false);
    Oven::CCoverSensorTest test(dlg);

    ErrorCode_t ret = (ErrorCode_t)test.Run();

    LogResult(TestName, ret);

    emit SetGUITabEnable(true);
}

/****************************************************************************/
/*!
 *  \brief Event handler for change events
 *
 *  \iparam p_Event = Change event
 */
/****************************************************************************/
void COven::changeEvent(QEvent *p_Event)
{
    QWidget::changeEvent(p_Event);
    switch (p_Event->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

} // namespace Diagnostics
