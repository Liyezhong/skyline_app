/****************************************************************************/
/*! \file SystemLogViewerDlg.cpp
 *
 *  \brief System Log Viewer implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2014-05-22
 *   $Author:  $ Sunny Qu
 *
 *  \b Company:
 *
 *       Leica Biosystems Shanghai.
 *
 *  (C) Copyright 2010 by Leica Biosystems Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#include <QTextStream>
#include <QDebug>

#include "Main/Include/HimalayaServiceEventCodes.h"
#include "Global/Include/SystemPaths.h"
#include "LogViewerDialog/Include/SystemLogViewerDlg.h"
#include "ui_SystemLogViewerDlg.h"
#include "Global/Include/GlobalDefines.h"
#include "LogViewerDialog/Include/RecoveryActionFilter.h"
#include "Global/Include/EventObject.h"

namespace LogViewer {

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent widget
 */
/****************************************************************************/
CSystemLogViewerDlg::CSystemLogViewerDlg(QWidget *p_Parent) : MainMenu::CDialogFrame(p_Parent), mp_Ui(new Ui::CSystemLogViewerDlg),
    mp_LogFilter(NULL),
    mp_RecoveryActionFilter(NULL)
{
    RetranslateUI();
    m_EventTypes = LogViewer::CLogFilter::m_AllTypes;

    mp_Ui->setupUi(GetContentFrame());
    m_LogFilePath = "";
    mp_TableWidget = new MainMenu::CBaseTable;
    mp_TableWidget->resize(600,330);

    mp_TableWidget->horizontalHeader()->show();

    mp_TableWidget->horizontalHeader()->resizeSection(0, 95);
    mp_TableWidget->horizontalHeader()->resizeSection(3, 75);
    mp_TableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    mp_Ui->widget->setMinimumSize(mp_TableWidget->width(), mp_TableWidget->height());
    mp_Ui->widget->SetContent(mp_TableWidget);

    mp_RecoveryActionDlg = new MainMenu::CTextDialog(this);
    mp_RecoveryActionDlg->SetCaption(tr(""));
    mp_Ui->allBtn->setChecked(true);
    ResetButtons(false);
    mp_Ui->showDetailsBtn->setEnabled(false);

    mp_Ui->recoveryActionBtn->setEnabled(false);

    mp_MessageDlg = new MainMenu::CMessageDlg(this);

    (void)connect(mp_Ui->allBtn, SIGNAL(clicked()), this, SLOT(CompleteLogInfo()));
    (void)connect(mp_Ui->errorBtn, SIGNAL(clicked()), this, SLOT(FilteredErrorLog()));
    (void)connect(mp_Ui->infoBtn, SIGNAL(clicked()), this, SLOT(FilteredInfoLog()));
    (void)connect(mp_Ui->undefinedBtn, SIGNAL(clicked()), this, SLOT(FilteredUndefinedLog()));
    (void)connect(mp_Ui->warningBtn, SIGNAL(clicked()), this, SLOT(FilteredWarningLog()));

    (void)connect(mp_TableWidget, SIGNAL(clicked(QModelIndex)), this, SLOT(SelectionChanged(QModelIndex)));
    (void)connect(mp_Ui->showDetailsBtn, SIGNAL(clicked()), this, SLOT(ShowRecoveryActionDetails()));
    (void)connect(mp_Ui->recoveryActionBtn, SIGNAL(clicked()), this, SLOT(RecoveryActionDialog()));
    (void)connect(mp_Ui->closeBtn, SIGNAL(clicked()), this, SLOT(close()));

}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CSystemLogViewerDlg::~CSystemLogViewerDlg()
{
    try {
        delete mp_Ui;
        delete mp_TableWidget;
        delete mp_RecoveryActionDlg;
        delete mp_MessageDlg;
        if (mp_LogFilter) {
            delete mp_LogFilter;
        }
        if (mp_RecoveryActionFilter) {
            delete mp_RecoveryActionFilter;
        }
        m_SelectedRowValues.clear();
    }
    catch (...) {
        // to please Lint
    }
}

void CSystemLogViewerDlg::SelectionChanged(QModelIndex Index)
{
    Q_UNUSED(Index);

    QItemSelectionModel* SelectionModel = mp_TableWidget->selectionModel();
    m_SelectedRowValues = SelectionModel->selectedIndexes(); //!< list of "selected" items

    QString Type = m_SelectedRowValues.at(3).data((int)Qt::DisplayRole).toString();
    if (Type.compare("Error")==0 || Type.compare("Fatal Error")==0) {
        mp_Ui->recoveryActionBtn->setEnabled(true);
    }
    else {
        mp_Ui->recoveryActionBtn->setEnabled(false);
    }
    mp_Ui->showDetailsBtn->setEnabled(true);
}

