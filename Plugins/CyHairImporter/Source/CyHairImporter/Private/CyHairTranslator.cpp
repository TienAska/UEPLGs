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
	uint32 hairCount = hairFile.LoadFromFile(TCHAR_TO_ANSI(*FilePath));
	if (hairCount < 0)
		return false;
	
	// Parse Object.
	{
		const uint16* segments = hairFile.GetSegmentsArray();
		const float* points = hairFile.GetPointsArray();
		const float* widthArray = hairFile.GetThicknessArray();
		bool bUseDWidth = (widthArray == nullptr);
		const float* colorArray = hairFile.GetColorsArray();
		bool bUseDColor = (colorArray == nullptr);
		for (size_t pointIndex = 0, hairIndex = 0; hairIndex < hairCount; hairIndex++)
		{
			int vertexCount = (segments ? segments[hairIndex] : hairFile.GetHeader().d_segments) + 1;

			FStrandID strandID = OutHairDescription.AddStrand();
			SetHairStrandAttribute(OutHairDescription, strandID, HairAttribute::Strand::VertexCount, vertexCount);
			for (size_t i = pointIndex; i < pointIndex + vertexCount; i++)
			{
				FVertexID vertexID = OutHairDescription.AddVertex();
				FVector position(points[i * 3], points[i * 3 + 1], points[i * 3 + 2]);
				SetHairVertexAttribute(OutHairDescription, vertexID, HairAttribute::Vertex::Position, position);
				
				if (!bUseDWidth)
					SetHairVertexAttribute(OutHairDescription, vertexID, HairAttribute::Vertex::Width, widthArray[i]);
				
				if (!bUseDColor)
				{
					FVector color(colorArray[i * 3], colorArray[i * 3 + 1], colorArray[i * 3 + 2]);
					SetHairVertexAttribute(OutHairDescription, vertexID, HairAttribute::Vertex::Color, color);
				}
			}
			pointIndex += vertexCount;
		}

		if (bUseDWidth)
			SetGroomAttribute(OutHairDescription, FGroomID(0), HairAttribute::Groom::Width, hairFile.GetHeader().d_thickness);
		if (bUseDColor)
		{
			const float* dColor = hairFile.GetHeader().d_color;
			FVector color(dColor[0], dColor[1], dColor[2]);
			SetGroomAttribute(OutHairDescription, FGroomID(0), HairAttribute::Groom::Color, color);
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
