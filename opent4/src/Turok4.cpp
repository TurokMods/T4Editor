#include "Turok4.h"

#include <assert.h>
#include <string>
#include <cctype>
#include <logger.h>

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
	inline bool fexists(const std::string& name) {
		struct stat buffer;   
		return (stat (name.c_str(), &buffer) == 0); 
	}

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
        if(m_Root) delete m_Root;
        //if(m_Mesh) delete m_Mesh; apparently something else deletes this
        if(m_Variables) delete m_Variables;

		for(size_t i = 0;i < m_ActorInstanceFiles.size();i++) {
			delete m_ActorInstanceFiles[i];
		}
    }

    /* ATRFile */
    bool ATRFile::Load(const std::string& Filename, ATRStorageInterface* atrStorage)
    {
		m_RealFile = Filename;
        printf("ATR: %s\n", Filename.c_str());
        FILE* fp = std::fopen(Filename.c_str(), "rb");
        if(!fp) { printf("Unable to open file.\n"); return false; }

        m_Data = new ByteStream(fp);
        std::fclose(fp);

        if(!CheckHeader()) return false;
        m_Root = new Block();
        m_Root->Load(m_Data);
		m_File = m_Root->getPostHdrString();

		m_Root->GetData()->SetOffset(0);

        ProcessBlocks(atrStorage);
        return true;
    }

    void ATRFile::ProcessBlocks(ATRStorageInterface* atrStorage)
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

                    m_Mesh = new ActorMesh();
                    if(!m_Mesh->Load(TransformPseudoPathToRealPath(m_ActorMeshFile))) {
                        delete m_Mesh;
                    }

                    break;
                }
                case BT_ACTOR_INSTANCES:
                {
                    Data->GetByte(); //Path length (not needed)
                    m_InstancesFile = Data->GetString();

                    ATIFile* ATI = new ATIFile(atrStorage);
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
                {
                    m_Variables = new ActorVariables();
                    if(!m_Variables->Load(Data)) {
                        printf("Failed to load global actor variables for %s\n", m_File.c_str());
                        delete m_Variables;
                        m_Variables = 0;
                    }
                }
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
            Data->SetOffset(0);
        }
    }

	bool ATRFile::SaveBlocks() {
		for(std::size_t i = 0; i < m_Root->GetChildCount(); i++)
        {
            Block* b = m_Root->GetChild(i);
            ByteStream* Data = b->GetData();
            Data->SetOffset(0);
            switch(b->GetType())
            {
                case BT_ACTOR_MESH: { break; } //Can't save this yet
                case BT_ACTOR_INSTANCES: { break; } //Can't save this yet
                case BT_VERSION: { break; } //Probably shouldn't save this...
                case BT_ACTOR_CODE: { break; } //Not sure what this means yet, so don't save it
                case BT_ACTOR_MESH_AXIS:
                {
					//Not sure what this does, but let's save it anyway
					if(!Data->WriteData(4, &m_ActorMeshAxis.x)) return false;
					if(!Data->WriteData(4, &m_ActorMeshAxis.y)) return false;
					if(!Data->WriteData(4, &m_ActorMeshAxis.z)) return false;
                    break;
                }
                case BT_ACTOR_PRECACHE_FILE: { break; } //Not sure what this means yet, so don't save it
                case BT_ACTOR_VARIABLES  :
                {
                    if(!m_Variables->Save(Data)) return false;
					break;
                }
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
		return true;
	}

    bool ATRFile::Save(const std::string& Filename, bool doSaveAti)
    {
		bool doSave = false;

		//back up the file
		if(!fexists(Filename+".editorbak")) {
			FILE* fp = fopen(Filename.c_str(), "rb");
			if(fp) {
				ByteStream* d = new ByteStream(fp);
				d->SetOffset(0);
				fclose(fp);
				fp = fopen((Filename+".editorbak").c_str(), "wb");
				if(fp) {
					if(fwrite(d->Ptr(), d->GetSize() - 1, 1, fp) != 1) {
						printf("Failed to write original data to back up file %s! Not saving changes to this file.\n", (Filename + ".editorbak").c_str());
					} else doSave = true;
					fclose(fp);
				} else printf("Failed to open backup file %s for writing! Not saving changes to this file.\n", (Filename + ".editorbak").c_str());
			} else printf("Failed to back up file %s! Not saving changes to this file.\n", Filename.c_str());
		} else doSave = true;


		if(doSave) {
			SaveBlocks();

			ByteStream* Data = new ByteStream();
			if(!Data->WriteData(4, m_Hdr)) {
				delete Data;
				return false;
			}

			m_Root->GetData()->SetOffset(0);
			if(!m_Root->Save(Data)) {
				delete Data;
				return false;
			}
			Data->SetOffset(0);

			FILE* fp = fopen(Filename.c_str(), "wb");
			if(fp) {
				fwrite(Data->Ptr(), Data->GetSize(), 1, fp);
				fclose(fp);
			} else {
				printf("Failed to modify %s\n", Filename.c_str());
			}
			delete Data;

			if(doSaveAti) {
				for(int i = 0;i < m_ActorInstanceFiles.size();i++) {
					m_ActorInstanceFiles[i]->Save(m_ActorInstanceFiles[i]->GetFile().c_str());
				}
			}
			return true;
		}
		return false;
    }

	bool ATRFile::Restore() {
		printf("Restoring previous %s\n", m_RealFile.c_str());
		FILE* fp = fopen((m_RealFile + ".editorbak").c_str(), "rb");
		if(fp) {
			ByteStream b(fp);
			fclose(fp);

			fp = fopen(m_RealFile.c_str(), "wb");
			if(fp) {
				if(fwrite(b.Ptr(), b.GetSize() - 1, 1, fp) != 1) {
					printf("WARNING: Failed to restore backup file %s.editorbak!\n", m_RealFile.c_str());
					return false;
				}
				fclose(fp);
			} else {
				printf("WARNING: Failed to restore backup file %s.editorbak!\n", m_RealFile.c_str());
				return false;
			}

		} else {
			printf("WARNING: Failed to restore backup file %s.editorbak!\n", m_RealFile.c_str());
			return false;
		}

		for(size_t i = 0;i < m_ActorInstanceFiles.size();i++) m_ActorInstanceFiles[i]->Restore();
		return true;
	}

    bool ATRFile::CheckHeader()
    {
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

	ATRStorageInterface::~ATRStorageInterface() {
		for(int i = 0;i < m_LoadedAtrs.size();i++) {
			delete m_LoadedAtrs[i];
		}
	}

	ATRFile* ATRStorageInterface::LoadATR(const std::string& path, bool doReload) {
        //see if the file was loaded already
        for(size_t i = 0;i < m_LoadedAtrs.size();i++) {
            if(m_LoadedAtrPaths[i] == path) {
				if(!doReload) {
					m_LoadedAttrRefs[i]++;
					return m_LoadedAtrs[i];
				} else {
					m_LoadedAttrRefs[i]--;
					delete m_LoadedAtrs[i];
					m_LoadedAtrs.erase(m_LoadedAtrs.begin() + i);
				}
            }
        }
        
        //nope
        ATRFile* file = new ATRFile();
        if(!file->Load(path, this)) {
            printf("Failed to load an ATR file referenced by the level\n");
            printf("The file was %s\n", path.c_str());
            delete file;
            return 0;
        }

        m_LoadedAtrs.push_back(file);
        m_LoadedAtrPaths.push_back(path);
        m_LoadedAttrRefs.push_back(1);
        return file;
	}

    /* ATIFile */
    ATIFile::~ATIFile()
    {
        for(size_t i = 0; i < m_Blocks.size(); i++) delete m_Blocks[i];
        for(size_t i = 0; i < m_Actors.size(); i++)
        {
            if(m_Actors[i]->Actor->GetActorVariables()) delete m_Actors[i]->Actor->GetActorVariables();
            delete m_Actors[i];
        }
    }

    bool ATIFile::Load(const std::string& File)
    {
        printf("ATI: %s\n", File.c_str());
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
		bool doSave = false;

		//back up the file
		if(!fexists(File+".editorbak")) {
			FILE* fp = fopen(File.c_str(), "rb");
			if(fp) {
				ByteStream* d = new ByteStream(fp);
				d->SetOffset(0);
				fclose(fp);
				fp = fopen((File+".editorbak").c_str(), "wb");
				if(fp) {
					if(fwrite(d->Ptr(), d->GetSize() - 1, 1, fp) != 1) {
						printf("Failed to write original data to back up file %s! Not saving changes to this file.\n", (File + ".editorbak").c_str());
					} else doSave = true;
					fclose(fp);
				} else printf("Failed to open backup file %s for writing! Not saving changes to this file.\n", (File + ".editorbak").c_str());
			} else printf("Failed to back up file %s! Not saving changes to this file.\n", File.c_str());
		} else doSave = true;

		if(doSave) {
			ByteStream* out = new ByteStream();
			out->WriteData(4, m_Hdr);
			for(size_t i = 0;i < m_Blocks.size();i++) {
				switch(m_Blocks[i]->GetType())
				{
					case BT_ACTOR           : { if(!SaveActorBlock(i)) { return false; } break; }
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

				if(!m_Blocks[i]->Save(out)) {
					printf("Failed to save block %d\n", i);
				}
			}
		
			out->SetOffset(0);
			FILE* fp = fopen(File.c_str(), "wb");
			fwrite(out->Ptr(), out->GetSize(), 1, fp);
			fclose(fp);
			delete out;

			for(size_t i = 0;i < m_LoadedAtrs.size();i++) {
				m_LoadedAtrs[i]->Save(m_LoadedAtrPaths[i]);
			}
			return true;
		}
        return false;
    }

	bool ATIFile::Restore() {
		printf("Restoring previous %s\n", m_File.c_str());
		FILE* fp = fopen((m_File + ".editorbak").c_str(), "rb");
		if(fp) {
			ByteStream b(fp);
			fclose(fp);

			fp = fopen(m_File.c_str(), "wb");
			if(fp) {
				if(fwrite(b.Ptr(), b.GetSize() - 1, 1, fp) != 1) {
					printf("WARNING: Failed to restore backup file %s.editorbak!\n", m_File.c_str());
					return false;
				}
				fclose(fp);
			} else {
				printf("WARNING: Failed to restore backup file %s.editorbak!\n", m_File.c_str());
				return false;
			}

		} else {
			printf("WARNING: Failed to restore backup file %s.editorbak!\n", m_File.c_str());
			return false;
		}

		for(size_t i = 0;i < m_LoadedAtrs.size();i++) m_LoadedAtrs[i]->Restore();
		return true;
	}

	ActorDef* ATIFile::DuplicateActor(ActorDef* newActor) {
		if(!newActor) return 0;
		printf("Finding actor instance block for actor with ID %d...\n", newActor->ID);
		//Find corresponding actor block
		Block* b = 0;
		for(size_t i = 0;i < m_Blocks.size();i++) {
			if(m_Blocks[i]->GetTypeString() == "ACTOR") {
				//The first child block always seems to be the ID, this is just to be safe
				for(size_t cb = 0;cb < m_Blocks[i]->GetChildCount();cb++) {
					if(m_Blocks[i]->GetChild(cb)->GetTypeString() == "ID") {
						unsigned int actor_id = 0;
						ByteStream* data = m_Blocks[i]->GetChild(cb)->GetData();
						size_t offset = data->GetOffset();
						data->SetOffset(0);
						if(data->GetSize() == 1) actor_id = (unsigned char)data->GetByte();
						else if(data->GetSize() == 2) actor_id = (unsigned short)data->GetInt16();
						else if(data->GetSize() == 4) actor_id = (unsigned int)data->GetInt32();
						data->SetOffset(0);

						if(actor_id == newActor->ID) {
							printf("Found actor instance block at position %d\n", i);
							printf("Saving modifications to instance block %d\n", i);
							SaveActorBlock(i);
							b = m_Blocks[i];
							break;
						}
					}
				}
				if(b) break;
			}
		}
		
		if(!b) {
			printf("Actor instance block to duplicate not found?\n");
			return nullptr;
		}

		printf("Cloning source actor instance block\n");
		Block* clone = new Block(*b);

		//Get next actor ID, insert index in the block list
		unsigned short id = GetNextActorID();
		auto process_idx = GetLastActorBlock();
		if(process_idx != m_Blocks.end()) process_idx++;
		printf("New actor ID: %d\n", id);

		//Set the actor ID
		for(size_t cb = 0;cb < clone->GetChildCount();cb++) {
			if(clone->GetChild(cb)->GetTypeString() == "ID") {
				Block* idblock = clone->GetChild(cb);
				ByteStream* data = idblock->GetData();
				data->SetOffset(0);
				if(id <= UINT8_MAX) data->WriteByte(id);
				else {
					if(id >= 384) idblock->setFlag(0x46);
					data->WriteInt16(id);
				}
				data->SetOffset(0);
				break;
			}
		}


		printf("Serializing actor instance block data...\n");
		clone->Save(0);

		printf("Inserting cloned actor instance block at the end of the instance block list\n");
		process_idx = m_Blocks.insert(process_idx, clone);

		printf("Processing cloned actor instance block...\n");
		return ProcessActorBlock(std::distance(m_Blocks.begin(), process_idx));
	}

	ActorDef* ATIFile::InstantiateActor(ATRFile* atr) {
		printf("Creating actor instance block...\n");
		Block* atiBlock = new Block(BT_ACTOR);

		//write actor path
		printf("Writing path to .atr file\n");
		atiBlock->setPostHdrString(atr->GetTurokFileName());

		//Get next actor ID, insert index in the block list
		unsigned short id = GetNextActorID();
		auto process_idx = GetLastActorBlock();
		if(process_idx != m_Blocks.end()) process_idx++;
		
		//Set actor ID
		printf("Creating actor instance ID block with ID: %d\n", id);
		Block* actorId = new Block(BT_ACTOR_ID);
		ByteStream* data = actorId->GetData();
		data->SetOffset(0);
		if(id < UINT8_MAX) data->WriteByte((unsigned char)id);
		else {
			if(id >= 384) actorId->setFlag(0x46);
			data->WriteInt16(id);
		}
		data->SetOffset(0);
		atiBlock->AddChildBlock(actorId);

		//default to just the file name with no path and no extension, should be fine
		string name = atr->GetFileName();
		name = name.substr(name.find_last_of('/') + 1);
		name = name.substr(0, name.find_last_of('.'));
		
		//Set actor name
		printf("Creating actor instance NAME block with name: %s\n", name.c_str());
		Block* actorName = new Block(BT_ACTOR_NAME);
		actorName->GetData()->WriteString(name);
		actorName->GetData()->SetOffset(0);
		atiBlock->AddChildBlock(actorName);
		
		//Set actor position
		printf("Creating actor instance POS block with position 0, 0, 0\n");
		Block* actorPos = new Block(BT_ACTOR_POSITION);
		actorPos->GetData()->WriteFloat(0.0f);
		actorPos->GetData()->WriteFloat(0.0f);
		actorPos->GetData()->WriteFloat(0.0f);
		actorPos->GetData()->SetOffset(0);
		atiBlock->AddChildBlock(actorPos);
		
		//Set actor position
		printf("Creating actor instance ROT block with rotation 0, 0, 0\n");
		Block* actorRot = new Block(BT_ACTOR_ROTATION);
		actorRot->GetData()->WriteFloat(0.0f);
		actorRot->GetData()->WriteFloat(0.0f);
		actorRot->GetData()->WriteFloat(0.0f);
		actorRot->GetData()->SetOffset(0);
		atiBlock->AddChildBlock(actorRot);
		
		//Set actor position
		printf("Creating actor instance SCALE block with scale 1, 1, 1\n");
		Block* actorScl = new Block(BT_ACTOR_SCALE);
		actorScl->GetData()->WriteFloat(1.0f);
		actorScl->GetData()->WriteFloat(1.0f);
		actorScl->GetData()->WriteFloat(1.0f);
		actorScl->GetData()->SetOffset(0);
		atiBlock->AddChildBlock(actorScl);

		//Set actor variables
		printf("Creating actor instance ACTOR_VARIABLES block\n");
		Block* actorVariables = new Block(BT_ACTOR_VARIABLES);
		//this can be empty, they can add variables with the editor
		atiBlock->AddChildBlock(actorVariables);

		printf("Serializing actor instance block data...\n");
		atiBlock->Save(0);
		
		printf("Inserting actor instance block at the end of the instance block list\n");
		process_idx = m_Blocks.insert(process_idx, atiBlock);

		printf("Processing new actor instance block...\n");
		return ProcessActorBlock(std::distance(m_Blocks.begin(), process_idx));
	}
	
	void ATIFile::DeleteActor(size_t block_idx) {
		bool foundActor = false;
        for(size_t i = 0; i < m_Actors.size(); i++)
        {
			if(m_Actors[i]->BlockIdx == block_idx) {
				foundActor = true;
				if(m_Actors[i]->Actor->GetActorVariables()) delete m_Actors[i]->Actor->GetActorVariables();
				delete m_Actors[i];
				m_Actors.erase(m_Actors.begin() + i);
			}
			if(i != m_Actors.size()) m_Actors[i]->BlockIdx = i;
        }
		if(!foundActor) {
			printf("Warning: could not find actor def to delete!\n");
			return;
		}
		delete m_Blocks[block_idx];
		m_Blocks.erase(m_Blocks.begin() + block_idx);
	}

	std::vector<Block*>::iterator ATIFile::GetLastActorBlock() {
		for(size_t i = 0;i < m_Blocks.size();i++) {
			if(m_Blocks[i]->GetType() != BT_ACTOR) return m_Blocks.begin() + (i - 1);
		}
		return m_Blocks.end();
	}
	
	unsigned short ATIFile::GetNextActorID() {
		unsigned short id = 0;

		for(auto b = m_Blocks.begin();b != m_Blocks.end();b++) {
			if((*b)->GetTypeString() == "ACTOR") {
				for(size_t cb = 0;cb < (*b)->GetChildCount();cb++) {
					if((*b)->GetChild(cb)->GetTypeString() == "ID") {
						ByteStream* data = (*b)->GetChild(cb)->GetData();
						size_t offset = data->GetOffset();
						data->SetOffset(0);
						unsigned short c_id = 0;
						if(data->GetSize() == 1) c_id = (unsigned char)data->GetByte();
						else if(data->GetSize() == 2) c_id = (unsigned short)data->GetInt16();
						data->SetOffset(0);
						
						if(c_id > id) id = c_id;

						break;
					}
				}
			}
		}

		return id + 1;
	}

    void ATIFile::ProcessBlocks()
    {
        for(size_t i = 0; i < m_Blocks.size(); i++)
        {
            switch(m_Blocks[i]->GetType())
            {
                case BT_ACTOR           : { ProcessActorBlock(i); break; }
                //case BT_PATH            : { break; }
                //case BT_NAVDATA         : { break; }
                //case BT_ACTOR_VARIABLES : { break; }
                //case BT_VERSION         : { break; }
                default:
                {
                    //printf("Unsupported ATI block type (%s).\n",BlockTypeIDs[m_Blocks[i]->GetType()].c_str());
                    break;
                }
            }
        }
    }

    ActorDef* ATIFile::ProcessActorBlock(size_t Idx)
    {        
        Block* b = m_Blocks[Idx];
        ATRFile* atr = LoadATR(TransformPseudoPathToRealPath(b->getPostHdrString()));
        if(!atr) return nullptr;

        ActorDef* d = new ActorDef();
        d->ActorFile = b->getPostHdrString();
        d->BlockIdx  = Idx ;
        d->Parent    = this;
        d->PathID = d->ID = -1;
        d->Actor = new Actor(atr);

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
                    std::string name = Data->GetString();
					memset(d->Name, 0, 255);
					snprintf(d->Name, 255, "%s", name.c_str());
                    break;
                }
                case BT_ACTOR_ID      :
                {
                    if(Data->GetSize() == 1) d->ID = (unsigned char)Data->GetByte();
					else if(Data->GetSize() == 2) d->ID = (unsigned short)Data->GetInt16();
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
                    d->Actor->SetActorVariables(v);
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

			Data->SetOffset(0);
        }

        m_Actors.push_back(d);
        d->Actor->m_Def = d;
		return d;
    }

    bool ATIFile::SaveActorBlock(size_t Idx)
    {
        Block* b = m_Blocks[Idx];
		ActorDef* d = m_Actors[Idx];

		//We don't need to re-save this value, but we need to navigate to the end of this data for the next part
        b->GetData()->SetOffset(1);
        b->GetData()->GetString();

		//update block data
        for(int i = 0;i < b->GetChildCount();i++)
        {
            Block* cBlock = b->GetChild(i);
            ByteStream* Data = cBlock->GetData();
			Data->SetOffset(0);

            switch(cBlock->GetType())
            {
                case BT_ACTOR_POSITION:
                {
					if(!Data->WriteFloat(d->Position.x)) return false;
					if(!Data->WriteFloat(d->Position.y)) return false;
					if(!Data->WriteFloat(d->Position.z)) return false;
                    break;
                }
                case BT_ACTOR_ROTATION:
                {
					if(!Data->WriteFloat(d->Rotation.x)) return false;
					if(!Data->WriteFloat(d->Rotation.y)) return false;
					if(!Data->WriteFloat(d->Rotation.z)) return false;
                    break;
                }
                case BT_ACTOR_SCALE   :
                {
					if(!Data->WriteFloat(d->Scale.x)) return false;
					if(!Data->WriteFloat(d->Scale.y)) return false;
					if(!Data->WriteFloat(d->Scale.z)) return false;
                    break;
                }
                case BT_ACTOR_NAME    :
                {
					if(!Data->WriteString(d->Name)) return false;
                    break;
                }
                case BT_ACTOR_ID      :
                {
                    if(Data->GetSize() == 1) Data->WriteByte(d->ID);
					else if(Data->GetSize() == 2) Data->WriteInt16(d->ID);
                    break;
                }
                case BT_ACTOR_PATH_ID :
                {
					if(!Data->WriteByte(d->PathID)) return false;
                    break;
                }
                case BT_ACTOR_VARIABLES:
                {
					cBlock->DeleteChildren();
					Data->Clear();
					ActorVariables* local = d->localVariables();
					for(int v = 0;v < local->GetBlockCount();v++) cBlock->AddChildBlock(new Block(*local->GetBlock(v)));
					if(!local->Save(Data)) return false;
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

			Data->SetOffset(0);
        }

		b->GetData()->SetOffset(0);
		return true;
    }
    
    ATRFile* ATIFile::LoadATR(const std::string &path) {
		ATRFile* file = m_atrStorage->LoadATR(path);
		if(!file) return nullptr;

		for(size_t i = 0;i < m_LoadedAtrPaths.size();i++) {
			if(m_LoadedAtrPaths[i] == path) return file;
		}

        m_LoadedAtrs.push_back(file);
        m_LoadedAtrPaths.push_back(path);
        return file;
    }
}
