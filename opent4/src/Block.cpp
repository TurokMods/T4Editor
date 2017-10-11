#include "Block.h"
#include <algorithm>

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

    bool Block::Save(ByteStream *Data, bool isRoot)
    {
		//size in header starts from here
		ByteStream* outData = new ByteStream();

		if(isRoot) {
			m_Data->SetOffset(0);
			unsigned char PathLen = m_Data->GetByte();
			if(!outData->WriteByte(PathLen)) {
				delete outData;
				return false;
			}

			std::string file = m_Data->GetString(PathLen);
			if(!outData->WriteString(file)) {
				delete outData;
				return false;
			}
		}
		
		if(m_Children.size() > 0) {
			//branch block
			for(int i = 0;i < m_Children.size();i++) {
				if(!m_Children[i]->Save(outData)) {
					delete outData;
					return false;
				}
			}
		}
        else if(m_Data->GetSize() > 0) {
			//leaf block (single value, group of values [vec2, vec3, etc...], or block of unknown data)
			m_Data->SetOffset(0);
			if(!outData->WriteData(m_Data->GetSize(), m_Data->Ptr())) {
				delete outData;
				return false;
			}
		}
		//ends here
		
		//Always use the 0xC2 header (Take that, Turok4 engine optimizers!)
		//Hdr[0] = Unknown
        //Hdr[1] = First  8 bits of 32 bit integer representing block size
        //Hdr[2] = Second 8 bits of 32 bit integer representing block size
        //Hdr[3] = Third  8 bits of 32 bit integer representing block size
        //Hdr[4] = Last   8 bits of 32 bit integer representing block size
        //Hdr[5] = Block ID string length

        //Write block header
		if(!Data->WriteByte(0xC2)) { delete outData; return false; }									//0xC2 (header id?)
        if(!Data->WriteByte(0)) { delete outData; return false; }										//unknown
		if(!Data->WriteInt32(outData->GetSize())) { delete outData; return false; }						//uint32 block size
		if(!Data->WriteByte(m_BlockID.length())) { delete outData; return false; }						//uint8 block name length

		 //This call includes the null byte after the string (T4 expects this)
        if(!Data->WriteString(m_BlockID)) { delete outData; return false; }								//block name

		if(outData->GetSize() > 0) {
			outData->SetOffset(0); //Write the data from the beginning
			if(!Data->WriteData(outData->GetSize(), outData->Ptr())) { delete outData; return false; }	//block data
		}

		delete outData;

		return true;
    }
	
	void Block::useUIBuf() {
		if(m_Children.size() != 0) {
			printf("You can't do that.\n");
		} else {
			m_useUiBuf = true;
			memset(m_uitextbuf, 0, 1024);
			memcpy(m_uitextbuf, m_Data->Ptr(), m_Data->GetSize() < 1024 ? m_Data->GetSize() : 1024);
		}
	}
}
