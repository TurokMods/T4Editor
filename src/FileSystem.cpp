#include <FileSystem.h>
#include <EngineTypes.h>


#ifdef _WIN32
#include <turokfs/dirent.h>
#include <direct.h>
#define atoll _atoi64
#else
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <memory.h>
#endif

#include <stdarg.h>
#include <ctype.h>
#include <cstdlib>
#include <logger.h>

namespace Bearclaw
{

    void DirectoryInfo::Populate(const string& Dir)
    {
        if(m_Entries) { delete [] m_Entries; m_Entries = 0; }
        m_EntryCount = 0;

        DIR *dp = opendir(Dir.c_str());
        if(dp != NULL)
        {
            struct dirent *ep = readdir(dp);
            while(ep)
            {
                m_EntryCount++;
                ep = readdir(dp);
            }
            rewinddir(dp);
            m_Entries = new DirEnt[m_EntryCount];
            int i = 0;
            ep = readdir(dp);
            while(ep)
            {
                #ifdef _WIN32
                string n = string(ep->d_name,ep->d_namlen);
                #else
                string n = string(ep->d_name);
                #endif
                if(n.size() != 0 && n != ".DS_Store")
                {
                    m_Entries[i].Name = n;
                    switch(ep->d_type)
                    {
                        case DT_REG: { m_Entries[i].Type = DET_FILE;    break; }
                        case DT_DIR: { m_Entries[i].Type = DET_FOLDER;  break; }
                        case DT_LNK: { m_Entries[i].Type = DET_LINK;    break; }
                        default:     { m_Entries[i].Type = DET_INVALID; break; }
                    }
                    if(ep->d_type == DT_REG)
                    {
                        size_t extbegin = m_Entries[i].Name.rfind('.');
                        if(extbegin != m_Entries[i].Name.npos) m_Entries[i].Extension = m_Entries[i].Name.substr(extbegin + 1);
                        else m_Entries[i].Extension = "";
                    }
                    i++;
                }
                ep = readdir(dp);
            }
            closedir(dp);
        }
    }

    DataContainer::~DataContainer()
    {
    }
    RefCount DataContainer::Release()
    {
        m_RefCount--;
        if(m_RefCount <= 0)
        {
            m_FileSys->Destroy(this);
            return 0;
        }
        return m_RefCount;
    }

    bool DataContainer::ReadData(void* Data,u32 Size)
    {
        if(m_Handle)
        {
            if(fread(Data,Size,1,m_Handle) != 1)
            {
                ERROR("Failed to read %u bytes from data container (%s): Call to fread failed\n",Size,m_Name.c_str());
                return false;
            }
            else
            {
                m_Offset += Size;
                return true;
            }
        }
        if(AtEnd(Size))
        {
            ERROR("Failed to read %u bytes from data container (%s): End of data encountered\n",Size,m_Name.c_str());
            return false;
        }
        memcpy(Data,Ptr(),Size);
        m_Offset += Size;
        return true;
    }
    bool DataContainer::WriteData(const void* Data,u32 Size)
    {
        if(m_Handle)
        {
            if(fwrite(Data,Size,1,m_Handle) != 1)
            {
                ERROR("Failed to write %u bytes to data container (%s): Call to fwrite failed\n",Size,m_Name.c_str());
                return false;
            }
            else
            {
                m_Offset += Size;
                m_Size += Size;
                return true;
            }
        }

        for(u64 i = 0;i < Size;i++)
        {
            m_Data.insert(m_Data.begin() + m_Offset,((u8*)Data)[i]);
            m_Offset++;
            m_Size++;
        }
        return true;
    }

    bool DataContainer::ReadUByte(u8& Data) { return Read(Data); }
    bool DataContainer::ReadByte (s8& Data) { return Read(Data); }

    bool IsWhitespace(char c)
    {
        return c == '\t' || c == ' ';
    }
    bool IsNumber(char c)
    {
        return isdigit(c);
    }

