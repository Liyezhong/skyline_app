/****************************************************************************/
/*! \file ReagentWidget.cpp.cpp
 *
 *  \brief ReagentWidget Implementation.
 *
 *   $Version: $ 0.2
 *   $Date:    $ 2013-01-15
 *   $Author:  $ Swati Tiwari
 *
 *  \b Company:
 *
 *       Leica Microsystems Ltd. Shanghai.
 *
 *  (C) Copyright 2012 by Leica Microsystems Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/
#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"
#include "MainMenu/Include/ScrollWheel.h"
#include "MainMenu/Include/SliderControl.h"
#include "Reagents/Include/ReagentRMSWidget.h"
#include "Reagents/Include/ModifyReagentRMSDlg.h"
#include "ui_ReagentsWidget.h"
#include <QDebug>

namespace Reagents {

Global::RMSOptions_t CReagentRMSWidget :: RMSPROCESSINGOPTION = Global::RMS_CASSETTES;
Global::RMSOptions_t CReagentRMSWidget :: RMSCLEANINGOPTIONS = Global::RMS_OFF;

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent object
 */
/****************************************************************************/
CReagentRMSWidget::CReagentRMSWidget(QWidget *p_Parent):
    MainMenu::CPanelFrame(p_Parent),
    mp_Ui(new Ui::CReagentRMSWidget),
    mp_DataConnector(NULL),
    mp_ReagentList(NULL),
    mp_Reagent(NULL),
    mp_ModifiyReagentRMSDlg(NULL)
{
    mp_Ui->setupUi(GetContentFrame());
    SetPanelTitle(tr("RMS"));

    m_ProcessRunning = false;
    m_ShowMessageDialog = false;

    m_ReagentCleaningModel.SetReagentGroupType(true);
    mp_TableWidgetCleaning = new MainMenu::CBaseTable;
    mp_TableWidgetCleaning->setModel(&m_ReagentCleaningModel);
    mp_Ui->scrolltable_cleaning->SetContent(mp_TableWidgetCleaning);

    m_ReagentRMSModel.SetReagentGroupType(false);
    mp_TableWidget = new MainMenu::CBaseTable;
    mp_TableWidget->setModel(&m_ReagentRMSModel);
    mp_Ui->scrollTable->SetContent(mp_TableWidget);

    m_CurrentUserRole = MainMenu::CMainWindow::GetCurrentUserRole();

    mp_Ui->radioCassettes->setChecked(true);
    mp_Ui->radioOff_Cleaning->setChecked(true);

    CONNECTSIGNALSLOT(mp_Ui->btnEdit, clicked(), this, OnEdit());
    CONNECTSIGNALSLOT(mp_Ui->btnNew, clicked(), this, OnNew());
    CONNECTSIGNALSLOT(mp_Ui->btnDelete, clicked(), this, OnDelete());
    CONNECTSIGNALSLOT(mp_Ui->radioOff, clicked(), this, OnRMSOFF());
    CONNECTSIGNALSLOT(mp_Ui->radioCassettes, clicked(), this, OnRMSCassettes());
    CONNECTSIGNALSLOT(mp_Ui->radioCycles, clicked(), this, OnRMSCycles());
    CONNECTSIGNALSLOT(mp_Ui->radioDays, clicked(), this, OnRMSDays());
    CONNECTSIGNALSLOT(mp_Ui->radioOff_Cleaning, clicked(), this, OnCleaningRMSOFF());
    CONNECTSIGNALSLOT(mp_Ui->radioCycles_Cleaning, clicked(), this, OnCleaningRMSCycles());
    CONNECTSIGNALSLOT(mp_Ui->radioDays_Cleaning, clicked(), this, OnCleaningRMSDays());
    CONNECTSIGNALSLOT(mp_TableWidget, pressed(QModelIndex), this, SelectionChanged(QModelIndex));
    CONNECTSIGNALSLOT(mp_TableWidgetCleaning, pressed(QModelIndex), this, SelectionChangedCleaningTable(QModelIndex));
    CONNECTSIGNALSLOT(this, UpdateReagentList(), &m_ReagentRMSModel, UpdateReagentList());
    CONNECTSIGNALSLOT(this, UpdateReagentList(), &m_ReagentCleaningModel, UpdateReagentList());
    CONNECTSIGNALSLOT(&m_ReagentRMSModel, modelReset(), this, CloseDialogs());
    PopulateReagentList();

    mp_TableWidgetCleaning->setColumnHidden(2, true);
    m_ReagentCleaningModel.ResetAndUpdateModel();
}

