#include "Mesh.h"

#include <stdio.h>
#include <assert.h>

#include <cstring>
#include <iostream>
#include <algorithm>

#include "Turok4.h"

namespace opent4
{
    /* MTFBLock */
    static std::string MTFBlockTypeIDStrings[MTFBlock::BT_COUNT] =
    {
        /* Knowns */
        "ELIF",
        "HSEM",
        "EMIT",
        "EPYT",
        "SREV",
        "ETAD",
        "RTXT",
        "LRTM",
        "OFNI",
        "STRV",
        "XDNI",
        "EDON",
        "XOBB",
        "EZIS",
        "ENOB",
        "ECAF",
        "LEKS",
        "SMNM",

        /* Unknowns */
        "MOEG",
        "TATS",
        "TSXT",
        "TSNR",
        "HSPV",
        "DISV",
        "LLOC",
        "SIRT",
        "SBUS",
        "OEGA",
        ".reV",
        "tceV",
        "NUOB",
        "TNEC",
        "UOBS",
        "ONCO",
        "ILAF",
        "KNHC",
        "SKNC",
        "TEGT",
        "RDH" ,
        "CBUS",
    };

    bool MTFBlock::Load(FILE* fp)
    {
        char bType[4];
        if(std::fread(bType, 4, 1, fp) != 1) return false;

        std::string TypeString;
        if(bType[0] != 0) TypeString = std::string(bType, 4);
        else TypeString = std::string(&bType[1], 3);
        //std::transform(TypeString.begin(), TypeString.end(), TypeString.begin(), ::toupper)
        

        m_Type = BT_COUNT;
        for(size_t i = 0; i < BT_COUNT; i++)
        {
            if(TypeString == MTFBlockTypeIDStrings[i])
            {
                m_Type = (BLOCK_TYPE)i;
                break;
            }
        }

        if(m_Type == BT_COUNT)
        {
            //std::cout << "Unexpected type string: \"" << TypeString << "\"." << std::endl;

        }

        //TODO: Replace with fread directly into a struct of this data
        if(std::fread(&m_Unk0,       sizeof(int), 1, fp) != 1) return false;
        if(std::fread(&m_Unk1,       sizeof(int), 1, fp) != 1) return false;
        if(std::fread(&m_Unk2,       sizeof(int), 1, fp) != 1) return false;
        if(std::fread(&m_DataOffset, sizeof(int), 1, fp) != 1) return false;
        if(std::fread(&m_DataSize  , sizeof(int), 1, fp) != 1) return false;

        return true;
    }

    std::string MTFBlock::GetTypeString() const
    {
        return MTFBlockTypeIDStrings[m_Type];
    }

    /* Sub Mesh */
    SubMesh::~SubMesh() {
        for(size_t i = 0;i < m_Chunks.size();i++) delete m_Chunks[i];
    }
    
    size_t SubMesh::GetVertexCount() const
    {
        switch(m_vType)
        {
            case  0: return m_VerticesA.size();
            case  3: return m_VerticesB.size();
            case -1: return m_VerticesC.size();
            default: { return 0; }
        }
        return 0;
    }

    void SubMesh::GetVertex(size_t Idx, float* Ptr) const
    {
        switch(m_vType)
        {
            case  0: { std::memcpy(Ptr, &m_VerticesA[Idx].x, sizeof(float) * 3); return; }
            case  3: { std::memcpy(Ptr, &m_VerticesB[Idx].x, sizeof(float) * 3); return; }
            case -1: { std::memcpy(Ptr, &m_VerticesC[Idx].x, sizeof(float) * 3); return; }
            default: { return; }
        }
    }

    void SubMesh::GetNormal(size_t Idx, float* Ptr) const
    {
        switch(m_vType)
        {
            case  0: { std::memcpy(Ptr, &m_VerticesA[Idx].nx, sizeof(float) * 3); return; }
            case  3: { std::memcpy(Ptr, &m_VerticesB[Idx].nx, sizeof(float) * 3); return; }
            case -1: { std::memcpy(Ptr, &m_VerticesC[Idx].nx, sizeof(float) * 3); return; }
            default: { return; }
        }
    }

    void SubMesh::GetTexCoord(size_t Idx, float* Ptr) const
    {
        switch(m_vType)
        {
            case -1: { std::memcpy(Ptr, &m_VerticesC[Idx].u, sizeof(float) * 2); return; }
            case  0: { std::memcpy(Ptr, &m_VerticesA[Idx].u, sizeof(float) * 2); return; }
            case  3: { std::memcpy(Ptr, &m_VerticesB[Idx].u, sizeof(float) * 2); return; }
            default: { return; }
        }
    }

