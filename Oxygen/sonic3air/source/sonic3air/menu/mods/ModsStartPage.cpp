/*
*	Part of the Oxygen Engine / Sonic 3 A.I.R. software distribution.
*	Copyright (C) 2017-2026 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#include "sonic3air/pch.h"
#include "sonic3air/menu/mods/ModsStartPage.h"
#include "sonic3air/menu/SharedResources.h"

#include "oxygen/platform/PlatformFunctions.h"


namespace
{
	enum class Option
	{
		OPEN_MODS_FOLDER = 0xfff0,
		OPEN_MANUAL		 = 0xfff1,
		GO_BACK			 = 0xffff,
	};
}


void ModsStartPage::initialize()
{
	if (!mMenuEntries.empty())
		return;

#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_MAC) || defined(PLATFORM_LINUX)
	mMenuEntries.addEntry("Abrir Carpeta de Mods " PLATFORM_DIRECTORY_STRING, (uint32)Option::OPEN_MODS_FOLDER);
#endif

	mMenuEntries.addEntry("Abrir Manual en el navegador", (uint32)Option::OPEN_MANUAL);
	mMenuEntries.addEntry("Volver", (uint32)Option::GO_BACK);
}

bool ModsStartPage::update(float timeElapsed)
{
	const InputManager::ControllerScheme& keys = InputManager::instance().getController(0);

	enum class ButtonEffect
	{
		NONE,
		ACCEPT,
		BACK
	};
	const ButtonEffect buttonEffect = (keys.Start.justPressed() || keys.A.justPressed()) ? ButtonEffect::ACCEPT :
									  (keys.B.justPressed() || keys.Back.justPressed()) ? ButtonEffect::BACK : ButtonEffect::NONE;

	switch (buttonEffect)
	{
		case ButtonEffect::ACCEPT:
		{
			// Accept current selection
			if (mMenuEntries.hasSelected())
			{
				switch ((Option)mMenuEntries.selected().mData)
				{
					case Option::OPEN_MODS_FOLDER:
						GameMenuBase::playMenuSound(0x63);
						PlatformFunctions::openDirectoryExternal(Configuration::instance().mAppDataPath + L"mods/");
						break;

					case Option::OPEN_MANUAL:
						GameMenuBase::playMenuSound(0x63);
						PlatformFunctions::openURLExternal("https://sonic3air.org/Manual.pdf");
						break;

					case Option::GO_BACK:
						return false;
				}
			}
			break;
		}

		case ButtonEffect::BACK:
			return false;

		case ButtonEffect::NONE:
			break;
	}

	const GameMenuEntries::UpdateResult result = mMenuEntries.update();
	if (result == GameMenuEntries::UpdateResult::ENTRY_CHANGED)
	{
		GameMenuBase::playMenuSound(0x5b);
	}
	return true;
}

void ModsStartPage::render(Drawer& drawer, float visibility)
{
	const int globalOffsetX = -roundToInt(saturate(1.0f - visibility) * 300.0f);

	Recti rect(globalOffsetX + 32, 16, 300, 18);
	drawer.printText(global::mSonicFontB, rect, "No hay mods instalados!", 1, Color(0.6f, 0.8f, 1.0f, visibility));
	rect.y += 22;

	Color color(0.8f, 0.9f, 1.0f, visibility);
	drawer.printText(global::mOxyfontSmall, rect, "Consulta el manual PDF del juego para ver las instrucciones", 1, color);
	rect.y += 14;
	drawer.printText(global::mOxyfontSmall, rect, "sobre instalar mods.", 1, color);
	rect.y += 18;
	drawer.printText(global::mOxyfontSmall, rect, "Ten en cuenta que deberas reiniciar Sonic 3 A.I.R.", 1, color);
	rect.y += 14;
	drawer.printText(global::mOxyfontSmall, rect, "para que busque nuevos mods.", 1, color);
	rect.y += 32;
	rect.x += 16;

	for (size_t index = 0; index < mMenuEntries.size(); ++index)
	{
		const auto& entry = mMenuEntries[index];
		const bool isSelected = ((int)index == mMenuEntries.mSelectedEntryIndex);
		color = isSelected ? Color::YELLOW : Color::WHITE;
		color.a *= visibility;

		drawer.printText(global::mOxyfontRegular, rect, entry.mText, 1, color);
		rect.y += 22;
	}
}
