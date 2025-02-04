#include "BillboardCore.h"
#include "BillboardInstance.h"
#include "Texture\Texture.h"
#include "Texture\TextureManager.h"
#include "Utils\BaseUtils.h"

#include "GraphicsManager.h"

#include "RenderableVertex\VertexTypes.h"
#include "RenderableVertex\RenderableVertex.h"
#include "RenderableVertex\IndexedVertexs.h"
#include "Effects/EffectManager.h"
#include "EngineManagers.h"
#include "EngineConfig.h"
#include "Timer/Timer.h"

CBillboardCore::CBillboardCore()
    : CName()
    , mTechnique( 0 )
    , mSize(1.0f)
    , mAlpha(1.0f)
    , mFlipUVHorizontal( false )
    , mFlipUVVertical( false )
    , mRandomAngle( false )
    , mDistance( 100 )
    , mAngle( 0.1f )
    , mUseTick( false )
    , mTick( 0.0f )
    , mUseDeltaTime( false )
{
}

CBillboardCore::~CBillboardCore()
{
  Flush();
}

bool CBillboardCore::Init( const CXMLTreeNode& atts )
{
    SetName( atts.GetAttribute<std::string>( "name", "unknown" ) );

    // Get the technique of the BillboardCore
    mTechnique        = atts.GetAttribute<CEffectTechnique>("technique");
    mSize             = atts.GetAttribute<float>( "size"                  , 1.0f  );
    mAngle            = atts.GetAttribute<float>( "angle"                 , 0.1f  );
    mAlpha            = atts.GetAttribute<float>( "alpha"                 , 1.0f  );
    mDistance         = atts.GetAttribute<float>( "distance"                 , 100  );
    mFlipUVHorizontal = atts.GetAttribute<bool> ( "flip_uv_horizontal"    , false );
    mFlipUVVertical   = atts.GetAttribute<bool> ( "flip_uv_vertical"      , false );
    mUseTick          = atts.GetAttribute<bool> ( "use_tick"              , false );
    mUseDeltaTime     = atts.GetAttribute<bool> ( "use_delta_time"        , false );
    mRandomAngle      = atts.GetAttribute<bool> ( "random_angle", false );
    mTick             = float( (rand() % 100 + 1) / 10);

    for( uint32 i = 0, lCount = atts.GetNumChildren(); i<lCount; ++i)
    {
        const CXMLTreeNode& lNode = atts(i);
        STextureStage lTexture = { lNode.GetAttribute<uint32>("stage_id", 0), lNode.GetAttribute<CTexture>("filename") };
        mTextures.push_back(lTexture);
    }

    ASSERT( mTechnique, "Null technique %s to render the BillboardCore %s!", atts.GetAttribute<std::string>( "technique", "null_technique" ), GetName().c_str() );
    return ( mTechnique != 0 );
}

void CBillboardCore::Render( CRenderableVertexs* aRV, CGraphicsManager* aGM, const Math::Vect3f& aCameraPosition )
{
  if ( !mInstances.empty() )
  {
    for( uint32 i = 0, lCount = mTextures.size(); i<lCount; ++i)
    {
        mTextures[i].m_Texture->Activate(mTextures[i].mStage);
    }
    
    CEffect* lEffect = mTechnique->GetEffect();

    for( uint32 i = 0, lCount = mInstances.size(); i < lCount; ++i )
    {
      if( mInstances[i]->GetPosition().Distance( aCameraPosition ) < mDistance )
      {
        CEffect* lEffect = mTechnique->GetEffect();

        lEffect->SetSize( mSize );
        lEffect->SetAlpha( mAlpha );
        lEffect->SetFlipUVs( mFlipUVHorizontal, mFlipUVVertical );

        if( mUseTick || mUseDeltaTime )
          lEffect->SetDeltaTime( mTick );

        mInstances[i]->Render(aRV, aGM, mTechnique );
      }
    }
  }
}

void CBillboardCore::AddInstance( CBillboardInstance* aInstance )
{
  if( aInstance )
  {
    aInstance->SetAngle( ( mRandomAngle  ) ? baseUtils::RandRange( 0.0f, 360.0f ) : mAngle );
    mInstances.push_back( aInstance );
  }
}

void CBillboardCore::Flush()
{
  for( uint32 i = 0, lCount = mInstances.size(); i < lCount; ++i )
  {
    CHECKED_DELETE( mInstances[i] );
  }
  mInstances.clear();
}

void CBillboardCore::Update()
{
    if( mUseTick )
    {
        mTick += constFrameTime;
        if (mTick >= 100.f)
            mTick = 0.f;
    }
    else
    {
        mTick = constFrameTime;
    }
    
}