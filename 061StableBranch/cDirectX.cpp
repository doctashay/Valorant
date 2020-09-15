#include "hDirectX.h"
#include "cMain.h"
#include <string>
#include "Offsets.h"
using namespace std;

//Globals
IDirect3D9Ex* p_Object = 0;
IDirect3DDevice9Ex* p_Device = 0;
D3DPRESENT_PARAMETERS p_Params;
HANDLE hProc;
ID3DXLine* p_Line;
ID3DXFont* pFontSmall = 0;
bool playerESP = true;
bool vehicleESP = true;
bool ESP = true;

//Helper functions
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

}Vector3, *pVector3;
VECTOR3 SubVectorDist(VECTOR3 playerFrom, VECTOR3 playerTo)
{
	VECTOR3 temp;

	temp.x = playerFrom.x - playerTo.x;
	temp.y = playerFrom.y - playerTo.y;
	temp.z = playerFrom.z - playerTo.z;

	return temp;
};
float Distance(Vector3 pos, Vector3 pos2)
{
	float distance = sqrt((pos.x - pos2.x) * (pos.x - pos2.x) +
		(pos.y - pos2.y) * (pos.y - pos2.y) +
		(pos.z - pos2.z) * (pos.z - pos2.z));
	return distance;
};
std::string readString(DWORD Address, int Size)
{
	char* buffer = new char[Size];

	if (ReadProcessMemory(hProcess, (LPCVOID)Address, buffer, Size, nullptr))
		return std::string(buffer);
	else
		return std::string("ERROR");
};
string GetPlayerName(int playerPtr)
{
	string playerName = "Player";

	//int myPlayerID = Mem.ReadInt(pLocal + 0x7E0); //Example, you should already have this. 
	int entityPlayerID = Read<int>(playerPtr + 0x0800); //Entity ID from ObjectTable iteration 
	int scoreBoardBase = Read<int>(ENFUSION_NETWORKMANAGER); //Make this a constant dude. Or you'll have fun times updating to new values. 
	int scoreBoardOffset = Read<int>(scoreBoardBase + ENFUSION_NETWORKCLIENT);
	int scoreBoardSize = Read<int>(scoreBoardOffset + ENFUSION_NETWORKCLIENT_PLAYERIDENTITY_SIZE);
	int scoreBoard = Read<int>(scoreBoardOffset + ENFUSION_NETWORKCLIENT_SCOREBOARD);

	for (int i = 0; i < scoreBoardSize; i++)
	{
		int iteratePlayerID = Read<int>(scoreBoard + (i * 0xE8) + 0x4); //Get iterated player ID from Scoreboard 

		if (iteratePlayerID == entityPlayerID) //If you don't want your own name use "&& iteratePlayerID != myPlayerID" 
		{
			int playerNamePointer = Read<int>(scoreBoard + 0x88 + (i * 0xE8));
			int playerNameSize = Read<int>(playerNamePointer + 0x4);
			playerName = readString(playerNamePointer + 0x8, playerNameSize);
		}
	}
	return playerName;
}
bool WorldToScreen(Vector3 WorldPos, float& ScreenposX, float& ScreenposY)
{
	DWORD World = Read<DWORD>(ENFUSION_WORLD);
	DWORD Camera = Read<DWORD>(World + ENFUSION_WORLD_CAMERA);
	Vector3 InvViewRight = Read<Vector3>(Camera + ENFUSION_CAMERA_VIEWRIGHT);
	Vector3 InvViewUp = Read<Vector3>(Camera + ENFUSION_CAMERA_VIEWUP);
	Vector3 InvViewForward = Read<Vector3>(Camera + ENFUSION_CAMERA_VIEWFORWARD);
	Vector3 InvViewTranslation = Read<Vector3>(Camera + ENFUSION_CAMERA_VIEWTRANSLATION);
	Vector3 ViewPortMatrix = Read<Vector3>(Camera + ENFUSION_CAMERA_VIEWPORTSIZE);
	Vector3 ProjD1 = Read<Vector3>(Camera + ENFUSION_CAMERA_PROJECTION_D1);
	Vector3 ProjD2 = Read<Vector3>(Camera + ENFUSION_CAMERA_PROJECTION_D2);

	Vector3 temp;

	temp = SubVectorDist(WorldPos, InvViewTranslation); // inv view is a vector 3

	float x = temp.dot(InvViewRight);// inv view is a vector 3
	float y = temp.dot(InvViewUp);// inv view is a vector 3
	float z = temp.dot(InvViewForward);// inv view is a vector 3

	if (z <= 0.f)
		return false;

	ScreenposX = ViewPortMatrix.x * (1.f + (x / ProjD1.x / z));// proj d1 is a float
	ScreenposY = ViewPortMatrix.y * (1.f - (y / ProjD2.y / z)); // proj d2 is a float

	return true;
};


