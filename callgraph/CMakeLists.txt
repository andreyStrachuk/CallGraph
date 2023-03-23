add_library(CallGraphPass MODULE
    FunctionCG.cpp
)

target_compile_features(CallGraphPass PRIVATE cxx_range_for cxx_auto_type)

set_target_properties(CallGraphPass PROPERTIES
    COMPILE_FLAGS "-fno-rtti"
)

if(APPLE)
    set_target_properties(CallGraphPass PROPERTIES
        LINK_FLAGS "-undefined dynamic_lookup"
    )
endif(APPLE)