/****************************************************************************/
/*!
     *  \brief Destructor
     */
/****************************************************************************/
CReagentRMSWidget::~CReagentRMSWidget()
{
    try {
        delete mp_TableWidget;
        delete mp_Ui;
        delete mp_ModifiyReagentRMSDlg;
        delete mp_TableWidgetCleaning;
    }
    catch (...) {
        // to please Lint.
    }
}

/****************************************************************************/
/*!
 *  \brief Event handler for change events
 *
 *  \iparam p_Event = Change event
 */
/****************************************************************************/
void CReagentRMSWidget::changeEvent(QEvent *p_Event)
{
    QWidget::changeEvent(p_Event);
    switch (p_Event->type()) {
    case QEvent::LanguageChange:
        mp_Ui->retranslateUi(this);
        RetranslateUI();
        break;
    default:
        break;
    }
}
/****************************************************************************/
/*!
 *  \brief Resizes the columns in the reagents table
 */
/****************************************************************************/
void CReagentRMSWidget::ResizeHorizontalSection()
{
    if (mp_TableWidget == NULL && mp_TableWidgetCleaning == NULL)
    {
        return;
    }
    mp_TableWidget->horizontalHeader()->resizeSection(0, 120);
    mp_TableWidget->horizontalHeader()->resizeSection(1, 150);
    mp_TableWidget->horizontalHeader()->resizeSection(2, 140);

    mp_TableWidgetCleaning->horizontalHeader()->resizeSection(0, 120);
    mp_TableWidgetCleaning->horizontalHeader()->resizeSection(1, 150);
    mp_TableWidgetCleaning->horizontalHeader()->resizeSection(2, 140);

}
/****************************************************************************/
/*!
 *  \brief Fills the reagents list with content
 */
/****************************************************************************/
void CReagentRMSWidget::PopulateReagentList()
{
    mp_TableWidget->SetVisibleRows(2,28,26);
    m_ReagentRMSModel.SetVisibleRowCount(2);
    mp_TableWidget->horizontalHeader()->show();
    mp_TableWidget->verticalHeader()->setDefaultSectionSize(26);

    mp_TableWidgetCleaning->SetVisibleRows(2,28,26);
    m_ReagentCleaningModel.SetVisibleRowCount(2);
    mp_TableWidgetCleaning->horizontalHeader()->show();
    mp_TableWidgetCleaning->verticalHeader()->setDefaultSectionSize(26);

    ResizeHorizontalSection();
    m_ReagentCleaningModel.ResetAndUpdateModel();
    m_ReagentRMSModel.ResetAndUpdateModel();
}
/****************************************************************************/
/*!
 *  \brief This slot is used to close all open dialogs of this widget
 */
/****************************************************************************/
void CReagentRMSWidget::CloseDialogs()
{
    ResetButtons();
    if (mp_ModifiyReagentRMSDlg) {
        mp_ModifiyReagentRMSDlg->accept();
    }
}

/****************************************************************************/
/*!
 *  \brief Slot will be called when processing RMS mode changed to Off
 */
/****************************************************************************/
void CReagentRMSWidget::OnRMSOFF()
{
    RMSPROCESSINGOPTION = Global::RMS_OFF;
    mp_TableWidget->setColumnHidden(2, true);
    m_ReagentRMSModel.ResetAndUpdateModel();
    emit RMSChanged(Global::RMS_OFF);
}

/****************************************************************************/
/*!
 *  \brief Slot will be called when processing RMS mode changed to Cassettes
 */
/****************************************************************************/
void CReagentRMSWidget::OnRMSCassettes()
{
    RMSPROCESSINGOPTION = Global::RMS_CASSETTES;
    mp_TableWidget->setColumnHidden(2, false);
    m_ReagentRMSModel.ResetAndUpdateModel();
    ResizeHorizontalSection();
    emit RMSChanged(Global::RMS_CASSETTES);
}

