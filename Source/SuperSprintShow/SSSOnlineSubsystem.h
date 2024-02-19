// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include <OnlineSessionSettings.h>
#include <Interfaces/OnlineSessionInterface.h>

#include "SSSOnlineSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FSSSSessionSettings
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	int32 MaxPlayers;

	UPROPERTY(BlueprintReadWrite)
	bool bLan = true;

	UPROPERTY(BlueprintReadWrite)
	FName LevelName;

	bool IsValid() const;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSessionDelegate, bool, Successful);

USTRUCT(BlueprintType)
struct FSSSSessionResult
{
	GENERATED_BODY()

	FOnlineSessionSearchResult SessionResult;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSessionsFoundDelegate, const TArray<FSSSSessionResult>&, SessionResults);

UCLASS()
class SUPERSPRINTSHOW_API USSSOnlineSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void CreateSession(const FSSSSessionSettings SessionSettings);
	void OnSessionCreated(FName SessionName, bool bSuccessful);
	UPROPERTY(BlueprintAssignable)
	FOnSessionDelegate OnCreateSessionCompleted;

	UFUNCTION(BlueprintCallable)
	void StartSession();
	void OnSessionStarted(FName SessionName, bool bSuccessful);
	UPROPERTY(BlueprintAssignable)
	FOnSessionDelegate OnStartSessionCompleted;

	UFUNCTION(BlueprintCallable)
	void UpdateSession(const FSSSSessionSettings SessionSettings);
	void OnSessionUpdated(FName SessionName, bool bSuccessful);
	UPROPERTY(BlueprintAssignable)
	FOnSessionDelegate OnUpdateSessionCompleted;

	UFUNCTION(BlueprintCallable)
	void FindSessions(int32 MaxSearchResults, bool bLan);
	void OnFindSession(bool bSuccessful);
	UPROPERTY(BlueprintAssignable)
	FOnSessionsFoundDelegate OnSessionsFound;

	UFUNCTION(BlueprintPure)
	bool IsValid(const FSSSSessionResult& Session);

	UFUNCTION(BlueprintPure)
	int32 GetPingInMs(const FSSSSessionResult& Session);

	UFUNCTION(BlueprintPure)
	FString GetHostName(const FSSSSessionResult& Session);

	UFUNCTION(BlueprintPure)
	FString GetMapName(const FSSSSessionResult& Session);

	UFUNCTION(BlueprintPure)
	int32 GetCurrentPlayers(const FSSSSessionResult& Session);

	UFUNCTION(BlueprintPure)
	int32 GetMaxPlayers(const FSSSSessionResult& Session);

	UFUNCTION(BlueprintCallable)
	void JoinSession(const FSSSSessionResult& Session);
	void OnSessionJoined(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	UPROPERTY(BlueprintAssignable)
	FOnSessionDelegate OnJoinSessionCompleted;

	UFUNCTION(BlueprintCallable)
	void DestroySession();
	void OnSessionDestroyed(FName SessionName, bool bSuccessful);
	UPROPERTY(BlueprintAssignable)
	FOnSessionDelegate OnDestroySessionCompleted;

private:
	TSharedPtr<FOnlineSessionSettings> LastSessionSettings;
	FDelegateHandle OnCreateSessionCompleteHandle;
	FDelegateHandle OnStartSessionCompleteHandle;
	FDelegateHandle OnUpdateSessionCompleteHandle;

	TSharedPtr<FOnlineSessionSearch> LastSessionSearch;
	FDelegateHandle OnFindSessionsCompleteHandle;

	FDelegateHandle OnJoinSessionsCompleteHandle;
	FDelegateHandle OnDestroySessionCompleteHandle;
};
