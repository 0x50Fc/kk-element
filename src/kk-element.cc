//
//  kk-element.cpp
//  KKElement
//
//  Created by zhanghailong on 2018/8/7.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//


#include "kk-config.h"
#include "kk-document.h"
#include "kk-element.h"

#if defined(KK_PLATFORM_IOS)

#else

#include "kk-string.h"

#endif

namespace kk {
    
    IMP_SCRIPT_CLASS_BEGIN(&Event::ScriptClass, ElementEvent, ElementEvent)
    
    static kk::script::Property propertys[] = {
        {"cancelBubble",(kk::script::Function) &ElementEvent::duk_isCancelBubble,(kk::script::Function) &ElementEvent::duk_setCancelBubble},
        {"data",(kk::script::Function) &ElementEvent::duk_data,(kk::script::Function) &ElementEvent::duk_setData},
        {"element",(kk::script::Function) &ElementEvent::duk_element,(kk::script::Function) &ElementEvent::duk_setElement},
    };
    
    kk::script::SetProperty(ctx, -1, propertys, sizeof(propertys) / sizeof(kk::script::Property));
    
    IMP_SCRIPT_CLASS_END
    
    ElementEvent::ElementEvent():element(nullptr),cancelBubble(false),data(nullptr) {
        
    }
    
    ElementEvent::ElementEvent(Element * element):element(element),cancelBubble(false),data(nullptr) {
        
    }
    
    duk_ret_t ElementEvent::duk_element(duk_context * ctx) {
        
        Element * e = element.as<Element>();
        
        if(e) {
            kk::script::PushObject(ctx, e);
            return 1;
        }
        
        return 0;
    }
    
    duk_ret_t ElementEvent::duk_setElement(duk_context * ctx) {
        
        int top = duk_get_top(ctx);
        
        if(top >0 && duk_is_object(ctx, -top)) {
            kk::Object * v = kk::script::GetObject(ctx, -top);
            kk::Element * e = dynamic_cast<kk::Element *>(v);
            element = e;
        }
        
        return 0;
    }
    
    duk_ret_t ElementEvent::duk_isCancelBubble(duk_context * ctx) {
        duk_push_boolean(ctx, cancelBubble);
        return 1;
    }
    
    duk_ret_t ElementEvent::duk_setCancelBubble(duk_context * ctx) {
        
        int top = duk_get_top(ctx);
        
        if(top >0 && duk_is_boolean(ctx, -top)) {
            cancelBubble = duk_to_boolean(ctx, -top);
        }
        
        return 0;
    }
    
    duk_ret_t ElementEvent::duk_data(duk_context * ctx) {
        kk::script::PushObject(ctx, data.get());
        return 1;
    }
    
    duk_ret_t ElementEvent::duk_setData(duk_context * ctx) {
        
        int top = duk_get_top(ctx);
        
        if(top >0 && duk_is_object(ctx, - top)) {
            Object * v = kk::script::GetObject(ctx, - top);
            if(v == nullptr) {
                data = new kk::script::Object(kk::script::GetContext(ctx), - top);
            } else {
                data = v;
            }
        }
        
        return 0;
    }
    
    IMP_SCRIPT_CLASS_BEGIN_NOALLOC(&EventEmitter::ScriptClass, Element, Element)
    
    static kk::script::Method methods[] = {
        {"get",(kk::script::Function) &Element::duk_get},
        {"set",(kk::script::Function) &Element::duk_set},
        {"append",(kk::script::Function) &Element::duk_append},
        {"before",(kk::script::Function) &Element::duk_before},
        {"after",(kk::script::Function) &Element::duk_after},
        {"remove",(kk::script::Function) &Element::duk_remove},
        {"appendTo",(kk::script::Function) &Element::duk_appendTo},
        {"beforeTo",(kk::script::Function) &Element::duk_beforeTo},
        {"afterTo",(kk::script::Function) &Element::duk_afterTo},
        {"object",(kk::script::Function) &Element::duk_object},
        {"setObject",(kk::script::Function) &Element::duk_setObject},
        {"toString",(kk::script::Function) &Element::duk_toString},
    };
    
