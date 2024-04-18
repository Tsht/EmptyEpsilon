--[[                  Stations
Rappel des categories : 
Vaisseau leger : 1 a 3 places ; aucun moteur Warp ou Jump possible ; shiptemplates_VaisseauxLegers.lua ;
Fregate : 4 a 9 places ; shiptemplates_Fregates.lua ;
Destroyer : 10 a 50 places ; shiptemplates_Escorteurs.lua ;
Croiseur Leger : 50 a 100 places ; shiptemplates_CroiseursLegers.lua ;
Croiseur : 100 a 250 places ; shiptemplates_Croiseurs.lua ;
Cuirasse : 250 a 500 places ;
Stations;

Les Stations apporte un soutien aux vaisseaux de combat et sert d'atelier de reparations.
----------------------------------------------------------]]

	
template = ShipTemplate():setName("Petite Station"):setModel("space_station_4"):setType("station"):setClass(_("Installation Fixe"), _("subclass", "Support"))
template:setDescription([[ Petite station spatiale, elle sert pour le ravitaillement en espace profond]])
template:setHull(150)
template:setShields(300)
template:setRadarTrace("smallstation.png")
template:setDockClasses("Drone","Vaisseau leger","Fregate","destroyer","Croiseur leger",'croiseur','Cuirasse','Vaisseau-Mere')

template:setSharesEnergyWithDocked(true)
template:setRepairDocked(true)
template:setRestocksScanProbes(true)

template = ShipTemplate():setName("Station Moyenne"):setModel("space_station_3"):setType("station"):setClass(_("Installation Fixe"), _("subclass", "Support"))
template:setDescription([[Station de taille moyenne, bien equipe, servant au ravitaillement et au commerce en espace profond]])
template:setHull(400)
template:setShields(800)
template:setRadarTrace("mediumstation.png")
template:setDockClasses("Drone","Vaisseau leger","Fregate","destroyer","Croiseur leger",'croiseur','Cuirasse','Vaisseau-Mere')
template:setSharesEnergyWithDocked(true)
template:setRepairDocked(true)
template:setRestocksScanProbes(true)



