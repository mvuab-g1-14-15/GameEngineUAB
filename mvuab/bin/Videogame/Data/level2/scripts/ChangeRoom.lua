g_Rooms = {Vect3f( 141.48, 6.75, -63.59 ), Vect3f( 76.30, -9.66, -34.77 ), Vect3f( 53.820, -16.66, -18.27 ), Vect3f( 40.07, -17.19, 52.25 ), Vect3f( -51.74, 62.45, 70.35 )}g_ActualRoom = 1

function ChangeRoom()
    g_ActualRoom = g_ActualRoom + 1
	if g_ActualRoom > #g_Rooms then
		g_ActualRoom = 1
	end
	sound_manager:PlayEvent("Open_Close_Door_Event","TestGameObject3d")
	return g_Rooms[g_ActualRoom]
end

function Respawn()
	sound_manager:PlayEvent("Open_Close_Door_Event","TestGameObject3d")
	return g_Rooms[g_ActualRoom]
end