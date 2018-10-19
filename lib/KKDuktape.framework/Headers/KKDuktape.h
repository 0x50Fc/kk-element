//
//  KKDuktape.h
//  KKDuktape
//
//  Created by zhanghailong on 2018/8/2.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef KKDUKTAPE_H
#define KKDUKTAPE_H

#ifdef DEBUG
#include <KKDuktape/duk_config_debug.h>
#else
#include <KKDuktape/duk_config.h>
#endif
#include <KKDuktape/duktape.h>

#ifdef __OBJC__
#if defined(__cplusplus)
extern "C" {
#endif
 
    void duk_push_NSObject(duk_context * ctx, id object);
    
    id duk_to_NSObject(duk_context * ctx,duk_idx_t idx);
    
#if defined(__cplusplus)
}
#endif

#endif

#endif

