// Copyright (c) 2026 Jasper Drescher. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"

#include "CkGamePlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCookiesChangedSignature, int32, NewCookieCount);

UCLASS()
class COOKIE_API ACkGamePlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ACkGamePlayerState();

	UPROPERTY(BlueprintAssignable, Category = "Cookies")
	FCookiesChangedSignature OnCookiesChanged;

	UFUNCTION(BlueprintCallable, Category = "Cookies")
	void AddCookie();

	UFUNCTION(BlueprintPure, Category = "Cookies")
	FORCEINLINE int32 GetCookies() const { return Cookies; }

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Team/Role")
	bool bIsHost = false;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Team/Role")
	FColor PlayerColor = FColor::White;

	UPROPERTY(ReplicatedUsing = OnRep_Cookies, BlueprintReadOnly, Category = "Cookies")
	int32 Cookies = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Team/Role")
	TArray<FColor> AvailablePlayerColors;

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_Cookies();

private:
	UFUNCTION()
	void PickRandomPlayerColor();
};
