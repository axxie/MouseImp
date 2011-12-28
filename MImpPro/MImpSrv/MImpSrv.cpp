// MImpSrv.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define MIMP_SERVICE_NAME           L"MImpSrv"
#define MIMP_SERVICE_DISPLAY_NAME   L"MouseImp elevated launcher"
#define MIMP_SERVICE_DESCRIPTION    L"Launches elevated MouseImp instance in each session on request. Elevation is required by MouseImp hooking in order to provide its functionality inside elevated processes"

#define MIMP_STOP_TIMEOUT           30000 // 30 seconds

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


VOID DoUpdateSvcDacl(SC_HANDLE serviceHandle)
{
    EXPLICIT_ACCESS      ea;
    SECURITY_DESCRIPTOR  sd;
    PSECURITY_DESCRIPTOR psd            = NULL;
    PACL                 pacl           = NULL;
    PACL                 pNewAcl        = NULL;
    BOOL                 bDaclPresent   = FALSE;
    BOOL                 bDaclDefaulted = FALSE;
    DWORD                dwError        = 0;
    DWORD                dwSize         = 0;
    DWORD                dwBytesNeeded  = 0;

    // Get the current security descriptor.

    if (!QueryServiceObjectSecurity(serviceHandle,
        DACL_SECURITY_INFORMATION, 
        &psd,           // using NULL does not work on all versions
        0, 
        &dwBytesNeeded))
    {
        if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
        {
            dwSize = dwBytesNeeded;
            psd = (PSECURITY_DESCRIPTOR)HeapAlloc(GetProcessHeap(),
                HEAP_ZERO_MEMORY, dwSize);
            if (psd == NULL)
            {
                // Note: HeapAlloc does not support GetLastError.
                printf("HeapAlloc failed\n");
                goto dacl_cleanup;
            }

            if (!QueryServiceObjectSecurity(serviceHandle,
                DACL_SECURITY_INFORMATION, psd, dwSize, &dwBytesNeeded))
            {
                printf("QueryServiceObjectSecurity failed (%d)\n", GetLastError());
                goto dacl_cleanup;
            }
        }
        else 
        {
            printf("QueryServiceObjectSecurity failed (%d)\n", GetLastError());
            goto dacl_cleanup;
        }
    }

    // Get the DACL.

    if (!GetSecurityDescriptorDacl(psd, &bDaclPresent, &pacl,
        &bDaclDefaulted))
    {
        printf("GetSecurityDescriptorDacl failed(%d)\n", GetLastError());
        goto dacl_cleanup;
    }

    // Build the ACE.

    BuildExplicitAccessWithName(
        &ea,
        TEXT("INTERACTIVE"),
        SERVICE_START,
        GRANT_ACCESS,
        NO_INHERITANCE);

    dwError = SetEntriesInAcl(1, &ea, pacl, &pNewAcl);
    if (dwError != ERROR_SUCCESS)
    {
        printf("SetEntriesInAcl failed(%d)\n", dwError);
        goto dacl_cleanup;
    }

    // Initialize a new security descriptor.

    if (!InitializeSecurityDescriptor(&sd, 
        SECURITY_DESCRIPTOR_REVISION))
    {
        printf("InitializeSecurityDescriptor failed(%d)\n", GetLastError());
        goto dacl_cleanup;
    }

    // Set the new DACL in the security descriptor.

    if (!SetSecurityDescriptorDacl(&sd, TRUE, pNewAcl, FALSE))
    {
        printf("SetSecurityDescriptorDacl failed(%d)\n", GetLastError());
        goto dacl_cleanup;
    }

    // Set the new DACL for the service object.

    if (!SetServiceObjectSecurity(serviceHandle, 
        DACL_SECURITY_INFORMATION, &sd))
    {
        printf("SetServiceObjectSecurity failed(%d)\n", GetLastError());
        goto dacl_cleanup;
    }
    else printf("Service DACL updated successfully\n");

dacl_cleanup:

    if(NULL != pNewAcl)
        LocalFree((HLOCAL)pNewAcl);
    if(NULL != psd)
        HeapFree(GetProcessHeap(), 0, (LPVOID)psd);
}

DWORD InstallService()
{
    wchar_t wszPath[MAX_PATH];

    if (GetModuleFileNameW(NULL, wszPath, ARRAYSIZE(wszPath)) == 0)
    {
        return GetLastError();
    }

    CScopedServiceHandle scmHandle = OpenSCManager(
        NULL,                   // machine (NULL == local)
        NULL,                   // database (NULL == default)
        SC_MANAGER_CONNECT | SC_MANAGER_CREATE_SERVICE  // access required
        );

    if (!scmHandle)
    {
        return GetLastError();
    }

    DWORD dwStartType = SERVICE_DEMAND_START;

    DWORD dwDesiredAccess = SERVICE_QUERY_STATUS|SERVICE_CHANGE_CONFIG|SERVICE_START|READ_CONTROL|WRITE_DAC;

    CScopedServiceHandle serviceHandle = CreateServiceW(
        scmHandle,                  // SCManager database
        MIMP_SERVICE_NAME,          // name of service
        MIMP_SERVICE_DISPLAY_NAME,  // name to display
        dwDesiredAccess,            // desired access
        SERVICE_WIN32_OWN_PROCESS,  // service type
        dwStartType,                // start type
        SERVICE_ERROR_NORMAL,       // error control type
        wszPath,                    // service's binary
        NULL,                       // no load ordering group
        NULL,                       // no tag identifier
        NULL,                       // dependencies
        NULL,                       // LocalSystem account
        NULL);                      // no password

    if (!serviceHandle)
    {
        DWORD lastError = GetLastError();

        if (lastError != ERROR_SERVICE_EXISTS)
        {
            return lastError;
        }

        //Service with the same name already exists
        serviceHandle = OpenService(
            scmHandle,
            MIMP_SERVICE_NAME,
            dwDesiredAccess
            );

        if (!serviceHandle)
        {
            printf("Cannot open service for repair!");
            return GetLastError();
        }

        if (!ChangeServiceConfig(
            serviceHandle,
            SERVICE_WIN32_OWN_PROCESS,
            dwStartType,
            SERVICE_ERROR_NORMAL,
            wszPath,
            L"",
            NULL,
            L"",
            L".\\LocalSystem",
            L"",
            MIMP_SERVICE_DISPLAY_NAME))
        {
            printf("Can't change service config, please reboot and try again");
            return GetLastError();
        }
    }

    SERVICE_DESCRIPTION svcDescription;
    svcDescription.lpDescription = MIMP_SERVICE_DESCRIPTION;

    if (!ChangeServiceConfig2(serviceHandle, SERVICE_CONFIG_DESCRIPTION, &svcDescription))
    {
        return GetLastError();
    }

    DoUpdateSvcDacl(serviceHandle);

    return ERROR_SUCCESS;
}

