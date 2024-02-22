-- Name: Basic Template
-- Description: Bac à sable (vide)
---
-- Type: Basic

require("utils.lua")
-- For this scenario, utils.lua provides:
--   vectorFromAngle(angle, length)
--      Returns a relative vector (x, y coordinates)
--   setCirclePos(obj, x, y, angle, distance)
--      Returns the object with its position set to the resulting coordinates.

require("ee.lua") --tsht for systems
require("luax.lua") --tsht for table functions
require("utils_mars21.lua") --tsht toutes les fonctions necessaires pour mars

-- Global variables
-- local xxx
-- End global variables



--Est appele pour tout nouveau vaisseau, CPU ou joueur (mais pas pour les stations)
--You should only have to modify what is between "Begin balance modification" and "end balance modification"
onNewShip(
    function(ship)
		doOnNewShip(ship)
    end
)


-- a tester
onNewPlayerShip(
	function(pc)
		doOnNewPlayerShip(pc)
		print("!!")
		print("beep boop")
	pc:registerModifier("comp", "recup_energie", "activated")
	pc:onModifierToggle(function(pc,name,state)
	print(name)
	print(state)
--		if((name == "recup_energie") and (state == "activated")) then
--			pc:setSystemPowerFactor("reactor", -35)
--		elseif ((name == "recup_energie") and (state == "deactivated")) then
	--		pc:setSystemPowerFactor("reactor", -30)
--		end
		end)
	end
)
	


--	pc:registerModifier("comp", "manoeuvre", "activated")
--[[	pc:onModifierToggle(function(pc,name,state) -- ne fonctionne pas pour le moment - cyv
		print(name)
		print(state)
		if((name == "manoeuvre") and (state == "activated")) then
			pc:setCombatManeuver(600, 250)
		elseif ((name == "manoeuvre") and (state == "deactivated")) then
			pc:setCombatManeuver(500, 200)
		end
	end)
	pc:registerModifier("comp", "Turn_rate", "activated")

	pc:onModifierToggle(function(pc,name,state)-- ne fonctionne pas pour le moment - cyv
		print(name)
		print(state)
		if((name == "Turn_rate") and (state == "activated")) then
			pc:setSpeed(85, 8, 8, 40, 8)
		elseif ((name == "Turn_rate") and (state == "deactivated")) then
			pc:setSpeed(85, 6, 8, 40, 8)
		end
	end)

	pc:registerModifier("comp", "cloaking", "activated")
	pc:onModifierToggle(function(pc,name,state) -- ne fonctionne pas pour le moment - cyv
		print(name)
		print(state)
		if((name == "cloaking") and (state == "activated")) then
			pc:setCloaking(true)
		elseif ((name == "cloaking") and (state == "deactivated")) then
			pc:setCloaking(false)
		end
	end)

	pc:registerModifier("comp", "hacking", "activated")
	pc:onModifierToggle(function(pc,name,state) -- ne fonctionne pas pour le moment - cyv
		print(name)
		print(state)
		if((name == "hacking") and (state == "activated")) then
			pc:setCanHack(true)
		elseif ((name == "hacking") and (state == "deactivated")) then
			pc:setCanHack(false)
		end
	end)

	pc:registerModifier("comp", "shield regen", "activated")
	pc:onModifierToggle(function(pc,name,state)-- ne fonctionne pas pour le moment - cyv
		print(name)
		print(state)
		if((name == "shield regen") and (state == "activated")) then
			pc:setShieldRechargeRate(70)
		elseif ((name == "shield regen") and (state == "deactivated")) then
			pc:setShieldRechargeRate(60)
		end
	end)
end)
]]--


--- Initialize scenario.
function init()
	doInit()
	

end

--- Update.
--
-- @tparam number delta the time delta (in seconds)
function update(delta)
   	doUpdateUtils(delta)
end 
