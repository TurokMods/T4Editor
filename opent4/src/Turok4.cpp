#include "Turok4.h"

#include <assert.h>

#if !defined(_WIN32)
#include <stdlib.h>
#include <string.h>

#include <dirent.h>
#include <errno.h>
#include <unistd.h>

// r must have strlen(path) + 2 bytes
static int casepath(char const *path, char *r)
{
    size_t l = strlen(path);
    char *p = (char*)alloca(l + 1);
    strcpy(p, path);
    size_t rl = 0;

    DIR *d;
    if (p[0] == '/')
    {
        d = opendir("/");
        p = p + 1;
    }
    else
    {
        d = opendir(".");
        r[0] = '.';
        r[1] = 0;
        rl = 1;
    }

    int last = 0;
    char *c = strsep(&p, "/");
    while (c)
    {
        if (!d)
        {
            return 0;
        }

        if (last)
        {
            closedir(d);
            return 0;
        }

        r[rl] = '/';
        rl += 1;
        r[rl] = 0;

        struct dirent *e = readdir(d);
        while (e)
        {
            if (strcasecmp(c, e->d_name) == 0)
            {
                strcpy(r + rl, e->d_name);
                rl += strlen(e->d_name);

                closedir(d);
                d = opendir(r);

                break;
            }

            e = readdir(d);
        }

        if (!e)
        {
            strcpy(r + rl, c);
            rl += strlen(c);
            last = 1;
        }

        c = strsep(&p, "/");
    }

    if (d) closedir(d);
    return 1;
}

bool stringLowerEquals(std::string a, std::string b)
{
    unsigned int sz = a.length();
    if (b.length() != sz)
        return false;
    for (unsigned int i = 0; i < sz; ++i)
        if (std::tolower(a[i]) != std::tolower(b[i]))
            return false;
    return true;
}

std::string casename(std::string dir, std::string filename)
{
    DIR *d;
    struct dirent *dirp;
    if((d = opendir(dir.c_str())) == NULL) {
        printf("Failed opening dir for case transforming: %s\n", dir.c_str());
        return dir+filename;
    }
    while((dirp = readdir(d)) != NULL) {
        std::string name = std::string(dirp->d_name);
        if(stringLowerEquals(filename, name)) {
            closedir(d);
            return dir + name;
        }
    }
    closedir(d);
    printf("Warning: Did not find filename %s in directory %s\n", filename.c_str(), dir.c_str());
    return dir + filename;
}

#endif

namespace opent4
{
    std::string _tDir;
    void SetTurokDirectory(const std::string& TurokDir)
    {
        //Insure there is no trailing slash in the turok dir
        _tDir = TurokDir;
        while(_tDir.rbegin() != _tDir.rend() && *_tDir.rbegin() == '/')
            _tDir.pop_back();

    }

    std::string GetTurokDirectory()
    {
        return _tDir;
    }

    std::string TransformPseudoPathToRealPath(const std::string& PseudoPath)
    {
        std::string RealPath;

        //Some hacky stuff to resolve various bugs with file paths
        if(PseudoPath[0] == 'Y' && PseudoPath[1] == ':') RealPath = PseudoPath.substr(2, PseudoPath.length() - 1);
        else if(PseudoPath[0] == '/' && PseudoPath[1] == '\\') RealPath = PseudoPath.substr(1, PseudoPath.length() - 1);
        else RealPath = PseudoPath;
        
        RealPath = GetTurokDirectory() + RealPath;

        for(int s = 0; s < RealPath.length(); s++) {
            if(RealPath[s] == '\\')
                RealPath[s] = '/';
            else
                RealPath[s] = std::tolower(RealPath[s]);
        }

        //printf("PseudoPath: %s\n", PseudoPath.c_str());
        //printf("Realpath: %s\n", RealPath.c_str());

        size_t extIdx = RealPath.find_last_of(".");
        if(extIdx != std::string::npos)
        {
            std::string Ext = RealPath.substr(extIdx, RealPath.length());
            if(Ext == ".bmp" || Ext == ".tga")
                RealPath = RealPath.substr(0, extIdx) + ".dds";
        }

#if !defined(_WIN32)
        //On a case sensitive file system os, make sure we have proper case path
        char *r = (char*)alloca(RealPath.length() + 2);
        if (casepath(RealPath.c_str(), r))
        {
            RealPath = std::string(r);
        }

        size_t fileIdx = RealPath.find_last_of("/");
        assert(fileIdx != std::string::npos);
        std::string path = RealPath.substr(0, fileIdx+1);
        std::string filename = RealPath.substr(fileIdx+1, RealPath.length());
        //printf("transforming path %s %s \n", path.c_str(), filename.c_str());
        RealPath = casename(path, filename);
#endif

        return RealPath;
    }

