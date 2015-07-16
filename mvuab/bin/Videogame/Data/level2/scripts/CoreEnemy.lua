class 'CCoreEnemyLUA'

function CCoreEnemyLUA:__Init()
	self.EnemyType = "no_type"
	self.Life = 0.0
    self.RespawnTime = 0.0
    self.TimeToShoot = 0.0
    self.ShootAccuracy = 0.0
    self.StateMachineName = "no_name"
    self.StateMachineFileName = "no_name"
end

function CCoreEnemyLUA:SetEnemyType(aEnemyType)
	self.EnemyType = aEnemyType
end

function CCoreEnemyLUA:GetEnemyType()
	return self.EnemyType
end

function CCoreEnemyLUA:SetLife(aLife)
	self.Life = aLife
end

function CCoreEnemyLUA:GetLife()
	return self.Life
end

function CCoreEnemyLUA:SetRespawnTime(aRespawnTime)
	self.RespawnTime = aRespawnTime
end

function CCoreEnemyLUA:GetRespawnTime()
	return self.RespawnTime
end

function CCoreEnemyLUA:SetTimeToShoot(aTimeToShoot)
	self.TimeToShoot = aTimeToShoot
end

function CCoreEnemyLUA:GetTimeToShoot()
	return self.TimeToShoot
end

function CCoreEnemyLUA:SetShootAccuracy(aShootAccuracy)
	self.ShootAccuracy = aShootAccuracy
end

function CCoreEnemyLUA:GetShootAccuracy()
	return self.ShootAccuracy
end

function CCoreEnemyLUA:SetStateMachineName(aStateMachineName)
	self.StateMachineName = aStateMachineName
end

function CCoreEnemyLUA:GetStateMachineName()
	return self.StateMachineName
end

function CCoreEnemyLUA:SetStateMachineFileName(aStateMachineFileName)
	self.StateMachineFileName = aStateMachineFileName
end

function CCoreEnemyLUA:GetStateMachineFileName()
	return self.StateMachineFileName
end