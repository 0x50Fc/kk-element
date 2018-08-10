//
//  kk-document.cpp
//  KKElement
//
//  Created by zhanghailong on 2018/8/7.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#include "kk-config.h"
#include "kk-document.h"
#include "kk-element.h"

namespace kk {
    
    IMP_SCRIPT_CLASS_BEGIN(&EventEmitter::ScriptClass, Document, Document)
    
    static kk::script::Method methods[] = {
        {"beginObserver",(kk::script::Function) &Document::duk_beginObserver},
        {"endObserver",(kk::script::Function) &Document::duk_endObserver},
        {"createElement",(kk::script::Function) &Document::duk_createElement},
        {"elementsByName",(kk::script::Function) &Document::duk_elementsByName},
        {"set",(kk::script::Function) &Document::duk_set},
        {"toString",(kk::script::Function) &Document::duk_toString},
    };
    
    kk::script::SetMethod(ctx, -1, methods, sizeof(methods) / sizeof(kk::script::Method));
    
    static kk::script::Property propertys[] = {
        {"rootElement",(kk::script::Function) &Document::duk_rootElement,(kk::script::Function) &Document::duk_setRootElement},
    };
    
    kk::script::SetProperty(ctx, -1, propertys, sizeof(propertys) / sizeof(kk::script::Property));
    
    
    IMP_SCRIPT_CLASS_END
    
    
    Document::Document():_autoKey(0),_defaultCreateElementFunc(nullptr) {
        
    }
    
    Document::~Document() {
        
    }
    
    Element * Document::rootElement() {
        return (Element *) _rootElement.get();
    }
    
    void Document::setRootElement(Element * element) {
        
        assert(element == nullptr || dynamic_cast<Element *>(element) != nullptr);
        
        _rootElement = element;
        
        {
            DocumentObserver * observer = getObserver();
            
            if(observer) {
                observer->root(this, element);
            }
        }
        
    }
    
    Strong Document::element(ElementKey elementId) {
        Strong v;
        std::map<ElementKey,Weak>::iterator i = _elements.find(elementId);
        if(i != _elements.end()) {
            Weak & vv = i->second;
            Element * e = (Element *) vv.get();
            if(e == nullptr) {
                _elements.erase(i);
            } else {
                v = e;
            }
        }
        return v;
    }
    
    Strong Document::createElement(CString name,ElementKey elementId) {
        
        Strong v;
        
        DocumentCreateElementFunc func = nullptr;
        
        std::map<String,DocumentCreateElementFunc>::iterator i = _createElementFuncs.find(name);
        
        if(i != _createElementFuncs.end()) {
            func = i->second;
        }
        
        if(elementId == 0) {
            elementId = ++ _autoKey;
        } else if(elementId > _autoKey) {
            _autoKey = elementId;
        }
        
        if(func) {
            v = (*func)(this,name,elementId);
            assert(v.as<Element>() != nullptr);
        } else if(_defaultCreateElementFunc) {
            v = (*_defaultCreateElementFunc)(this,name,elementId);
            assert(v.as<Element>() != nullptr);
        } else {
            v = new Element(this,name,elementId);
        }
        
        _elements[elementId] = v.get();

        {
            std::map<String,std::list<Weak>>::iterator i = _elementsByName.find(name);
            if(i == _elementsByName.end()) {
                _elementsByName[name] = std::list<Weak>();
                i = _elementsByName.find(name);
            }
            std::list<Weak> & vs = i->second;
            vs.push_back(v.get());
        }
        
        {
            DocumentObserver * observer = getObserver();
            
            if(observer) {
                observer->alloc(this, (Element *) v.get());
            }
        }
        
        return v;
        
    }
    
    Strong Document::createElement(CString name) {
        return createElement(name, 0);
    }
    
    void Document::set(DocumentCreateElementFunc func) {
        _defaultCreateElementFunc = func;
    }
    
    void Document::set(CString name,DocumentCreateElementFunc func) {
        _createElementFuncs[name] = func;
    }
    
    kk::CString Document::key(ElementKey key) {
        std::map<ElementKey,String>::iterator i = _elementKeys.find(key);
        if(i != _elementKeys.end()) {
            return i->second.c_str();
        }
        return nullptr;
    }
    
    ElementKey Document::elementKey(CString name) {
        
        std::map<String,ElementKey>::iterator i = _keys.find(name);
        
        if(i == _keys.end()) {
            
            ElementKey key = ++ _autoKey;
            
            _keys[name] = key;
            _elementKeys[key] = name;
            
            {
                DocumentObserver * observer = getObserver();
                
                if(observer) {
                    observer->key(this, key,name);
                }
            }
            
            return key;
        }
        
        return i->second;
    }
    
    void Document::beginObserver(DocumentObserver * observer) {
        _observers.push_back(observer);
    }
    