void CSystemLogViewerDlg::RecoveryActionDialog()
{
    Global::EventObject::Instance().RaiseEvent(EVENT_GUI_LOGVIEWER_SYSTEMLOG_SERVICERECOVERYACTION_FOR_ERROR);

    if(m_SelectedRowValues.count() == 0)
    {
        mp_MessageDlg->SetTitle(m_strWarning);
        mp_MessageDlg->SetButtonText(1, m_strOK);
        mp_MessageDlg->HideButtons();
        mp_MessageDlg->SetText(m_strMsgSelectRowText);
        mp_MessageDlg->SetIcon(QMessageBox::Warning);
        mp_MessageDlg->show();
    }
    else
    {
        QString Type = m_SelectedRowValues.at(3).data((int)Qt::DisplayRole).toString();
        if ( Type == QString("Info") || Type == QString("Undefined") || Type == QString("Warning")) {
            mp_MessageDlg->SetTitle(m_strMsgRecoveryActionTitle);
            mp_MessageDlg->SetButtonText(1, m_strOK);
            mp_MessageDlg->HideButtons();
            QString Text = m_strMsgRecoveryActionText + Type + ".";
            mp_MessageDlg->SetText(Text);
            mp_MessageDlg->SetIcon(QMessageBox::Warning);
            mp_MessageDlg->show();
        }
        else {
            QString Path = Global::SystemPaths::Instance().GetSettingsPath() + "/RecoveryActionText.txt";

            if (mp_RecoveryActionFilter == NULL) {
                mp_RecoveryActionFilter = new LogViewer::CRecoveryActionFilter(Path);
            }

            QString EventId = m_SelectedRowValues.at(2).data((int)Qt::DisplayRole).toString();
            QString Line = QString(mp_RecoveryActionFilter->GetRecoveryActionText(EventId));
            QStringList List = Line.split(";");
            if (List.size()>0) {
                QString InputText = m_strErrorCode;
                InputText.append(m_strDescription);
                if (List.count() > 1)
                    InputText.append(List.at(1));
                InputText.append("\n\n");
                InputText.append(m_strRecoveryActionText);
                if (List.count() > 2)
                    InputText.append(List.at(2));
                mp_RecoveryActionDlg->SetDialogTitle(m_strRecoveryActionTitle);
                mp_RecoveryActionDlg->resize(428, 428);
                mp_RecoveryActionDlg->SetText(InputText);
                mp_RecoveryActionDlg->show();
            }
            else {
                mp_MessageDlg->SetTitle(m_strMsgServerHelpTitle);
                mp_MessageDlg->SetButtonText(1, m_strOK);
                mp_MessageDlg->HideButtons();
                mp_MessageDlg->SetText(m_strMsgServerHelpText);
                mp_MessageDlg->SetIcon(QMessageBox::Warning);
                mp_MessageDlg->show();

            }
        }
    }
}

void CSystemLogViewerDlg::SetTableModel()
{
    QStringList HeaderLabels;

    mp_Model = mp_LogFilter->GetItemModel(m_EventTypes);
    if (mp_Model->rowCount() == 0)
        return ;

    HeaderLabels.append(m_strDate);
    HeaderLabels.append(m_strTimeStamp);
    HeaderLabels.append(m_strEventID);
    HeaderLabels.append(m_strType);
    HeaderLabels.append(m_strRecoveryActionInfoTitle);

    mp_Model->setHorizontalHeaderLabels(HeaderLabels);


    mp_TableWidget->setModel(mp_Model);

}

