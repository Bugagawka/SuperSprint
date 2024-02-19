// Fill out your copyright notice in the Description page of Project Settings.


#include "SSSOnlineSubsystem.h"

#include <OnlineSubsystemUtils.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(SSSOnlineSubsystem)

bool FSSSSessionSettings::IsValid() const
{
	if (MaxPlayers < 2)
	{
		return false;
	}

	return true;
}

void USSSOnlineSubsystem::CreateSession(const FSSSSessionSettings SessionSettings)
{
	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (!sessionInterface.IsValid() || !SessionSettings.IsValid())
	{
		OnCreateSessionCompleted.Broadcast(false);
		return;
	}
	LastSessionSettings = MakeShareable<FOnlineSessionSettings>(new FOnlineSessionSettings());
	LastSessionSettings->NumPublicConnections = SessionSettings.MaxPlayers;
	LastSessionSettings->bAllowJoinInProgress = true;
	LastSessionSettings->bAllowJoinViaPresence = true;
	LastSessionSettings->bUsesPresence = true;
	LastSessionSettings->bIsLANMatch = SessionSettings.bLan;
	LastSessionSettings->bShouldAdvertise = true;

	LastSessionSettings->Set(FName(TEXT("SETTING_MAPNAME")), SessionSettings.LevelName.ToString(), EOnlineDataAdvertisementType::ViaOnlineService);

	OnCreateSessionCompleteHandle = sessionInterface->AddOnCreateSessionCompleteDelegate_Handle(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnSessionCreated));

	const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();

	const bool bSessionCreated = sessionInterface->CreateSession(*localPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *LastSessionSettings);

	if (!bSessionCreated)
	{
		sessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteHandle);

		OnCreateSessionCompleted.Broadcast(false);
	}
}

void USSSOnlineSubsystem::OnSessionCreated(FName SessionName, bool bSuccessful)
{
	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (sessionInterface)
	{
		sessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteHandle);
	}

	OnCreateSessionCompleted.Broadcast(bSuccessful);
}

void USSSOnlineSubsystem::StartSession()
{
	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (!sessionInterface.IsValid())
	{
		OnStartSessionCompleted.Broadcast(false);
		return;
	}

	OnStartSessionCompleteHandle = sessionInterface->AddOnStartSessionCompleteDelegate_Handle(FOnStartSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnSessionStarted));

	if (!sessionInterface->StartSession(NAME_GameSession))
	{
		sessionInterface->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteHandle);
		OnStartSessionCompleted.Broadcast(false);
	}
}

void USSSOnlineSubsystem::OnSessionStarted(FName SessionName, bool bSuccessful)
{
	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (sessionInterface)
	{
		sessionInterface->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteHandle);
	}

	OnStartSessionCompleted.Broadcast(bSuccessful);

	FString levelName;
	LastSessionSettings.Get()->Get(FName(TEXT("SETTING_MAPNAME")), levelName);

	GetWorld()->ServerTravel(levelName + TEXT("?listen"), true);
}

void USSSOnlineSubsystem::UpdateSession(const FSSSSessionSettings SessionSettings)
{
	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (!sessionInterface.IsValid() || !SessionSettings.IsValid())
	{
		OnUpdateSessionCompleted.Broadcast(false);
		return;
	}

	TSharedPtr<FOnlineSessionSettings> updatedSessionSettings = MakeShareable<FOnlineSessionSettings>(new FOnlineSessionSettings(*LastSessionSettings));
	updatedSessionSettings->NumPublicConnections = SessionSettings.MaxPlayers;
	updatedSessionSettings->bIsLANMatch = SessionSettings.bLan;
	updatedSessionSettings->Set(FName(TEXT("SETTING_MAPNAME")), SessionSettings.LevelName.ToString(), EOnlineDataAdvertisementType::ViaOnlineService);

	OnUpdateSessionCompleteHandle = sessionInterface->AddOnUpdateSessionCompleteDelegate_Handle(FOnUpdateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnSessionUpdated));

	if (sessionInterface->UpdateSession(NAME_GameSession, *updatedSessionSettings))
	{
		LastSessionSettings = updatedSessionSettings;
	}
	else
	{
		sessionInterface->ClearOnUpdateSessionCompleteDelegate_Handle(OnUpdateSessionCompleteHandle);
		OnUpdateSessionCompleted.Broadcast(false);
	}
}

void USSSOnlineSubsystem::OnSessionUpdated(FName SessionName, bool bSuccessful)
{
	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (sessionInterface)
	{
		sessionInterface->ClearOnUpdateSessionCompleteDelegate_Handle(OnUpdateSessionCompleteHandle);
	}

	OnUpdateSessionCompleted.Broadcast(bSuccessful);
}

