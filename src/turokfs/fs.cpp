#include <turokfs/fs.h>
#include <turokfs/entry.h>
#include <unordered_map>
#include <Turok4.h>
#include <logger.h>
using namespace opent4;

#include <render/SOIL/SOIL.h>
#include <stdio.h>

#include <algorithm>
#include <sstream>
using namespace std;

#include <lodepng.h>

namespace t4editor {
	string convertTexture(const string& filename, const string& outDir) {
		i32 w, h, ch;
		unsigned char* Data = SOIL_load_image(filename.c_str(), &w, &h, &ch, 4);
		if (Data) {
			LodePNGColorType outType;
			switch(ch) {
				case 3: outType = LCT_RGB; break;
				case 4: outType = LCT_RGBA; break;
				default:
					printf("Interesting... %d channels?\n", ch);
					break;
			}
			lodepng::encode((outDir + "/" + filename.substr(filename.find_last_of("/")) + ".png").c_str(), Data, w ,h, outType);

			SOIL_free_image_data(Data);
			return filename.substr(filename.find_last_of("/") + 1) + ".png";
		}
		return "";
	}
	string convert_mesh(SubMesh* m) {
		return "";
	}
	string fmt(const char* fmt, ...) {
		va_list p;
		va_start(p, fmt);
		char out[512];
		memset(out, 0, 512);
		vsnprintf(out,512,fmt,p);
		va_end(p);
		return string(out);
	}
	void save_as_obj(ATRFile* atr, const string& file, const string& outDir) {
		ActorMesh* mesh = atr->GetMesh();
		if(mesh) {
			string out;
			string mtl_out;

			int v_off = 0;

            for(size_t i = 0;i < mesh->GetSubMeshCount();i++) {
				string tex_file = "";
				if (i < mesh->m_MeshInfos.size()) {
					MeshInfo minfo = mesh->m_MeshInfos[i];
					if(minfo.TSNR_ID != -1)
					{
						int TexID = mesh->m_TXSTs[mesh->m_TSNRs[minfo.TSNR_ID].TXST_ID].TextureID;
						if(TexID < mesh->m_Textures.size()) tex_file = convertTexture(mesh->m_Textures[TexID], outDir);
					}
				}

                SubMesh* sm = mesh->GetSubMesh(i);
				string sm_name = fmt("submesh_%d", i);
				//out += "o " + sm_name + "\n";

				for(int v = 0;v < sm->GetVertexCount();v++) {
					float pos[3];
					sm->GetVertex(v, pos);
					out += fmt("v %f %f %f\n",pos[0], pos[1], pos[2]);
				}
				out += "\n";

				for(int v = 0;v < sm->GetVertexCount();v++) {
					float tex[2];
					sm->GetTexCoord(v, tex);
					out += fmt("vt %f %f\n",tex[0], 1.0f - tex[1]);
				}
				out += "\n";

				for(int v = 0;v < sm->GetVertexCount();v++) {
					float nrm[3];
					sm->GetNormal(v, nrm);
					out += fmt("vn %f %f %f\n",nrm[0], nrm[1], nrm[2]);
				}
				out += "\n";

				if(sm->GetIndexCount() > 0) {
					out += "g " + sm_name + "\n";
					if(tex_file.length() > 0) {
						out += fmt("usemtl %s\ns 1\n", sm_name.c_str());
						mtl_out += fmt("newmtl %s\nKd 1.0000 1.0000 1.0000\nmap_Kd %s\n\n", sm_name.c_str(), tex_file.c_str());
					}

					for(int t = 0;t < sm->GetIndexCount() - 2;t++) {
						if(t % 2 == 0) {
							int a = sm->GetIndex(t + 0) + 1 + v_off;
							int b = sm->GetIndex(t + 1) + 1 + v_off;
							int c = sm->GetIndex(t + 2) + 1 + v_off;
							out += fmt("f %d/%d/%d %d/%d/%d %d/%d/%d\n",a,a,a,b,b,b,c,c,c);
						} else {
							int a = sm->GetIndex(t + 0) + 1 + v_off;
							int c = sm->GetIndex(t + 1) + 1 + v_off;
							int b = sm->GetIndex(t + 2) + 1 + v_off;
							out += fmt("f %d/%d/%d %d/%d/%d %d/%d/%d\n",a,a,a,b,b,b,c,c,c);
						}
					}
					out += "\n";
				}

				v_off += sm->GetVertexCount();

				for(size_t ch = 0;ch < sm->GetChunkCount();ch++) {
					string sm_tex_file = "";
					if(ch < mesh->m_MTRLs.size())
                    {
                        if(mesh->m_MTRLs[ch].Unk4 >= 0 && mesh->m_MTRLs[ch].Unk4 < mesh->m_TSNRs.size())
                        {
                            int TexID = mesh->m_TXSTs[mesh->m_TSNRs[mesh->m_MTRLs[ch].Unk4].TXST_ID].TextureID;
                            if(TexID < mesh->m_Textures.size()) sm_tex_file = convertTexture(mesh->m_Textures[TexID], outDir);
                        }
                    }

					if(sm->GetChunk(ch)->GetIndexCount() > 0) {
						out += fmt("g %s_chunk_%d\n", sm_name.c_str(), ch);
						if(sm_tex_file.length() > 0) {
							out += fmt("usemtl %s_chunk_%d\ns 1\n", sm_name.c_str(), ch);
							mtl_out += fmt("newmtl %s_chunk_%d\nKd 1.0000 1.0000 1.0000\nmap_Kd %s\n\n", sm_name.c_str(), ch, sm_tex_file.c_str());
						} else if(tex_file.length() > 0) {
							out += fmt("usemtl %s_chunk_%d\ns 1\n", sm_name.c_str(), ch);
							mtl_out += fmt("newmtl %s_chunk_%d\nKd 1.0000 1.0000 1.0000\nmap_Kd %s\n\n", sm_name.c_str(), ch, tex_file.c_str());
						}

						for(int t = 0;t < sm->GetChunk(ch)->GetIndexCount() - 2;t++) {
							if(t % 2 == 0) {
								int a = sm->GetChunk(ch)->GetIndex(t + 0) + 1;
								int b = sm->GetChunk(ch)->GetIndex(t + 1) + 1;
								int c = sm->GetChunk(ch)->GetIndex(t + 2) + 1;
								out += fmt("f %d/%d/%d %d/%d/%d %d/%d/%d\n",a,a,a,b,b,b,c,c,c);
							} else {
								int a = sm->GetChunk(ch)->GetIndex(t + 0) + 1;
								int c = sm->GetChunk(ch)->GetIndex(t + 1) + 1; //<--
								int b = sm->GetChunk(ch)->GetIndex(t + 2) + 1; //<--
								out += fmt("f %d/%d/%d %d/%d/%d %d/%d/%d\n",a,a,a,b,b,b,c,c,c);
							}
						}
						out += "\n";
					}
				}
			}

			if(out.length() > 0) {
				FILE* fp = fopen(file.c_str(), "w");
				if(fp) {
					string name = file.substr(file.find_last_of("/") + 1, file.find_last_of(".") - file.find_last_of("/") - 1);
					if(mtl_out.length() > 0) {
						fprintf(fp, "mtllib %s\n", (name + ".mtl").c_str());
					}
					fwrite(out.c_str(), out.length(), 1, fp);
					fclose(fp);

					if(mtl_out.length() > 0) {
						fp = fopen((outDir + "/" + name + ".mtl").c_str(),"w");
						if(fp) {
							fwrite(mtl_out.c_str(), mtl_out.length(), 1, fp);
							fclose(fp);
						}
					}
				}
			}
        }
	}
	void construct_db(Db* db, turokfs* fs);

