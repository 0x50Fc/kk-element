//
//  kk-document-binary.cc
//  KKElement
//
//  Created by zhanghailong on 2018/8/7.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#include "kk-config.h"
#include "kk-document-binary.h"
#include "kk-element.h"

#if defined(KK_PLATFORM_IOS)

#include <KKObject/kk-bio.h>

#else

#include "kk-bio.h"

#endif

#define BUF_EX_SIZE 40960

namespace kk {
    
    static char TAG[] = {'K','K',0x00,0x00};
    
    IMP_SCRIPT_CLASS_BEGIN(nullptr, DocumentBinaryObserver, DocumentBinaryObserver)
    
    static kk::script::Method methods[] = {
        {"data",(kk::script::Function) &DocumentBinaryObserver::duk_data},
    };
    
    kk::script::SetMethod(ctx, -1, methods, sizeof(methods) / sizeof(kk::script::Method));
    
    duk_push_c_function(ctx, DocumentBinaryObserver::duk_decode, 2);
    duk_put_prop_string(ctx, -2, "decode");
    
    IMP_SCRIPT_CLASS_END
    
    
    DocumentBinaryObserver::DocumentBinaryObserver():_data(nullptr),_size(0),_length(0) {
        
    }
    
    DocumentBinaryObserver::~DocumentBinaryObserver() {
        if(_data) {
            free(_data);
        }
    }
    
    void DocumentBinaryObserver::alloc(Document * document,Element * element) {
        append((Byte)DocumentObserverTypeAlloc);
        append((Int64)element->elementId());
        append((CString)element->name());
    }
    
    void DocumentBinaryObserver::root(Document * document,Element * element) {
        append((Byte)DocumentObserverTypeRoot);
        if(element == nullptr ){
            append((Int64) 0);
        } else {
            append((Int64) element->elementId());
        }
    }
    
    void DocumentBinaryObserver::set(Document * document,Element * element,ElementKey key,CString value) {
        append((Byte)DocumentObserverTypeSet);
        append((Int64)element->elementId());
        append((Int64)key);
        append((CString)value);
    }
    
    void DocumentBinaryObserver::append(Document * document, Element * element,Element * e) {
        append((Byte)DocumentObserverTypeAppend);
        append((Int64)element->elementId());
        append((Int64)e->elementId());
    }
    
    void DocumentBinaryObserver::before(Document * document, Element * element,Element * e) {
        append((Byte)DocumentObserverTypeBefore);
        append((Int64)element->elementId());
        append((Int64)e->elementId());
    }
    
    void DocumentBinaryObserver::after(Document * document, Element * element,Element * e) {
        append((Byte)DocumentObserverTypeAfter);
        append((Int64)element->elementId());
        append((Int64)e->elementId());
    }
    
    void DocumentBinaryObserver::remove(Document * document, ElementKey elementId) {
        append((Byte)DocumentObserverTypeRemove);
        append((Int64)elementId);
    }
    
    void DocumentBinaryObserver::key(Document * document, ElementKey key, CString name) {
        append((Byte)DocumentObserverTypeKey);
        append((Int64)key);
        append((CString)name);
    }
    
    static void encodeAttributes(DocumentBinaryObserver * observer,Document * document, Element * element) {
    
        std::map<String,String> & attrs = element->attributes();
        std::map<String,String>::iterator i = attrs.begin();
        
        while(i != attrs.end()) {
            observer->set(document, element, document->elementKey(i->first.c_str()), i->second.c_str());
            i ++;
        }
        
    }
    
    static void encodeElement(DocumentBinaryObserver * observer,Document * document, Element * element) {
        
        observer->alloc(document, element);
        
        encodeAttributes(observer,document,element);
        
        Element * p = element->firstChild();
        
        while(p) {
            encodeElement(observer, document, p);
            p = p->nextSibling();
        }
        
    }
    
    void DocumentBinaryObserver::encode(Document * document) {
        
        {
            std::map<ElementKey,String> & keys = document->elementKeys();
            std::map<ElementKey,String>::iterator i = keys.begin();
            while(i != keys.end()) {
                this->key(document, i->first, i->second.c_str());
            }
        }
        
        {
            Element * p = document->rootElement();
            
            if(p != nullptr) {
                encodeElement(this,document,p);
            }
            
            this->root(document, p);
            
        }
        
    }
    
    Byte * DocumentBinaryObserver::data() {
        return _data;
    }
    
    size_t DocumentBinaryObserver::length() {
        return _length;
    }
    
    void DocumentBinaryObserver::append(Byte * data, size_t n) {
       
        presize(n);
        
        memcpy(_data + _length, data, n);
        
        _length += n;
        
    }
    
    void DocumentBinaryObserver::append(Byte byte) {
        append(&byte, 1);
    }
    
    void DocumentBinaryObserver::append(Int32 v) {
        presize(Bio::Int32_Size);
        _length += Bio::encode(v, _data + _length, _size - _length);
    }
    
