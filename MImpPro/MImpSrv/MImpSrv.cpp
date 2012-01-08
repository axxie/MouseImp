// MImpSrv.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MImpSrvCtl.h"

bool SetPrivilege(
    HANDLE  tokenHandle,          
    LPCTSTR privilegeName,  
    bool    enablePrivilege) 
{
    TOKEN_PRIVILEGES tp;
    LUID luid;

    if (!LookupPrivilegeValue( 
        NULL,            // lookup privilege on local system
        privilegeName,   // privilege to lookup 
        &luid))          // receives LUID of privilege
    {
        printf("LookupPrivilegeValue error: %u\n", GetLastError() ); 
        return false; 
    }

    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    if (enablePrivilege)
    {
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    }
    else
    {
        tp.Privileges[0].Attributes = 0;
    }

    // Enable the privilege or disable all privileges.
    if (!AdjustTokenPrivileges(
        tokenHandle, 
        FALSE, 
        &tp, 
        sizeof(TOKEN_PRIVILEGES), 
        (PTOKEN_PRIVILEGES) NULL, 
        (PDWORD) NULL))
    { 
        printf("AdjustTokenPrivileges error: %u\n", GetLastError() ); 
        return false; 
    } 

    if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)

    {
        printf("The token does not have the specified privilege. \n");
        return false;
    } 

    return true;
}

bool EnablePrivilege(
    LPCTSTR privilegeName,
    bool    enablePrivilege)
{
    CScopedHandle tokenHandle;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &tokenHandle))
    {
        return false;
    }

    return SetPrivilege(tokenHandle, privilegeName, enablePrivilege); 
}



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
    wchar_t modulePath[MAX_PATH];

    if (GetModuleFileNameW(NULL, modulePath, ARRAYSIZE(modulePath)) == 0)
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
        modulePath,                 // service's binary
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
            modulePath,
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

    StopMImpSrv(true);

    CScopedServiceHandle serviceHandle = OpenMImpSrvHandle(DELETE | SERVICE_QUERY_STATUS);

    if (!serviceHandle)
    {
        printf("Cannot open service");
        return GetLastError(); 
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

void LaunchMouseImpInOneSession(DWORD sessionID)
{
    CScopedHandle impersonationToken;
    CScopedHandle primaryToken;
    LPVOID pEnvironment = NULL;
    TOKEN_MANDATORY_LABEL tokenHighIntegiryLabel = {0};
    SID_IDENTIFIER_AUTHORITY mandatoryLabelAuthority = SECURITY_MANDATORY_LABEL_AUTHORITY;
    STARTUPINFO startInfo = {0}; 
    PROCESS_INFORMATION processInfo = {0};

    WCHAR moduleFolder[MAX_PATH];

    if (GetModuleFileNameW(NULL, moduleFolder, ARRAYSIZE(moduleFolder)) == 0)
    {
        goto finally;
    }
    PathRemoveFileSpec(moduleFolder);

    if (!WTSQueryUserToken(sessionID, &impersonationToken)) 
    {
        goto finally;
    }
    
    if (!DuplicateTokenEx(
        impersonationToken,
        MAXIMUM_ALLOWED,
        NULL,
        SecurityAnonymous,
        TokenPrimary,
        &primaryToken)) 
    {
        goto finally;
    }

    if (!CreateEnvironmentBlock(&pEnvironment, primaryToken, TRUE)) 
    {
        goto finally;
    }

    tokenHighIntegiryLabel.Label.Attributes = SE_GROUP_INTEGRITY;

    if (!AllocateAndInitializeSid(
        &mandatoryLabelAuthority,
        1,
        SECURITY_MANDATORY_HIGH_RID,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        &tokenHighIntegiryLabel.Label.Sid))
    {
        goto finally;
    }

    if (!SetTokenInformation(
        primaryToken,
        TokenIntegrityLevel,
        &tokenHighIntegiryLabel,
        sizeof(TOKEN_MANDATORY_LABEL) + GetLengthSid(tokenHighIntegiryLabel.Label.Sid)))
    {
        goto finally;
    }

    if (!ImpersonateLoggedOnUser(primaryToken))
    {
        goto finally;
    }

    startInfo.cb = sizeof(startInfo);

    WCHAR modulePath[MAX_PATH];
    wcscpy_s(modulePath, MAX_PATH, moduleFolder);
    PathAppend(modulePath, MIMP_HOST_EXE_NAME);

    for( int i = 0; i < 5; i++ )
    {
        if (CreateProcessAsUser(
            primaryToken,
            NULL,
            modulePath, 
            NULL,
            NULL,
            FALSE, 
            CREATE_NO_WINDOW | CREATE_UNICODE_ENVIRONMENT, 
            pEnvironment,
            NULL,
            &startInfo,
            &processInfo))
        {
            break;
        }
        Sleep(1000);
    }

    wcscpy_s(modulePath, MAX_PATH, moduleFolder);
    PathAppend(modulePath, L"x64\\" MIMP_HOST_EXE_NAME);

    for( int i = 0; i < 5; i++ )
    {
        if (CreateProcessAsUser(
            primaryToken,
            NULL,
            modulePath, 
            NULL,
            NULL,
            FALSE, 
            CREATE_NO_WINDOW | CREATE_UNICODE_ENVIRONMENT, 
            pEnvironment,
            NULL,
            &startInfo,
            &processInfo))
        {
            break;
        }
        Sleep(1000);
    }

    RevertToSelf();

finally:

    if (tokenHighIntegiryLabel.Label.Sid)
    {
        FreeSid(tokenHighIntegiryLabel.Label.Sid);
    }
    if (pEnvironment)
    {
        DestroyEnvironmentBlock( pEnvironment );
    }
}

void LaunchMouseImpInEverySession()
{
    PWTS_SESSION_INFO sessionInfo = NULL;
    DWORD sessionCount = 0;
    
    if (!WTSEnumerateSessions(
        WTS_CURRENT_SERVER_HANDLE,
        0,
        1,
        &sessionInfo,
        &sessionCount))
    {
        return;
    }

    EnablePrivilege(SE_TCB_NAME, true);
    EnablePrivilege(SE_INCREASE_QUOTA_NAME, true);
    EnablePrivilege(SE_ASSIGNPRIMARYTOKEN_NAME, true);

    for (DWORD i = 0; i < sessionCount; ++i)
    {
        LaunchMouseImpInOneSession(sessionInfo[i].SessionId);
    }

    WTSFreeMemory(sessionInfo);

    EnablePrivilege(SE_TCB_NAME, false);
    EnablePrivilege(SE_INCREASE_QUOTA_NAME, false);
    EnablePrivilege(SE_ASSIGNPRIMARYTOKEN_NAME, false);
}


DWORD WINAPI StopServiceThreadProc(LPVOID)
{
    Sleep(5000);
    StopMImpSrv(false);
    return ERROR_SUCCESS;
}


void ServiceMain(
    DWORD   dwArgc,
    LPTSTR *lpszArgv)
{
    g_serviceStatusHandle = RegisterServiceCtrlHandlerEx( 
        MIMP_SERVICE_NAME,
        &ServiceCtrlHandler,
        NULL);
        
    ReportStatusToSCM(SERVICE_START_PENDING);

    ReportStatusToSCM(SERVICE_RUNNING);

    LaunchMouseImpInEverySession();

    CloseHandle(CreateThread(NULL, 0, &StopServiceThreadProc, NULL, 0, NULL));

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
    else if (!wcscmp(argv[1], L"--debug"))
    {
        return LaunchMouseImpInEverySession();
    }

	return 0;
}

