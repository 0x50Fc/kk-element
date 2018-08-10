//
//  kk-element-style.h
//  KKElement
//
//  Created by zhanghailong on 2018/8/7.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef kk_element_style_h
#define kk_element_style_h


#if defined(KK_PLATFORM_IOS)

#include <KKElement/kk-element.h>

#else

#include "kk-element.h"
#include "kk-string.h"

#endif


namespace kk {
    
    
    class StyleElement : public Element {
    public:
        
        KK_DEF_ELEMENT_CREATE(StyleElement)
        
        virtual std::map<String,String>& style(CString name);
        virtual CString status();
        virtual void setStatus(CString status);
        virtual void addStatus(CString status);
        virtual void removeStatus(CString status);
        virtual Boolean hasStatus(CString status);
        virtual void changedStatus();
        virtual void changedKey(String& key);
        virtual void changedKeys(std::set<String>& keys);
        virtual CString get(CString key);
        virtual CString get(ElementKey key);
        virtual void set(ElementKey key,CString value);
        virtual void set(CString key,CString value);
        
        virtual duk_ret_t duk_addStatus(duk_context * ctx);
        virtual duk_ret_t duk_removeStatus(duk_context * ctx);
        virtual duk_ret_t duk_changedStatus(duk_context * ctx);
        virtual duk_ret_t duk_hasStatus(duk_context * ctx);
        
        
        DEF_SCRIPT_CLASS_NOALLOC
        
    protected:
        virtual void onDidAddChildren(Element * element);
        virtual void change(kk::CString key,CString value);
        std::map<String,std::map<String,String>> _styles;
    };
    
}


#endif /* kk_element_style_h */