void CSystemLogViewerDlg::RetranslateUI()
{
    m_strOK = QApplication::translate("LogViewer::CSystemLogViewerDlg", "OK", 0, QApplication::UnicodeUTF8);
    m_strWarning = QApplication::translate("LogViewer::CSystemLogViewerDlg", "Warning", 0, QApplication::UnicodeUTF8);
    m_strMsgSelectRowText = QApplication::translate("LogViewer::CSystemLogViewerDlg", "Please Select a row to continue..",
                                                    0, QApplication::UnicodeUTF8);
    m_strMsgRecoveryActionTitle = QApplication::translate("LogViewer::CSystemLogViewerDlg", "Recovery Action",
                                                          0, QApplication::UnicodeUTF8);
    m_strMsgRecoveryActionText = QApplication::translate("LogViewer::CSystemLogViewerDlg",
                                                         "Recovery Action is not applicable for the type ",
                                                         0, QApplication::UnicodeUTF8);
    m_strErrorCode = QApplication::translate("LogViewer::CSystemLogViewerDlg", "Error Code: ", 0, QApplication::UnicodeUTF8);
    m_strDescription = QApplication::translate("LogViewer::CSystemLogViewerDlg", "Description: ",
                                               0, QApplication::UnicodeUTF8);
    m_strRecoveryActionText = QApplication::translate("LogViewer::CSystemLogViewerDlg",
                                                      "Recovery Action Text", 0, QApplication::UnicodeUTF8);
    m_strRecoveryActionTitle = QApplication::translate("LogViewer::CSystemLogViewerDlg",
                                                       "Recovery Action Text", 0, QApplication::UnicodeUTF8);
    m_strMsgServerHelpTitle = QApplication::translate("LogViewer::CSystemLogViewerDlg",
                                                      "Service Help Text", 0, QApplication::UnicodeUTF8);
    m_strMsgServerHelpText = QApplication::translate("LogViewer::CSystemLogViewerDlg",
                                                     "Recovery Action Text is not available.",
                                                     0, QApplication::UnicodeUTF8);
    m_strDate = QApplication::translate("LogViewer::CSystemLogViewerDlg", "Date", 0, QApplication::UnicodeUTF8);
    m_strTimeStamp = QApplication::translate("LogViewer::CSystemLogViewerDlg", "TimeStamp", 0, QApplication::UnicodeUTF8);
    m_strEventID = QApplication::translate("LogViewer::CSystemLogViewerDlg", "Event ID", 0, QApplication::UnicodeUTF8);
    m_strType = QApplication::translate("LogViewer::CSystemLogViewerDlg", "Type", 0, QApplication::UnicodeUTF8);
    m_strRecoveryActionInfoTitle = QApplication::translate("LogViewer::CSystemLogViewerDlg", "Log Information",
                                                           0, QApplication::UnicodeUTF8);
}

bool CSystemLogViewerDlg::InitDialog(QString Path)
{
    Global::EventObject::Instance().RaiseEvent(EVENT_GUI_LOGVIEWER_SYSTEMLOG_DETAILED_INFO);
    QList<int> Columns;
    Columns.append(0);
    Columns.append(1);
    Columns.append(2);
    Columns.append(3);

    m_LogFilePath = Path;
    mp_LogFilter = new CLogFilter(Path, Columns, true);
    if (mp_LogFilter->InitData() == false )
    {
        return false;
    }
    SetTableModel();
    return true;
}

void CSystemLogViewerDlg::changeEvent(QEvent *p_Event)
{
    QWidget::changeEvent(p_Event);
    switch (p_Event->type()) {
        case QEvent::LanguageChange:
            this->RetranslateUI();
            break;
        default:
            break;
    }
}

void CSystemLogViewerDlg::ShowRecoveryActionDetails()
{
    Global::EventObject::Instance().RaiseEvent(EVENT_GUI_LOGVIEWER_SERVICERECOVERYACTION_DISPLAY_INFO);
    if(m_SelectedRowValues.count() == 0)
    {
        mp_MessageDlg->SetTitle(m_strWarning);
        mp_MessageDlg->SetButtonText(1, m_strOK);
        mp_MessageDlg->HideButtons();
        mp_MessageDlg->SetText(m_strMsgSelectRowText);
        mp_MessageDlg->SetIcon(QMessageBox::Warning);
        mp_MessageDlg->show();
    }
    else
    {
        QString Text;
        for(int i=0; i<m_SelectedRowValues.count(); i++)
        {
            Text.append(m_SelectedRowValues.at(i).data((int)Qt::DisplayRole).toString());
            Text.append("\n\n");
        }
        mp_RecoveryActionDlg->SetDialogTitle(m_strRecoveryActionInfoTitle);
        mp_RecoveryActionDlg->SetText(Text);
        mp_RecoveryActionDlg->resize(428,428);
        mp_RecoveryActionDlg->show();
    }
}

void CSystemLogViewerDlg::ResetButtons(bool EnableFlag)
{
    mp_Ui->errorBtn->setEnabled(EnableFlag);
    mp_Ui->infoBtn->setEnabled(EnableFlag);
    mp_Ui->warningBtn->setEnabled(EnableFlag);
    mp_Ui->undefinedBtn->setEnabled(EnableFlag);

    mp_Ui->errorBtn->setChecked(false);
    mp_Ui->infoBtn->setChecked(false);
    mp_Ui->warningBtn->setChecked(false);
    mp_Ui->undefinedBtn->setChecked(false);
}

