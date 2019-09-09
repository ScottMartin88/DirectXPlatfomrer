#include "gameData.h"
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <cstdio>
// global declarations
IDXGISwapChain *swapchain;             // the pointer to the swap chain interface
ID3D11Device *dev;                     // the pointer to our Direct3D device interface
ID3D11DeviceContext *devcon;           // the pointer to our Direct3D device context
ID3D11RenderTargetView *backbuffer;    // the pointer to our back buffer
ID3D11DepthStencilView *zbuffer;       // the pointer to our depth buffer
ID3D11InputLayout *pLayout;            // the pointer to the input layout
ID3D11VertexShader *pVS;               // the pointer to the vertex shader
ID3D11PixelShader *pPS;                // the pointer to the pixel shader
ID3D11Buffer *pVBuffer;                // the pointer to the vertex buffer
ID3D11Buffer *pCBuffer;                // the pointer to the constant buffer
ID3D11Buffer *pIBuffer;                // the pointer to the index buffer
ID3D11ShaderResourceView *pTexture;    // the pointer to the texture

#pragma region Prototypes
// function prototypes
void InitD3D(HWND hWnd, gameData*);    // sets up and initializes Direct3D

void levelSetup(gameData* , std::vector<std::string>);
void counterSetup(int * counter[], std::vector<std::string> Level);
void setPlayerLocation(gameData* newGame, float x, float y);

void smallSquare(gameData* newGame, LPCWSTR texture, int);

void RenderWalls(gameData* newGame, D3DXMATRIX matView, D3DXMATRIX matProjection, int objcount, wall wall[], float collisonRadius);     // renders a single frame
void RenderPlayer(gameData*);     // renders a single frame
void RenderEnemy(gameData*, D3DXMATRIX matView, D3DXMATRIX matProjection, gameEnemy, float);

void RenderScene(gameData*,int);     // renders a single frame

void CleanD3D(void);        // closes Direct3D and releases memory
void InitPipeline(void);    // loads and prepares the shaders

void update(gameData*);
bool collisionDetection(gameData*, int, float, wall wall[]);
bool collisionDetection(gameData* newGame, int i, float collide, gameEnemy enemy[]);
int enemycollision(gameData* newGame, float collide,int);

std::vector<std::string> loadLevel(std::string);

// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
#pragma endregion

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#pragma region Window Setup
	HWND hWnd;
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = L"WindowClass";
	RegisterClassEx(&wc);
	RECT wr = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
	hWnd = CreateWindowEx(NULL,
		L"WindowClass",
		L"Our First Direct3D Program",
		WS_OVERLAPPEDWINDOW,
		300,
		300,
		wr.right - wr.left,
		wr.bottom - wr.top,
		NULL,
		NULL,
		hInstance,
		NULL);
	ShowWindow(hWnd, nCmdShow);
