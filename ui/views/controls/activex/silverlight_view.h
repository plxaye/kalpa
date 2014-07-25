
#ifndef __ui_views_controls_activex_silverlight_view_h__
#define __ui_views_controls_activex_silverlight_view_h__

#include "ui/views/controls/activex/xcp_ctrl.h"

#include "ui/views/controls/activex/ax_host.h"
#include "ui/views/view.h"

namespace views
{

	  // todo(hege),fix visiblechanged,maybe should use nativecontrolhost
    class VIEWS_EXPORT SilverlightView : public IPropertyBag, public IPropertyBag2,
        public View, public AxHostDelegate, public IXcpControlHost2
    {
    public:
        explicit SilverlightView();
        virtual ~SilverlightView();

        IXcpControl* xcp_control()  const { return xcp_control_; }
        bool Play(const std::wstring& url);

        // Overridden from View:
        //virtual bool OnSetCursor(const gfx::Point& p) OVERRIDE;
        virtual bool OnMousePressed(const ui::MouseEvent& event) OVERRIDE;
        virtual bool OnMouseDragged(const ui::MouseEvent& event) OVERRIDE;
        virtual void OnMouseReleased(const ui::MouseEvent& event) OVERRIDE;
        virtual void OnMouseMoved(const ui::MouseEvent& event) OVERRIDE;
        virtual bool OnKeyPressed(const ui::KeyEvent& event) OVERRIDE;
        virtual bool OnKeyReleased(const ui::KeyEvent& event) OVERRIDE;
        //virtual LRESULT OnImeMessages(UINT message, WPARAM w_param,
        //    LPARAM l_param, BOOL* handled) OVERRIDE;

    protected:
        // IUnknown:
        STDMETHOD_(ULONG, AddRef)();
        STDMETHOD_(ULONG, Release)();
        STDMETHOD(QueryInterface)(REFIID iid, void** object);

        // IPropertyBag
        STDMETHOD(Read)(LPCOLESTR pszPropName,
            VARIANT* pVar,
            IErrorLog* pErrorLog);
        STDMETHOD(Write)(LPCOLESTR pszPropName, VARIANT* pVar);

        // IPropertyBag2
        STDMETHOD(Read)(ULONG cProperties,
            PROPBAG2* pPropBag,
            IErrorLog* pErrLog,
            VARIANT* pvarValue,
            HRESULT* phrError);
        STDMETHOD(Write)(ULONG cProperties,
            PROPBAG2* pPropBag,
            VARIANT* pvarValue);
        STDMETHOD(CountProperties)(ULONG* pcProperties);
        STDMETHOD(GetPropertyInfo)(ULONG iProperty,
            ULONG cProperties,
            PROPBAG2* pPropBag,
            ULONG* pcProperties);
        STDMETHOD(LoadObject)(LPCOLESTR pstrName,
            DWORD dwHint,
            IUnknown* pUnkObject,
            IErrorLog* pErrLog);

        // Overridden from View:
        virtual void OnPaint(gfx::Canvas* canvas) OVERRIDE;
        virtual void Layout() OVERRIDE;
        virtual void ViewHierarchyChanged(const ViewHierarchyChangedDetails& details) OVERRIDE;
        virtual void OnFocus() OVERRIDE;
        virtual void OnBlur() OVERRIDE;

        // Overridden from AxHostDelegate:
        virtual HWND GetAxHostWindow() const OVERRIDE;
        virtual void OnAxCreate(AxHost* host) OVERRIDE;
        virtual void OnAxInvalidate(const gfx::Rect& rect);
        virtual HRESULT QueryService(REFGUID guidService,
            REFIID riid, void** ppvObject) OVERRIDE;

        // IXcpControlHost
        STDMETHOD(GetHostOptions)(DWORD* pdwOptions);
        STDMETHOD(NotifyLoaded)();
        STDMETHOD(NotifyError)(BSTR bstrError,
            BSTR bstrSource,
            long nLine,
            long nColumn);
        STDMETHOD(InvokeHandler)(BSTR bstrName,
            VARIANT varArg1,
            VARIANT varArg2,
            VARIANT* pvarResult);
        STDMETHOD(GetBaseUrl)(BSTR* pbstrUrl);
        STDMETHOD(GetNamedSource)(BSTR bstrSourceName,
            BSTR* pbstrSource);
        STDMETHOD(DownloadUrl)(BSTR bstrUrl,
            IXcpControlDownloadCallback* pCallback,
            IStream** ppStream);

        // IXcpControlHost2
        STDMETHOD(GetCustomAppDomain)(IUnknown** ppAppDomain);
        STDMETHOD(GetControlVersion)(UINT* puMajorVersion,
            UINT* puMinorVersion);

    private:
        scoped_ptr<AxHost> ax_host_;
        base::win::ScopedComPtr<IXcpControl> xcp_control_;

        bool initialized_;

        struct PropertyParam
        {
            PropertyParam(const std::wstring& name, const std::wstring& value)
            {
                this->name = name;
                this->value = value;
            }
            PropertyParam() {}
            std::wstring name;
            std::wstring value;
        };
        typedef std::vector<PropertyParam> PropertyParams;
        PropertyParams props_;
    };

} //namespace view

#endif //__ui_views_controls_activex_silverlight_view_h__