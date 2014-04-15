/****************************************************************************/
/*! \file CmdOvenGetRecentTemperature.h
 *
 *  \brief CmdOvenGetRecentTemperature command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 29.03.2013
 *   $Author:  $ L. Yang
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


#ifndef CMDOVENGETRECENTTEMPERATURE_H
#define CMDOVENGETRECENTTEMPERATURE_H

#include "Scheduler/Commands/Include/CmdSchedulerCommandBase.h"

namespace Scheduler{

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of class CmdOvenGetRecentTemperature
 */
/****************************************************************************/
class CmdOvenGetRecentTemperature : public CmdSchedulerCommandBase
{
public:
    CmdOvenGetRecentTemperature(int Timeout, SchedulerMainThreadController *controller);
    ~CmdOvenGetRecentTemperature();

    static QString NAME;    ///< Command name.


    QString GetName() const{return NAME;}

    qreal GetResult()const {return m_result;}
	void SetResult(qreal result) { m_result = result;}
	bool GetResult(qreal& result) const{result = m_result; return true;}
	DeviceControl::OVENTempCtrlType_t GetType(){return m_Type;}
	void SetType(DeviceControl::OVENTempCtrlType_t Type){m_Type = Type;}
	quint8 GetIndex(){return m_Index;}
	void SetIndex(quint8 Index){m_Index = Index;}
	
	QString GetParameters()const{ return GetName() + QString("(%1,%2)").arg(m_Type).arg(m_Index);}
	QString GetStrResult()const{ return QString("%1").arg(m_result);}

private:
    CmdOvenGetRecentTemperature();                                                    ///< Not implemented.
    CmdOvenGetRecentTemperature(const CmdOvenGetRecentTemperature &);                     ///< Not implemented.
    const CmdOvenGetRecentTemperature & operator = (const CmdOvenGetRecentTemperature &); ///< Not implemented.


	mutable qreal m_result;
	mutable DeviceControl::OVENTempCtrlType_t m_Type;
	mutable quint8 m_Index;
	
};



}

#endif // CMDOVENGETRECENTTEMPERATURE_H
