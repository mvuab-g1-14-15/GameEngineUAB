//----------------------------------------------------------------------------------
// CSlider class
// Author: Enric Vergara
//
// Description:
// A Image..
//----------------------------------------------------------------------------------
#pragma once
#ifndef INC_MAP_H
#define INC_MAP_H

#include "GuiElement\GuiElement.h"
#include "Math\Vector3.h"

//---Forward Declarations---
class CTexture;
//--------------------------

class CMap: public CGuiElement
{
private:
  class CItemMap
  {
  public:
    std::string  m_Name;
    CTexture* m_Texture;
    Math::Vect2f  m_PosInMap;
    uint32  m_Width;
    uint32  m_Height;
    float  m_Yaw;
    std::string  m_PositionScript;
    std::string  m_OrientationScript;
    CItemMap( std::string Name, CTexture* Texture, Math::Vect2f PosInMap, uint32 Width, uint32 Height, float Yaw, std::string  PositionScript,
              std::string  OrientationScript );
  };

  class CEnemyMap
  {
  public:
    std::string  m_Name;
    CTexture* m_Texture;
    Math::Vect2f  m_PosInMap; //Posici�n en coordenadas de textura
    Math::Vect3f  m_PosIn3D; //Posici�n en 3D
    uint32  m_Width;
    uint32  m_Height;
    float  m_Yaw;
    std::string  m_PositionScript;
    std::string  m_OrientationScript;
    CEnemyMap( std::string Name, CTexture* Texture, Math::Vect3f PosIn3D, Math::Vect2f PosInMap, uint32 Width, uint32 Height, float Yaw,
               std::string  PositionScript, std::string  OrientationScript );
  };
  class CPlayer
  {
  public:
    std::string  m_Name;
    CTexture* m_Texture;
    Math::Vect3f m_PosPlayer;
    uint32  m_Width;
    uint32  m_Height;
    float m_Yaw;
    std::string  m_PositionScript;
    std::string  m_OrientationScript;
    CPlayer( std::string Name, CTexture* Texture, Math::Vect3f PosPlayer, uint32 Width, uint32 Height, float Yaw, std::string  PositionScript,
             std::string  OrientationScript );
  };

public:
  CMap( const CXMLTreeNode& aNode, const Math::Vect2i& screenResolution );

  virtual ~CMap();

  //---------------CGuiElement Interface----------------------
  virtual void Render();
  virtual void Update();
  virtual void OnClickedChild( const std::string& name );
  Math::Vect2f NormalizePlayerPos( float x, float z );

  void AddItem( const std::string& Name, const std::string& Texture, Math::Vect3f PosInMap3D, uint32 Width,
                uint32 Height, float Yaw, std::string  PositionScript, std::string  OrientationScript );
  void AddEnemy( CTexture* aTexture, const std::string& Texture, uint32 Width, uint32 Height, float Yaw, std::string PositionScript,
                 std::string OrientationScript );
  void AddEnemys( const std::string& aTexture, uint32 Width, uint32 Height, std::string  PositionScript, std::string  OrientationScript );
  void AddEnemyLUA( const std::string& aName, CTexture* aTexture, Math::Vect3f aPosInMap3d, Math::Vect2f aPosInMap, uint32 aWidth,
                    uint32 aHeight, float aYaw, std::string  aPositionScript, std::string  aOrientationScript );
  void AddPlayer( const std::string& Name, const std::string& Texture, Math::Vect3f PosPlayer, uint32 Width, uint32 Height, float Yaw,
                  std::string PositionScript,
                  std::string  OrientationScript );

  void SetPositionPlayer( Math::Vect3f position );
  void SetYawPlayer( float yaw );
  void SetPositionEnemy( const std::string& Name, Math::Vect3f position );
  void SetYawEnemy( const std::string& Name, float yaw );

private:
  CTexture*  m_Marco;
  CTexture*  m_Mapa; // Textura del mapa/radar del escenario
  CTexture*  m_Cone;
  CPlayer* m_Player; // Textura del player en el mapa
  CTexture*  m_Camara; // Textura del player en el mapa
  Math::Vect2f m_posNPlayer; // Posici�n del Player en el mapa/radar
  Math::Vect2f m_MinMaxMap[2];
  float  m_Width_Map;
  float  m_Height_Map;
  std::vector<CItemMap*>  m_vItems;
  std::vector<CEnemyMap*>  m_vEnemy;
  //Math::Vect2f m_SizeWorld3D; // Zona del mapa que estamos cubriendo
};

#endif //INC_IMAGE_H