#pragma endregion

		MSG msg;
		bool endGame = false;
		int state = 0;

		std::vector<std::string> pauseScreen;
		pauseScreen = loadLevel("Levels\\PauseScreen.txt");

		while (!endGame){

			std::vector<std::string> level;
			int enemyCount;


			if (state == 0)
			{
				enemyCount = 1;
				level = loadLevel("Levels\\Title.txt");
			}
			else if (state == 1)
			{
				enemyCount = 2;
				level = loadLevel("Levels\\level1.txt");
			}
			else if (state == 2)
			{
				enemyCount = 4;
				level = loadLevel("Levels\\level2.txt");
			}


			int * counter[14] = { 0 };
			int newCounter[14] = { 0 };
			counterSetup(counter, level);

			for (int i = 0; i < 14; i++)
			{
				newCounter[i] = (int)counter[i];
			}


			//Setup the GameData For the level.
			gameData * newGame = new gameData(newCounter, enemyCount);
			levelSetup(newGame, level);

			//Setup the player object locations for the game.
			if (state == 0)
			{
				newGame->enemy[0].enemyPosition(13.0f, 180.0f, true);
				setPlayerLocation(newGame, 15.0f, 11.0f);
			}
			else if (state == 1)
			{
				setPlayerLocation(newGame, 3.0f, 15.0f);
				newGame->enemy[0].enemyPosition(13.0f, 18.0f, true);
				newGame->enemy[1].enemyPosition(32.0f, 18.0f, true);
			}
			else if (state == 2){
				setPlayerLocation(newGame, 3.0f, 15.0f);
				newGame->enemy[0].enemyPosition(13.0f, 17.0f, true);
				newGame->enemy[1].enemyPosition(33.0f, 20.0f, true);
				newGame->enemy[2].enemyPosition(25.0f, 17.0f, false);
				newGame->enemy[3].enemyPosition(55.0f, 20.0f, true);
			}

			bool paused = false;
	
			// set up and initialize Direct3D
			InitD3D(hWnd, newGame);
			//runs better when this starts playing??????
			PlaySound(TEXT("music.wav"), NULL, SND_FILENAME | SND_LOOP | SND_ASYNC);
			
			//Game Loop
			while (!newGame->restart)
			{
				if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&msg);	DispatchMessage(&msg);
					if (msg.message == WM_QUIT){
						endGame = true;
						break; 
					}
				}

				if (msg.message == WM_KEYDOWN)
				{
					switch (msg.wParam)
					{
					case 'A':{ newGame->newPlayer->moveLeft = 1;  break; }
					case 'D':{ newGame->newPlayer->moveRight = -1;  break; }
					case VK_SPACE:{ newGame->newPlayer->jump = true;  break; }
					case VK_ESCAPE:{paused = true;  break; }
					case 'R':{newGame->restart = true;  break; }
					}
				}

				if (state == 0)
				{
					paused = false;
				}

				if (msg.message == WM_KEYUP)
				{
					switch (msg.wParam)
					{
					case 'A':{ newGame->newPlayer->moveLeft = 0;  break; }
					case 'D':{ newGame->newPlayer->moveRight = 0;  break; }
					case VK_SPACE:{ newGame->newPlayer->jump = false;  break; }
					}
				}

				//Setup the Pause screen in game
				while (paused)
				{
					if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
					{
						TranslateMessage(&msg);	DispatchMessage(&msg);
						if (msg.message == WM_QUIT)
							break;
					}
					if (msg.message == WM_KEYDOWN)
					{
						switch (msg.wParam)
						{
						case VK_ESCAPE:{paused = false;  break; }
						}
					}
				}

				update(newGame);
				RenderScene(newGame, state);

				// clear the back buffer to a deep blue
				devcon->ClearRenderTargetView(backbuffer, newGame->D3DColourBlack);
				// clear the depth buffer
				devcon->ClearDepthStencilView(zbuffer, D3D11_CLEAR_DEPTH, 1.0f, 0);
			}
			//End of level loop.

			// clean up DirectX and COM
			CleanD3D();
			newGame->restart = false;
			delete newGame; newGame = NULL;

			state += 1;
			if (state > 2){
				state = 0;
			}

		}

		return msg.wParam;
}

// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	} break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}
// this function initializes and prepares Direct3D for use
void InitD3D(HWND hWnd, gameData* newGame)
{
	// create a struct to hold information about the swap chain
	DXGI_SWAP_CHAIN_DESC scd;

	// clear out the struct for use
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	// fill the swap chain description struct
	scd.BufferCount = 1;                                   // one back buffer
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;    // use 32-bit color
	scd.BufferDesc.Width = SCREEN_WIDTH;                   // set the back buffer width
	scd.BufferDesc.Height = SCREEN_HEIGHT;                 // set the back buffer height
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;     // how swap chain is to be used
	scd.OutputWindow = hWnd;                               // the window to be used
	scd.SampleDesc.Count = 4;                              // how many multisamples
	scd.Windowed = TRUE;                                   // windowed/full-screen mode
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;    // allow full-screen switching

	// create a device, device context and swap chain using the information in the scd struct
	D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		NULL,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&swapchain,
		&dev,
		NULL,
		&devcon);


	// create the depth buffer texture
	D3D11_TEXTURE2D_DESC texd;
	ZeroMemory(&texd, sizeof(texd));

	texd.Width = SCREEN_WIDTH;
	texd.Height = SCREEN_HEIGHT;
	texd.ArraySize = 1;
	texd.MipLevels = 1;
	texd.SampleDesc.Count = 4;
	texd.Format = DXGI_FORMAT_D32_FLOAT;
	texd.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	ID3D11Texture2D *pDepthBuffer;
	dev->CreateTexture2D(&texd, NULL, &pDepthBuffer);

	// create the depth buffer
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(dsvd));

	dsvd.Format = DXGI_FORMAT_D32_FLOAT;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;

	dev->CreateDepthStencilView(pDepthBuffer, &dsvd, &zbuffer);
	pDepthBuffer->Release();

	// get the address of the back buffer
	ID3D11Texture2D *pBackBuffer;
	swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	// use the back buffer address to create the render target
	dev->CreateRenderTargetView(pBackBuffer, NULL, &backbuffer);
	pBackBuffer->Release();

	// set the render target as the back buffer
	devcon->OMSetRenderTargets(1, &backbuffer, zbuffer);



	// Set the viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = SCREEN_WIDTH;
	viewport.Height = SCREEN_HEIGHT;
	viewport.MinDepth = 0;    // the closest an object can be on the depth buffer is 0.0
	viewport.MaxDepth = 1;    // the farthest an object can be on the depth buffer is 1.0

	devcon->RSSetViewports(1, &viewport);

	InitPipeline();
	
}
// this is the function used to render a single frame
void RenderWalls(gameData* newGame, D3DXMATRIX matView, D3DXMATRIX matProjection, int objcount, wall wall[], float collisionRadius)
{
	CBUFFER cBuffer;

	cBuffer.LightVector = D3DXVECTOR4(2.0f, 2.0f, 2.0f, 0.0f);
	cBuffer.LightColor = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
	cBuffer.AmbientColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	D3DXMATRIX matTranslate, matRotate;
	D3DXMATRIX matFinal;


	// select which vertex buffer to display
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	devcon->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);
	devcon->IASetIndexBuffer(pIBuffer, DXGI_FORMAT_R32_UINT, 0);

	static float Time = 0.0f;// Time += 0.001;

	for (int i = 0; i < objcount; i++){
		if (collisionDetection(newGame, i, collisionRadius, wall)){
			// create a rotation matrix
			D3DXMatrixRotationY(&matRotate, Time);
			D3DXMatrixTranslation(&matTranslate, wall[i].wallX, wall[i].wallY, 0);

			// create the final transform
			cBuffer.Final = matTranslate * matRotate * matView * matProjection;
			cBuffer.Rotation = matRotate;

			//draw
			devcon->UpdateSubresource(pCBuffer, 0, 0, &cBuffer, 0, 0);
			devcon->PSSetShaderResources(0, 1, &pTexture);
			devcon->DrawIndexed(6, 0, 0);
		}
	}
	// switch the back buffer and the front buffer


}
void RenderPlayer(gameData* newGame, D3DXMATRIX matView, D3DXMATRIX matProjection)
{
	CBUFFER cBuffer;

	cBuffer.LightVector = D3DXVECTOR4(2.0f, 2.0f, 2.0f, 0.0f);
	cBuffer.LightColor = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
	cBuffer.AmbientColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
	D3DXMATRIX matTranslate, matRotate;
	D3DXMATRIX matFinal;


	// select which vertex buffer to display
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	devcon->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);
	devcon->IASetIndexBuffer(pIBuffer, DXGI_FORMAT_R32_UINT, 0);


	static float Time = 0.0f;


	// create a rotation matrix
	D3DXMatrixRotationY(&matRotate, Time);
	D3DXMatrixTranslation(&matTranslate, newGame->newPlayer[0].getPlayerPosition('x'), newGame->newPlayer[0].getPlayerPosition('y'), 0);

	// create the final transform
	cBuffer.Final = matTranslate * matRotate * matView * matProjection;
	cBuffer.Rotation = matRotate;

	//draw
	devcon->UpdateSubresource(pCBuffer, 0, 0, &cBuffer, 0, 0);
	devcon->PSSetShaderResources(0, 1, &pTexture);
	devcon->DrawIndexed(6, 0, 0);

}
void RenderEnemy(gameData* newGame, D3DXMATRIX matView, D3DXMATRIX matProjection, gameEnemy enemy[], float collisionRadius)
{
	CBUFFER cBuffer;

	cBuffer.LightVector = D3DXVECTOR4(2.0f, 2.0f, 2.0f, 0.0f);
	cBuffer.LightColor = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
	cBuffer.AmbientColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
	D3DXMATRIX matTranslate, matRotate;
	D3DXMATRIX matFinal;


	// select which vertex buffer to display
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	devcon->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);
	devcon->IASetIndexBuffer(pIBuffer, DXGI_FORMAT_R32_UINT, 0);


	static float Time = 0.0f;
	for (int i = 0; i < newGame->enemyNo; i++){
		if (collisionDetection(newGame, i, collisionRadius, enemy)){
			// create a rotation matrix
			D3DXMatrixRotationY(&matRotate, Time);
			D3DXMatrixTranslation(&matTranslate, newGame->enemy[i].xPos, newGame->enemy[i].yPos, 0);

			// create the final transform
			cBuffer.Final = matTranslate * matRotate * matView * matProjection;
			cBuffer.Rotation = matRotate;

			//draw
			devcon->UpdateSubresource(pCBuffer, 0, 0, &cBuffer, 0, 0);
			devcon->PSSetShaderResources(0, 1, &pTexture);
			devcon->DrawIndexed(6, 0, 0);
		}
	}
}
void RenderScene(gameData* newGame, int state){
	D3DXMATRIX matView;
	D3DXMATRIX matProjection;
	if (newGame->newPlayer[0].getPlayerPosition('x') < -1.6f){
	D3DXMatrixLookAtLH(&matView,
		&D3DXVECTOR3(newGame->newPlayer[0].getPlayerPosition('x'), newGame->newPlayer[0].getPlayerPosition('y'), 1.07f),    // the camera position
		&D3DXVECTOR3(newGame->newPlayer[0].getPlayerPosition('x'), newGame->newPlayer[0].getPlayerPosition('y'), 0.0f),    // the look-at position
		&D3DXVECTOR3(0.0f, 1.0f, 0.0f));   // the up direction
	newGame->cameraX = newGame->newPlayer[0].getPlayerPosition('x');
	newGame->cameraY = newGame->newPlayer[0].getPlayerPosition('Y');
		}
	else{
		D3DXMatrixLookAtLH(&matView,
			&D3DXVECTOR3(newGame->cameraX, newGame->newPlayer[0].getPlayerPosition('y'), 1.07f),    // the camera position
			&D3DXVECTOR3(newGame->cameraX, newGame->newPlayer[0].getPlayerPosition('y'), 0.0f),    // the look-at position
			&D3DXVECTOR3(0.0f, 1.0f, 0.0f));   // the up direction
	}
	

		// select which primtive type we are using
		devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// create a projection matrix
		D3DXMatrixPerspectiveFovLH(&matProjection,
			(FLOAT)D3DXToRadian(100),                    // field of view
			(FLOAT)SCREEN_WIDTH / (FLOAT)SCREEN_HEIGHT, // aspect ratio
			1.0f,                                       // near view-plane
			100.0f);                                    // far view-plane

		//Player Animation
		if (newGame->newPlayer->move == 1){
			smallSquare(newGame, L"playerleft.png", 0);
		}
		else if (newGame->newPlayer->move == -1){
			smallSquare(newGame, L"playerright.png", 0);
		}
		else{
			if (newGame->counter  < 150){
				smallSquare(newGame, L"player.png", 0);
			}
			else if (newGame->counter  > 165){
				newGame->counter = 0;
				smallSquare(newGame, L"player.png", 0);
			}
			else if (newGame->counter  > 160){
				smallSquare(newGame, L"player2.png", 0);
			}
			else if (newGame->counter  > 155){
				smallSquare(newGame, L"player3.png", 0);
			}
			else{
				smallSquare(newGame, L"player2.png", 0);
			}
			newGame->counter++;
		}

		float renderDistance = 3.5f;
		RenderPlayer(newGame, matView, matProjection);
		smallSquare(newGame, L"enemy.png", 0);
		RenderEnemy(newGame, matView, matProjection, newGame->enemy, renderDistance);
		
	//Rendering background, Needs a small square with a texture to be chosen, then told to render the obj
		smallSquare(newGame, L"error.png", 0);
		RenderWalls(newGame, matView, matProjection, newGame->errorCount, newGame->newError, renderDistance);
		smallSquare(newGame, L"grass.png", state);
		RenderWalls(newGame, matView, matProjection, newGame->grassCount, newGame->newWall, renderDistance);
		smallSquare(newGame, L"dirtgrass.png", state);
		RenderWalls(newGame, matView, matProjection, newGame->dirtGrassCount, newGame->newDirtGrass, renderDistance);
		smallSquare(newGame, L"dirt.png", state);
		RenderWalls(newGame, matView, matProjection, newGame->dirtCount, newGame->newDirt, renderDistance);
		smallSquare(newGame, L"cloud.png", state);
		RenderWalls(newGame, matView, matProjection, newGame->cloudCount, newGame->newCloud, renderDistance);


		smallSquare(newGame, L"hillleft.png", state);
		RenderWalls(newGame, matView, matProjection, newGame->hillLeftCount, newGame->newHillLeft, renderDistance);
		smallSquare(newGame, L"hillmiddle.png", state);
		RenderWalls(newGame, matView, matProjection, newGame->hillMiddleCount, newGame->newHillMiddle, renderDistance);
		smallSquare(newGame, L"hillright.png", state);
		RenderWalls(newGame, matView, matProjection, newGame->hillRightCount, newGame->newHillRight, renderDistance);
		smallSquare(newGame, L"mountainleft.png", state);
		RenderWalls(newGame, matView, matProjection, newGame->MountLeftCount, newGame->newMountLeft, renderDistance);
		smallSquare(newGame, L"mountainmiddle.png", state);
		RenderWalls(newGame, matView, matProjection, newGame->MountMiddleCount, newGame->newMountMiddle, renderDistance);
		smallSquare(newGame, L"mountainright.png", state);
		RenderWalls(newGame, matView, matProjection, newGame->MountRightCount, newGame->newMountRight, renderDistance);


		if (newGame->flowerAniCounter <= 5){
			smallSquare(newGame, L"flower.png", state);
			RenderWalls(newGame, matView, matProjection, newGame->flowerCount, newGame->newFlower, renderDistance);
		}
		else if( newGame->flowerAniCounter > 15 ){
			smallSquare(newGame, L"flower.png", state);
			RenderWalls(newGame, matView, matProjection, newGame->flowerCount, newGame->newFlower, renderDistance);
			newGame->flowerAniCounter = 0;
		}
		else if (newGame->flowerAniCounter > 10){
			smallSquare(newGame, L"flower3.png", state);
			RenderWalls(newGame, matView, matProjection, newGame->flowerCount, newGame->newFlower, renderDistance);
		}
		else if (newGame->flowerAniCounter > 5){
			smallSquare(newGame, L"flower2.png", state);
			RenderWalls(newGame, matView, matProjection, newGame->flowerCount, newGame->newFlower, renderDistance);
		}
		
		newGame->flowerAniCounter++;
		smallSquare(newGame, L"flowerstem.png", state);
		RenderWalls(newGame, matView, matProjection, newGame->flowerStemCount, newGame->newFlowerStem, renderDistance);

		smallSquare(newGame, L"sky.png", state);
		RenderWalls(newGame, matView, matProjection, newGame->skyCount, newGame->newSky, renderDistance);

		swapchain->Present(0, 0);

}
// this is the function that cleans up Direct3D and COM
void CleanD3D(void)
{
	swapchain->SetFullscreenState(FALSE, NULL);    // switch to windowed mode

	// close and release all existing COM objects
	zbuffer->Release();
	pLayout->Release();
	pVS->Release();
	pPS->Release();
	pVBuffer->Release();
	pIBuffer->Release();
	pCBuffer->Release();
	pTexture->Release();
	swapchain->Release();
	backbuffer->Release();
	dev->Release();
	devcon->Release();
}
// this is the function that creates the shape to render
void smallSquare(gameData* newGame, LPCWSTR texture, int state)
{
	LPCWSTR DIRECTORY;
	//Texture Directory
	if (state == 1){
		DIRECTORY = L"Texture\\Grass\\";
	}
	else if (state == 2){
		DIRECTORY = L"Texture\\Cave\\";
	}
	else{
		DIRECTORY = L"Texture\\";
	}
	//Combining the texture and directory so "Texture\\" doesn't need to be repeatedly added to the LPCWSTR that is sent over.
	std::wstring comb = std::wstring(DIRECTORY) + texture;
	LPCWSTR objTexture = comb.c_str();


	VERTEX OurVertices[] =
	{
		{ -0.0625f, -0.0625f, 0.0625f, D3DXVECTOR3(0.0f, 0.0f, 1.0f), 1.0f, 1.0f },    // Vertices
		{ 0.0625f, -0.0625f, 0.0625f, D3DXVECTOR3(0.0f, 0.0f, 1.0f), 0.0f, 1.0f },
		{ -0.0625f, 0.0625f, 0.0625f, D3DXVECTOR3(0.0f, 0.0f, 1.0f), 1.0f, 0.0f },
		{ 0.0625f, 0.0625f, 0.0625f, D3DXVECTOR3(0.0f, 0.0f, 1.0f), 0.0f, 0.0f },
	};

	// create the vertex buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	dev->CreateBuffer(&bd, NULL, &pVBuffer);

	// copy the vertices into the buffer
	D3D11_MAPPED_SUBRESOURCE ms;
	devcon->Map(pVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);    // map the buffer
	memcpy(ms.pData, OurVertices, sizeof(OurVertices));                 // copy the data
	devcon->Unmap(pVBuffer, NULL);
	// create the index buffer out of DWORDs

	DWORD OurIndices[] =
	{
		0, 1, 2, 
		2, 1, 3,
	};

	// create the index buffer
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(DWORD) * 36;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;

	dev->CreateBuffer(&bd, NULL, &pIBuffer);

	devcon->Map(pIBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);    // map the buffer
	memcpy(ms.pData, OurIndices, sizeof(OurIndices));                   // copy the data
	devcon->Unmap(pIBuffer, NULL);
	
	D3DX11CreateShaderResourceViewFromFile(dev,        // the Direct3D device
		objTexture,    // load Wood.png in the local folder
		NULL,           // no additional information
		NULL,           // no multithreading
		&pTexture,      // address of the shader-resource-view
		NULL);          // no multithreading
}
// this function loads and prepares the shaders
void InitPipeline()
{
    // compile the shaders
    ID3D10Blob *VS, *PS;
    D3DX11CompileFromFile(L"shaders.shader", 0, 0, "VShader", "vs_4_0", 0, 0, 0, &VS, 0, 0);
    D3DX11CompileFromFile(L"shaders.shader", 0, 0, "PShader", "ps_4_0", 0, 0, 0, &PS, 0, 0);

    // create the shader objects
    dev->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &pVS);
    dev->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &pPS);

    // set the shader objects
    devcon->VSSetShader(pVS, 0, 0);
    devcon->PSSetShader(pPS, 0, 0);

    // create the input element object
    D3D11_INPUT_ELEMENT_DESC ied[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    // use the input element descriptions to create the input layout
    dev->CreateInputLayout(ied, 3, VS->GetBufferPointer(), VS->GetBufferSize(), &pLayout);
    devcon->IASetInputLayout(pLayout);

    // create the constant buffer
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = 176;
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    dev->CreateBuffer(&bd, NULL, &pCBuffer);

    devcon->VSSetConstantBuffers(0, 1, &pCBuffer);
}
void update(gameData* newGame){
	float wallX[10000] = { 0 };
	float wallY[10000] = { 0 };
	for (int i = 0; i < newGame->grassCount; i++)
	{		
			wallX[i] = newGame->newWall[i].wallX;
			wallY[i] = newGame->newWall[i].wallY;
	}
	newGame->newPlayer->movePlayer(wallX, wallY, newGame->grassCount);
	for (int i = 0; i < newGame->enemyNo; i++)
	{
		newGame->enemy[i].moveEnemy();
		int collide = enemycollision(newGame, 0.1f, i);
		if (collide == 0)
		{
			newGame->newPlayer->vSpeed = 0.025f;
			newGame->enemy[i].xPos = -1000;
			newGame->enemy[i].yPos = -1000;
		}

		else if (collide == 1)
		{
			newGame->restart = true;
		}
	}
}

