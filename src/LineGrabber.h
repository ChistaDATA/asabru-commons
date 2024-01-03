#pragma once
#include <cstring>
#include <string>

class LineGrabber
{
public:
    LineGrabber(char * pbuffer, int size) : m_buffer(pbuffer), m_index(0), m_size(size) { }

    bool isEof() { return m_index >= m_size; }

    std::string getNextLine()
    {
        int buffer_size = 1024;
        char buffer[buffer_size];
        memset(buffer, 0, buffer_size);
        char * ptr = buffer;
        if (m_index >= m_size)
        {
            return {""};
        }
        int curr_buffer_size = 0;
        while (m_index < m_size)
        {
            if (curr_buffer_size > buffer_size) return {""};
            if (m_buffer[m_index] != 13 && m_buffer[m_index] != 10) {
                *ptr++ = m_buffer[m_index++];
                curr_buffer_size++;
            }
            else
            {
                if (m_buffer[m_index] == 13)
                {
                    m_index += 2;
                    curr_buffer_size += 2;
                    break;
                }
                else if (m_buffer[m_index] == 10)
                {
                    m_index++;
                    curr_buffer_size++;
                    break;
                }
            }
        }
        return {buffer};
    }

    std::string getContentTillEof()
    {
        char buffer[1024];
        memset(buffer, 0, 1024);
        char * ptr = buffer;
        if (m_index >= m_size)
        {
            return std::string("");
        }
        while (m_index < m_size)
        {
            *ptr++ = m_buffer[m_index++];
        }
        return std::string(buffer);
    }

private:
    char * m_buffer;
    int m_index;
    int m_size;
};