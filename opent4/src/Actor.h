#ifndef LOADER_ACTOR_H
#define LOADER_ACTOR_H

#include <string>
#include <vector>
#include <stdint.h>

#include "Mesh.h"
#include "Util.h"

namespace opent4
{
    class ATRFile;
    class ATIFile;
    class Block;
    class Actor;
    class ActorVariables;

    class ActorVec3
    {
        public:
            float x;
            float y;
            float z;
    };

    struct ActorDef
    {
        std::string ActorFile;
        Actor* Actor;
        int BlockIdx;
        ATIFile* Parent;
        
        ActorVariables* localVariables();
        ActorVariables* globalVariables();

        //Actor Props
        int ID;
        int PathID;
        ActorVec3 Position;
        ActorVec3 Rotation;
        ActorVec3 Scale;
        std::string Name;
    };

    class ActorVariables
    {
        public:
            ActorVariables() { }
            ~ActorVariables() {
				for(int i = 0;i < m_Blocks.size();i++) delete m_Blocks[i];
			}

            bool Load(ByteStream* Data);
            bool Save(ByteStream* Data);

            size_t GetBlockCount()   const { return m_Blocks.size(); }
            Block* GetBlock(int Idx) const { return m_Blocks[Idx];   }
			void AddBlock(Block* b);

            ActorVec3 Spin;

        protected:
            void ProcessBlocks();
            std::vector<Block*> m_Blocks;
    };

    class Actor
    {
        public:
            Actor(ATRFile* File = 0) : m_Def(0), m_Variables(0), m_File(File) {}
            ~Actor();

            std::string GetFilename() const;

            ActorDef* GetDef() const { return m_Def; }
            ATRFile* GetATR() const { return m_File; }

            void SetActorVariables(ActorVariables* v) { m_Variables = v; }
            ActorVariables* GetActorVariables() const { return m_Variables; }

        protected:
            friend class ATRFile;
            friend class ATIFile;

            ActorDef* m_Def;
            ActorVariables* m_Variables;
            ATRFile* m_File;
    };
}

#endif