    void DocumentBinaryObserver::append(Int64 v) {
        presize(Bio::Int64_Size);
        _length += Bio::encode(v, _data + _length, _size - _length);
    }
    
    void DocumentBinaryObserver::append(Boolean v) {
        presize(Bio::Boolean_Size);
        _length += Bio::encode(v, _data + _length, _size - _length);
    }
    
    void DocumentBinaryObserver::append(Float v) {
        presize(Bio::Float_Size);
        _length += Bio::encode(v, _data + _length, _size - _length);
    }
    
    void DocumentBinaryObserver::append(Double v) {
        presize(Bio::Double_Size);
        _length += Bio::encode(v, _data + _length, _size - _length);
    }
    
    void DocumentBinaryObserver::append(CString v) {
        
        size_t n = v == nullptr ? 0 : strlen(v) + 1;
        
        presize(Bio::Int32_Size + n);
        
        _length += Bio::encode((Int32) n, _data + _length, _size - _length);
        
        if(v != nullptr) {
            append((Byte *) v, n);
        }
    
    }
    
    void DocumentBinaryObserver::presize(size_t length) {
        if(_length + length > _size) {
            _size = MAX(sizeof(TAG) + _length + length,_size +BUF_EX_SIZE);
            if(_data == nullptr) {
                _data = (Byte *) malloc(_size);
                memcpy(_data, TAG, sizeof(TAG));
                _length = sizeof(TAG);
            } else {
                _data = (Byte *) realloc(_data, _size);
            }
        }
    }
    