DWORD UninstallService(bool *rebootRequired)
{
    *rebootRequired = false;

    CScopedServiceHandle scmHandle = OpenSCManager(
        NULL,               // machine (NULL == local)
        NULL,               // database (NULL == default)
        SC_MANAGER_CONNECT  // access required
        );

    if (!scmHandle)
    {
        printf("Cannot open SCM to remove service!");
        return GetLastError(); 
    }

    CScopedServiceHandle serviceHandle = OpenService(
        scmHandle, 
        MIMP_SERVICE_NAME,
        DELETE | SERVICE_STOP | SERVICE_QUERY_STATUS);

    if (!serviceHandle)
    {
        printf("Cannot open service");
        return GetLastError(); 
    }

    SERVICE_STATUS serviceStatus;

    // try to stop the service
    ControlService(serviceHandle, SERVICE_CONTROL_STOP, &serviceStatus);

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
            printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
            return GetLastError();
        }

        if (serviceStatus.dwCurrentState == SERVICE_STOPPED)
        {
            break;
        }

        if (GetTickCount() - startTime > MIMP_STOP_TIMEOUT)
        {
            printf("Wait timed out\n");
            break;
        }
    }

    if (DeleteService(serviceHandle))
    {
        return ERROR_SUCCESS;
    }

    if (GetLastError() == ERROR_SERVICE_MARKED_FOR_DELETE)
    {
        *rebootRequired = true;
        return ERROR_SUCCESS;
    }

    return GetLastError();
}

bool g_serviceMustExit = false;
SERVICE_STATUS_HANDLE g_serviceStatusHandle;

DWORD g_dwCheckPoint = 1;

void ReportStatusToSCM(
    DWORD dwCurrentState)
{
    SERVICE_STATUS serviceStatus;
    serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS; 
    serviceStatus.dwCurrentState = dwCurrentState;
    serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    serviceStatus.dwWin32ExitCode = NO_ERROR;
    serviceStatus.dwServiceSpecificExitCode = 0; 
    serviceStatus.dwCheckPoint = g_dwCheckPoint;
    serviceStatus.dwWaitHint = 2000;

    g_dwCheckPoint++;

    SetServiceStatus(g_serviceStatusHandle, &serviceStatus);
}

DWORD WINAPI ServiceCtrlHandler(
    DWORD    dwControl,
    DWORD    dwEventType,
    LPVOID   lpEventData,
    LPVOID   lpContext)
{
    if (dwControl == SERVICE_CONTROL_STOP)
    {
        ReportStatusToSCM(SERVICE_STOP_PENDING);
        g_serviceMustExit = true;
    }
    return NO_ERROR;
}


void ServiceMain(
    DWORD dwArgc,
    LPTSTR *lpszArgv)
{
    g_serviceStatusHandle = RegisterServiceCtrlHandlerEx( 
        MIMP_SERVICE_NAME,
        &ServiceCtrlHandler,
        NULL);
        
    ReportStatusToSCM(SERVICE_START_PENDING);

    ReportStatusToSCM(SERVICE_RUNNING);

    while (!g_serviceMustExit)
    {
        Sleep(1000);
    }

    ReportStatusToSCM(SERVICE_STOPPED);
}

DWORD ExecuteAsService() 
{
    SERVICE_TABLE_ENTRY dispatchTable[] =
    {
        { (LPWSTR)(LPCWSTR)MIMP_SERVICE_NAME, (LPSERVICE_MAIN_FUNCTION)ServiceMain},
        { NULL, NULL}
    };

    if (!StartServiceCtrlDispatcher(dispatchTable))
    {
        return GetLastError();
    }

    return ERROR_SUCCESS;
}

int _tmain(int argc, _TCHAR* argv[])
{
    if (argc < 2)
    {
        return ExecuteAsService();
    }

    if (!wcscmp(argv[1], L"--install"))
    {
        return InstallService();
    }
    else if (!wcscmp(argv[1], L"--uninstall"))
    {
        bool rebootRequired;
        DWORD lastError = UninstallService(&rebootRequired);
        if (rebootRequired)
        {
            return MIMP_ERROR_REBOOT_REQUIRED;
        }

        return lastError;
    }

	return 0;
}

