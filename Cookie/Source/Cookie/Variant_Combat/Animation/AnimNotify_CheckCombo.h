// Copyright (c) 2026 Jasper Drescher. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"

#include "AnimNotify_CheckCombo.generated.h"

/**
 *  AnimNotify to perform a combo string check.
 */
UCLASS()
class UAnimNotify_CheckCombo : public UAnimNotify
{
	GENERATED_BODY()
	
public:

	/** Perform the Anim Notify */
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	/** Get the notify name */
	virtual FString GetNotifyName_Implementation() const override;
};