bool collisionDetection(gameData* newGame, int i, float collide, wall wall[]){
	float a = newGame->newPlayer->getPlayerPosition('x') - wall[i].wallX;
	a = a * a;
	float b = newGame->newPlayer->getPlayerPosition('y') - wall[i].wallY;
	b = b * b;
	float c = sqrt(a + b);
	if (c <= collide)
	{
		return true;
	}
	else
	{
		return false;
	}

}
bool collisionDetection(gameData* newGame, int i, float collide, gameEnemy enemy[])
{
	float a = newGame->newPlayer->getPlayerPosition('x') - enemy[i].xPos;
	a = a * a;
	float b = newGame->newPlayer->getPlayerPosition('y') - enemy[i].yPos;
	b = b * b;
	float c = sqrt(a + b);
	if (c <= collide){
		return true;
	}
	else
	{
		return false;
	}

}
int enemycollision(gameData* newGame, float collide, int i)
{
	
		float a = newGame->newPlayer->getPlayerPosition('x') - newGame->enemy[i].xPos;
		a = a * a;
		float b = newGame->newPlayer->getPlayerPosition('y') - newGame->enemy[i].yPos;
		b = b * b;
		float c = sqrt(a + b);
		if ((c <= collide) && (newGame->newPlayer->getPlayerPosition('y') > newGame->enemy[i].yPos))
		{
			return 0;
		}
		else if ((c <= collide) && (newGame->newPlayer->getPlayerPosition('y') <= newGame->enemy[i].yPos))
		{
			return 1;
		}
		else
		{
			return 2;
		}
}


