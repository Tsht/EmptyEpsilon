--[[                  Escorteurs alias Destroyers
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

L’escorteur (destroyer) est un puissant vaisseau de défense contre les vaisseaux moyens et lourds.
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






--[[Destroyer Imperial Cobra]]
template = ShipTemplate():setName("Destroyer Imperial Cobra"):setClass("Destroyer", "Combat"):setModel("Navy_Destroyer")
template:setDescription([[Les destroyers de classe Cobra composent la majeure partie de la flotte d'escorteurs de la Flotte de Guerre Obscuras notamment en raison de sa grande vitesse lui permettant de rivaliser avec la plupart des autres vaisseaux, y compris Eldar.]])
template:setHull(650)
template:setShields(200, 200)
template:setSpeed(105, 20, 25)
--                  Arc, Dir, Range, CycleTime, Dmg
template:setBeam(0, 100, 90, 2000.0, 3, 9)
template:setBeam(1, 100,-90, 2000.0, 3, 9)
--                                Arc, Dir, Rotate speed

--       Tubes
template:setCustomWeapon("HVLI", "MCANMK2", 3, 1500.0, "Kinetic", 25)
template:setCustomWeaponMultiple("MCANMK2",1,3)
template:setCustomWeaponColor("MCANMK2", 255, 150, 103)
template:setCustomWeaponStorage("MCANMK2", 120)


template:setCustomWeapon("HVLI", "MCANMK2", 3, 1500.0, "Kinetic", 25)
template:setCustomWeaponMultiple("MCANMK2",1,3)
template:setCustomWeaponColor("MCANMK2", 255, 150, 103)
template:setCustomWeaponStorage("MCANMK2", 120)

template:setCustomWeapon("Nuke", "SEEKMK2", 2, 200.0, "Kinetic", 60)
template:setCustomWeaponColor("SEEKMK2", 255, 204, 0)
template:setCustomWeaponStorage("SEEKMK2", 12)
template:setCustomWeaponMultiple("SEEKMK2",1,2)


template:setTubes(4, 12.0)
template:setTubeDirection(0, 1):weaponTubeDisallowCustomMissile(0,"MCANMK2"):weaponTubeDisallowCustomMissile(0,"Mineswarm")
template:setTubeSize(1, "small")
template:setTubeDirection(1, -1):weaponTubeDisallowCustomMissile(1,"MCANMK2"):weaponTubeDisallowCustomMissile(1,"Mineswarm")
template:setTubeSize(2, "small")
template:setTubeDirection(2, 90)
template:setTubeDirection(3, -90)
--DockClasses
template:setDockClasses("Drone","Vaisseau leger")
template:setDefaultAI('default')

--Macro-canon : (rafale, non homing) 

-- disallow missile à rajouter ? )


template:setDockClasses("Drone","Vaisseau leger")
template:setDefaultAI('default')




--[[Destroyer Imperial Windowmaker]]
template = ShipTemplate():setName("Destroyer widowmaker"):setClass("Destroyer", "mineur"):setModel("Navy_Destroyer")
template:setDescription([[Le destroyer de classe Windowmaker est un escorteur léger poseur de mine et detecteur longue portée. Il est également capable deminer les mines ennemies. il dispose de torpilles à l'avant sur le modèle du Cobra et de lasers legers de défense]])
template:setLocaleName("Destroyer widowmaker")
--                  Arc, Dir, Range, CycleTime, Dmg
template:setBeam(0, 100, 90, 2000.0, 3, 9)
template:setBeam(1, 100,-90, 2000.0, 3, 9)


template:setHull(700)
template:setShields(250, 250)
template:setSpeed(105, 12, 15)

template:setTubes(4, 12.0)
template:setCustomWeapon("HVLI", "MCANMK2", 3, 1500.0, "Kinetic", 25)
template:setCustomWeaponMultiple("MCANMK2",1,3)
template:setCustomWeaponColor("MCANMK2", 255, 150, 103)
template:setCustomWeaponStorage("MCANMK2", 120)

--template:setCustomWeapon("Nuke", "SEEKMK2", 2, 200.0, "Kinetic", 60)
--template:setCustomWeaponColor("SEEKMK2", 255, 204, 0)
--template:setCustomWeaponStorage("SEEKMK2", 6)
--template:setCustomWeaponMultiple("SEEKMK2",1,2)
--template:setShortRangeRadarRange(7500)
template:setDefaultAI('default')



--mine test
template:setCustomWeapon("Mine", "Mineswarm", 1.5, 50, "Kinetic", -1)
template:setCustomWeaponColor("Mineswarm", 255, 204, 0)
template:setCustomWeaponStorage("Minewarm", 12)
template:setCustomWeaponMultiple("Mineswarm",1,2)
template:setWeaponStorage("Mine", 60)

template:setTubeDirection(0, 180):weaponTubeDisallowCustomMissile(0,"MCANMK2"):weaponTubeDisallowCustomMissile(0,"SEEKMK2")

template:setTubeDirection(1, 0):weaponTubeDisallowCustomMissile(1,"MCANMK2"):weaponTubeDisallowCustomMissile(1,"Mineswarm"):weaponTubeDisallowMissle(1,"mine")
template:setTubeSize(1, "small")
template:setTubeDirection(2, 90):weaponTubeDisallowCustomMissile(2,"SEEKMK2")
template:setTubeDirection(3, -90):weaponTubeDisallowCustomMissile(3,"SEEKMK2")




variation = template:copy("Destroyer widowmaker MK2"):setType("playership")
variation:setLocaleName("Destroyer widowmaker MK2")
variation:setDescription([[Le destroyer de classe Windowmaker est un escorteur léger multirole, poseur de mine et detecteur longue portée. Il est  également capable deminer les mines ennemies. il dispose de torpilles à l'avant sur le modèle du Cobra et de lasers legers de défense]])
variation:setHull(700)
variation:setShields(250, 250)
variation:setSpeed(105, 14, 12, 55, 10)

variation:setTubeDirection(0, 180):weaponTubeDisallowCustomMissile(0,"MCANMK2"):weaponTubeDisallowCustomMissile(0,"SEEKMK2")
variation:setTubeDirection(1, 0):weaponTubeDisallowCustomMissile(1,"MCANMK2"):weaponTubeDisallowCustomMissile(1,"Mineswarm")
variation:setTubeSize(1, "small")

-- PJ
variation:setCombatManeuver(400, 250)
variation:addRoomSystem(1, 0, 2, 1, "Maneuver");
variation:addRoomSystem(1, 1, 2, 1, "BeamWeapons");
variation:addRoom(2, 2, 2, 1);
variation:addRoomSystem(0, 3, 1, 2, "RearShield");
variation:addRoomSystem(1, 3, 2, 2, "Reactor");
variation:addRoomSystem(3, 3, 2, 2, "Warp");
variation:addRoomSystem(5, 3, 1, 2, "JumpDrive");
variation:addRoom(6, 3, 2, 1);
variation:addRoom(6, 4, 2, 1);
variation:addRoomSystem(8, 3, 1, 2, "FrontShield");
variation:addRoom(2, 5, 2, 1);
variation:addRoomSystem(1, 6, 2, 1, "MissileSystem");
variation:addRoomSystem(1, 7, 2, 1, "Impulse");
variation:addDoor(1, 1, true);
variation:addDoor(2, 2, true);
variation:addDoor(3, 3, true);
variation:addDoor(1, 3, false);
variation:addDoor(3, 4, false);
variation:addDoor(3, 5, true);
variation:addDoor(2, 6, true);
variation:addDoor(1, 7, true);
variation:addDoor(5, 3, false);
variation:addDoor(6, 3, false);
variation:addDoor(6, 4, false);
variation:addDoor(8, 3, false);
variation:addDoor(8, 4, false); 
variation:setShortRangeRadarRange(10000)
variation:setLongRangeRadarRange(45000)
variation:setJumpDrive(true)
--Airlock doors
--variation:addDoor(2, 2, false);
--variation:addDoor(2, 5, false);	

variation:setCustomWeapon("Mine", "Mineswarm", 1.5, 50, "Kinetic", -1)
variation:setCustomWeaponColor("Mineswarm", 255, 204, 0)
variation:setCustomWeaponStorage("Mineswarm", 12)
variation:setCustomWeaponMultiple("Mineswarm",1,2)


variation:setCustomWeapon("HVLI", "MCANMK2", 3, 1500.0, "Kinetic", 25)
variation:setCustomWeaponMultiple("MCANMK2",1,3)
variation:setCustomWeaponColor("MCANMK2", 255, 150, 103)
variation:setCustomWeaponStorage("MCANMK2", 120)

variation:setCustomWeapon("Nuke", "SEEKMK2", 2, 200.0, "Kinetic", 60)
variation:setCustomWeaponColor("SEEKMK2", 255, 204, 0)
variation:setCustomWeaponStorage("SEEKMK2", 12)
variation:setCustomWeaponMultiple("SEEKMK2",1,2)



--mine test



--[[Destroyer du chaos Iconoclast]] --a redeclarer 
template = ShipTemplate():setName("Destroyer du chaos Iconoclast"):setClass("Destroyer", "Combat"):setModel("Navy_Destroyer")
template:setLocaleName("Destroyer du chaos Iconoclast")
template:setModel("Chaos_Destroyer")
--template:setRadarTrace("adv_striker.png")
template:setRadarTrace("RadarArrow.png")
template:setDescription([[Le Destroyer Iconoclast est un vaisseau de conception simple reprenant les principales caracteristiques des differents escorteurs utilises par les Libre-Marchands ou la Flotte Marchande, ce qui fait qu'il peut etre produit dans pratiquement n'importe quel Dock Orbital de la Galaxie.
Le destroyer Iconoclast est redoute pour sa puissance de feu, important pour un vaisseau de cette taille, et constitue une menace permanente pour tous les vaisseaux navigants dans les immensite de l'espace, y compris pour les vaisseaux de ligne pouvant etre mis a mal par un escadron d'Iconoclast.]])

template:setHull(700)
template:setShields(300, 300)
template:setSpeed(110, 12, 15)


template:setBeam(0, 100, 90, 3000.0, 3, 12)
template:setBeam(1, 100,-90, 3000.0, 3, 12)

template:setCustomWeapon("homing", "chaosM", 1, 800.0, "Kinetic", 60)
template:setCustomWeaponColor("chaosM", 255, 204, 0)
template:setCustomWeaponStorage("chaosM", 300)
template:setCustomWeaponMultiple("chaosM",1,2)
template:setDefaultAI('default')

template:setTubes(6, 24.0)
template:setTubeDirection(0, 1):setWeaponTubeExclusiveForCustom(0,"chaosM")
template:setTubeDirection(1, 2):setWeaponTubeExclusiveForCustom(1,"chaosM")
template:setTubeDirection(2, -1):setWeaponTubeExclusiveForCustom(2,"chaosM")
template:setTubeDirection(3, -2):setWeaponTubeExclusiveForCustom(3,"chaosM")
template:setTubeDirection(4, -90):setWeaponTubeExclusiveForCustom(2,"chaosM")
template:setTubeDirection(5, 90):setWeaponTubeExclusiveForCustom(2,"chaosM")