    kk::script::SetMethod(ctx, -1, methods, sizeof(methods) / sizeof(kk::script::Method));
    
    static kk::script::Property propertys[] = {
        {"id",(kk::script::Function) &Element::duk_id,(kk::script::Function) nullptr},
        {"firstChild",(kk::script::Function) &Element::duk_firstChild,(kk::script::Function) nullptr},
        {"lastChild",(kk::script::Function) &Element::duk_lastChild,(kk::script::Function) nullptr},
        {"nextSibling",(kk::script::Function) &Element::duk_nextSibling,(kk::script::Function) nullptr},
        {"prevSibling",(kk::script::Function) &Element::duk_prevSibling,(kk::script::Function) nullptr},
        {"parent",(kk::script::Function) &Element::duk_parent,(kk::script::Function) nullptr},
        {"attributes",(kk::script::Function) &Element::duk_attributes,(kk::script::Function) nullptr},
    };
    
    kk::script::SetProperty(ctx, -1, propertys, sizeof(propertys) / sizeof(kk::script::Property));
    
    
    IMP_SCRIPT_CLASS_END
    
    
    Element::Element(Document * document,CString name, ElementKey elementId)
        :_document(document),_name(name),_elementId(elementId),_depth(0) {
        
    }
    
    Element::~Element() {
        
    }
    
    ElementKey Element::elementId() {
        return _elementId;
    }
    
    CString Element::name() {
        return _name.c_str();
    }
    
    Document * Element::document() {
        return (Document *) _document.get();
    }
    
    
    Element * Element::firstChild() {
        return (Element *) _firstChild.get();
    }
    
    Element * Element::lastChild() {
        return (Element *) _lastChild.get();
    }
    
    Element * Element::nextSibling() {
        return (Element *) _nextSibling.get();
    }
    
    Element * Element::prevSibling() {
        return (Element *) _prevSibling.get();
    }
    
    Element * Element::parent() {
        return (Element *) _parent.get();
    }
    
    void Element::append(Element * element) {
        
        if(element == nullptr) {
            return;
        }
        
        Strong e(element);
        
        element->remove();
        
        Element * lastChild = this->lastChild();
        
        if(lastChild) {
            lastChild->_nextSibling = element;
            element->_prevSibling = lastChild;
            _lastChild = element;
            element->_parent = this;
        } else {
            _firstChild = _lastChild = element;
            element->_parent = this;
        }
        
        {
            Document * v = document();
            
            if(v != nullptr) {
                
                DocumentObserver * observer = v->getObserver();
                
                if(observer) {
                    observer->append(v, this, element);
                }
                
            }
            
        }
        
        onDidAddChildren(element);
        
    }
    
    void Element::before(Element * element) {
        
        if(element == nullptr) {
            return;
        }
        
        Strong e(element);
        
        element->remove();
        
        Element * prevSibling = this->prevSibling();
        Element * parent = this->parent();
        
        if(prevSibling) {
            prevSibling->_nextSibling = element;
            element->_prevSibling = prevSibling;
            element->_nextSibling = this;
            element->_parent = parent;
            _prevSibling = element;
        } else if(parent) {
            element->_nextSibling = this;
            element->_parent = parent;
            _prevSibling = element;
            parent->_firstChild = element;
        }
        
        if(parent) {
            
            {
                Document * v = document();
                
                if(v != nullptr) {
                    
                    DocumentObserver * observer = v->getObserver();
                    
                    if(observer) {
                        observer->before(v, this, element);
                    }
                    
                }
                
            }
            
            parent->onDidAddChildren(element);
        }
    }
    
