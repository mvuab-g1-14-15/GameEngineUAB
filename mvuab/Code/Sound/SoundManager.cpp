#include "SoundManager.h"
#include "Utils\Defines.h"
#include "XML\XMLTreeNode.h"

#include "alc.h"
#include "alut.h"

CSoundManager::CSoundManager()
{
    m_XmlPath = "";
}

CSoundManager::~CSoundManager()
{
    m_XmlPath.clear();
}

bool CSoundManager::LoadSounds(const std::string &l_XmlPath)
{
    m_XmlPath = l_XmlPath;
    
    CXMLTreeNode newFile;
    if(!newFile.LoadFile(l_XmlPath.c_str()))
    {
        LOG_ERROR_APPLICATION("CSoundManager::Load Can't open %s\n", l_XmlPath.c_str());
        newFile.Done();
        return false;
    }

    CXMLTreeNode xmlNodes = newFile["sounds"];
    if(!xmlNodes.Exists())
    {
        LOG_ERROR_APPLICATION("CSoundManager::Load Can't read tag sounds");
        newFile.Done();
        return false;
    }

    for(int i = 0; i < xmlNodes.GetNumChildren(); ++i)
    {
        std::string l_SoundName(xmlNodes(i).GetPszProperty("name", "no_name"));
        std::string l_SoundPath(xmlNodes(i).GetPszProperty("path", "no_path"));
    }

    newFile.Done();
    return true;
}

//-----------GENERAL FUNCTIONS---------------------
void CSoundManager::Update(float delatTime)
{
}

void CSoundManager::Reset()
{
}

void CSoundManager::Pause()
{
}

void CSoundManager::Stop()
{
}

void CSoundManager::SetGain(float gain)
{
}

float CSoundManager::GetGain()
{
    return 0.0f;
}

//---------ACTION FUNCTIONS-------------------------------
bool CSoundManager::PlayAction2D(const std::string& action)
{
    return true;
}

bool CSoundManager::PlayAction3D(const std::string& action, const Math::Vect3f& position)
{
    return true;
}
		
//-----SOURCE FUNCTIONS----------------------------
uint32 CSoundManager::CreateSource()
{
    return 0;
}

bool CSoundManager::DeleteSource(uint32 source)
{
    return true;
}

bool CSoundManager::PlaySource2D(uint32 source, const std::string& action, bool loop)
{
    return true;
}

bool CSoundManager::PlaySource3D(uint32 source, const std::string& action, bool loop)
{
    return true;
}

bool CSoundManager::PauseSource(uint32 source)
{
    return true;
}

bool CSoundManager::StopSource(uint32 source)
{
    return true;
}

bool CSoundManager::SetSourceGain(uint32 source, float inGain)
{
    return true;
}

bool CSoundManager::GetSourceGain(uint32 source, float& outGain)
{
    return true;
}

bool CSoundManager::SetSourcePosition(uint32 source, const Math::Vect3f& inPosition)
{
    return true;
}

bool GetSourcePosition(uint32 source, Math::Vect3f& outPosition)
{
    return true;
}

bool CSoundManager::SetSourceVelocity(uint32 source, const Math::Vect3f& inVelocity)
{
    return true;
}

bool CSoundManager::GetSourceVelocity(uint32 source, Math::Vect3f& outVelocity)
{
    return true;
}	 
		
//-----LISTENER FUNCTIONS-------------------------
void CSoundManager::SetListenerPosition(const Math::Vect3f& inPosition)
{
}

void CSoundManager::GetListenerPosition(Math::Vect3f& outPosition)
{
}

void CSoundManager::SetListenerVelocity(const Math::Vect3f& inVelocity)
{
}

void CSoundManager::GetListenerVelocity(Math::Vect3f& outVelocity)
{
}

void CSoundManager::SetListenerOrientation(const Math::Vect3f& inAt, const Math::Vect3f& inUp)
{
}

void CSoundManager::GetListenerOrientation(Math::Vect3f& outAt, Math::Vect3f& outUp)
{
}

