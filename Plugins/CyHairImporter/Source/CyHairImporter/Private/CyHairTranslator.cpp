// Fill out your copyright notice in the Description page of Project Settings.


#include "CyHairTranslator.h"

#include "GroomCache.h"
#include "GroomImportOptions.h"
//#include "HAL/IConsoleManager.h"
#include "HairDescription.h"
//#include "Misc/Paths.h"

FCyHairTranslator::FCyHairTranslator()
{
}

FCyHairTranslator::~FCyHairTranslator()
{
	EndTranslation();
}

bool FCyHairTranslator::Translate(const FString& FilePath, FHairDescription& OutHairDescription, const FGroomConversionSettings& ConversionSettings)
{
	cyHairFile hairFile;
	int result = hairFile.LoadFromFile(TCHAR_TO_ANSI(*FilePath));
	if (result < 0)
		return false;
	
	// Parse Object.
	{

		uint32 pointIndex = 0;
		uint32 hairCount = hairFile.GetHeader().hair_count;
		const uint16* segments = hairFile.GetSegmentsArray();
		for (size_t hairIndex = 0; hairIndex < hairCount; pointIndex += segments[hairIndex++] + 1)
		{
			const float* points = hairFile.GetPointsArray();

			FStrandID strandID = OutHairDescription.AddStrand();
			SetHairStrandAttribute(OutHairDescription, strandID, HairAttribute::Strand::VertexCount, segments[hairIndex] + 1);
			for (size_t i = pointIndex; i < pointIndex + segments[hairIndex] + 1; i++)
			{
				FVertexID vertexID = OutHairDescription.AddVertex();
				FVector position(points[i * 3], points[i * 3 + 1], points[i * 3 + 2]);

				SetHairVertexAttribute(OutHairDescription, vertexID, HairAttribute::Vertex::Position, position);
			}
		}
	}

	return true;
}

bool FCyHairTranslator::CanTranslate(const FString& FilePath)
{
	if (!IsFileExtensionSupported(FPaths::GetExtension(FilePath)))
	{
		return false;
	}

	return true;
}

bool FCyHairTranslator::IsFileExtensionSupported(const FString& FileExtension) const
{
	return GetSupportedFormat().StartsWith(FileExtension);
}

FString FCyHairTranslator::GetSupportedFormat() const
{
	return TEXT("hair;Cem Yuksel hair file");
}
