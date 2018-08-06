#pragma once

//-------------------------------------------------------------------------
class mybstr_t
{
private:
    BSTR m_str = nullptr;
public:
    ~mybstr_t()
    {
        free();
    }
    
    void free()
    {
        if (m_str != nullptr)
        {
            SysFreeString(m_str);
            m_str = nullptr;
        }
    }

    operator bool()
    {
        return m_str != nullptr;
    }

    operator BSTR()
    {
        return m_str;
    }

    void detach(mybstr_t &other)
    {
        if (&other != this)
        {
            other.m_str = m_str;
            m_str = nullptr;
        }
    }

    mybstr_t(): m_str(nullptr)
    {
    }

    mybstr_t(const char *str)
    {
        assign(str);
    }

    void assign(const char *str)
    {
        free();
        int slen = strlen(str);
        int wsize = MultiByteToWideChar(CP_ACP, 0, str, slen, nullptr, 0);
        if (wsize == 0)
            return;

        WCHAR *wstr = new WCHAR[wsize + 1];
        if (wstr == nullptr)
            return;

        if (MultiByteToWideChar(CP_ACP, 0, str, slen, wstr, wsize) == wsize)
        {
            wstr[wsize] = L'\0';
            m_str = SysAllocString(wstr);
        }
    }
};