void levelSetup(gameData* newGame, std::vector<std::string> Level) 
{
	for (int j = 0; j < (int)Level.size(); j++)
	{

		for (int i = 0; i < (int)Level[j].length(); i++)
		{
			if (Level[j][i] == 'U'){
				newGame->newSky[newGame->skyCount].wallY = (float)-j / 8.0f;
				newGame->newSky[newGame->skyCount].wallX = (float)-i / 8.0f;
				newGame->skyCount += 1;
			}
			else if (Level[j][i] == 'G'){
				newGame->newWall[newGame->grassCount].wallY = (float)-j / 8.0f;
				newGame->newWall[newGame->grassCount].wallX = (float)-i / 8.0f;
				newGame->grassCount += 1;
			}
			else if (Level[j][i] == 'F'){
				newGame->newDirtGrass[newGame->dirtGrassCount].wallY = (float)-j / 8.0f;
				newGame->newDirtGrass[newGame->dirtGrassCount].wallX = (float)-i / 8.0f;
				newGame->dirtGrassCount += 1;
			}
			else if (Level[j][i] == 'B'){
				newGame->newDirt[newGame->dirtCount].wallY = (float)-j / 8.0f;
				newGame->newDirt[newGame->dirtCount].wallX = (float)-i / 8.0f;
				newGame->dirtCount += 1;
			}
			else if (Level[j][i] == 'C'){
				newGame->newCloud[newGame->cloudCount].wallY = (float)-j / 8.0f;
				newGame->newCloud[newGame->cloudCount].wallX = (float)-i / 8.0f;
				newGame->cloudCount += 1;
			}
			else if (Level[j][i] == 'P'){
				newGame->newFlower[newGame->flowerCount].wallY = (float)-j / 8.0f;
				newGame->newFlower[newGame->flowerCount].wallX = (float)-i / 8.0f;
				newGame->flowerCount += 1;
			}
			else if (Level[j][i] == 'L'){
				newGame->newFlowerStem[newGame->flowerStemCount].wallY = (float)-j / 8.0f;
				newGame->newFlowerStem[newGame->flowerStemCount].wallX = (float)-i / 8.0f;
				newGame->flowerStemCount += 1;
			}
			else if (Level[j][i] == 'A'){
				newGame->newHillLeft[newGame->hillLeftCount].wallY = (float)-j / 8.0f;
				newGame->newHillLeft[newGame->hillLeftCount].wallX = (float)-i / 8.0f;
				newGame->hillLeftCount += 1;
			}
			else if (Level[j][i] == 'S'){
				newGame->newHillMiddle[newGame->hillMiddleCount].wallY = (float)-j / 8.0f;
				newGame->newHillMiddle[newGame->hillMiddleCount].wallX = (float)-i / 8.0f;
				newGame->hillMiddleCount += 1;
			}
			else if (Level[j][i] == 'D'){
				newGame->newHillRight[newGame->hillRightCount].wallY = (float)-j / 8.0f;
				newGame->newHillRight[newGame->hillRightCount].wallX = (float)-i / 8.0f;
				newGame->hillRightCount += 1;
			}
			else if (Level[j][i] == 'Q'){
				newGame->newMountLeft[newGame->MountLeftCount].wallY = (float)-j / 8.0f;
				newGame->newMountLeft[newGame->MountLeftCount].wallX = (float)-i / 8.0f;
				newGame->MountLeftCount += 1;
			}
			else if (Level[j][i] == 'W'){
				newGame->newMountMiddle[newGame->MountMiddleCount].wallY = (float)-j / 8.0f;
				newGame->newMountMiddle[newGame->MountMiddleCount].wallX = (float)-i / 8.0f;
				newGame->MountMiddleCount += 1;
			}
			else if (Level[j][i] == 'E'){
				newGame->newMountRight[newGame->MountRightCount].wallY = (float)-j / 8.0f;
				newGame->newMountRight[newGame->MountRightCount].wallX = (float)-i / 8.0f;
				newGame->MountRightCount += 1;
			}


			else {
				newGame->newError[newGame->errorCount].wallY = (float)-j / 8.0f;
				newGame->newError[newGame->errorCount].wallX = (float)-i / 8.0f;
				newGame->errorCount += 1;
			}
		}
	}
}