/****************************************************************************/
/*!
 *  \brief Slot will be called when processing RMS mode changed to Cycles
 */
/****************************************************************************/
void CReagentRMSWidget::OnRMSCycles()
{
    RMSPROCESSINGOPTION = Global::RMS_CYCLES;
    mp_TableWidget->setColumnHidden(2, false);
    m_ReagentRMSModel.ResetAndUpdateModel();
    ResizeHorizontalSection();
    emit RMSChanged(Global::RMS_CYCLES);
}

/****************************************************************************/
/*!
 *  \brief Slot will be called when processing RMS mode changed to Days
 */
/****************************************************************************/
void CReagentRMSWidget::OnRMSDays()
{
    RMSPROCESSINGOPTION = Global::RMS_DAYS;
    mp_TableWidget->setColumnHidden(2, false);
    m_ReagentRMSModel.ResetAndUpdateModel();
    ResizeHorizontalSection();
    emit RMSChanged(Global::RMS_DAYS);
}


/****************************************************************************/
/*!
 *  \brief Slot will be called when Cleaning RMS mode changed to Off
 */
/****************************************************************************/
void CReagentRMSWidget::OnCleaningRMSOFF()
{
    RMSCLEANINGOPTIONS = Global::RMS_OFF;
    mp_TableWidgetCleaning->setColumnHidden(2, true);
    m_ReagentCleaningModel.ResetAndUpdateModel();
    ResetButtons();
}
/****************************************************************************/
/*!
 *  \brief Slot will be called when Cleaning RMS mode changed to Cycles
 */
/****************************************************************************/
void CReagentRMSWidget::OnCleaningRMSCycles()
{
    RMSCLEANINGOPTIONS = Global::RMS_CYCLES;
    mp_TableWidgetCleaning->setColumnHidden(2, false);
    ResizeHorizontalSection();
    m_ReagentCleaningModel.ResetAndUpdateModel();
    ResetButtons();
}

/****************************************************************************/
/*!
 *  \brief Slot will be called when Cleaning RMS mode changed to Days
 */
/****************************************************************************/
void CReagentRMSWidget::OnCleaningRMSDays()
{
    RMSCLEANINGOPTIONS = Global::RMS_DAYS;
    mp_TableWidgetCleaning->setColumnHidden(2, false);
    ResizeHorizontalSection();
    m_ReagentCleaningModel.ResetAndUpdateModel();
    ResetButtons();
}

/****************************************************************************/
/*!
 *  \brief Displays a dialog for the modification of a staining reagent
 */
/****************************************************************************/
void CReagentRMSWidget::OnEdit()
{
    m_MessageDlg.SetText(tr("Staining Process has started, Editing is no longer possible."
                            "\nPlease close the dialog with \"Close\""));
    mp_ModifiyReagentRMSDlg->SetDialogTitle(tr("Edit Reagent"));
    mp_ModifiyReagentRMSDlg->SetButtonType(EDIT_BTN_CLICKED);
    mp_ModifiyReagentRMSDlg->InitDialog(mp_Reagent, mp_DataConnector->ReagentGroupList, Reagents::CReagentRMSWidget::RMSPROCESSINGOPTION);
    mp_ModifiyReagentRMSDlg->show();
}

/****************************************************************************/
/*!
 *  \brief Creates a new staining reagent and opens its edit dialog
 */
/****************************************************************************/
void CReagentRMSWidget::OnNew()
{
    m_MessageDlg.SetText(tr("Staining Process has started, editing is no longer possible."
                            "\nPlease close the dialog with \"Close\""));
    mp_ModifiyReagentRMSDlg->SetDialogTitle(tr("New Reagent"));
    mp_ModifiyReagentRMSDlg->SetButtonType(NEW_BTN_CLICKED);
    mp_Reagent = NULL;
    mp_ModifiyReagentRMSDlg->InitDialog(mp_Reagent, mp_DataConnector->ReagentGroupList,Reagents::CReagentRMSWidget::RMSPROCESSINGOPTION);
    mp_ModifiyReagentRMSDlg->show();
}

