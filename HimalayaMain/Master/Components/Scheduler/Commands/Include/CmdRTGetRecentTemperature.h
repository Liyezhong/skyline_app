/****************************************************************************/
/*! \file CmdRTGetRecentTemperature.h
 *
 *  \brief CmdRTGetRecentTemperature command definition.
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


#ifndef CMDRTGETRECENTTEMPERATURE_H
#define CMDRTGETRECENTTEMPERATURE_H

#include "Scheduler/Commands/Include/CmdSchedulerCommandBase.h"

namespace Scheduler{

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of class CmdRTGetRecentTemperature
 */
/****************************************************************************/
class CmdRTGetRecentTemperature : public CmdSchedulerCommandBase
{
public:
    CmdRTGetRecentTemperature(int Timeout, SchedulerMainThreadController *controller);
    ~CmdRTGetRecentTemperature();

    static QString NAME;    ///< Command name.


    QString GetName() const{return NAME;}

    qreal GetResult()const {return m_result;}
	void SetResult(qreal result) { m_result = result;}
	bool GetResult(qreal& result) const{result = m_result; return true;}
	DeviceControl::RTTempCtrlType_t GetType(){return m_Type;}
	void SetType(DeviceControl::RTTempCtrlType_t Type){m_Type = Type;}
	quint8 GetIndex(){return m_Index;}
	void SetIndex(quint8 Index){m_Index = Index;}
	
	QString GetParameters()const{ return GetName() + QString("(%1,%2)").arg(m_Type).arg(m_Index);}
	QString GetStrResult()const{ return QString("%1").arg(m_result);}

private:
    CmdRTGetRecentTemperature();                                                    ///< Not implemented.
    CmdRTGetRecentTemperature(const CmdRTGetRecentTemperature &);                     ///< Not implemented.
    const CmdRTGetRecentTemperature & operator = (const CmdRTGetRecentTemperature &); ///< Not implemented.


	mutable qreal m_result;
	mutable DeviceControl::RTTempCtrlType_t m_Type;
	mutable quint8 m_Index;
	
};



}

#endif // CMDRTGETRECENTTEMPERATURE_H
