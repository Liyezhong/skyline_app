#include "Scheduler/Include/SessionManager.h"
#include "HimalayaDataManager/Include/DataManager.h"
#include <QSharedPointer>

using namespace DataManager;
namespace Scheduler{

int SessionManager::SessionNo = -1;
SessionManager::SessionManager(CDataManager* pDataManager)
    :m_pDataManager(pDataManager)
{
    m_SessionList.clear();
}

int SessionManager::CreateSession(const QString &retortId, const QString& protocoId)
{
    auto* pProgram = m_pDataManager->GetProgramList()->GetProgram(protocoId);
    QList<QSharedPointer<const CProgramStep>> programSteps;

    //auto session = new Session(retortId, const_cast<CProgram*>(pProgram));
    SessionNo++;
    m_SessionList.insert(SessionNo,QSharedPointer<Session>(new Session(retortId, const_cast<CProgram*>(pProgram))));
    return SessionNo;
}

void SessionManager::DestroySession(int sessionId)
{
    m_SessionList.remove(sessionId);
}

Session* SessionManager::GetSessionById(int sessionId)
{
    if (m_SessionList.find(sessionId) != m_SessionList.end())
    {
        return m_SessionList.find(sessionId).value().data();
    }
    else
    {
        return nullptr;
    }
}

Session* SessionManager::GetSessionByRetortId(const QString &retortId, Session::SessionStatus_t status)
{
    foreach(auto session, m_SessionList)
    {
        if(session->GetRetortID() == retortId
                && session->GetStatus() == status)
        {
            return session.data();
        }
    }
    return nullptr;
}



}
