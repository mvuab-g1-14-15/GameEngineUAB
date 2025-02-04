#ifndef _PARTICLE_SYSTEM_MANAGER_H
#define _PARTICLE_SYSTEM_MANAGER_H

#include "XML\XMLTreeNode.h"
#include "ParticleSystemCore.h"
#include "InstanceParticle.h"
#include "Utils\Manager.h"
#include "Utils\TemplatedVectorMapManager.h"

#include <string>
#include <vector>

class CEmitterFactory;
class CParticleSystemCore;
class CParticleInstance;

class CParticleSystemManager: public CManager
{
    public:
        CParticleSystemManager();
        CParticleSystemManager( CXMLTreeNode& atts );
        ~CParticleSystemManager();

        void Init();
        void Update();
        void Render();
        void LoadInstances( const std::string& aFileName, const std::string& aRoomName );
        void ActivateInstancesOfRoom( const std::string& aRoomName );
        void DeactivateInstancesOfRoom( const std::string& aRoomName );
        void Refresh();
        CParticleSystemCore* GetPSCore( const std::string & aCoreName );
        void AddInstance( CParticleInstance* aParticleInstance );
        CParticleInstance* GetInstance( const std::string& aInstanceName, const std::string& aCoreName, const std::string& aRoomName );
    private:
        CEmitterFactory* mEmitterFactory;
        CTemplatedVectorMapManager<CParticleSystemCore> mCores;
        CTemplatedVectorMapManager<CParticleInstance>  mInstances;
        void LoadXML();
};

#endif //_PARTICLE_SYSTEM_MANAGER_H