    #define ParseInteger(Type,Min,Max,MinFmt,MaxFmt) \
    char c = 0; \
    if(!Read(c)) { ERROR("Failed to parse integer from data container (%s)\n",m_Name.c_str()); return false; } \
    while(IsWhitespace(c)) { if(!Read(c)) { ERROR("Failed to parse integer from data container (%s)\n",m_Name.c_str()); return false; } } \
    bool IsNeg = c == '-'; \
    if(IsNeg) \
    { \
        if(!Read(c)) { ERROR("Failed to parse integer from data container (%s)\n",m_Name.c_str()); return false; } \
    } \
    string s; \
    while(IsNumber(c)) \
    { \
        s += c; \
        if(!Read(c)) { ERROR("Failed to parse integer from data container (%s)\n",m_Name.c_str()); return false; } \
    } \
    long long i = atoll(s.c_str()); \
    if(i > Max) \
    { \
        WARNING("Parsing integer from data container (%s) that will not fit in data type ("#Type")\n",m_Name.c_str()); \
        LOG("Info: Parsed value: %lli | Min/Max value for data type: "#MinFmt"/"#MaxFmt,i,Min,Max); \
    } \
    if(IsNeg) Data = -i; \
    else Data = i;

    bool DataContainer::ReadUInt16(u16& Data)
    {
        if(m_Mode == DM_BINARY) return Read(Data);
        else
        {
            ParseInteger(u16,0,UINT16_MAX,%d,%d);
        }
        return true;
    }
    bool DataContainer::ReadUInt32(u32& Data)
    {
        if(m_Mode == DM_BINARY) return Read(Data);
        else
        {
            ParseInteger(u32,0,UINT32_MAX,%d,%d);
        }
        return true;
    }
    bool DataContainer::ReadUInt64(u64& Data)
    {
        if(m_Mode == DM_BINARY) return Read(Data);
        else
        {
            ParseInteger(u64,0,UINT64_MAX,%d,%llu);
        }
        return true;
    }
    bool DataContainer::ReadInt16(i16& Data)
    {
        if(m_Mode == DM_BINARY) return Read(Data);
        else
        {
            ParseInteger(i16,INT16_MIN,INT16_MAX,%d,%d);
        }
        return true;
    }
    bool DataContainer::ReadInt32(i32& Data)
    {
        if(m_Mode == DM_BINARY) return Read(Data);
        else
        {
            ParseInteger(i32,INT32_MIN,INT32_MAX,%d,%d);
        }
        return true;
    }
    bool DataContainer::ReadInt64(i64& Data)
    {
        if(m_Mode == DM_BINARY) return Read(Data);
        else
        {
            ParseInteger(i64,INT64_MIN,INT64_MAX,%lli,%lli);
        }
        return true;
    }
    bool DataContainer::ReadFloat32(f32& Data)
    {
        if(m_Mode == DM_BINARY) return Read(Data);
        else
        {
            char c = 0;
            if(!Read(c)) { ERROR("Failed to parse float32 from data container (%s)\n",m_Name.c_str()); return false; }

            //Skip whitespace
            while(IsWhitespace(c)) { if(!Read(c)) { ERROR("Failed to parse float32 from data container (%s)\n",m_Name.c_str()); return false; } }

            //Skip '-' if negative
            bool IsNeg = c == '-';
            if(IsNeg)
            {
                if(!Read(c)) { ERROR("Failed to parse float32 from data container (%s)\n",m_Name.c_str()); return false; }
            }

            //Parse value
            bool DecimalEncountered = false;
            string s;
            while(IsNumber(c) || (c == '.' && !DecimalEncountered))
            {
                if(c == '.') DecimalEncountered = true;
                s += c;
                if(!Read(c)) { ERROR("Failed to parse float32 from data container (%s)\n",m_Name.c_str()); return false; }
            }

            f64 f = atof(s.c_str());
            if(f > FLT_MAX)
            {
                WARNING("Parsing real number from data container (%s) that will not fit in data type (f32)\n",m_Name.c_str());
                LOG("Info: Parsed value: %f | Min/Max value for data type: %f,%f\n",f,FLT_MIN,FLT_MAX);
            }
            u32 Digits = s.length();
            if(DecimalEncountered) Digits--;
            if(Digits > 7)
            {
                WARNING("Parsing real number from data container (%s) into data type (f32) that may lose precision (more than 7 digits)\n",m_Name.c_str());
                LOG("Info: Actual value: %s | float value: %f\n",s.c_str(),(f32)f);
            }

            if(IsNeg) Data = -f;
            else Data = f;
        }
        return true;
    }
    bool DataContainer::ReadFloat64(f64& Data)
    {
        if(m_Mode == DM_BINARY) return Read(Data);
        else
        {
            char c = 0;
            if(!Read(c)) { ERROR("Failed to parse float64 from data container (%s)\n",m_Name.c_str()); return false; }

            //Skip whitespace
            while(IsWhitespace(c)) { if(!Read(c)) { ERROR("Failed to parse float64 from data container (%s)\n",m_Name.c_str()); return false; } }

            //Skip '-' if negative
            bool IsNeg = c == '-';
            if(IsNeg)
            {
                if(!Read(c)) { ERROR("Failed to parse float64 from data container (%s)\n",m_Name.c_str()); return false; }
            }

            //Parse value
            bool DecimalEncountered = false;
            string s;
            while(IsNumber(c) || (c == '.' && !DecimalEncountered))
            {
                if(c == '.') DecimalEncountered = true;
                s += c;
                if(!Read(c)) { ERROR("Failed to parse float64 from data container (%s)\n",m_Name.c_str()); return false; }
            }

            f64 f = atof(s.c_str());
            if(f > DBL_MAX)
            {
                WARNING("Parsing real number from data container (%s) that will not fit in data type (f64)\n",m_Name.c_str());
                LOG("Info: Parsed value: %f | Min/Max value for data type: %f,%f\n",f,DBL_MIN,DBL_MAX);
            }
            u32 Digits = s.length();
            if(DecimalEncountered) Digits--;
            if(Digits > 7)
            {
                WARNING("Parsing real number from data container (%s) into data type (f64) that may lose precision (more than 16 digits)\n",m_Name.c_str());
                LOG("Info: Actual value: %s | float64 value: %f\n",s.c_str(),(f64)f);
            }

            if(IsNeg) Data = -f;
            else Data = f;
        }
        return true;
    }

    bool DataContainer::WriteUByte(const u8& Data) { return Write(Data); }
    bool DataContainer::WriteByte (const s8& Data) { return Write(Data); }

	i32 _snprintf(char* Out,size_t MaxLen,const char* fmt,...)
	{
		//Windows friendly
		va_list Args;
		va_start(Args,fmt);
#ifdef _WIN32
		i32 r = vsnprintf_s(Out,MaxLen,MaxLen,fmt,Args);
#else
		i32 r = vsnprintf(Out,MaxLen,fmt,Args);
#endif
		va_end(Args);
		return r;
	}

    #define WriteInteger(Type,MaxStrLen,fmt) \
    char Out[MaxStrLen]; \
    i32 r = _snprintf(Out,MaxStrLen,#fmt,Data); \
    if(r < 0) \
    { \
        ERROR("Failed to convert integer (" #Type ") to string for data container (%s): call to snprintf returned %d\n",m_Name.c_str(),r); \
        return false; \
    } \
    else return WriteData(Out,r);

    bool DataContainer::WriteUInt16(const u16& Data)
    {
        if(m_Mode == DM_BINARY) return Write(Data);
        else
        {
            WriteInteger(u16,32,%d);
        }
        return true;
    }
    bool DataContainer::WriteUInt32(const u32& Data)
    {
        if(m_Mode == DM_BINARY) return Write(Data);
        else
        {
            WriteInteger(u32,32,%d);
        }
        return true;
    }
    bool DataContainer::WriteUInt64(const u64& Data)
    {
        if(m_Mode == DM_BINARY) return Write(Data);
        else
        {
            WriteInteger(u64,32,%llu);
        }
        return true;
    }
    bool DataContainer::WriteInt16(const i16& Data)
    {
        if(m_Mode == DM_BINARY) return Write(Data);
        else
        {
            WriteInteger(i16,32,%d);
        }
        return true;
    }
    bool DataContainer::WriteInt32(const i32& Data)
    {
        if(m_Mode == DM_BINARY) return Write(Data);
        else
        {
            WriteInteger(i32,32,%d)
        }
        return true;
    }
    bool DataContainer::WriteInt64(const i64& Data)
    {
        if(m_Mode == DM_BINARY) return Write(Data);
        else
        {
            WriteInteger(i64,32,%lli)
        }
        return true;
    }
    bool DataContainer::WriteFloat32(const f32& Data)
    {
        if(m_Mode == DM_BINARY) return Write(Data);
        else
        {
            char Out[32];
            i32 r = _snprintf(Out,32,"%f",Data);
            if(r < 0)
            {
                ERROR("Failed to convert real number (f32) to string for data container (%s): call to snprintf returned %d\n",m_Name.c_str(),r);
                return false;
            }
            else return WriteData(Out,r);
        }
        return true;
    }
    bool DataContainer::WriteFloat64(const f64& Data)
    {
        if(m_Mode == DM_BINARY) return Write(Data);
        else
        {
            char Out[32];
            i32 r = _snprintf(Out,32,"%f",Data);
            if(r < 0)
            {
                ERROR("Failed to convert real number (f64) to string for data container (%s): call to snprintf returned %d\n",m_Name.c_str(),r);
                return false;
            }
            else return WriteData(Out,r);
        }
        return true;
    }

    void DataContainer::Seek(i32 Offset)
    {
        if(m_Offset + Offset > m_Size)
        {
            ERROR("Call to Seek(%u) with data container (%s) failed: End of data would be exceeded\n",Offset,m_Name.c_str());
            return;
        }
        if(((i32)m_Offset + (i32)Offset) < 0)
        {
            ERROR("Call to Seek(%u) with data container (%s) failed: Resulting data position less than 0\n",Offset,m_Name.c_str());
            return;
        }

        if(m_Handle)
        {
            i32 r = fseek(m_Handle,Offset,SEEK_CUR);
            if(r != 0)
            {
                ERROR("Call to Seek(%d) with data container (%s) failed: Call to fseek returned (%d)\n",Offset,m_Name.c_str(),r);
            }
            else m_Offset += Offset;
        }
    }
    void DataContainer::SetPosition(u32 Offset)
    {
        if(Offset > m_Size)
        {
            ERROR("Call to SetPosition(%u) with data container (%s) failed: New position exceeds end of data\n",Offset,m_Name.c_str());
            return;
        }

        if(m_Handle)
        {
            i32 r = fseek(m_Handle,Offset,SEEK_SET);
            if(r != 0)
            {
                ERROR("Call to Seek(%d) with data container (%s) failed: Call to fseek returned (%d)\n",Offset,m_Name.c_str(),r);
            }
            else m_Offset = Offset;
        }
        else m_Offset = Offset;
    }

    DataContainer* FileSystem::Create(DATA_MODE Mode,const string& Name)
    {
        DataContainer* c = new DataContainer(this,0,Name,Mode);
        if(Name.length() == 0)
        {
            char addr[32];
            memset(addr,0,32);
            _snprintf(addr,32,"0x%lX",(Ptr)c);
            c->m_Name = addr;
        }
        c->m_Iterator = m_Containers.insert(m_Containers.end(),c);
        return c;
    }
    DataContainer* FileSystem::Open(const string& File,DATA_MODE Mode,const string& Name)
    {
        #ifndef __WIN32
        FILE* fp = fopen(File.c_str(),"rb+");
        #else
        #endif
        if(!fp)
        {
            ERROR("Failed to open file (%s)\n",File.c_str());
            return 0;
        }

        DataContainer* c = new DataContainer(this,fp,Name.length() == 0 ? File : Name,Mode);
        fseek(fp,0,SEEK_END);
        c->m_Size = ftell(fp);
        fseek(fp,0,SEEK_SET);

        c->m_Iterator = m_Containers.insert(m_Containers.end(),c);
        return c;
    }
    DataContainer* FileSystem::Load(const string& File,DATA_MODE Mode,const string& Name)
    {
        #ifndef __WIN32
        FILE* fp = fopen(File.c_str(),"rb+");
        #else
        #endif
        if(!fp)
        {
            ERROR("Failed to open file (%s)\n",File.c_str());
            return 0;
        }

        fseek(fp,0,SEEK_END);
        u32 Sz = ftell(fp);
        fseek(fp,0,SEEK_SET);

        if(Sz == 0)
        {
            fclose(fp);
            WARNING("Loading empty file (%s) into data container (%s)\n",File.c_str(),Name.length() == 0 ? File.c_str() : Name.c_str());

            DataContainer* c = new DataContainer(this,0,Name.length() == 0 ? File : Name,Mode);
            c->m_Iterator = m_Containers.insert(m_Containers.end(),c);
            return c;
        }
        else
        {
            u8* Data = new u8[Sz];
            if(fread(Data,Sz,1,fp) != 1)
            {
                ERROR("Failed to load %u bytes from file (%s) into memory\n",Sz,File.c_str());
                fclose(fp);
                return 0;
            }

            DataContainer* c = new DataContainer(this,0,Name.length() == 0 ? File : Name,Mode);
            c->m_Size = Sz;
            for(u32 i = 0;i < c->m_Size;i++) c->m_Data.push_back(Data[i]);
            delete [] Data;
            fclose(fp);

            c->m_Iterator = m_Containers.insert(m_Containers.end(),c);
            return c;
        }
    }
    bool FileSystem::Save(DataContainer* Data,const string& File)
    {
        #ifndef __WIN32
        FILE* fp = fopen(File.c_str(),"wb");
        #else
        #endif
        if(!fp)
        {
            ERROR("Failed to open file (%s)\n",File.c_str());
            return false;
        }

        if(fwrite(&Data->m_Data[0],Data->m_Size,1,fp) != 1)
        {
            ERROR("Failed to write %u bytes from data container (%s) to file (%s)\n",Data->m_Size,Data->m_Name.c_str(),File.c_str());
            fclose(fp);
            return false;
        }

        fclose(fp);
        return true;
    }

#ifdef _WIN32
#define getcwd _getcwd
#define chdir _chdir
#endif

    string FileSystem::GetWorkingDirectory() const
    {
        char cwd[256];
        getcwd(cwd,256);
        return cwd;
    }
    void FileSystem::SetWorkingDirectory(const string& Dir)
    {
        if(!Exists(Dir))
        {
            ERROR("Unable to set current working directory (%s): Directory does not exist\n",Dir.c_str());
            return;
        }

		chdir(Dir.c_str());
    }
    DirectoryInfo* FileSystem::ParseDirectory(const string& Dir)
    {
        if(!Exists(Dir))
        {
            ERROR("Unable to parse directory (%s): Directory does not exist\n",Dir.c_str());
            return 0;
        }

        DirectoryInfo* Info = new DirectoryInfo();
        Info->Populate(Dir);
        return Info;
    }
    bool FileSystem::Exists(const string& Item)
    {
        struct stat buf;
        return (stat(Item.c_str(),&buf) == 0);
    }

    void FileSystem::Shutdown()
    {
        if(m_Containers.size() != 0)
        {
            WARNING("FileSystem has %lu unreleased data containers at shutdown time\n",m_Containers.size());

            for(DataContainer::ContainerID c = m_Containers.begin();c != m_Containers.end();c++)
            {
                LOG("Destroying unreleased container: %s\n",(*c)->m_Name.c_str());
                delete *c;
            }
        }
    }

    void FileSystem::Destroy(DataContainer *Container)
    {
        if(Container->m_FileSys == 0) return;
        Container->m_FileSys = 0;
        m_Containers.erase(Container->m_Iterator);
        if(Container->m_Handle) fclose(Container->m_Handle);
        Container->m_Handle = 0;
        delete Container;
    }
};
