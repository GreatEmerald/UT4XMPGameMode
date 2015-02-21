/* Copyright (C) 2015 Dainius "GreatEmerald" Masiliunas
 *
 * This file is part of UT4XMP Game Mode.
 *
 * UT4XMP Game Mode is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * UT4XMP Game Mode is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with UT4XMP Game Mode. If not, see <http://www.gnu.org/licenses/>.
 *
 * Additional permission under GNU GPL version 3 section 7:
 *
 * If you modify this Program, or any covered work, by linking or combining it
 * with Unreal Engine 4 (or a modified version of that program), containing
 * parts covered by the terms of the Unreal Engine End User License Agreement,
 * the licensors of this Program grant you additional permission to convey the
 * resulting work.
 */

#include "UnrealTournament.h"
#include "XMPGameMode.h"

#include "Core.h"
#include "Engine.h"
#include "ModuleManager.h"
#include "ModuleInterface.h"

class FXMPGameModePlugin : public IModuleInterface
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

IMPLEMENT_MODULE(FXMPGameModePlugin, XMPGameMode)
void FXMPGameModePlugin::StartupModule(){}
void FXMPGameModePlugin::ShutdownModule(){}
