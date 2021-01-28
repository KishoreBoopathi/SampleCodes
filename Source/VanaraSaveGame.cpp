// Fill out your copyright notice in the Description page of Project Settings.


#include "VanaraSaveGame.h"
#include "QuestLog.h"
#include "Quest.h"

UVanaraSaveGame::UVanaraSaveGame()
{
	PlayerName = TEXT("Default");
	UserIndex = 0;

	PlayerStats.WeaponName = TEXT("");
}