/****************************************************************************/
/*! \file CmdParaffinBathStatus.h
 *
 *  \brief CmdParaffinBathStatus command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 27.03.2013
 *   $Author:  $ Abe Yang
 *
 *  \b Company:
 *
  *       Leica Microsystems Ltd. Shanghai.
 *
 *  (C) Copyright 2013 by Leica Microsystems Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#ifndef MSGCLASSES_CMDPARAFFINBATHSTATUS_H
#define MSGCLASSES_CMDPARAFFINBATHSTATUS_H

#include <Global/Include/Commands/Command.h>
#include <HimalayaDataContainer/Helper/Include/Global.h>

namespace MsgClasses {

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdParaffinBathStatus command.
 *
 * \todo implement
 */
/****************************************************************************/
class CmdParaffinBathStatus : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdParaffinBathStatus &);
    friend QDataStream & operator >> (QDataStream &, CmdParaffinBathStatus &);
public:
    CmdParaffinBathStatus();                                             ///< Not implemented.

    static QString NAME;    ///< Command name.
    /****************************************************************************/
    CmdParaffinBathStatus(int Timeout, const QString& ParaffinBathID, DataManager::ParaffinBathStatusType_t ParaffinBathStatusType);
    ~CmdParaffinBathStatus();
    virtual QString GetName() const;
    inline DataManager::ParaffinBathStatusType_t ParaffinBathStatusType() const {return m_ParaffinBathStatusType;} 
    inline const QString& ParaffinBathID()const {return m_ParaffinBathID;}
private:
    CmdParaffinBathStatus(const CmdParaffinBathStatus &);                     ///< Not implemented.
    const CmdParaffinBathStatus & operator = (const CmdParaffinBathStatus &); ///< Not implemented.
private:
    QString m_ParaffinBathID;
    DataManager::ParaffinBathStatusType_t m_ParaffinBathStatusType;
    
}; // end class CmdParaffinBathStatus

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \param[in]       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdParaffinBathStatus &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << Cmd.m_ParaffinBathID;
    Stream << Cmd.m_ParaffinBathStatusType;
    return Stream;
}

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream from.
 * \param[in]       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator >> (QDataStream &Stream, CmdParaffinBathStatus &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_ParaffinBathID;
    int temp;
    Stream >> temp;
    Cmd.m_ParaffinBathStatusType = (DataManager::ParaffinBathStatusType_t)temp;
    return Stream;
}
} // end namespace MsgClasses

#endif // MSGCLASSES_CMDPARAFFINBATHSTATUS_H
