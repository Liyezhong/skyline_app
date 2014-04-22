/****************************************************************************/
/*! \file DashboardScene.h
 *
 *  \brief DashboardScene definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 17-Jan-2013
 *   $Author:  $ Santhosha Kumar Raju
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

#ifndef DASHBOARD_DASHBOARDSCENE_H
#define DASHBOARD_DASHBOARDSCENE_H

#include "Core/Include/ReagentRMSModel.h"
#include "Core/Include/DashboardStationItem.h"
#include "MainMenu/Include/PanelFrame.h"
#include "MainMenu/Include/MainWindow.h"
#include <QGraphicsScene>

#include <HimalayaDataContainer/Helper/Include/Global.h>

namespace Core {
    class CDataConnector;
}

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of class PipePathAndOrientation
 */
/****************************************************************************/
struct PipePathAndOrientation
{
    PipePathAndOrientation()
    {

    }

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function PipePathAndOrientation
     *
     *  \return from PipePathAndOrientation
     */
    /****************************************************************************/
    PipePathAndOrientation(const QPainterPath& painterPath, const QString& orientation, const QPoint& brushOrigin)
    {
        m_PainterPath = painterPath;
        m_orientation = orientation;
        m_brushOrigin = brushOrigin;
    }

    QPainterPath m_PainterPath;       ///<  Definition/Declaration of variable m_PainterPath
    QString m_orientation;       ///<  Definition/Declaration of variable m_orientation
    QPoint m_brushOrigin;       ///<  Definition/Declaration of variable m_brushOrigin
};

namespace Dashboard {

/****************************************************************************/
/**
 * \brief This class is the graphics scene for the bath layout.
 */
/****************************************************************************/
class CDashboardScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit CDashboardScene(Core::CDataConnector *p_DataConnector,
                        QObject *p_Parent = NULL,
                        MainMenu::CMainWindow *p_MainWindow = NULL);
    virtual ~CDashboardScene();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function UpdateRetortStatus
     *
     *  \return from UpdateRetortStatus
     */
    /****************************************************************************/
    void UpdateRetortStatus(DataManager::ContainerStatusType_t retortStatusType);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function UpdateRetortLockedStatus
     *
     *  \return from UpdateRetortLockedStatus
     */
    /****************************************************************************/
    void UpdateRetortLockedStatus(bool locked);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function OnPauseStationSuckDrain
     *
     *  \return from OnPauseStationSuckDrain
     */
    /****************************************************************************/
    void OnPauseStationSuckDrain();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function OnStationSuckDrain
     *
     *  \return from OnStationSuckDrain
     */
    /****************************************************************************/
    void OnStationSuckDrain(const QString& StationId, bool IsStart, bool IsSuck);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function HaveExpiredReagent
     *
     *  \return from HaveExpiredReagent
     */
    /****************************************************************************/
    bool HaveExpiredReagent();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ExpiredReagentStationBlinking
     *
     *  \return from ExpiredReagentStationBlinking
     */
    /****************************************************************************/
    void ExpiredReagentStationBlinking(bool bStart);
protected:

private:
    int m_CurTabIndex;       ///<  Definition/Declaration of variable m_CurTabIndex
    Core::CDataConnector *mp_DataConnector;                                          //!< Global data container
    MainMenu::CMainWindow *mp_MainWindow;                                            //!< Reference to main window.
    QList<Core::CDashboardStationItem *> mp_DashboardStationItems;                   //!< list of station items
    Core::CDashboardStationItem *mp_DashboardStationRetort;       ///<  Definition/Declaration of variable mp_DashboardStationRetort
    QList<QPointF> m_DashboardStationItemPositions;                                  //!< list of station item positions
    QHash<QString, DataManager::CDashboardStation*> m_DashboardStationList;          //!< Hash table of Stations
    QHash<QString, QPointF> m_StationJointList;          //!< Hash table of Station joints
    QHash<QString, PipePathAndOrientation> m_PipeRectList;       ///<  Definition/Declaration of variable m_PipeRectList

