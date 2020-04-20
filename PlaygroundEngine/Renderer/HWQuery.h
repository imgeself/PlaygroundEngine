#pragma once

#include "../Core.h"

enum HWQueryType {
    QUERY_EVENT,
    QUERY_OCCLUSION,
    QUERY_TIMESTAMP,
    QUERY_TIMESTAMP_DISJOINT,
    QUERY_PIPELINE_STATISTICS,
    QUERY_OCCLUSION_PREDICATE
};

enum HWQueryFlags : uint8_t {
    QUERY_MISC_PREDICATEHINT = BIT(1)
};

struct HWQueryDesc {
    HWQueryType queryType;
    uint8_t flags;
};

class HWQuery {
public:
    virtual ~HWQuery() = default;
};

