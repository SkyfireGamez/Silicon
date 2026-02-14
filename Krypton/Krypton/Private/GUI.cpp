#include "pch.h"
#include "../Public/GUI.h"
#include <d3d11.h>
#include "../../ImGui/imgui.h"
#include "../../ImGui/imgui_impl_win32.h"
#include "../../ImGui/imgui_impl_dx11.h"
#include "../Public/Configuration.h"
#include "../Public/Events.h"
#include "../../FortniteGame/Public/BattleRoyaleGamePhaseLogic.h"
#include "../../FortniteGame/Public/BuildingSMActor.h"
#include <sstream>
#include <fstream>
#pragma comment(lib, "d3d11.lib")


#include <vector> // std
#include <mutex>
#include <cstdarg>
#include <cstdio>

#include <algorithm> // For std::count
#include <iostream>  // For printing output

#include <string>
#include <Windows.h>

///#include <string>

// forward declarations
bool IsValidRadminIP();
bool IsValidRadminIP(const std::string& typedIP);

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

UINT g_ResizeWidth = 0, g_ResizeHeight = 0;

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        g_ResizeWidth = (UINT)LOWORD(lParam);
        g_ResizeHeight = (UINT)HIWORD(lParam);
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

/*auto WindowWidth = 533;
auto WindowHeight = 400;*/

auto WindowWidth = 800;
auto WindowHeight = 600;

