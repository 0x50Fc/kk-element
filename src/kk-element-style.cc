//
//  kk-element-style.cc
//  KKElement
//
//  Created by zhanghailong on 2018/8/7.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//


#include "kk-config.h"
#include "kk-element-style.h"

namespace kk {
    
    IMP_SCRIPT_CLASS_BEGIN(&Element::ScriptClass, StyleElement, StyleElement)
    
    static kk::script::Method methods[] = {
        {"addStatus",(kk::script::Function) &StyleElement::duk_addStatus},
        {"removeStatus",(kk::script::Function) &StyleElement::duk_removeStatus},
        {"hasStatus",(kk::script::Function) &StyleElement::duk_hasStatus},
        {"changedStatus",(kk::script::Function) &StyleElement::duk_changedStatus},
    };
    
    kk::script::SetMethod(ctx, -1, methods, sizeof(methods) / sizeof(kk::script::Method));
    
    IMP_SCRIPT_CLASS_END
    
    StyleElement::StyleElement():kk::Element(),_keyInStatus(0),_keyStatus(0) {
        
    }
    
    StyleElement::StyleElement(Document * document,CString name, ElementKey elementId):kk::Element(document,name,elementId) {
        _keyStatus = document->key("status");
        _keyInStatus = document->key("in-status");
    }
    
    CString StyleElement::status() {
        CString v = Element::get(_keyStatus);
        if(v == nullptr) {
            v = Element::get(_keyInStatus);
        }
        return v;
    }
    
    void StyleElement::setStatus(CString status) {
        set(_keyStatus,status);
        changedStatus();
    }
    
    void StyleElement::addStatus(CString status) {
        
        String s(status);
        
        std::set<String> vs;
        
        {
            CString v = Element::get(_keyStatus);
            
            if(v != nullptr) {
                CStringSplit(v, " ", vs);
            }
            
        }
        
        {
            std::set<String>::iterator i = vs.find(s);
            if(i == vs.end()) {
                vs.insert(s);
                String v = CStringJoin(vs, " ");
                Element::set(_keyStatus,v.c_str());
            }
        }
    }
    
    void StyleElement::removeStatus(CString status) {
        String s(status);
        std::set<String> vs;
        
        {
            CString v = Element::get(_keyStatus);
            
            if(v != nullptr) {
                CStringSplit(v, " ", vs);
            }
        }
        {
            std::set<String>::iterator i = vs.find(s);
            if(i != vs.end()) {
                vs.erase(i);
                String v = CStringJoin(vs, " ");
                Element::set(_keyStatus,v.c_str());
            }
        }
    }
    
    Boolean StyleElement::hasStatus(CString status) {
        
        std::set<String> vs;
        
        {
            CString v = Element::get(_keyStatus);
            
            if(v != nullptr) {
                CStringSplit(v, " ", vs);
            }
        }
        
        {
            std::set<String>::iterator i = vs.find(status);
            return i != vs.end();
        }
        
        return false;
    }
    
    void StyleElement::changedKeys(std::set<String>& keys) {

    }
    
    void StyleElement::set(ElementKey key,CString value) {
        Element::set(key, value);
    }
    
    void StyleElement::set(CString key,CString value) {
        Element::set(key, value);
        if(CStringEqual(key, "status") || CStringEqual(key, "in-status")) {
            changedStatus();
        } else if(CStringEqual(key, "style") || CStringHasPrefix(key, "style:")) {
        
            String name = key;
            
            if(CStringHasPrefix(key, "style:")) {
                name = name.substr(6);
            } else {
                name = "";
            }
            
            std::set<String> keys;
            std::map<String,String> & style = this->style(name);
            std::vector<String> items;
            
            CStringSplit(value, ";", items);
            
            std::vector<String>::iterator i = items.begin();
            
            while(i != items.end()) {
                std::vector<String> kv;
                CStringSplit((* i).c_str(), ":", kv);
                if(kv.size() > 1) {
                    String& key = CStringTrim(kv[0]);
                    keys.insert(key);
                    style[key] = CStringTrim(kv[1]);
                }
                i ++;
            }
            
            changedKeys(keys);
            
        } else {
            std::set<String> keys;
            keys.insert(key);
            changedKeys(keys);
        }
    }
    
    CString StyleElement::get(ElementKey key) {
        return Element::get(key);
    }
    
    CString StyleElement::get(CString key) {
        
        CString v = Element::get(key);
        
        if(v == nullptr) {
            
            CString status = this->status();
            
            std::vector<String> vs;
            
            if(status) {
                CStringSplit(status, " ", vs);
            }
            
            vs.push_back("");
            
            std::vector<String>::iterator vi = vs.begin();
            
            while(vi != vs.end()) {
                
                std::map<String,std::map<String,String>>::iterator i = _styles.find(*vi);
                
                if(i != _styles.end()) {
                    std::map<String,String>& attrs = i->second;
                    std::map<String,String>::iterator ii = attrs.find(key);
                    if(ii != attrs.end()) {
                        v = ii->second.c_str();
                        break;
                    }
                }
                
                vi ++;
            }
            
        }
        
        return v;
        
    }
    
    void StyleElement::changedStatus() {
       
        std::set<String> keys;
        
        std::vector<String> vs;
        
        vs.push_back("");
        
        CString value = Element::get(_keyStatus);
        
        if(value != nullptr) {
            CStringSplit(value, " ", vs);
        }
        
        std::vector<String>::iterator vi = vs.begin();
        
        while(vi != vs.end()) {
            std::map<String,std::map<String,String>>::iterator i = _styles.find(*vi);
            if(i != _styles.end()) {
                std::map<String,String> & attrs = i->second;
                std::map<String,String>::iterator ikey = attrs.begin();
                while(ikey != attrs.end()) {
                    keys.insert(ikey->first);
                    ikey ++;
                }
            }
            vi ++;
        }
        
        changedKeys(keys);
        
        {
            Element * e = firstChild();
            
            while(e) {
                
                StyleElement * ee = dynamic_cast<StyleElement *>(e);
                
                if(ee) {
                    CString v = e->get(_keyStatus);
                    if(v == nullptr) {
                        ee->set(_keyInStatus, value);
                    }
                }
                e = e->nextSibling();
            }
        }
    }

    Element * StyleElement::Create(Document * document,CString name, ElementKey elementId) {
        return new StyleElement(document,name,elementId);;
    }
    
    duk_ret_t StyleElement::duk_addStatus(duk_context * ctx) {
        
        int top = duk_get_top(ctx);
        
        if(top > 0 && duk_is_string(ctx, -top)) {
            addStatus(duk_to_string(ctx, -top));
        }
        
        return 0;
    }
    
    duk_ret_t StyleElement::duk_removeStatus(duk_context * ctx) {
        
        int top = duk_get_top(ctx);
        
        if(top > 0 && duk_is_string(ctx, -top)) {
            removeStatus(duk_to_string(ctx, -top));
        }
        
        return 0;
    }
    
    duk_ret_t StyleElement::duk_changedStatus(duk_context * ctx) {
        
        changedStatus();
        
        return 0;
    }
    
    duk_ret_t StyleElement::duk_hasStatus(duk_context * ctx) {
        
        int top = duk_get_top(ctx);
        
        if(top > 0 && duk_is_string(ctx, -top)) {
            duk_push_boolean(ctx, hasStatus(duk_to_string(ctx, -top)));
            return 1;
        }
        
        return 0;
    }
    
}