    void SubMesh::GetUnk0(size_t Idx, char * Ptr) const
    {
        switch(m_vType)
        {
            case  0: { std::memcpy(Ptr, &m_VerticesA[Idx].Unk0, 4); return; }
            case  3: { std::memcpy(Ptr, &m_VerticesB[Idx].Unk0, 4); return; }
            case -1: { std::memcpy(Ptr, &m_VerticesC[Idx].Unk0, 6); return; }
            default: { return; }
        }
    }

    void SubMesh::GetUnk1(size_t Idx, float* Ptr) const
    {
        switch(m_vType)
        {
            case  3: { std::memcpy(Ptr, &m_VerticesB[Idx].Unk1, sizeof(float) * 2); return; }
            case -1: { std::memcpy(Ptr, &m_VerticesC[Idx].Unk1, sizeof(float) * 1); return; }
            default: { return; }
        }
    }

    void SubMesh::GetUnk2(size_t Idx, char* Ptr) const
    {
        switch(m_vType)
        {
            case -1: { memcpy(Ptr, &m_VerticesC[Idx].Unk2, 8); return; }
            default: { return; }
        }
    }

    /* ActorMesh */
    ActorMesh::~ActorMesh()
    {
        for(size_t i = 0;i < m_Blocks.size();i++) delete m_Blocks[i];
        for(size_t i = 0;i < m_SubMeshes.size();i++) delete m_SubMeshes[i];

        m_Blocks.clear();
    }

    bool ActorMesh::Load(const std::string& File)
    {
        FILE* fp = std::fopen(File.c_str(), "rb");
        if(!fp) return false;

        size_t extPos = File.find_last_of(".");
        if(extPos != std::string::npos) {
            std::string ext = File.substr(extPos+1);
            if(ext == "atf" || ext == "ATF")
                m_IsAnimated = true;
        }

        int BlockCount;
        if(std::fread(&BlockCount, sizeof(int), 1, fp) != 1) { std::fclose(fp); return false; }

        for(size_t i = 0; i < BlockCount; i++)
        {
            MTFBlock* b = new MTFBlock();
            if(!b->Load(fp)) { delete b; std::fclose(fp); return false; }
            m_Blocks.push_back(b);
        }

        assert(BlockCount == m_Blocks.size());
        for(size_t i = 0; i < m_Blocks.size(); i++)
        {
            if(m_Blocks[i]->m_DataOffset == 0 || m_Blocks[i]->m_DataSize == -1) continue;
            std::fseek(fp, m_Blocks[i]->m_DataOffset, SEEK_SET);

            m_Blocks[i]->m_Data = new char[m_Blocks[i]->m_DataSize];
            if(std::fread(m_Blocks[i]->m_Data, m_Blocks[i]->m_DataSize, 1, fp) != 1)
            {
                delete [] m_Blocks[i]->m_Data;
                m_Blocks[i]->m_Data = 0;
                std::fclose(fp);
                return false;
            }

            if(m_Blocks[i]->GetType() == MTFBlock::BT_SKELETON) m_IsAnimated = true;
        }

        for(size_t i = 0; i < m_Blocks.size(); i++)
            ProcessBlock(i);

        std::fclose(fp);
        return true;
    }