	int get_block_id(int mesh_id, const string& type, i32 idx, Db* db) {
		Statement s = db->Query(fmt("SELECT id FROM tblMTFBlock WHERE mesh_id = %d AND type = '%s' LIMIT %d, 1", mesh_id, type.c_str(), idx).c_str());
		if(s.Next()) {
			return s.GetField(0);
		} else printf("Failed to get block id!\n");
		return -1;
	}

    turokfs::turokfs(const string& dir) {
        m_fsys = new FileSystem();
        m_fsys->SetWorkingDirectory(dir);
        
        printf("Filesystem initialized. Parsing game assets...\n");
        
        recursiveParseLevels("data/levels");
        if(m_levels.size() > 0) printf("Found %lu levels\n", m_levels.size());
        recursiveParseActors("data/actors");
        if(m_actors.size() > 0) printf("Found %lu actors\n", m_actors.size());

		m_atrStorage = new ATRStorageInterface();

		//m_db = new Db((dir + "/T4.db").c_str());
		//construct_db(m_db, this);

		//recursiveParseAll("data");
		
		//system("mkdir \"C:/Users/Computer/Desktop/t4_convert\"");

		/*
		for(size_t i = 0;i < m_actors.size();i++) {
			ATRStorageInterface atr_store;
			ATRFile* atr = atr_store.LoadATR(m_actors[i]->path());
			if(!atr) continue;
			
			string path = m_actors[i]->path();
			path = path.substr(0, path.find_last_of("/"));
			//system((string("mkdir \"C:/Users/Computer/Desktop/t4_convert/") + path + "\"").c_str());
			//save_as_obj(atr, "C:/Users/Computer/Desktop/t4_convert/" + path + "/" + m_actors[i]->name() + ".obj", "C:/Users/Computer/Desktop/t4_convert/" + path);

			store_in_db(atr, m_db);
		}

		for(size_t i = 0;i < m_levels.size();i++) {
			ATRStorageInterface atr_store;
			ATRFile* atr = atr_store.LoadATR(m_levels[i]->path());
			if(!atr) continue;
			
			string path = m_levels[i]->path();
			path = path.substr(0, path.find_last_of("/"));
			//system((string("mkdir \"C:/Users/Computer/Desktop/t4_convert/") + path + "\"").c_str());
			//save_as_obj(atr, "C:/Users/Computer/Desktop/t4_convert/" + path + "/" + m_levels[i]->name() + ".obj", "C:/Users/Computer/Desktop/t4_convert/" + path);
			
			store_in_db(atr, m_db);
		}
		*/
		m_isProcessing = false;
    }
    
