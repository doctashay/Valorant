#include "hMain.h"
#include "cMain.h"
#include "Offsets.h"

//Globals
const MARGINS Margin = { 0, 0, 1920, 1080 };
char lWindowName[256] = "gerihgjblgrhg%$PUjnmls,dkWL";
char tWindowName[256] = "DayZ"; /* tWindowName ? Target Window Name */
HWND hWnd;
HWND tWnd;
int Width = 1920;
int Height = 1080;
RECT tSize;
MSG Message;
HANDLE hProcess;

//Feature toggles
bool noGrass = false;
bool noRecoil = false;
bool alwaysDay = false;
bool noFatigue = false;
bool speedhack = false;
bool cheatFunctions = false;

//Essential Functions
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

//Overlay
LRESULT CALLBACK Proc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{
	case WM_PAINT:
		Render();
		break;
	case WM_CREATE:
		DwmExtendFrameIntoClientArea(hWnd, &Margin);
		break;
	case WM_DESTROY:
		PostQuitMessage(1);
		return 0;

	default:
		return DefWindowProc(hWnd, Message, wParam, lParam);
		break;
	};

	return 0;
};

void SetWindowToTarget(void)
{
	while (true)
	{
		tWnd = FindWindow(NULL, tWindowName);

		if (tWnd)
		{
			GetWindowRect(tWnd, &tSize);
			Width = tSize.right - tSize.left;
			Height = tSize.bottom - tSize.top;
			DWORD dwStyle = GetWindowLong(tWnd, GWL_STYLE);

			if (dwStyle & WS_BORDER)
			{
				tSize.top += 23;
				Height -= 23;
			}

			MoveWindow(hWnd, tSize.left, tSize.top, Width, Height, true);
		}
		else
		{
			MessageBox(0, "Overlay failed", "Overlay failed", MB_OK);
			exit(1); // Game is no longer running, close the overlay.
		}
	}
}

//Start memory thread

void CheatThread(void)
{
#if defined _DEBUG
	printf("Thread starting.\n");
#endif
	while (true)
	{
		if (GetAsyncKeyState(VK_LCONTROL))
		{
			noGrass = !noGrass;
			if (noGrass)
			{
				printf("No Grass ON\n");
				Write<int>(Read<int>(ENFUSION_WORLD) + ENFUSION_WORLD_TERRAINGRID, 50.f);
				printf("Read code was %d\n", GetLastError());
			}
			else
			{
				printf("No Grass OFF\n");
				Write<int>(Read<int>(ENFUSION_WORLD) + ENFUSION_WORLD_TERRAINGRID, 10.f);
			};
			Sleep(1000);
		}
		if (GetAsyncKeyState(VK_INSERT))
		{
			ESP = !ESP;
			if (ESP)
			{
				printf("ESP ON\n");
			}
			else
			{
				printf("ESP OFF\n");
			}
			Sleep(1000);
		}

		if (GetAsyncKeyState(VK_END))
		{
			MessageBox(0, "Exiting...", "Exiting...", MB_OK);
			exit(1);
		}

		if (GetAsyncKeyState(VK_NUMPAD1))
		{
			noFatigue = !noFatigue;
			if (noFatigue)
			{
				printf("No Fatigue ON\n");
				DWORD CameraOn = Read<int>((ENFUSION_WORLD)+ENFUSION_WORLD_CAMERAON);
				DWORD RealPlayer = Read<int>(CameraOn + ENFUSION_WORLD_REALPLAYER);
				Write<float>(RealPlayer + ENFUSION_PLAYER_FATIGUE, 0.f);
				Write<float>(RealPlayer + ENFUSION_PLAYER_FATIGUELEFT, 1.f);
			}
			else
			{
				printf("No Fatigue OFF\n");
			}
			Sleep(1000);
		}

		if (GetAsyncKeyState(VK_NUMPAD2))
		{
			noRecoil = !noRecoil;
			if (noRecoil)
			{
				printf("No Recoil ON\n");
				DWORD CameraOn = Read<int>((ENFUSION_WORLD)+ENFUSION_WORLD_CAMERAON);
				DWORD RealPlayer = Read<int>(CameraOn + ENFUSION_WORLD_REALPLAYER);
				Write<float>(RealPlayer + ENFUSION_PLAYER_RECOIL, 0.f);

			}
			else
			{
				printf("No Recoil OFF\n");
				DWORD CameraOn = Read<int>((ENFUSION_WORLD)+ENFUSION_WORLD_CAMERAON);
				DWORD RealPlayer = Read<int>(CameraOn + ENFUSION_WORLD_REALPLAYER);
				Write<float>(RealPlayer + ENFUSION_PLAYER_RECOIL, 1.f);
			}
			Sleep(1000);
		}
		Sleep(1);
	}
}

