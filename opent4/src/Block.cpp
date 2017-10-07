#include "Block.h"

namespace opent4
{
    static std::string BlockTypeIDs[BT_COUNT] =
    {
        "ACTOR",
        "PATH",
        "NAVDATA",

        "ID",
        "POS",
        "ROT",
        "SCALE",
        "NAME",
        "ACTOR_VARIABLES",
        "LINK",
        "CAUSE",
        "EVENT",
        "PATHID",
        "ACTOR_CODE",
        "ACTOR_MESH_AXIS",
        "ACTOR_MESH_BOUNDS",
        "ACTOR_PROPERTIES",
        "LINK_LISTS",
        "ACTOR_TEXTURE_SET",
        "GRND_Y",
        "MODES",

        "ACTOR_MESH",
        "ACTORINSTANCES",
        "PRECACHEFILE",

        "AFFECTSTARGET",
        "TIMETOREACHMAXSTRENGTH",
        "TIMETOTURNOFF",
        "HIDE",
        "HOLD",
        "GAGROUPNAME",
        "GAMINNUMBERS",
        "GAGROUPATTACK",
        "SXZANGLE",
        "SOUNDRADIUS",
        "CLOSERANGEDIST",
        "MEDIUMRANGEDIST",
        "AU_A",
        "AU_B",
        "AU_C",
        "AU_D",
        "HEALTH",
        "MAXHEALTH",
        "TURNON",
        "TYPE",
        "COUNTERS",
        "ONLYPTARGET",
        "SPAWNACTOR1CHANCE",
        "CLOSERANGECHANCE",
        "IGNOREPLAYER",
        "PROVOKEONLY",
        "FDMULT",
        "COLLIDES",
        "IGNORES",
        "TOUCHES",
        "LIGHTCOLOR",
        "LIGHTINTENSITY",
        "FRICTION",
        "SPINX",
        "SPINY",
        "SPINZ",
        "A_NDD",
        "A_MDD",
        "A_SON",
        "A_FL",
        "C_B",
        "F_B",
        "F_C",
        "F_F",
        "F_T",
        "F_ED",
        "SMTYPE",
        "LEASHRADIUS",
        "LDSOUND",
        "MDSOUND",
        "HDSOUND",
        "SIGHTRADIUS",
        "SYANGLE",
        "ATTACKRESETTIME",
        "USEHEADTRACKING",
        "INITIALSTATE",
        "TURNINGSPEED",
        "FLAPBEHAVIOR",
        "UPGRADE1SLOT0",
        "UPGRADE2SLOT0",
        "WEAPONSLOT0",
        "WEAPONSLOT1",
        "WEAPONSLOT2",
        "WEAPONSLOT3",
        "WEAPONSLOT4",
        "WEAPONSLOT5",
        "WEAPONSLOT6",
        "WEAPONSLOT7",
        "STARTSOFFWITHWEAPON0",
        "STARTSOFFWITHWEAPON1",
        "STARTSOFFWITHWEAPON2",
        "STARTSOFFWITHWEAPON3",
        "STARTSOFFWITHWEAPON4",
        "STARTSOFFWITHWEAPON5",
        "STARTSOFFWITHWEAPON6",
        "STARTSOFFWITHWEAPON7",
        "ALWAYSRUN",
        "DEFTRANSBLEND",

        "DUMMIES",
        "DEFT",
        "COLS",
        "VERSION",
        "HOTPS",
        "TRNS",
        "LINK",
        "LINKS",
    };

    BLOCK_TYPE GetBlockTypeFromID(const std::string& ID)
    {
        for(size_t i = 0; i < BT_COUNT; i++)
        {
            if(BlockTypeIDs[i] == ID) return (BLOCK_TYPE)i;
        }
        return BT_COUNT;
    }
    Block::~Block() {
        if(m_Data) delete m_Data;
        for(size_t i = 0;i < m_Children.size();i++) {
            delete m_Children[i];
        }
    }
    