    turokfs::~turokfs() {
        delete m_fsys;
		delete m_atrStorage;
		delete m_db;
    }

	void turokfs::update_actor_cache(const string& file) {
		if(m_isProcessing) return;

		m_Mutex.lock();
		m_Progress = 0.0f;
		m_lastUsedProgress = 0.0f;
		m_isProcessing = true;
		m_Mutex.unlock();

		float load_weight = 0.95f;
		float save_weight = 0.04f;

		int actor_count = 0;
		int actor_page = 1;

		unordered_map <string, vector<pair<bool, string> > > category_actors;
		for(size_t i = 0;i < m_actors.size();i++) {
			ATRFile* atr = LoadATR(m_actors[i]->path());
			if(!atr) continue;
			
			string category = atr->GetActorCode();
			//There is over 1000 in the "Actor" category. It needs to be broken up, this should be enough
			if(category == "Actor") {
				ostringstream o;
				o << " [page: " << actor_page << "]";
				category += o.str();
				actor_count++;
				if(actor_count % 70 == 0) actor_page++;
			}

			if(category_actors.find(category) == category_actors.end()) category_actors[category] = vector<pair<bool, string> >();
			category_actors[category].push_back(pair<bool, string>(atr->GetMesh() != nullptr, m_actors[i]->path()));

			m_Mutex.lock();
			m_Progress = (float(i) / float(m_actors.size())) * load_weight;
			m_LastFileProcessed = atr->GetFileName();
			m_LastFileProcessed.substr(m_LastFileProcessed.find_last_of('/'));
			m_Mutex.unlock();
		}

		ByteStream* output = new ByteStream();
		int aid = 0;
		for(auto i = category_actors.begin();i != category_actors.end();i++) {
			output->WriteString(i->first); //category (actor code)
			output->WriteInt32(i->second.size()); //number of actors in category
			vector<pair<bool, string> > actors = i->second;
			for(auto p = actors.begin();p != actors.end();p++) {
				output->WriteString(p->second); //path to file (turok's fake relative path from Y:\)
				output->WriteByte(p->first); //is_renderable

				m_Mutex.lock();
				m_Progress = load_weight + ((float(aid) / float(m_actors.size())) * save_weight);
				m_Mutex.unlock();
				m_LastFileProcessed = p->second;
				aid++;
			}
		}
		
		printf("Saving actor cache to file: %s\n", file.c_str());
		FILE* fp = fopen(file.c_str(), "wb");
		output->SetOffset(0);
		fwrite(output->Ptr(), output->GetSize(), 1, fp);
		fclose(fp);
		delete output;

		m_Mutex.lock();
		m_Progress = 1.0f;
		m_isProcessing = false;
		m_Mutex.unlock();
	}

	float turokfs::get_cache_progress(bool& is_complete, string* last_file) {
		if(m_Mutex.try_lock()) {
			m_lastUsedProgress = m_Progress;
			if(last_file) *last_file = m_LastFileProcessed;
			is_complete = !m_isProcessing;
			m_Mutex.unlock();
		} else is_complete = false;

		return m_lastUsedProgress;
	}

	void turokfs::restore_backup() {
		if(m_isProcessing) return;
		vector<string> backupFiles;
		recursiveParseBackups("data", backupFiles);

		m_Mutex.lock();
		m_Progress = 0.0f;
		m_lastUsedProgress = 0.0f;
		m_isProcessing = true;
		m_Mutex.unlock();

		unordered_map <string, vector<pair<bool, string> > > category_actors;
		for(size_t i = 0;i < backupFiles.size();i++) {
			FILE* fp = fopen(backupFiles[i].c_str(), "rb");
			if(fp) {
				ByteStream b(fp);
				fclose(fp);

				fp = fopen(backupFiles[i].substr(0, backupFiles[i].find_last_of(".editorbak") - 9).c_str(), "wb");
				if(fp) {
					if(fwrite(b.Ptr(), b.GetSize() - 1, 1, fp) != 1) printf("WARNING: Failed to restore backup file %s!\n", backupFiles[i].c_str());
					fclose(fp);
				} else printf("WARNING: Failed to restore backup file %s!\n", backupFiles[i].c_str());

			} else {
				printf("WARNING: Failed to restore backup file %s!\n", backupFiles[i].c_str());
			}


			m_Mutex.lock();
			m_Progress = float(i) / float(backupFiles.size());
			m_LastFileProcessed = backupFiles[i];
			m_LastFileProcessed.substr(m_LastFileProcessed.find_last_of('/'));
			m_Mutex.unlock();
		}

		m_Mutex.lock();
		m_Progress = 1.0f;
		m_isProcessing = false;
		m_Mutex.unlock();
	}