/****************************************************************************/
/*!
 *  \brief Deletes the currently selected staining reagent
 */
/****************************************************************************/
void CReagentRMSWidget::OnDelete()
{
    m_MessageDlg.SetText(tr("Staining Process has started, Editing is no longer possible."));
    m_MessageDlg.SetTitle(tr("Confirmation Message"));
    m_MessageDlg.SetIcon(QMessageBox::Information);
    m_MessageDlg.SetButtonText(1, tr("Yes"));
    m_MessageDlg.SetButtonText(3, tr("Cancel"));
    m_MessageDlg.HideCenterButton();
    m_MessageDlg.SetText(tr("Do you really want to delete the selected reagent"));

    if(m_MessageDlg.exec()==(int)QDialog::Accepted){
        emit RemoveReagent(mp_Reagent->GetReagentID());
    }
}

/****************************************************************************/
/*!
 *  \brief This slot is called when Cancel button in ModifyReagentDlg is pressed.
 */
/****************************************************************************/
void CReagentRMSWidget::OnCancelPressed()
{
    mp_TableWidget->clearSelection();
    ResetButtons();
}

/****************************************************************************/
/*!
 *  \brief Sets the settings data object for this class
 *
 *  \iparam p_UserSettings = Global user settings data
 */
/****************************************************************************/
void CReagentRMSWidget::SetUserSettings(DataManager::CUserSettings *p_UserSettings)
{
    mp_UserSettings = p_UserSettings;
    m_ReagentRMSModel.SetUserSettings(p_UserSettings);
}

/****************************************************************************/
/*!
 *  \brief Translates the strings in UI to the selected language
 */
/****************************************************************************/
//void CReagentRMSWidget::RetranslateUI()
//{
//   MainMenu::CPanelFrame::SetPanelTitle(QApplication::translate("Reagents::CReagentRMSWidget", "Reagents", 0, QApplication::UnicodeUTF8));
//}

/****************************************************************************/
/*!
 *  \brief Called when the selection of the table is changed
 *
 *  \iparam Index = Model Index
 */
/****************************************************************************/
void CReagentRMSWidget::SelectionChanged(QModelIndex Index)
{
    mp_TableWidgetCleaning->clearSelection();
    QString Id = m_ReagentRMSModel.data(Index, (int)Qt::UserRole).toString();
    // change the background color when the row is selected for the reagent group column

    UpdateButtons(Id);
}


/****************************************************************************/
/*!
 *  \brief Called when the selection of the table is changed
 *
 *  \iparam Index = Model Index
 */
/****************************************************************************/
void CReagentRMSWidget::SelectionChangedCleaningTable(QModelIndex Index)
{
    mp_TableWidget->clearSelection();
    QString Id = m_ReagentCleaningModel.data(Index, (int)Qt::UserRole).toString();
    // change the background color when the row is selected for the reagent group column
    UpdateButtons(Id);
}

/****************************************************************************/
/*!
 *  \brief Called when the selection of the table is changed
 *
 *  \iparam Index = Reagent ID
 */
/****************************************************************************/
void CReagentRMSWidget::UpdateButtons(QString Id)
{
    mp_Reagent = mp_ReagentList->GetReagent(Id);
    if (mp_Reagent) {
        if ((m_CurrentUserRole == MainMenu::CMainWindow::Admin ||
             m_CurrentUserRole == MainMenu::CMainWindow::Service) &&
                (!m_ProcessRunning)) {
            //Edit Mode
            mp_Ui->btnNew->setEnabled(true);
            mp_Ui->btnEdit->setEnabled(true);
            mp_Ui->btnDelete->setEnabled(true);

        }
        else {
            mp_Ui->btnNew->setEnabled(false);
            mp_Ui->btnDelete->setEnabled(false);
        }

        if (mp_DataConnector->ProgramList->GetReagentIDList().contains(Id))
        {
            mp_Ui->btnDelete->setEnabled(false);
        }
    }
}
/****************************************************************************/
/*!
 *  \brief This slot is called when User Role changes
 */
