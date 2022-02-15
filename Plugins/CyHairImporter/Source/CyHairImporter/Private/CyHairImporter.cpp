// Copyright Epic Games, Inc. All Rights Reserved.

#include "CyHairImporter.h"

#include "CyHairTranslator.h"
#include "HairStrandsEditor.h"

#define LOCTEXT_NAMESPACE "FCyHairImporterModule"

void FCyHairImporterModule::StartupModule()
{
	FGroomEditor::Get().RegisterHairTranslator<FCyHairTranslator>();
}

void FCyHairImporterModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCyHairImporterModule, CyHairImporter)