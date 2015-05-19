#include "Billboard.h"
#include "Texture\Texture.h"
#include "Texture\TextureManager.h"

#include "Cameras\CameraFPShooter.h"
#include "Cameras\CameraManager.h"
#include "GraphicsManager.h"

#include "RenderableVertex\VertexTypes.h"
#include "RenderableVertex\RenderableVertex.h"
#include "RenderableVertex\IndexedVertexs.h"
#include "Effects\EffectManager.h"
#include "EngineManagers.h"
#include "EngineConfig.h"


CRenderableVertexs* CBillboard::sRV = 0;

CBillboard::CBillboard()
    : CName()
    , CObject3D()
    , mSize()
    , m_Active(true)
    , m_Texture(0)
    , mTechnique(0)
{
}

CBillboard::~CBillboard()
{
}

bool CBillboard::Init( const CXMLTreeNode& atts )
{
    bool lOk( true );

    SetName( atts.GetPszProperty("name", "unknown") );

    lOk = lOk && CObject3D::Init(atts);

    mSize = atts.GetVect2fProperty("size", Math::Vect2f(1.0f, 1.0f) );
    m_Active = atts.GetBoolProperty("enabled", false );

    // Get the texture of the billboard
    m_Texture = TextureMInstance->GetTexture( atts.GetPszProperty("texture") );
    lOk = lOk && ( m_Texture != 0 );

    // Get the technique of the billboard
    mTechnique = EffectManagerInstance->GetEffectTechnique( atts.GetPszProperty("technique"));
    lOk = lOk && ( mTechnique != 0 );

    ASSERT(mTechnique, "Null technique %s to render the billboard!", atts.GetPszProperty("technique") )

    return lOk;
}

bool CBillboard::Init
(
    const std::string&  aName,
    const Math::Vect3f& aPosition,
    const Math::Vect2f& aSize,
    const std::string & aTextureName,
    const std::string & aTechniqueName,
    bool                aActive
)
{
    bool lOk( true );

    SetName( aName );

    m_Position = aPosition;
    mSize      = aSize;
    m_Active   = aActive;

    // Get the texture of the billboard
    m_Texture = TextureMInstance->GetTexture( aTextureName );
    lOk = lOk && ( m_Texture != 0 );

    // Get the technique of the billboard
    mTechnique = EffectManagerInstance->GetEffectTechnique( aTechniqueName );
    lOk = lOk && ( mTechnique != 0 );

    return lOk;
}


void CBillboard::Update()
{
	MakeTransform();
}

void CBillboard::Render()
{
    if(m_Active)
    {
        CGraphicsManager* lGM = GraphicsInstance;
        lGM->SetTransform( GetTransform() );
		if( m_Texture )
			m_Texture->Activate(0);
        sRV->Render(lGM, mTechnique);
        lGM->SetTransform( Math::Mat44f() );
    }
}

const Math::Vect2f & CBillboard::GetSize() const
{
    return mSize;
}

const CTexture * CBillboard::GetTexture() const
{
    return m_Texture;
}

void CBillboard::SetTexture(const CTexture* aTexture)
{
    m_Texture = const_cast<CTexture*>(aTexture);
}

void CBillboard::SetSize(const Math::Vect2f & aSize )
{
    mSize = aSize;
}

void CBillboard::CreateBillBoardGeometry()
{
    ASSERT(sRV == 0, "The billboard geometry is already initialized");

    const uint32 lIdxCount = 6;
    const uint32 lVtxCount = 4;

    TT1_VERTEX lVtx[lVtxCount] =
    {
        {  -0.5f, 0.0f, -0.5f, 0, 0 },    // vertex 0
        {  -0.5f, 0.0f,  0.5f, 0, 1 },    // vertex 1
        {   0.5f, 0.0f,  0.5f, 1, 1 },    // vertex 2
        {   0.5f, 0.0f, -0.5f, 1, 0 }     // vertex 3
    };

    unsigned short int lIdx[lIdxCount] = { 0, 1, 2,  2, 3, 0 };

    sRV = new CIndexedVertexs<TT1_VERTEX>(GraphicsInstance, &lVtx, &lIdx, lVtxCount, lIdxCount);
}

void CBillboard::DestroyBillBoardGeometry()
{
    ASSERT(sRV != 0, "The billboard geometry is already destroyed");
    CHECKED_DELETE(sRV);
}
