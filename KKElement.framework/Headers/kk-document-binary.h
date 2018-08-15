//
//  kk-document-binary.h
//  KKElement
//
//  Created by zhanghailong on 2018/8/7.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef kk_document_binary_h
#define kk_document_binary_h


#if defined(KK_PLATFORM_IOS)

#include <KKElement/kk-document.h>

#else

#include "kk-document.h"

#endif

namespace kk {
    
    class DocumentBinaryObserver : public kk::Object, public DocumentObserver, public kk::script::IObject {
    public:
        DocumentBinaryObserver();
        virtual ~DocumentBinaryObserver();
        virtual void alloc(Document * document,Element * element);
        virtual void root(Document * document,Element * element);
        virtual void set(Document * document,Element * element,ElementKey key,CString value);
        virtual void append(Document * document, Element * element,Element * e);
        virtual void before(Document * document, Element * element,Element * e);
        virtual void after(Document * document, Element * element,Element * e);
        virtual void remove(Document * document, ElementKey elementId);
        virtual void key(Document * document, ElementKey key, CString name);
        
        virtual void encode(Document * document);
        virtual Byte * data();
        virtual size_t length();
        
        virtual kk::CString title();
        virtual void setTitle(kk::CString title);
        
        static size_t decode(Document * document,Byte * data, size_t size,kk::CString title);
        
        virtual duk_ret_t duk_data(duk_context * ctx);
        virtual duk_ret_t duk_encode(duk_context * ctx);
        
        virtual duk_ret_t duk_setTitle(duk_context * ctx);
        virtual duk_ret_t duk_title(duk_context * ctx);
        
        static duk_ret_t duk_decode(duk_context * ctx);
        
        
        
        DEF_SCRIPT_CLASS
        
    protected:
        
        virtual void append(Byte * data, size_t n);
        virtual void append(Byte byte);
        virtual void append(Int32 v);
        virtual void append(Int64 v);
        virtual void append(Boolean v);
        virtual void append(Float v);
        virtual void append(Double v);
        virtual void append(CString v);
        virtual void presize(size_t length);
        
        Byte * _data;
        size_t _length;
        size_t _size;
        
        kk::String _title;
    };
    
}

#endif /* kk_document_binary_h */
