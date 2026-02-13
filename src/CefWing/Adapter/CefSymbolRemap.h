#ifndef CEF_SYMBOL_REMAP_H
#define CEF_SYMBOL_REMAP_H

#ifdef __loongarch64__
#include <dlfcn.h>
#include <string>
#include <map>

// 符号重映射表
static std::map<std::string, std::string> SYMBOL_REMAP = {
    // CefV8Value
    {"_ZN10CefV8Value13CreateNullEv", "_ZN10CefV8Value10CreateNullEv"},
    {"_ZN10CefV8Value15CreateUndefinedEv", "_ZN10CefV8Value15CreateUndefinedEv"},
    {"_ZN10CefV8Value13CreateBoolEb", "_ZN10CefV8Value10CreateBoolEb"},
    {"_ZN10CefV8Value12CreateIntEi", "_ZN10CefV8Value9CreateIntEi"},
    {"_ZN10CefV8Value14CreateDoubleEd", "_ZN10CefV8Value12CreateDoubleEd"},
    {"_ZN10CefV8Value13CreateStringERK13CefStringBaseI20CefStringTraitsUTF16E", "_ZN10CefV8Value12CreateStringERK13CefStringBaseI20CefStringTraitsUTF16E"},
    {"_ZN10CefV8Value13CreateArrayEi", "_ZN10CefV8Value11CreateArrayEi"},
    {"_ZN10CefV8Value13CreateObjectE13scoped_refptrI13CefV8AccessorES3_I20CefV8InterceptorE", "_ZN10CefV8Value12CreateObjectE13scoped_refptrI13CefV8AccessorES0_I16CefV8InterceptorE"},
    {"_ZN10CefV8Value13CreateFunctionERK13CefStringBaseI20CefStringTraitsUTF16E13scoped_refptrI13CefV8HandlerE", "_ZN10CefV8Value14CreateFunctionERK13CefStringBaseI20CefStringTraitsUTF16E13scoped_refptrI12CefV8HandlerE"},
    
    // CefProcessMessage
    {"_ZN16CefProcessMessage13CreateERK13CefStringBaseI20CefStringTraitsUTF16E", "_ZN17CefProcessMessage6CreateERK13CefStringBaseI20CefStringTraitsUTF16E"},
    
    // CefCommandLine
    {"_ZN14CefCommandLine17CreateCommandLineEv", "_ZN15CefCommandLine17CreateCommandLineEv"},
    
    // CefValue
    {"_ZN9CefValue6CreateEv", "_ZN8CefValue6CreateEv"},
    
    // CefListValue
    {"_ZN12CefListValue6CreateEv", "_ZN13CefListValue6CreateEv"},
    
    // CefDictionaryValue
    {"_ZN18CefDictionaryValue6CreateEv", "_ZN19CefDictionaryValue6CreateEv"},
    
    // CefV8Context
    {"_ZN13CefV8Context18GetCurrentContextEv", "_ZN12CefV8Context17GetCurrentContextEv"},
    
    // CefExecuteProcess
    {"_ZN11CefExecuteProcessERK11CefMainArgs13scoped_refptrI5CefAppEPv", "_Z17CefExecuteProcessRK11CefMainArgs13scoped_refptrI6CefAppEPv"},
};

class CefSymbolResolver {
private:
    static void* handle;
    static void* get_symbol(const std::string& name) {
        if (!handle) {
            handle = dlopen("libcef.so", RTLD_LAZY | RTLD_GLOBAL);
        }
        
        // 先尝试原始符号
        void* sym = dlsym(handle, name.c_str());
        if (sym) return sym;
        
        // 如果找不到，尝试重映射
        auto it = SYMBOL_REMAP.find(name);
        if (it != SYMBOL_REMAP.end()) {
            sym = dlsym(handle, it->second.c_str());
            if (sym) return sym;
        }
        
        fprintf(stderr, "Failed to find symbol: %s\n", name.c_str());
        return NULL;
    }
    
public:
    template<typename T, typename... Args>
    static auto call(const std::string& name, Args&&... args) {
        typedef T (*Func)(Args...);
        Func f = (Func)get_symbol(name);
        if (f) return f(std::forward<Args>(args)...);
        using ReturnType = decltype(((T*)nullptr)->operator()(std::forward<Args>(args)...));
        return ReturnType();
    }
};

void* CefSymbolResolver::handle = nullptr;

#define CEF_CALL(ret, name, ...) \
    CefSymbolResolver::call<ret>(name, ##__VA_ARGS__)

#endif // __loongarch64__
#endif // CEF_SYMBOL_REMAP_H