//-----PRIVATE FUNCTIONS-------------------------
bool CSoundManager::_initAL()
{
    ALenum error;
    ALCdevice* pDevice;
    ALCcontext* pContext;
    
    // Get handle to default device.
    pDevice = alcOpenDevice(NULL);
    
    // Get the device specifier.
    //const ALCubyte* deviceSpecifier = alcGetString(pDevice, ALC_DEVICE_SPECIFIER);
    
    // Create audio context.
    pContext = alcCreateContext(pDevice, NULL);
    
    // Set active context.
    alcMakeContextCurrent(pContext);
    
    // Check for an error.
    if ((error = alcGetError(pDevice)) != ALC_NO_ERROR)
    {
        std::string description = "Can't create openAL context (" + _getALErrorString(error) + ")";
        LOG_ERROR_APPLICATION("CSoundManager::_initAL %s", description.c_str());
        return false;
    }
    
    return true;
}

void CSoundManager::_finalizeAL()
{
    _clear();
    
    ALCcontext* pCurContext;
    ALCdevice* pCurDevice;
    
    // Get the current context.
    pCurContext = alcGetCurrentContext();
    
    // Get the device used by that context.
    pCurDevice = alcGetContextsDevice(pCurContext);
    
    // Reset the current context to NULL.
    alcMakeContextCurrent(NULL);
    
    // Release the context and the device.
    alcDestroyContext(pCurContext);
    alcCloseDevice(pCurDevice);
}

bool CSoundManager::_loadSound(const std::string& file, tIdBuffer& buffer)
{
    FILE *fd;

    ALenum format;
    ALenum error;

    ALsizei size;
    ALsizei freq;

    ALvoid* data;
    ALboolean loop;
    
    // Load wav data into buffers.
    alGenBuffers(1, &buffer);
    if((error = alGetError()) != AL_NO_ERROR)
    {    
        alDeleteBuffers(1,&buffer);
        std::string description = "Error: Can't create openAL Buffer (" + _getALErrorString(error)  + ")";
        LOG_ERROR_APPLICATION("CSoundManager::_loadSound %s", description.c_str());

        return false;
    }
    
    // Check if the file exists
    fopen_s(&fd, file.c_str(), "r");
    if (fd == NULL)
    {
        alDeleteBuffers(1, &buffer);
        std::string description = "Error: Can't open file " + file;
        LOG_ERROR_APPLICATION("CSoundManager::_loadSound %s", description.c_str());
        
        return false;
    }
    else
    {
        fclose(fd);
    }

    #pragma warning( disable : 4996 )
    alutLoadWAVFile((ALbyte*)file.c_str(), &format, &data, &size, &freq, &loop);
    alBufferData(buffer, format, data, size, freq);
    alutUnloadWAV(format, data, size, freq);
    #pragma warning( default : 4996 )
    
    if ((error=alGetError()) != AL_NO_ERROR)
    {   	 
        alDeleteBuffers(1, &buffer);
        std::string description = "Error: Can't load sound file " + file + " (" + _getALErrorString(error)  + ")";
        LOG_ERROR_APPLICATION("CSoundManager::_loadSound %s", description.c_str());
        
        return false;
    }   	 
    
    return true;
}

void CSoundManager::_clear()
{
    ALuint aux;    
    std::map<tAction, tIdBuffer>::iterator itBuffer;
    
    for(itBuffer = m_Buffers.begin(); itBuffer != m_Buffers.end(); ++itBuffer)
    {
        aux = (*itBuffer).second;
        alDeleteBuffers (1,&aux);
    }
    
    std::vector<tInfoSource>::iterator itSource;
    for( itSource = m_Sources.begin(); itSource != m_Sources.end(); ++itSource )
    {
        aux = (*itSource).m_uSource;
        alDeleteSources (1,&aux);
    }
    
    m_Buffers.clear();
    m_Sources.clear();
}

int CSoundManager::_getSource(bool reserved)
{
    return 0;
}

void Release()
{
}

std::string	_getALErrorString(ALenum err)
{
    return "";
}

