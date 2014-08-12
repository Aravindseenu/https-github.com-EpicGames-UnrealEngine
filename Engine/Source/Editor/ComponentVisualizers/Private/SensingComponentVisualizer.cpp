// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "ComponentVisualizersPrivatePCH.h"
#include "SensingComponentVisualizer.h"
#include "Perception/PawnSensingComponent.h"


void FSensingComponentVisualizer::DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	if (View->Family->EngineShowFlags.VisualizeSenses)
	{
		const UPawnSensingComponent* Senses = Cast<const UPawnSensingComponent>(Component);
		if (Senses != NULL)
		{
			TArray<FVector> Verts;

			//LOS hearing
			if (Senses->LOSHearingThreshold > 0.0f)
			{
				DrawWireSphere(PDI, Senses->GetSensorLocation(), FColor::Yellow, Senses->LOSHearingThreshold, 16, SDPG_World);
			}

			//Hearing
			if (Senses->HearingThreshold > 0.0f)
			{
				DrawWireSphere(PDI, Senses->GetSensorLocation(), FColor::Cyan, Senses->HearingThreshold, 16, SDPG_World);
			}

			// Sight
			if (Senses->SightRadius > 0.0f)
			{
				DrawWireCone(PDI, FTranslationMatrix(Senses->GetSensorLocation()), Senses->SightRadius, Senses->GetPeripheralVisionAngle(), 10, FColor::Green, SDPG_World, Verts);
			}
		}
	}
}
