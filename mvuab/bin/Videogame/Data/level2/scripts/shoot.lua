class 'CShootLUA'

function CShootLUA:__init(speed, direction, position, damage)
	self.Speed = speed
    self.Damage = damage
    self.Impacted = false
	--direction.y = direction.y - 0.001
    self.Direction = direction
	self.Position = position
	self.Yaw = 0.0
	self.Pitch = 0.0
	self.Name = "Shoot" .. id_manager:GetId("Shoot")
	
    self.Billboard = CreateBillBoard()
	if not billboard_manager:AddResource(self.Name, self.Billboard) then
		self.Billboard = billboard_manager:GetResource(self.Name)
	end
	
	self.Billboard:Init( "ShootBillBoard", self.Position, 1.0, 0.0, 1.0, "Data/textures/particles/fire3.png", "SmokeTechnique", true )
    self.Light = CreateOmniLight()
	self.Light:SetName(self.Name)
    self.Light:SetIntensity( 0.65 )
    self.Light:SetEndRangeAttenuation( 2.0 )
    self.Light:SetColor( CColor(1.0, 0.0, 0.0, 1.0 ) )
    self.Light:SetPosition( self.Position	)
    self.Light:SetRenderShadows( false )
	light_manager:AddResource(self.Name, self.Light)
end

function CShootLUA:Update()
	if not self.Impacted then
		dt = timer:GetElapsedTime()
		lVelocity 		= self.Direction * self.Speed * dt
		lLength 		= lVelocity:Length()
		lNewPosition 	= self.Position + lVelocity
		
		hit_info = physic_manager:RaycastCollisionGroup( self.Position, self.Direction, 0xffffff, 200.0 )
		
		if not (hit_info.Distance == 0.0) then
			lCollisionPoint = Vect3f(hit_info.CollisionPoint)
			lDistance = lCollisionPoint:Distance( lNewPosition )

			if ( lDistance < lLength ) then
				g_Player:AddDamage(5.0)
				
				self.Impacted = true

				self.Position = lCollisionPoint

				self.Billboard:SetActive( false )
			else
				self.Position = lNewPosition
			end
		else
			self.Position = lNewPosition
		end
		self.Billboard:SetPosition( self.Position )
		self.Billboard:MakeTransform()
		self.Light:SetPosition( self.Position )
		self.Light:MakeTransform()
		--MakeTransform();
	end
end

function CShootLUA:GetImpacted()
	return self.Impacted
end

function CShootLUA:Destroy()
	self.Billboard:SetActive( false )
	light_manager:RemoveResource( self.Name )
end