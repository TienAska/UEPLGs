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
			}
			pointIndex += vertexCount;
		}

		TVertexAttributesRef<float> vertexWidthArributeRef = OutHairDescription.VertexAttributes().GetAttributesRef<float>(HairAttribute::Vertex::Width);
		if (!vertexWidthArributeRef.IsValid())
		{
			OutHairDescription.VertexAttributes().RegisterAttribute<float>(HairAttribute::Vertex::Width);
			vertexWidthArributeRef = OutHairDescription.VertexAttributes().GetAttributesRef<float>(HairAttribute::Vertex::Width);
		}
		const float* thicknesses = hairFile.GetThicknessArray();
		for (size_t i = 0; i < hairFile.GetHeader().point_count; i++)
		{
			float thickness = thicknesses ? thicknesses[i] : hairFile.GetHeader().d_thickness;
			vertexWidthArributeRef[FVertexID(i)] = thickness;
		}

		//TVertexAttributesRef<FVector> vertexColorArributeRef = OutHairDescription.VertexAttributes().GetAttributesRef<FVector>(HairAttribute::Vertex::Color);
		//if (!vertexColorArributeRef.IsValid())
		//{
		//	OutHairDescription.VertexAttributes().RegisterAttribute<FVector>(HairAttribute::Vertex::Width);
		//	vertexColorArributeRef = OutHairDescription.VertexAttributes().GetAttributesRef<FVector>(HairAttribute::Vertex::Color);
		//}
		//float* colors = hairFile.GetColorsArray();
		//for (size_t i = 0; i < hairFile.GetHeader().point_count; i++)
		//{
		//	FVector color(hairFile.GetHeader().d_color[0], hairFile.GetHeader().d_color[1], hairFile.GetHeader().d_color[2]);
		//	if (colors) color.Set(colors[i * 3], colors[i * 3 + 1], colors[i * 3 + 2]);
		//	vertexColorArributeRef[FVertexID(i)] = color;
		//}
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