    void Element::after(Element * element) {
        
        if(element == nullptr){
            return;
        }
        
        Strong e(element);
        
        element->remove();
        
        Element * nextSibling = this->nextSibling();
        Element * parent = this->parent();
        
        if(nextSibling) {
            nextSibling->_prevSibling = element;
            element->_nextSibling = nextSibling;
            element->_prevSibling = this;
            element->_parent = parent;
            _nextSibling = element;
        } else if(parent) {
            element->_prevSibling = this;
            element->_parent = parent;
            _nextSibling = element;
            parent->_lastChild = element;
        }
        
        if(parent) {
            
            {
                Document * v = document();
                
                if(v != nullptr) {
                    
                    DocumentObserver * observer = v->getObserver();
                    
                    if(observer) {
                        observer->after(v, this, element);
                    }
                    
                }
                
            }
            
            parent->onDidAddChildren(element);
        }
    }
    
    void Element::remove() {
        
        Strong e(this);
        
        Element * prevSibling = this->prevSibling();
        Element * nextSibling = this->nextSibling();
        Element * parent = this->parent();
        
        if(prevSibling) {
            parent->onWillRemoveChildren(this);
            prevSibling->_nextSibling = nextSibling;
            if(nextSibling) {
                nextSibling->_prevSibling = prevSibling;
            } else {
                parent->_lastChild = prevSibling;
            }
        } else if(parent) {
            parent->onWillRemoveChildren(this);
            parent->_firstChild = nextSibling;
            if(nextSibling) {
                nextSibling->_prevSibling = (Element *) nullptr;
            } else {
                parent->_lastChild = (Element *) nullptr;
            }
        }
        
        _parent = nullptr;
        _prevSibling = nullptr;
        _nextSibling = nullptr;
        
        if(parent) {
            {
                Document * v = document();
                
                if(v != nullptr) {
                    
                    DocumentObserver * observer = v->getObserver();
                    
                    if(observer) {
                        observer->remove(v, _elementId);
                    }
                    
                }
                
            }
        }
    }
    
    void Element::appendTo(Element * element) {
        if(element != nullptr) {
            element->append(this);
        }
    }
    
    void Element::beforeTo(Element * element) {
        if(element != nullptr) {
            element->before(this);
        }
    }
    
    void Element::afterTo(Element * element) {
        if(element != nullptr) {
            element->after(this);
        }
    }
    
    void Element::onDidAddChildren(Element * element) {
        element->onDidAddToParent(this);
        element->_depth = _depth + 1;
    }
    
    void Element::onWillRemoveChildren(Element * element) {
        element->onWillRemoveFromParent(this);
        element->_depth = 0;
    }
    
    void Element::onDidAddToParent(Element * element) {
        
    }
    
    void Element::onWillRemoveFromParent(Element * element) {
        
    }
    
    void Element::set(ElementKey key,CString value) {
        Document * v = document();
        if(v == nullptr) {
            return;
        }
        set(v->key(key),value);
    }
    
    CString Element::get(CString key) {
        std::map<String,String>::iterator i = _attributes.find(key);
        if(i != _attributes.end()) {
            String &v = i->second;
            return v.c_str();
        }
        return nullptr;
    }
    
    CString Element::get(ElementKey key) {
        Document * v = document();
        if(v == nullptr) {
            return nullptr;
        }
        return get(v->key(key));
    }
    
    void Element::set(CString key,CString value) {
        
        if(value == nullptr) {
            
            std::map<String,String>::iterator i = _attributes.find(key);
            
            if(i != _attributes.end()) {
                _attributes.erase(i);
            }
            
        } else {
            _attributes[key] = value;
        }
        
        {
            Document * v = document();
            
            if(v != nullptr) {
                
                DocumentObserver * observer = v->getObserver();
                
                if(observer) {
                    observer->set(v, this,v->elementKey(key),value);
                }
                
            }
            
        }
    }
    
    std::map<String,String> & Element::attributes() {
        return _attributes;
    }
    
    void Element::emit(String name,Event * event) {
        
        ElementEvent * e = dynamic_cast<ElementEvent *>(event);
        
        if(e && e->element.get() == nullptr)  {
            e->element = this;
        }
        
        EventEmitter::emit(name, event);
        
        if(e && !e->cancelBubble) {
            Element * p = parent();
            if(p) {
                p->emit(name, event);
            } else  {
                Document * doc = document();
                if(doc) {
                    doc->emit(name,event);
                }
            }
        }
        
    }
    
