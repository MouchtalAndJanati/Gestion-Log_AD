#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <strsafe.h>
#include "cconio.h"
#include <iostream>





#define MAX_TIMESTAMP_LEN       23 + 1   // mm/dd/yyyy hh:mm:ss.mmm
#define MAX_RECORD_BUFFER_SIZE  0x70000  
#define CATEGORY_RESOURCE_DLL        L"CUELECategory.dll" //Category File Maessage
#define EVENT_RESOURCE_DLL           L"CUELEvent.dll"  //Event FIle Message
#define PATHFOLDER   L"C:/........"


HANDLE GetMessageResources();
LPWSTR GetCategoryString(DWORD MessageId);
DWORD DumpRecordsInBuffer(PBYTE pBuffer, DWORD dwBytesRead,int id);
DWORD GetEventTypeName(DWORD EventType);
LPWSTR GetMessageString(DWORD Id, DWORD argc, LPWSTR args);
SYSTEMTIME GetTimestamp(const DWORD Time);
DWORD ApplyParameterStringsToMessage(CONST LPCWSTR pMessage, LPWSTR & pFinalMessage);

void menu();
void search();

CONST LPWSTR pEventTypeNames[] = {L"Error", L"Warning", L"Information", L"Audit Success", L"Audit Failure"};


HANDLE g_hResources = NULL;
HANDLE category_hResources = NULL;

//LoadLibrary Category Resources
HANDLE GetCategoryResources()
{
    HANDLE hResources = NULL;

    hResources = LoadLibraryEx(CATEGORY_RESOURCE_DLL, NULL, LOAD_LIBRARY_AS_IMAGE_RESOURCE | LOAD_LIBRARY_AS_DATAFILE);
    if (NULL == hResources)
    {

       wprintf(L"LoadLibrary failed with %lu.\n", GetLastError());
    }

    return hResources;
}

//LoadLibrary Event Resources
HANDLE GetEventResources()
{
    HANDLE hResources = NULL;

    hResources = LoadLibraryEx(EVENT_RESOURCE_DLL, NULL, LOAD_LIBRARY_AS_IMAGE_RESOURCE | LOAD_LIBRARY_AS_DATAFILE);
    if (NULL == hResources)
    {
        wprintf(L"LoadLibrary failed with %lu.\n", GetLastError());
    }

    return hResources;
}



void main(void)
{
	
    HANDLE hEventLog = NULL;
    DWORD status = ERROR_SUCCESS;
    DWORD dwBytesToRead = 0;
    DWORD dwBytesRead = 0;
    DWORD dwMinimumBytesToRead = 0;
    PBYTE pBuffer = NULL;
    PBYTE pTemp = NULL;
    std::wstring fileName =L"";
	
	int choix;
retour :
	menu(); // Menu of Application
	scanf("%d",&choix);
	system("cls");
	textcolor(15);
	
	switch(choix)
	{
		case 1: fileName = L"Application.evtx";
			break;
		case 2: fileName = L"Security.evtx";
			break;
		case 3: fileName = L"Setup.evtx";
		break;
		case 4: fileName = L"System.evtx";
		break;
	}

	std::wstring s(PATHFOLDER);
	s += L"/";
	s += fileName;


    hEventLog = OpenBackupEventLog(NULL,s.c_str()); // source name
    
    if (NULL == hEventLog)
    {
        wprintf(L"OpenEventLog failed with 0x%x.\n", GetLastError());
		getch();
        goto cleanup;
    }


	g_hResources = GetEventResources();
	category_hResources = GetCategoryResources();
    // Allocate an initial block of memory used to read event records. 
   
    dwBytesToRead = MAX_RECORD_BUFFER_SIZE;
    pBuffer = (PBYTE)malloc(dwBytesToRead);
    if (NULL == pBuffer)
    {
        wprintf(L"Failed to allocate the initial memory for the record buffer.\n");
        goto cleanup;
    }

    // Read blocks of records until you reach the end of the log or an 
    // error occurs. The records are read from newest to oldest. If the buffer
    // is not big enough to hold a complete event record, reallocate the buffer.
    while (ERROR_SUCCESS == status)
    {
        if (!ReadEventLog(hEventLog, 
            EVENTLOG_SEQUENTIAL_READ | EVENTLOG_BACKWARDS_READ,
            0, 
            pBuffer,
            dwBytesToRead,
            &dwBytesRead,
            &dwMinimumBytesToRead))
        {
            status = GetLastError();
            if (ERROR_INSUFFICIENT_BUFFER == status)
            {
                status = ERROR_SUCCESS;

                pTemp = (PBYTE)realloc(pBuffer, dwMinimumBytesToRead);
                if (NULL == pTemp)
                {
                    wprintf(L"Failed to reallocate the memory for the record buffer (%d bytes).\n", dwMinimumBytesToRead);
                    goto cleanup;
                }

                pBuffer = pTemp;
                dwBytesToRead = dwMinimumBytesToRead;
            }
            else 
            {
                if (ERROR_HANDLE_EOF != status)
                {
                    wprintf(L"ReadEventLog failed with %lu.\n", status);
                    goto cleanup;
                }
            }
        }
        else
        {
            
			getch();
			system("cls");
			search(); //Menu Search
			int choix;
			scanf("%d",&choix);
			system("cls");
            textcolor(15);
  switch (choix)
    {
        case 2:
            choix = EVENTLOG_ERROR_TYPE;
            break;
        case 3:
            choix = EVENTLOG_WARNING_TYPE;
            break;
        case 1:
            choix = EVENTLOG_INFORMATION_TYPE;
            break;
        case 4:
            choix = EVENTLOG_AUDIT_SUCCESS;
            break;
        case 5:
            choix = EVENTLOG_AUDIT_FAILURE;
            break;
		case 6:
            goto retour;
            break;
    }
            DumpRecordsInBuffer(pBuffer, dwBytesRead,choix); // Print the contents of each record in the buffer.
        }
    }

	getch();

cleanup:

    if (hEventLog)
        CloseEventLog(hEventLog);

    if (pBuffer)
        free(pBuffer);
}