	float turokfs::get_backup_restore_progress(bool& is_complete, string* last_file) {
		if(m_Mutex.try_lock()) {
			m_lastUsedProgress = m_Progress;
			if(last_file) *last_file = m_LastFileProcessed;
			is_complete = !m_isProcessing;
			m_Mutex.unlock();
		} else is_complete = false;

		return m_lastUsedProgress;
	}
    
    void turokfs::recursiveParseLevels(const string &dir) {
        DirectoryInfo* d = m_fsys->ParseDirectory(dir);
        if(!d) {
            printf("Error reading directory '%s'\n", dir.c_str());
            return;
        }
        
        for(u32 i = 0;i < d->GetEntryCount();i++) {
            string name = d->GetEntryName(i);
            if(name == "." || name == "..") continue;
            
            switch(d->GetEntryType(i)) {
                case DET_FILE: {
                    string ext = d->GetEntryExtension(i);
                    transform(ext.begin(), ext.end(), ext.begin(), tolower);
                    if(ext == "atr") {
                        m_levels.push_back(new level_entry(this, name.substr(0, name.length() - 4), dir + "/" + name));
                    }
                    break;
                }
                case DET_FOLDER: {
                    recursiveParseLevels(dir + "/" + name);
                    break;
                }
                default: {
                    break;
                }
            }
        }
    }

    void turokfs::recursiveParseActors(const string &dir) {
        DirectoryInfo* d = m_fsys->ParseDirectory(dir);
        if(!d) {
            printf("Error reading directory '%s'\n", dir.c_str());
            return;
        }
        
        for(u32 i = 0;i < d->GetEntryCount();i++) {
            string name = d->GetEntryName(i);
            if(name == "." || name == "..") continue;
            
            switch(d->GetEntryType(i)) {
                case DET_FILE: {
                    string ext = d->GetEntryExtension(i);
                    transform(ext.begin(), ext.end(), ext.begin(), tolower);
                    if(ext == "atr") {
                        m_actors.push_back(new actor_entry(this, name.substr(0, name.length() - 4), dir + "/" + name));
                    }
                    break;
                }
                case DET_FOLDER: {
                    recursiveParseActors(dir + "/" + name);
                    break;
                }
                default: {
                    break;
                }
            }
        }
    }

    void turokfs::recursiveParseBackups(const string& dir, vector<string>& files) {
		DirectoryInfo* d = m_fsys->ParseDirectory(dir);
        if(!d) {
            printf("Error reading directory '%s'\n", dir.c_str());
            return;
        }
        
        for(u32 i = 0;i < d->GetEntryCount();i++) {
            string name = d->GetEntryName(i);
            if(name == "." || name == "..") continue;
            
            switch(d->GetEntryType(i)) {
                case DET_FILE: {
                    string ext = d->GetEntryExtension(i);
                    transform(ext.begin(), ext.end(), ext.begin(), tolower);
                    if(ext == "editorbak") {
                        files.push_back(dir + "/" + name);
                    }
                    break;
                }
                case DET_FOLDER: {
                    recursiveParseBackups(dir + "/" + name, files);
                    break;
                }
                default: {
                    break;
                }
            }
        }
	}

	void turokfs::recursiveParseAll(const string& dir) {
		DirectoryInfo* d = m_fsys->ParseDirectory(dir);
        if(!d) {
            printf("Error reading directory '%s'\n", dir.c_str());
            return;
        }
        
        for(u32 i = 0;i < d->GetEntryCount();i++) {
            string name = d->GetEntryName(i);
            if(name == "." || name == "..") continue;
            
            switch(d->GetEntryType(i)) {
                case DET_FILE: {
                    string ext = d->GetEntryExtension(i);
                    transform(ext.begin(), ext.end(), ext.begin(), tolower);
					try {
						m_db->Query(fmt("INSERT INTO tblFile (path, filename, extension) VALUES (\"%s\", \"%s\", \"%s\")",
								   (dir + "/" + name).c_str(),
									name.substr(0, name.find_last_of(".")).c_str(),
									ext.c_str()).c_str()).Next();
					} catch(Exception& e) {
						printf("SQL exception: %s\n", e.GetErrorMsg());
					}
					
                    break;
                }
                case DET_FOLDER: {
                    recursiveParseAll(dir + "/" + name);
                    break;
                }
                default: {
                    break;
                }
            }
        }
	}