void USSSOnlineSubsystem::FindSessions(int32 MaxSearchResults, bool bLan)
{
	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (!sessionInterface.IsValid())
	{
		OnSessionsFound.Broadcast(TArray<FSSSSessionResult>());
		return;
	}

	OnFindSessionsCompleteHandle = sessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSession));

	LastSessionSearch = MakeShareable(new FOnlineSessionSearch());
	LastSessionSearch->MaxSearchResults = MaxSearchResults;
	LastSessionSearch->bIsLanQuery = bLan;
	LastSessionSearch->QuerySettings.Set(FName(TEXT("PRESENCESEARCH")), true, EOnlineComparisonOp::Equals);

	const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!sessionInterface->FindSessions(*localPlayer->GetPreferredUniqueNetId(), LastSessionSearch.ToSharedRef()))
	{
		sessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteHandle);

		OnSessionsFound.Broadcast(TArray<FSSSSessionResult>());
	}
}

void USSSOnlineSubsystem::OnFindSession(bool bSuccessful)
{
	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (sessionInterface)
	{
		sessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteHandle);
	}

	if (!bSuccessful || LastSessionSearch->SearchResults.Num() <= 0)
	{
		OnSessionsFound.Broadcast(TArray<FSSSSessionResult>());
		return;
	}

	TArray<FSSSSessionResult> Sessions;

	for (FOnlineSessionSearchResult& searchResult : LastSessionSearch->SearchResults)
	{
		FSSSSessionResult session;
		session.SessionResult = searchResult;
		Sessions.Add(session);
	}

	OnSessionsFound.Broadcast(Sessions);
}

bool USSSOnlineSubsystem::IsValid(const FSSSSessionResult& Session)
{
	return Session.SessionResult.IsSessionInfoValid() && Session.SessionResult.Session.NumOpenPublicConnections > 0;
}

int32 USSSOnlineSubsystem::GetPingInMs(const FSSSSessionResult& Session)
{
	return Session.SessionResult.PingInMs;
}

FString USSSOnlineSubsystem::GetHostName(const FSSSSessionResult& Session)
{
	return Session.SessionResult.Session.OwningUserName;
}

FString USSSOnlineSubsystem::GetMapName(const FSSSSessionResult& Session)
{
	FString mapName;
	Session.SessionResult.Session.SessionSettings.Get(FName(TEXT("SETTING_MAPNAME")), mapName);
	return mapName;
}

int32 USSSOnlineSubsystem::GetCurrentPlayers(const FSSSSessionResult& Session)
{
	return Session.SessionResult.Session.SessionSettings.NumPublicConnections - Session.SessionResult.Session.NumOpenPublicConnections;
}

int32 USSSOnlineSubsystem::GetMaxPlayers(const FSSSSessionResult& Session)
{
	return Session.SessionResult.Session.SessionSettings.NumPublicConnections;
}

void USSSOnlineSubsystem::JoinSession(const FSSSSessionResult& Session)
{
	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (!sessionInterface.IsValid())
	{
		OnJoinSessionCompleted.Broadcast(false);
		return;
	}

	OnJoinSessionsCompleteHandle = sessionInterface->AddOnJoinSessionCompleteDelegate_Handle(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnSessionJoined));

	const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!sessionInterface->JoinSession(*localPlayer->GetPreferredUniqueNetId(), NAME_GameSession, Session.SessionResult))
	{
		sessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionsCompleteHandle);

		OnJoinSessionCompleted.Broadcast(false);
	}
}

void USSSOnlineSubsystem::OnSessionJoined(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (sessionInterface)
	{
		sessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionsCompleteHandle);
	}

	OnJoinSessionCompleted.Broadcast(true);

	FString connectString;

	if (!sessionInterface->GetResolvedConnectString(NAME_GameSession, connectString))
	{
		return;
	}

	APlayerController* playerController = GetWorld()->GetFirstPlayerController();
	playerController->ClientTravel(connectString, TRAVEL_Absolute);
}

void USSSOnlineSubsystem::DestroySession()
{
	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (!sessionInterface.IsValid())
	{
		OnDestroySessionCompleted.Broadcast(false);
		return;
	}

	OnDestroySessionCompleteHandle = sessionInterface->AddOnDestroySessionCompleteDelegate_Handle(FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnSessionDestroyed));

	if (!sessionInterface->DestroySession(NAME_GameSession))
	{
		sessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteHandle);

		OnDestroySessionCompleted.Broadcast(false);
	}
}

void USSSOnlineSubsystem::OnSessionDestroyed(FName SessionName, bool bSuccessful)
{
	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (sessionInterface)
	{
		sessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteHandle);
	}

	OnDestroySessionCompleted.Broadcast(bSuccessful);
}