// Loop through the buffer and print the contents of each record 
// in the buffer.
DWORD DumpRecordsInBuffer(PBYTE pBuffer, DWORD dwBytesRead,int id)
{
    DWORD status = ERROR_SUCCESS;
    PBYTE pRecord = pBuffer;
    PBYTE pEndOfRecords = pBuffer + dwBytesRead;
    LPWSTR pMessage = NULL;
    LPWSTR pFinalMessage = NULL;
    WCHAR TimeStamp[MAX_TIMESTAMP_LEN];

	wprintf(L"Niveau\tDate et Heure  ID de l'evenement Source\tCategorie de la tache   Description\n\n");
    while (pRecord < pEndOfRecords)
    {
		if(((PEVENTLOGRECORD)pRecord)->EventType == id) {

		   SYSTEMTIME st= GetTimestamp(((PEVENTLOGRECORD)pRecord)->TimeGenerated);
           wprintf(L"%s\t", pEventTypeNames[GetEventTypeName(((PEVENTLOGRECORD)pRecord)->EventType)]);
		
		   wprintf(L"%02d/%02d/%04d  %02d:%02d:%02d   ",st.wDay,st.wMonth,st.wYear,st.wHour,st.wMinute,st.wSecond);
           
		   wprintf(L"%d\t", ((PEVENTLOGRECORD)pRecord)->EventID & 0xFFFF);
			
		   pMessage = GetMessageString(((PEVENTLOGRECORD)pRecord)->EventID,((PEVENTLOGRECORD)pRecord)->NumStrings, (LPWSTR)(pRecord + ((PEVENTLOGRECORD)pRecord)->StringOffset));

            if (pMessage)
            {
                status = ApplyParameterStringsToMessage(pMessage, pFinalMessage);

                wprintf(L"%s\t", (pFinalMessage) ? pFinalMessage : pMessage);
                LocalFree(pMessage);
                pMessage = NULL;

                if (pFinalMessage)
                {
                    free(pFinalMessage);
                    pFinalMessage = NULL;
                }
            }


           pMessage = GetMessageString(((PEVENTLOGRECORD)pRecord)->EventCategory,0, NULL);

            if (pMessage)
            {
                wprintf(L"%s\t", pMessage);
                LocalFree(pMessage);
                pMessage = NULL;
            }

			else
			{
                LocalFree(pMessage);
                pMessage = NULL;
			}

          


            // write the event data

            if (((PEVENTLOGRECORD)pRecord)->DataLength > 0)
            {
                wprintf(L"%s\n", (LPWSTR)(pRecord + ((PEVENTLOGRECORD)pRecord)->DataOffset));
            }

         
		   }

        pRecord += ((PEVENTLOGRECORD)pRecord)->Length;

		  
	}
	
    getchar();
    return status;
}