    kk::Boolean Element::hasBubble(String name) {
        
        if(EventEmitter::has(name)) {
            return true;
        }
        
        Element * p = parent();
        
        if(p != nullptr) {
            return p->hasBubble(name);
        }
        
        Document * doc = document();
        
        if(doc != nullptr) {
            return doc->has(name);
        }
        
        return false;
    }
    
    duk_ret_t Element::duk_set(duk_context * ctx) {
        
        int nargs = duk_get_top(ctx);
        
        const char * key = nullptr;
        const char * value = nullptr;
        
        if(nargs > 0 && duk_is_string(ctx, - nargs)) {
            key = duk_to_string(ctx, - nargs);
        }
        
        if(nargs > 1 && duk_is_string(ctx, - nargs + 1)) {
            value = duk_to_string(ctx, - nargs + 1);
        }
        
        if(key) {
            
            if(value) {
                set(key, value);
            } else {
                set(key, nullptr);
            }
        }
        
        return 0;
    }
    
    duk_ret_t Element::duk_get(duk_context * ctx) {
        
        int nargs = duk_get_top(ctx);
        
        const char * key = nullptr;
        
        if(nargs > 0 && duk_is_string(ctx, - nargs)) {
            key = duk_to_string(ctx, - nargs);
        }
        
        
        if(key) {
            
            CString v = get(key);
            
            if(v != nullptr) {
                duk_push_string(ctx, v);
                return 1;
            }
        }
        
        return 0;
    }
    
    duk_ret_t Element::duk_firstChild(duk_context * ctx) {
        Element * e = firstChild();
        if(e) {
            kk::script::PushObject(ctx,e);
            return 1;
        }
        return 0;
    }
    
    duk_ret_t Element::duk_lastChild(duk_context * ctx) {
        Element * e = lastChild();
        if(e) {
            kk::script::PushObject(ctx,e);
            return 1;
        }
        return 0;
    }
    
    duk_ret_t Element::duk_nextSibling(duk_context * ctx) {
        Element * e = nextSibling();
        if(e) {
            kk::script::PushObject(ctx,e);
            return 1;
        }
        return 0;
    }
    
    duk_ret_t Element::duk_prevSibling(duk_context * ctx) {
        Element * e = prevSibling();
        if(e) {
            kk::script::PushObject(ctx,e);
            return 1;
        }
        return 0;
    }
    
    duk_ret_t Element::duk_parent(duk_context * ctx) {
        Element * e = parent();
        if(e) {
            kk::script::PushObject(ctx,e);
            return 1;
        }
        return 0;
    }
    
    duk_ret_t Element::duk_append(duk_context * ctx) {
        int top = duk_get_top(ctx);
        if(top > 0 && duk_is_object(ctx, -top)) {
            kk::Element * e =dynamic_cast<kk::Element *>(kk::script::GetObject(ctx, -top));
            if(e) {
                append(e);
            }
        }
        return 0;
    }
    
    duk_ret_t Element::duk_before(duk_context * ctx) {
        int top = duk_get_top(ctx);
        if(top > 0 && duk_is_object(ctx, -top)) {
            kk::Element * e =dynamic_cast<kk::Element *>(kk::script::GetObject(ctx, -top));
            if(e) {
                before(e);
            }
        }
        return 0;
    }
    
    duk_ret_t Element::duk_after(duk_context * ctx) {
        int top = duk_get_top(ctx);
        if(top > 0 && duk_is_object(ctx, -top)) {
            kk::Element * e =dynamic_cast<kk::Element *>(kk::script::GetObject(ctx, -top));
            if(e) {
                after(e);
            }
        }
        return 0;
    }
    
    duk_ret_t Element::duk_remove(duk_context * ctx) {
        remove();
        return 0;
    }
    
