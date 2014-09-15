/****************************************************************************/
/*! \file HeatingTestEmpty.h
 *
 *  \brief Declaration of Retort Heating test empty.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2013-05-29
 *   $Author:  $ R.Wu
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

#ifndef DIAGNOSTICS_RETORT_HEATINGTESTEMPTY_H
#define DIAGNOSTICS_RETORT_HEATINGTESTEMPTY_H

#include "Diagnostics/Include/TestBase.h"

namespace Diagnostics {

namespace Retort {

class CHeatingTestEmpty : public Diagnostics::CTestBase
{
    Q_OBJECT

public:
    CHeatingTestEmpty();
    ~CHeatingTestEmpty(void);

    int Run(void);

public Q_SLOTS:
    void StartHeating(void);

private Q_SLOTS:
    void Succeed(void);

    void Fail(void);
};

} // namespace Retort

} // namespace Diagnostics

#endif // DIAGNOSTICS_RETORT_HEATINGTESTEMPTY_H
