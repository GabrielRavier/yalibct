#pragma once

#ifdef NDEBUG
#error "ARGGGHHHH!!!! NOOOOOOOOO !! DON'T DISABLE ASSERT !!!!!!!!!!!!!!!"
#endif

// Makes cosmopolitan have some features that are sometimes missing (in particular those useful for debugging)
#ifdef __COSMOPOLITAN__
STATIC_YOINK("__fmt_dtoa");
STATIC_YOINK("__die");
STATIC_YOINK("__zipos_stat");
STATIC_YOINK("__zipos_get");
STATIC_YOINK("__zipos_parseuri");
STATIC_YOINK("PrintBacktraceUsingSymbols");
STATIC_YOINK("ShowBacktrace");
STATIC_YOINK("malloc_inspect_all");
STATIC_YOINK("GetSymbolByAddr");
STATIC_YOINK("ShowCrashReports");
#endif