    DocumentObserver * Document::getObserver() {
        if(_observers.empty()) {
            return nullptr;
        }
        return _observers.back();
    }
    
    void Document::endObserver() {
        _observers.pop_back();
    }
    
    duk_ret_t Document::duk_rootElement(duk_context * ctx) {
        
        kk::script::PushObject(ctx, _rootElement.get());
        
        return 1;
    }
    
    duk_ret_t Document::duk_setRootElement(duk_context * ctx) {
        
        Element * e = nullptr;
        
        int top = duk_get_top(ctx);
        
        if(top >0  && duk_is_object(ctx, -top)) {
            kk::Object * v = kk::script::GetObject(ctx, -top);
            if(v != nullptr) {
                e = dynamic_cast<Element *>(v);
            }
        }
        
        setRootElement(e);
        
        return 0;
    }
    
    duk_ret_t Document::duk_createElement(duk_context * ctx) {
        
        CString name = nullptr;
        
        int top = duk_get_top(ctx);
        
        if(top >0  && duk_is_string(ctx, -top)) {
            name = duk_to_string(ctx, -top);
        }
        
        if(name) {
            Strong v = createElement(name);
            kk::script::PushObject(ctx, v.get());
            return 1;
        }
        
        return 0;
    }
    
    duk_ret_t Document::duk_beginObserver(duk_context * ctx) {
        
        DocumentObserver * e = nullptr;
        
        int top = duk_get_top(ctx);
        
        if(top >0  && duk_is_object(ctx, -top)) {
            kk::Object * v = kk::script::GetObject(ctx, -top);
            if(v != nullptr) {
                e = dynamic_cast<DocumentObserver *>(v);
            }
        }
        
        beginObserver(e);
        
        return 0;
    }
    
    duk_ret_t Document::duk_endObserver(duk_context * ctx) {
        
        endObserver();
        
        return 0;
    }
    
    static std::map<String,DocumentCreateElementFunc> gLibrary;
    
    void Document::set(CString name,CString library) {
        
        std::map<String,DocumentCreateElementFunc>::iterator i = gLibrary.find(library);
        
        if(i != gLibrary.end()) {
            set(name,i->second);
        }
    }
    
    duk_ret_t Document::duk_set(duk_context * ctx) {
        
        CString name = nullptr;
        CString library = nullptr;
        
        int top = duk_get_top(ctx);
        
        if(top >1 && duk_is_string(ctx, -top) && duk_is_string(ctx, -top + 1)) {
            name = duk_to_string(ctx, -top);
            library = duk_to_string(ctx, -top +1);
        }
        
        if(name && library) {
            set(name,library);
        }
        
        return 0;
    }

    void Document::library(CString name,DocumentCreateElementFunc func) {
        gLibrary[name] = func;
    }
    
    void Document::set(CString name,ElementKey key) {
        if(key > _autoKey) {
            _autoKey = key;
        }
        _keys[name] = key;
        _elementKeys[key] = name;
    }
    
    std::map<ElementKey,String> & Document::elementKeys() {
        return _elementKeys;
    }
    
    void Document::elementsByName(CString name,std::list<Strong> & elements) {
        std::map<String,std::list<Weak>>::iterator i = _elementsByName.find(name);
        if(i != _elementsByName.end()) {
            std::list<Weak> & vs = i->second;
            std::list<Weak>::iterator n = vs.begin();
            while(n != vs.end()) {
                Weak & v = *n;
                Element * e = (Element *) v.get();
                if(e == nullptr) {
                    n = vs.erase(n);
                } else {
                    elements.push_back(e);
                    n ++;
                }
            }
        }
    }
   
    duk_ret_t Document::duk_elementsByName(duk_context * ctx) {
        
        int top = duk_get_top(ctx);
        
        if(top > 0 && duk_is_string(ctx, -top)) {
            
            CString name = duk_to_string(ctx, -top);
            
            duk_push_array(ctx);
            
            duk_int_t idx = 0;
            
            std::map<String,std::list<Weak>>::iterator i = _elementsByName.find(name);
            
            if(i != _elementsByName.end()) {
                std::list<Weak> & vs = i->second;
                std::list<Weak>::iterator n = vs.begin();
                while(n != vs.end()) {
                    Weak & v = *n;
                    Element * e = (Element *) v.get();
                    if(e == nullptr) {
                        n = vs.erase(n);
                    } else {
                        kk::script::PushObject(ctx, e);
                        duk_put_prop_index(ctx, -2, idx);
                        idx ++;
                        n ++;
                    }
                }
            }
            
            return 1;
            
        }
        
        return 0;
    }
    
    String Document::toString() {
        String v;
        
        Element * e = rootElement();
        
        if(e) {
            return e->toString();
        }
        
        return v;
    }
    
    duk_ret_t Document::duk_toString(duk_context * ctx) {
        String v = toString();
        duk_push_lstring(ctx, v.c_str(), v.size());
        return 1;
    }
    
}