    bool Block::Load(ByteStream *Data)
    {
        //Determine whether or not the block's size is a 16 bit integer or an 8 bit integer
        m_PreBlockFlag = Data->GetByte();
        size_t Size = 0;

        switch(m_PreBlockFlag)
        {
            case 0x41:
            case 0x42:
            case 0x43:
            case 0x44:
            case 0x45:
            case 0x46:
            case 0x47:
            case 0x48:
            case 0x4A:
            case 0x4B:
            case 0x4C:
            case 0x4D:
            case 0x4F:
            {
                //Hdr[0] = Unknown (when not part of 2-byte block size)
                //Hdr[1] = Block size in bytes (starts after block ID, including the null)
                //Hdr[2] = Block ID string length

                //Read block header
                Data->GetData(3,m_Hdr);

                //Read block ID string (ACTOR, ID, ACTOR_VARIABLES, EVENTS, etc...)
                m_BlockID = Data->GetString();

                //Establish block size
                Size = (size_t)(unsigned char)m_Hdr[1];
                break;
            }
            case 0x81: { }
            case 0x82:
            {
                //Hdr[0] = Unknown
                //Hdr[1] = First 8 bits of 16 bit integer representing block size
                //Hdr[2] = Last  8 bits of 16 bit integer representing block size
                //Hdr[3] = Block ID string length

                //Read block header
                Data->GetData(4,m_Hdr);

                //Read block ID string (ACTOR, ID, ACTOR_VARIABLES, EVENTS, etc...)
                m_BlockID = Data->GetString();

                //Establish block size
                Size = (size_t)*((uint16_t*)&m_Hdr[1]);
                break;
            }
            case 0x61:
            case 0x6C:
            {
                //Hdr[0] = Unknown
                //Hdr[1] = First 8 bits of 16 bit integer representing block size
                //Hdr[2] = Unknown
                //Hdr[3] = Unknown
                //Hdr[4] = Block ID string length

                //Read block header
                Data->GetData(5,m_Hdr);

                //Read block ID string (ACTOR, ID, ACTOR_VARIABLES, EVENTS, etc...)
                m_BlockID = Data->GetString();

                //Establish block size
                Size = (size_t)(unsigned char)m_Hdr[1];

                break;
            }
            case 0xA1:
            {
                //Hdr[0] = Unknown
                //Hdr[1] = First 8 bits of 16 bit integer representing block size
                //Hdr[2] = Last  8 bits of 16 bit integer representing block size
                //Hdr[3] = Unknown
                //Hdr[4] = Unknown
                //Hdr[5] = Block ID string length

                //Read block header
                Data->GetData(6,m_Hdr);

                //Read block ID string (ACTOR, ID, ACTOR_VARIABLES, EVENTS, etc...)
                m_BlockID = Data->GetString();

                //Establish block size
                Size = (size_t)*((uint16_t*)&m_Hdr[1]);

                break;
            }
            case 0xC2:
            {
                //Hdr[0] = Unknown
                //Hdr[1] = First  8 bits of 32 bit integer representing block size
                //Hdr[2] = Second 8 bits of 32 bit integer representing block size
                //Hdr[3] = Third  8 bits of 32 bit integer representing block size
                //Hdr[4] = Last   8 bits of 32 bit integer representing block size
                //Hdr[5] = Block ID string length

                //Read block header
                Data->GetData(6,m_Hdr);

                //Read block ID string (ACTOR, ID, ACTOR_VARIABLES, EVENTS, etc...)
                m_BlockID = Data->GetString();

                //Establish block size
                Size = (size_t)*((uint32_t*)&m_Hdr[1]);
                break;
            }
            case 0xE1:
            {
                //Hdr[0] = Unknown
                //Hdr[1] = First  8 bits of 32 bit integer representing block size
                //Hdr[2] = Second 8 bits of 32 bit integer representing block size
                //Hdr[3] = Third  8 bits of 32 bit integer representing block size
                //Hdr[4] = Last   8 bits of 32 bit integer representing block size
                //Hdr[5] = Unknown
                //Hdr[6] = Unknown
                //Hdr[7] = Block ID string length

                //Read block header
                Data->GetData(8,m_Hdr);

                //Read block ID string (ACTOR, ID, ACTOR_VARIABLES, EVENTS, etc...)
                m_BlockID = Data->GetString();

                //Establish block size
                Size = (size_t)*((uint32_t*)&m_Hdr[1]);
                break;
            }
            default  :
            {
                printf("Unknown block type 0x%2X.\n",m_PreBlockFlag);
            }
        }

        //printf("0x%2X | %10zu | %16s\n",m_PreBlockFlag,Size,m_BlockID.c_str());
        m_Type = GetBlockTypeFromID(m_BlockID);

        m_Data = Data->SubData(Size);
        if(m_Data) m_Data->SetOffset(0);
        return true;
    }

    bool Block::Save(ByteStream *Data)
    {
        //TODO:
        return false;
    }
}
