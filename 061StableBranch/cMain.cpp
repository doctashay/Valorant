#include "hMain.h"
#include "cMain.h"
#include "Offsets.h"
#include <tlhelp32.h>



//Globals
HANDLE hProcess;
HWND hWnd;

//Read/write memory
template <class cData> void Write(DWORD dwAddress, cData Value)
{
	WriteProcessMemory(hProcess, (LPVOID)dwAddress, &Value, sizeof(cData), NULL);
}

template <typename ReadType> ReadType Read(DWORD Address)
{
	ReadType Data;
	(ReadProcessMemory(hProcess, (PBYTE)(Address), &Data, sizeof(ReadType), 0));
	return Data;

}

typedef struct VECTOR3
{
	float x, y, z;

	VECTOR3(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	};

	VECTOR3(void)
	{
		this->x = 0.f;
		this->y = 0.f;
		this->z = 0.f;
	};

	float dot(VECTOR3 dot)
	{
		return (this->x * dot.x + this->y * dot.y + this->z * dot.z);
	};

}VECTOR3, *pVECTOR3;

void EnableDebugPriv()
{
	HANDLE hToken;
	LUID luid;
	TOKEN_PRIVILEGES tkp;

	OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);

	LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid);

	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Luid = luid;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL);

	CloseHandle(hToken);
}

void Hook(void*)
{
#if defined _DEBUG
	MessageBoxA(0, "DEBUG: DLL injected.", "Initializing...", MB_OK);


	if (!AllocConsole())
		MessageBoxA(0, "DEBUG: AllocConsole failed", "DEBUG: AllocConsole failed", MB_OK);
#endif

	SetConsoleTitleA("Borderlands Memory Client");

	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
	EnableDebugPriv();

	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry) == TRUE)
	{
		while (Process32Next(snapshot, &entry) == TRUE)
		{
			if (stricmp(entry.szExeFile, "Borderlands3.exe") == 0)
			{
				HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);

				printf("Borderlands 3 Process ID is %d", entry.th32ProcessID);

				printf("hProcess result is %p", hProcess);
				printf("hProcess: GetLastError() returned %d\n", GetLastError());

				if (hProcess)
				{
					MessageBoxA(0, "Handle valid", "Success", MB_OK);
					printf("GetLastError() returned %d\n", GetLastError());
				}

				else
				{
					MessageBoxA(0, "Handle is not valid", "Failure", MB_OK);
					printf("GetLastError() returned %d\n", GetLastError());
				}
				Sleep(5000);

				CloseHandle(hProcess);
			}
		}
	}

	CloseHandle(snapshot);

}

BOOL WINAPI DllMain(HMODULE hDll, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		_beginthread(Hook, 0, 0);
	}

	return TRUE;
}