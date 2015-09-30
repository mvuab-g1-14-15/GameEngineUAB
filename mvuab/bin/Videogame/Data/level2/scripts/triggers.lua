g_bChargeEnergy = false
g_bChangeRoom = false
g_bPressX = false
g_bPressedX = false
g_bOpenDoor2 = false
g_bInBarrel = false
CuentaAtras = 3
engine = CEngine.GetSingletonPtr()

function OnEnter()
	process = engine:GetProcess()
	physicUserData = process:GetNewPUD("Box6")
	physicUserData:SetColor(0,1,0,1)
	physicUserData:SetPaint(true)
	process:AddPudVector(physicUserData)
	physicActor = process:GetNewPhysicActor(physicUserData)
	physicActor:AddBoxShape(Vect3f( 1, 1, 1), Vect3f( 5, 8, 5), Vect3f( 0, 0, 0), Vect3f( 0, 0, 0))
	physicActor:CreateBody(1, 0.5, 0.5)
	process:AddPhysicActorVector(physicActor)
	physic_manager:AddPhysicActor(physicActor)
end

function OnLeave(other_shape)
	process = engine:GetProcess()
	process:SetSalir(true)
	physicActor = physic_manager:GetActor(other_shape)
	physicUserData = physicActor:GetUserData()
	physicUserData:SetColor(1,1,0,1)
end

function OnStay(other_shape)
	process = engine:GetProcess()
	physicUserData = process:GetLastPUDInserted()
	physicUserData:SetColor(1,0,0,1)
	
	physicActor2 = physic_manager:GetActor(other_shape)
	physicUserData2 = physicActor2:GetUserData()
	physicUserData2:SetColor(0,1,1,1)
end

function ShowImage(other_shape)
	--cinematic_manager:Execute("cinematica_1")
	if g_bChargeEnergy then
		g_bChargeEnergy = false
	else
		g_bChargeEnergy = true
	end
	gui_manager:ShowStaticText("ButtonEnergy")
end

function ShowImage1(other_shape)
	--cinematic_manager:Execute("cinematica_1")
	if g_bChargeEnergy then
		g_Player:SetWeak(false)
		g_EnemyManager:AlarmRoom("room2")
		g_bChargeEnergy = false
	else
		g_bChargeEnergy = true
	end
	gui_manager:ShowStaticText("ButtonEnergy")
end

function ShowImageRoom(other_shape)
	--cinematic_manager:Execute("cinematica_1")
	if g_bChangeRoom then
		g_bChangeRoom = false
	else
		g_bChangeRoom = true
	end
	gui_manager:ShowStaticText("CambiarSala")
end

function ShowText(text, other_shape)
	if g_bPressX then
		g_bPressX = false
	else
		g_bPressX = true
	end
	gui_manager:ShowStaticText(text)
end

function StayText(room, other_shape)
	local enemigosVivos = GetEnemyLive(room)
	if g_bPressedX and enemigosVivos then
		CuentaAtras = CuentaAtras - timer:GetElapsedTime()
		if CuentaAtras <= 0 then
			gui_manager:ShowStaticText("Block")
			CuentaAtras = 3
			g_bPressedX = false
		end
	elseif g_bPressedX then
		
		gui_manager:ShowStaticText("Block")
		CuentaAtras = 3
		g_bPressedX = false
		g_bPressX = false
		g_bOpenDoor2 = true
		--Codigo para cambiar de sala o abrir la puerta
	end
end

function GetEnemyLive(room)
	lEnemy = g_EnemyManager:GetEnemys()
	engine:Trace("La room es "..room)
	for i in pairs (lEnemy) do
		lActualEnemy = lEnemy[i]
		if lEnemy[i] ~= nil and lActualEnemy:GetRoom() == room then
			return true
		end
	end
	return false	
end

function HiddenBarrelOnEnter(aName, other_shape)
	if not g_bInBarrel then
		gui_manager:ShowStaticText("HideInBarrel")
		g_bInBarrel = true
		g_BarrelName = aName
	end
end

function HiddenBarrelOnLeave(aName, other_shape)
	if g_bInBarrel then
		gui_manager:ShowStaticText("HideInBarrel")
		g_bInBarrel = false
	end
end

function HiddenBarrelExit(aName, other_shape)
	local lBarrel = g_Barrels[aName]
	lBarrel:RestoreBarrel()
end

function HideImage(other_shape)
	gui_manager:ShowImage(false)
end

function ChargeEnergy()
	g_Player:AddEnergy(5)
end

function TracePhysX(message, other_shape)
	--engine:Trace(message)
end