    std::string TransformRealPathToPseudoPath(const std::string& RealPath)
    {
        return "Not needed yet.";
    }
    
    ATRFile::~ATRFile() {
        if(m_Data) delete m_Data;
        if(m_Actor) delete m_Actor;
        if(m_Root) delete m_Root;
    }

    /* ATRFile */
    bool ATRFile::Load(const std::string& Filename)
    {
        FILE* fp = std::fopen(Filename.c_str(), "rb");
        if(!fp) { printf("Unable to open file.\n"); return false; }

        m_Data = new ByteStream(fp);
        std::fclose(fp);

        m_Actor = new Actor(this);
        if(!CheckHeader()) return false;
        m_Root = new Block();
        m_Root->Load(m_Data);

        unsigned char PathLen = m_Root->GetData()->GetByte();
        m_File = m_Root->GetData()->GetString(PathLen);
        m_Root->GetData()->Offset(1);

        while(!m_Root->GetData()->AtEnd(1))
        {
            Block* b = new Block();
            b->Load(m_Root->GetData());
            m_Root->AddChildBlock(b);
        }

        ProcessBlocks();
        return true;
    }

    void ATRFile::ProcessBlocks()
    {
        for(std::size_t i = 0; i < m_Root->GetChildCount(); i++)
        {
            Block* b = m_Root->GetChild(i);
            ByteStream* Data = b->GetData();
            Data->SetOffset(0);
            switch(b->GetType())
            {
                case BT_ACTOR_MESH:
                {
                    Data->GetByte(); //Path length (not needed)
                    m_ActorMeshFile = Data->GetString();

                    ActorMesh* m = new ActorMesh();
                    m->Load(TransformPseudoPathToRealPath(m_ActorMeshFile));
                    m_Actor->m_Mesh = m;

                    break;
                }
                case BT_ACTOR_INSTANCES:
                {
                    Data->GetByte(); //Path length (not needed)
                    m_InstancesFile = Data->GetString();

                    ATIFile* ATI = new ATIFile();
                    std::string path = TransformPseudoPathToRealPath(m_InstancesFile);
                    if(!ATI->Load(path)) break;
                    m_ActorInstanceFiles.push_back(ATI);

                    break;
                }
                case BT_VERSION:
                {
                    m_Version = Data->GetFloat();
                    break;
                }
                case BT_ACTOR_CODE:
                {
                    m_ActorCode = Data->GetString();
                    break;
                }
                case BT_ACTOR_MESH_AXIS:
                {
                    m_ActorMeshAxis.x = Data->GetFloat();
                    m_ActorMeshAxis.y = Data->GetFloat();
                    m_ActorMeshAxis.z = Data->GetFloat();
                    break;
                }
                case BT_ACTOR_PRECACHE_FILE:
                {
                    Data->GetByte(); //Path length (not needed)
                    m_PrecacheFile = Data->GetString();
                    //printf("Precache files not yet understood.\n");

                    break;
                }
                case BT_ACTOR_VARIABLES  :
                case BT_ACTOR_MESH_BOUNDS:
                case BT_ACTOR_TEXTURE_SET:
                case BT_ACTOR_PROPERTIES :
                case BT_ACTOR_POSITION   :
                case BT_ACTOR_ROTATION   :
                case BT_ACTOR_SCALE      :
                case BT_DUMMIES          :
                case BT_GRND_Y           :
                case BT_DEFT             :
                case BT_COLS             :
                case BT_MODES            :
                case BT_LINK_LISTS       :
                case BT_LINK             :
                case BT_LINKS            :
                case BT_TRNS             :
                case BT_HOTPS            : { break; }
                default:
                {
                    //printf("Unsupported ATR block type (%s).\n",BlockTypeIDs[b->GetType()].c_str());
                    break;
                }
            }
        }
    }

    bool ATRFile::Save(const std::string& Filename)
    {
        ByteStream* Data = new ByteStream();
        Data->WriteData(4, m_Hdr);
        m_Root->Save(Data);
        return true;
    }

    bool ATRFile::CheckHeader()
    {
        char m_Hdr[4];
        if(!m_Data->GetData(4, m_Hdr))
        {
            printf("Unable to read header. (Empty file?)\n");
                        return false;
        }

        if(m_Hdr[1] != 'a'
        || m_Hdr[2] != 't'
        || m_Hdr[3] != 'r')
        {
            printf("Invalid ATR header.\n");
            return false;
        }

        return true;
    }

