#pragma once


/// IF this config is very messy for u then idgf  cuz i wont make this easier for you skidda!!
/// Also theres a LOT of expermental shit here so just read ALL comments!!!!!!
//NOTE: before u say all this is "AI" NO only the Playlist tab for experentall stuff in GUI.cpp IS Ai made!

struct FConfiguration
{
    /// Playlist selectorV1

    static inline auto Playlist = L"/Game/Athena/Playlists/Playlist_DefaultSolo.Playlist_DefaultSolo";
    //static inline auto Playlist = L"/Game/Athena/Playlists/Creative/Playlist_PlaygroundV2.Playlist_PlaygroundV2";
    //static inline auto Playlist = L"/Game/Athena/Playlists/Playground/Playlist_Playground.Playlist_Playground";
    //static inline auto Playlist = L"/Game/Athena/Playlists/Showdown/Playlist_ShowdownAlt_Solo.Playlist_ShowdownAlt_Solo";
    //static inline auto Playlist = L"/Game/Athena/Playlists/Showdown/Playlist_Showdown_Solo.Playlist_Showdown_Solo";



    static inline auto MaxTickRate = 120; // 120 is pretty good because it's smooth and optimized, it works good on low/high end too

    
    ///  dont juse this untill its done its like s shit from reboot but in my way
    static inline auto bSeasonMapper = true; /// This ues a SPECIFIC season allower or smthing (i kinda forgot but its "FortniteSeason" in Offsets.h)
    // + this is  for webhook so shows the season / CORRECT version (not done)
    

    // - late game stuff

    static inline auto bTestingforLG = false;
    static inline auto bClientConsole67K = false; // not added yet


    static inline auto bLateGame = false;
    static inline auto bLateGameVersionized = false; // (V2)  a Loot controller between the OLD and NEW "versionized" Late Game (DONT TURN THIS OFF if ur runing version below 4.5!!)
    static inline auto bLateGameCustom = false; // Custom LG loadout maker and settings for gui (BETA)
    static inline auto LateGameZone = 4; // starting zone
    static inline auto bLateGameLongZone = false; // zone doesnt close for a long time


    // game rules (mainly for LG)
    static inline auto bInfiniteMats = true;
    static inline auto bInfiniteAmmo = true;
    static inline auto bForceRespawns = true; // build your client with this too!
    static inline int RespawnHightClient = 10000; // respawn hight value (Client.cpp Line: 173)
    static inline int RespawnTimeClient = 3; // respawnt time for clients as value (Client.cpp Line: 179)
    static inline int RespawnHightGamemode = 10000; // (FortGamemode.cpp Line: 74)
    static inline int RespawnTimeGamemode = 3;// (FortGamemode.cpp Line: 80)
    static inline auto bJoinInProgress = false;
    static inline auto bKeepInventory = false;



    // - server stuff
    static inline auto bEnableCheats = true;
    static inline auto SiphonAmount = 50; // set to 0 to disable
    static inline auto bAutoRestart = false;
    static inline auto Port = 7777;
    // static inline auto JoinIp = L"open 127.0.0.1"; // for webhooks
    static inline constexpr auto bEnableIris = true;
    static inline constexpr auto bGUI = true;
    static inline constexpr auto bCustomCrashReporter = true;
    static inline constexpr auto bUseStdoutLog = false;
    static inline constexpr auto WebhookURL = ""; // fill in if you want status to send to a webhook

    static inline auto PlotDefinition = L"/Game/Playgrounds/Items/Plots/TheBlock_Season7"; // put any map / plot here (e.g. "/Game/Playgrounds/Items/Plots/TheBlock_Season7" The Block from S7)
    static inline auto bMagicMethodCreative = true;  // magic cool fantastic ud creaative method Silicon 2025 -miko


	/// DONT CHANGE THESE SHITS | Some stuff are GUI related so be careful when changing them
    static inline auto bUsePlaylistTab = false; 
    static inline auto bShowExtraTabs = false; // oops might not work

    static inline bool bAnim = true;  // gui stuff dont change it plz
    static inline bool bEnablePlaylistTab = true; // not added yet

    static inline wchar_t CustomShotgunItem[500] = L"/Game/Athena/Items/Weapons/WID_Shotgun_Standard_Athena_SR_Ore_T03.WID_Shotgun_Standard_Athena_SR_Ore_T03";
    static inline wchar_t CustomAssaultRifleItem[500] = L"/Game/Athena/Items/Weapons/WID_Assault_Heavy_Athena_SR_Ore_T03.WID_Assault_Heavy_Athena_SR_Ore_T03";
    static inline wchar_t CustomSniperItem[500] = L"/Game/Athena/Items/Weapons/WID_Sniper_Heavy_Athena_SR_Ore_T03.WID_Sniper_Heavy_Athena_SR_Ore_T03";
    static inline wchar_t CustomUtilItem[500] = L"/Game/Athena/Items/Consumables/ShockwaveGrenade/Athena_ShockGrenade.Athena_ShockGrenade";
    static inline int CustomUtilItemCount = 1;


    
    // currentlz not finished  yet BUT tested on 1.11 it should work ig
    static inline auto bUseCustomShit = false; // DONT ENABLE THIS SHIT IF U DONT KNOW WHAT YOU ARE DOING!!!! (ALSO Check FortGameMode.cpp before yout use this!!)
    static inline auto CustomBusDef = L"/Game/Athena/Items/Cosmetics/BattleBuses/BBID_DefaultBus.BBID_DefaultBus"; // bus def
    static inline auto CustomSupplyDropDef = L"/Game/Athena/SupplyDrops/AthenaSupplyDrop.AthenaSupplyDrop_C"; // default supply drop maybe  | Default: /Game/Athena/SupplyDrops/AthenaSupplyDrop.AthenaSupplyDrop_C
    
    
};


/// Very expermental

// Playlist selectorV2 |  used for GUI plz enable "bUsePlaylistTab" to use it!!
struct FPlaylist
{
    
    // This is the ONLY thing that matters
    static inline int PlaylistIndex = 0;

    // Playlist paths
    static inline const wchar_t* Playlists[] =
    {
        L"/Game/Athena/Playlists/Playlist_DefaultSolo.Playlist_DefaultSolo", // 0
        L"/Game/Athena/Playlists/Playlist_DefaultDuo.Playlist_DefaultDuo",   // 1
        L"/Game/Athena/Playlists/Creative/Playlist_PlaygroundV2.Playlist_PlaygroundV2", // 2
        L"/Game/Athena/Playlists/Playground/Playlist_Playground.Playlist_Playground",   // 3

        L"/Game/Athena/Playlists/Showdown/Playlist_ShowdownAlt_Solo.Playlist_ShowdownAlt_Solo", // 4
        L"/Game/Athena/Playlists/Showdown/Playlist_Showdown_Solo.Playlist_Showdown_Solo", // 5


        L"/Game/Athena/Playlists/gg/Playlist_Gg_Reverse.Playlist_Gg_Reverse" // 6
    };

    // Used by the GameServer
    static inline const wchar_t* Playlist = Playlists[PlaylistIndex];
};

