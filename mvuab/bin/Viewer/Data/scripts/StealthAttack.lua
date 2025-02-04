
class 'CStealthAttack'

function CStealthAttack:__init()
	self.Doing = false
	
	self.MaxDistance = 3.0
	self.MaxAngle = 0.507
	self.Show = 0
    engine:Trace("StealthAttack initialized")
end

function CStealthAttack:Update()
	self:UpdateInput()
	local enemy = physic_manager:GetClosestEnemy()
	if enemy == nil then
		self.Doing = false
		return
	end
			
	local dist = PlayerDistance(enemy)
	if dist < self.MaxDistance then
		local vPlayerEnemy = enemy:GetPosition() - GetPlayerPosition()
		vPlayerEnemy:Normalize()
		local enemyDirection = enemy:GetDirection()
		
		self.Show = self.Show + 1
		if self.Doing then	
			local angle = GetAngleEnemyPlayer(enemy)
			engine:Trace("Angulo".. angle)
			if angle > self.MaxAngle then
				engine:Trace("Enemy life before: " .. enemy:GetLife())
				enemy:AddDamage(enemy:GetLife())
				engine:Trace("Enemy life after: " .. enemy:GetLife())
			end
		end
	else
		if self.Show > 0 then
			self.Show = 0
			gui_manager:ShowStaticText("StealthAttack")
		end
	end
	
	if self.Show == 1 then
		gui_manager:ShowStaticText("StealthAttack")
	end
	self.Doing = false
end

function CStealthAttack:UpdateInput()
	if not g_ConsoleActivate and not g_CinematicActive then
		if action_manager:DoAction("StealthAttack") then
			self.Doing = true
			engine:Trace("Stealth attack press")
		end
	end
end