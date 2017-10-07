#ifndef LOADER_MESH_H
#define LOADER_MESH_H

#include <string>
#include <vector>
#include <stdint.h>

#include "MeshStructs.h"

namespace opent4
{
    class SubMesh;

    class MTFBlock
    {
        public:
            enum BLOCK_TYPE
            {
                BT_FILE,         //ELIF
                BT_MESH,         //HSEM
                BT_TIME,         //EMIT
                BT_TYPE,         //EPYT
                BT_VERSION,      //SREV
                BT_DATE,         //ETAD
                BT_TEXTURE,      //RTXT
                BT_MATERIAL,     //LRTM
                BT_INFO,         //OFNI
                BT_VERTICES,     //STRV
                BT_INDICES,      //XDNI
                BT_NODE,         //EDON
                BT_BOUNDINGBOX,  //XOBB
                BT_SIZE,         //EZIS
                BT_BONE,         //ENOB
                BT_FACE,         //ECAF
                BT_SKELETON,     //LEKS
                BT_SUBMESHNAMES, //MNMS

                /* Unknown */
                BT_GEOM, //Geometry?
                BT_STAT,
                BT_TXST,
                BT_RNST,
                BT_VPSH,
                BT_VSID,
                BT_COLL, //Collision?
                BT_TRIS, //Triangles?
                BT_SUBS,
                BT_AGEO,
                BT_VER_, //Version?
                BT_VECT, //Vector?
                BT_BOUN, //Bounds?
                BT_CENT, //Center?
                BT_SBOU,
                BT_OCNO,
                BT_FALI,
                BT_CHNK, //Chunk?
                BT_CNKS,
                BT_TGET,
                BT_RDH_,
                BT_SUBC,

                BT_COUNT,
            };

            MTFBlock() : m_Data(0) {}
            ~MTFBlock() { if(m_Data) delete[] m_Data; m_Data = 0; }

            bool Load(FILE* fp);
            BLOCK_TYPE GetType() const { return m_Type; }
            std::string GetTypeString() const;

            int m_Unk0;
            int m_Unk1;
            int m_Unk2;
            int m_DataOffset;
            int m_DataSize;

            char* m_Data;

        protected:
            BLOCK_TYPE m_Type;
    };

    class MeshChunk
    {
        public:
            MeshChunk(SubMesh* m) : m_Mesh(m) {}
            ~MeshChunk() {}

            void AddIndex(int16_t i)     { m_Indices.push_back(i);  }
            void Remove  (size_t Idx)    { m_Indices.erase(m_Indices.begin() + Idx); }

            size_t GetIndexCount() const { return m_Indices.size(); }
            int16_t GetIndex(size_t Idx) { return m_Indices[Idx];   }

        protected:
            SubMesh* m_Mesh;
            std::vector<int16_t> m_Indices;
    };

    class SubMesh
    {
        public:
            SubMesh() {}
            ~SubMesh();

            void AddVertex(const MTFVertexTypeA& v) { m_VerticesA.push_back(v); }
            void AddVertex(const MTFVertexTypeB& v) { m_VerticesB.push_back(v); }
            void AddVertex(const ATFVertexTypeA& v) { m_VerticesC.push_back(v); }
            void AddIndex(int16_t i)        { m_Indices.push_back(i);    }
            void AddChunk(MeshChunk* Chunk) { m_Chunks.push_back(Chunk); }

            int GetVertexType() const { return m_vType; }
            size_t GetVertexCount() const;
            void GetVertex  (size_t Idx, float* Ptr) const;
            void GetNormal  (size_t Idx, float* Ptr) const;
            void GetTexCoord(size_t Idx, float* Ptr) const;
            void GetUnk0    (size_t Idx, char*  Ptr) const;
            void GetUnk1    (size_t Idx, float* Ptr) const; //MTF-B, ATF-A only
            void GetUnk2    (size_t Idx, char*  Ptr) const; //ATF-A only

            size_t GetChunkCount() const    { return m_Chunks.size(); }
            MeshChunk* GetChunk(size_t Idx) { return m_Chunks[Idx];   }

            size_t GetIndexCount() const { return m_Indices.size(); }
            int16_t GetIndex(size_t Idx) { return m_Indices[Idx];   }

        protected:
            friend class ActorMesh;
            int m_vType;

            std::vector<MTFVertexTypeA> m_VerticesA;
            std::vector<MTFVertexTypeB> m_VerticesB;
            std::vector<ATFVertexTypeA> m_VerticesC;

            std::vector<int16_t> m_Indices;
            std::vector<MeshChunk*> m_Chunks;
    };

    class ActorMesh
    {
        public:
            ActorMesh() : m_IsAnimated(false) {}
            ~ActorMesh();

            bool Load(const std::string& File);

            size_t GetBlockCount() const { return m_Blocks.size(); }
            MTFBlock* GetBlock(size_t Idx) const { return m_Blocks[Idx]; }

            size_t GetSubMeshCount() const { return m_SubMeshes.size(); }
            SubMesh* GetSubMesh(size_t Idx) const { return m_SubMeshes[Idx]; }

        //protected:
            void ProcessBlock(size_t Idx);
            bool m_IsAnimated;
            std::vector<MTFBlock*> m_Blocks;

            std::vector<std::string> m_SubMeshNames;
            std::vector<std::string> m_Textures;

            std::vector<SubMesh*> m_SubMeshes;
            std::vector<MeshInfo> m_MeshInfos;

            std::vector<MTRL> m_MTRLs;
            std::vector<TSNR> m_TSNRs;
            std::vector<TXST> m_TXSTs;
    };
}

#endif