void CSystemLogViewerDlg::CompleteLogInfo()
{
    mp_Ui->showDetailsBtn->setEnabled(false);
    mp_Ui->recoveryActionBtn->setEnabled(false);
    mp_TableWidget->clearSelection();

    if (mp_Ui->allBtn->isChecked()) {
        ResetButtons(false);

        m_EventTypes = LogViewer::CLogFilter::m_AllTypes;
        SetTableModel();
    }
    else {
        ResetButtons(true);
        mp_TableWidget->setModel(NULL);
        m_EventTypes = 0;
    }

}

void CSystemLogViewerDlg::FilteredErrorLog()
{
    Global::EventObject::Instance().RaiseEvent(EVENT_GUI_LOGVIEWER_SYSTEMLOG_FILTERERING,
                                               Global::tTranslatableStringList() << "Error");
    mp_Ui->showDetailsBtn->setEnabled(false);
    mp_Ui->recoveryActionBtn->setEnabled(false);
    mp_TableWidget->clearSelection();

    mp_TableWidget->clearSelection();
    if (mp_Ui->errorBtn->isChecked()) {
        m_EventTypes |= (1<<(int)Global::EVTTYPE_ERROR);
        m_EventTypes |= (1<<(int)Global::EVTTYPE_FATAL_ERROR);
    }
    else {
        m_EventTypes &= (0xFF-(1<<(int)Global::EVTTYPE_ERROR));
        m_EventTypes &= (0xFF-(1<<(int)Global::EVTTYPE_FATAL_ERROR));
    }
    if (m_EventTypes == 0) {
        mp_TableWidget->setModel(NULL);
    }
    else {
       SetTableModel();
    }
}

void CSystemLogViewerDlg::FilteredInfoLog()
{
    Global::EventObject::Instance().RaiseEvent(EVENT_GUI_LOGVIEWER_SYSTEMLOG_FILTERERING,
                                               Global::tTranslatableStringList() << "Info");
    mp_Ui->showDetailsBtn->setEnabled(false);
    mp_Ui->recoveryActionBtn->setEnabled(false);
    mp_TableWidget->clearSelection();

    if (mp_Ui->infoBtn->isChecked()) {
        m_EventTypes |= (1<<(int)Global::EVTTYPE_INFO);
    }
    else {
        m_EventTypes &= (0xFF-(1<<(int)Global::EVTTYPE_INFO));
    }
    if (m_EventTypes == 0) {
        mp_TableWidget->setModel(NULL);
    }
    else {
       SetTableModel();
    }
}

void CSystemLogViewerDlg::FilteredUndefinedLog()
{
    Global::EventObject::Instance().RaiseEvent(EVENT_GUI_LOGVIEWER_SYSTEMLOG_FILTERERING,
                                               Global::tTranslatableStringList() << "Undefined");
    mp_Ui->showDetailsBtn->setEnabled(false);
    mp_Ui->recoveryActionBtn->setEnabled(false);
    mp_TableWidget->clearSelection();

    if (mp_Ui->undefinedBtn->isChecked()) {
        m_EventTypes |= (1<<(int)Global::EVTTYPE_UNDEFINED);
    }
    else {
        m_EventTypes &= (0xFF-(1<<(int)Global::EVTTYPE_UNDEFINED));
    }
    if (m_EventTypes == 0) {
        mp_TableWidget->setModel(NULL);
    }
    else {
       SetTableModel();
    }
}

void CSystemLogViewerDlg::FilteredWarningLog()
{
    Global::EventObject::Instance().RaiseEvent(EVENT_GUI_LOGVIEWER_SYSTEMLOG_FILTERERING,
                                               Global::tTranslatableStringList() << "Warning");
    mp_Ui->showDetailsBtn->setEnabled(false);
    mp_Ui->recoveryActionBtn->setEnabled(false);
    mp_TableWidget->clearSelection();

    if (mp_Ui->warningBtn->isChecked()) {
        m_EventTypes |= (1<<(int)Global::EVTTYPE_WARNING);
    }
    else {
        m_EventTypes &= (0xFF-(1<<(int)Global::EVTTYPE_WARNING));
    }
    if (m_EventTypes == 0) {
        mp_TableWidget->setModel(NULL);
    }
    else {
       SetTableModel();
    }
}
}