int DirectXInit(HWND hWnd, HANDLE hProcess)
{
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &p_Object)))
		exit(1);

	ZeroMemory(&p_Params, sizeof(p_Params));
	p_Params.Windowed = TRUE;
	p_Params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	p_Params.hDeviceWindow = hWnd;
	p_Params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	p_Params.BackBufferFormat = D3DFMT_A8R8G8B8;
	p_Params.BackBufferWidth = 1920;
	p_Params.BackBufferHeight = 1080;
	p_Params.EnableAutoDepthStencil = TRUE;
	p_Params.AutoDepthStencilFormat = D3DFMT_D16;

	if (FAILED(p_Object->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &p_Params, 0, &p_Device)))
		exit(1);

	if (!p_Line)
		D3DXCreateLine(p_Device, &p_Line);
	//p_Line->SetAntialias(1); *removed cuz crosshair was blurred*

	D3DXCreateFont(p_Device, 18, 0, 0, 0, false, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Arial", &pFontSmall);

	hProc = hProcess;

	return 0;
};

int Render()
{
	p_Device->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
	p_Device->BeginScene();

	if (tWnd == GetForegroundWindow()) // DO ALL DRAWINGS INSIDE HERE VVVVV
	{
		DWORD World = Read<DWORD>(ENFUSION_WORLD);
		DWORD CameraOn = Read<DWORD>(World + ENFUSION_WORLD_CAMERAON);
		DWORD RealPlayer = Read<DWORD>(CameraOn + ENFUSION_WORLD_REALPLAYER);
		DWORD RenderVisualState = Read<DWORD>(RealPlayer + ENFUSION_PLAYER_RENDERERVISUALSTATE);
		Vector3 FutureVisualState = Read<Vector3>(RealPlayer + ENFUSION_PLAYER_FUTUREVISUALSTATE);

		DWORD AnimalContainerClose = Read<DWORD>(World + ENFUSION_WORLD_NEARANIMALTABLE);
		int AnimalContainerCloseSize = Read<int>(World + 0xA40);
		if (ESP)
		{
			DrawString("DayZ Menu v0.07", 15, 50, 255, 0, 0, pFontSmall);
			for (int i = 0; i < AnimalContainerCloseSize; i++)
			{
				DWORD pEntity = Read<DWORD>(AnimalContainerClose + (i * 0x4));

				if (!pEntity || pEntity == CameraOn)
					continue;

				DWORD pEntityType = Read<DWORD>(pEntity + 0x7C);

				if (!pEntityType)
					continue;

				DWORD pSimulationName = Read<DWORD>(pEntityType + 0x70);

				if (!pSimulationName)
					continue;

				int SimulationNameSize = Read<int>(pSimulationName + 0x4);

				if (!SimulationNameSize)
					continue;

				std::string SimulationName = readString(pSimulationName + 0x8, SimulationNameSize);

				if (SimulationName.find("car") != std::string::npos && vehicleESP)
				{
					DWORD pVisualState = Read<DWORD>(pEntity + ENFUSION_ENTITY_FUTUREVISUALSTATE);
					Vector3 Pos3D = Read<Vector3>(pVisualState + 0x28);
					float x, y;

					if (WorldToScreen(Pos3D, x, y))
					{
						char buffer[256];
						sprintf(buffer, "Car");
						DrawString(buffer, x, y, 110, 244, 66, pFontSmall);

					};

				}

				else if (SimulationName.find("soldier") != std::string::npos && playerESP) // We have found a player
				{
					DWORD pVisualState = Read<DWORD>(pEntity + ENFUSION_ENTITY_FUTUREVISUALSTATE);
					Vector3 Pos3D = Read<Vector3>(pVisualState + 0x28);
					bool isDead = Read<bool>(pEntity + 0x2D0);
					//byte entityStance = Read<byte>(pEntity + 0x14E0);
					//string curStance = "";

					//if (entityStance == 0)
					//		curStance = "Standing";
					//	if (entityStance == 1)
					//		curStance = "Crouching";
					//	if (entityStance == 2)
					//		curStance = "Laying";
					float x, y;
					char buffer[256];
					if (WorldToScreen(Pos3D, x, y))
					{
						if (isDead)
						{
							sprintf_s(buffer, "Dead Player");
							DrawString(buffer, x, y, 50, 50, 50, pFontSmall);
						}
						else
						{

							DrawString("Player", x, y, 244, 226, 66, pFontSmall);
						};
					};

				};

				DWORD AnimalContainerFar = Read<DWORD>(World + 0xAE4);
				int AnimalContainerFarSize = Read<int>(World + 0xAE8);

				for (int i = 0; i < AnimalContainerFarSize; i++)
				{
					DWORD pEntity = Read<DWORD>(AnimalContainerFar + (i * 0x4));

					if (!pEntity || pEntity == CameraOn)
						continue;

					DWORD pEntityType = Read<DWORD>(pEntity + ENFUSION_ENTITY_ENTITYTYPE);

					if (!pEntityType)
						continue;

					DWORD pSimulationName = Read<DWORD>(pEntityType + 0x70);

					if (!pSimulationName)
						continue;

					int SimulationNameSize = Read<int>(pSimulationName + 0x4);

					if (!SimulationNameSize)
						continue;

					std::string SimulationName = readString(pSimulationName + 0x8, SimulationNameSize);

					if (SimulationName.find("car") != std::string::npos && vehicleESP)
					{
						DWORD pVisualState = Read<DWORD>(pEntity + ENFUSION_ENTITY_FUTUREVISUALSTATE);
						Vector3 Pos3D = Read<Vector3>(pVisualState + 0x28);

						float x, y;

						if (WorldToScreen(Pos3D, x, y))
						{

							char buffer[256];

							sprintf(buffer, "Car");
							DrawString(buffer, x, y, 110, 244, 66, pFontSmall);
						};

					}


					else if (SimulationName.find("soldier") != std::string::npos && playerESP) // We have found a variable
					{
						DWORD pVisualState = Read<DWORD>(pEntity + ENFUSION_ENTITY_FUTUREVISUALSTATE);
						Vector3 Pos3D = Read<Vector3>(pVisualState + 0x28);
						bool isDead = Read<bool>(pEntity + 0x2D0);
						string playerName = GetPlayerName(RealPlayer);

						//Removed pending offset update
						//byte entityStance = Read<byte>(pEntity + 0x14E0);
						//	string curStance = "";

						//if (entityStance == 0)
						//		curStance = "Standing";
						//	if (entityStance == 1)
						//		curStance = "Crouching";
						//	if (entityStance == 2)
						//		curStance = "Laying"; 
						char buffer[256];
						float x, y;

						if (WorldToScreen(Pos3D, x, y))
						{
							if (isDead)
							{
								sprintf(buffer, "Dead Player");
								DrawString(buffer, x, y, 50, 50, 50, pFontSmall);
							}
							else
							{
								DrawString("Player", x, y, 244, 226, 66, pFontSmall);
							};
						}

					}


				}
			}
		}

	}


	p_Device->EndScene();
	p_Device->PresentEx(0, 0, 0, 0, 0);
	return 0;
};
