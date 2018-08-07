//
//  kk-document.h
//  KKElement
//
//  Created by zhanghailong on 2018/8/7.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef kk_document_h
#define kk_document_h

#if defined(KK_PLATFORM_IOS)

#include <KKObject/KKObject.h>

#else

#include "kk-event.h"

#endif

#include <list>

namespace kk {
    
    typedef Uint64 ElementKey;
    
    class Element;
    class Document;
    
    enum DocumentObserverType {
        DocumentObserverTypeAlloc,
        DocumentObserverTypeRoot,
        DocumentObserverTypeSet,
        DocumentObserverTypeAppend,
        DocumentObserverTypeBefore,
        DocumentObserverTypeAfter,
        DocumentObserverTypeRemove,
        DocumentObserverTypeKey
    };
    
    class DocumentObserver {
    public:
        virtual void alloc(Document * document,Element * element) = 0;
        virtual void root(Document * document,Element * element) = 0;
        virtual void set(Document * document,Element * element,ElementKey key,CString value) = 0;
        virtual void append(Document * document, Element * element,Element * e) = 0;
        virtual void before(Document * document, Element * element,Element * e) = 0;
        virtual void after(Document * document, Element * element,Element * e) = 0;
        virtual void remove(Document * document, ElementKey elementId) = 0;
        virtual void key(Document * document, ElementKey key, CString name) = 0;
    };
    
    typedef Element * (* DocumentCreateElementFunc)(Document * document,CString name, ElementKey elementId);
    
    class Document : public EventEmitter {
    public:
        Document();
        virtual ~Document();
        virtual Element * rootElement();
        virtual void setRootElement(Element * element);
        virtual Strong createElement(CString name);
        virtual Strong createElement(CString name,ElementKey elementId);
        virtual void set(CString name,DocumentCreateElementFunc func);
        virtual void set(CString name,CString library);
        virtual void set(CString name,ElementKey key);
        virtual ElementKey key(CString name);
        virtual void beginObserver(DocumentObserver * observer);
        virtual DocumentObserver * getObserver();
        virtual void endObserver();
        
        virtual duk_ret_t duk_rootElement(duk_context * ctx);
        virtual duk_ret_t duk_setRootElement(duk_context * ctx);
        virtual duk_ret_t duk_createElement(duk_context * ctx);
        virtual duk_ret_t duk_beginObserver(duk_context * ctx);
        virtual duk_ret_t duk_endObserver(duk_context * ctx);
        virtual duk_ret_t duk_set(duk_context * ctx);
        
        static void library(CString name,DocumentCreateElementFunc func);
        
        DEF_SCRIPT_CLASS
    protected:
        ElementKey _autoKey;
        Strong _rootElement;
        std::map<String,DocumentCreateElementFunc> _createElementFuncs;
        std::map<String,ElementKey> _elementKeys;
        std::list<DocumentObserver *> _observers;
    };
    
    
}

#endif /* kk_document_h */
