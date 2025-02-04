#pragma once

#ifndef _TEMPLATED_VECTOR_MAP_MANAGER_H
#define _TEMPLATED_VECTOR_MAP_MANAGER_H

#include <string>
#include <vector>
#include <map>
#include "Utils\Defines.h"

template <class T> class CTemplatedVectorMapManager
{
    public:
        class CMapResourceValue
        {
            public:
                T*     m_Value;
                size_t m_Id;

                CMapResourceValue(T* value, size_t id) : m_Value(value), m_Id(id) { }
                CMapResourceValue() : m_Value(0), m_Id(0) { }
        };

        typedef std::vector<T*>                          TVectorResources;
        typedef std::map<std::string, CMapResourceValue> TMapResources;

    protected:
        TVectorResources    m_ResourcesVector;
        TMapResources       m_ResourcesMap;

    public:
        CTemplatedVectorMapManager() { }
        virtual ~CTemplatedVectorMapManager()
        {
            Destroy();
        }

        bool Exist( const std::string & Name )
        {
            return m_ResourcesMap.find( Name ) != m_ResourcesMap.end();
        }

        void RemoveResource( const std::string& Name )
        {
            TMapResources::iterator it = m_ResourcesMap.find(Name);
            if ( it == m_ResourcesMap.end() ) return;

            CHECKED_DELETE(it->second.m_Value);
            size_t l_ID = it->second.m_Id;

            m_ResourcesMap.erase(it);
            m_ResourcesVector.erase(m_ResourcesVector.begin() + l_ID);

            for (size_t i =  l_ID; i < m_ResourcesVector.size();  ++i)
            {
                T* l_TElement = m_ResourcesVector[i];
                TMapResources::iterator l_ItMap = m_ResourcesMap.begin();

                while (l_ItMap->second.m_Value != l_TElement) ++l_ItMap;
                l_ItMap->second.m_Id = i;
            }
        }

        virtual T* GetResourceById(size_t Id)
        {
            return (m_ResourcesVector.size() > Id) ? m_ResourcesVector[Id] : 0;
        }

        virtual T* GetConstResourceById(size_t Id) const
        {
          return (m_ResourcesVector.size() > Id) ? m_ResourcesVector[Id] : 0;
        }

        virtual T* GetResource(const std::string& Name)
        {
            TMapResources::iterator it = m_ResourcesMap.find(Name );
            return (it != m_ResourcesMap.end()) ? it->second.m_Value : 0;
        }

        virtual T* GetConstResource(const std::string& Name) const
        {
          TMapResources::const_iterator it = m_ResourcesMap.find(Name );
          return (it != m_ResourcesMap.end()) ? it->second.m_Value : 0;
        }

        virtual bool AddResource( const std::string& Name, T* Resource )
        {
            bool lOk = false;
            if (m_ResourcesMap.find(Name) == m_ResourcesMap.end() )
            {
              CMapResourceValue l_Resource(Resource, m_ResourcesVector.size());
              m_ResourcesVector.push_back(Resource);
              m_ResourcesMap[Name] = l_Resource;
              lOk = true;
            }
            else
            {
              LOG_WARNING_APPLICATION("Trying to add twice %s", Name.c_str() );
            }

            return lOk;
        }

        virtual void Destroy()
        {
            for ( size_t i = 0; i < m_ResourcesVector.size() ; ++i ) 
            {
                CHECKED_DELETE( m_ResourcesVector[i] );
            }

            m_ResourcesMap.clear();
            m_ResourcesVector.clear();
        }

        TMapResources &GetResourcesMap()
        {
            return m_ResourcesMap;
        }

        TVectorResources &GetResourcesVector()
        {
            return m_ResourcesVector;
        }

        void Clear()
        {
            m_ResourcesVector.clear();
            m_ResourcesMap.clear();
        }

        const uint32 GetResourcesCount() const
        {
            ASSERT( m_ResourcesMap.size() == m_ResourcesVector.size(), "map size != vector size" );
            return m_ResourcesVector.size();
        }
};

#endif