    size_t DocumentBinaryObserver::decode(Document * document,Byte * data, size_t size) {
        
        std::map<ElementKey,Strong> elements;
    
        size_t n = 0;
        
        if(size >= sizeof(TAG)) {
            
            if(memcmp(data, TAG, sizeof(TAG)) != 0) {
                kk::Log("[DOCUMENT] [BINARY] [DECODE] [ERROR] TAG");
                return 0;
            }
            
            n += sizeof(TAG);
        }
        
        while(n < size) {
            
            Byte v = 0;
            
            n += Bio::decode(&v, data + n, size - n);
            
            if(v == DocumentObserverTypeAlloc ) {
                
                if(size - n >= Bio::Int64_Size + Bio::Int32_Size) {
                    
                    Int64 elementId = 0;
                    Int32 length = 0;
                    n += Bio::decode(& elementId, data + n, size - n);
                    n += Bio::decode(& length, data + n, size - n);
                    
                    if(elementId && length > 0) {
                        Strong v = document->createElement(data + n, elementId);
                        elements[elementId] = v.get();
                    } else {
                        kk::Log("[DOCUMENT] [BINARY] [DECODE] [ERROR] [ALLOC]");
                        break;
                    }
                    
                    n += length;
                    
                } else {
                    break;
                }
            } else if(v == DocumentObserverTypeRoot) {
                
                if(size - n >= Bio::Int64_Size) {
                    
                    Int64 elementId = 0;
                    
                    n += Bio::decode(& elementId, data + n, size - n);
                    
                    Strong e = document->element(elementId);
     
                    document->setRootElement((Element *) e.get());
                    
                } else {
                    break;
                }
                
            } else if(v == DocumentObserverTypeSet) {
                
                if(size - n >= Bio::Int64_Size + Bio::Int64_Size + Bio::Int32_Size) {
                    
                    Int64 elementId = 0;
                    Int64 key = 0;
                    Int32 length = 0;
                    
                    n += Bio::decode(& elementId, data + n, size - n);
                    n += Bio::decode(& key, data + n, size - n);
                    n += Bio::decode(& length, data + n, size - n);
                    
                    Strong e = document->element(elementId);
                    Element * element = (Element *) e.get();
                    
                    if(element != nullptr) {
                    
                        element->set(key, length == 0 ? nullptr : data + n);
                        
                    } else {
                        kk::Log("[DOCUMENT] [BINARY] [DECODE] [ERROR] [SET]");
                        break;
                    }
                    
                    n += length;
                    
                } else {
                    break;
                }
            } else if(v == DocumentObserverTypeAppend) {
                
                if(size - n >= Bio::Int64_Size + Bio::Int64_Size ) {
                    
                    Int64 elementId = 0;
                    Int64 eid = 0;
                    
                    n += Bio::decode(& elementId, data + n, size - n);
                    n += Bio::decode(& eid, data + n, size - n);
                    
                    Strong e = document->element(elementId);
                    Element * element = (Element *) e.get();
                    
                    if(element != nullptr) {
                        
                        Strong ee = document->element(eid);
                        Element * el = (Element *) ee.get();
                        
                        if(el != nullptr) {
                            element->append(el);
                        }
                        
                    } else {
                        kk::Log("[DOCUMENT] [BINARY] [DECODE] [ERROR] [APPEND]");
                        break;
                    }
                    
                } else {
                    break;
                }
            } else if(v == DocumentObserverTypeBefore) {
                
                if(size - n >= Bio::Int64_Size + Bio::Int64_Size ) {
                    
                    Int64 elementId = 0;
                    Int64 eid = 0;
                    
                    n += Bio::decode(& elementId, data + n, size - n);
                    n += Bio::decode(& eid, data + n, size - n);
                    
                    Strong e = document->element(elementId);
                    Element * element = (Element *) e.get();
                    
                    if(element != nullptr) {
                        
                        Strong ee = document->element(eid);
                        Element * el = (Element *) ee.get();
                        
                        if(el != nullptr) {
                            element->before(el);
                        }
                        
                    } else {
                        kk::Log("[DOCUMENT] [BINARY] [DECODE] [ERROR] [BEFORE]");
                        break;
                    }
                    
                } else {
                    break;
                }
            } else if(v == DocumentObserverTypeAfter) {
                
                if(size - n >= Bio::Int64_Size + Bio::Int64_Size ) {
                    
                    Int64 elementId = 0;
                    Int64 eid = 0;
                    
                    n += Bio::decode(& elementId, data + n, size - n);
                    n += Bio::decode(& eid, data + n, size - n);
                    
                    Strong e = document->element(elementId);
                    Element * element = (Element *) e.get();
                    
                    if(element != nullptr) {
                        
                        Strong ee = document->element(eid);
                        Element * el = (Element *) ee.get();
                        
                        if(el != nullptr) {
                            element->after(el);
                        }
                        
                    } else {
                        kk::Log("[DOCUMENT] [BINARY] [DECODE] [ERROR] [AFTER]");
                        break;
                    }
                    
                } else {
                    break;
                }
            } else if(v == DocumentObserverTypeRemove) {
                
                if(size - n >= Bio::Int64_Size) {
                    
                    Int64 elementId = 0;
                    
                    n += Bio::decode(& elementId, data + n, size - n);
                    
                    Strong e = document->element(elementId);
                    Element * element = (Element *) e.get();
                    
                    if(element != nullptr) {
                        
                        element->remove();
                        
                    } else {
                        kk::Log("[DOCUMENT] [BINARY] [DECODE] [ERROR] [REMOVE]");
                        break;
                    }
                    
                } else {
                    break;
                }
            } else if(v == DocumentObserverTypeKey) {
                
                if(size - n >= Bio::Int64_Size + Bio::Int32_Size) {
                    
                    Int64 key = 0;
                    Int32 length = 0;
                    n += Bio::decode(& key, data + n, size - n);
                    n += Bio::decode(& length, data + n, size - n);
                    
                    if(key && length > 0) {
                        document->set((CString) (data + n), key);
                    } else {
                        kk::Log("[DOCUMENT] [BINARY] [DECODE] [ERROR] [KEY]");
                        break;
                    }
                    
                    n += length;
                    
                } else {
                    break;
                }
                
            } else {

                kk::Log("[DOCUMENT] [BINARY] [DECODE] [ERROR] [TYPE]");
                break;
            }
            
            
        }
        
        return n;
    }
    
    
    duk_ret_t DocumentBinaryObserver::duk_data(duk_context * ctx) {
        
        if(_length > 0) {
            
            void * d = duk_push_fixed_buffer(ctx, _length);
    
            memcpy(d, _data, _length);
            
            duk_push_buffer_object(ctx, -1, 0, _length, DUK_BUFOBJ_UINT8ARRAY);
            
            duk_remove(ctx, -2);
            
            return 1;
            
        }
        
        return 0;
    }
    
    duk_ret_t DocumentBinaryObserver::duk_decode(duk_context * ctx) {
        
        int top = duk_get_top(ctx);
        
        if(top > 1 && duk_is_object(ctx, -top) && duk_is_buffer_data(ctx, - top + 1)) {
            
            kk::Object * v = kk::script::GetObject(ctx, -top);
            
            if(v) {
                
                Document * doc = dynamic_cast<Document *>(v);
                
                if(doc) {
                    
                    duk_size_t n;
                    Byte * bytes = (Byte *) duk_get_buffer_data(ctx, - top + 1, &n);
                    
                    DocumentBinaryObserver::decode(doc, bytes, n);
                    
                }
                
            }
        }
        
        return 0;
        
    }
    
    duk_ret_t DocumentBinaryObserver::duk_encode(duk_context * ctx) {
        
        int top = duk_get_top(ctx);
        
        if(top > 0 && duk_is_object(ctx, -top) ) {
            
            kk::Object * v = kk::script::GetObject(ctx, -top);
            
            if(v) {
                
                Document * doc = dynamic_cast<Document *>(v);
                
                if(doc) {
                    
                    encode(doc);
                    
                }
                
            }
        }
        
        return 0;
        
    }
    
    
}
