#ifdef __APPLE__

struct MTFVertexTypeA
{
    float x,y,z;
    float nx,ny,nz;
    char Unk0[4];
    float u,v;
} __attribute__((packed));

struct MTFVertexTypeB
{
    float x,y,z;
    float nx,ny,nz;
    char Unk0[4];
    float u,v;
    float Unk1,Unk2;
} __attribute__((packed));

struct ATFVertexTypeA
{
    float x,y,z;
    float nx,ny,nz;
    float u,v;
    char Unk0[6];
    float Unk1; //Usually 1.0f
    char Unk2[8];
} __attribute__((packed));

struct MeshInfo
{
    int TSNR_ID;
    int Unk0;
    int Unk1; //Vertex type possibly
    int Unk2;
    int Unk3;
    int Unk4;
    int Unk5;
    int Unk6;
    int Unk7;
    int Unk8;
} __attribute__((packed));

struct TSNR
{
    int Unk0;
    int TXST_ID;
    int Unk1;
    int Unk2;
    int Unk3;
    int Unk4;
} __attribute__((packed));

struct TXST
{
    int   Unk0;
    int   TextureID;
    int   Unk1;
    int   Unk2;
    float Unk3;
    int   Unk4;
    int   Unk5;
    int   Unk6;
    int   Unk7;
    float Unk8;
    float Unk9;
    int   Unk10;
    int   Unk11;
    int   Unk12;
    int   Unk13;
    int   Unk14;
    float Unk15;
    int   Unk16;
    float Unk17;
    int   Unk18;
} __attribute__((packed));

struct MTRL
{
    int Unk0;
    float Unk1;
    int Unk2;
    int Unk3;
    int Unk4;
} __attribute__((packed));

#else

#pragma pack(push,1)
struct MTFVertexTypeA
{
    float x,y,z;
    float nx,ny,nz;
    char Unk0[4];
    float u,v;
};

struct MTFVertexTypeB
{
    float x,y,z;
    float nx,ny,nz;
    char Unk0[4];
    float u,v;
    float Unk1,Unk2;
};

struct ATFVertexTypeA
{
    float x,y,z;
    float nx,ny,nz;
    float u,v;
    char Unk0[6];
    float Unk1; //Usually 1.0f
    char Unk2[8];
};

struct MeshInfo
{
    int TSNR_ID;
    int Unk0;
    int Unk1; //Vertex type possibly
    int Unk2;
    int Unk3;
    int Unk4;
    int Unk5;
    int Unk6;
    int Unk7;
    int Unk8;
};

struct TSNR
{
    int Unk0;
    int TXST_ID;
    int Unk1;
    int Unk2;
    int Unk3;
    int Unk4;
};

struct TXST
{
    int   Unk0;
    int   TextureID;
    int   Unk1;
    int   Unk2;
    float Unk3;
    int   Unk4;
    int   Unk5;
    int   Unk6;
    int   Unk7;
    float Unk8;
    float Unk9;
    int   Unk10;
    int   Unk11;
    int   Unk12;
    int   Unk13;
    int   Unk14;
    float Unk15;
    int   Unk16;
    float Unk17;
    int   Unk18;
};

struct MTRL
{
    int Unk0;
    float Unk1;
    int Unk2;
    int Unk3;
    int Unk4;
};

#pragma pack(pop)
#endif
