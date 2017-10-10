#ifndef TUROK_FOUR_H
#define TUROK_FOUR_H

#include <string>
#include <vector>
#include <stdint.h>

#include "Actor.h"
#include "Mesh.h"
#include "Block.h"

namespace opent4
{
    void SetTurokDirectory(const std::string& TurokDir);
    std::string GetTurokDirectory();
    std::string TransformPseudoPathToRealPath(const std::string& PseudoPath);
    std::string TransformRealPathToPseudoPath(const std::string& RealPath  );
    
    class ATRFile;
    class ATIFile
    {
        public:
            ATIFile() {}
            ~ATIFile();

            bool Load(const std::string& File);
            bool Save(const std::string& File);

            size_t GetActorCount() const { return m_Actors.size(); }
            ActorDef* GetActorDef(size_t Idx) const { return m_Actors[Idx]; }
            std::string GetFile() const { return m_File; }

        protected:
            void ProcessBlocks();
            void ProcessActorBlock(size_t Idx);
            bool SaveActorBlock(size_t Idx);
            ATRFile* LoadATR(const std::string& path);

            char m_Hdr[4];
            std::vector<Block*> m_Blocks;
            std::vector<ActorDef*> m_Actors;
            std::vector<ATRFile*> m_LoadedAtrs;
            std::vector<std::string> m_LoadedAtrPaths;
            std::vector<uint32_t> m_LoadedAttrRefs;

            std::string m_File;
    };

    class ATRFile
    {
        public:
            ATRFile() : m_Mesh(0), m_Data(0), m_Root(0), m_Variables(0) {}
            ~ATRFile();

            bool Load(const std::string& Filename);
            bool Save(const std::string& Filename);
			std::string GetFileName() const { return m_RealFile; }

            /* For levels only */
            ATIFile* GetActors() const { if(m_ActorInstanceFiles.size() <= 0) return 0; return m_ActorInstanceFiles[0]; }

            /* All Actors */
            float GetVersion()              const { return m_Version;       }
            ActorVec3 GetMeshAxis()         const { return m_ActorMeshAxis; }
            std::string GetActorCode()      const { return m_ActorCode;     }
            std::string GetActorMeshFile()  const { return m_ActorMeshFile; }
            std::string GetInstancesFile()  const { return m_InstancesFile; }
            std::string GetPrecacheFile()   const { return m_PrecacheFile;  }
            ActorMesh* GetMesh() { return m_Mesh; }
            ActorVariables* GetActorVariables() const { return m_Variables; }
        
            Block* GetRootBlock() const { return m_Root; }


        protected:
            friend class Actor;
            char m_Hdr[4];
            bool CheckHeader();
            void ProcessBlocks();

            float m_Version;
            ActorVec3 m_ActorMeshAxis;
            std::string m_ActorCode;
            std::string m_ActorMeshFile;
            std::string m_InstancesFile;
            std::string m_PrecacheFile;

            ByteStream* m_Data;
            Block* m_Root;
            std::string m_File;
			std::string m_RealFile;
            ActorMesh* m_Mesh;
            ActorVariables* m_Variables;

            /*
             * Only actor .ATR files use these
             */
            ATIFile* m_Parent;

            /*
             * Only level .ATR files use these
             */
            std::vector<ATIFile*> m_ActorInstanceFiles;
    };
}

#endif
