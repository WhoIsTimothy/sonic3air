/*
*	Part of the Oxygen Engine / Sonic 3 A.I.R. software distribution.
*	Copyright (C) 2017-2026 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#include "sonic3air/pch.h"
#include "sonic3air/menu/options/OptionsConfig.h"
#include "oxygen/application/Application.h"


namespace
{
	struct ConfigBuilder
	{
		explicit ConfigBuilder(OptionsConfig::OptionsTab& optionsTab) :
			mOptionsTab(optionsTab)
		{
			mOptionsTab.mCategories.clear();
		}

		ConfigBuilder& addCategory(std::string_view name)
		{
			vectorAdd(mOptionsTab.mCategories).mName = name;
			return *this;
		}

		ConfigBuilder& addSetting(std::string_view name, option::Option key)
		{
			OptionsConfig::Setting& setting = vectorAdd(mOptionsTab.mCategories.back().mSettings);
			setting.mKey = key;
			setting.mName = name;
			return *this;
		}

		ConfigBuilder& addOption(std::string_view name, uint32 value)
		{
			OptionsConfig::Setting& setting = mOptionsTab.mCategories.back().mSettings.back();
			setting.mOptions.emplace_back(name, value);
			return *this;
		}

		ConfigBuilder& addNumberOptions(int minValue, int maxValue, int step, const char* postfix = "")
		{
			OptionsConfig::Setting& setting = mOptionsTab.mCategories.back().mSettings.back();
			for (int value = minValue; value <= maxValue; value += step)
				setting.mOptions.emplace_back(std::to_string(value) + postfix, value);
			return *this;
		}

		OptionsConfig::OptionsTab& mOptionsTab;
	};
}



void OptionsConfig::build()
{
	buildSystem();
	buildDisplay();
	buildAudio();
	buildVisuals();
	buildGameplay();
	buildControls();
	buildTweaks();
}

#define CATEGORY(_name_) configBuilder.addCategory(_name_);

void OptionsConfig::buildSystem()
{
	ConfigBuilder configBuilder(mSystemOptions);

#if !defined(PLATFORM_VITA)
	CATEGORY("Actualizaciones")
	{
		configBuilder.addSetting("Buscar actualizaciones", option::_CHECK_FOR_UPDATE)
			.addOption("Actualizaciones estables", 0)
			.addOption("Estable y preliminares", 1)
			.addOption("Todas, incluidas versiones de prueba", 2);
	}

	CATEGORY("Sincronizacion de Fantasmas")
	{
		configBuilder.addSetting("Activar sinc. de fantasmas", option::GHOST_SYNC)
			.addOption("Desactivado", 0)
			.addOption("Activado", 1);

		configBuilder.addSetting("Visualizacion de Fantasmas", option::GHOST_SYNC_RENDERING)
			.addOption("Opacidad total", 1)
			.addOption("Semi-transparente", 2)
			.addOption("Estilo fantasma", 3);
	}
#endif

#if defined(SUPPORT_IMGUI)
	CATEGORY("Gestion de Datos")
	{
		configBuilder.addSetting("Abrir en explorador", option::_OPEN_FILE_BROWSER);
	}
#endif

	CATEGORY("Mas Informacion")
	{
		configBuilder.addSetting("Abrir Pagina del Juego", option::_OPEN_HOMEPAGE);
		configBuilder.addSetting("Abrir Manual", option::_OPEN_MANUAL);
	}

	CATEGORY("Depuracion")
	{
		configBuilder.addSetting("Optimizacion de scripts", option::SCRIPT_OPTIMIZATION)
			.addOption("Automatico (Predeterminado)", -1)
			.addOption("Desactivado", 0)
			.addOption("Basico", 1)
			.addOption("Completo", 3);

		configBuilder.addSetting("Grabacion de Depuracion", option::GAME_RECORDING_MODE)
			.addOption("Automatico (Predeterminado)", -1)
			.addOption("Desactivado", 0)
			.addOption("Activado", 1);
	}

	CATEGORY("Modo Desarrollador")
	{
		configBuilder.addSetting("Modo Desarrollador", option::DEV_MODE)
			.addOption("Apagado", 0)
			.addOption("Encendido", 1);
	}
}

void OptionsConfig::buildDisplay()
{
	ConfigBuilder configBuilder(mDisplayOptions);

	CATEGORY("General")
	{
		configBuilder.addSetting("Renderizador:", option::RENDERER);
		const Configuration::RenderMethod highest = Configuration::getHighestSupportedRenderMethod();

	#if !defined(PLATFORM_VITA)
		configBuilder.addOption("Compatibilidad / Software", (uint32)Configuration::RenderMethod::SOFTWARE);
		if (highest >= Configuration::RenderMethod::OPENGL_SOFT)
			configBuilder.addOption("OpenGL por Software", (uint32)Configuration::RenderMethod::OPENGL_SOFT);
		if (highest >= Configuration::RenderMethod::OPENGL_FULL)
			configBuilder.addOption("OpenGL por Hardware", (uint32)Configuration::RenderMethod::OPENGL_FULL);
	#else
		// OpenGL Hardware does not work correctly on PSVita
		configBuilder.addOption("OpenGL por Software", (uint32)Configuration::RenderMethod::OPENGL_SOFT);
	#endif

		configBuilder.addSetting("Sincronizacion:", option::FRAME_SYNC)
			.addOption("V-Sync Apagado", 0)
			.addOption("V-Sync Encendido", 1)
			.addOption("V-Sync + Limite de FPS", 2);

		configBuilder.addSetting("Escalado:", option::UPSCALING)
			.addOption("Escalado Entero", 1)
			.addOption("Ajustar Proporcion", 0)
			.addOption("Estirar 50%", 2)
			.addOption("Estirar 100%", 3);
			//.addOption("Scale To Fill", 4);	// Works, but shouldn't be an option, as it looks a bit broken

		configBuilder.addSetting("Backdrop:", option::BACKDROP)
			.addOption("Fondo", 0)
			.addOption("Marco clasico 1", 1)
			.addOption("Marco clasico 2", 2)
			.addOption("Marco clasico 3", 3);

		configBuilder.addSetting("Desenfoque de Fondo:", option::BG_BLUR)
			.addOption("Apagado", 0)
			.addOption("25%", 1)
			.addOption("50%", 2)
			.addOption("75%", 3)
			.addOption("100%", 4);
	}

	CATEGORY("Filtro de Pantalla")
	{
	#if !defined(PLATFORM_VITA)
		configBuilder.addSetting("Filtro de pantalla:", option::SCREEN_FILTER_INDEX)
			.addOption("Pixel", 1)
			.addOption("xBRZ", 2)
			.addOption("HQx", 3);
	#else
		// High quality filters on the PSVITA is playing in slowmotion...
		configBuilder.addSetting("Filtro de pantalla:", option::SCREEN_FILTER_INDEX)
			.addOption("Pixel", 1);
	#endif

		configBuilder.addSetting("Variante:", option::SCREEN_FILTER_PIXEL_VARIANT)
			.addOption("Nitido", 0)
			.addOption("Suave 1", 1)
			.addOption("Suave 2", 2);

		configBuilder.addSetting("Variante:", option::SCREEN_FILTER_HQX_VARIANT)
			.addOption("HQ2x", 0)
			.addOption("HQ3x", 1)
			.addOption("HQ4x", 2);

		configBuilder.addSetting("Lineas de escaneo:", option::SCREEN_FILTER_SCANLINES)
			.addOption("Apagado", 0)
			.addOption("25%", 1)
			.addOption("50%", 2)
			.addOption("75%", 3)
			.addOption("100%", 4);
	}

	CATEGORY("Modo Ventana")
	{
	#if !defined(PLATFORM_VITA)
	#if defined(PLATFORM_LINUX)
		// Under Linux, the fullscreen with desktop resolution works better, so that's what we present as option
		const Configuration::WindowMode borderlessFullscreenMode = Configuration::WindowMode::FULLSCREEN_DESKTOP;
	#else
		const Configuration::WindowMode borderlessFullscreenMode = Configuration::WindowMode::FULLSCREEN_BORDERLESS;
	#endif

		configBuilder.addSetting("Pantalla Actual:", option::WINDOW_MODE)
			.addOption("En Ventana", (uint32)Configuration::WindowMode::WINDOWED)
			.addOption("Pantalla Completa", (uint32)borderlessFullscreenMode)
			.addOption("Pantalla Completa Exclusiva", (uint32)Configuration::WindowMode::FULLSCREEN_EXCLUSIVE);

		configBuilder.addSetting("Pantalla de Inicio:", option::WINDOW_MODE_STARTUP)
			.addOption("En Ventana", (uint32)Configuration::WindowMode::WINDOWED)
			.addOption("Pantalla Completa", (uint32)borderlessFullscreenMode)
			.addOption("Pantalla Completa Exclusiva", (uint32)Configuration::WindowMode::FULLSCREEN_EXCLUSIVE);
	#else
		// These aren't supposed to show up on the Vita
		configBuilder.addSetting("Pantalla Actual:", option::WINDOW_MODE)
			.addOption("Pantalla Completa Exclusiva", 0);

		configBuilder.addSetting("Pantalla de Inicio:", option::WINDOW_MODE_STARTUP)
			.addOption("Pantalla Completa Exclusiva", 0);
	#endif
	}

	CATEGORY("Informacion de Rendimiento")
	{
		configBuilder.addSetting("Mostrar Rendimiento:", option::PERFORMANCE_DISPLAY)
			.addOption("Apagado", 0)
			.addOption("Mostrar FPS", 1)
			.addOption("Analisis Completo", 2);
	}
}

void OptionsConfig::buildAudio()
{
	ConfigBuilder configBuilder(mAudioOptions);

	CATEGORY("Volumen")
	{
		const char* volumeName[] = { "Volumen General:", "Volumen de Musica:", "Volumen de Sonido:" };
		for (int k = 0; k < 3; ++k)
		{
			configBuilder.addSetting(volumeName[k], (option::Option)(option::MASTER_VOLUME + k))
				.addOption("Apagado", 0)
				.addNumberOptions(5, 100, 5, "%");
		}
	}

	CATEGORY("Banda Sonora")
	{
		configBuilder.addSetting("Tipo de Banda Sonora:", option::SOUNDTRACK)
			.addOption("Emulada", 0)
			.addOption("Remasterizada", 1);

		configBuilder.addSetting("Prueba de Sonido:", option::SOUND_TEST);		// Will be filled with content in "OptionsMenu::initialize()"
	}

	CATEGORY("Seleccion de Temas")
	{
		configBuilder.addSetting("Tema de Titulo:", option::TITLE_THEME)
			.addOption("Sonic 3", 0)
			.addOption("Sonic & Knuckles", 1);

		configBuilder.addSetting("Melodia de Vida Extra:", option::EXTRA_LIFE_JINGLE)
			.addOption("Sonic 3", 0)
			.addOption("Sonic & Knuckles", 1)
			.addOption("Elegir Por Zona", 0x10);

		configBuilder.addSetting("Tema de Invincibilidad:", option::INVINCIBILITY_THEME)
			.addOption("Sonic 3", 0)
			.addOption("Sonic & Knuckles", 1)
			.addOption("Elegir Por Zona", 0x10);

		configBuilder.addSetting("Tema Super/Hyper:", option::SUPER_THEME)
			.addOption("Musica normal de nivel", 0)
			.addOption("Musica rapida de nivel", 1)
			.addOption("Sonic 2", 2)
			.addOption("Sonic 3", 3)
			.addOption("Sonic & Knuckles", 4)
			.addOption("S3 Prototipo", 5);

		configBuilder.addSetting("Tema de Mini-Jefe:", option::MINIBOSS_THEME)
			.addOption("Sonic 3", 0)
			.addOption("Sonic & Knuckles", 1)
			.addOption("Elegir Por Zona", 0x10);

		configBuilder.addSetting("Tema de Knuckles:", option::KNUCKLES_THEME)
			.addOption("Sonic 3", 0)
			.addOption("Sonic & Knuckles", 1)
			.addOption("S3 Prototipo", 2)
			.addOption("Elegir Por Zona", 0x10);
	}

	CATEGORY("Musica De Nivel")
	{
		configBuilder.addSetting("Carnival Night Acto 1:", option::LEVELMUSIC_CNZ1)
			.addOption("Version Final", 0x00000001)
			.addOption("S3 Prototipo", 0x80000001);

		configBuilder.addSetting("Carnival Night Acto 2:", option::LEVELMUSIC_CNZ2)
			.addOption("Version Final", 0x00000002)
			.addOption("S3 Prototipo", 0x80000002);

		configBuilder.addSetting("IceCap Acto 1:", option::LEVELMUSIC_ICZ1)
			.addOption("Version Final", 0x00000001)
			.addOption("S3 Prototipo", 0x80000001);

		configBuilder.addSetting("IceCap Acto 2:", option::LEVELMUSIC_ICZ2)
			.addOption("Version Final", 0x00000002)
			.addOption("S3 Prototipo", 0x80000002);

		configBuilder.addSetting("Launch Base Acto 1:", option::LEVELMUSIC_LBZ1)
			.addOption("Version Final", 0x00000001)
			.addOption("S3 Prototipo", 0x80000001);

		configBuilder.addSetting("Launch Base Acto 2:", option::LEVELMUSIC_LBZ2)
			.addOption("Version Final", 0x00000002)
			.addOption("S3 Prototipo", 0x80000002);
	}

	CATEGORY("Seleccion de Musica")
	{
		configBuilder.addSetting("Jefe de Trampa-Laser de FBZ:", option::FBZ2_MIDBOSS_TRACK)
			.addOption("Musica de Minijefe", 1)
			.addOption("Musica de Jefe Principal", 0);

		configBuilder.addSetting("En Hidden Palace:", option::HPZ_MUSIC)
			.addOption("Sonic 3", 0)
			.addOption("Sonic & Knuckles", 1)
			.addOption("S3 + S&K Minijefe", 2)
			.addOption("S3 Prototipo", 3);

		configBuilder.addSetting("Jefes de Sky Sanctuary:", option::SSZ_BOSSTRACKS)
			.addOption("Musica Normal de Jefe", 0)
			.addOption("Temas de Sonic 1 & 2", 1);

		configBuilder.addSetting("Musica de Cierre:", option::OUTRO_MUSIC)
			.addOption("Sky Sanctuary", 0)
			.addOption("Creditos de Sonic 3", 1)
			.addOption("S3 Prototipl", 2);

		configBuilder.addSetting("Menu de Competicion:", option::COMPETITION_MENU_MUSIC)
			.addOption("Sonic 3", 0)
			.addOption("S3 Prototipo", 1);

		configBuilder.addSetting("Pantalla de Continuacion:", option::CONTINUE_SCREEN_MUSIC)
			.addOption("Sonic 3", 0)
			.addOption("Sonic & Knuckles", 1);
	}

	CATEGORY("Comportamiento de Musica")
	{
		configBuilder.addSetting("Al (Re)Iniciar Nivel:", option::CONTINUE_MUSIC)
			.addOption("Reiniciar Musica", 0)
			.addOption("Continuar Musica", 1);
	}

	CATEGORY("Efectos")
	{
		configBuilder.addSetting("Sonido Bajo El Agua:", option::UNDERWATER_AUDIO)
			.addOption("Normal", 0)
			.addOption("Amortiguado", 1);
	}
}

void OptionsConfig::buildVisuals()
{
	ConfigBuilder configBuilder(mVisualsOptions);

	CATEGORY("Mejoras Visuales")
	{
		configBuilder.addSetting("Rotacion de Personajes:", option::ROTATION)
			.addOption("Original", 0)
			.addOption("Suave", 1)
			.addOption("Fiel a Mania", 2);

		configBuilder.addSetting("Visualizacion de Tiempo:", option::TIME_DISPLAY)
			.addOption("Original", 0)
			.addOption("Extendido", 1);

		configBuilder.addSetting("Visualizacion de Vidas:", option::LIVES_DISPLAY)
			.addOption("Automatico", 0)
			.addOption("Clasico", 1)
			.addOption("Movil", 2);

		configBuilder.addSetting("Efecto Speed-Shoes", option::SPEEDUP_AFTER_IMAGES)
			.addOption("Ninguno (Original)", 0)
			.addOption("Estelas", 1);

		configBuilder.addSetting("Animacion de Fast Run:", option::FAST_RUN_ANIM)
			.addOption("Ninguno (Original)", 0)
			.addOption("Peel-Out", 1);

		configBuilder.addSetting("Efectos de Parpadeo:", option::ANTI_FLICKER)
			.addOption("Como El Original", 0)
			.addOption("Suavizado Leve", 1)
			.addOption("Suavizado Intenso", 2);
	}

	CATEGORY("Camara")
	{
		configBuilder.addSetting("Camara Dinamica:", option::CAMERA_OUTRUN)
			.addOption("Apagado", 0)
			.addOption("Encendido", 1);

		configBuilder.addSetting("Camara Extendida:", option::EXTENDED_CAMERA)
			.addOption("Apagado", 0)
			.addOption("Encendido", 1);
	}

	CATEGORY("Objetos")
	{
		configBuilder.addSetting("Estilo de Monitor:", option::MONITOR_STYLE)
			.addOption("Sonic 1 / 2", 1)
			.addOption("Sonic 3 & Knuckles", 0);
	}

	CATEGORY("Cambios de Color")
	{
		configBuilder.addSetting("Hora Inicial de IceCap:", option::ICZ_NIGHTTIME)
			.addOption("Dia", 0)
			.addOption("Amanecer", 1);
	}

	CATEGORY("Fases Especiales")
	{
		configBuilder.addSetting("Estilo de Blue Spheres:", option::SPECIAL_STAGE_VISUALS)
			.addOption("Clasico", 0)
			.addOption("Modernizado", 3);

		configBuilder.addSetting("Contador de Anillos:", option::SPECIAL_STAGE_RING_COUNT)
			.addOption("Ascendente", 0)
			.addOption("Descendente", 1);
	}
}

void OptionsConfig::buildGameplay()
{
	ConfigBuilder configBuilder(mGameplayOptions);

	CATEGORY("Niveles")
	{
		configBuilder.addSetting("Version de Niveles:", option::LEVEL_LAYOUTS)
			.addOption("Sonic 3", 0)
			.addOption("Sonic 3 & Knuckles", 1)
			.addOption("Sonic 3 A.I.R.", 2);
	}

	CATEGORY("Cambios de Dificultad")
	{
		configBuilder.addSetting("Bombardeo en Angel Island:", option::AIZ_BLIMPSEQUENCE)
			.addOption("Original", 0)
			.addOption("Alternativo", 1);

		configBuilder.addSetting("Combate contra Big Arms:", option::LBZ_BIGARMS)
			.addOption("Solo Knuckles", 0)
			.addOption("Todos Los Personajes", 1);

		configBuilder.addSetting("Fantasmas en Sandopolis:", option::SOZ_GHOSTSPAWN)
			.addOption("Desactivados", 1)
			.addOption("Activados", 0);

		configBuilder.addSetting("Jefe de Lava Reef Acto 2:", option::LRZ2_BOSS)
			.addOption("8 impactos", 1)
			.addOption("14 impactos (original)", 0);

		configBuilder.addSetting("Conservar Escudos Tras La Zona:", option::MAINTAIN_SHIELDS)
			.addOption("Desactivado", 0)
			.addOption("Activado", 1);
	}

	CATEGORY("Contrarreloj")
	{
		configBuilder.addSetting("Max. de Fantasmas Guardados:", option::TIMEATTACK_GHOSTS)
			.addOption("Apagado", 0)
			.addOption("1", 1)
			.addOption("3", 3)
			.addOption("5", 5);

		configBuilder.addSetting("Reinicio Rapido:", option::TIMEATTACK_INSTANTRESTART)
			.addOption("Mantener pulsado Y", 0)
			.addOption("Pulsar Y", 1);
	}
}

void OptionsConfig::buildControls()
{
	ConfigBuilder configBuilder(mControlsOptions);

	CATEGORY("Desbloqueado por Secretos")
	{
		configBuilder.addSetting("Drop Dash de Sonic:", option::DROP_DASH)
			.addOption("Apagado", 0)
			.addOption("Encendido", 1);

		configBuilder.addSetting("Super Peel-Out de Sonic:", option::SUPER_PEELOUT)
			.addOption("Apagado", 0)
			.addOption("Encendido", 1);
	}

	CATEGORY("Controles")
	{
		configBuilder.addSetting("Configurar Teclado Y Controles...", option::CONTROLLER_SETUP);		// This text here won't be used, see rendering

		for (int k = 0; k < InputManager::NUM_PLAYERS; ++k)
		{
			configBuilder.addSetting(*String(0, "Control del Jugador %d", k+1), (option::Option)(option::CONTROLLER_PLAYER_1 + k));
			if (Application::instance().hasVirtualGamepad())
				configBuilder.addOption("Ninguno (Solo tactil)", -1);
			else
				configBuilder.addOption("Ninguno (Solo teclado)", -1);
			// Actual options will get filled in inside "OptionsMenu::refreshGamepadLists"
		}

		configBuilder.addSetting("Otros controles", option::CONTROLLER_AUTOASSIGN)
			.addOption("Sin usar", -1)
			.addOption("Asignar al Jugador 1", 0)
			.addOption("Asignar al Jugador 2", 1)
			.addOption("Asignar al Jugador 3", 2)
			.addOption("Asignar al Jugador 4", 3);
	}

	if (Application::instance().hasVirtualGamepad())
	{
		CATEGORY("Control Virtual")
		{
			configBuilder.addSetting("Visibilidad:", option::VGAMEPAD_OPACITY).addNumberOptions(0, 100, 10, "%");
			configBuilder.addSetting("Tamaño de la Cruzeta:",	option::VGAMEPAD_DPAD_SIZE).addNumberOptions(50, 150, 10);
			configBuilder.addSetting("Tamaño de los Botones:", option::VGAMEPAD_BUTTONS_SIZE).addNumberOptions(50, 150, 10);
			configBuilder.addSetting("Distribucion del Control Tactil...", option::VGAMEPAD_SETUP);
		}
	}

	CATEGORY("Vibracion Del Controles")
	{
		for (int k = 0; k < InputManager::NUM_PLAYERS; ++k)
		{
			configBuilder.addSetting(*String(0, "Vibracion del Jugador %d", k+1), (option::Option)(option::CONTROLLER_RUMBLE_P1 + k));
			configBuilder.addOption("Apagado", 0);
			for (int i = 20; i <= 100; i += 20)
				configBuilder.addOption(*String(0, "%d %%", i), i);
		}
	}

	CATEGORY("Habilidades")
	{
		configBuilder.addSetting("Insta-Shield de Sonic:", option::INSTA_SHIELD)
			.addOption("Apagado", 0)
			.addOption("Encendido", 1);

		configBuilder.addSetting("Asistencia de Tails:", option::TAILS_ASSIST)
			.addOption("Apagado", 0)
			.addOption("Estilo Sonic 3 A.I.R.", 1)
			.addOption("Estilo Hibrido", 2)
			.addOption("Estilo de Sonic Mania", 3);

		configBuilder.addSetting("Cancelar Vuelo de Tails:", option::TAILS_FLIGHT_CANCEL)
			.addOption("Apagado", 0)
			.addOption("Abajo + Salto", 1);

		configBuilder.addSetting("Bloqueo al Saltar Rodando:", option::NO_CONTROL_LOCK)
			.addOption("Bloqueado (Clasico)", 0)
			.addOption("Movimiento Libre", 1);

		configBuilder.addSetting("Rebote del Escudo Burbuja:", option::BUBBLE_SHIELD_BOUNCE)
			.addOption("Estilo Sonic 3", 0)
			.addOption("Estilo de Sonic Mania", 1);
	}

	CATEGORY("Formas Super e Hyper")
	{
		configBuilder.addSetting("Formas Super de Tails:", option::HYPER_TAILS)
			.addOption("Solo Super Tails", 0)
			.addOption("Super e Hyper Tails", 1);

		configBuilder.addSetting("Cancelar Forma Super:", option::SUPER_CANCEL)
			.addOption("Apagado", 0)
			.addOption("Encendido", 1);

		configBuilder.addSetting("Habilidad de salto de Super Sonic:", option::SUPER_SONIC_ABILITY)
			.addOption("Ninguna (Original)", 0)
			.addOption("Escudo", 1)
			.addOption("Super Impulso", 2);

		configBuilder.addSetting("Hyper Dash de Sonic:", option::HYPER_DASH_CONTROLS)
			.addOption("Como El Original", 0)
			.addOption("Solo Al Mantener Cruzeta", 1);
	}
}

void OptionsConfig::buildTweaks()
{
	ConfigBuilder configBuilder(mTweaksOptions);

	CATEGORY("Desbloqueado Por Secretos")
	{
		configBuilder.addSetting("Modo Debug:", option::DEBUG_MODE)
			.addOption("Apagado", 0)
			.addOption("Encendido", 1);

		configBuilder.addSetting("Pantalla de Titulo:", option::TITLE_SCREEN)
			.addOption("Sonic 3", 0)
			.addOption("Sonic & Knuckles", 1);

		configBuilder.addSetting("Velocidad De Juego:", option::GAME_SPEED)
			.addOption("50 Hz (lento)", 50)
			.addOption("60 Hz (normal)", 60)
			.addOption("75 Hz (rapido)", 75)
			.addOption("90 Hz (muy rapido)", 90)
			.addOption("120 Hz (ya bajale we)", 120)
			.addOption("144 Hz (que le bajes alv-)", 144);
	}

	CATEGORY("Accesibilidad")
	{
		configBuilder.addSetting("Vidas Infinitas:", option::INFINITE_LIVES)
			.addOption("Desactivado", 0)
			.addOption("Activado", 1);

		configBuilder.addSetting("Tiempo Infinito:", option::INFINITE_TIME)
			.addOption("Desactivado", 0)
			.addOption("Activado", 1);
	}

	CATEGORY("Variedad de Juego")
	{
		configBuilder.addSetting("Escudos:", option::SHIELD_TYPES)
			.addOption("Escudo clasico", 0)
			.addOption("Escudos elementales", 1)
			.addOption("Clasico + Elementales", 2)
			.addOption("Escudos Mejorables", 3);

		configBuilder.addSetting("Monitores Aleatorios:", option::RANDOM_MONITORS)
			.addOption("Monitores Normales", 0)
			.addOption("Escudos Aleatorios", 1)
			.addOption("Monitores Aleatorios", 2);

		configBuilder.addSetting("Comportamiento Del Monitor:", option::MONITOR_BEHAVIOR)
			.addOption("Predeterminado", 0)
			.addOption("Caen al golpearlos", 1);

		configBuilder.addSetting("Monitores Ocultos:", option::HIDDEN_MONITOR_HINT)
			.addOption("Sin Pista", 0)
			.addOption("Brilla Cerca del Cartel", 1);
	}

	CATEGORY("Fases Especiales")
	{
		configBuilder.addSetting("Diseño de Fases Especiales:", option::RANDOM_SPECIALSTAGES)
			.addOption("Original", 0)
			.addOption("Generadas Aleatoriamente", 1);

		configBuilder.addSetting("Al Fallar:", option::SPECIAL_STAGE_REPEAT)
			.addOption("Avanzar a la siguiente", 0)
			.addOption("No avanzar", 1);
	}

	CATEGORY("Region")
	{
		configBuilder.addSetting("Codigo de Region:", option::REGION)
			.addOption("Occidental (\"Tails\")", 0x80)
			.addOption("Japon (\"Miles\")", 0x00);
	}

	CATEGORY("Carrera de velocidad")
	{
		configBuilder.addSetting("Correccion de errores:", option::FIX_GLITCHES)
			.addOption("Sin correcciones", 0)
			.addOption("Solo correcciones basicas", 1)
			.addOption("Todas (recomendado)", 2);
	}

	CATEGORY("Otras Mejoras")
	{
		configBuilder.addSetting("Velocidad Al Empujar Objetos:", option::FASTER_PUSH)
			.addOption("Original", 0)
			.addOption("Mas rapido", 1);

		configBuilder.addSetting("Acelerar el Recuento de Puntos:", option::LEVELRESULT_SCORE)
			.addOption("Apagado", 0)
			.addOption("Encendido", 1);

		configBuilder.addSetting("Transporte por tubos de LBZ:", option::LBZ_TUBETRANSPORT)
			.addOption("Velocidad Original", 0)
			.addOption("Mas rapido", 1);

		configBuilder.addSetting("Ascensor de MHZ:", option::MHZ_ELEVATOR)
			.addOption("Velocidad Original", 0)
			.addOption("Mas rapido", 1);

		configBuilder.addSetting("FBZ Door Opening:", option::FBZ_SCREWDOORS)
			.addOption("Velocidad Original", 0)
			.addOption("Mas rapido", 1);

		configBuilder.addSetting("Elevacion de Piramide de SOZ:", option::SOZ_PYRAMID)
			.addOption("Velocidad Original", 0)
			.addOption("Mas rapido", 1);

		configBuilder.addSetting("Intro de Knuckles en AIZ:", option::AIZ_INTRO_KNUCKLES)
			.addOption("Apagado", 0)
			.addOption("Encendido", 1);

		configBuilder.addSetting("Comportamiento del Cilindro en FBZ:", option::FBZ_ENTERCYLINDER)
			.addOption("Original", 0)
			.addOption("Se puede entrar desde arriba", 1);

		configBuilder.addSetting("Jugador 2 fuera de pantalla:", option::PLAYER2_OFFSCREEN)
			.addOption("No mostrar", 0)
			.addOption("Mostrar en el borde", 1);
	}
}

#undef CATEGORY