// Get an index value to the pEventTypeNames array based on 
// the event type value.
DWORD GetEventTypeName(DWORD EventType)
{
    DWORD index = 0;

    switch (EventType)
    {
        case EVENTLOG_ERROR_TYPE:
            index = 0;
            break;
        case EVENTLOG_WARNING_TYPE:
            index = 1;
            break;
        case EVENTLOG_INFORMATION_TYPE:
            index = 2;
            break;
        case EVENTLOG_AUDIT_SUCCESS:
            index = 3;
            break;
        case EVENTLOG_AUDIT_FAILURE:
            index = 4;
            break;

    }

    return index;
}


// Formats the specified message. If the message uses inserts, build
// the argument list to pass to FormatMessage.

LPWSTR GetCategoryString(DWORD MessageId)
{
    LPWSTR pMessage = NULL;
    DWORD dwFormatFlags = FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_ALLOCATE_BUFFER;
    DWORD_PTR* pArgs = NULL;
    DWORD i =0;
  
    if (!FormatMessage(dwFormatFlags,
                       category_hResources,
                       MessageId,
                       0,  
                       (LPWSTR)&pMessage, 
                       0, 
                       (va_list*)pArgs))
    {
        wprintf(L"Format message failed with %lu\n", GetLastError());
    }
    return pMessage;
}


LPWSTR GetMessageString(DWORD MessageId, DWORD argc, LPWSTR argv)
{
    LPWSTR pMessage = NULL;
    DWORD dwFormatFlags = FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_ALLOCATE_BUFFER;
    DWORD_PTR* pArgs = NULL;
    LPWSTR pString = argv;
    DWORD i =0;
    if (argc > 0)
    {
        pArgs = (DWORD_PTR*)malloc(sizeof(DWORD_PTR) * argc);
        if (pArgs)
        {
            dwFormatFlags = dwFormatFlags | FORMAT_MESSAGE_ARGUMENT_ARRAY;

            for (i= 0; i < argc; i++)
            {
                pArgs[i] = (DWORD_PTR)pString;
                pString += wcslen(pString) + 1;
            }
        }
        else
        {
            dwFormatFlags |= FORMAT_MESSAGE_IGNORE_INSERTS;
            wprintf(L"Failed to allocate memory for the insert string array.\n");
        }
    }
    if (!FormatMessage(dwFormatFlags,
                       g_hResources,
                       MessageId,
                       0,  
                       (LPWSTR)&pMessage, 
                       0, 
                       (va_list*)pArgs))
    {
        wprintf(L"Format message failed with %lu\n", GetLastError());
    }

    if (pArgs)
        free(pArgs);

    return pMessage;
}