void GUI::Init()
{
    ImGui_ImplWin32_EnableDpiAwareness();
    float main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));

    WNDCLASS wc{};
    wc.lpszClassName = L"SiliconWC";
    wc.lpfnWndProc = WndProc;
    RegisterClass(&wc);

    wchar_t buffer[67];
    swprintf_s(buffer, VersionInfo.EngineVersion >= 5.0 ? L"Silicon (FN %.2f, UE %.1f)" : (VersionInfo.FortniteVersion >= 5.00 || VersionInfo.FortniteVersion < 1.2 ? L"Silicon (FN %.2f, UE %.2f)" : L"Silicon (FN %.1f, UE %.2f)"), VersionInfo.FortniteVersion, VersionInfo.EngineVersion);
    auto hWnd = CreateWindow(wc.lpszClassName, buffer, WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME, 100, 100, (int)(WindowWidth * main_scale), (int)(WindowHeight * main_scale), nullptr, nullptr, nullptr, nullptr);

    IDXGISwapChain* g_pSwapChain = nullptr;
    ID3D11Device* g_pd3dDevice = nullptr;
    ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED)
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res != S_OK)
        return;

    ID3D11RenderTargetView* g_mainRenderTargetView;

    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
    pBackBuffer->Release();

    ShowWindow(hWnd, SW_SHOWDEFAULT);
    UpdateWindow(hWnd);
    DWORD dwMyID = ::GetCurrentThreadId();
    DWORD dwCurID = ::GetWindowThreadProcessId(hWnd, NULL);
    AttachThreadInput(dwCurID, dwMyID, TRUE);
    SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
    SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOMOVE);
    SetForegroundWindow(hWnd);
    SetFocus(hWnd);
    SetActiveWindow(hWnd);
    AttachThreadInput(dwCurID, dwMyID, FALSE);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.IniFilename = NULL;
    //io.DisplaySize = ImGui::GetMainViewport()->Size;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImFontConfig FontConfig;
    FontConfig.FontDataOwnedByAtlas = false;
    ImGui::GetIO().Fonts->AddFontFromMemoryTTF((void*)font, sizeof(font), 17.f, &FontConfig);

    auto& mStyle = ImGui::GetStyle();
    mStyle.WindowRounding = 0.f;
    mStyle.ItemSpacing = ImVec2(20, 6);
    mStyle.ItemInnerSpacing = ImVec2(8, 4);
    mStyle.FrameRounding = 4.5f;
    mStyle.GrabMinSize = 14.0f;
    mStyle.GrabRounding = 16.0f;
    mStyle.ScrollbarSize = 12.0f;
    mStyle.ScrollbarRounding = 16.0f;
    mStyle.ChildRounding = 8.f;

    ImGuiStyle& style = mStyle;
    style.Colors[ImGuiCol_Text] = ImVec4(0.85f, 0.95f, 0.90f, 0.80f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.85f, 0.95f, 0.90f, 0.30f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.20f, 0.22f, 0.27f, 0.75f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.43f, 0.43f, 0.43f, 0.85f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.46f, 0.46f, 0.46f, 1.00f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.20f, 0.22f, 0.27f, 0.47f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.67f, 0.67f, 0.67f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.20f, 0.20f, 0.20f, 0.67f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.25f, 0.25f, 0.25f, 0.75f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.43f, 0.43f, 0.43f, 0.85f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.46f, 0.46f, 0.46f, 1.00f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.92f, 0.18f, 0.29f, 0.76f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.86f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.92f, 0.18f, 0.29f, 0.43f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.20f, 0.22f, 0.27f, 0.9f);
    style.Colors[ImGuiCol_Tab] = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
    style.Colors[ImGuiCol_TabSelected] = ImVec4(0.29f, 0.29f, 0.29f, 1.0f);
    style.Colors[ImGuiCol_TabHovered] = ImVec4(0.32f, 0.32f, 0.32f, 1.0f);
    //ImGui::StyleColorsDark();

    //ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);
    style.FontScaleDpi = main_scale;

    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    bool done = false;
    bool g_SwapChainOccluded = false;

    while (!done)
    {
        MSG msg;

        while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }

        if (done)
            break;

        if (g_SwapChainOccluded && g_pSwapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED)
        {
            Sleep(10);
            continue;
        }

        if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
        {
            g_mainRenderTargetView->Release();

            g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
            g_ResizeWidth = g_ResizeHeight = 0;

            ID3D11Texture2D* pBackBuffer;
            g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
            g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
            pBackBuffer->Release();
        }

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(WindowWidth * main_scale, WindowHeight * main_scale), ImGuiCond_Always);

        ImGui::Begin("Silicon", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);

        int SelectedUI = 0;
        int hasEvent = 0;

        

        if (hasEvent == 0)
        {
            hasEvent = 1;
            for (auto& Event : Events::EventsArray)
            {
                if (Event.EventVersion != VersionInfo.FortniteVersion)
                    continue;

                hasEvent = 2;
            }
        }
        if (ImGui::BeginTabBar(""))
        {
            if (ImGui::BeginTabItem("Main"))
            {


                SelectedUI = 0;
                ImGui::EndTabItem();
            }

            if (gsStatus == StartedMatch)
            {
                if (ImGui::BeginTabItem("Zones"))
                {
                    SelectedUI = 1;
                    ImGui::EndTabItem();
                }

                if (hasEvent == 2 && ImGui::BeginTabItem("Events"))
                {
                    SelectedUI = 10;
                    ImGui::EndTabItem();
                }
            }
            if (FConfiguration::bUsePlaylistTab) // make sure to comment out all the "( static inline auto Playlist L"Playlist_X"; )"  stuff to work
            {
                if (gsStatus == NotReady)
                {
                    if (ImGui::BeginTabItem("Playlist"))
                    {
                        SelectedUI = 2;
                        ImGui::EndTabItem();
                    }
                }
            }
            if (ImGui::BeginTabItem("Dump"))
            {
                SelectedUI = 4;
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Misc"))
            {
                SelectedUI = 3;
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Dev"))
            {
                SelectedUI = 5;
                ImGui::EndTabItem();
            }
            if (FConfiguration::bLateGame)
            {
                if (ImGui::BeginTabItem("LateGame"))
                {
                    SelectedUI = 9;
                    ImGui::EndTabItem();
                }

            };
            /*
            if (FConfiguration::bExploit)
            {
                if (FConfiguration::bAuthed)
                {
                    if (ImGui::BeginTabItem("Exploits"))
                    {
                        SelectedUI = 8;
                        ImGui::EndTabItem();
                    }
                }
            };
            */
            if (FConfiguration::bShowExtraTabs)
            {

                if (ImGui::BeginTabItem("Extra1"))
                {
                    SelectedUI = 11;
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Extra1"))
                {
                    SelectedUI = 12;
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Extra2"))
                {
                    SelectedUI = 13;
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Extra3"))
                {
                    SelectedUI = 14;
                    ImGui::EndTabItem();
                }

            };

            ImGui::EndTabBar();
        }

        static char commandBuffer[1024] = { 0 };
        auto GameMode = UWorld::GetWorld() ? (AFortGameMode*)UWorld::GetWorld()->AuthorityGameMode : nullptr;
        switch (SelectedUI)
        {
        case 0:
            if (gsStatus >= Joinable)
                ImGui::BeginChild("ServerInfo", ImVec2(245 * main_scale, 130 * main_scale), ImGuiChildFlags_Borders/*, ImGuiWindowFlags_ChildWindow | ImGuiWindowFlags_AlwaysHorizontallScrollbar */);
            ImGui::Text((std::string("Status: ") + (gsStatus == NotReady ? "Setting up the server..." : (gsStatus == Joinable ? "Joinable!" : "Match Started"))).c_str());

            if (gsStatus >= Joinable)
            {
                ImGui::Text((std::string("Player Count: ") + std::to_string(GameMode->AlivePlayers.Num())).c_str());
                ImGui::Text((std::string("Port: ") + std::to_string(FConfiguration::Port)).c_str());

                auto Playlist = VersionInfo.FortniteVersion >= 3.5 && GameMode->HasWarmupRequiredPlayerCount() ? (GameMode->GameState->HasCurrentPlaylistInfo() ? GameMode->GameState->CurrentPlaylistInfo.BasePlaylist : GameMode->GameState->CurrentPlaylistData) : nullptr;

                if (Playlist)
                {
                    FString Name = UKismetTextLibrary::Conv_TextToString(Playlist->UIDisplayName);
                    ImGui::Text((UEAllocatedString("Playlist: ") + Name.ToString()).c_str());
                }
                ImGui::Text((std::string("Running for ") + std::to_string((int)floor(UGameplayStatics::GetTimeSeconds(GameMode))) + "s").c_str());
            }
            if (gsStatus >= Joinable)
            {
                ImGui::EndChild();
                ImGui::Spacing();
            }


            
            if (gsStatus <= Joinable)
                ImGui::Checkbox("Lategame", &FConfiguration::bLateGame);

            if (gsStatus == Joinable && ImGui::Button("   Start Bus Early   "))
            {
                if (UFortGameStateComponent_BattleRoyaleGamePhaseLogic::GetDefaultObj())
                {
                    UFortGameStateComponent_BattleRoyaleGamePhaseLogic::bStartAircraft = true;
                    //auto GamePhaseLogic = UFortGameStateComponent_BattleRoyaleGamePhaseLogic::Get();

                    //GamePhaseLogic->StartAircraftPhase();
                }
                else
                    UKismetSystemLibrary::ExecuteConsoleCommand(UWorld::GetWorld(), FString(L"startaircraft"), nullptr);
            }
            
            /// EXPLOITS
            /*
            if (FConfiguration::bExploit)
            {

                ImGui::TextColored(ImVec4(1.f, 0.2f, 0.2f, 1.f), "Your DLL is Expoited!");
                ImGui::Separator();
            }
            if (FConfiguration::bExploit)
            {
                if (FConfiguration::bEAuth)
                {
                    static char TypedIP[32] = { 0 };
                    static bool bLoginPressed = false;

                    ImGui::InputText("IP Address", TypedIP, IM_ARRAYSIZE(TypedIP));
                    ImGui::SameLine();

                    if (ImGui::Button("Login"))
                    {
                        bLoginPressed = true;

                        // REQUIRE input
                        if (strlen(TypedIP) == 0)
                        {
                            FConfiguration::bAuthed = false;
                        }
                        else
                        {
                            FConfiguration::bAuthed = IsValidRadminIP(std::string(TypedIP));
                        }
                    }

                    // Status display
                    if (!bLoginPressed)
                    {
                        ImGui::TextColored(
                            ImVec4(1.f, 0.5f, 0.2f, 1.f),
                            "EAuth: None"
                        );
                    }
                    else if (FConfiguration::bAuthed)
                    {
                        ImGui::TextColored(
                            ImVec4(0.2f, 1.f, 0.2f, 1.f),
                            "EAuth: Success"
                        );
                    }
                    else
                    {
                        ImGui::TextColored(
                            ImVec4(1.f, 0.2f, 0.2f, 1.f),
                            "EAuth: Failed"
                        );
                    }

                    ImGui::Separator();
                }
            }
            */
            // EXPLOITS end
            


            ImGui::Spacing();

            ImGui::InputText("Console Command", commandBuffer, 1024);

            if (ImGui::Button("Execute"))
            {
                std::string str = commandBuffer;
                auto wstr = std::wstring(str.begin(), str.end());

                UKismetSystemLibrary::ExecuteConsoleCommand(UWorld::GetWorld(), FString(wstr.c_str()), nullptr);
            }
			ImGui::Checkbox("Auto Restart (EARLY BETA)", &FConfiguration::bAutoRestart);
            break;
        case 1:
            if (ImGui::Button("Pause Safe Zone"))
            {
                UFortGameStateComponent_BattleRoyaleGamePhaseLogic::bPausedZone = true;
                if (GameMode->HasbSafeZonePaused())
                    GameMode->bSafeZonePaused = true;
                UKismetSystemLibrary::ExecuteConsoleCommand(UWorld::GetWorld(), FString(L"pausesafezone"), nullptr);
            }

            if (ImGui::Button("Resume Safe Zone"))
            {
                UFortGameStateComponent_BattleRoyaleGamePhaseLogic::bPausedZone = false;
                if (GameMode->HasbSafeZonePaused())
                    GameMode->bSafeZonePaused = false;
                UKismetSystemLibrary::ExecuteConsoleCommand(UWorld::GetWorld(), FString(L"startsafezone"), nullptr);
            }

            if (ImGui::Button("Skip Safe Zone"))
            {
                if (GameMode->HasSafeZoneIndicator())
                {
                    if (GameMode->SafeZoneIndicator)
                    {
                        GameMode->SafeZoneIndicator->SafeZoneStartShrinkTime = (float)UGameplayStatics::GetTimeSeconds(UWorld::GetWorld());
                        GameMode->SafeZoneIndicator->SafeZoneFinishShrinkTime = GameMode->SafeZoneIndicator->SafeZoneStartShrinkTime + 0.05f;
                    }
                }
                else
                {
                    auto GamePhaseLogic = UFortGameStateComponent_BattleRoyaleGamePhaseLogic::Get(UWorld::GetWorld());

                    if (GamePhaseLogic->SafeZoneIndicator)
                    {
                        GamePhaseLogic->SafeZoneIndicator->SafeZoneStartShrinkTime = (float)UGameplayStatics::GetTimeSeconds(UWorld::GetWorld());
                        GamePhaseLogic->SafeZoneIndicator->SafeZoneFinishShrinkTime = GamePhaseLogic->SafeZoneIndicator->SafeZoneStartShrinkTime + 0.05f;
                    }
                }

                // UKismetSystemLibrary::ExecuteConsoleCommand(UWorld::GetWorld(), FString(L"skipsafezone"), nullptr);
            }

            if (ImGui::Button("Start Shrinking Safe Zone"))
            {
                if (GameMode->HasSafeZoneIndicator())
                {
                    if (GameMode->SafeZoneIndicator)
                        GameMode->SafeZoneIndicator->SafeZoneStartShrinkTime = (float)UGameplayStatics::GetTimeSeconds(UWorld::GetWorld());
                }
                else
                {
                    auto GamePhaseLogic = UFortGameStateComponent_BattleRoyaleGamePhaseLogic::Get(UWorld::GetWorld());

                    if (GamePhaseLogic->SafeZoneIndicator)
                        GamePhaseLogic->SafeZoneIndicator->SafeZoneStartShrinkTime = (float)UGameplayStatics::GetTimeSeconds(UWorld::GetWorld());
                }

                // UKismetSystemLibrary::ExecuteConsoleCommand(UWorld::GetWorld(), FString(L"startshrinksafezone"), nullptr);
            }

            break;


            break;

        case 2: // test1 tab - ducki67
        {

            ImGui::Text("PLAYLIST OPTIONS");

            if (ImGui::BeginTable("Table", 2))
            {

                ImGui::TableNextRow();

                ImGui::TableNextColumn();
                ImGui::Text("Playlist list");
                ImGui::BeginChild("Gamemodes", ImVec2(385, 200), true, ImGuiWindowFlags_HorizontalScrollbar);
                ImGui::Text("Gamemodes:");

                ImGui::RadioButton("BR Solo", &FPlaylist::PlaylistIndex, 0);
                ImGui::RadioButton("BR Duos", &FPlaylist::PlaylistIndex, 1);
                ImGui::RadioButton("Creative (7.20+)", &FPlaylist::PlaylistIndex, 2);
                ImGui::RadioButton("Playground (4.5 - 10.40)", &FPlaylist::PlaylistIndex, 3);
                ImGui::RadioButton("Arena Solo (8.51+)", &FPlaylist::PlaylistIndex, 4);
                ImGui::RadioButton("Tournament Solo", &FPlaylist::PlaylistIndex, 5);

                ImGui::RadioButton("Arsenal (Gun Game)", &FPlaylist::PlaylistIndex, 6);

                // Update active playlist
                FPlaylist::Playlist = FPlaylist::Playlists[FPlaylist::PlaylistIndex];

                ImGui::EndChild();


                ImGui::TableNextColumn();
                ImGui::Text("Custom Playlist settings");
                ImGui::BeginChild("CustomPlaylistsettingsChild", ImVec2(375, 200), true, ImGuiWindowFlags_HorizontalScrollbar);
                ImGui::Text("Settings will be here...");

                ImGui::Text("Uhm idfk skidda");


                ImGui::EndChild();


                ImGui::EndTable();
            }

            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Text("Playlist Path: %ls", FPlaylist::Playlist);
            ImGui::Spacing();
            ImGui::Separator();

            break;
        }

        case 3:
        {
			// Child box for server settings
            ImGui::Text("MISC OPTIONS:");
			ImGui::BeginChild("ServerSettings", ImVec2(0, 400), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);

            ImGui::Separator();
			ImGui::Spacing();
			ImGui::Text("Server Settings:");
            ImGui::SliderInt("Tick Rate:", &FConfiguration::MaxTickRate, 30, 120);
            ImGui::Checkbox("Allow CheatManager Clients", &FConfiguration::bEnableCheats);

			ImGui::Separator();
			ImGui::Spacing();
            ImGui::Text("Game Rules:");
            ImGui::Checkbox("Infinite Materials", &FConfiguration::bInfiniteMats);
            ImGui::Checkbox("Infinite Ammo", &FConfiguration::bInfiniteAmmo);
            ImGui::Checkbox("Keep Inventory", &FConfiguration::bKeepInventory);
			ImGui::Checkbox("Join Progess (BETA)", &FConfiguration::bJoinInProgress);
            ImGui::SliderInt("Siphon Amount:", &FConfiguration::SiphonAmount, 0, 200);

            
            ImGui::Checkbox("Allow ForceRespawn Clients", &FConfiguration::bForceRespawns);
            if (FConfiguration::bForceRespawns)
            {
                ImGui::SliderInt("RespawnHightClient (Client dlls) \n(Client.cpp Line: 173)", &FConfiguration::RespawnHightClient, 1000, 65000);
                ImGui::SliderInt("RespawnTimeClient (Client dlls) \n(Client.cpp Line: 179)", &FConfiguration::RespawnTimeClient, 3, 25);

                ImGui::SliderInt("RespawnHightGamemode (GS dlls) \n(FortGamemode.cpp Line: 74)", &FConfiguration::RespawnHightGamemode, 1000, 65000);
                ImGui::SliderInt("RespawnTimeGamemode (GS dlls) \n(FortGamemode.cpp Line: 80)", &FConfiguration::RespawnTimeGamemode, 3, 25);

			}
            ImGui::Separator();
			ImGui::Spacing();
			ImGui::Text("Other: (maybe later)");

            ImGui::EndChild();

            ImGui::Checkbox("Reset Builds Anim (Dont use thise for now skidda)", &FConfiguration::bAnim);


            if (ImGui::Button("Reset Builds"))
            {
                TArray<ABuildingSMActor*> Builds;
                Utils::GetAll<ABuildingSMActor>(Builds);

                if (FConfiguration::bAnim)
                {
                    for (auto& Build : Builds)
                        if (Build && Build->bPlayerPlaced)
                            Build->K2_DestroyActor();

                    std::cout << "[ReseBuilds] >> Reset WITH Animations\n";
                }
                else
                {
                    for (auto& Build : Builds)
                        if (Build && Build->bPlayerPlaced)
                            Build->SilentDie(true);

                    std::cout << "[ReseBuilds] >> Reset with NO Animations\n";
                }

                Builds.Free();
            }

            

        }
		

		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Text("NOTE: Some of these options may cause instability or crashes, use with caution!");// just a warning for the users, not gonna lie, some of these options can be pretty sketchy if you dont know what you are doing, so a warning is always good to have

       



            break;
        case 4:
            static auto PlaylistClass = UFortPlaylistAthena::StaticClass();

            if (ImGui::Button("Dump Items"))
            {
                std::stringstream ss;

                ss << "Generated by Silicon (https://github.com/plooshi/Silicon)\n";
                char version[6];

                sprintf_s(version, VersionInfo.FortniteVersion >= 5.00 || VersionInfo.FortniteVersion < 1.2 ? "%.2f" : "%.1f", VersionInfo.FortniteVersion);
                ss << "Fortnite version: " << version << "\n\n";

                auto RarityEnum = EFortRarity::StaticEnum();
                for (int i = 0; i < TUObjectArray::Num(); i++)
                {
                    auto Object = TUObjectArray::GetObjectByIndex(i);
                    if (!Object || !Object->Class || Object->IsDefaultObject() || !Object->IsA<UFortWorldItemDefinition>())
                        continue;
                    auto Item = (UFortWorldItemDefinition*)Object;

                    FString Name = UKismetTextLibrary::Conv_TextToString(Item->HasDisplayName() ? Item->DisplayName : Item->ItemName);

                    ss << "- " << UKismetSystemLibrary::GetPathName(Item).ToString() << "\n";
                    ss << "-     Name: " << (Name.GetData() ? Name.ToString() : "None") << "\n";

                    auto Names = *(TArray<TPair<FName, int64>>*)(__int64(RarityEnum) + 0x40);

                    for (int i = 0; i < Names.Num(); i++)
                    {
                        auto& Pair = Names[i];
                        auto& Name = Pair.Key();
                        auto& Value = Pair.Value();

                        if (Value == Item->Rarity)
                        {
                            auto str = Name.ToString();
                            auto colcolIdx = str.find_last_of("::");

                            auto RealName = colcolIdx == -1 ? str : str.substr(colcolIdx + 1);

                            ss << "-     Rarity: " << RealName << "\n";
                        }
                    }
                }

                std::ofstream of("DumpedItems.txt", std::ios::trunc);

                of << ss.str();
                of.close();
            }
            else if (PlaylistClass && ImGui::Button("Dump Playlists"))
            {
                std::stringstream ss;

                ss << "Generated by Silicon (https://github.com/plooshi/Silicon)\n";
                char version[6];

                sprintf_s(version, VersionInfo.FortniteVersion >= 5.00 || VersionInfo.FortniteVersion < 1.2 ? "%.2f" : "%.1f", VersionInfo.FortniteVersion);
                ss << "Fortnite version: " << version << "\n\n";

                auto RarityEnum = EFortRarity::StaticEnum();
                for (int i = 0; i < TUObjectArray::Num(); i++)
                {
                    auto Object = TUObjectArray::GetObjectByIndex(i);
                    if (!Object || !Object->Class || Object->IsDefaultObject() || !Object->IsA<UFortPlaylistAthena>())
                        continue;
                    auto Playlist = (UFortPlaylistAthena*)Object;

                    FString Name = UKismetTextLibrary::Conv_TextToString(Playlist->UIDisplayName);

                    ss << "- " << UKismetSystemLibrary::GetPathName(Playlist).ToString() << "\n";
                    ss << "-     Name: " << (Name.GetData() ? Name.ToString() : "None") << "\n";
                    if (Playlist->HasMaxPlayers())
                        ss << "-     Max players: " << std::to_string(Playlist->MaxPlayers) << "\n";
                    if (Playlist->HasMaxSquadSize())
                        ss << "-     Squad size: " << std::to_string(Playlist->MaxSquadSize) << "\n";
                }

                std::ofstream of("DumpedPlaylists.txt", std::ios::trunc);

                of << ss.str();
                of.close();
            }

            break;
        case 5:
        {

            static ImGuiTextBuffer LogBuffer;
            static bool bConsoleOpen = false;
            static bool bFormattedLogs = true;

            // ---- Top controls ----
            ImGui::Checkbox("Formatted logs", &bFormattedLogs);
            ImGui::SameLine();
            ImGui::Checkbox("Console attached", &bConsoleOpen);

            ImGui::SameLine();
            if (ImGui::Button("Clear"))
                LogBuffer.clear();
            ImGui::SameLine();

            if (ImGui::Button("Yap log"))
            {
                LogBuffer.append("[TEST]  Yap...\n");
                LogBuffer.append("[TEST]  Yap...\n");
                LogBuffer.append("[TEST]  Yap...\n");

            }

            ImGui::Separator();

            // ---- Console buttons ----
            if (ImGui::Button("Alloc Console") && !bConsoleOpen)
            {
                AllocConsole();
                FILE* fOut;
                FILE* fIn;
                freopen_s(&fOut, "CONOUT$", "w", stdout);
                freopen_s(&fIn, "CONIN$", "r", stdin);

                bConsoleOpen = true;
                LogBuffer.append("[INIT] Console allocated\n");
            }

            ImGui::SameLine();

            if (ImGui::Button("Dealloc Console") && bConsoleOpen)
            {
                FILE* dummy;
                freopen_s(&dummy, "NUL", "w", stdout);
                freopen_s(&dummy, "NUL", "r", stdin);

                FreeConsole();
                bConsoleOpen = false;
                LogBuffer.append("[SHUTDOWN] Console deallocated\n");
            }

            ImGui::Separator();

            // ---- Calculate remaining space for logs ----
            ImVec2 avail = ImGui::GetContentRegionAvail();

            ImGui::BeginChild(
                "LogsChild",
                ImVec2(avail.x, avail.y), // fills remaining space
                true,
                ImGuiWindowFlags_HorizontalScrollbar
            );

            const char* buf = LogBuffer.begin();
            const char* buf_end = LogBuffer.end();

            while (buf < buf_end)
            {
                const char* line_end = buf;
                while (line_end < buf_end && *line_end != '\n')
                    line_end++;

                if (bFormattedLogs)
                {
                    ImGui::TextColored(
                        ImVec4(0.7f, 0.3f, 1.0f, 1.0f),
                        "Silicon log:"
                    );
                    ImGui::SameLine();
                }

                ImVec4 color = ImVec4(1, 1, 1, 1);

                if (strstr(buf, "[ERROR]"))
                    color = ImVec4(1, 0.2f, 0.2f, 1);
                else if (strstr(buf, "[INIT]"))
                    color = ImVec4(0.2f, 0.6f, 1, 1);
                else if (strstr(buf, "[LOAD]"))
                    color = ImVec4(1, 0.85f, 0.2f, 1);

                ImGui::TextColored(
                    color,
                    "%.*s",
                    (int)(line_end - buf),
                    buf
                );

                buf = line_end + 1;
            }

            ImGui::SetScrollHereY(1.0f);
            ImGui::EndChild();

        }
        break;
        case 8:
        {
            ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "EXPLOITS!");
            ImGui::Spacing();

          
             
        }
        break;

         case 9:
         {
             /*if (VersionInfo.FortniteVersion < 1.2 || VersionInfo.FortniteVersion > 2.5)
             {*/
                 ImGui::Checkbox("Versionized Lootpool", &FConfiguration::bLateGameVersionized);
			 //}
             // Customer 7000 | U7K
             ImGui::Checkbox("Custom Late Game loadout  (Not fully done yet!!!)", &FConfiguration::bLateGameCustom);
             if (FConfiguration::bLateGameCustom)
             {

                 FConfiguration::bLateGameVersionized = false;

             // child box here
                //ImGui::SetNextWindowContentSize(ImVec2(150, 300));
                 ImGui::BeginChild("LGc", ImVec2(500, 200), true, ImGuiWindowFlags_HorizontalScrollbar);

				 ImGui::Text("Item Slots:");

                 // 1. Shotgun
                 char bufShotgun[500];
                 sprintf_s(bufShotgun, "%ls", FConfiguration::CustomShotgunItem);
                 if (ImGui::InputText("Shotgun", bufShotgun, 500)) {
                     swprintf_s(FConfiguration::CustomShotgunItem, L"%hs", bufShotgun);
                 }

                 // 2. Assault Rifle
                 char bufAssault[500];
                 sprintf_s(bufAssault, "%ls", FConfiguration::CustomAssaultRifleItem);
                 if (ImGui::InputText("Assault Rifle", bufAssault, 500)) {
                     swprintf_s(FConfiguration::CustomAssaultRifleItem, L"%hs", bufAssault);
                 }

                 // 3. Sniper
                 char bufSniper[500];
                 sprintf_s(bufSniper, "%ls", FConfiguration::CustomSniperItem);
                 if (ImGui::InputText("Sniper", bufSniper, 500)) {
                     swprintf_s(FConfiguration::CustomSniperItem, L"%hs", bufSniper);
                 }

                 // 4. Utility
                 char bufUtil[500];
                 sprintf_s(bufUtil, "%ls", FConfiguration::CustomUtilItem);
                 if (ImGui::InputText("Utility", bufUtil, 500)) {
                     swprintf_s(FConfiguration::CustomUtilItem, L"%hs", bufUtil);
                 }



                 //ImGui::SameLine();
                 ImGui::Spacing();
                 ImGui::Text("Item Count configs:");
                 // need a slider for FCconfiguration::CustomUtilItemCount
                 ImGui::SliderInt("Utility Item count", &FConfiguration::CustomUtilItemCount, 1, 99);


                 // ammo



                 ImGui::EndChild();
             }

             ImGui::Text("Zone Phase settings:");
             ImGui::Spacing();
             ImGui::SliderInt("Zone phase", &FConfiguration::LateGameZone, 3, 7); // pahse 7 is playeble ig
             // fix / not use Long Zones for pre-s13 so from 11.00 to 13.30
             if (VersionInfo.FortniteVersion < 11 || VersionInfo.FortniteVersion > 13.30)
             {
                 ImGui::Spacing();
                 ImGui::Spacing();
                 ImGui::Text("LateGame Zone Type:");
                 ImGui::Spacing();
                 ImGui::Checkbox("Long Zone", &FConfiguration::bLateGameLongZone);
             }

             ImGui::Separator();
             ImGui::Spacing();
             ImGui::Text("Quick Notes:");
             ImGui::BulletText("LG Zone Phase: Phases can be 3 - 7 !!!");
             ImGui::BulletText("Long Zone: A zone doesnt close for a long time.");
             ImGui::BulletText("Custom loadout: This option makes only 1 Loadout for everyone in the game. You can add your own WID's to it.");
         }
         break;
        case 10:
        {
            if (ImGui::Button("Start Event"))
                Events::StartEvent();
        }
        break;


        /*case 11:
        {
            ImGui::Text("Extra Tab!");
        }
        break;

        case 12:
        {
            ImGui::Text("Extra Tab!");
        }
        break;

        case 13:
        {
            ImGui::Text("Extra Tab!");
        }
        break;

        case 14:
        {
            ImGui::Text("Extra Tab!");
        }
        break;*/

        }

        ImGui::End();


        ImGui::Render();
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        HRESULT hr = g_pSwapChain->Present(1, 0);
        g_SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
    }

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    g_pSwapChain->Release();
    g_pd3dDeviceContext->Release();
    g_pd3dDevice->Release();
    DestroyWindow(hWnd);
    UnregisterClass(wc.lpszClassName, wc.hInstance);
    TerminateProcess(GetCurrentProcess(), 0);
}
