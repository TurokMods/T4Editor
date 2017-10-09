#include "Actor.h"
#include "Turok4.h"

namespace opent4
{
    ActorVariables* ActorDef::localVariables() {
        return Actor->GetActorVariables();
    }
    ActorVariables* ActorDef::globalVariables() {
        return Actor->GetATR()->GetActorVariables();
    }
    /* Actor Variables */
    bool ActorVariables::Load(ByteStream *Data)
    {
        Data->SetOffset(0);
        while(!Data->AtEnd(1))
        {
            Block* b = new Block();
            if(!b->Load(Data)) { delete b; return false; }
            m_Blocks.push_back(b);
        }

        ProcessBlocks();
        return true;
    }

    bool ActorVariables::Save(ByteStream *Data)
    {
        return false;
    }

    void ActorVariables::ProcessBlocks()
    {
        for(size_t i = 0; i < m_Blocks.size(); i++)
        {
            Block* b = m_Blocks[i];
            ByteStream* d = b->GetData();
            d->SetOffset(0);
            switch(b->GetType())
            {
                case BT_SPIN_X:
                {
                    Spin.x = *((float*)d->Ptr());
                    break;
                }
                case BT_SPIN_Y:
                {
                    Spin.y = *((float*)d->Ptr());
                    break;
                }
                case BT_SPIN_Z:
                {
                    Spin.z = *((float*)d->Ptr());
                    break;
                }
                default:
                    break;
            }
            if(m_Blocks[i]->GetType() == BT_COUNT)
            {
                // Print error, usupported actor variable
            }
        }
    }

    /* Actor */
    Actor::~Actor()
    {
    }

    std::string Actor::GetFilename() const
    {
        if(!m_File) return "";
        return m_File->m_File;
    }
}
