/****************************************************************************/
/*! \file CPressureInputDialog.h
 *
 *  \brief Header file for class CPressureInputDialog.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2014-06-09
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

#ifndef PRESSUREINPUTDIALOG_H
#define PRESSUREINPUTDIALOG_H

#include "MainMenu/Include/DialogFrame.h"
#include "Core/Include/ServiceDefines.h"
#include <QTimer>
#include <KeyBoard/Include/KeyBoard.h>

#include "ServiceDataManager/Include/TestCaseGuide.h"
#include "ServiceDataManager/Include/TestCase.h"
#include "ServiceDataManager/Include/TestCaseFactory.h"

namespace DiagnosticsManufacturing {

namespace Ui {
    class CPressureInputDialog;
}

/****************************************************************************/
/**
 * \brief This dialog is started, when the user has to wait for the result of
 *        a background process.
 */
/****************************************************************************/
class CPressureInputDialog : public MainMenu::CDialogFrame
{
    Q_OBJECT
    friend class  CTestMainMenu;

public:
    /****************************************************************************/
    /*!
     *  \brief Constructor
     *
     *  \iparam p_Parent = Parent widget
     */
    /****************************************************************************/
    explicit CPressureInputDialog(Service::ModuleTestCaseID Id, QWidget *p_Parent = 0);

    /****************************************************************************/
    /*!
     *  \brief Destructor
     */
    /****************************************************************************/
    virtual ~CPressureInputDialog();

    /****************************************************************************/
    /*!
     *  \brief Sets the text displayed in the wait dialog
     *
     *  \iparam Text = Label text
     */
    /****************************************************************************/
    void SetText(QString Text);


    /****************************************************************************/
    /*!
     *  \brief Update label status
     *
     *  \iparam Status = Label test status
     */
    /****************************************************************************/
    void UpdateLabel(const Service::ModuleTestStatus &Status);


    /****************************************************************************/
    /*!
     *  \brief Get input value
     *
     */
    /****************************************************************************/

    QString GetInputValue( quint8 index)
    {
        if (index == 0) {
            return m_LineEditStringLeft;
        }
        else if (index == 1) {
            return m_LineEditStringMiddle;
        }
        else if (index == 2) {
            return m_LineEditStringRight;
        }
        else {
            return "";
        }
    }

    bool GetResult( )
    {
   //     qDebug() <<"GetResult = " << m_PassFlag;
        return m_PassFlag;
    }

private slots:
    /****************************************************************************/
    /*!
     *  \brief This function is called when OK is clicked
     *  \iparam EnteredString = Stores line edit string
     */
    /****************************************************************************/
    void OnOkClicked(QString EnteredString);

    /****************************************************************************/
    /*!
     *  \brief This function hides the keyboard when Esc is clicked
     */
    /****************************************************************************/
    void OnESCClicked();

    /****************************************************************************/
    /*!
     *  \brief Abort Dialog
     */
    /****************************************************************************/
    void AbortDialog();

    /****************************************************************************/
    /*!
     *  \brief Connects signals and slots of keyboard.
     */
    /****************************************************************************/
    void RetranslateUI();

    /****************************************************************************/
    /**
     * \brief This slot is called when the timer time out happens.
     */
    /****************************************************************************/
    void OnTimeout();

protected:
    bool eventFilter(QObject *p_Object, QEvent *p_Event);

private:
    Ui::CPressureInputDialog *mp_Ui;     //!< User interface
    KeyBoard::CKeyBoard *mp_KeyBoardWidget;     //!< Keyboard widget
    QString m_LineEditStringLeft;                   //!< Stores user input value
    QString m_LineEditStringMiddle;                   //!< Stores user input value
    QString m_LineEditStringRight;                   //!< Stores user input value
    QTimer   *m_Timer;                    //!< Timer to remind user input current pressure.
    int     m_Seconds ;
    int     m_WaitSeconds;
    bool    m_PassFlag;                   //!< Test result.
    Service::ModuleTestCaseID m_TestCaseId; //!< Test case Id


    void ConnectKeyBoardSignalSlots();
    void DisconnectKeyBoardSignalSlots();

    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(CPressureInputDialog)

};

} // end namespace DiagnosticsManufacturing

#endif // PRESSUREINPUTDIALOG_H