/****************************************************************************/
void CReagentRMSWidget::OnUserRoleChanged()
{
    m_CurrentUserRole = MainMenu::CMainWindow::GetCurrentUserRole();
    m_ProcessRunning = MainMenu::CMainWindow::GetProcessRunningStatus();
    if ((m_CurrentUserRole == MainMenu::CMainWindow::Admin ||
         m_CurrentUserRole == MainMenu::CMainWindow::Service) &&
            (!m_ProcessRunning)) {
        //Edit Mode
        mp_Ui->btnNew->setEnabled(true);
        mp_Ui->groupRMS->setEnabled(true);
        mp_Ui->CleaningReagent_GroupRMS->setEnabled(true);
    }
    else {
        mp_Ui->btnNew->setEnabled(false);
        mp_Ui->groupRMS->setEnabled(false);
        mp_Ui->CleaningReagent_GroupRMS->setEnabled(false);
    }
    m_UserRoleChanged = true;
}

/****************************************************************************/
/*!
 *  \brief This slot is called when Process state changes
 */
/****************************************************************************/
void CReagentRMSWidget::OnProcessStateChanged()
{
    m_CurrentUserRole = MainMenu::CMainWindow::GetCurrentUserRole();
    m_ProcessRunning = MainMenu::CMainWindow::GetProcessRunningStatus();
    if ((m_CurrentUserRole == MainMenu::CMainWindow::Admin ||
         m_CurrentUserRole == MainMenu::CMainWindow::Service) &&
            (!m_ProcessRunning)) {

        mp_Ui->btnNew->setEnabled(true);
        mp_Ui->btnEdit->setEnabled(true);
        mp_Ui->btnDelete->setEnabled(true);
        mp_Ui->groupRMS->setEnabled(true);
        mp_Ui->CleaningReagent_GroupRMS->setEnabled(true);

    }
    else {
        mp_Ui->btnNew->setEnabled(false);
        mp_Ui->btnEdit->setEnabled(false);
        mp_Ui->btnDelete->setEnabled(false);
        mp_Ui->groupRMS->setEnabled(false);
        mp_Ui->CleaningReagent_GroupRMS->setEnabled(false);

        if(m_ProcessRunning && m_ShowMessageDialog){
            m_MessageDlg.SetTitle(tr("Information Message"));
            m_MessageDlg.SetIcon(QMessageBox::Information);
            m_MessageDlg.SetButtonText(1, tr("Ok"));
            m_MessageDlg.HideButtons();
            (void)m_MessageDlg.exec();
        }
    }
}

/****************************************************************************/
/*!
 *  \brief This resets the buttons on the right of the screen
 */
/****************************************************************************/
void CReagentRMSWidget::ResetButtons()
{
    m_CurrentUserRole = MainMenu::CMainWindow::GetCurrentUserRole();
    if ((m_CurrentUserRole == MainMenu::CMainWindow::Admin ||
         m_CurrentUserRole == MainMenu::CMainWindow::Service) && (!m_ProcessRunning)) {
        //Edit Mode
        mp_Ui->btnNew->setEnabled(true);
        mp_Ui->btnEdit->setEnabled(false);
        mp_Ui->btnDelete->setEnabled(false);
        mp_Ui->groupRMS->setEnabled(true);
        mp_Ui->CleaningReagent_GroupRMS->setEnabled(true);
    }
    else {
        //View Mode
        mp_Ui->btnEdit->setEnabled(false);
        mp_Ui->btnDelete->setEnabled(false);
        mp_Ui->btnNew->setEnabled(false);
        mp_Ui->groupRMS->setEnabled(false);
        mp_Ui->CleaningReagent_GroupRMS->setEnabled(false);
    }
}

/*****************************************************************************/
/*!
  *  \brief Translates the strings in UI to the selected language
  */
