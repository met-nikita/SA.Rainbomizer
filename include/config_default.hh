const char config_toml[] = R"(
########################################################

# Rainbomizer - A (probably fun) Grand Theft Auto San Andreas Mod that
#				randomizes stuff
# Copyright (C) 2019 - Parik

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.	If not, see <https://www.gnu.org/licenses/>.
#
#######################################################
# General Configuration

[Randomizers]
# Set to false to disable all Rainbomizer features regardless of other settings
EnableRainbomizer = true

# Set to true or false to enable / disable specific randomizers. 
# Brief explanations of each can be found at the end of this file.

ColourRandomizer = true
TimeCycleRandomizer = true
TrafficRandomizer = true
ScriptVehicleRandomizer = true
ParkedCarRandomizer = true
MissionRandomizer = true
WeaponRandomizer = true
PickupsRandomizer = true
PlayerRandomizer = true
PedRandomizer = true
CutsceneRandomizer = true
SoundRandomizer = true
LanguageRandomizer = true
LicensePlateRandomizer = true
MapRandomizer = true
RiotRandomizer = true
PoliceHeliRandomizer = true
BlipRandomizer = true
WantedLevelRandomizer = true
DYOMRandomizer = true
CheatRandomizer = false

# EXPERIMENTAL RANDOMIZERS (high risk of crashes / softlocks)

HandlingRandomizer = false
WeaponStatsRandomizer = false
ParticleRandomizer = false
ObjectRandomizer = false
RespawnPointRandomizer = false
AnimationRandomizer = false

# General Settings
[EnableRainbomizer]

Seed	  = -1 # -1 for random

Unprotect = true
AutosaveSlot = 8 # Set it to a non-existent slot (eg. -1 or 9) to disable
ModifyCredits = true

# Additional settings for individual randomizers follow.

#######################################################
[ColourRandomizer]

RandomizeCarCols = true # (Car Colours)
ChangeCarColsOnFade = true # (Re-randomizes car colours on every fade)

RandomizeMarkers = true # (Mission Markers and Arrows)
RandomizeText = true # (HUD elements and menu text)
RandomizeWeaponSprites = false # (Weapon icons visible in the top right of the HUD)

# Set to true to use older style of text colour randomization from previous versions
# This means elements such as subtitles, the in-game time, etc, are not randomized.
# Also disables weapon sprite colour randomization if enabled.
UseMinimalistTextRandomization = false

RandomizeLights = true # (Light emitters e.g. light posts, headlights)

# Set to true to make random light colours consistent between every light of that type
# rather than a different light colour for every single light.
ConsistentLights = false

RandomizeClouds = true # (Clouds in the sky)
RandomizeStars = true # (Stars at night time)
RandomizeRainbows = false # (Natural rainbows that can occur after rainfall)
RandomizeFireLighting = false # (Light effects emitted by fire. WARNING: Hard on eyes)
ChangeOnFade = true # (Re-randomizes all randomized colours other than car colours every fade)

RainbowHueCycle = false # (All randomized elements will have transition between rainbow colours)

RandomizeFades = false # (In-game fade-in/outs. WARNING: Hard on eyes)
CrazyMode = false # Changes images and menu background (WARNING: VERY hard on eyes)

#######################################################
[TimeCycleRandomizer]

# Randomizes timecyc.dat values, which will change appearance and colours of sky and objects.
RandomizeTimeCycle = true
ChangeOnFade = true # (Generates a new random timecycle set every fade, otherwise is the same from game start)

RandomTimecycleOdds = 35

# Randomizes weather cycle at regular intervals.
RandomizeWeather = true

#######################################################
[TrafficRandomizer]

# Forces one specific vehicle to spawn in traffic. Set to -1 to not force a vehicle.
ForcedVehicleID = -1

# Change these options to disable certain types of vehicles from spawning in traffic
EnableTrains = true
EnableBoats = true
EnableAircrafts = true
EnableCars = true
EnableBikes = true
EnableTrailers = true

# Ped to spawn in odd Ambulances and Fire Truck
DefaultModel = 0

#######################################################
[ScriptVehicleRandomizer]

# Makes time requirements for schools more lenient so that they are completable with slower vehicles
EnableExtraTimeForSchools = true

# Randomizes the type of trains spawned in missions
RandomizeTrains = true

# Allow certain vehicle checks for sub-missions or markers to be skipped so they can be activated with any vehicle
LowriderMissions = true # (Lowrider Minigame, High Stakes Low-Rider)
WuZiMu = true # (Wu Zi Mu start mission marker)
SweetsGirl = true # (Picking up Sweet in Sweet's Girl)
CourierMissions = true # (Courier Missions in LS / SF / LV)
BMXChallenge = true # (LS BMX Challenge)
NRG500Challenge = true # (SF NRG-500 Challenge)
ChiliadChallenge = true # (All 3 Chiliad Challenge routes)