void counterSetup(int * counter[], std::vector<std::string> Level)
{
	for (int j = 0; j < (int)Level.size(); j++)
	{
		for (int i = 0; i < (int)Level[j].length(); i++)
		{
			if (Level[j][i] == 'U'){ counter[0] += 1; }
			else if (Level[j][i] == 'G'){ counter[1] += 1; }
			else if (Level[j][i] == 'F'){ counter[2] += 1; }
			else if (Level[j][i] == 'B'){ counter[3] += 1; }
			else if (Level[j][i] == 'C'){ counter[4] += 1; }
			else if (Level[j][i] == 'P'){ counter[5] += 1; }
			else if (Level[j][i] == 'L'){ counter[6] += 1; }
			else if (Level[j][i] == 'A'){ counter[7] += 1; }
			else if (Level[j][i] == 'S'){ counter[8] += 1; }
			else if (Level[j][i] == 'D'){ counter[9] += 1; }
			else if (Level[j][i] == 'Q'){ counter[10] += 1; }
			else if (Level[j][i] == 'W'){ counter[11] += 1; }
			else if (Level[j][i] == 'E'){ counter[12] += 1; }
			else { counter[13] += 1; }
		}
	}
}
void setPlayerLocation(gameData* newGame, float x, float y)
{
	newGame->newPlayer->setPlayerX(x);
	newGame->newPlayer->setPlayerY(y);
}

std::vector<std::string> loadLevel(std::string Level){
	std::ifstream file;
	file.open(Level);
	std::vector<std::string> _loadLevel;
	
	std::string line;

	while (getline(file, line)){
		_loadLevel.push_back(line);
	}
	file.close();
	return _loadLevel;
}



