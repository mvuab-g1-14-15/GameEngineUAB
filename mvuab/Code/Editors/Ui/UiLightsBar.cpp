﻿#include "UiLightsBar.h"
#include "EngineManagers.h"
#include "Lights/LightManager.h"
#include "Lights/Light.h"
#include "Math/Vector3.h"

CUiLightsBar::CUiLightsBar()
    : CUiBar( std::string( "Light Bar") )
{
}

CUiLightsBar::~CUiLightsBar()
{
}

bool CUiLightsBar::Create()
{
    Math::Vect3f& lAmbientLight = LightMInstance->GetAmbientLight();
    AddProperty<float32>("Red", lAmbientLight.x, 0.0f, 1.0f, 0.01f );
    AddProperty<float32>("Green", lAmbientLight.y, 0.0f, 1.0f, 0.01f );
    AddProperty<float32>("Blue", lAmbientLight.z, 0.0f, 1.0f, 0.01f );

    AddProperty<Math::Vect3f>
    (
        "AmbientLight",
        lAmbientLight,
        Math::Vect3f(0.0f, 0.0f, 0.0f),
        Math::Vect3f(1.0f, 1.0f, 1.0f),
        Math::Vect3f(0.01f, 0.01f, 0.01f)
    );

    return true;
}