#include "ParticleEmitter.h"
#include "Utils/BaseUtils.h"
#include "Utils/Defines.h"
#include "XML/XMLTreeNode.h"
#include "EngineManagers.h"
#include "Effects/EffectManager.h"

CParticleEmitter::CParticleEmitter()
    : CName()
    , mIsLoop( false )
    , mIsActive( false )
    , mMaxParticles(0)
    , mParticlesCount(0)
    , mTimeToEmit(Math::Vect2f(1.0f, 1.0f))
    , mTimeToLive(Math::Vect2f(1.0f, 1.0f))
    , mActualTime(0.0f)
    , mSize(Math::Vect2f(1.0f, 1.0f))
    , mAcceleration(Math::Vect3f(0.0f, 0.0f, 0.0f))
    , mDirection(Math::Vect3f(0.0f, 0.0f, 0.0f))
    , mPosition(Math::Vect3f(0.0f, 0.0f, 0.0f))
    , mVelocity(Math::Vect3f(0.0f, 0.0f, 0.0f))
    , mTechniqueName( "" )
	, mIsInmortal( 0 )
	, mParticlesXEmission(Math::Vect2f(1.0f, 1.0f))
{

}

CParticleEmitter::~CParticleEmitter()
{
}

bool CParticleEmitter::Init( const CXMLTreeNode& atts )
{
    mIsLoop				= atts.GetBoolProperty("loop", false );
	mIsInmortal			= !mIsLoop;
    mIsActive			= atts.GetBoolProperty("active", false );
    mMaxParticles		= atts.GetIntProperty("max_particles", 0);
    mTimeToEmit			= atts.GetVect2fProperty("time_to_emit", Math::Vect2f(1.0f, 1.0f));
    mTimeToLive			= atts.GetVect2fProperty("time_to_live", Math::Vect2f(1.0f, 1.0f));
    mSize				= atts.GetVect2fProperty("particle_size_range", 0.0f );
    mAcceleration		= atts.GetVect3fProperty("acceleration", Math::Vect3f());
    mDirection			= atts.GetVect3fProperty("direction", Math::Vect3f());
    mPosition			= atts.GetVect3fProperty("position", Math::Vect3f());
    mVelocity			= atts.GetVect3fProperty("velocity", Math::Vect3f());
    mTechniqueName   	= atts.GetPszProperty("technique", "none");
	mParticlesXEmission = atts.GetVect2fProperty("particles_per_emission", Math::Vect2f(1.0f, 1.0f) );

	// Get textures
	for( uint32 i = 0, lCount = atts.GetNumChildren(); i < lCount; ++i )
	{
		const std::string& lTagName = atts(i).GetName();
		if( lTagName == "texture" )
		{
			mTextures.push_back( atts(i).GetPszProperty("name") );
		}
	}

    return true;
}

void CParticleEmitter::Update( float dt )
{
	EmitParticles();
	for( uint32 i = 0, lParticles = GetParticleCount(); i < lParticles; ++i)
    { 
		CParticle* lParticle = GetParticle( i );
		lParticle->Update( dt );
    }
}

void CParticleEmitter::Render()
{
	for( uint32 i = 0, lParticles = GetParticleCount(); i < lParticles; ++i)
    { 
		CParticle* lParticle = GetParticle( i );
		lParticle->Render();
    }
}

void CParticleEmitter::EmitParticles()
{
	const uint32 lParticlesToEmit = (uint32)baseUtils::RandRange( mParticlesXEmission.x, mParticlesXEmission.y );
	for( uint32 i = 0; i < lParticlesToEmit && (GetParticleCount() < mMaxParticles); ++i )
	{
		CParticle* lNewParticle = new CParticle();

		if( lNewParticle->Init
			(	baseUtils::RandRange( mTimeToLive.x, mTimeToLive.y),
				baseUtils::RandRange( Math::Vect3f(0.0f,0.0f,0.0f) , Math::Vect3f(1.0f,1.0f,1.0f) ),
				Math::Vect3f(0.0f,5.0f,0.0f),
				Math::Vect3f(0.0f,1.0f,0.0f), // TODO
				Math::Vect3f(0.0f, 1.0f, 0.0f),
				baseUtils::RandRange( mSize.x, mSize.y ),
				mTextures[ baseUtils::RandRange( (unsigned int)(0), (unsigned int)(mTextures.size()) ) ],
				mTechniqueName
			) 
		   )
		{
			mParticles.push_back( lNewParticle );
		}
		else
		{
			CHECKED_DELETE( lNewParticle );
		}	
	}
}