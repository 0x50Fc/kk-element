//
//  kk-element.h
//  KKElement
//
//  Created by zhanghailong on 2018/8/7.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef kk_element_h
#define kk_element_h

#if defined(KK_PLATFORM_IOS)

#include <KKElement/kk-document.h>

#else

#include "kk-document.h"

#endif

#define KK_DEF_ELEMENT_CREATE(isa) public: \
isa(kk::Document * document,kk::CString name, kk::ElementKey elementId); \
static kk::Element * Create(kk::Document * document,kk::CString name, kk::ElementKey elementId);

#define KK_IMP_ELEMENT_CREATE(isa) \
kk::Element * isa::Create(kk::Document * document,kk::CString name, kk::ElementKey elementId) {\
    return new isa(document,name,elementId);\
}

namespace kk {
    
    class ElementEvent : public Event {
    public:
        ElementEvent();
        ElementEvent(Element * element);
        
        virtual duk_ret_t duk_element(duk_context * ctx);
        virtual duk_ret_t duk_setElement(duk_context * ctx);
        virtual duk_ret_t duk_isCancelBubble(duk_context * ctx);
        virtual duk_ret_t duk_setCancelBubble(duk_context * ctx);
        virtual duk_ret_t duk_data(duk_context * ctx);
        virtual duk_ret_t duk_setData(duk_context * ctx);
        
        DEF_SCRIPT_CLASS
        
        Strong data;
        Strong element;
        bool cancelBubble;
        
    };
    
    class Element : public EventEmitter {
    public:
        Element(Document * document,CString name, ElementKey elementId);
        virtual ~Element();
        virtual ElementKey elementId();
        virtual CString name();
        virtual Document * document();
        
        virtual Element * firstChild();
        virtual Element * lastChild();
        virtual Element * nextSibling();
        virtual Element * prevSibling();
        virtual Element * parent();
        virtual void append(Element * element);
        virtual void before(Element * element);
        virtual void after(Element * element);
        virtual void remove();
        virtual void appendTo(Element * element);
        virtual void beforeTo(Element * element);
        virtual void afterTo(Element * element);
        
        virtual CString get(ElementKey key);
        virtual CString get(CString key);
        virtual void set(ElementKey key,CString value);
        virtual void set(CString key,CString value);
        virtual std::map<String,String> & attributes();
        
        
        virtual void emit(String name,Event * event);
        virtual kk::Boolean hasBubble(String name);
        
        virtual kk::Object * object(CString key);
        virtual void setObject(CString key,kk::Object * object);
        
        virtual String toString();
        
        virtual duk_ret_t duk_set(duk_context * ctx);
        virtual duk_ret_t duk_get(duk_context * ctx);
        virtual duk_ret_t duk_firstChild(duk_context * ctx);
        virtual duk_ret_t duk_lastChild(duk_context * ctx);
        virtual duk_ret_t duk_nextSibling(duk_context * ctx);
        virtual duk_ret_t duk_prevSibling(duk_context * ctx);
        virtual duk_ret_t duk_parent(duk_context * ctx);
        virtual duk_ret_t duk_append(duk_context * ctx);
        virtual duk_ret_t duk_before(duk_context * ctx);
        virtual duk_ret_t duk_after(duk_context * ctx);
        virtual duk_ret_t duk_remove(duk_context * ctx);
        virtual duk_ret_t duk_appendTo(duk_context * ctx);
        virtual duk_ret_t duk_beforeTo(duk_context * ctx);
        virtual duk_ret_t duk_afterTo(duk_context * ctx);
    
        virtual duk_ret_t duk_object(duk_context * ctx);
        virtual duk_ret_t duk_setObject(duk_context * ctx);
        
        virtual duk_ret_t duk_toString(duk_context * ctx);
        
        virtual duk_ret_t duk_id(duk_context * ctx);
        
        DEF_SCRIPT_CLASS_NOALLOC
        
    protected:
        virtual void onDidAddChildren(Element * element);
        virtual void onDidAddToParent(Element * element);
        virtual void onWillRemoveChildren(Element * element);
        virtual void onWillRemoveFromParent(Element * element);
        
    protected:
        Weak _document;
        ElementKey _elementId;
        String _name;
        
        Strong _firstChild;
        Strong _lastChild;
        Strong _nextSibling;
        Weak _prevSibling;
        Weak _parent;
        
        std::map<String,String> _attributes;
        std::map<String,Strong> _objects;
        
        Int _depth;
    };
    
    
}


#endif /* kk_element_h */
