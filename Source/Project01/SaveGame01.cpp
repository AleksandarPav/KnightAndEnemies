// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveGame01.h"

USaveGame01::USaveGame01()
{
	// default vrednosti
	PlayerName = TEXT("Default");
	UserIndex = 0;

	CharacterStats.WeaponName = TEXT("");
	CharacterStats.LevelName = TEXT("");
}