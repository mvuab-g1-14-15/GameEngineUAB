dofile("./data/scripts/Blash.lua")
class 'CBlaster'

function CBlaster:__init()
    self.TimePressed = 0.0
    self.MaxTimePressed = 3.0
	
	self.BaseDamage = 20.0
	self.MaxDamage = 50.0
	
	self.IsAcumulatorSound = false
	
	self.Energy = 100.0
	self.Multiplicador = 2
	
	self.ShootSpeed = 50.0
	
	self.Shoots = {}
	
	self.IsShooting = false
	FinishShooting = false
	countdowntimer_manager:AddTimer("BlasterFinish", 0.2, false)
	countdowntimer_manager:AddTimer("BlasterChargedFinish", 0.5, false)
	
	self.Blash = CBlash( "Player" )
	self.Ammunition = {}
	self.MaxAmmo = 25;
	self.AmmoId = 0;
	for i=1,self.MaxAmmo do
		table.insert( self.Ammunition, CAmmo(i) );
		self.AmmoId = self.AmmoId + 1;
	end
	
	self.AmmoCharged = CAmmoCharged();
end

function CBlaster:CalculateDamage()
    local l_Percentage = self.TimePressed / self.MaxTimePressed
	local damage = (1 - l_Percentage) * self.BaseDamage + l_Percentage * self.MaxDamage
	if l_Percentage == 1 then
		damage = self.MaxDamage + self.MaxDamage / 2.0
	end
	return damage
end

function CBlaster:HasAmmo()
	if #self.Ammunition == 0 then
		return false;
	else
		return true;
	end
end

function CBlaster:UpdateAmmo()
	for i=1,#self.Ammunition do
		lAmmo = self.Ammunition[i];
		lAmmo:Update();
	end
end

function CBlaster:EndAmmo()
	for i=1,#self.Ammunition do
		lAmmo = self.Ammunition[i];
		lAmmo:End();
	end
end

function CBlaster:Shoot( aPosition )
	self.Blash:Begin(aPosition)
	local lAlreadyShoot = false;
	for i=1,#self.Ammunition do
		lAmmo = self.Ammunition[i];
		if not lAmmo:IsActive() and not lAlreadyShoot then
			local lDirection = camera_manager:GetCurrentCamera():GetDirection()
			lAmmo:Begin( aPosition, lDirection, 30.0, self:CalculateDamage() );
			lAlreadyShoot = true;
		end
		-- Update the impacted ones
		if lAmmo:IsImpacted() then
			lAmmo:End();
		end
	end
end

function CBlaster:ShootCharged( aPosition )
	engine:Trace("CBlaster:Charged:Begin")
	self.AmmoCharged:Begin( aPosition, camera_manager:GetCurrentCamera():GetDirection(), self:CalculateDamage() );
end

function CBlaster:GetEnemyFromRay()
	local l_OriRay = camera_manager:GetCurrentCamera():GetPosition()
	local l_DirRay = camera_manager:GetCurrentCamera():GetDirection()
    l_DirRay:Normalize()
	local l_ImpactMask = BitOr(2 ^ CollisionGroup.ECG_ENEMY.value, 2 ^ CollisionGroup.ECG_ESCENE.value)
	local l_EnemyName = physic_manager:RaycastClosestActorName(l_OriRay, l_DirRay, l_ImpactMask)
    return enemy_manager:GetResource(l_EnemyName)
end

function CBlaster:Update( aPosition )
	if not g_ConsoleActivate and not g_CinematicActive then
		self:UpdateAmmo();
		self.AmmoCharged:Update( aPosition )
		if action_manager:DoAction("Shoot") then
			if self.Energy > 1 then
				self.IsShooting = true
				self.FinishShooting = false
				if self.TimePressed == 0 then
					g_Player:SetAnimation("charge_loop")
				end
				if self.TimePressed < self.MaxTimePressed then
					--Implementar shoot acumulado
					self.TimePressed = self.TimePressed + timer:GetElapsedTime()
				else
					self.TimePressed = self.MaxTimePressed
				end
				if self.TimePressed  > (self.MaxTimePressed * 0.1) and not self.IsAcumulatorSound then 
					sound_manager:PlayEvent( "Play_Acumulator_Long_Shoot_Event", "Logan" )
					g_Player:SetAnimation("charge_loop")
					self.IsAcumulatorSound = true
				end
			end
		end
		if action_manager:DoAction("ShootUp") then
			engine:Trace("ShootUp")
			if self.Energy > 1 then
				if self.TimePressed < (self.MaxTimePressed * 0.1) then
					sound_manager:PlayEvent( "Play_Short_Shoot_Event", "Logan" )
					self.Energy = self.Energy - 1
					self:Shoot( aPosition )
				else
					sound_manager:PlayEvent( "Play_Long_Shoot_Event", "Logan" )
					self.Energy = self.Energy - (self.TimePressed*self.Multiplicador)
					self:ShootCharged( aPosition )
				end
				g_Player:SetAnimation("shoot")
			else
			--SONIDO DE PEDO AQUI
			end
			self.TimePressed = 0.0
			self.IsAcumulatorSound = false
			self.FinishShooting = true
			countdowntimer_manager:SetActive("BlasterFinish", true)
		end
		
		-- Miramos que nunca tengamos un energy negativo, para que la barra de energia no haga cosas raras
		if self.Energy < 1 then
			self.Energy = 0;
		end
		
		if self.FinishShooting then
			if countdowntimer_manager:isTimerFinish("BlasterFinish") then
				self.IsShooting = false
				self.FinishShooting = false
				countdowntimer_manager:Reset("BlasterFinish", false)
			end
		end
	end
	
	--Update the blash
	local lDirection = camera_manager:GetCurrentCamera():GetDirection()
	self.Blash:Update( aPosition + lDirection* 0.1)
end

function CBlaster:GetIsShooting()
	return self.IsShooting
end

function CBlaster:GetEnergy()
	return self.Energy
end

function CBlaster:SetEnergy(amount)
	self.Energy = amount
end
