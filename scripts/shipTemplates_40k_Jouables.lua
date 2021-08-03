--[[                  Vaisseaux Uniquement Joueurs
Rappel des catégories : 
Vaisseau léger : 1 à 3 places ; aucun moteur Warp ou Jump possible ; shipTemplates_VaisseauxLegers.lua ;
Frégate : 4 à 9 places ; shipTemplates_Fregates.lua ;
Destroyer : 10 à 50 places ; shipTemplates_Escorteurs.lua ;
Croiseur Léger : 50 à 100 places ; shipTemplates_CroiseursLegers.lua ;
Croiseur : 100 à 250 places ; shipTemplates_Croiseurs.lua ;
Cuirassé : 250 à 500 places ; shipTemplates_Cuirasse.lua ;
Porte-Vaisseaux : plus de 500 places; shipTemplates_Capitaux.lua ;
Drones et Créatures : N.C ; shipTemplates_Autres.lua ;
Stations;


----------------------------------------------------------]]

function couleur(template,name,model)
	for _, color in ipairs({"Blue", "Green", "Grey", "Red", "White", "Yellow"}) do
		variation = template:copy(name .. " " .. color):setModel(model .. color):setSecret(true)
	end
end

function PZ(template,name,model)
	for _, color in ipairs({"-IMP-","-CHAOS-","-ELDAR-","-TAU-","-TRADE-","-MECH-"}) do
		variation = template:copy(name .. " " .. color):setModel(model .. color):setSecret(true)
	end
end

--[[Hathcock]]
template = ShipTemplate():setName("Hathcock"):setClass("Croiseurs", "Sniper"):setModel("Navy_Cruiser"):setType("playership")
template:setDescription([[Les croiseurs de classe Hathcock...]])
template:setHull(1200)
template:setShields(200, 200)
template:setSpeed(60, 6, 8)
--                  Arc, Dir, Range, CycleTime, Dmg
template:setBeam(0, 10, 0, 100.0, 30.0, 200)
template:setBeam(1, 10, 90, 2000.0, 7.0, 25)
template:setBeam(2, 10, -90, 2000.0, 7.0, 25)
template:setBeam(3, 15, 90, 2000.0, 1.5, 6)
template:setBeam(4, 15,-90, 2000.0, 1.5, 6)
template:setBeam(5, 15, 90, 2000.0, 1.5, 6)
template:setBeam(6, 15,-90, 2000.0, 1.5, 6)
--                              Arc, Dir, Rotate speed
template:setBeamWeaponTurret(1, 100,  90, 5)
template:setBeamWeaponTurret(2, 100, -90, 5)
template:setBeamWeaponTurret(3, 150,  90, 5)
template:setBeamWeaponTurret(4, 150, -90, 5)
template:setBeamWeaponTurret(5, 150,  90, 5)
template:setBeamWeaponTurret(6, 150, -90, 5)
--       Tubes
template:setTubes(8, 12.0)
template:setTubeDirection(0, 1)
template:setTubeDirection(1, -1)
template:setTubeDirection(2, 90)
template:setTubeDirection(3, -90)
template:setTubeDirection(4, 90)
template:setTubeDirection(5, -90)
template:setTubeDirection(6, 90)
template:setTubeDirection(7, -90)
--MGAA Skyreaper (homing trés rapide) fregrate et plus
template:setCustomWeapon("Homing", "SKYR", 0.25, 1000.0, "Kinetic", -1)
template:setCustomWeaponMultiple("SKYR",3,1)
template:setCustomWeaponColor("SKYR", 255, 204, 0)
template:setCustomWeaponStorage("SKYR", 12)
--Macro-canon : (rafale, non homing) 
template:setCustomWeapon("HVLI", "MCAN", 2, 1000.0, "Kinetic", -1)
template:setCustomWeaponMultiple("MCAN",4,1)
template:setCustomWeaponColor("MCAN", 255, 150, 103)
template:setCustomWeaponStorage("MCAN", 32)
--TG Seeker (homing) 
template:setCustomWeapon("Homing", "SEEK", 1.5, 600.0, "Kinetic", -1)
template:setCustomWeaponColor("SEEK", 255, 204, 0)
template:setCustomWeaponStorage("SEEK", 14)
--MGAA Flakburst (nuke faible dommage) 
template:setCustomWeapon("Nuke", "FLAK", 0.5, 500.0, "Kinetic", -1)
template:setCustomWeaponColor("FLAK", 220, 87, 20)
template:setCustomWeaponStorage("FLAK", 3)
--TG fusionmelta (Nuke homing) 
template:setCustomWeapon("Nuke", "MELT", 2, 300.0, "Kinetic", -1)
template:setCustomWeaponColor("MELT", 204, 0, 0)
template:setCustomWeaponStorage("MELT", 1)
--DockClasses
template:setDockClasses("Drone","Vaisseau leger","Fregate")
-- PJ
template:setCombatManeuver(200, 150)
template:setJumpDrive(true)
template:setRepairCrewCount(8)
--	(H)oriz, (V)ert	   HC,VC,HS,VS, system    (C)oordinate (S)ize
template:addRoomSystem( 0, 0, 1, 4, "Reactor")
template:addRoomSystem( 1, 0, 1, 1, "JumpDrive")
template:addRoomSystem( 1, 3, 1, 1, "Warp")
template:addRoomSystem( 2, 0, 1, 1, "FrontShield")
template:addRoomSystem( 2, 3, 1, 1, "RearShield")
template:addRoomSystem( 3, 0, 1, 1, "MissileSystem")
template:addRoomSystem( 3, 3, 1, 1, "Impulse")
template:addRoomSystem( 3, 1, 2, 1, "Maneuver")
template:addRoom( 3, 2, 2, 1)
template:addRoomSystem( 5, 1, 2, 2, "Beamweapons")

--(H)oriz, (V)ert H, V, true = horizontal
template:addDoor( 1, 0, false)
template:addDoor( 1, 3, false)
template:addDoor( 2, 0, false)
template:addDoor( 2, 3, false)
template:addDoor( 3, 0, false)
template:addDoor( 3, 3, false)
template:addDoor( 3, 3, true)
template:addDoor( 3, 2, true)
template:addDoor( 5, 1, false)
