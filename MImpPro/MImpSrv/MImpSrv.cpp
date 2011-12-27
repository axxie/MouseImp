// MImpSrv.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define MIMP_SERVICE_NAME           L"MImpSrv"
#define MIMP_SERVICE_DISPLAY_NAME   L"MouseImp elevated launcher service"
#define MIMP_SERVICE_DESCRIPTION    L"Launches elevated MouseImp instance in each session on request. Elevation is required by MouseImp hooking in order to provide its functionality inside elevated processes"


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


int _tmain(int argc, _TCHAR* argv[])
{
    InstallService();
	return 0;
}