DWORD ApplyParameterStringsToMessage(CONST LPCWSTR pMessage, LPWSTR & pFinalMessage)
{
    DWORD status = ERROR_SUCCESS;
    DWORD dwParameterCount = 0;  // Number of insertion strings found in pMessage
    size_t cbBuffer = 0;         // Size of the buffer in bytes
    size_t cchBuffer = 0;        // Size of the buffer in characters
    size_t cchParameters = 0;    // Number of characters in all the parameter strings
    size_t cch = 0;
    DWORD i = 0;
    LPWSTR* pStartingAddresses = NULL;  // Array of pointers to the beginning of each parameter string in pMessage
    LPWSTR* pEndingAddresses = NULL;    // Array of pointers to the end of each parameter string in pMessage
    DWORD* pParameterIDs = NULL;        // Array of parameter identifiers found in pMessage
    LPWSTR* pParameters = NULL;         // Array of the actual parameter strings
    LPWSTR pTempMessage = (LPWSTR)pMessage;
    LPWSTR pTempFinalMessage = NULL;

    // Determine the number of parameter insertion strings in pMessage.
    while (pTempMessage = wcschr(pTempMessage, L'%'))
    {
        dwParameterCount++;
        pTempMessage++;
    }

    // If there are no parameter insertion strings in pMessage, return.
    if (0 == dwParameterCount)
    {
        pFinalMessage = NULL;
        goto cleanup;
    }

    // Allocate an array of pointers that will contain the beginning address 
    // of each parameter insertion string.
    cbBuffer = sizeof(LPWSTR) * dwParameterCount;
    pStartingAddresses = (LPWSTR*)malloc(cbBuffer);
    if (NULL == pStartingAddresses)
    {
        wprintf(L"Failed to allocate memory for pStartingAddresses.\n");
        status = ERROR_OUTOFMEMORY;
        goto cleanup;
    }

    RtlZeroMemory(pStartingAddresses, cbBuffer);

    // Allocate an array of pointers that will contain the ending address (one
    // character past the of the identifier) of the each parameter insertion string.
    pEndingAddresses = (LPWSTR*)malloc(cbBuffer);
    if (NULL == pEndingAddresses)
    {
        wprintf(L"Failed to allocate memory for pEndingAddresses.\n");
        status = ERROR_OUTOFMEMORY;
        goto cleanup;
    }

    RtlZeroMemory(pEndingAddresses, cbBuffer);

    // Allocate an array of pointers that will contain pointers to the actual
    // parameter strings.
    pParameters = (LPWSTR*)malloc(cbBuffer);
    if (NULL == pParameters)
    {
        wprintf(L"Failed to allocate memory for pEndingAddresses.\n");
        status = ERROR_OUTOFMEMORY;
        goto cleanup;
    }

    RtlZeroMemory(pParameters, cbBuffer);

    // Allocate an array of DWORDs that will contain the message identifier
    // for each parameter.
    pParameterIDs = (DWORD*)malloc(cbBuffer);
    if (NULL == pParameterIDs)
    {
        wprintf(L"Failed to allocate memory for pParameterIDs.\n");
        status = ERROR_OUTOFMEMORY;
        goto cleanup;
    }

    RtlZeroMemory(pParameterIDs, cbBuffer);

    // Find each parameter in pMessage and get the pointer to the
    // beginning of the insertion string, the end of the insertion string,
    // and the message identifier of the parameter.
    pTempMessage = (LPWSTR)pMessage;
    while (pTempMessage = wcschr(pTempMessage, L'%'))
    {
        if (isdigit(*(pTempMessage+1)))
        {
            pStartingAddresses[i] = pTempMessage;

            pTempMessage++;
            pParameterIDs[i] = (DWORD)_wtoi(pTempMessage);

            while (isdigit(*++pTempMessage))
                ;

            pEndingAddresses[i] = pTempMessage;

            i++;
        }
    }

    // For each parameter, we use the message identifier to get the
    // actual parameter string.
    for (DWORD i = 0; i < dwParameterCount; i++)
    {
        pParameters[i] = GetMessageString(pParameterIDs[i], 0, NULL);
        if (NULL == pParameters[i])
        {
            wprintf(L"GetMessageString could not find parameter string for insert %lu.\n", i);
            status = ERROR_INVALID_PARAMETER;
            goto cleanup;
        }

        cchParameters += wcslen(pParameters[i]);
    }

    // Allocate enough memory for pFinalMessage based on the length of pMessage
    // and the length of each parameter string. The pFinalMessage buffer will contain 
    // the completed parameter substitution.
    pTempMessage = (LPWSTR)pMessage;
    cbBuffer = (wcslen(pMessage) + cchParameters + 1) * sizeof(WCHAR);
    pFinalMessage = (LPWSTR)malloc(cbBuffer);
    if (NULL == pFinalMessage)
    {
        wprintf(L"Failed to allocate memory for pFinalMessage.\n");
        status = ERROR_OUTOFMEMORY;
        goto cleanup;
    }

    RtlZeroMemory(pFinalMessage, cbBuffer);
    cchBuffer = cbBuffer / sizeof(WCHAR);
    pTempFinalMessage = pFinalMessage;

    // Build the final message string.
    for (DWORD i = 0; i < dwParameterCount; i++)
    {
        // Append the segment from pMessage. In the first iteration, this is "8 " and in the
        // second iteration, this is " = 2 ".
        wcsncpy_s(pTempFinalMessage, cchBuffer, pTempMessage, cch = (pStartingAddresses[i] - pTempMessage));
        pTempMessage = pEndingAddresses[i];
        cchBuffer -= cch;

        // Append the parameter string. In the first iteration, this is "quarts" and in the
        // second iteration, this is "gallons"
        pTempFinalMessage += cch;
        wcscpy_s(pTempFinalMessage, cchBuffer, pParameters[i]);
        cchBuffer -= cch = wcslen(pParameters[i]);

        pTempFinalMessage += cch;
    }

    // Append the last segment from pMessage, which is ".".
    wcscpy_s(pTempFinalMessage, cchBuffer, pTempMessage);

cleanup:

    if (ERROR_SUCCESS != status)
        pFinalMessage = (LPWSTR)pMessage;

    if (pStartingAddresses)
        free(pStartingAddresses);

    if (pEndingAddresses)
        free(pEndingAddresses);

    if (pParameterIDs)
        free(pParameterIDs);

    for (DWORD i = 0; i < dwParameterCount; i++)
    {
        if (pParameters[i])
            LocalFree(pParameters[i]);
    }

    return status;
}


