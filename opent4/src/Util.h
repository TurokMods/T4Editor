#ifndef LOADER_UTIL_H
#define LOADER_UTIL_H

#include <cstring>
#include <string>
#include <vector>

#include <stdint.h>
#include <stdio.h>

namespace opent4
{
    class ByteStream
    {
        public:
            ByteStream(FILE* fp) : m_Offset(0) { while(!feof(fp)) { m_Bytes.push_back(getc(fp)); } }
            ByteStream() : m_Offset(0) { }
            ~ByteStream() { }

            bool WriteByte(char b)
            {
                if(m_Offset == m_Bytes.size()) {
                    m_Bytes.push_back(b);
                    m_Offset++;
                }
                else m_Bytes[m_Offset++] = b;
                return true;
            }

            bool WriteInt32 (int i)
            {
                WriteByte(((char*)&i)[0]);
                WriteByte(((char*)&i)[1]);
                WriteByte(((char*)&i)[2]);
                WriteByte(((char*)&i)[3]);
                return true;
            }

            bool WriteInt16 (int i)
            {
                int16_t i16 = i;
                WriteByte(((char*)&i16)[0]);
                WriteByte(((char*)&i16)[1]);
                return true;
            }

            bool WriteFloat (float f)
            {
                WriteByte(((char*)&f)[0]);
                WriteByte(((char*)&f)[1]);
                WriteByte(((char*)&f)[2]);
                WriteByte(((char*)&f)[3]);
                return true;
            }

            bool WriteString(const std::string& s)
            {
                for(size_t i = 0; i < s.length(); i++)
                    WriteByte(s[i]);
                WriteByte(0);
                return true;
            }

            bool WriteData(size_t Sz, void* Ptr)
            {
                for(size_t i = 0; i < Sz; i++)
                    WriteByte(((char*)Ptr)[i]);
                return true;
            }

            char GetByte()
            {
                if(m_Offset == m_Bytes.size()) return EOF;
                return m_Bytes[m_Offset++];
            }

            int GetInt32()
            {
                int i;
                std::memcpy(&i,&m_Bytes[m_Offset],4);
                m_Offset += 4;
                return i;
            }

            int16_t GetInt16()
            {
                int16_t i;
                std::memcpy(&i,&m_Bytes[m_Offset],2);
                m_Offset += 2;
                return i;
            }

            float GetFloat()
            {
                float i;
                std::memcpy(&i,&m_Bytes[m_Offset],4);
                m_Offset += 4;
                return i;
            }

            std::string GetString(size_t Len = 0)
            {
                if(Len == 0)
                {
                    std::string s;
                    char c = GetByte();
                    while(c != 0 && !AtEnd()) { s.push_back(c); c = GetByte(); }
                    return s;
                }
                std::string s;
                for(int i = 0;i < Len;i++) s.push_back(GetByte());
                return s;
            }

            bool GetData(size_t Sz,void* Ptr)
            {
                if(m_Offset + Sz > m_Bytes.size()) return false;
                memcpy(Ptr,&m_Bytes[m_Offset],Sz);
                m_Offset += Sz;
                return true;
            }

            ByteStream* SubData(size_t Sz)
            {
                if(m_Offset + Sz > m_Bytes.size()) return 0;
                ByteStream* s = new ByteStream();
                for(int i = 0;i < Sz;i++) s->WriteByte(GetByte());
                return s;
            }

            int     GetOffset() const       { return m_Offset; }
            void    SetOffset(int Offset)   { m_Offset = Offset; }
            void    Offset(int Offset)      { m_Offset += Offset; }
            size_t  GetSize() const         { return m_Bytes.size(); }
			void*   Ptr()                   { return (m_Bytes.size() == 0) ? nullptr : &m_Bytes[m_Offset]; }
			void	Clear()					{ m_Offset = 0; m_Bytes.clear(); }

            bool AtEnd(int EndOffset = 0) const { return m_Offset + EndOffset >= m_Bytes.size(); }

        protected:
            int m_Offset;
            std::vector<char> m_Bytes;
    };
}

#endif
