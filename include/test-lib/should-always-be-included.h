#pragma once

#ifdef NDEBUG
#error "ARGGGHHHH!!!! NOOOOOOOOO !! DON'T DISABLE ASSERT !!!!!!!!!!!!!!!"
#endif

// Makes cosmopolitan have some features that are sometimes missing (in particular those useful for debugging)
#ifdef __COSMOPOLITAN__
__static_yoink("__die");
#endif
