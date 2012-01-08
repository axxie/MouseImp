
#pragma once



#define MIMP_SERVICE_NAME           _T("MImpSrv")
#define MIMP_SERVICE_DISPLAY_NAME   L"MouseImp elevated launcher"
#define MIMP_SERVICE_DESCRIPTION    L"Launches elevated MouseImp instance in each session on request. Elevation is required by MouseImp hooking in order to provide its functionality inside elevated processes"

#define MIMP_STOP_TIMEOUT           30000 // 30 seconds

#define MIMP_HOST_EXE_NAME          L"MIProHst.exe"

#define MIMP_ERROR_REBOOT_REQUIRED  -42

class CScopedServiceHandle
{
public:
    CScopedServiceHandle(
        SC_HANDLE controlledHandle)
        :
    m_controlledHandle(controlledHandle)
    {
    }

    ~CScopedServiceHandle()
    {
        if (m_controlledHandle)
        {
            CloseServiceHandle(m_controlledHandle);
            m_controlledHandle = NULL;
        }
    }

    operator SC_HANDLE()
    {
        return m_controlledHandle;
    }
    CScopedServiceHandle& operator=(const SC_HANDLE& src)
    {
        _ASSERTE(m_controlledHandle == NULL);
        m_controlledHandle = src;
        return *this;
    }

private:
    // no copy
    CScopedServiceHandle(const CScopedServiceHandle&);
    CScopedServiceHandle& operator=(const CScopedServiceHandle&);

    SC_HANDLE m_controlledHandle;
};


class CScopedHandle
{
public:
    CScopedHandle()
        :
    m_controlledHandle(INVALID_HANDLE_VALUE)
    {
    }

    ~CScopedHandle()
    {
        if (m_controlledHandle)
        {
            CloseHandle(m_controlledHandle);
            m_controlledHandle = NULL;
        }
    }

    operator HANDLE()
    {
        return m_controlledHandle;
    }

    HANDLE* operator &()
    {
        return &m_controlledHandle;
    }

    CScopedHandle& operator=(const HANDLE& src)
    {
        _ASSERTE(m_controlledHandle == NULL);
        m_controlledHandle = src;
        return *this;
    }

private:
    // no copy
    CScopedHandle(const CScopedHandle&);
    CScopedHandle& operator=(const CScopedHandle&);

    HANDLE m_controlledHandle;
};


SC_HANDLE OpenMImpSrvHandle(
    DWORD desiredAccess) 
{
    CScopedServiceHandle scmHandle = OpenSCManager(
        NULL,               // machine (NULL == local)
        NULL,               // database (NULL == default)
        SC_MANAGER_CONNECT  // access required
        );

    if (!scmHandle)
    {
        return NULL;
    }

    return OpenService(
        scmHandle, 
        MIMP_SERVICE_NAME,
        desiredAccess);
}


DWORD StopMImpSrv(bool waitForStop)
{
    CScopedServiceHandle serviceHandle = OpenMImpSrvHandle(SERVICE_STOP | SERVICE_QUERY_STATUS);

    if (!serviceHandle)
    {
        return GetLastError(); 
    }

    SERVICE_STATUS serviceStatus;

    // try to stop the service
    ControlService(serviceHandle, SERVICE_CONTROL_STOP, &serviceStatus);

    if (!waitForStop)
    {
        return ERROR_SUCCESS;
    }

    DWORD startTime = GetTickCount();

    while (serviceStatus.dwCurrentState != SERVICE_STOPPED)
    {
        DWORD dwBytesNeeded;

        Sleep(serviceStatus.dwWaitHint);

        if (!QueryServiceStatusEx( 
            serviceHandle, 
            SC_STATUS_PROCESS_INFO,
            (LPBYTE)&serviceStatus, 
            sizeof(SERVICE_STATUS_PROCESS),
            &dwBytesNeeded))
        {
            return GetLastError();
        }

        if (serviceStatus.dwCurrentState == SERVICE_STOPPED)
        {
            break;
        }

        if (GetTickCount() - startTime > MIMP_STOP_TIMEOUT)
        {
            break;
        }
    }

    return ERROR_SUCCESS;
}

DWORD StartMImpSrv()
{
    CScopedServiceHandle serviceHandle = OpenMImpSrvHandle(SERVICE_START);

    if (!serviceHandle)
    {
        return GetLastError(); 
    }

    StartService(serviceHandle, 0, NULL);

    return ERROR_SUCCESS;
}