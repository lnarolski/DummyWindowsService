// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#define SVCNAME TEXT("DummyWindowsService")

SERVICE_STATUS serviceStatus;
SERVICE_STATUS_HANDLE serviceStatusHandle;
HANDLE stopEvent = NULL;

VOID UpdateServiceStatus(DWORD currentState)
{
    serviceStatus.dwCurrentState = currentState;
    SetServiceStatus(serviceStatusHandle, &serviceStatus);
}

DWORD ServiceHandler(DWORD controlCode, DWORD eventType, LPVOID eventData, LPVOID context)
{
    switch (controlCode)
    {
    case SERVICE_CONTROL_STOP:
        serviceStatus.dwCurrentState = SERVICE_STOPPED;
        SetEvent(stopEvent);
        break;
    case SERVICE_CONTROL_SHUTDOWN:
        serviceStatus.dwCurrentState = SERVICE_STOPPED;
        SetEvent(stopEvent);
        break;
    case SERVICE_CONTROL_PAUSE:
        serviceStatus.dwCurrentState = SERVICE_PAUSED;
        break;
    case SERVICE_CONTROL_CONTINUE:
        serviceStatus.dwCurrentState = SERVICE_RUNNING;
        break;
    case SERVICE_CONTROL_INTERROGATE:
        break;
    default:
        break;
    }

    UpdateServiceStatus(SERVICE_RUNNING);

    return NO_ERROR;
}

void WriteToEventLog(LPCWSTR pszMessage)
{
    HANDLE hEventLog = NULL;
    LPCWSTR lpszStrings[1] = { NULL };

    // Open a handle to the Application event log.
    hEventLog = OpenEventLog(NULL,  // NULL for local machine
        L"Application");  // Source name to open

    if (hEventLog == NULL)
    {
        return;
    }

    // Set the message that you want to log.
    lpszStrings[0] = pszMessage;

    // Report an informational event.
    if (!ReportEvent(hEventLog,          // Event log handle
        EVENTLOG_INFORMATION_TYPE,  // Event type
        0,                   // Event category
        0,                   // Event identifier
        NULL,                // No user security identifier
        1,                   // Size of lpszStrings array
        0,                   // No binary data
        lpszStrings,         // Array of strings
        NULL))               // No binary data
    {
        
    }

    // Close the event log handle.
    CloseEventLog(hEventLog);
}

STARTUPINFO si;
PROCESS_INFORMATION pi;

VOID ExecuteServiceCode()
{
    stopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    UpdateServiceStatus(SERVICE_RUNNING);

    // Start the child process. 
    if (!CreateProcess(L"DummyWindowsExe.exe",   // No module name (use command line)
        NULL,        // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi)           // Pointer to PROCESS_INFORMATION structure
        )
    {
        WriteToEventLog(L"CreateProcess failed: " + GetLastError());
        return;
    }

    WriteToEventLog(L"WindowsDummyService started");

    while (1)
    {
        WaitForSingleObject(stopEvent, INFINITE);
        UpdateServiceStatus(SERVICE_STOPPED);
        
        TerminateProcess(pi.hProcess, 1);

        WriteToEventLog(L"WindowsDummyService stopped");
        return;
    }
}

extern "C" __declspec(dllexport) VOID WINAPI ServiceMain(DWORD argC, LPWSTR * argV)
{
    serviceStatusHandle = RegisterServiceCtrlHandler(SVCNAME, (LPHANDLER_FUNCTION)ServiceHandler);

    serviceStatus.dwServiceType = SERVICE_WIN32_SHARE_PROCESS;
    serviceStatus.dwServiceSpecificExitCode = 0;

    UpdateServiceStatus(SERVICE_START_PENDING);
    ExecuteServiceCode();
}