    duk_ret_t Element::duk_appendTo(duk_context * ctx) {
        int top = duk_get_top(ctx);
        if(top > 0 && duk_is_object(ctx, -top)) {
            kk::Element * e =dynamic_cast<kk::Element *>(kk::script::GetObject(ctx, -top));
            if(e) {
                appendTo(e);
            }
        }
        return 0;
    }
    
    duk_ret_t Element::duk_beforeTo(duk_context * ctx) {
        int top = duk_get_top(ctx);
        if(top > 0 && duk_is_object(ctx, -top)) {
            kk::Element * e =dynamic_cast<kk::Element *>(kk::script::GetObject(ctx, -top));
            if(e) {
                beforeTo(e);
            }
        }
        return 0;
    }
    
    duk_ret_t Element::duk_afterTo(duk_context * ctx) {
        int top = duk_get_top(ctx);
        if(top > 0 && duk_is_object(ctx, -top)) {
            kk::Element * e =dynamic_cast<kk::Element *>(kk::script::GetObject(ctx, -top));
            if(e) {
                afterTo(e);
            }
        }
        return 0;
    }
    
    kk::Object * Element::object(CString key) {
        
        std::map<String,Strong>::iterator i = _objects.find(key);
        
        if(i != _objects.end()) {
            return i->second.get();
        }
        
        return nullptr;
    }
    
    void Element::setObject(CString key,kk::Object * object) {
        if(object == nullptr) {
            std::map<String,Strong>::iterator i = _objects.find(key);
            if(i != _objects.end()) {
                _objects.erase(i);
            }
        } else {
            _objects[key] = object;
        }
    }
    
    duk_ret_t Element::duk_object(duk_context * ctx) {
        
        int top = duk_get_top(ctx);
        
        if(top > 0 && duk_is_string(ctx, -top)) {
            kk::Object * v = this->object(duk_to_string(ctx, -top));
            if(v) {
                kk::script::PushObject(ctx, v);
                return 1;
            }
        }
        
        return 0;
    }
    
    duk_ret_t Element::duk_setObject(duk_context * ctx) {
        
        int top = duk_get_top(ctx);
        
        if(top > 1 && duk_is_string(ctx, -top)) {
            CString key = duk_to_string(ctx, -top);
            Strong v = new kk::script::Object(kk::script::GetContext(ctx),-top +1);
            setObject(key, v.get());
        }
        
        return 0;
    }
    
    duk_ret_t Element::duk_attributes(duk_context * ctx) {
        
        duk_push_object(ctx);
        
        std::map<String,String>::iterator i = _attributes.begin();
        
        while(i != _attributes.end()) {
            
            duk_push_string(ctx, i->first.c_str());
            duk_push_string(ctx, i->second.c_str());
            duk_put_prop(ctx, -3);
            
            i ++;
        }
        
        return 1;
    }
    
    String Element::toString() {
        String v;
        
        for(int i=0;i<_depth;i++) {
            v.append("\t");
        }
        
        v.append("<").append(_name);
        
        std::map<String,String>::iterator i = _attributes.begin();
        
        while(i != _attributes.end()) {
            
            if(!kk::CStringHasPrefix(i->first.c_str(), "#")) {
                v.append(" ").append(i->first).append("=\"").append(i->second).append("\"");
            }
            
            i ++;
        }
        
        v.append(">");
        
        Element * e = firstChild();
        
        if(e) {
            
            while(e) {
                
                v.append("\n");
                
                v.append(e->toString());
                
                e = e->nextSibling();
                
            }
            
            v.append("\n");
            
            for(int i=0;i<_depth;i++) {
                v.append("\t");
            }
            
        } else {
            CString vv = get("#text");
            if(vv) {
                v.append(vv);
            }
        }
        
        v.append("</").append(_name).append(">");
        
        return v;
    }
    
    duk_ret_t Element::duk_toString(duk_context * ctx) {
        String v = toString();
        duk_push_lstring(ctx, v.c_str(), v.size());
        return 1;
    }
    
    duk_ret_t Element::duk_id(duk_context * ctx) {
        duk_push_sprintf(ctx, "%lld",_elementId);
        return 1;
    }
    
    
}