// Get a string that contains the time stamp of when the event 
// was generated.
SYSTEMTIME GetTimestamp(const DWORD Time)
{
    ULONGLONG ullTimeStamp = 0;
    ULONGLONG SecsTo1970 = 116444736000000000;
    SYSTEMTIME st;
    FILETIME ft, ftLocal;

    ullTimeStamp = Int32x32To64(Time, 10000000) + SecsTo1970;
    ft.dwHighDateTime = (DWORD)((ullTimeStamp >> 32) & 0xFFFFFFFF);
    ft.dwLowDateTime = (DWORD)(ullTimeStamp & 0xFFFFFFFF);
    
    FileTimeToLocalFileTime(&ft, &ftLocal);
    FileTimeToSystemTime(&ftLocal, &st);

	return st;
}

void menu()
{ 
system("cls");
    textcolor(14);
gotoxy(10,2); printf("******************************************************");
gotoxy(10,3);printf("*          Analyse des Logs Active Directory          *");
gotoxy(10,4);printf("*******************************************************");

gotoxy(10,7); printf("*******************************************************");
gotoxy(10,8); printf("*                                                     *");
gotoxy(10,9); printf("*  1.         Analyser les Logs Application           *");
gotoxy(10,10); printf("*                                                     *");
gotoxy(10,11); printf("*  2.         Analyser les Logs Security              *");
gotoxy(10,12);printf("*                                                     *");
gotoxy(10,13);printf("*  3.         Analyser les Logs Setup                 *");
gotoxy(10,14);printf("*                                                     *");
gotoxy(10,15);printf("*  4.         Analyser les Logs System                *");
gotoxy(10,16);printf("*                                                     *");
gotoxy(10,17);printf("*  5.         Exit                                    *");
gotoxy(10,18);printf("*******************************************************");

gotoxy(10,22);printf("Saisir Votre Choix :");

}

void search()
{ 
system("cls");
    textcolor(14);
gotoxy(10,2); printf("******************************************************");
gotoxy(10,3);printf("*          Analyse des Logs Active Directory          *");
gotoxy(10,4);printf("*******************************************************");

gotoxy(10,7); printf("*******************************************************");
gotoxy(10,8); printf("*                                                     *");
gotoxy(10,9); printf("*  1.         Filtrer les Logs de type Information    *");
gotoxy(10,10);printf("*                                                     *");
gotoxy(10,11);printf("*  2.         Filtrer les Logs de type Error          *");
gotoxy(10,12);printf("*                                                     *");
gotoxy(10,13);printf("*  3.         Filtrer les Logs de type Warning        *");
gotoxy(10,14);printf("*                                                     *");
gotoxy(10,15);printf("*  4.         Filtrer les Logs Audit Success          *");
gotoxy(10,16);printf("*                                                     *");
gotoxy(10,17);printf("*  5.         Filtrer les Logs  Audit Failure         *");
gotoxy(10,18);printf("*                                                     *");
gotoxy(10,19);printf("*  6.         Retour                                  *");
gotoxy(10,20);printf("*******************************************************");

gotoxy(10,24);printf("Saisir Votre Choix :");

}