    QStringList m_DashboardStationIDs;                                               //!< StationIds list
    QList<StationGroupType_t> m_DashboardStationGroup;                               //!< StationGroup list
    QList<QString> m_DashboardStationLabels;       ///<  Definition/Declaration of variable m_DashboardStationLabels

    QString m_CurrentReagentColorValue;       ///<  Definition/Declaration of variable m_CurrentReagentColorValue
    QStringList m_PipeListP1;       ///<  Definition/Declaration of variable m_PipeListP1
    QStringList m_PipeListP2;       ///<  Definition/Declaration of variable m_PipeListP2
    QStringList m_PipeListP3;       ///<  Definition/Declaration of variable m_PipeListP3

    QStringList m_PipeOrientationList;       ///<  Definition/Declaration of variable m_PipeOrientationList

    QStringList m_PipeListS1;       ///<  Definition/Declaration of variable m_PipeListS1
    QStringList m_PipeListS2;       ///<  Definition/Declaration of variable m_PipeListS2
    QStringList m_PipeListS3;       ///<  Definition/Declaration of variable m_PipeListS3
    QStringList m_PipeListS4;       ///<  Definition/Declaration of variable m_PipeListS4
    QStringList m_PipeListS5;       ///<  Definition/Declaration of variable m_PipeListS5
    QStringList m_PipeListS6;       ///<  Definition/Declaration of variable m_PipeListS6