	#define query(...) db->Query(fmt(__VA_ARGS__).c_str())
	int construct_actor(Db* db, ATRFile* atr, int file_id) {
		string mesh_id = "NULL";

		if(atr->GetMesh()) {
			string path = TransformPseudoPathToRealPath(atr->GetActorMeshFile());
			path = path.substr(32); // length of "c:/users/computer/desktop/turok/"
			transform(path.begin(), path.end(), path.begin(), ::tolower);

			Statement mesh_file = query("SELECT id FROM tblFile WHERE LOWER(path) = \"%s\"", path.c_str());
			if(mesh_file.Next()) {
				int mesh_file_id = mesh_file.GetField(0);
				Statement existing_mesh_id = query("SELECT id FROM tblActorMesh WHERE file_id = %d", mesh_file_id);
				if(existing_mesh_id.Next()) {
					mesh_id = fmt("%d", (int)existing_mesh_id.GetField(0)); 
				} else {
					query("INSERT INTO tblActorMesh (file_id) VALUES (%d)", mesh_file_id).Next();
					mesh_id = fmt("%d", (int)db->LastId());
				}
			}
		}

		query("INSERT INTO tblActor (file_id, code, version, mesh_id) VALUES (%d, \"%s\", %f, %s)", file_id, atr->GetActorCode().c_str(), atr->GetVersion(), mesh_id.c_str()).Next();
		int actor_id = db->LastId();
		if(atr->GetActorVariables()) {
			ActorVariables* av = atr->GetActorVariables();
			for(int i = 0;i < av->GetBlockCount();i++) {
				Block* b = av->GetBlock(i);
				Statement v = query("SELECT id FROM tblActorVariable WHERE name = \"%s\" AND id IN (SELECT id FROM tblActorVariableMap WHERE actor_id = %d)", b->GetTypeString().c_str(), actor_id);
				if(v.Next()) {
					int var_id = v.GetField(0);
				} else {
					Statement insert = query("INSERT INTO tblActorVariable (block_flag, name, size, data, hdr_0, hdr_1, hdr_2, hdr_3, hdr_4, hdr_5, hdr_6, hdr_7) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
					insert.Bind<char>(1, b->getFlag());
					insert.Bind<string>(2, b->GetTypeString());
					if(!b->GetData()) {
						insert.Bind<long long>(3, 0);
						insert.BindBlob(4, 0, 0);
					}
					else {
						b->GetData()->SetOffset(0);
						insert.Bind<long long>(3, b->GetData()->GetSize());
						insert.BindBlob(4, b->GetData()->Ptr(), b->GetData()->GetSize());
					}
					insert.Bind<char>(5, b->getHeader()[0]);
					insert.Bind<char>(6, b->getHeader()[1]);
					insert.Bind<char>(7, b->getHeader()[2]);
					insert.Bind<char>(8, b->getHeader()[3]);
					insert.Bind<char>(9, b->getHeader()[4]);
					insert.Bind<char>(10, b->getHeader()[5]);
					insert.Bind<char>(11, b->getHeader()[6]);
					insert.Bind<char>(12, b->getHeader()[7]);
					insert.Next();

					int var_id = db->LastId();
					query("INSERT INTO tblActorVariableMap (actor_id, variable_id) VALUES (%d, %d)", actor_id, var_id).Next();
				}
			}
		}

		return actor_id;
	}

	void update_actor(Db* db, ATRFile* atr, const Statement& actor) {
		int actor_id = actor.GetField(0);

		if(atr->GetMesh()) {
			string path = TransformPseudoPathToRealPath(atr->GetActorMeshFile());
			path = path.substr(32); // length of "c:/users/computer/desktop/turok/"
			transform(path.begin(), path.end(), path.begin(), ::tolower);

			int mesh_id = actor.GetField(4);
		}

		if(atr->GetActorVariables()) {
			ActorVariables* av = atr->GetActorVariables();
			for(int i = 0;i < av->GetBlockCount();i++) {
				Block* b = av->GetBlock(i);
				Statement v = query("SELECT id FROM tblActorVariable WHERE name = \"%s\" AND id IN (SELECT id FROM tblActorVariableMap WHERE actor_id = %d)", b->GetTypeString().c_str(), actor_id);
				if(v.Next()) {
					int var_id = v.GetField(0);
				} else {
					Statement insert = query("INSERT INTO tblActorVariable (block_flag, name, size, data, hdr_0, hdr_1, hdr_2, hdr_3, hdr_4, hdr_5, hdr_6, hdr_7) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
					insert.Bind<char>(1, b->getFlag());
					insert.Bind<string>(2, b->GetTypeString());
					if(!b->GetData()) {
						insert.Bind<long long>(3, 0);
						insert.BindBlob(4, 0, 0);
					}
					else {
						b->GetData()->SetOffset(0);
						insert.Bind<long long>(3, b->GetData()->GetSize());
						insert.BindBlob(4, b->GetData()->Ptr(), b->GetData()->GetSize());
					}
					insert.Bind<char>(5, b->getHeader()[0]);
					insert.Bind<char>(6, b->getHeader()[1]);
					insert.Bind<char>(7, b->getHeader()[2]);
					insert.Bind<char>(8, b->getHeader()[3]);
					insert.Bind<char>(9, b->getHeader()[4]);
					insert.Bind<char>(10, b->getHeader()[5]);
					insert.Bind<char>(11, b->getHeader()[6]);
					insert.Bind<char>(12, b->getHeader()[7]);
					insert.Next();

					int var_id = db->LastId();
					query("INSERT INTO tblActorVariableMap (actor_id, variable_id) VALUES (%d, %d)", actor_id, var_id).Next();
				}
			}
		}
	}

	int construct_mesh(Db* db, ActorMesh* mesh, int file_id) {
		query("INSERT INTO tblActorMesh (file_id) VALUES (%d)", file_id).Next();
		int mesh_id = db->LastId();

		for(int i = 0;i < mesh->GetBlockCount();i++) {
			MTFBlock* b = mesh->GetBlock(i);
			Statement mtf_insert = query("INSERT INTO tblMTFBlock (mesh_id, unk_0, unk_1, unk_2, data_offset, data_size, data, type) VALUES (%d, %d, %d, %d, %d, %d, ?, \"%s\")",
				mesh_id, b->m_Unk0, b->m_Unk1, b->m_Unk2, b->m_DataOffset, b->m_DataSize, b->GetTypeString().c_str());

			if(b->m_Data) mtf_insert.BindBlob(1, b->m_Data, b->m_DataSize);

			mtf_insert.Next();
		}

		return mesh_id;
	}

	void update_mesh(Db* db, ActorMesh* mesh, const Statement& meshEntry) {
		int mesh_id = meshEntry.GetField(0);
		//not sure how to detect if a block already exists for a mesh... this is the only way to do handle this right now
		query("DELETE FROM tblMTFBlock WHERE mesh_id = %d", mesh_id).Next();

		for(int i = 0;i < mesh->GetBlockCount();i++) {
			MTFBlock* b = mesh->GetBlock(i);
			Statement mtf_insert = query("INSERT INTO tblMTFBlock (mesh_id, unk_0, unk_1, unk_2, data_offset, data_size, data, type) VALUES (%d, %d, %d, %d, %d, %d, ?, \"%s\")",
				mesh_id, b->m_Unk0, b->m_Unk1, b->m_Unk2, b->m_DataOffset, b->m_DataSize, b->GetTypeString().c_str());

			if(b->m_Data) mtf_insert.BindBlob(1, b->m_Data, b->m_DataSize);

			mtf_insert.Next();
		}
	}

	void construct_level_block(Db* db, int level_id, int parent_id, Block* b) {
		string parentParam = "NULL";
		int sz = 0;
		if(b->GetData()) sz = b->GetData()->GetSize();

		Statement insert = query("INSERT INTO tblATRBlock (level_id, parent_id, flag, hdr_0, hdr_1, hdr_2, hdr_3, hdr_4, hdr_5, hdr_6, hdr_7, type, post_hdr_string, data_size, data) VALUES (%d, %s, ?, ?, ?, ?, ?, ?, ?, ?, ?, \"%s\", \"%s\", %d, ?)",
				level_id, parentParam.c_str(), b->GetTypeString().c_str(), b->getPostHdrString().c_str(), sz);
		insert.Bind<char>(1, b->getFlag());
		insert.Bind<char>(2, b->getHeader()[0]);
		insert.Bind<char>(3, b->getHeader()[1]);
		insert.Bind<char>(4, b->getHeader()[2]);
		insert.Bind<char>(5, b->getHeader()[3]);
		insert.Bind<char>(6, b->getHeader()[4]);
		insert.Bind<char>(7, b->getHeader()[5]);
		insert.Bind<char>(8, b->getHeader()[6]);
		insert.Bind<char>(9, b->getHeader()[7]);
		if(b->GetData()) {
			b->GetData()->SetOffset(0);
			insert.BindBlob(10, b->GetData()->Ptr(), b->GetData()->GetSize());
		}
		else insert.BindNull(10);
		insert.Next();
		int block_id = db->LastId();
		for(int i = 0;i < b->GetChildCount();i++) construct_level_block(db, level_id, block_id, b->GetChild(i));
	}

	void construct_db(Db* db, turokfs* fs) {
		try {
			/* already done, takes forever so don't make this need to happen again
			// process atr files
			Statement actor_files = query("SELECT id, path FROM tblFile WHERE extension = \"atr\"");
			while(actor_files.Next()) {
				int id = actor_files.GetField(0);
				string path = actor_files.GetField(1);

				ATRStorageInterface si;
				ATRFile* atr = si.LoadATR(fmt("C:/Users/Computer/Desktop/Turok/%s", path.c_str()));
				if(atr) {
					Statement actor = query("SELECT * FROM tblActor WHERE file_id = %d", id);
					if(actor.Next()) update_actor(db, atr, actor);
					else construct_actor(db, atr, id);
				} else {
					printf("Failed to load ATR: %s\n", path.c_str());
				}
			}
			*/

			/*
			Statement mesh_files = query("SELECT id, path FROM tblFile WHERE extension = \"mtf\" OR extension = \"atf\"");
			while(mesh_files.Next()) {
				int id = mesh_files.GetField(0);
				string path = mesh_files.GetField(1);

				ActorMesh mesh;
				if(mesh.Load(fmt("C:/Users/Computer/Desktop/Turok/%s", path.c_str()))) {
					printf("Loading %s\n", path.c_str());
					Statement meshEntry = query("SELECT * FROM tblActorMesh WHERE file_id = %d", id);
					if(meshEntry.Next()) update_mesh(db, &mesh, meshEntry);
					else construct_mesh(db, &mesh, id);
				} else {
					printf("Failed to load mesh file: %s\n", path.c_str());
				}
			}
			*/

			/*
			for(int i = 0;i < fs->levels().size();i++) {
				level_entry* entry = fs->levels()[i];
				ATRStorageInterface si;
				ATRFile* atr = si.LoadATR(fmt("C:/Users/Computer/Desktop/Turok/%s", entry->path().c_str()));
				if(atr) {
					string path = entry->path();
					transform(path.begin(), path.end(), path.begin(), ::tolower);
					Statement actor = query("SELECT * FROM tblActor WHERE file_id = (SELECT id FROM tblFile WHERE LOWER(path) = \"%s\")", path.c_str());
					if(actor.Next()) {
						int actor_id = actor.GetField(0);
						int file_id = actor.GetField(1);

						ATIFile* instances = atr->GetActors();
						string level_path = instances->GetFile();
						transform(level_path.begin(), level_path.end(), level_path.begin(), ::tolower);
						level_path = level_path.substr(32); // length of "c:/users/computer/desktop/turok/"

						Statement level = query("SELECT id FROM tblFile WHERE LOWER(path) = \"%s\"", level_path.c_str());
						if(level.Next()) {
							int level_file_id = level.GetField(0);

							query("INSERT INTO tblLevel (actor_id, file_id) VALUES (%d, %d)", actor_id, level_file_id).Next();
							int level_id = db->LastId();

							// insert ati blocks
							for(int lb = 0;lb < instances->blocks().size();lb++) {
								construct_level_block(db, level_id, 0, instances->blocks()[lb]);
							}
						} else {
							printf("Level file doesn't exist in the db!\n");
						}

					} else {
						printf("Actor doesn't exist in the db!\n");
					}
				} else {
					printf("Failed to load ATR: %s\n", entry->path().c_str());
				}
			}
			*/

			/*
			for(int i = 0;i < fs->levels().size();i++) {
				level_entry* entry = fs->levels()[i];
				ATRStorageInterface si;
				ATRFile* atr = si.LoadATR(fmt("C:/Users/Computer/Desktop/Turok/%s", entry->path().c_str()));
				if(atr) {
					ATIFile* instances = atr->GetActors();
					string level_path = instances->GetFile();
					transform(level_path.begin(), level_path.end(), level_path.begin(), ::tolower);
					level_path = level_path.substr(32); // length of "c:/users/computer/desktop/turok/"

					Statement level = query("SELECT id FROM tblLevel WHERE file_id = (SELECT id FROM tblFile WHERE LOWER(path) = \"%s\")", level_path.c_str());
					if(level.Next()) {
						int level_id = level.GetField(0);
						vector<int>block_ids;
						Statement blocks = query("SELECT id FROM tblATRBlock WHERE level_id = %d AND type = \"ACTOR\"", level_id);
						while(blocks.Next()) block_ids.push_back(blocks.GetField(0));

						for(int a = 0;a < instances->GetActorCount();a++) {
							ActorDef* def = instances->GetActorDef(a);
							string actor_path = TransformPseudoPathToRealPath(def->ActorFile);
							transform(actor_path.begin(), actor_path.end(), actor_path.begin(), ::tolower);
							actor_path = actor_path.substr(32); // length of "c:/users/computer/desktop/turok/"
							Statement actor = query("SELECT id FROM tblActor WHERE file_id = (SELECT id FROM tblFile WHERE LOWER(path) = \"%s\")", actor_path.c_str());
							if(actor.Next()) {
								int actor_id = actor.GetField(0);
								query("INSERT INTO tblActorInstance (level_id, actor_id, block_id, instance_id, name, tx, ty, tz, rx, ry, rz, sx, sy, sz) VALUES (%d, %d, %d, %d, \"%s\", %f, %f, %f, %f, %f, %f, %f, %f, %f)",
								level_id, actor_id, block_ids[a], (int)def->ID, def->Name, def->Position.x, def->Position.y, def->Position.z,
								def->Rotation.x, def->Rotation.y, def->Rotation.z, def->Scale.x, def->Scale.y, def->Scale.z).Next();
							}
						}
					} else {
						printf("Level file doesn't exist in the db!\n");
					}
				} else {
					printf("Failed to load ATR: %s\n", entry->path().c_str());
				}
			}
			*/

			/*
			unordered_map<int, int> mesh_texcounts;
		
			string qry = "SELECT mesh.id as mesh_id, file.path as path FROM tblActorMesh as mesh LEFT OUTER JOIN tblFile as file ON file.id = mesh.file_id WHERE mesh.id >= 2361";
			try {
				Statement q = m_db->Query(qry.c_str());
				while(q.Next()) {
					int mesh_id = q.GetField(0);
					string path = q.GetField(1);
					ActorMesh m;
					printf("Dimensionalizing actor mesh %s... ", path.c_str());
					if(m.Load(path)) {
						//m.m_TXSTs
						vector<int> txst_ids;
						for(auto i = 0;i < m.m_TXSTs.size();i++) {
							TXST& txst = m.m_TXSTs[i];
							int block_id = get_block_id(mesh_id, "TSXT", i, m_db);
							string texture_query = fmt("(SELECT id FROM tblMeshTexture WHERE mesh_id = %d LIMIT %d, 1)", mesh_id, txst.TextureID);
							if(txst.TextureID == -1) texture_query = "NULL";

							qry = "INSERT INTO tblTXST (mesh_id, block_id, unk_0, texture_id, unk_1, unk_2, unk_3, unk_4, unk_5, unk_6, unk_7, unk_8, unk_9, unk_10, unk_11, unk_12, unk_13, unk_14, unk_15, unk_16, unk_17, unk_18) VALUES ";
							qry += fmt("(%d, %d, %d, %s, %d, %d, %f, %d, %d, %d, %d, %f, %f, %d, %d, %d, %d, %d, %f, %d, %f, %d)",
										mesh_id, block_id, txst.Unk0, texture_query.c_str(), txst.Unk1, txst.Unk2, txst.Unk3, txst.Unk4, txst.Unk5, txst.Unk6, txst.Unk7, txst.Unk8, txst.Unk9, txst.Unk10,
										txst.Unk11, txst.Unk12, txst.Unk13, txst.Unk14, txst.Unk15, txst.Unk16, txst.Unk17, txst.Unk18);
							m_db->Query(qry.c_str()).Next();
							txst_ids.push_back(m_db->LastId());
						}

						//m.m_TSNRs
						vector<int>tsnr_ids;
						for(auto i = 0;i < m.m_TSNRs.size();i++) {
							TSNR& tsnr = m.m_TSNRs[i];
							int block_id = get_block_id(mesh_id, "TSNR", i, m_db);
							int txst_id = txst_ids[tsnr.TXST_ID];

							qry = fmt("INSERT INTO tblRNST (mesh_id, block_id, unk_0, TSXT_id, unk_1, unk_2, unk_3, unk_4) VALUES (%d, %d, %d, %d, %d, %d, %d, %d)",
									  mesh_id, block_id, tsnr.Unk0, txst_id, tsnr.Unk1, tsnr.Unk2, tsnr.Unk3, tsnr.Unk4);
							m_db->Query(qry.c_str()).Next();
							tsnr_ids.push_back(m_db->LastId());
						}

						//m.m_MeshInfos
						vector<int>info_ids;
						for(auto i = 0;i < m.m_MeshInfos.size();i++) {
							MeshInfo& info = m.m_MeshInfos[i];
							int block_id = get_block_id(mesh_id, "OFNI", i, m_db);
							string rnst_id = (info.TSNR_ID == -1 || info.TSNR_ID >= tsnr_ids.size()) ? "NULL" : fmt("%d", tsnr_ids[info.TSNR_ID]);

							qry = fmt("INSERT INTO tblINFO (mesh_id, block_id, RNST_id, unk_0, vertex_type, unk_2, unk_3, unk_4, unk_5, unk_6, unk_7, unk_8) VALUES (%d, %d, %s, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
									  mesh_id, block_id, rnst_id.c_str(), info.Unk0, info.Unk1, info.Unk2, info.Unk3, info.Unk4, info.Unk5, info.Unk6, info.Unk7, info.Unk8);
							m_db->Query(qry.c_str()).Next();
							info_ids.push_back(m_db->LastId());
						}
						printf("Okay.\n");
					} else {
						printf("Failed.\n");
					}
				}
			} catch(Exception& e) {
				printf("SQL exception: %s\nQuery: %s\n", e.GetErrorMsg(), qry.c_str());
			}
			*/
		} catch(Exception& e) {
			printf("SQL exception: %s\n", e.GetErrorMsg());
		}
	}
}
