class 'CActionLUA'

function CActionLUA:__init( Node )
	self.Function = Node:GetPszProperty( "function", "no_function", false )
	
	engine:Trace("Loaded the action " .. self.Function)
end

function CActionLUA:Execute()
	script_manager:RunCode(self.Function)
end

function CActionLUA:GetLuaFunction()
	return self.Function
end

function CActionLUA:SetLuaFunction( LuaFunction )
	self.Function = LuaFunction
end

function CActionLUA:__tostring()
	return "Action: " .. self.Function
end