    QStringList m_PipeListS7;       ///<  Definition/Declaration of variable m_PipeListS7
    QStringList m_PipeListS8;       ///<  Definition/Declaration of variable m_PipeListS8
    QStringList m_PipeListS9;       ///<  Definition/Declaration of variable m_PipeListS9
    QStringList m_PipeListS10;       ///<  Definition/Declaration of variable m_PipeListS10
    QStringList m_PipeListS11;       ///<  Definition/Declaration of variable m_PipeListS11
    QStringList m_PipeListS12;       ///<  Definition/Declaration of variable m_PipeListS12
    QStringList m_PipeListS13;       ///<  Definition/Declaration of variable m_PipeListS13
    bool m_IsSuck;       ///<  Definition/Declaration of variable m_IsSuck
    bool m_bProcessRunning;       ///<  Definition/Declaration of variable m_bProcessRunning
    QTimer* m_pPipeAnimationTimer;       ///<  Definition/Declaration of variable m_pPipeAnimationTimer
    QTimer* m_pBlinkingIntervalTimer;       ///<  Definition/Declaration of variable m_pBlinkingIntervalTimer
    QTimer* m_pStartBlinkingTimer;       ///<  Definition/Declaration of variable m_pStartBlinkingTimer
    QGraphicsPathItem* m_pWorkingPipeGraphicsRectItem;       ///<  Definition/Declaration of variable m_pWorkingPipeGraphicsRectItem
    QList<QGraphicsPathItem*> m_pGraphicsPathItemPipeList;       ///<  Definition/Declaration of variable m_pGraphicsPathItemPipeList
    QGraphicsPathItem* m_WholePipeGraphicsRectItem;       ///<  Definition/Declaration of variable m_WholePipeGraphicsRectItem
    QGraphicsPathItem* m_UsedPipeGraphicsRectItem;       ///<  Definition/Declaration of variable m_UsedPipeGraphicsRectItem
    int m_currentTimerOrder;       ///<  Definition/Declaration of variable m_currentTimerOrder
    QString m_SuckDrainStationId;       ///<  Definition/Declaration of variable m_SuckDrainStationId
    QList<QString> m_SelectedStationList;       ///<  Definition/Declaration of variable m_SelectedStationList
    void InitDashboardStationIDs();
    void InitDashboardStationItemsPositions();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function InitStationConnectedPipeList
     *
     *  \return from InitStationConnectedPipeList
     */
    /****************************************************************************/
    void InitStationConnectedPipeList();
    void InitDashboardStationGroups();
    void InitDashboardStationLabels();
    void AddDashboardStationItemsToScene();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function AddGraphicsProxyWidgetsToScene
     *
     *  \return from AddGraphicsProxyWidgetsToScene
     */
    /****************************************************************************/
    void AddGraphicsProxyWidgetsToScene();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function AddPathLeftTopArc
     *
     *  \return from AddPathLeftTopArc
     */
    /****************************************************************************/
    void AddPathLeftTopArc(QPainterPath& path, QPointF& pnt);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function AddPathRightTopArc
     *
     *  \return from AddPathRightTopArc
     */
    /****************************************************************************/
    void AddPathRightTopArc(QPainterPath& path, QPointF& pnt);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function MakeHorizontalPipePath
     *
     *  \return from MakeHorizontalPipePath
     */
    /****************************************************************************/
    QPainterPath MakeHorizontalPipePath(const QString& stationID, const QString& otherStationID, bool bShooter = false);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function MakeHorizontalBinaryPipePath
     *
     *  \return from MakeHorizontalBinaryPipePath
     */
    /****************************************************************************/
    QPainterPath MakeHorizontalBinaryPipePath(const QString& stationID, const QString& OtherStationID,
                                        const QString& MidTopStationID, bool IsReturnLeftOne, bool bShortor = false);//IsReturnLeftOne is false ,it will return the right one
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CollectPipeRect
     *
     *  \return from CollectPipeRect
     */
    /****************************************************************************/
    void CollectPipeRect();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CreateAllPipe
     *
     *  \return from CreateAllPipe
     */
    /****************************************************************************/
    void CreateAllPipe();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetPipeList
     *
     *  \return from GetPipeList
     */
    /****************************************************************************/
    QStringList* GetPipeList(const QString& StationID);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RepresentCurrentWorkingPipe
     *
     *  \return from RepresentCurrentWorkingPipe
     */
    /****************************************************************************/
    void RepresentCurrentWorkingPipe(const QString& StationID);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ClearCurrentWorkingPipe
     *
     *  \return from ClearCurrentWorkingPipe
     */
    /****************************************************************************/
    void ClearCurrentWorkingPipe();

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function RepresentUsedPipe
     *
     *  \return from RepresentUsedPipe
     */
    /****************************************************************************/
    void RepresentUsedPipe(const QList<QString>& selectedStationList, bool isRunning);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ClearUsedPipe
     *
     *  \return from ClearUsedPipe
     */
    /****************************************************************************/
    void ClearUsedPipe();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function EnableBlink
     *
     *  \return from EnableBlink
     */
    /****************************************************************************/
    void EnableBlink(bool bEnable);
private slots:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot UpdateReagentList
     */
    /****************************************************************************/
    void UpdateDashboardStations();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot UpdateReagentList
     */
    /****************************************************************************/
    void UpdateDashboardStation(const QString& strStationId);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot UpdateReagentList
     */
    /****************************************************************************/
    void UpdateDashboardSceneReagentsForProgram(QString&, QList<QString>&);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot UpdateReagentList
     */
    /****************************************************************************/
    void PipeSuckDrainAnimation();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot UpdateReagentList
     */
    /****************************************************************************/
    void BlinkingStation();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot UpdateReagentList
     */
    /****************************************************************************/
    void OnUpdateUserSettings();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot UpdateReagentList
     */
    /****************************************************************************/
    void OnInteractStart();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot UpdateReagentList
     */
    /****************************************************************************/
    void OnAppIdle();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot UpdateReagentList
     */
    /****************************************************************************/
    void OnProcessStateChanged();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot UpdateReagentList
     */
    /****************************************************************************/
    void OnCurrentTabChanged(int);

};

} // end namespace Dashboard

#endif // DASHBOARD_DASHBOARDSCENE_H
