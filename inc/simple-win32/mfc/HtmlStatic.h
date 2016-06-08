#pragma	once

class HtmlStatic : public CStatic {
public:
    HtmlStatic() : m_dTextFormat(0)						{}

public:
    virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

public:
    void						SetHtmlText(const CString& sText, bool bRedraw=true);
    const CString&				GetHtmlText();

    void						SetHtmlFormat(DWORD dFormat, bool bRedraw=true);
    DWORD						GetHtmlFormat();

protected:
    void						DoRedraw();

protected:
    DWORD						m_dTextFormat;
    CString						m_sHtmlText;
};
