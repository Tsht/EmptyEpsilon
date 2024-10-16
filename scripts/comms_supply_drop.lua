--- Supply ship comms.
--
-- Stripped comms that do not allow any interaction.
-- Used for transport ships spawned in `util_random_transports.lua`.
--
-- TODO `player` can be replaced by `comms_source`
--
-- @script comms_supply_drop

--- Main menu.
function commsShipMainMenu(comms_source, comms_target)
    if comms_source:isFriendly(comms_target) then
        setCommsMessage(_("commsShip", "Transporting goods."))
        return true
    end
    if player:isEnemy(comms_target) then
        return false
    end
    setCommsMessage(_("commsShip", "We have nothing for you.\nGood day."))
end

commsShipMainMenu()
