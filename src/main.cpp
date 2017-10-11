#include <app.h>
using namespace t4editor;

#include <gui/ui.h>
#include <turokfs/entry.h>

#include <unordered_map>
using namespace std;

vector<ATRFile*> levels;
void parse_levels();

int main(int argc,const char* argv[]) {
    application app(argc, argv);
    if(app.initialize()) {

		for(int i = 0;i < app.getTurokData()->levels().size();i++) {
			level_entry* e = app.getTurokData()->levels()[i];
			string path = e->path();
			ATRFile* f = new ATRFile();
			if(f->Load(e->path())) levels.push_back(f);
			else {
				delete f;
				printf("Couldn't load level: %s\n", path.c_str());
			}
		}

		parse_levels();
		return -1;

        register_ui(&app);
        int r = app.run();
        destroy_ui();
        return r;
    }
    return -1;
}
void parse_block(Block* b, unordered_map<unsigned char,vector<Block*> >& map) {
	for(int i = 0;i < b->GetChildCount();i++) {
		parse_block(b->GetChild(i), map);
	}
	if(b->GetChildCount() == 0) {
		auto it = map.find(b->m_PreBlockFlag);
		if(it == map.end()) map[b->m_PreBlockFlag] = vector<Block*>();
		map[b->m_PreBlockFlag].push_back(b);
	}
}

void parse_levels() {
	unordered_map<unsigned char,vector<Block*> > m_BlockMap;

	for(int i = 0;i < levels.size();i++) {
		ATRFile* file = levels[i];
		parse_block(file->GetRootBlock(), m_BlockMap);

		ATIFile* ati = file->GetActors();
		if(ati) {
			for(int a = 0;a < ati->GetActorCount();a++) {
				ActorDef* actorDef = ati->GetActorDef(a);
				parse_block(actorDef->Actor->GetATR()->GetRootBlock(), m_BlockMap);
			}
		}
	}

	for(auto it = m_BlockMap.begin();it != m_BlockMap.end();it++) {
		unsigned char flag = it->first;
		vector<Block*> blocks = it->second;
		
		char buf[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		snprintf(buf, 16, "log.0x%X.txt", flag);

		FILE* fp = fopen(buf, "w");
		fprintf(fp, "\n\n\n\n--------------BLOCK HEADER 0x%2X (%d)--------------\n", flag, flag);
		printf("\n\n\n\n--------------BLOCK HEADER 0x%2X (%d)--------------\n", flag, flag);
		for(int i = 0;i < blocks.size();i++) {
			fprintf(fp, "-----Block: %s | %lu bytes\n", blocks[i]->GetTypeString().c_str(), blocks[i]->GetData()->GetSize());
			printf("-----Block: %s | %lu bytes\n", blocks[i]->GetTypeString().c_str(), blocks[i]->GetData()->GetSize());
		}
		fprintf(fp, "end\n");
		printf("end\n");
		fclose(fp);

		
		fp = fopen((string(buf) + ".dat").c_str(), "wb");
		for(int i = 0;i < blocks.size();i++) {
			fwrite(blocks[i]->m_Hdr, 8, 1, fp);
		}
		fclose(fp);
	}

	
	return;
}