template = ShipTemplate():setName("Grande Station"):setModel("space_station_2"):setType("station"):setClass(_("Installation Fixe"), _("subclass", "Support"))
template:setDescription([[De la taille d'une ville tentaculaire, les stations de cette echelle representent le centre de la puissance spatiale de cette region. Elles remplissent plusieurs fonctions a la fois et representent un investissement considerable en temps, en argent et en main d'oeuvre. Les boucliers et l'epaisse coque d'une enorme station peuvent la maintenir intacte assez longtemps pour que des renforts arrivent, même en cas de siege ou d'assaut massif parfaitement coordonne.]])
template:setHull(500)
template:setShields(1000, 1000, 1000)
template:setRadarTrace("largestation.png")
template:setDockClasses("Drone","Vaisseau leger","Fregate","destroyer","Croiseur leger",'croiseur','Cuirasse','Vaisseau-Mere')



template:setSharesEnergyWithDocked(true)
template:setRepairDocked(true)
template:setRestocksScanProbes(true)

template = ShipTemplate():setName("Immense Station"):setModel("space_station_1"):setType("station"):setClass(_("Installation Fixe"), _("subclass", "Support"))
template:setDescription([[De la taille d'une ville tentaculaire, les stations de cette echelle representent le centre de la puissance spatiale de cette region. Elles remplissent plusieurs fonctions a la fois et representent un investissement considerable en temps, en argent et en main d'oeuvre. Les boucliers et l'epaisse coque d'une enorme station peuvent la maintenir intacte assez longtemps pour que des renforts arrivent, même en cas de siege ou d'assaut massif parfaitement coordonne.]])
template:setHull(800)
template:setShields(1200, 1200, 1200, 1200)
template:setRadarTrace("hugestation.png")
template:setDockClasses("Drone","Vaisseau leger","Fregate","destroyer","Croiseur leger",'croiseur','Cuirasse','Vaisseau-Mere')


template:setSharesEnergyWithDocked(true)
template:setRepairDocked(true)
template:setRestocksScanProbes(true)





--[[Grande station Imperial]]
template = ShipTemplate():setName("Grande station Imperial"):setModel("Navy_Station"):setType("station"):setClass(_("Installation Fixe"), _("subclass", "Support"))
template:setDescription([[De la taille d'une ville tentaculaire, les stations de cette echelle representent le centre de la puissance spatiale de cette region. Elles remplissent plusieurs fonctions a la fois et representent un investissement considerable en temps, en argent et en main d'oeuvre. Les boucliers et l'epaisse coque d'une enorme station peuvent la maintenir intacte assez longtemps pour que des renforts arrivent, même en cas de siege ou d'assaut massif parfaitement coordonne.]])
template:setHull(3000)
template:setShields(1000, 1000, 1000, 1000)
template:setRadarTrace("hugestation.png")
--                  Arc, Dir, Range, CycleTime, Dmg
template:setBeam(0, 100, 1, 100.0, 30.0, 175)
template:setBeam(1, 100, 180, 100.0, 30.0, 175)
template:setBeam(2, 100, 90, 100.0, 30.0, 175)
template:setBeam(3, 100, -90, 100.0, 30.0, 175)
template:setBeam(4, 100, 45, 2000.0, 7.0, 25)
template:setBeam(5, 100, -45, 2000.0, 7.0, 25)
template:setBeam(6, 100, 135, 2000.0, 7.0, 25)
template:setBeam(7, 100, -135, 2000.0, 7.0, 25)
template:setBeam(8, 100, 42, 2000.0, 1.5, 6)
template:setBeam(9, 100, -42, 2000.0, 1.5, 6)
template:setBeam(10, 100, 48, 2000.0, 1.5, 6)
template:setBeam(11, 100,-48, 2000.0, 1.5, 6)
template:setBeam(12, 100, 132, 2000.0, 1.5, 6)
template:setBeam(13, 100,-132, 2000.0, 1.5, 6)
template:setBeam(14, 100, 138, 2000.0, 1.5, 6)
template:setBeam(15, 100,-138, 2000.0, 1.5, 6)
template:setDockClasses("Drone","Vaisseau leger","Fregate","Destroyer","Croiseur leger","Croiseur","Cuirasse","Vaisseau-Mere","Alpha")
template:setCanDock(true)


template:setRestocksMissilesDocked(true)
template:setSharesEnergyWithDocked(true)
template:setRepairDocked(true)
template:setRestocksScanProbes(true)



--                              Arc, Dir, Rotate speed

--       Tubes
template:setTubes(10, 12.0)
template:setTubeDirection(0, 1)
template:setTubeDirection(1, -1)
template:setTubeDirection(2, 90)
template:setTubeDirection(3, -90)
template:setTubeDirection(4, 90)
template:setTubeDirection(5, -90)
template:setTubeDirection(6, 90)
template:setTubeDirection(7, -90)
template:setTubeDirection(8, 90)
template:setTubeDirection(9, -90)
--TG Seeker (homing) 
--template:setCustomWeapon("Homing", "SEEK", 1.5, 800.0, "Kinetic", -1)
--template:setCustomWeaponColor("SEEK", 255, 204, 0)
--template:setCustomWeaponStorage("SEEK", 60)
--template:setCustomWeaponMultiple("SEEK",1,4)

--Macro-canon : (rafale, non homing) 
--template:setCustomWeapon("HVLI", "MCANMK3", 6, 1500.0, "Kinetic", 25)
--template:setCustomWeaponMultiple("MCANMK3",1,4)
--template:setCustomWeaponColor("MCANMK3", 255, 150, 103)
--template:setCustomWeaponStorage("MCANMK3", 90)
--MGAA Flakburst (nuke faible dommage) 
template:setCustomWeapon("Nuke", "FLAK", 0.5, 500.0, "Kinetic", -1)
template:setCustomWeaponColor("FLAK", 220, 87, 20)
template:setCustomWeaponStorage("FLAK", 6)





-- The weapons-platform is a stationary platform with beam-weapons. It's extremely slow to turn, but it's beam weapons do a huge amount of damage.
-- Smaller ships can dock to this platform to re-supply.
template = ShipTemplate():setName("Plateforme de defense"):setLocaleName(_("Plateforme de defense")):setClass(_("Installation Fixe"), _("subclass", "Support")):setModel("space_station_4")
template:setDescription(_([[Stations de defense rotative standard.]]))
template:setRadarTrace("smallstation.png")
template:setHull(600)
template:setShields(400, 400, 400, 400, 400, 400)
template:setSpeed(0, 0.5, 0)	
template:setDockClasses("Drone","Vaisseau leger","Fregate","Destroyer","Croiseur leger","Croiseur","Cuirasse","Vaisseau-Mere","Alpha")

--               Arc, Dir, Range, CycleTime, Dmg
template:setBeam(0, 30,   0, 5000.0, 1.5, 50)
template:setBeam(1, 30,  60, 5000.0, 1.5, 50)
template:setBeam(2, 30, 120, 5000.0, 1.5, 50)
template:setBeam(3, 30, 180, 5000.0, 1.5, 50)
template:setBeam(4, 30, 240, 5000.0, 1.5, 50)
template:setBeam(5, 30, 300, 5000.0, 1.5, 50)



template:setRestocksMissilesDocked(true)
template:setSharesEnergyWithDocked(true)
template:setRepairDocked(true)
template:setRestocksScanProbes(true)

-- The weapons-platform is a stationary platform with beam-weapons. It's extremely slow to turn, but it's beam weapons do a huge amount of damage.
-- Smaller ships can dock to this platform to re-supply.
template = ShipTemplate():setName("Laser de defense planetaire"):setLocaleName(_("laser planetaire")):setClass(_("defense"), _("subclass", "Support")):setModel("space_station_4")
template:setDescription(_([[Laser de defense standard.]]))
template:setRadarTrace("smallstation.png")
template:setHull(1000)
template:setShields(1200, 1200, 1200, 1200, 1200, 1200)
template:setSpeed(0, 0.5, 0)

template:setDockClasses("Drone","Vaisseau leger","Fregate","Destroyer","Croiseur leger","Croiseur","Cuirasse","Vaisseau-Mere","Alpha")
--               Arc, Dir, Range, CycleTime, Dmg
template:setBeam(0, 30,   0, 7000.0, 10, 150)
template:setBeam(1, 30,  60, 7000.0, 10, 150)
template:setBeam(2, 30, 120, 7000.0, 10, 150)
template:setBeam(3, 30, 180, 7000.0, 10, 150)
template:setBeam(4, 30, 240, 7000.0, 10, 150)
template:setBeam(5, 30, 300, 7000.0, 10, 150)





--[[Réappro imperial ]]
template = ShipTemplate():setName("Plateforme logistique militaire"):setModel("Navy_Station"):setClass(_("Installation Fixe"), _("subclass", "Support"))
template:setDescription([[De la taille d'une ville tentaculaire, les stations de cette echelle representent le centre de la puissance spatiale de cette region. Elles remplissent plusieurs fonctions a la fois et representent un investissement considerable en temps, en argent et en main d'oeuvre. Les boucliers, les armes et l'epaisse coque d'une enorme station peuvent la maintenir intacte assez longtemps pour que des renforts arrivent, même en cas de siege ou d'assaut massif parfaitement coordonne.]])
template:setHull(2000)
template:setShields(700, 700, 700,700)
template:setSpeed(0, 0.5, 0)
template:setRadarTrace("hugestation.png")
--                  Arc, Dir, Range, CycleTime, Dmg

template:setCustomWeapon("HVLI", "XLAC", 8, 2200.0, "Kinetic", -1)
template:setCustomWeaponMultiple("XLAC",4,2)
template:setCustomWeaponColor("XLAC", 255, 51, 0)
template:setCustomWeaponStorage("XLAC", 180)

template:setCustomWeapon("Nuke", "FLAK", 0.25, 1000.0, "Kinetic", -1)
template:setCustomWeaponColor("FLAK", 220, 87, 20)
template:setCustomWeaponStorage("FLAK", 12)	

template:setTubes(6, 6.0)
template:setTubeDirection(0, 0)
template:setTubeDirection(1, 60)
template:setTubeDirection(2, 120)
template:setTubeDirection(3, 180)
template:setTubeDirection(4, 240)
template:setTubeDirection(5, 300)

template:setTubeSize(0, "large")
template:setTubeSize(1, "large")
template:setTubeSize(2, "large")
template:setTubeSize(3, "large")
template:setTubeSize(4, "large")
template:setTubeSize(5, "large")




template:setDockClasses("Drone","Vaisseau leger","Fregate","Destroyer","Croiseur leger","Croiseur","Cuirasse","Vaisseau-Mere","Alpha")
template:setCanDock(true)
template:setRestocksMissilesDocked(true)
template:setSharesEnergyWithDocked(true)
template:setRepairDocked(true)
template:setRestocksScanProbes(true)

template = ShipTemplate():setName("Station Relai"):setLocaleName(_("station relai")):setClass(_("Installation Fixe"), _("subclass", "Support")):setModel("space_station_4"):setType("playership")
template:setDescription(_([[Stations de defense rotative standard.]]))
template:setRadarTrace("smallstation.png")
template:setHull(2000)
template:setShields(400, 400, 400, 400, 400, 400)
template:setSpeed(0, 0.5, 0)	
template:setDockClasses("Drone","Vaisseau leger","Fregate","Destroyer","Croiseur leger","Croiseur","Cuirasse","Vaisseau-Mere","Alpha")

--               Arc, Dir, Range, CycleTime, Dmg
template:setBeam(0, 30,   0, 5000.0, 1.5, 50)
template:setBeam(1, 30,  60, 5000.0, 1.5, 50)
template:setBeam(2, 30, 120, 5000.0, 1.5, 50)
template:setBeam(3, 30, 180, 5000.0, 1.5, 50)
template:setBeam(4, 30, 240, 5000.0, 1.5, 50)
template:setBeam(5, 30, 300, 5000.0, 1.5, 50)
