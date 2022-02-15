// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HairStrandsTranslator.h"
#include "cyHairFile.h"
/**
 * 
 */
class FCyHairTranslator : public IGroomTranslator
{
public:
	FCyHairTranslator();
	~FCyHairTranslator();

	virtual bool Translate(const FString& FilePath, FHairDescription& OutHairDescription, const FGroomConversionSettings& ConversionSettings) override;
	virtual bool CanTranslate(const FString& FilePath) override;
	virtual bool IsFileExtensionSupported(const FString& FileExtension) const override;
	virtual FString GetSupportedFormat() const override;
};
