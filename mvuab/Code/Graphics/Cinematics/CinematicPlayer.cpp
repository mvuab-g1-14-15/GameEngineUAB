#include "CinematicPlayer.h"


#include "Timer\Timer.h"

CCinematicPlayer::CCinematicPlayer() :
    m_Playing(false),
    m_CurrentTime(0.0),
    m_Duration(0.0),
    m_Cycle(false)
{
}

CCinematicPlayer::~CCinematicPlayer()
{
}

void CCinematicPlayer::Init(float Duration)
{
    m_Duration = Duration;
}

void CCinematicPlayer::Update()
{
    if (m_Playing)
    { m_CurrentTime += constFrameTime; }
}

void CCinematicPlayer::Stop()
{
    m_Playing = false;
    m_CurrentTime = 0.0f;
}

void CCinematicPlayer::Play(bool Cycle)
{
    m_Playing = true;
    m_Cycle = Cycle;
}

void CCinematicPlayer::Pause()
{
    m_Playing = false;
}

void CCinematicPlayer::OnRestartCycle()
{
    m_CurrentTime = 0.0f;
}