    /* ATIFile */
    ATIFile::~ATIFile()
    {
        for(size_t i = 0; i < m_Blocks.size(); i++) delete m_Blocks[i];
        for(size_t i = 0; i < m_Actors.size(); i++)
        {
            if(m_Actors[i]->Actor->GetActor()->GetActorVariables()) delete m_Actors[i]->Actor->GetActor()->GetActorVariables();
            if(m_Actors[i]->Mesh) delete m_Actors[i]->Mesh;
            delete m_Actors[i]->Actor;
            delete m_Actors[i];
        }
    }

    bool ATIFile::Load(const std::string& File)
    {
        printf("Ati loading: %s\n", File.c_str());
        FILE* fp = std::fopen(File.c_str(), "rb");
        if(!fp) { printf("Unable to open file at path: %s\n", File.c_str()); return false; }
        ByteStream* Data = new ByteStream(fp);
        std::fclose(fp);

        if(!Data->GetData(4,m_Hdr)) { printf("Unable to read header.\n"); delete Data; return false; }

        if(m_Hdr[1] != 'a'
        || m_Hdr[2] != 't'
        || m_Hdr[3] != 'i')
        {
            printf("Incorrect header, not actor instances file.\n");
            delete Data;
            return false;
        }

        while(!Data->AtEnd(1))
        {
            Block* b = new Block();
            b->Load(Data);
            m_Blocks.push_back(b);
        }

        ProcessBlocks();

        delete Data;
        m_File = File;
        return true;
    }

    bool ATIFile::Save(const std::string& File)
    {
        //TODO:
        return false;
    }

    void ATIFile::ProcessBlocks()
    {
        for(size_t i = 0; i < m_Blocks.size(); i++)
        {
            switch(m_Blocks[i]->GetType())
            {
                case BT_ACTOR           : { ProcessActorBlock(i); break; }
                case BT_PATH            : { break; }
                case BT_NAVDATA         : { break; }
                case BT_ACTOR_VARIABLES : { break; }
                case BT_VERSION         : { break; }
                default:
                {
                    //printf("Unsupported ATI block type (%s).\n",BlockTypeIDs[m_Blocks[i]->GetType()].c_str());
                    break;
                }
            }
        }
    }

    void ATIFile::ProcessActorBlock(size_t Idx)
    {
        Block* b = m_Blocks[Idx];
        b->GetData()->SetOffset(0);
        unsigned char PathLen = b->GetData()->GetByte();
        std::string Path = b->GetData()->GetString(PathLen);
        b->GetData()->Offset(1);

        while(!b->GetData()->AtEnd(1))
        {
            Block* aBlock = new Block();
            aBlock->Load(b->GetData());
            b->AddChildBlock(aBlock);
        }

        ActorDef* d = new ActorDef();
        d->ActorFile = Path;
        d->BlockIdx  = Idx ;
        d->Parent    = this;
        d->PathID = d->ID = -1;
        d->Actor = new ATRFile();
        if(!d->Actor->Load(TransformPseudoPathToRealPath(Path)))
        {
            delete d->Actor;
            return;
        }

        for(int i = 0;i < b->GetChildCount();i++)
        {
            Block* cBlock = b->GetChild(i);
            ByteStream* Data = cBlock->GetData();

            switch(cBlock->GetType())
            {
                case BT_ACTOR_POSITION:
                {
                    d->Position.x = Data->GetFloat();
                    d->Position.y = Data->GetFloat();
                    d->Position.z = Data->GetFloat();
                    break;
                }
                case BT_ACTOR_ROTATION:
                {
                    d->Rotation.x = Data->GetFloat();
                    d->Rotation.y = Data->GetFloat();
                    d->Rotation.z = Data->GetFloat();
                    break;
                }
                case BT_ACTOR_SCALE   :
                {
                    d->Scale.x = Data->GetFloat();
                    d->Scale.y = Data->GetFloat();
                    d->Scale.z = Data->GetFloat();
                    break;
                }
                case BT_ACTOR_NAME    :
                {
                    d->Name = Data->GetString();
                    break;
                }
                case BT_ACTOR_ID      :
                {
                    d->ID = (unsigned char)Data->GetByte();
                    break;
                }
                case BT_ACTOR_PATH_ID :
                {
                    d->PathID = (unsigned char)Data->GetByte();
                    break;
                }
                case BT_ACTOR_VARIABLES:
                {
                    ActorVariables* v = new ActorVariables();
                    if(!v->Load(Data)) { printf("Unable to load actor variables.\n"); }
                    d->Actor->GetActor()->SetActorVariables(v);
                    break;
                }
                case BT_ACTOR_LINK:
                {
                    break;
                }
                default:
                {
                    //printf("Unsupported actor block type (%s).\n",BlockTypeIDs[cBlock->GetType()].c_str());
                }
            }
        }

        m_Actors.push_back(d);
        d->Actor->GetActor()->m_Def = d;
    }

    void ATIFile::SaveActorBlock(size_t Idx)
    {
        //TODO:
    }
}
