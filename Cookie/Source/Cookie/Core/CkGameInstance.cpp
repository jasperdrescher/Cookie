// Copyright (c) 2025 Jasper Drescher. All rights reserved.

#include "Core/CkGameInstance.h"

#include "Cookie.h"
#include "ThirdParty/Steamworks/Steamv161/sdk/public/steam/steam_api.h"

void UCkGameInstance::SetSteamRichPresence(const FString& Key, const FString& Value)
{
	if (SteamAPI_Init() && SteamFriends() != nullptr)
	{
		UE_LOG(LogCookie, Log, TEXT("Attempting to set Steam Rich Presence for key: %s, value: %s"), *Key, *Value);

		const bool bResult = SteamFriends()->SetRichPresence(TCHAR_TO_UTF8(*Key), TCHAR_TO_UTF8(*Value));
		if (bResult)
		{
			UE_LOG(LogCookie, Log, TEXT("Set Steam Rich Presence"));
		}
		else
		{
			UE_LOG(LogCookie, Warning, TEXT("Failed to set Steam Rich Presence"));
		}
	}
	else
	{
		UE_LOG(LogCookie, Warning, TEXT("Steam is not running or the SteamFriends interface is unavailable"));
	}
}

void UCkGameInstance::ClearSteamRichPresence()
{
	if (SteamAPI_Init() && SteamFriends() != nullptr)
	{
		UE_LOG(LogCookie, Log, TEXT("Attempting to clear Steam Rich Presence"));

		SteamFriends()->ClearRichPresence();
	}
	else
	{
		UE_LOG(LogCookie, Warning, TEXT("Steam is not running or the SteamFriends interface is unavailable"));
	}
}
