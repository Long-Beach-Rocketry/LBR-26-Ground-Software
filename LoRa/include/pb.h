/* Minimal nanopb stub for proto-less dev environment */
/* This stub provides only the essential types and functions needed for TelemetryMessage */

#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>

/* Minimal pb_field_t and related types */
typedef std::uint8_t pb_type_t;
typedef std::size_t pb_size_t;

#define PB_LTYPE_FIXED32  0x04
#define PB_LTYPE_NONE     0x00

typedef struct {
    std::uint32_t first;
    std::uint32_t tag;
    pb_type_t type;
    std::uint16_t offset;
    void *pData;
    void *pSize;
    int64_t long_val;
} pb_field_t;

typedef struct {
    std::uint32_t field_count;
    const pb_field_t *fields;
} pb_msgdesc_t;

/* Stream definitions */
typedef struct {
    const std::uint8_t *buf;
    std::size_t bufsize;
    std::size_t pos;
} pb_istream_t;

/* Minimal pb_decode stub */
inline bool pb_decode(pb_istream_t *stream, const pb_field_t *fields, void *dest_struct) {
    if (!stream || !fields || !dest_struct) return false;
    
    // Simplified: copy bytes directly into struct (little-endian uint32 fields)
    // This assumes TelemetryMessage contains 4 x uint32_t in sequence
    if (stream->bufsize < 16) return false;  // Need at least 16 bytes for 4 uint32s
    
    std::uint32_t *fields_ptr = static_cast<std::uint32_t*>(dest_struct);
    for (int i = 0; i < 4; ++i) {
        std::uint32_t val = 0;
        if (stream->pos + 4 <= stream->bufsize) {
            std::memcpy(&val, stream->buf + stream->pos, 4);
            fields_ptr[i] = val;
            stream->pos += 4;
        }
    }
    return true;
}

/* pb_istream_t constructor stub */
inline pb_istream_t pb_istream_from_buffer(const std::uint8_t *buffer, std::size_t size) {
    pb_istream_t stream = {buffer, size, 0};
    return stream;
}

/* pb_common.h stub */
namespace {
    template<typename T>
    inline std::size_t offsetof_impl(T *p, void *member) {
        return (std::size_t)member - (std::size_t)p;
    }
}

#endif  // PB_H_STUB
