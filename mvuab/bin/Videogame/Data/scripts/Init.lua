dofile("./data/scripts/Player.lua")
dofile("./data/scripts/PatrolEnemy.lua")

g_Player = nil
g_HUD = nil
g_ConsoleActivate = false
local initialized = false

function load_basics()
	-- basic loads
end

function load_gameplay()
    g_Player = CPlayer()
	g_HUD = CHUD()
	
	--local pos = Vect3f( 0.0, 0.0, 0.0 )
	local waypoints = {Vect3f(-2.12, 0.4, 5.19), Vect3f(3.30, 0.4, 2.94)}
	--g_Enemy = CPatrolEnemyLUA(waypoints)
	
	--g_stateMachine = CStateMachineLUA("Data/enemies/AI-patroll.xml")
	
	sound_manager:PlayEvent("Play_Long_Ambient", "Ambient" )
	initialized = true
end

function update_gameplay()
	if not initialized then
		load_gameplay()
	end
	
	if( CameraType.Free.value == camera_manager:GetCurrentCamera():GetCameraType() ) then
		UpdateFree()
	else
		g_Player:Update()
	end
	
	g_HUD:Update()
	--g_Enemy:Update()
	
	CheckLevel()
	g_ConsoleActivate = gui_manager:GetConsole():GetVisible()
	if not g_ConsoleActivate then
		if action_manager:DoAction("ChangeRoom") then
			g_Player:SetPosition(ChangeRoom())
		end
	end
	
	if action_manager:DoAction("ChangeCamera" ) then
		if "FreeCam" == camera_manager:GetCurrentCameraName() then
			g_Player:SetPosition(camera_manager:GetCurrentCamera():GetPosition())
			l_Yaw = camera_manager:GetCurrentCamera():GetYaw()
			camera_manager:SetCurrentCamera( "TestProcessCam" )
			camera_manager:GetCurrentCamera():SetYaw(l_Yaw)
		else
			l_Yaw = camera_manager:GetCurrentCamera():GetYaw()
			camera_manager:SetCurrentCamera( "FreeCam" )
			camera_manager:GetCurrentCamera():SetYaw(l_Yaw)
		end
	end
end