    void ActorMesh::ProcessBlock(size_t Idx)
    {
        MTFBlock* b = m_Blocks[Idx];
        switch(b->GetType())
        {
            case MTFBlock::BT_TIME:
            {
                //printf("Block[%4d]: ",Idx);
                int Tm = *(int*)b->m_Data;
                //printf("Time(?): %d.\n",Tm);
                break;
            }
            case MTFBlock::BT_INFO:
            {
                //printf("Block[%4d]: ",Idx);
                int *Info = (int*)b->m_Data;
                //printf("Info(?): %4d %4d %4d %4d %4d %4d %4d %4d %4d %4d.\n",Info[0] ,Info[1] ,Info[2] ,Info[3] ,Info[4]
                //                                                            ,Info[5] ,Info[6] ,Info[7] ,Info[8] ,Info[9]);

                MeshInfo m;
                std::memcpy(&m,Info,10 * sizeof(int));
                m_MeshInfos.push_back(m);
                break;
            }
            case MTFBlock::BT_SUBMESHNAMES:
            {
                int dPos = 0;
                int NameCount = *(int*)&b->m_Data[dPos]; dPos += sizeof(int);

                for(int i = 0;i < NameCount;i++)
                {
                    int NameLen = *(int*)&b->m_Data[dPos]; dPos += sizeof(int);
                    std::string Name;
                    for(int s = 0;s < NameLen;s++)
                    {
                        Name += b->m_Data[dPos++];
                    }
                    m_SubMeshNames.push_back(Name);
                }

                break;
            }
            case MTFBlock::BT_TYPE:
            {
                //printf("Block[%4d]: ",Idx);
                int Type = *(int*)b->m_Data;
                //printf("Type(?): %d.\n",Type);
                break;
            }
            case MTFBlock::BT_VERSION:
            {
                //printf("Block[%4d]: ",Idx);
                int Version = *(int*)b->m_Data;
                //printf("Version(?): %d.\n",Version);
                break;
            }
            case MTFBlock::BT_DATE:
            {
                //printf("Block[%4d]: ",Idx);
                char* Date = b->m_Data;
                //printf("Creation Date: %s\n",Date);
                break;
            }
            case MTFBlock::BT_TEXTURE:
            {
                //printf("Block[%4d]: ",Idx);
                char* Texture = b->m_Data;

                std::string ActualFilename = TransformPseudoPathToRealPath(std::string(Texture,b->m_DataSize));

                //printf("Texture: %s\n",ActualFilename.c_str());

                m_Textures.push_back(ActualFilename);

                break;
            }
            case MTFBlock::BT_RNST:
            {
                //printf("Block[%4d]: ",Idx);
                int* RNST = (int*)b->m_Data;
                //printf("RNST: %6d %6d %6d %6d %6d %6d\n",RNST[0],RNST[1],RNST[2],RNST[3],RNST[4],RNST[5]);
                if(b->m_DataSize != sizeof(TSNR))
                {
                    //printf("Warning: Data block size != sizeof(TSNR).\n");
                }

                TSNR t;
                std::memcpy(&t,RNST,sizeof(TSNR));
                m_TSNRs.push_back(t);
                break;
            }
            case MTFBlock::BT_TXST:
            {
                //printf("Block[%4d]: ",Idx);
                TXST t;
                std::memcpy(&t,b->m_Data,b->m_DataSize);
                //printf("TXST(?): %4d %4d %4d %4d %f %4d %4d %4d %4d %f %f %4d %4d %4d %4d %4d %f %4d %f %d\n",t.Unk0 ,t.TextureID,t.Unk1 ,t.Unk2
                //                                                                                             ,t.Unk3 ,t.Unk4     ,t.Unk5 ,t.Unk6
                //                                                                                             ,t.Unk7 ,t.Unk8     ,t.Unk9 ,t.Unk10
                //                                                                                             ,t.Unk11,t.Unk12    ,t.Unk13,t.Unk14
                //                                                                                             ,t.Unk15,t.Unk16    ,t.Unk17,t.Unk18);

                m_TXSTs.push_back(t);
            }
            case MTFBlock::BT_SUBS:
            {
                //printf("Block[%4d]: ",Idx);
                int Version = *(int*)b->m_Data;
                //printf("SBUS(?): %d.\n",Version);
                break;
            }
            case MTFBlock::BT_VSID:
            {
                //printf("Block[%4d]: ",Idx);
                int DISV = *(int*)b->m_Data;
                //printf("DISV(?): %d\n",DISV);
                break;
            }
            case MTFBlock::BT_MATERIAL:
            {
                //printf("Block[%4d]: ",Idx);
                if(b->m_DataSize == 20)
                {
                    int Unk0 = *(int*)&b->m_Data[0];
                    float Unk1 = *(float*)&b->m_Data[4];
                    int Unk2 = *(int*)&b->m_Data[8];
                    int Unk3 = *(int*)&b->m_Data[12];
                    int Unk4 = *(int*)&b->m_Data[16];

                    //printf("Material(?): %4d %4.5f %4d %4d %4d.\n",Unk0,Unk1,Unk2,Unk3,Unk4);

                    MTRL m;
                    std::memcpy(&m,b->m_Data,20);
                    m_MTRLs.push_back(m);
                }
                else if(b->m_DataSize == 4)
                {
                    float Unk0 = *(float*)&b->m_Data[0];

                    //printf("Material(?): %f.\n",Unk0);
                }
                //else printf("(Null material)\n");
                break;
            }
            case MTFBlock::BT_BOUNDINGBOX:
            {
                //printf("Block[%4d]: ",Idx);
                float* Bounds = (float*)b->m_Data;
                //printf("Bounding Box: (%4.5f, %4.5f, %4.5f) (%4.5f, %4.5f, %4.5f).\n",Bounds[0],Bounds[1],Bounds[2],
                //                                                                      Bounds[3],Bounds[4],Bounds[5]);
                break;
            }
            case MTFBlock::BT_VERTICES:
            {
                //printf("Block[%4d]: ",Idx);
                SubMesh* m = new SubMesh();
                if(m_IsAnimated) m->m_vType = -1;
                else m->m_vType = m_MeshInfos[m_MeshInfos.size() - 1].Unk1;

                int dPos = 0;
                switch(m->m_vType)
                {
                    case -1:
                    {
                        int vCount = *(int*)b->m_Data; dPos += sizeof(int);
                        //printf("SubMesh (V-Type C): %lu VC: %d.\n",m_SubMeshes.size() + 1,vCount);
                        for(int i = 0;i < vCount;i++)
                        {
                            ATFVertexTypeA v;
                            std::memcpy(&v,&b->m_Data[dPos],sizeof(ATFVertexTypeA));
                            dPos += sizeof(ATFVertexTypeA);

                            //printf("v: %5.5f,%5.5f,%5.5f %5.5f,%5.5f,%5.5f %5.5f,%5.5f\n",v.x,v.y,v.z,v.nx,v.ny,v.nz,v.u,v.v);

                            m->AddVertex(v);
                        }
                        break;
                    }
                    case 0:
                    {
                        int vCount = b->m_DataSize / sizeof(MTFVertexTypeA);
                        //printf("SubMesh (V-Type A): %lu VC: %d.\n",m_SubMeshes.size() + 1,vCount);
                        for(int i = 0;i < vCount;i++)
                        {
                            MTFVertexTypeA v;
                            std::memcpy(&v,&b->m_Data[dPos],sizeof(MTFVertexTypeA));
                            dPos += sizeof(MTFVertexTypeA);

                            //printf("v: %f,%f,%f %f,%f,%f %f,%f\n",v.x,v.y,v.z,v.nx,v.ny,v.nz,v.u,v.v);

                            m->AddVertex(v);
                        }
                        break;
                    }
                    case 3:
                    {
                        int vCount = b->m_DataSize / sizeof(MTFVertexTypeB);
                        //printf("SubMesh (V-Type B): %lu VC: %d.\n",m_SubMeshes.size() + 1,vCount);
                        for(int i = 0;i < vCount;i++)
                        {
                            MTFVertexTypeB v;
                            std::memcpy(&v,&b->m_Data[dPos],sizeof(MTFVertexTypeB));
                            dPos += sizeof(MTFVertexTypeB);

                            //printf("v: %f,%f,%f %f,%f,%f %f,%f\n",v.x,v.y,v.z,v.nx,v.ny,v.nz,v.u,v.v);

                            m->AddVertex(v);
                        }
                        break;
                    }
                    default:
                    {
                        //printf("Encountered unknown vertex type! Study me!\n");
                    }
                }
                m_SubMeshes.push_back(m);
                break;
            }
            case MTFBlock::BT_INDICES:
            {
                //printf("Block[%4d]: ",Idx);
                SubMesh* m = m_SubMeshes[m_SubMeshes.size() - 1];
                int iCount = b->m_DataSize / sizeof(int16_t);
                //printf("SubMesh: %lu IC: %d.\n",m_SubMeshes.size(),iCount);
                int dPos = 0;
                for(int i = 0;i < iCount;i++)
                {
                    int16_t Idx = 0;
                    std::memcpy(&Idx,&b->m_Data[dPos],sizeof(int16_t));
                    dPos += sizeof(int16_t);

                    //printf("i: %d\n",Idx);

                    m->AddIndex(Idx);
                }
                break;
            }
            case MTFBlock::BT_SUBC:
            {
                //printf("Block[%4d]: ",Idx);
                SubMesh* m = m_SubMeshes[m_SubMeshes.size() - 1];
                MeshChunk* Chunk = new MeshChunk(m);

                int dPos = 0;
                dPos += 4; //There always seems to be two null indices at the start of every index list too
                while(dPos != b->m_DataSize)
                {
                    int16_t Idx = 0;
                    std::memcpy(&Idx,&b->m_Data[dPos],sizeof(int16_t));
                    dPos += sizeof(int16_t);
                    if((uint16_t)Idx == 0xFFFF)
                    {
                        //Last two indices always null for some reason
                        Chunk->Remove(Chunk->GetIndexCount() - 1);
                        Chunk->Remove(Chunk->GetIndexCount() - 1);

                        //m->AddChunk(Chunk);

                        //Chunk = new MeshChunk(m);
                        dPos += 2; //There's always a second 0xFFFF
                        dPos += 4; //There always seems to be two null indices at the start of every index list too
                        continue;
                    }

                    Chunk->AddIndex(Idx);
                }
                //Last two indices always null for some reason
                Chunk->Remove(Chunk->GetIndexCount() - 1);
                Chunk->Remove(Chunk->GetIndexCount() - 1);
                m->AddChunk(Chunk);

                //printf("%zu Sections in chunk.\n",m->GetChunkCount());
                break;
            }
            case MTFBlock::BT_FACE:
            {
                //printf("\n");
                break;
            }
            default:
            {
                //printf("Unknown.\n");
                break;
            }

        }
    }
}