# When side missions in a vehicle are started, the mod will re-randomize it into a different one for you to use.
# Set to false to disable this behaviour for any of the following side missions.
TaxiMissions = true
Firefighting = true
Vigilante = true
Burglary = true
Pimping = true
Paramedic = true
Courier = true
BikeChallenges = true

# Set to true to restore behaviour from previous versions where planes in Air Raid 
# crash immediately before reaching the roof.
SlowPlanesInAirRaid = false

# Forces every script vehicle to be replaced by one of your choice.
# Set to -1 to not force a vehicle.
ForcedVehicleId = -1

# Uses a generic set of rules for randomizing vehicles that doesn't require patterns 
# for mod compatibility and/or simplicity.
UseGenericPatterns = false

# Gives you any vehicle for every mission and ignores all VehiclePatterns.txt patterns
# Please note that the pre-created patterns already allow all possible and stable vehicles for every mission
# So expect additional softlocks and crashes using this.
SkipChecks = false

#######################################################
[ParkedCarRandomizer]

RandomizeFixedSpawns = true # (Fixed spawns like Sweet's car)
RandomizeRandomSpawns = true # (For example: Car Parks)

# Randomizes fixed car spawns within their original group (e.g. boats randomize between only other boats)
RandomizeToSameType = false

#######################################################
[MissionRandomizer]

# Forces every mission to start a specific mission using its respective ID from the main.scm file. 
# Set to -1 to not force a mission and randomize normally.
ForcedMissionID = -1

# RandomizeOnce - means that each mission marker is always linked to the same random mission
RandomizeOnce = true
RandomizeOnceSeed = "" # https://en.wikipedia.org/wiki/Random_seed - useful for races
ForcedRandomizeOnceSeed = false # Force the seed on existing save files

# Set to true to keep your speed through end of mission teleports
# If false, you will always be forced to a dead stop regardless of speed after a teleport
ConserveMomentumThroughTeleports = true

DisableMainScmCheck = false # Allow custom main.scm's to run with mission randomizer.
		      	    # Note: Missions might not progress properly with a custom
			    #       main.scm file.

#######################################################
[WeaponRandomizer]

RandomizePlayerWeapons = true
SkipChecks = false # Checks related to weapons required for certain missions
		   # You might not be able to complete some missions
		   # if this is true.

#######################################################
[PickupsRandomizer]

# Randomizes the weapons dropped by dead peds.
RandomizePedWeaponDrops = true

ReplaceWithWeaponsOnly = false # Randomizes only weapon-type pickups with other weapons.

# Randomize money to be given by certain random pickups
MoneyFromRandomPickups = true

# Randomize horseshoes/snapshots/oysters
RandomizeCollectables = true

SkipChecks = false # Checks related to weapons required for certain missions
		   # You might not be able to complete some missions
		   # if this is true.

#######################################################
[PlayerRandomizer]

# If both activated, you will have a chance to randomize between both other ped models and 
# CJ wearing random clothing at any point.
RandomizePlayerModel = true
RandomizePlayerClothing = true

# If both of the config options above are activated, this number dictates the chances of
# the player becoming a new model vs the player becoming CJ with different clothes.
# Odds are out of 100, so default odds are 80 in 100 chance of a new non-CJ model.
OddsOfNewModel = 80

# Set to true to allow models featuring nudity
IncludeNSFWModels = false

# Chooses a single random model or outfit for CJ that is maintained for the entire session.
RandomizePlayerOnce = false

# Forces specific model for player when using random player models using their model ID.
# Set to -1 for normal randomization.
# Set to 298 to always use a special mission model.
# To use a specific special mission model, set to 298 and set ForcedSpecialModel to the model name.
ForcedModel = -1
ForcedSpecialModel = ""

#######################################################
[PedRandomizer]

RandomizeGenericModels = true # (Peds on the street or in interiors, non-named peds in missions)
RandomizeCops = true # (Cops can look like any ped, but will behave the same)
RandomizeGangMembers = true # (Members of all gangs can look like anyone, but will behave the same)
RandomizeSpecialModels = true # (Mission characters e.g. Sweet, Big Smoke, Cesar)

# Set to true to allow models featuring nudity
IncludeNSFWModels = false

# Force specific ped model for generic street peds using their model ID. 
# Set to -1 for normal randomization
ForcedPedModel = -1

# Force specific special model for mission characters using their model name.
# Set to -1 for normal randomization
ForcedSpecialModel = ""

#######################################################
[CutsceneRandomizer]

RandomizeModels = true

# Set to true to only randomize cutscene peds between other peds with proper facial animations (a.k.a. no missing jaws).
UseOnlyNormalCutsceneModels = false

RandomizeLocations = true
RandomizeCutsceneToPlay = true

#######################################################
[SoundRandomizer]

# Main part of this randomizer which changes spoken dialogue in missions.
RandomizeScriptVoiceLines = true
MatchSubtitles = true

