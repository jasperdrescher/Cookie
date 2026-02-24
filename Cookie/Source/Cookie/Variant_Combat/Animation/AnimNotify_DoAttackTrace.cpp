// Copyright (c) 2026 Jasper Drescher. All rights reserved.

#include "AnimNotify_DoAttackTrace.h"

#include "CombatAttacker.h"
#include "Components/SkeletalMeshComponent.h"

void UAnimNotify_DoAttackTrace::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	// cast the owner to the attacker interface
	if (ICombatAttacker* AttackerInterface = Cast<ICombatAttacker>(MeshComp->GetOwner()))
	{
		AttackerInterface->DoAttackTrace(AttackBoneName);
	}
}

FString UAnimNotify_DoAttackTrace::GetNotifyName_Implementation() const
{
	return FString("Do Attack Trace");
}
