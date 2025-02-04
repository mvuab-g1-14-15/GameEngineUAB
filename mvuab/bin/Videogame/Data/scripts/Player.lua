dofile("./data/scripts/PlayerController.lua")
dofile("./data/scripts/Blaster.lua")
dofile("./data/scripts/Grenade.lua")
dofile("./data/scripts/StealthAttack.lua")

class "CPlayer"

function CPlayer:__init()
	self.PlayerController = CPlayerController()
	
	self.GrenadeQueue = {}
	self.GrenadeQueue[#self.GrenadeQueue + 1] = CGrenade()
	self.GrenadeQueue[#self.GrenadeQueue + 1] = CGrenade()
	
	self.Blaster = CBlaster()
	self:SetEnergy(0.0) --TODO: Change to 0.0
	self.StealthAttack = CStealthAttack()
	self.Grenade = nil
	
	self.InsideBarrel = false
	self.BarrelName = ""
	
	self.MeshOffset = Vect3f(0.4, -0.1, 0.0)
	
	self.RenderableObject = renderable_objects_manager_characters:GetResource("Player")
	if self.RenderableObject == nil then
		self.RenderableObject = CreateAnimatedInstanceModel("Player", "Player")
		self.RenderableObject:SetRoomName( "sala1" )
		renderable_objects_manager_characters:AddResource("Player", self.RenderableObject)
	end

	local l_MeshPosition = self.PlayerController:GetPosition()
	self.RenderableObject:SetPosition(l_MeshPosition);
	self.RenderableObject:SetYaw(-self.PlayerController:GetYaw() + 4*g_HalfPi);
	--self.RenderableObject:SetScale( Vect3f(2.0) )
	self.RenderableObject:MakeTransform();

	self.Life = 100.0
	self.Room = "sala1"
	
	--Luz de logan para iluminar un poco al player
	--self.Light = light_manager:GetResource( "PlayerLight" );
	--if self.Light == nil then
	--	self.Light = light_manager:CreateLight("omni", "core")
	--	self.Light:SetName("PlayerLight" )
	--	self.Light:SetIntensity( 1 )
	--	self.Light:SetEndRangeAttenuation( 1.0 )
	--	self.Light:SetColor( CColor(0.5, 0.5, 0.5, 1.0 ) )
	--	self.Light:SetPosition( Vect3f(0, 0, 0)	)
	--	self.Light:SetRenderShadows( false )
	--end
	--light_manager:AddResource(self.Light:GetName(), self.Light)
	
	self.Checkpoint = {}
	self.Checkpoint.room = "sala1"
	self.Checkpoint.position = Vect3f( 136.84, -7.99, -62 )
	self.Checkpoint.life = 100.0
	self.Checkpoint.energy = 0.0
	self.Checkpoint.yaw = 0.0 --TODO: Check the initial yaw of the game
	
	self.CanMove = true
	
	self.ArmPosition = Vect3f(0.0)
	
	self.GodMode = false
end

function CPlayer:Update()
	--profiler:AddInit("CPlayer:Update()")
	self.PlayerController:Update()
	
	local l_MeshOffset = self:GetMeshOffset()
	local l_MeshPosition = self.PlayerController:GetPosition()
	--self.Light:SetPosition( l_MeshPosition );
	
	l_MeshPosition = l_MeshPosition + l_MeshOffset
	self.RenderableObject:SetPosition(l_MeshPosition)
	self.RenderableObject:SetYaw(-self.PlayerController:GetYaw() + g_HalfPi)
	self.RenderableObject:SetRoomName("sala1")
	
	self.RenderableObject:SetPitch(camera_manager:GetCurrentCamera():GetPitch())

	self.RenderableObject:MakeTransform();
	self.ArmPosition = self.RenderableObject:GetBonePosition("CATRigRArmPalm")
	if self.CanMove then
		self.Blaster:Update( self.ArmPosition )
	end
	self.StealthAttack:Update( self.ArmPosition )

	if not self.Blaster:GetIsCharging() and not self.Blaster:GetIsShooting() and not self:GetIsCrouch() then
        self:SetAnimation("aim")
	end
	
	if not g_ConsoleActivate and not g_CinematicActive then
		if action_manager:DoAction("ThrowGrenade") and #self.GrenadeQueue > 0 then
		   self.Grenade = table.remove(self.GrenadeQueue, 1)
		   self.Grenade:Throw()
		end
		
		if action_manager:DoAction("ExplodeGrenade") and self.Grenade ~= nil then
			self.Grenade:Explode()
			self.Grenade = nil
		end
		
		if self.InsideBarrel then
			if action_manager:DoAction("EnterBarrel") then
				self:ExitBarrel()
			end
		end
	end
	
	if self.Life <= 0 then
		--FunctionGameOver()
		self:SetPosition(self.Checkpoint.position)
		self.Life= self.Checkpoint.life
		self.Blaster:SetEnergy(self.Checkpoint.energy)
		self:SetYaw(self.Checkpoint.yaw)
		self:SetPitch(0.0)
		enemy_manager:Reinit(self.Checkpoint.room)
	end
	
	if self.Life < 30 then
		self.Life = self.Life + 2.0 * g_FrameTime
	else
		self.Life = self.Life + 0.5 * g_FrameTime
		if self.Life > 100.0 then
			self.Life = 100.0
		end
	end
	
	if self.Life < 25 then
		if not countdowntimer_manager:ExistTimer("LowLifeTimer") then
			countdowntimer_manager:AddTimer("LowLifeTimer", 60.0, false)
			sound_manager:PlayEvent("Play_GP_Poca_Vida", "Logan")
		else
			if not countdowntimer_manager:IsActive("LowLifeTimer") then
				sound_manager:PlayEvent("Play_GP_Poca_Vida", "Logan")
			end
			countdowntimer_manager:SetActive("LowLifeTimer", true)
		end
		if countdowntimer_manager:isTimerFinish("LowLifeTimer") then
			sound_manager:PlayEvent("Play_GP_Poca_Vida", "Logan")
			countdowntimer_manager:Reset("LowLifeTimer", true)
		end
	else
		countdowntimer_manager:Reset("LowLifeTimer", true)
	end
	
	if self.Life < 15 then
		if not self.Pain then
			sound_manager:PlayEvent("Play_Panting_Logan_Loop", "Logan")
			self.Pain = true
		end
	else
		if self.Pain then
			sound_manager:PlayEvent("Stop_Panting_Logan_Loop", "Logan")
			self.Pain = false
		end
	end
	
	if self:GetEnergy() < 25 then
		if g_BrazoOperativo then
			if not countdowntimer_manager:ExistTimer("LowEnergyTimer") then
				countdowntimer_manager:AddTimer("LowEnergyTimer", 60.0, false)
				sound_manager:PlayEvent("Play_GP_Poca_Energia", "Logan")
			else
				if not countdowntimer_manager:IsActive("LowEnergyTimer") then
					sound_manager:PlayEvent("Play_GP_Poca_Energia", "Logan")
				end
				countdowntimer_manager:SetActive("LowEnergyTimer", true)
			end
			if countdowntimer_manager:isTimerFinish("LowEnergyTimer") then
				sound_manager:PlayEvent("Play_GP_Poca_Energia", "Logan")
				countdowntimer_manager:Reset("LowEnergyTimer", true)
			end
		end
	else
		if not countdowntimer_manager:ExistTimer("LowEnergyTimer") then
			countdowntimer_manager:AddTimer("LowEnergyTimer", 60.0, false)
		else
			countdowntimer_manager:Reset("LowEnergyTimer", true)
		end
	end
		
	--profiler:AddEnd("CPlayer:Update()")
end

function CPlayer:SetGodMode(aBool)
	self.GodMode = aBool
end

function CPlayer:SetCheckpoint(aRoom, aPosition, aLife, aEnergy, aYaw)
	self.Checkpoint.room = aRoom
	self.Checkpoint.position = aPosition
	self.Checkpoint.life = aLife
	self.Checkpoint.energy = aEnergy
	self.Checkpoint.yaw = aYaw
end

function CPlayer:SetRoom( aName )
	self.Room = aName
	self.RenderableObject:SetRoomName(aName)
end

function CPlayer:GetRoom()
	return self.Room
end

function CPlayer:SetAnimation( aName )
	self.RenderableObject:SetAnimationState(aName)
end

function CPlayer:ClearAnimation( aName )
	self.RenderableObject:RemoveAction(aName)
end

function CPlayer:GetMeshOffset()
	return self.PlayerController:GetMeshOffset( self.MeshOffset )
end

function CPlayer:SetPosition(position)
	self.PlayerController:SetPosition(position)
end

function CPlayer:AddDamage(amount)
	if not self.GodMode then
		if self.InsideBarrel then
			lBarrel = g_Barrels[self.BarrelName]
			lBarrel:SetIsSafe(false)
			self:ExitBarrel()
		else
			if self.Blaster:GetEnergy() == 0 then
				amount = amount * 2.0
			end
			self.Life = self.Life - amount
		end
	end
end

function CPlayer:GetLife()
	return self.Life
end

function CPlayer:GetBlaster()
	return self.Blaster
end

function CPlayer:GetPosition()
	return self.PlayerController:GetPosition()
end

function CPlayer:GetYaw()
	return self.PlayerController:GetYaw()
end

function CPlayer:SetYaw(aYaw)
	self.PlayerController:SetYaw(aYaw)
end

function CPlayer:SetPitch(aPitch)
	self.PlayerController:SetPitch(aPitch)
end

function CPlayer:GetHeight()
	return self.PlayerController:GetHeight()
end

function CPlayer:GetEnergy()
	return self.Blaster:GetEnergy()
end

function CPlayer:SetEnergy(aAmount)
	return self.Blaster:SetEnergy(aAmount)
end

function CPlayer:AddEnergy(amount)
	local TotalEnergy = self.Blaster:GetEnergy()+amount
	if TotalEnergy > 100.0 then
		TotalEnergy = 100.0
	end
	self.Blaster:SetEnergy(TotalEnergy)
end

function CPlayer:AddLife(aAmount)
	local lTotalLife = self.Life+aAmount
	if lTotalLife > 100.0 then
		lTotalLife = 100.0
	end
	self.Life = lTotalLife
end

function CPlayer:GetRenderableObject()
	return self.RenderableObject;
end

function CPlayer:SetEnergy(amount)
	self.Blaster:SetEnergy(amount)
end

function CPlayer:GetHandPos()
	return self.ArmPosition
end

function CPlayer:HideInBarrel( aName )
	if not self.InsideBarrel then
		------engine:Trace("No estoy oculto")
		local l_Can = true
		if not self.PlayerController.Crouch then
			------engine:Trace("No estoy agachado")
			l_Can = self.PlayerController:MakeCrouch()
		end
		
		if l_Can then
			------engine:Trace("Puedo entrar en el barril")
			scene_renderer_commands_manager:SetVisibleCommand("InsideBarrel", true)
			self.InsideBarrel = true
			self.BarrelName = aName
		end
		return l_Can
	else
		local l_Can = true
		if self.PlayerController.Crouch then
			l_Can = self.PlayerController:MakeCrouch()
		end
		
		if l_Can then
			scene_renderer_commands_manager:SetVisibleCommand("InsideBarrel", false)
			self.InsideBarrel = false
		end
		return l_Can
	end
end

function CPlayer:ExitBarrel()
	------engine:Trace("BarrelName = "..self.BarrelName)
	if self:HideInBarrel( self.BarrelName ) then
		lBarrel = g_Barrels[self.BarrelName]
		lBarrel:ExitBarrel(self:GetPosition())
		self.BarrelName = ""
	end
end

function CPlayer:GetIsHidden()
	if self.InsideBarrel then
		lBarrel = g_Barrels[self.BarrelName]
		if lBarrel:GetIsSafe() then
			return not self.PlayerController:GetIsMoving()
		else
			return false
		end
	else
		return false
	end
end

function CPlayer:GetIsInBarrel()
	return self.InsideBarrel
end

function CPlayer:GetActualBarrel()
	return g_Barrels[self.BarrelName];
end

function CPlayer:GetIsRunning()
	return self.PlayerController:GetIsRunning()
end

function CPlayer:GetIsCrouch()
	return self.PlayerController:GetIsCrouch()
end

function CPlayer:UpdatePlayer(pos, yaw, pitch)
	if not self:GetIsCrouch() then
		pos.y = pos.y - self.PlayerController.Height/2
	else
		pos.y = pos.y - self.PlayerController.Height/4
	end
	self.PlayerController.CharacterController:SetPosition(pos)
	self.RenderableObject:SetPosition(pos)
	self.RenderableObject:SetYaw(-yaw + g_HalfPi)
	
	self.RenderableObject:SetPitch(pitch)
	self.RenderableObject:MakeTransform()
end

function CPlayer:SetCanMove(aBool)
	self.CanMove = aBool
	self.PlayerController:SetCanMove(aBool)
end