# Randomizes the type of speech event a ped uses when they're about to speak
# For example, a line about bumping into someone could become a line about falling to your death.
RandomizeGenericPedSpeech = true

# Randomizes generic frontend audio sound effects (mostly bleep and bloop sounds).
RandomizeGenericSfx = true

# Force a single voice line. Set to -1 for normal randomization.
ForcedAudioLine = -1

#######################################################
[LanguageRandomizer]

MinTimeBeforeTextChange = 1 # seconds
MaxTimeBeforeTextChange = 5 # seconds

#######################################################
[RiotRandomizer]

# Adds a small chance of riot mode briefly activating as you enter new areas.
RandomizeRiots = true

# Randomizes which light is activated next (green, yellow, red) as well as
# the time between light changes.
RandomizeTrafficLights = true

#######################################################
[WantedLevelRandomizer]

# Randomizes number of wanted stars you get in missions like 2 stars in Drive-by
RandomizeMissionWantedLevels = true

# Randomizes how many "chaos points" you get for each crime.
# You could, for example, get 3 stars for stealing a cop bike, but lose 1 star for killing
# a cop.
RandomizeChaosPoints = false

#######################################################
[DYOMRandomizer]

# Picks missions only from those designated as English by the DYOM website.
UseEnglishOnlyFilter = false

AutoTranslateToEnglish = true
RandomSpawn = true

# Languages in translation chain separated by a semicolon, e.g. "pl;el;zh-CN;en" means Polish->Greek->Simplified Chinese->English, see https://cloud.google.com/translate/docs/languages for supported languages and their codes
TranslationChain = "en"

EnableTextToSpeech = false
OverrideTTSVolume = -1.0

#######################################################
[CheatRandomizer]

EnableEasterEgg = true

#######################################################
# EXPLANATIONS OF RANDOMIZERS

# Colour Randomizer
# Randomizes the colours of various in-game elements, including cars, the HUD, and fades. 

# Timecycle Randomizer
# Randomizes the appearance and colours of the sky, water, lighting, and other time-based elements.
# Can also randomize the occurrence of weather.

# Traffic Randomizer
# Randomizes cars that spawn in traffic including law enforcement vehicles.

# Script Vehicle Randomizer 
# Randomizes the vehicles you are given in missions.

# Parked Car Randomizer 
# Randomizes cars which are found parked around the map including airplanes at airports with entirely random spawns.

# Mission Randomizer
# Randomizes which mission is started by each mission marker.

# Weapon Randomizer
# Randomizes the weapons that are wielded by the player and other NPCs.

# Pickups Randomizer
# Randomizes pickups around the map upon starting a new game, as well as pickups during missions.

# Player Randomizer
# Randomizes the player's appearance every fade between other NPC models and random clothing options for CJ.

# Ped Randomizer
# Randomizes the appearance of peds on the street, cops, and mission peds.

# Cutscene Randomizer
# Randomizes the models used in motion-captured cutscenes, as well as the location in which they take place.

# Voice Line Randomizer	
# Randomizes dialogue spoken by characters in missions. (Requires original AudioEvents.txt)
# Can also randomize generic ped lines and some other types of sound effects.

# Language Randomizer
# Randomizes the language of text (except for subtitles when using Voice Line Randomizer). 
# Some types of text will change language at a regular interval which can be adjusted above.
# WARNING: This randomizer may be incompatible with other mods that add text and should be disabled in such cases.

# License Plate Randomizer
# Randomizes the license plates of vehicles to random words from the game script.

# Map Randomizer
# Randomizes map objects existing within the base game world. 
# Currently, this only randomizes tags between other tags.

# Riot Randomizer
# Activates a chance for the game's riot mode to activate briefly as you play the game.
# Traffic Lights can also be randomized as part of this so that the 
# time and order of lights is random rather than on a cycle.

# Police Heli Randomizer
# Randomizes the helicopters that the police spawn in with working spotlight and gun.

# Blip Randomizer
# Randomizes every radar blip on the map with a different icon. WARNING: Blips are permanently saved when you save the game.

# Wanted Level Randomizer
# Randomizes how you get wanted levels during and outside of missions.

# DYOM Randomizer
# When using the Design Your Own Mission (DYOM) mod, an option is added to the menu to download and play a 
# random mission from the DYOM website.

# Cheat Randomizer
# Randomizes which cheat activates when you enter a cheat code.

# EXPERIMENTAL RANDOMIZERS (high risk of crashes / softlocks)

# Handling Randomizer
# Shuffles the handlings of vehicles.

# Weapon Stats Randomizer
# Randomizes properties of all weapons, including their damage, fire rate, and clip size. 
# Thanks to SRewo for creating this randomizer.

# Particle Randomizer
# Randomizes all of the game's particle effects.

# Object Randomizer
# Randomizes objects spawned by mission scripts into other objects.

# Respawn Point Randomizer
# Randomizes where you respawn after death or arrest.

# Animation Randomizer
# Randomizes every animation in the game.
)";