/****************************************************************************/
void CReagentRMSWidget::RetranslateUI()
{
    MainMenu::CPanelFrame::SetPanelTitle(QApplication::translate("Reagents::CReagentRMSWidget",
                                                                 "RMS", 0, QApplication::UnicodeUTF8));
    (void) m_ReagentRMSModel.setHeaderData(0,Qt::Horizontal,QApplication::translate("Core::CReagentRMSModel",
                                                                                 "Reagent", 0, QApplication::UnicodeUTF8),0);
    (void) m_ReagentRMSModel.setHeaderData(1,Qt::Horizontal,QApplication::translate("Core::CReagentRMSModel",
                                                                                 "ReagentGroup", 0, QApplication::UnicodeUTF8),0);

    QString SecondColumnName("");
    switch(Reagents:: CReagentRMSWidget::RMSPROCESSINGOPTION) {
        case Global::RMS_CASSETTES:
            SecondColumnName = "Cassettes until change";
            break;
        case Global::RMS_CYCLES:
            SecondColumnName = "Cycles until change";
            break;
        case Global::RMS_DAYS:
            SecondColumnName = "Days until change";
            break;
    }
    (void) m_ReagentRMSModel.setHeaderData(2,Qt::Horizontal,QApplication::translate("Core::CReagentRMSModel",
                                                                                 SecondColumnName.toUtf8(), 0, QApplication::UnicodeUTF8),0);
}

/*
void CReagentRMSWidget ::UpdateUserSetting()
{
    mp_UserSettings = mp_DataConnector->SettingsInterface->GetUserSettings();;
    m_ReagentRMSModel.SetUserSettings(p_UserSettings);
}*/

/****************************************************************************/
/*!
 *  \brief This event is called whenever widget is shown
 */
/****************************************************************************/
void CReagentRMSWidget::showEvent(QShowEvent *)
{
    m_MessageDlg.SetText(tr("Staining Process has started, Editing is no longer possible."));
    m_ShowMessageDialog = true ;
    mp_TableWidget->clearSelection();  
    mp_Ui->scrollTable->Reset();
    m_ReagentRMSModel.UpdateReagentList();

    mp_TableWidgetCleaning->clearSelection();
    m_ReagentCleaningModel.UpdateReagentList();
    if(RMSCLEANINGOPTIONS == Global::RMS_OFF)
        OnCleaningRMSOFF();
    ResetButtons();
}


/****************************************************************************/
/*!
 *  \brief Used to set pointer to mainwindow, used to retreive user role and
 *         process state changed.
 */
/****************************************************************************/
void CReagentRMSWidget::SetPtrToMainWindow(Core::CDataConnector *p_DataConnector,
                                         DataManager::CDataReagentList *p_ReagentList,
                                         MainMenu::CMainWindow *p_MainWindow,
                                         KeyBoard::CKeyBoard *p_KeyBoard)
{
    mp_MainWindow = p_MainWindow;
    mp_DataConnector = p_DataConnector;
    mp_ReagentList = p_ReagentList;
    mp_KeyBoard = p_KeyBoard;
    // set the reagent ist to the model
    m_ReagentRMSModel.SetReagentList(mp_ReagentList, 3);
    m_ReagentRMSModel.SetReagentGroupList(mp_DataConnector->ReagentGroupList);

    m_ReagentCleaningModel.SetReagentList(mp_ReagentList, 3);
    m_ReagentCleaningModel.SetReagentGroupList(mp_DataConnector->ReagentGroupList);
    ResizeHorizontalSection();

    mp_ModifiyReagentRMSDlg = new CModifyReagentRMSDlg(this, mp_KeyBoard, mp_MainWindow, mp_DataConnector);
    mp_ModifiyReagentRMSDlg->setModal(true);

    CONNECTSIGNALSLOT(mp_ModifiyReagentRMSDlg, UpdateReagent(DataManager::CReagent &), mp_DataConnector,
                        SendReagentUpdate(DataManager::CReagent &));
    CONNECTSIGNALSLOT(mp_ModifiyReagentRMSDlg, AddReagent(DataManager::CReagent &) ,
                        mp_DataConnector, SendReagentAdd(DataManager::CReagent &));
    CONNECTSIGNALSLOT(this, RemoveReagent(QString) ,
                      mp_DataConnector, SendReagentRemove(QString));
    //CONNECTSIGNALSLOT(mp_DataConnector,UserSettingsUpdated(),this,UpdateUserSetting());

    CONNECTSIGNALSLOT(mp_ModifiyReagentRMSDlg, CancelPressed(), this, OnCancelPressed());
}

} // end namespace Reagents
