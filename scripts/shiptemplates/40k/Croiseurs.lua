--[[                  Croiseurs
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

Le croiseur est un puissant vaisseau d'attaque lourdement blindé et armé.
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


--[[Croiseur Impérial Gothic]]
template = ShipTemplate():setName("Croiseur Imperial Gothic"):setClass("Croiseur", "Combat"):setModel("Navy_Cruiser")
template:setRadarTrace("battleship.png")
template:setDescription([[Les croiseurs de classe Gothic furent un type de vaisseaux tres repandus a travers l'ensemble de l'Imperium.
La puissance de feu des vaisseaux de ce type permet d'engager et de detruire des vaisseaux ennemis de toutes taille, faisant du croiseur Gothic un adversaire dangereux.
Toutefois, les premiers engagements de ce type de vaisseaux lors de la Guerre Gothique revelerent les lacunes des croiseurs Gothic, leur puissance de feu ne pouvant que difficilement rivaliser avec la mobilite de leurs adversaires.]])

template:setHull(1600)
template:setEnergyStorage(1500)
template:setShields(750, 750)
template:setSpeed(85, 6, 8, 45, 8)
template:setCombatManeuver(300, 250)
template:setShortRangeRadarRange(7500)
template:setEnergyConsumptionRatio(0.8)
--                  Arc, Dir, Range, CycleTime, Dmg
template:setBeam(0, 45, 0, 100.0, 30.0, 10)
template:setBeam(1, 90, 90, 3000.0, 14.0, 35)
template:setBeam(2, 90, -90, 3000.0, 14.0, 35)
template:setBeam(3, 90, 90, 3000, 14, 35)
template:setBeam(4, 90,-90, 3000, 14, 35)
template:setBeam(5, 90, 90, 2000, 3, 12)
template:setBeam(6, 90,-90, 2000, 3, 12)
--                              Arc, Dir, Rotate speed
--template:setBeamWeaponTurret(1, 100,  90, 5)
--template:setBeamWeaponTurret(2, 100, -90, 5)
--template:setBeamWeaponTurret(3, 150,  90, 5)
--template:setBeamWeaponTurret(4, 150, -90, 5)
--template:setBeamWeaponTurret(5, 150,  90, 5)
--template:setBeamWeaponTurret(6, 150, -90, 5)
--       Tubes
template:setTubes(3, 12.0)
template:setTubeDirection(0, 1)
template:setTubeSize(0, "small"):weaponTubeDisallowCustomMissile(0,"MCANMK3"):weaponTubeDisallowCustomMissile(0,"MAGMCAN")
template:setTubeDirection(1, -1)
template:setTubeSize(1, "small"):weaponTubeDisallowCustomMissile(1,"MCANMK3"):weaponTubeDisallowCustomMissile(1,"MAGMCAN")
template:setTubeDirection(2, 0)
template:setTubeSize(2, "small")
template:setTubeLoadTime(0, 45)
template:setTubeLoadTime(1, 45)
template:setTubeLoadTime(2, 45)


--Torpille
template:setCustomWeapon("Nuke", "SEEKMK2", 2, 400.0, "Kinetic", 60)
template:setCustomWeaponColor("SEEKMK2", 255, 204, 0)
template:setCustomWeaponStorage("SEEKMK2", 12)
template:setCustomWeaponMultiple("SEEKMK2",1,2)
-- Torpille EMP 
template:setCustomWeapon("EMP", "MAGSEEK", 0.5, 400.0, "EMP", 30)
template:setCustomWeaponColor("MAGSEEK", 0, 204, 255)
template:setCustomWeaponStorage("MAGSEEK", 1)
template:setCustomWeaponMultiple("MAGSEEK",1,2)
template:setDefaultAI('default')
template:setDockClasses("Drone","Vaisseau leger","Fregate","destroyer")
template:setCanHack(false)


--DockClasses
template:setDockClasses("Drone","Vaisseau leger","Fregate","destroyer")
template:setDefaultAI('default')

--[[Croiseur Gothic]]
variation = template:copy("Croiseur Gothic"):setType("playership")
variation:setLocaleName("Croiseur Gothic")
variation:setSpeed(85, 10, 12, 45, 10)
variation:setSystemDamageRatio(0.9)
variation:setSystemDamageHullThreshold(0.85)
-- PJ
variation:setCombatManeuver(300, 250)
variation:setJumpDrive(true)
--variation:setRepairCrewCount(2)
variation:addRoomSystem(0, 0, 1, 4, "RearShield")
variation:addRoom(1, 0, 1, 1)
variation:addRoomSystem(1, 1, 3, 2, "MissileSystem")
variation:addRoom(1, 3, 1, 1)
variation:addRoomSystem(2, 0, 2, 1, "Beamweapons")
variation:addRoomSystem(2, 3, 2, 1, "Maneuver")
variation:addRoomSystem(4, 0, 2, 1, "Warp")
variation:addRoomSystem(4, 3, 2, 1, "JumpDrive")
variation:addRoomSystem(5, 1, 1, 2, "Reactor")
variation:addRoom(6, 0, 1, 1)
variation:addRoomSystem(6, 1, 1, 2, "Impulse")
variation:addRoom(6, 3, 1, 1)
variation:addRoomSystem(7, 0, 1, 4, "FrontShield")
variation:addDoor(1, 0, false)
variation:addDoor(2, 0, false)
variation:addDoor(4, 0, false)
variation:addDoor(6, 0, false)
variation:addDoor(7, 0, false)
variation:addDoor(1, 1, true)
variation:addDoor(1, 3, true)
variation:addDoor(6, 1, true)
variation:addDoor(6, 2, false)
variation:addDoor(6, 3, true)
variation:addDoor(1, 3, false)
variation:addDoor(2, 3, false)
variation:addDoor(4, 3, false)
variation:addDoor(6, 3, false)
variation:addDoor(7, 3, false)


variation:setTubes(3, 12.0)
variation:setTubeDirection(0, 1)
variation:setTubeSize(0, "small")
variation:setTubeDirection(1, -1)
variation:setTubeSize(1, "small")
variation:setTubeDirection(2, 0)
variation:setTubeSize(2, "small")
variation:setShortRangeRadarRange(7500)


--Torpille
variation:setCustomWeapon("Nuke", "SEEKMK2", 2, 400.0, "Kinetic", 60)
variation:setCustomWeaponColor("SEEKMK2", 255, 204, 0)
variation:setCustomWeaponStorage("SEEKMK2", 16)
variation:setCustomWeaponMultiple("SEEKMK2",1,2)
-- Torpille EMP 
variation:setCustomWeapon("EMP", "MAGSEEK", 0.5, 400.0, "EMP", 30)
variation:setCustomWeaponColor("MAGSEEK", 0, 204, 255)
variation:setCustomWeaponStorage("MAGSEEK", 4)
variation:setCustomWeaponMultiple("MAGSEEK",1,2)


variation:setCustomWeapon("Homing", "TABORD", 0.5, 400.0, "EMP", 60)
variation:setCustomWeaponColor("TABORD", 57, 142, 55)
variation:setCustomWeaponStorage("TABORD", 30)
variation:setCustomWeaponMultiple("TABORD",1,2)


--#region


--[[Croiseur Impérial Lunar IA  ]]
template = ShipTemplate():setName("Croiseur Lunar"):setClass("Croiseur", "Artillerie"):setModel("Navy_Cruiser")
template:setRadarTrace("battleship.png")
template:setDescription([[ Concu pour remplacer l'ancien Croiseur Murder, le croiseur de classe Lunar est un vaisseau de conception simple et robuste pouvant etre produit par pratiquement tous les mondes imperiaux, y compris ceux incapables de produire des cuirasses ou des croiseurs.
Cette facilite de production a fait du croiseur Lunar l'epine dorsale de l'ensemble de la Flotte Imperiale. La Flotte Obscuras comprend, par exemple, près de 600 vaisseaux de cette classe repartis dans les secteurs du Segmentum Obscurus, dont une vingtaine rien que dans le Secteur Gothique. 1
De même, près d'un tiers des vaisseaux ayant participe à la Troisième Guerre d'Armageddon sous les ordres de l'Amiral Parol furent des croiseurs Lunar. 3

Alliant la portee de ses Lance-Torpilles avec la puissance de ses Batteries d'armes et Pièces d'artillerie navale, le croiseur Lunar est arme pour remplir aussi bien des rôles d'escorte, de patrouille ou d'assaut. 
La variante MK2 remplace son lance torpille par un canon nova ]])

template:setHull(1600)
template:setShields(700, 700)
template:setSpeed(85, 6, 8)

----template:setReverseSpeed(40,10)
template:setSystemDamageRatio(0.9)
template:setSystemDamageHullThreshold(0.85)
template:setCombatManeuver(300, 250)
--                  Arc, Dir, Range, CycleTime, Dmg
template:setBeam(0, 45, 0, 100.0, 30.0, 10)
template:setBeam(1, 90, 90, 3000.0, 14.0, 35)
template:setBeam(2, 90, -90, 3000.0, 14.0, 35)
template:setBeam(3, 90, 90, 2000, 3, 12)
template:setBeam(4, 90,-90, 2000, 3, 12)
template:setBeam(5, 90, 90, 2000, 3, 12)
template:setBeam(6, 90,-90, 2000, 3, 12)

template:setSystemDamageRatio(0.9)
template:setSystemDamageHullThreshold(0.85)
template:setCanHack(false)
template:setJumpDrive(false)
template:setShortRangeRadarRange(7500)


--MCAN et Torpille
template:setCustomWeapon("HVLI", "MCANMK3", 4, 1500.0, "Kinetic", 25)
template:setCustomWeaponMultiple("MCANMK3",1,4)
template:setCustomWeaponColor("MCANMK3", 255, 150, 103)
template:setCustomWeaponStorage("MCANMK3", 120)

template:setCustomWeapon("Nuke", "SEEKMK2", 2, 400.0, "Kinetic", 60)
template:setCustomWeaponColor("SEEKMK2", 255, 204, 0)
template:setCustomWeaponStorage("SEEKMK2", 12)
template:setCustomWeaponMultiple("SEEKMK2",1,2)


template:setTubes(7, 12.0)

template:setTubeDirection(0, 1)
template:setTubeSize(0, "small"):weaponTubeDisallowCustomMissile(0,"MCANMK3"):weaponTubeDisallowCustomMissile(0,"MAGMCAN")
template:setTubeDirection(1, -1)
template:setTubeSize(1, "small"):weaponTubeDisallowCustomMissile(1,"MCANMK3"):weaponTubeDisallowCustomMissile(1,"MAGMCAN")
template:setTubeDirection(2, 0)
template:setTubeSize(2, "small")




template:setTubeDirection(3, 90):weaponTubeDisallowCustomMissile(1,'SEEKMK2')
template:setTubeDirection(4, -90):weaponTubeDisallowCustomMissile(2,'SEEKMK2')
template:setTubeDirection(5, 90):weaponTubeDisallowCustomMissile(3,'SEEKMK2')
template:setTubeDirection(6, -90):weaponTubeDisallowCustomMissile(4,'SEEKMK2')






--[[Croiseur Impérial Lunar + nova / jouable  ]]
template = ShipTemplate():setName("Croiseur Lunar MK2"):setClass("Croiseur", "Artillerie"):setModel("Navy_Cruiser"):setType("playership")
template:setRadarTrace("battleship.png")
template:setDescription([[ Concu pour remplacer l'ancien Croiseur Murder, le croiseur de classe Lunar est un vaisseau de conception simple et robuste pouvant etre produit par pratiquement tous les mondes imperiaux, y compris ceux incapables de produire des cuirasses ou des croiseurs.
Cette facilite de production a fait du croiseur Lunar l'epine dorsale de l'ensemble de la Flotte Imperiale. La Flotte Obscuras comprend, par exemple, près de 600 vaisseaux de cette classe repartis dans les secteurs du Segmentum Obscurus, dont une vingtaine rien que dans le Secteur Gothique. 1
De même, près d'un tiers des vaisseaux ayant participe à la Troisième Guerre d'Armageddon sous les ordres de l'Amiral Parol furent des croiseurs Lunar. 3

Alliant la portee de ses Lance-Torpilles avec la puissance de ses Batteries d'armes et Pièces d'artillerie navale, le croiseur Lunar est arme pour remplir aussi bien des rôles d'escorte, de patrouille ou d'assaut. 
La variante MK2 remplace son lance torpille par un canon nova ]])

template:setHull(1600)
template:setShields(700, 700)
template:setSpeed(85, 6, 8, 40, 8)
----template:setReverseSpeed(40,10)
template:setSystemDamageRatio(0.9)
template:setSystemDamageHullThreshold(0.85)
template:setCombatManeuver(300, 250)
--                  Arc, Dir, Range, CycleTime, Dmg
template:setBeam(0, 45, 0, 100.0, 30.0, 10)
template:setBeam(1, 90, 90, 3000.0, 14.0, 35)
template:setBeam(2, 90, -90, 3000.0, 14.0, 35)
template:setBeam(3, 90, 90, 2000, 3, 12)
template:setBeam(4, 90,-90, 2000, 3, 12)
template:setBeam(5, 90, 90, 2000, 3, 12)
template:setBeam(6, 90,-90, 2000, 3, 12)

template:setSystemDamageRatio(0.9)
template:setSystemDamageHullThreshold(0.85)
template:setCanHack(false)
template:setJumpDrive(true)
--template:setRepairCrewCount(2)
template:addRoomSystem(0, 0, 1, 4, "RearShield")
template:addRoom(1, 0, 1, 1)
template:addRoomSystem(1, 1, 3, 2, "MissileSystem")
template:addRoom(1, 3, 1, 1)
template:addRoomSystem(2, 0, 2, 1, "Beamweapons")
template:addRoomSystem(2, 3, 2, 1, "Maneuver")
template:addRoomSystem(4, 0, 2, 1, "Warp")
template:addRoomSystem(4, 3, 2, 1, "JumpDrive")
template:addRoomSystem(5, 1, 1, 2, "Reactor")
template:addRoom(6, 0, 1, 1)
template:addRoomSystem(6, 1, 1, 2, "Impulse")
template:addRoom(6, 3, 1, 1)
template:addRoomSystem(7, 0, 1, 4, "FrontShield")
template:addDoor(1, 0, false)
template:addDoor(2, 0, false)
template:addDoor(4, 0, false)
template:addDoor(6, 0, false)
template:addDoor(7, 0, false)
template:addDoor(1, 1, true)
template:addDoor(1, 3, true)
template:addDoor(6, 1, true)
template:addDoor(6, 2, false)
template:addDoor(6, 3, true)
template:addDoor(1, 3, false)
template:addDoor(2, 3, false)
template:addDoor(4, 3, false)
template:addDoor(6, 3, false)
template:addDoor(7, 3, false)
template:setShortRangeRadarRange(7500)

template:setTubes(5, 12.0)
template:setTubeDirection(0, 0)
template:setTubeSize(0, "large")

--Canon nova
template:setCustomWeapon("Nuke", "NOVAMK2", 3, 2200.0, "Kinetic", 20)
template:setCustomWeaponColor("NOVAMK2", 0, 204, 255)
template:setCustomWeaponStorage("NOVAMK2", 6)

template:setTubeLoadTime(0, 80):setWeaponTubeExclusiveForCustom(0,'NOVAMK2')


template:setTubeDirection(1, 90):weaponTubeDisallowCustomMissile(1,'NOVAMK2')
template:setTubeDirection(2, -90):weaponTubeDisallowCustomMissile(2,'NOVAMK2')
template:setTubeDirection(3, 90):weaponTubeDisallowCustomMissile(3,'NOVAMK2')
template:setTubeDirection(4, -90):weaponTubeDisallowCustomMissile(4,'NOVAMK2')

template:setCustomWeapon("HVLI", "MAGMCAN", 1, 1000.0, "EMP", 25)
template:setCustomWeaponMultiple("MAGMCAN",1,4)
template:setCustomWeaponColor("MAGMCAN", 51, 51, 255)
template:setCustomWeaponStorage("MAGMCAN", 20)


-- macro canon EMP 
template:setCustomWeapon("HVLI", "MCANMK3", 4, 1500.0, "Kinetic", 25)
template:setCustomWeaponMultiple("MCANMK3",1,4)
template:setCustomWeaponColor("MCANMK3", 255, 150, 103)
template:setCustomWeaponStorage("MCANMK3", 120)
template:setDockClasses("Drone","Vaisseau leger","Fregate","destroyer")
template:setCanDock(true)
template:setCanHack(false)




--[[Croiseur admech lunar admech + nova / jouable  ]]
template = ShipTemplate():setName("Croiseur Lunar Admech"):setClass("Croiseur", "Explorator"):setModel("Adeptus_Mechanicus_cruiser"):setType("playership")
template:setRadarTrace("battleship.png")
template:setDescription([[ Concu pour remplacer l'ancien Croiseur Murder, le croiseur de classe Lunar est un vaisseau de conception simple et robuste pouvant etre produit par pratiquement tous les mondes imperiaux, y compris ceux incapables de produire des cuirasses ou des croiseurs.
Cette facilite de production a fait du croiseur Lunar l'epine dorsale de l'ensemble de la Flotte Imperiale. La Flotte Obscuras comprend, par exemple, près de 600 vaisseaux de cette classe repartis dans les secteurs du Segmentum Obscurus, dont une vingtaine rien que dans le Secteur Gothique. 1
De même, près d'un tiers des vaisseaux ayant participe à la Troisième Guerre d'Armageddon sous les ordres de l'Amiral Parol furent des croiseurs Lunar. 3

Alliant la portee de ses Lance-Torpilles avec la puissance de ses Batteries d'armes et Pièces d'artillerie navale, le croiseur Lunar est arme pour remplir aussi bien des rôles d'escorte, de patrouille ou d'assaut. 
La variante MK3 utilise par l'adeptus mechanicus remplace son lance torpille par un canon nova a graviton ]])

template:setHull(1500)
template:setShields(800, 800)
template:setSpeed(95, 6, 8, 45, 10)
template:setCombatManeuver(350, 200)
--                  Arc, Dir, Range, CycleTime, Dmg
template:setBeam(0, 30, 0, 4000.0, 30.0, 150)
template:setBeam(1, 90, 90, 3000.0, 14.0, 35)
template:setBeam(2, 90, -90, 3000.0, 14.0, 35)
template:setBeam(3, 90, 90, 3000, 14, 25)
template:setBeam(4, 90,-90, 3000, 14, 25)
template:setBeam(5, 90, 90, 2000, 3, 12)
template:setBeam(6, 90,-90, 2000, 3, 12)
template:setEnergyConsumptionRatio(0.92)
template:setSystemDamageRatio(0.95)
template:setSystemDamageHullThreshold(0.9)

template:setCanHack(true)
template:setJumpDrive(true)
--template:setRepairCrewCount(2)
template:addRoomSystem(0, 0, 1, 4, "RearShield")
template:addRoom(1, 0, 1, 1)
template:addRoomSystem(1, 1, 3, 2, "MissileSystem")
template:addRoom(1, 3, 1, 1)
template:addRoomSystem(2, 0, 2, 1, "Beamweapons")
template:addRoomSystem(2, 3, 2, 1, "Maneuver")
template:addRoomSystem(4, 0, 2, 1, "Warp")
template:addRoomSystem(4, 3, 2, 1, "JumpDrive")
template:addRoomSystem(5, 1, 1, 2, "Reactor")
template:addRoom(6, 0, 1, 1)
template:addRoomSystem(6, 1, 1, 2, "Impulse")
template:addRoom(6, 3, 1, 1)
template:addRoomSystem(7, 0, 1, 4, "FrontShield")
template:addDoor(1, 0, false)
template:addDoor(2, 0, false)
template:addDoor(4, 0, false)
template:addDoor(6, 0, false)
template:addDoor(7, 0, false)
template:addDoor(1, 1, true)
template:addDoor(1, 3, true)
template:addDoor(6, 1, true)
template:addDoor(6, 2, false)
template:addDoor(6, 3, true)
template:addDoor(1, 3, false)
template:addDoor(2, 3, false)
template:addDoor(4, 3, false)
template:addDoor(6, 3, false)
template:addDoor(7, 3, false)
template:setShortRangeRadarRange(7500)
template:setLongRangeRadarRange(45000)
template:setTubes(5, 12.0)
template:setTubeDirection(0, 0)
template:setTubeSize(0, "large")

--Canon nova to add effet grav 
template:setCustomWeapon("Emp", "NOVAGRAV", 3, 2200.0, "EMP", 20)
template:setCustomWeaponColor("NOVAGRAV", 0, 204, 255)
template:setCustomWeaponStorage("NOVAGRAV", 6)

template:setCustomWeapon("Emp", "XGRAV", 1.5, 2200.0, "Emp", 5)
template:setCustomWeaponColor("XGRAV", 0, 255, 0)

template:onCustomWeaponDetonation("XGRAV", function(self, typeOfDetonation, hitObject)
    print(typeOfDetonation)
    if typeOfDetonation == "HitShip" or typeOfDetonation == "Expired" then
        local posx, posy = self:getPosition()
        Script():setVariable("posx", posx):setVariable("posy", posy):setVariable("runtime", 20):run("weapon_wormhole.lua")
    end
    end)
template:setCustomWeaponStorage("XGRAV", 3)


template:setTubeLoadTime(0, 60)--:setWeaponTubeExclusiveForCustom(0,'NOVAGRAV'):setWeaponTubeExclusiveForCustom(0,'XNOVAGRAV')


template:setTubeDirection(1, 90):weaponTubeDisallowCustomMissile(1,'NOVAGRAV'):weaponTubeDisallowCustomMissile(1,'XGRAV')
template:setTubeDirection(2, -90):weaponTubeDisallowCustomMissile(2,'NOVAGRAV'):weaponTubeDisallowCustomMissile(2,'XGRAV')
template:setTubeDirection(3, 90):weaponTubeDisallowCustomMissile(3,'NOVAGRAV'):weaponTubeDisallowCustomMissile(3,'XGRAV')
template:setTubeDirection(4, -90):weaponTubeDisallowCustomMissile(4,'NOVAGRAV'):weaponTubeDisallowCustomMissile(4,'XGRAV')

template:setCustomWeapon("HVLI", "MAGMCAN", 1, 1000.0, "EMP", 25)
template:setCustomWeaponMultiple("MAGMCAN",1,4)
template:setCustomWeaponColor("MAGMCAN", 51, 51, 255)
template:setCustomWeaponStorage("MAGMCAN", 20)



-- macro canon EMP 
template:setCustomWeapon("HVLI", "MCANMK3", 4, 1500.0, "Kinetic", 25)
template:setCustomWeaponMultiple("MCANMK3",1,4)
template:setCustomWeaponColor("MCANMK3", 255, 150, 103)
template:setCustomWeaponStorage("MCANMK3", 120)
template:setDockClasses("Drone","Vaisseau leger","Fregate","destroyer")
template:setCanDock(true)
--template:setCanHack(false)


--[[Croiseur Inquisitoriale]]
template = ShipTemplate():setName("Croiseur Inquisitorial"):setClass("Croiseur", "Combat"):setModel("Inquisitorial_Cruiser")
template:setRadarTrace("battleship.png")
template:setDescription([[Les croiseurs inquisitoires sont des navires de classe Croiseur utilises par l'Inquisition. Ces navires sont specialement construits pour l'Inquisition et utilisent une technologie et des armes disponibles uniquement pour les flottes de Mars elles-memes.
En consequence, ils sont plus rapides, mieux armes et mieux proteges que les navires de meme classe chez l'Imperium. Bien que quelques modeles standard soient utilises par l'Inquisition, ils sont tout aussi souvent concus a cet effet et optimises pour des roles specifiques.
Ces navires sont generalement utilises par les inquisiteurs en mission avant d'etre transferes, bien qu'il ne soit pas rare que des inquisiteurs aient des croiseurs permanents attaches a leurs noms. ]])
template:setHull(1600)
template:setShields(750, 750)
template:setSpeed(95, 6, 8, 45, 10)
--                  Arc, Dir, Range, CycleTime, Dmg
template:setBeam(0, 45, 0, 100.0, 30.0, 100)
template:setBeam(1, 90, 90, 3000.0, 14.0, 35)
template:setBeam(2, 90, -90, 3000.0, 14.0, 35)
template:setBeam(3, 90, 90, 2000.0, 3, 9)
template:setBeam(4, 90,-90, 2000.0, 3, 9)
template:setBeam(5, 90, 90, 2000.0, 3, 9)
template:setBeam(6, 90,-90, 2000.0, 3, 9)
--                              Arc, Dir, Rotate speed
--template:setBeamWeaponTurret(1, 100,  90, 5)
--template:setBeamWeaponTurret(2, 100, -90, 5)
--template:setBeamWeaponTurret(3, 150,  90, 5)
--template:setBeamWeaponTurret(4, 150, -90, 5)
--template:setBeamWeaponTurret(5, 150,  90, 5)
--template:setBeamWeaponTurret(6, 150, -90, 5)
--       Tubes
template:setTubes(8, 12.0)
template:setTubeDirection(0, 1)template:setTubeSize(0, "small"):weaponTubeDisallowCustomMissile(0,"MCANMK3"):weaponTubeDisallowCustomMissile(0,"MAGMCAN")
template:setTubeDirection(1, -1)template:setTubeSize(1, "small"):weaponTubeDisallowCustomMissile(1,"MCANMK3"):weaponTubeDisallowCustomMissile(1,"MAGMCAN")
template:setTubeLoadTime(0, 45)
template:setTubeLoadTime(1, 45)
template:setTubeDirection(2, 90):weaponTubeDisallowCustomMissile(2,"SEEKMK2"):weaponTubeDisallowCustomMissile(2,"MAGSEEK")
template:setTubeDirection(3, -90):weaponTubeDisallowCustomMissile(3,"SEEKMK2"):weaponTubeDisallowCustomMissile(3,"MAGSEEK")
template:setTubeDirection(4, 90):weaponTubeDisallowCustomMissile(4,"SEEKMK2"):weaponTubeDisallowCustomMissile(4,"MAGSEEK")
template:setTubeDirection(5, -90):weaponTubeDisallowCustomMissile(5,"SEEKMK2"):weaponTubeDisallowCustomMissile(5,"MAGSEEK")
template:setTubeDirection(6, 90):weaponTubeDisallowCustomMissile(6,"SEEKMK2"):weaponTubeDisallowCustomMissile(6,"MAGSEEK")
template:setTubeDirection(7, -90):weaponTubeDisallowCustomMissile(7,"SEEKMK2"):weaponTubeDisallowCustomMissile(7,"MAGSEEK")

template:setDefaultAI('default')
--MGAA Skyreaper (homing tres rapide) fregrate et plus
--Torpille
template:setCustomWeapon("Nuke", "SEEKMK2", 2, 400.0, "Kinetic", 60)
template:setCustomWeaponColor("SEEKMK2", 255, 204, 0)
template:setCustomWeaponStorage("SEEKMK2", 16)
template:setCustomWeaponMultiple("SEEKMK2",1,2)
-- Torpille EMP 
template:setCustomWeapon("EMP", "MAGSEEK", 0.5, 400.0, "EMP", 30)
template:setCustomWeaponColor("MAGSEEK", 0, 204, 255)
template:setCustomWeaponStorage("MAGSEEK", 4)
template:setCustomWeaponMultiple("MAGSEEK",1,2)
-- macro canon 4 
template:setCustomWeapon("HVLI", "MCANMK3", 4, 1500.0, "Kinetic", 25)
template:setCustomWeaponMultiple("MCANMK3",1,4)
template:setCustomWeaponColor("MCANMK3", 255, 150, 103)
template:setCustomWeaponStorage("MCANMK3", 120)
-- macro canon EMP
template:setCustomWeapon("HVLI", "MAGMCAN", 1, 1000.0, "EMP", 25)
template:setCustomWeaponMultiple("MAGMCAN",1,4)
template:setCustomWeaponColor("MAGMCAN", 51, 51, 255)
template:setCustomWeaponStorage("MAGMCAN", 20)
template:setShortRangeRadarRange(7500)

--DockClasses
template:setDefaultAI('default')
template:setDockClasses("Drone","Vaisseau leger","Fregate","destroyer")
template:setCanHack(false)


--[[Croiseur Inquisitoriale PJ ]]
template = ShipTemplate():setName("Croiseur Inquisitorial MK2"):setClass("Croiseur", "Furtif"):setModel("Inquisitorial_Cruiser"):setType("playership")
template:setRadarTrace("battleship.png")
template:setDescription([[Les croiseurs inquisitoires sont des navires de classe Croiseur utilises par l'Inquisition. Ces navires sont specialement construits pour l'Inquisition et utilisent une technologie et des armes disponibles uniquement pour les flottes de Mars elles-memes.
En consequence, ils sont plus rapides, mieux armes et mieux proteges que les navires de meme classe chez l'Imperium. Bien que quelques modeles standard soient utilises par l'Inquisition, ils sont tout aussi souvent concus a cet effet et optimises pour des roles specifiques.
Ces navires sont generalement utilises par les inquisiteurs en mission avant d'etre transferes, bien qu'il ne soit pas rare que des inquisiteurs aient des croiseurs permanents attaches a leurs noms. ]])



template:setHull(1600)
template:setEnergyStorage(1500)
template:setShields(750, 750)
template:setSpeed(100, 6, 8, 45, 10)
--template:setReverseSpeed(45,10)
template:setCombatManeuver(300, 250)
template:setShortRangeRadarRange(7500)
template:setEnergyConsumptionRatio(1)
--                  Arc, Dir, Range, CycleTime, Dmg
template:setBeam(0, 20, 0, 5000.0, 30.0, 300)
template:setBeam(1, 90, 90, 3000.0, 7.0, 25)
template:setBeam(2, 90, -90, 3000.0, 7.0, 25)
template:setBeam(3, 90, 90, 2000, 3, 12)
template:setBeam(4, 90,-90, 2000, 3, 12)
--                              Arc, Dir, Rotate speed
--template:setBeamWeaponTurret(1, 100,  90, 5)
--template:setBeamWeaponTurret(2, 100, -90, 5)
--template:setBeamWeaponTurret(3, 150,  90, 5)
--template:setBeamWeaponTurret(4, 150, -90, 5)
--template:setBeamWeaponTurret(5, 150,  90, 5)
--template:setBeamWeaponTurret(6, 150, -90, 5)
--       Tubes
template:setTubes(6, 12.0)
--template:setTubeDirection(0, 1)template:setTubeSize(0, "small"):weaponTubeDisallowCustomMissile(0,"MCANMK3"):weaponTubeDisallowCustomMissile(0,"MAGMCAN")
--template:setTubeDirection(1, -1)template:setTubeSize(1, "small"):weaponTubeDisallowCustomMissile(1,"MCANMK3"):weaponTubeDisallowCustomMissile(1,"MAGMCAN")
--template:setTubeLoadTime(0, 45)
--template:setTubeLoadTime(1, 45)
template:setTubeDirection(0, 90):weaponTubeDisallowCustomMissile(2,"SEEKMK2"):weaponTubeDisallowCustomMissile(2,"MAGSEEK")
template:setTubeDirection(1, -90):weaponTubeDisallowCustomMissile(3,"SEEKMK2"):weaponTubeDisallowCustomMissile(3,"MAGSEEK")
template:setTubeDirection(2, 90):weaponTubeDisallowCustomMissile(4,"SEEKMK2"):weaponTubeDisallowCustomMissile(4,"MAGSEEK")
template:setTubeDirection(3, -90):weaponTubeDisallowCustomMissile(5,"SEEKMK2"):weaponTubeDisallowCustomMissile(5,"MAGSEEK")
template:setTubeDirection(4, 90):weaponTubeDisallowCustomMissile(6,"SEEKMK2"):weaponTubeDisallowCustomMissile(6,"MAGSEEK")
template:setTubeDirection(5, -90):weaponTubeDisallowCustomMissile(7,"SEEKMK2"):weaponTubeDisallowCustomMissile(7,"MAGSEEK")


--Torpille
--template:setCustomWeapon("Nuke", "SEEKMK2", 2, 400.0, "Kinetic", 60)
--template:setCustomWeaponColor("SEEKMK2", 255, 204, 0)
--template:setCustomWeaponStorage("SEEKMK2", 12)
--template:setCustomWeaponMultiple("SEEKMK2",1,2)
-- Torpille EMP 
--template:setCustomWeapon("EMP", "MAGSEEK", 0.5, 400.0, "EMP", 30)
--template:setCustomWeaponColor("MAGSEEK", 0, 204, 255)
--template:setCustomWeaponStorage("MAGSEEK", 6)
--template:setCustomWeaponMultiple("MAGSEEK",1,2)


template:setCustomWeapon("HVLI", "MAGMCAN", 1, 1000.0, "EMP", 25)
template:setCustomWeaponMultiple("MAGMCAN",1,4)
template:setCustomWeaponColor("MAGMCAN", 51, 51, 255)
template:setCustomWeaponStorage("MAGMCAN", 20)



-- macro canon EMP 
template:setCustomWeapon("HVLI", "MCANMK3", 4, 1500.0, "Kinetic", 25)
template:setCustomWeaponMultiple("MCANMK3",1,4)
template:setCustomWeaponColor("MCANMK3", 255, 150, 103)
template:setCustomWeaponStorage("MCANMK3", 120)


template:setDefaultAI('default')
template:setDockClasses("Drone","Vaisseau leger","Fregate","destroyer")
template:setCanHack(false)

template:addDrones("Chasseur Lourd Mk2",2)
template:addDrones("Vaisseau d'abordage Shark Mk2",2)
template:addDrones("Intercepteur Fury",4)
template:addDrones("Bombardier Starhawk",4)
template:setDocks(3, 0, 0, 0, 4, 6)


--DockClasses
template:setDockClasses("Drone","Vaisseau leger","Fregate","destroyer")
template:setDefaultAI('default')



--[[Croiseur Corsair Shadow]]
template = ShipTemplate():setName("Croiseur Corsaire Shadow"):setClass("Croiseur", "Combat"):setModel("Eldar_Corsair_Cruiser")
template:setRadarTrace("battleship.png")
template:setDescription([[Le Croiseur Shadow est un vaisseau de ligne des Flottes Eldars.
Le modele Shadow est le plus grand des vaisseaux qui composent generalement les flottes des Eldars. Comme beaucoup de vaisseaux Xenos, le Croiseur Shadow est souvent confondu avec d'autres classes de croiseur et de nombreuses attaques sont ainsi attribuees faussement a leurs cousins Eldars. ]])
template:setHull(1400)
template:setShields(500, 500)
template:setSpeed(150, 18, 18)
--template:setReverseSpeed(40,10)
--                  Arc, Dir, Range, CycleTime, Dmg
template:setBeam(0, 20, 0, 4500.0, 30.0, 300)
template:setBeam(1, 45, 0, 4000.0, 14.0, 35)
template:setBeam(2, 45, 0, 4000.0, 14.0, 35)
template:setBeam(3, 90, 90, 3000.0, 3, 12)
template:setBeam(4, 90,-90, 3000.0, 3, 12)
template:setBeam(5, 90, 90, 3000.0, 3, 12)
template:setBeam(6, 90,-90, 3000.0, 3, 12)
--                              Arc, Dir, Rotate speed

--       Tubes
template:setTubes(8, 12.0)
template:setTubeDirection(0, 1)template:setTubeSize(0, "small"):weaponTubeDisallowCustomMissile(0,"MCANMK3")
template:setTubeDirection(1, -1)template:setTubeSize(1, "small"):weaponTubeDisallowCustomMissile(1,"MCANMK3")
template:setTubeLoadTime(0, 45)
template:setTubeLoadTime(1, 45)
template:setTubeDirection(2, 90):weaponTubeDisallowCustomMissile(2,"SEEKMK2"):weaponTubeDisallowCustomMissile(2,"MAGSEEK")
template:setTubeDirection(3, -90):weaponTubeDisallowCustomMissile(3,"SEEKMK2"):weaponTubeDisallowCustomMissile(3,"MAGSEEK")
template:setTubeDirection(4, 90):weaponTubeDisallowCustomMissile(4,"SEEKMK2"):weaponTubeDisallowCustomMissile(4,"MAGSEEK")
template:setTubeDirection(5, -90):weaponTubeDisallowCustomMissile(5,"SEEKMK2"):weaponTubeDisallowCustomMissile(5,"MAGSEEK")
template:setTubeDirection(6, 90):weaponTubeDisallowCustomMissile(6,"SEEKMK2"):weaponTubeDisallowCustomMissile(6,"MAGSEEK")
template:setTubeDirection(7, -90):weaponTubeDisallowCustomMissile(7,"SEEKMK2"):weaponTubeDisallowCustomMissile(7,"MAGSEEK")

template:setWeaponStorage("EMP", 5) -- degats bouclier 150

--TG Seeker (homing) 
template:setCustomWeapon("HVLI", "MCANMK3", 4, 1500.0, "Kinetic", 25)
template:setCustomWeaponMultiple("MCANMK3",1,4)
template:setCustomWeaponColor("MCANMK3", 255, 150, 103)
template:setCustomWeaponStorage("MCANMK3", 120)

--MGAA Flakburst (nuke faible dommage) 


template:setCustomWeapon("Nuke", "SEEKMK2", 2, 400.0, "Kinetic", 60)
template:setCustomWeaponColor("SEEKMK2", 255, 204, 0)
template:setCustomWeaponStorage("SEEKMK2", 16)
template:setCustomWeaponMultiple("SEEKMK2",1,2)

-- Torpille EMP 
template:setCustomWeapon("EMP", "MAGSEEK", 0.5, 400.0, "EMP", 30)
template:setCustomWeaponColor("MAGSEEK", 0, 204, 255)
template:setCustomWeaponStorage("MAGSEEK", 10)
template:setCustomWeaponMultiple("MAGSEEK",1,2)

template:setShortRangeRadarRange(7500)
--DockClasses)
template:setDefaultAI('default')
template:setDockClasses("Drone","Vaisseau leger","Fregate","destroyer")
template:setCanHack(false)

--[[Croiseur du Chaos Carnage]]
template = ShipTemplate():setName("Croiseur du Chaos Carnage"):setClass("Croiseur", "Combat"):setModel("Chaos_Cruiser")
template:setRadarTrace("battleship.png")
template:setDescription([[Le Croiseur Carnage fut developpe par la Flotte Obscura afin d'obtenir un vaisseau pouvant fournir des tirs de soutien a longue portee a d'autres croiseurs tout en restant a l'abris des ripostes.
Basee sur des nombreux emplacements de Batteries d'armes, la conception du Carnage se retrouva entravee par les difficultes d'approvisionnement en energie pour ces armes. Ces difficultes conduisirent a de profondes dissensions au sein des commandants de la Flotte au sujet de la pertinence de cette classe de navire. ]])
template:setHull(1700)
template:setShields(650, 650)
template:setSpeed(85, 9, 8)
--template:setReverseSpeed(40,10)
--                  Arc, Dir, Range, CycleTime, Dmg
template:setBeam(0, 10, 0, 100.0, 30.0, 20)
template:setBeam(1, 90, 90, 4500.0, 14.0, 35)
template:setBeam(2, 90, -90, 4500.0, 14.0, 35)
template:setBeam(3, 90, 90, 3000.0, 3, 9)
template:setBeam(4, 90,-90, 3000.0, 3, 9)
template:setBeam(5, 90, 90, 3000.0, 3, 9)
template:setBeam(6, 90,-90, 3000.0, 3, 9)
template:setShortRangeRadarRange(7500)
--                              Arc, Dir, Rotate speed

--       Tubes
template:setTubes(8, 12.0)

template:setCustomWeapon("homing", "chaosM", 2, 600.0, "Kinetic", 60)
template:setCustomWeaponColor("chaosM", 255, 204, 0)
template:setCustomWeaponStorage("chaosM", 36)
template:setCustomWeaponMultiple("chaosM",2,3)


template:setCustomWeapon("HVLI", "MCANMK3", 4, 1500.0, "Kinetic", 25)
template:setCustomWeaponMultiple("MCANMK3",1,4)
template:setCustomWeaponColor("MCANMK3", 255, 150, 103)
template:setCustomWeaponStorage("MCANMK3", 120)



template:setTubeDirection(0, 90)template:setTubeSize(0, "small"):weaponTubeDisallowCustomMissile(0,"MCANMK3")
template:setTubeDirection(1, -90)template:setTubeSize(1, "small"):weaponTubeDisallowCustomMissile(1,"MCANMK3")
template:setTubeDirection(2, 90):weaponTubeDisallowCustomMissile(2,"SEEKMK2")
template:setTubeDirection(3, -90):weaponTubeDisallowCustomMissile(3,"SEEKMK2")
template:setTubeDirection(4, 90):weaponTubeDisallowCustomMissile(4,"SEEKMK2")
template:setTubeDirection(5, -90):weaponTubeDisallowCustomMissile(5,"SEEKMK2")
template:setTubeDirection(6, 90):weaponTubeDisallowCustomMissile(6,"SEEKMK2")
template:setTubeDirection(7, -90):weaponTubeDisallowCustomMissile(7,"SEEKMK2")



--MGAA Skyreaper (homing trés rapide) fregrate et plus

-- Torpille EMP 


--DockClasses
template:setDefaultAI('missilevolley')
template:setDockClasses("Drone","Vaisseau leger","Fregate","destroyer")

template:setCanHack(false)

--[[Croiseur Marchand Civil]]
template = ShipTemplate():setName("Croiseur Marchand Civil Graal"):setClass("Croiseur", "Marchand"):setModel("Civilian_Transport_Cruiser")
template:setRadarTrace("transport.png")
template:setDescription([[Ces transporteurs sont generalement des vaisseaux civils issus de la Flotte Marchande. Malgre leur courage et leur volonte, ces vaisseaux ne sont pas equipes pour lutter contre des vaisseaux de guerre, ne disposant que d'un armement leger contre les attaques de pirates, manquant d'officiers experimentes et n'ayant pas la motorisation permettant de semer leurs adversaires.]])
template:setHull(1000)
template:setShields(500, 500)
template:setSpeed(80, 6, 8)
--template:setReverseSpeed(40,10)
--                  Arc, Dir, Range, CycleTime, Dmg
template:setBeam(0, 10, 0, 100.0, 30.0, 10)
template:setBeam(1, 90, 90, 3000.0, 14.0, 35)
template:setBeam(2, 90, -90, 3000.0, 14.0, 35)
template:setBeam(3, 90, 90, 2000.0, 3, 9)
template:setBeam(4, 90,-90, 2000.0, 3, 9)
template:setBeam(5, 90, 90, 200.0, 3, 9)
template:setBeam(6, 90,-90, 2000.0, 3, 9)
template:setShortRangeRadarRange(7500)
--                              Arc, Dir, Rotate speed

--       Tubes
template:setTubes(8, 12.0)
template:setTubeDirection(0, 90)
template:setTubeDirection(1, -90)
template:setTubeDirection(2, 90)
template:setTubeDirection(3, -90)
template:setTubeDirection(4, 90)
template:setTubeDirection(5, -90)
--MGAA Skyreaper (homing trés rapide) fregrate et plus
template:setCustomWeapon("HVLI", "MCANMK2", 3, 1500.0, "Kinetic", 25)
template:setCustomWeaponMultiple("MCANMK2",1,4)
template:setCustomWeaponColor("MCANMK2", 255, 150, 103)
template:setCustomWeaponStorage("MCANMK2", 120)
--Macro-canon : (rafale, non homing) 

template:setDefaultAI('missilevolley')

--DockClasses
template:setDockClasses("Drone","Vaisseau leger")

--[[Croiseur Marchand Rogue]]

template = ShipTemplate():setName("Croiseur Rogue Repulse"):setClass("Croiseur", "Transport Lourd"):setModel("Rogue_Trader_Cruiser")
template:setRadarTrace("tug.png")
template:setLocaleName("Croiseur Marchand Rogue")
template:setClass("Croiseur", "Marchand","Rogue")
template:setModel("Rogue_Trader_Cruiser")
template:setDescription([[Ce croiseur repulse est construit à Kar Duniash. Rapide mais lourdement armée, Cette version concu pour les libres-marchands offre un bon compromis pour celui qui veux faire du commerce tranquillement]])
template:setHull(1600)
template:setShields(600, 600)
template:setSpeed(100, 17, 10)
--template:setReverseSpeed(40,10)
template:setDefaultAI('default')
template:setDockClasses("Drone","Vaisseau leger","Fregate","destroyer")
template:setCanHack(false)
template:setJumpDrive(false)
template:setJumpDriveRange(5000.0, 1300000.0)
--template:setJumpDriveEnergy(0.1)
template:setJumpDriveEnergy(1)

template:setTubes(8, 12.0)
template:setTubeDirection(0, 1)template:setTubeSize(0, "small"):weaponTubeDisallowCustomMissile(0,"MCANMK3"):weaponTubeDisallowCustomMissile(0,"MAGMCAN")
template:setTubeDirection(1, -1)template:setTubeSize(1, "small"):weaponTubeDisallowCustomMissile(1,"MCANMK3"):weaponTubeDisallowCustomMissile(1,"MAGMCAN")
template:setTubeLoadTime(0, 45)
template:setTubeLoadTime(1, 45)
template:setTubeDirection(2, 90):weaponTubeDisallowCustomMissile(2,"SEEKMK2"):weaponTubeDisallowCustomMissile(2,"MAGSEEK")
template:setTubeDirection(3, -90):weaponTubeDisallowCustomMissile(3,"SEEKMK2"):weaponTubeDisallowCustomMissile(3,"MAGSEEK")
template:setTubeDirection(4, 90):weaponTubeDisallowCustomMissile(4,"SEEKMK2"):weaponTubeDisallowCustomMissile(4,"MAGSEEK")
template:setTubeDirection(5, -90):weaponTubeDisallowCustomMissile(5,"SEEKMK2"):weaponTubeDisallowCustomMissile(5,"MAGSEEK")
template:setTubeDirection(6, 90):weaponTubeDisallowCustomMissile(6,"SEEKMK2"):weaponTubeDisallowCustomMissile(6,"MAGSEEK")
template:setTubeDirection(7, -90):weaponTubeDisallowCustomMissile(7,"SEEKMK2"):weaponTubeDisallowCustomMissile(7,"MAGSEEK")
template:setShortRangeRadarRange(7500)

--template:setSystemDamageRatio(1.2)


--Macro-canon : (rafale, non homing) 
template:setCustomWeapon("HVLI", "MCANMK3", 4, 1500.0, "Kinetic", 25)
template:setCustomWeaponMultiple("MCANMK3",1,4)
template:setCustomWeaponColor("MCANMK3", 255, 150, 103)
template:setCustomWeaponStorage("MCANMK3", 120)
 
-- Torpille EMP 
--template:setCustomWeapon("EMP", "MAGSEEK", 0.5, 400.0, "EMP", 30)
--template:setCustomWeaponColor("MAGSEEK", 0, 204, 255)
--template:setCustomWeaponStorage("MAGSEEK", 8)
--template:setCustomWeaponMultiple("MAGSEEK",1,2)

-- macro canon EMP
--template:setCustomWeapon("HVLI", "MAGMCAN", 1, 1000.0, "EMP", 25)
--template:setCustomWeaponMultiple("MAGMCAN",1,4)
--template:setCustomWeaponColor("MAGMCAN", 51, 51, 255)
--template:setCustomWeaponStorage("MAGMCAN", 20)
template:setDefaultAI('missilevolley')

--[[Croiseur Rogue Repulse]]
template = ShipTemplate():setName("Croiseur Rogue Repulse MK2"):setClass("Croiseur Marchand", "Experimental"):setModel("Rogue_Trader_Cruiser"):setType("playership")
template:setRadarTrace("tug.png")
template:setDescription([[Ce croiseur repulse est construit à Kar Duniash. Rapide mais lourdement armée, Cette version concu pour les libres-marchands offre un bon compromis pour celui qui veux faire du commerce tranquillement]])
template:setHull(1300)
template:setShields(800, 800)
template:setSpeed(100, 15, 10, 50, 10)
template:addDrones("Chasseur Lourd Mk2",2)
template:addDrones("Chasseur Lourd MKX2",1)
template:addDrones("Vaisseau d'abordage Shark Mk2",2)
template:addDrones("Intercepteur Fury",4)
template:addDrones("Bombardier Starhawk",4)
template:setDocks(3, 0, 0, 0, 6, 7)
template:setEnergyConsumptionRatio(0.9)

template:setJumpDrive(true)
template:setJumpDriveRange(5000.0, 1300000.0)
--template:setJumpDriveEnergy(0.1)
template:setJumpDriveEnergy(1)
--                  Arc, Dir, Range, CycleTime, Dmg
template:setBeam(0, 90, 0, 100.0, 30.0, 10)
template:setBeam(1, 90, 90, 3000.0, 14.0, 35)
template:setBeam(2, 90, -90, 3000.0, 14.0, 35)
template:setBeam(3, 90, 90, 2000.0, 3, 9)
template:setBeam(4, 90,-90, 2000.0, 3, 9)
template:setBeam(5, 90, 90, 2000.0, 3, 9)
template:setBeam(6, 90,-90, 2000.0, 3, 9)
--                              Arc, Dir, Rotate speed
template:setCustomWeapon("EMP", "XSEEK", 1.5, 600.0, "EMP", 30)
template:setCustomWeaponColor("XSEEK", 0, 204, 255)
template:setCustomWeaponStorage("XSEEK", 10)
template:setCustomWeaponMultiple("XSEEK",1,2)



--       Tubes
template:setTubes(8, 12.0)
template:setTubeDirection(0, 1)template:setTubeSize(0, "small"):weaponTubeDisallowCustomMissile(0,"MCANMK3"):weaponTubeDisallowCustomMissile(0,"MAGMCAN")
template:setTubeDirection(1, -1)template:setTubeSize(1, "small"):weaponTubeDisallowCustomMissile(1,"MCANMK3"):weaponTubeDisallowCustomMissile(1,"MAGMCAN")
template:setTubeLoadTime(0, 45)
template:setTubeLoadTime(1, 45)
template:setTubeDirection(2, 90):weaponTubeDisallowCustomMissile(2,"SEEKMK2"):weaponTubeDisallowCustomMissile(2,"MAGSEEK"):weaponTubeDisallowCustomMissile(2,"XSEEK"):weaponTubeDisallowCustomMissile(2,"TABORD")
template:setTubeDirection(3, -90):weaponTubeDisallowCustomMissile(3,"SEEKMK2"):weaponTubeDisallowCustomMissile(3,"MAGSEEK"):weaponTubeDisallowCustomMissile(3,"XSEEK"):weaponTubeDisallowCustomMissile(3,"TABORD")
template:setTubeDirection(4, 90):weaponTubeDisallowCustomMissile(4,"SEEKMK2"):weaponTubeDisallowCustomMissile(4,"MAGSEEK"):weaponTubeDisallowCustomMissile(4,"XSEEK"):weaponTubeDisallowCustomMissile(4,"TABORD")
template:setTubeDirection(5, -90):weaponTubeDisallowCustomMissile(5,"SEEKMK2"):weaponTubeDisallowCustomMissile(5,"MAGSEEK"):weaponTubeDisallowCustomMissile(5,"XSEEK"):weaponTubeDisallowCustomMissile(5,"TABORD")
template:setTubeDirection(6, 90):weaponTubeDisallowCustomMissile(6,"SEEKMK2"):weaponTubeDisallowCustomMissile(6,"MAGSEEK"):weaponTubeDisallowCustomMissile(6,"XSEEK"):weaponTubeDisallowCustomMissile(6,"TABORD")
template:setTubeDirection(7, -90):weaponTubeDisallowCustomMissile(7,"SEEKMK2"):weaponTubeDisallowCustomMissile(7,"MAGSEEK"):weaponTubeDisallowCustomMissile(7,"XSEEK"):weaponTubeDisallowCustomMissile(7,"TABORD")

--MGAA Skyreaper (homing trés rapide) fregrate et plus
-- Torpille lourde  
template:setCustomWeapon("Nuke", "SEEKMK2", 2, 400.0, "Kinetic", 60)
template:setCustomWeaponColor("SEEKMK2", 255, 204, 0)
template:setCustomWeaponStorage("SEEKMK2", 16)
template:setCustomWeaponMultiple("SEEKMK2",1,2)

-- macro canon EMP
template:setCustomWeapon("HVLI", "MAGMCAN", 1, 1000.0, "EMP", 25)
template:setCustomWeaponMultiple("MAGMCAN",1,4)
template:setCustomWeaponColor("MAGMCAN", 51, 51, 255)
template:setCustomWeaponStorage("MAGMCAN", 20)

--)
-- Torpille EMP de vargas / techno experimental 


template:setCustomWeapon("Homing", "TABORD", 0.5, 400.0, "EMP", 60)
template:setCustomWeaponColor("TABORD", 57, 142, 55)
template:setCustomWeaponStorage("TABORD", 30)
template:setCustomWeaponMultiple("TABORD",1,2)

--Macro-canon : (rafale, non homing) 
template:setCustomWeapon("HVLI", "MCANMK3", 4, 1500.0, "Kinetic", 25)
template:setCustomWeaponMultiple("MCANMK3",1,4)
template:setCustomWeaponColor("MCANMK3", 255, 150, 103)
template:setCustomWeaponStorage("MCANMK3", 120)
--DockClasses
template:setDockClasses("Drone","Vaisseau leger","Fregate","destroyer")
-- PJ
template:setJumpDrive(true)
template:setCanHack(false)
template:setCombatManeuver(250,150)
--template:setRepairCrewCount(8)
--	(H)oriz, (V)ert	   HC,VC,HS,VS, system    (C)oordinate (S)ize
template:addRoomSystem( 0, 1, 2, 4, "Impulse")
template:addRoomSystem( 2, 0, 2, 2, "RearShield")
template:addRoomSystem( 2, 2, 2, 2, "Warp")
template:addRoom( 2, 4, 2, 2)
template:addRoomSystem( 4, 1, 1, 4, "Maneuver")
template:addRoom( 5, 0, 2, 2)
template:addRoomSystem( 5, 2, 2, 2, "JumpDrive")
template:addRoomSystem( 5, 4, 2, 2, "Beamweapons")
template:addRoomSystem( 7, 1, 3, 2, "Reactor")
template:addRoomSystem( 7, 3, 3, 2, "MissileSystem")
template:addRoomSystem(10, 2, 2, 2, "FrontShield")
template:addDoor( 2, 2, false)
template:addDoor( 2, 4, false)
template:addDoor( 3, 2, true)
template:addDoor( 4, 3, false)
template:addDoor( 5, 2, false)
template:addDoor( 5, 4, true)
template:addDoor( 7, 3, false)
template:addDoor( 7, 1, false)
template:addDoor( 8, 3, true)
template:addDoor(10, 2, false)

template:setShortRangeRadarRange(7500)



--Support: mine layer
--Support: mine sweeper
--Support: science vessel
--Support: deep space recon
--Support: light repair
--Support: resupply


--[[Croiseur Marchand Il'porrui]]
template = ShipTemplate():setName("Croiseur Marchand Il'porrui"):setClass("Croiseur", "Marchand"):setModel("Tau_Merchant_Cruiser")
template:setRadarTrace("transport.png")
template:setDescription([[Les Il'porruis servent souvent de transport pour des dignitaire de la Caste de l'Eau, desCommandeurs ou des Etheres. Malgre sa fonction, il est suffisamment bien arme pour se defendre contre des ennemis du meme gabarit que lui.
Les Il'porrui sont devenus communs dans l'Empire Tau et sont meme souvent utilises en dehors lors de missions diplomatiques. Ces navires font egalement d'excellents vaisseaux de commerce et sont de vrai betes de somme au sein de leur jeune empire. ]])
template:setHull(1200)
template:setShields(1000, 1000)
template:setSpeed(80, 6, 8)
--template:setReverseSpeed(40,10)
--                  Arc, Dir, Range, CycleTime, Dmg
template:setBeam(0, 60, 0, 4500.0, 30.0, 200)
template:setBeam(1, 120, 90, 4000.0, 14.0, 35)
template:setBeam(2, 120, -90, 4000.0, 14.0, 35)
template:setBeam(3, 120, 90, 3000.0, 3, 12)
template:setBeam(4, 120,-90, 3000.0, 3, 12)
template:setBeam(5, 120, 90, 3000.0, 3, 12)
template:setBeam(6, 120,-90, 3000.0, 3, 12)
template:setShortRangeRadarRange(7500)
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
--TG Seeker (homing) 
template:setCustomWeapon("Homing", "SEEK", 1.5, 8000.0, "Kinetic", 12)
template:setCustomWeaponColor("SEEK", 0, 204, 255)
template:setCustomWeaponColor("SEEK", 0, 204, 255)
template:setCustomWeaponMultiple("SEEK",1,3)
template:setCustomWeaponStorage("SEEK", 18)
--TG fusionmelta (Nuke homing) 

template:setWeaponStorage("EMP", 5)

template:setCustomWeapon("EMP", "XSEEK", 1, 400.0, "EMP", 30)
template:setCustomWeaponColor("XSEEK", 0, 204, 255)
template:setCustomWeaponStorage("XSEEK", 10)
template:setCustomWeaponMultiple("XSEEK",1,2)

--DockClasses
template:setDefaultAI('default')
template:setDockClasses("Drone","Vaisseau leger","Fregate","destroyer")




--[[Croiseur Impérial Mars + nova / jouable + torp et laser frontal ]]
template = ShipTemplate():setName("Croiseur Mars MK2"):setClass("Croiseur", "Artillerie"):setModel("Navy_Battleship_Mars"):setType("playership")
template:setRadarTrace("battleship.png")
template:setDescription([[ Les croiseurs de combat de classe Mars sont des navires polyvalents pouvant larguer des escadrilles de Chasseurs Fury ou de Bombardiers Starhawk, utiliser ses Batteries d'armes de flanc, ses Pièces d'artillerie navale dorsales ou employer son redoutable Canon Nova de proue. 
Peu de croiseurs de combat Mars en service dans la Flotte Impériale gardent dorénavant leur configuration d'origine, malgré les récriminations des Technoprêtres les plus orthodoxes.
]])

template:setHull(2000)
template:setShields(1200, 1200) -- recharge rate
template:setSpeed(85, 6, 8, 40, 8)
----template:setReverseSpeed(40,10)
template:setSystemDamageRatio(0.8)
template:setSystemDamageHullThreshold(0.80)
template:setCombatManeuver(600, 250) -- si nerf, penser a donner une upgrade
--                  Arc, Dir, Range, CycleTime, Dmg
--template:setBeam(0, 45, 0, 100.0, 30.0, 10) -- a remplacer
template:setBeam(1, 90, 90, 3000.0, 14.0, 30)
template:setBeam(2, 90, -90, 3000.0, 14.0, 30)
template:setBeam(3, 90, 90, 2000, 3, 11)
template:setBeam(4, 90,-90, 2000, 3, 11) -- template mais pas variation 
template:setBeam(5, 90, 90, 2000, 3, 11)
template:setBeam(6, 90,-90, 2000, 3, 11)
template:setBeamWeaponEnergyPerFire(1, 1)
template:setBeamWeaponEnergyPerFire(2, 1)
template:setBeamWeaponHeatPerFire(1, 0.01)
template:setBeamWeaponHeatPerFire(2, 0.01)



--Tourelles de défense
--Axe de tir (N° d'arme, arc de tir, Orientation, portée, délais de tir, dégâts)
template:setBeamWeapon(0, 10, 0, 3000, 14, 45)
template:setBeamWeaponTurret(0, 30, 0, 15)
--Ajustement de la consommation d'énergie des tirs
template:setBeamWeaponEnergyPerFire(0, 1)
--Ajustement de la surchauffe engendrée par les tirs
template:setBeamWeaponHeatPerFire(0, 0.01)
template:setShortRangeRadarRange(7500)
--template:setBeam(0, 30, 0, 4000.0, 30.0, 150)
--exemple firestorm : template:setBeam(2,30, 0, 5000.0, 40.0, 200) 
--200.0

--template:setSystemDamageRatio(0.9)
--template:setSystemDamageHullThreshold(0.85)
template:setCanHack(false)
template:setJumpDrive(true)
template:addRoom(1, 0, 1, 1)
--template:setRepairCrewCount(2)
--[[template:addRoomSystem(0, 0, 1, 4, "RearShield")
template:addRoom(1, 0, 1, 1)
template:addRoomSystem(1, 1, 3, 2, "MissileSystem")
template:addRoom(1, 3, 1, 1)
template:addRoomSystem(2, 0, 2, 1, "Beamweapons")
template:addRoomSystem(2, 3, 2, 1, "Maneuver")
template:addRoomSystem(4, 0, 2, 1, "Warp")
template:addRoomSystem(4, 3, 2, 1, "JumpDrive")
template:addRoomSystem(5, 1, 1, 2, "Reactor")
template:addRoom(6, 0, 1, 1)
template:addRoomSystem(6, 1, 1, 2, "Impulse")
template:addRoom(6, 3, 1, 1)
template:addRoomSystem(7, 0, 1, 4, "FrontShield")
template:addDoor(1, 0, false)
template:addDoor(2, 0, false)
template:addDoor(4, 0, false)
template:addDoor(6, 0, false)
template:addDoor(7, 0, false)
template:addDoor(1, 1, true)
template:addDoor(1, 3, true)
template:addDoor(6, 1, true)
template:addDoor(6, 2, false)
template:addDoor(6, 3, true)
template:addDoor(1, 3, false)
template:addDoor(2, 3, false)
template:addDoor(4, 3, false)
template:addDoor(6, 3, false)
template:addDoor(7, 3, false)
--]]
template:setShortRangeRadarRange(7500) -- a tester face a 4500

template:setTubes(8, 12.0)
template:setTubeDirection(0, 0)
template:setTubeSize(0, "large")
--Canon nova
template:setCustomWeapon("Nuke", "NOVAMK2", 3, 2200.0, "Kinetic", 20) -- 3 = facteur de degats face au commun
template:setCustomWeaponColor("NOVAMK2", 0, 204, 255)
template:setCustomWeaponStorage("NOVAMK2", 6)


template:setTubeLoadTime(0, 80):setWeaponTubeExclusiveForCustom(0,'NOVAMK2')


template:setTubeDirection(7, 0)
template:setTubeSize(7, "large")
template:setCustomWeapon("Nuke", "NOVAMK3", 3, 2200.0, "Kinetic", 20) -- 3 = facteur de degats face au commun
template:setCustomWeaponColor("NOVAMK3", 0, 204, 255)
template:setCustomWeaponStorage("NOVAMK3", 2)
template:setTubeLoadTime(7, 600):weaponTubeDisallowCustomMissile(7,'NOVAMK2'):weaponTubeDisallowCustomMissile(7,'SEEKMK2'):weaponTubeDisallowCustomMissile(7,'MAGSEEK'):weaponTubeDisallowCustomMissile(7,'MAGMCAN'):weaponTubeDisallowCustomMissile(7,'MAGMCAN'):weaponTubeDisallowCustomMissile(7,'MCANMK3'):weaponTubeDisallowCustomMissile(7,'NOVAMK3')


template:setTubeDirection(1, 90)
template:setTubeDirection(2, -90)
template:setTubeDirection(3, 90)
template:setTubeDirection(4, -90)
template:setTubeLoadTime(1, 12):weaponTubeDisallowCustomMissile(1,'NOVAMK2'):weaponTubeDisallowCustomMissile(1,'SEEKMK2'):weaponTubeDisallowCustomMissile(1,'MAGSEEK'):weaponTubeDisallowCustomMissile(1,'NOVAMK3')
template:setTubeLoadTime(2, 12):weaponTubeDisallowCustomMissile(2,'NOVAMK2'):weaponTubeDisallowCustomMissile(2,'SEEKMK2'):weaponTubeDisallowCustomMissile(2,'MAGSEEK'):weaponTubeDisallowCustomMissile(2,'NOVAMK3')
template:setTubeLoadTime(3, 12):weaponTubeDisallowCustomMissile(3,'NOVAMK2'):weaponTubeDisallowCustomMissile(3,'SEEKMK2'):weaponTubeDisallowCustomMissile(3,'MAGSEEK'):weaponTubeDisallowCustomMissile(3,'NOVAMK3')
template:setTubeLoadTime(4, 12):weaponTubeDisallowCustomMissile(4,'NOVAMK2'):weaponTubeDisallowCustomMissile(4,'SEEKMK2'):weaponTubeDisallowCustomMissile(4,'MAGSEEK'):weaponTubeDisallowCustomMissile(4,'NOVAMK3')

--Torpille
template:setTubeDirection(5, 1)
template:setTubeLoadTime(5, 20):weaponTubeDisallowCustomMissile(5,'NOVAMK2'):weaponTubeDisallowCustomMissile(5,'MCANMK3'):weaponTubeDisallowCustomMissile(5,'MAGMCAN'):weaponTubeDisallowCustomMissile(5,'NOVAMK3')
template:setTubeSize(5,"small")
template:setCustomWeapon("Nuke", "SEEKMK2", 2, 400.0, "Kinetic", 60)
template:setCustomWeaponColor("SEEKMK2", 255, 204, 0)
template:setCustomWeaponStorage("SEEKMK2", 16)
template:setCustomWeaponMultiple("SEEKMK2",1,2)
-- Torpille EMP 
template:setTubeDirection(6, -1)
template:setTubeSize(6, "small")
template:setTubeLoadTime(6, 20):weaponTubeDisallowCustomMissile(6,'NOVAMK2'):weaponTubeDisallowCustomMissile(6,'MCANMK3'):weaponTubeDisallowCustomMissile(6,'MAGMCAN'):weaponTubeDisallowCustomMissile(6,'NOVAMK3')
template:setCustomWeapon("EMP", "MAGSEEK", 0.5, 400.0, "EMP", 30)
template:setCustomWeaponColor("MAGSEEK", 0, 204, 255)
template:setCustomWeaponStorage("MAGSEEK", 4)
template:setCustomWeaponMultiple("MAGSEEK",1,2)

-- attention aux homing missiles
-- macro canon EMP 
template:setCustomWeapon("HVLI", "MAGMCAN", 1, 1000.0, "EMP", 25)
template:setCustomWeaponMultiple("MAGMCAN",1,4)
template:setCustomWeaponColor("MAGMCAN", 51, 51, 255)
template:setCustomWeaponStorage("MAGMCAN", 20)

-- macro canon kinetic
template:setCustomWeapon("HVLI", "MCANMK3", 4, 1500.0, "Kinetic", 25)
template:setCustomWeaponMultiple("MCANMK3",1,4)
template:setCustomWeaponColor("MCANMK3", 255, 150, 103)
template:setCustomWeaponStorage("MCANMK3", 120)

--variation:setDockClasses("Drone","Vaisseau leger","Fregate","destroyer")  A TESTER
template:setDockClasses("Drone","Vaisseau leger","Fregate","destroyer","Chasseur")

template:setCanDock(true)
template:setCanHack(false)

template:registerSquadronComposition("Ch", 5, 15, "Chasseur Mk2", "Chasseur", "Chasseur","Chasseur")
template:registerSquadronComposition("Ch2", 5, 15, "Chasseur Mk2", "Chasseur", "Chasseur","Chasseur")
template:registerSquadronComposition("Ch3", 5, 15, "Chasseur Mk2", "Chasseur", "Chasseur","Chasseur")
template:registerSquadronComposition("Ch4", 5, 15, "Chasseur Mk2", "Chasseur", "Chasseur","Chasseur")
template:setBlueprintAvailable("Ch", "Ch3")

--Dock/balise de renfort de la marine

--template:addDrones("Fregate sword",1)
--template:addDrones("Fregate Firestorm",1)
--template:addDrones("Vaisseau d'abordage Shark Mk2",2)
--template:addDrones("Destroyer widowmaker MK2",1)
--template:setDocks(3, 0, 0, 0, 6, 7)

-- mettre deux sword / deux firestorm en possibilité de summon