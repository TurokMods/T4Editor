#pragma once
#include <Bearclaw/EngineTypes.h>

#include <stdio.h>
#include <string>
#include <vector>
#include <list>
using namespace std;

namespace Bearclaw
{
    enum DIR_ENTRY_TYPE
    {
        DET_FILE,
        DET_FOLDER,
        DET_LINK,
        DET_INVALID,
    };

    class DirEnt
    {
        public:
            DirEnt() : Type(DET_INVALID) { }
            DirEnt(const DirEnt& o);
            ~DirEnt() { }
            
            DIR_ENTRY_TYPE Type;
            string Name;
            string Extension;
    };

    class DirectoryInfo
    {
        public:
            DirectoryInfo() : m_Entries(0), m_EntryCount(0) { }
            ~DirectoryInfo() { if(m_Entries) delete [] m_Entries; }
        
            void Populate(const string& Dir);
            int GetEntryCount() const { return m_EntryCount; }
            DIR_ENTRY_TYPE GetEntryType(i32 Idx) const { return m_Entries[Idx].Type; }
            string GetEntryName(i32 Idx) const { return m_Entries[Idx].Name; }
            string GetEntryExtension(i32 Idx) const { return m_Entries[Idx].Extension; }
        
            /* std::vector refuses to work in this case... */
            DirEnt* m_Entries;
            i32 m_EntryCount;
    };

    enum DATA_MODE
    {
        DM_BINARY,
        DM_TEXT,
    };
    
    /* 
     * NOTE:
     * Data written with this class in non-streaming mode will not be
     * written to a file until Save is called.
     */
    class FileSystem;
    class DataContainer
    {
        public:
            RefCount Hold   () { return ++m_RefCount; }
            RefCount Release();
        
            bool ReadData    (      void* Data,u32 Size);
            bool WriteData   (const void* Data,u32 Size);
            bool WriteString (const string& Data) { return WriteData(&Data[0],Data.length()); }
        
            template <typename T> bool Read (      T& Data) { return ReadData ((      void*)&Data,sizeof(T)); }
            template <typename T> bool Write(const T& Data) { return WriteData((const void*)&Data,sizeof(T)); }
        
            /*
             * The following functions functions automatically parse
             * text if the file is not opened in binary mode
             */
            bool  ReadUByte   (u8&   Data);
            bool  ReadByte    (s8&   Data);
            bool  ReadUInt16  (u16&  Data);
            bool  ReadUInt32  (u32&  Data);
            bool  ReadUInt64  (u64&  Data);
            bool  ReadInt16   (i16&  Data);
            bool  ReadInt32   (i32&  Data);
            bool  ReadInt64   (i64&  Data);
            bool  ReadFloat32 (f32&  Data);
            bool  ReadFloat64 (f64&  Data);
            
            /*
             * The following functions functions automatically
             * convert values to text if the file is not opened
             * in binary mode.
             */
            bool  WriteUByte  (const u8&   Data);
            bool  WriteByte   (const s8&   Data);
            bool  WriteUInt16 (const u16&  Data);
            bool  WriteUInt32 (const u32&  Data);
            bool  WriteUInt64 (const u64&  Data);
            bool  WriteInt16  (const i16&  Data);
            bool  WriteInt32  (const i32&  Data);
            bool  WriteInt64  (const i64&  Data);
            bool  WriteFloat32(const f32&  Data);
            bool  WriteFloat64(const f64&  Data);
        
            void  Seek        (i32 Offset); /* Adds Offset to m_Offset */
            void  SetPosition (u32 Offset);
            u32   GetPosition ()                  const { return m_Offset; }
            bool  AtEnd       (u32 EndOffset = 0) const { return m_Offset >= m_Size - EndOffset; }
            void* Ptr         ()                        { return &m_Data[m_Offset]; }
            u32   GetSize     ()                  const { return m_Size; }
        
            string GetName() const { return m_Name; }
        
        protected:
            friend class FileSystem;
            DataContainer(FileSystem* fs,FILE* fp,const string& Name,DATA_MODE Mode) :
                m_RefCount(0), m_FileSys(fs), m_Name(Name), m_Mode(Mode), m_Handle(fp), m_Size(0), m_Offset(0) { }
            ~DataContainer();
        
            typedef list<DataContainer*>::iterator ContainerID;
        
            RefCount    m_RefCount;
            FileSystem* m_FileSys;
            string      m_Name;
            DATA_MODE   m_Mode;
            ContainerID m_Iterator;
        
            /* For streamed data */
            FILE*       m_Handle;
            u32         m_Size;
        
            /* For in-memory data */
            vector<u8>  m_Data;
            u32         m_Offset;
    };
    
    class FileSystem
    {
        public:
            FileSystem() { }
            ~FileSystem() { }
        
            /* Container names default to filenames when not set (or pointer address when Create() is used) */
            DataContainer* Create               (                   DATA_MODE Mode,const string& Name = "");
            DataContainer* Open                 (const string& File,DATA_MODE Mode,const string& Name = "");
            DataContainer* Load                 (const string& File,DATA_MODE Mode,const string& Name = "");
            bool           Save                 (DataContainer* Data,const string& File);
        
            string         GetWorkingDirectory  () const;
            void           SetWorkingDirectory  (const string& Dir);
            DirectoryInfo* ParseDirectory       (const string& Dir);
            bool           Exists               (const string& Item);
    
            void           Shutdown();
        
        protected:
            friend class DataContainer;
            void Destroy(DataContainer* Container);
            
            list<DataContainer*> m_Containers;
        
    };
};