void Hook(void*)
{
#if defined _DEBUG
	MessageBox(0, "DEBUG: DLL injected.", "Initializing...", MB_OK);


	if (!AllocConsole())
		MessageBox(0, "DEBUG: AllocConsole failed", "DEBUG: AllocConsole failed", MB_OK);
#endif

	SetConsoleTitleA("DayZ Memory Client");

	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
	printf("DayZ Memory Client");
	printf("Left Control: No Grass\n");
	printf("Insert: Toggle ESP\n");
	printf("Numpad 1: No Fatigue\n");
	printf("Numpad 2: No Recoil\n");
	hWnd = FindWindow(0, "DayZ");

	DWORD DZPID;

	GetWindowThreadProcessId(hWnd, &DZPID);
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, DZPID);

	if (hProcess)
	{
		MessageBox(0, "Handle valid", "Success", MB_OK);
		printf("GetLastError() returned %d\n", GetLastError());
	}

	else
	{
		MessageBox(0, "Opened a valid handle to game process", "Failure", MB_OK);
		printf("GetLastError() returned %d\n", GetLastError());
	}


	/*
		HINSTANCE hInst = HINSTANCE();

	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)SetWindowToTarget, 0, 0, 0);

	WNDCLASSEX wClass;
	wClass.cbClsExtra = NULL;
	wClass.cbSize = sizeof(WNDCLASSEX);
	wClass.cbWndExtra = NULL;
	wClass.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));
	wClass.hCursor = LoadCursor(0, IDC_ARROW);
	wClass.hIcon = LoadIcon(0, IDI_APPLICATION);
	wClass.hIconSm = LoadIcon(0, IDI_APPLICATION);
	wClass.hInstance = hInst;
	wClass.lpfnWndProc = Proc;
	wClass.lpszClassName = lWindowName;
	wClass.lpszMenuName = lWindowName;
	wClass.style = CS_VREDRAW | CS_HREDRAW;

	if (!RegisterClassEx(&wClass))
		MessageBox(0, "RegisterClassEx failed, closing\n", "RegisterClassEx failed, closing\n", MB_OK);

	tWnd = FindWindowA(NULL, tWindowName);

	if (tWnd)
	{
		GetWindowRect(tWnd, &tSize);
		Width = tSize.right - tSize.left;
		Height = tSize.bottom - tSize.top;
		hWnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED, lWindowName, lWindowName, WS_POPUP, 1, 1, Width, Height, 0, 0, 0, 0);
		SetLayeredWindowAttributes(hWnd, 0, 1.0f, LWA_ALPHA);
		SetLayeredWindowAttributes(hWnd, 0, RGB(0, 0, 0), LWA_COLORKEY);
		ShowWindow(hWnd, SW_SHOW);
		printf("Created overlay successfully\n");
	};

	printf("Intializing DirectX Drawing Framework\n");

	DirectXInit(hWnd, hProcess);

	for (;;)
	{
		Sleep(1);
		if (PeekMessage(&Message, hWnd, 0, 0, PM_REMOVE))
		{
			DispatchMessage(&Message);
			TranslateMessage(&Message);
		};


	};
	
	*/

#if defined _DEBUG
	printf("Overlay thread started...\n");
#endif


}

BOOL WINAPI DllMain(HMODULE hDll, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		_beginthread(Hook, 0, 0);
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)CheatThread, 0, 0, 0);
		//CreateThread(0, 0, (LPTHREAD_START_ROUTINE)SpeedThread, 0, 0, 0);
	}

	return TRUE;
}