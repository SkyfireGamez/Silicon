#include "pch.h"
#include "../SDK/Offsets.h"

#include "Krypton/Public/Configuration.h"

#include <Windows.h>
#include <iostream>

void ValidateSeasonAndVersion()
{
    if (!FConfiguration::bSeasonMapper) /// This ues a SPECIFIC season allower or smthing (i kinda forgot but its "FortniteSeason" in Offsets.h)
    {
        std::cout << "[GS] Season mapper is NOT used!!\n";
    }
    if (FConfiguration::bSeasonMapper)
    {
        // If no season is configured, skip validation entirely
        if (VersionInfo.FortniteSeason <= 0)
        {
            std::cout << "[GS] No FortniteSeason set in config, skipping season validation.\n";
            return;
        }

        // Sync config season into VersionInfo
        //VersionInfo.FortniteSeason = FConfiguration::FortniteSeason;


        // =========================
        // SEASON 0 (1.6.0 – 1.7.2)
        // =========================
        if (VersionInfo.FortniteSeason == 0)
        {
            if (VersionInfo.FortniteVersion < 1.6 ||
                VersionInfo.FortniteVersion > 1.72)
            {
                ExitProcess(0);
            }
        }

        // =========================
        // SEASON 1 (1.8 – 1.10)
        // =========================
        else if (VersionInfo.FortniteSeason == 1)
        {
            if (VersionInfo.FortniteVersion < 1.8 ||
                VersionInfo.FortniteVersion > 1.10)
            {
                ExitProcess(0);
            }
        }

        // =========================
        // SEASON 2 (2.0 – 2.5)
        // =========================
        else if (VersionInfo.FortniteSeason == 2)
        {
            if (VersionInfo.FortniteVersion < 2.0 ||
                VersionInfo.FortniteVersion > 2.5)
            {
                ExitProcess(0);
            }
        }

        // =========================
        // SEASON 3 (3.0 – 3.5)
        // =========================
        else if (VersionInfo.FortniteSeason == 3)
        {
            if (VersionInfo.FortniteVersion < 3.0 ||
                VersionInfo.FortniteVersion > 3.5)
            {
                ExitProcess(0);
            }
        }

        // =========================
        // UNSUPPORTED SEASON
        // =========================
        else
        {
            ExitProcess(0);
        }

        std::cout << "[GS] Season and Fortnite version validated successfully.\n";
        std::cout << "[GS] Allowed Season: " << VersionInfo.FortniteSeason << "\n";
    }
}
