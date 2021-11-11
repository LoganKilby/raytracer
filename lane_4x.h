/* date = October 27th 2021 0:44 pm */

#ifndef LANE_4X_H
#define LANE_4X_H

struct lane_f32
{
    __m128 v;
    
    lane_f32 &operator=(f32 A);
};

struct lane_f64
{
    __m128d v;
};

struct lane_v3
{
    lane_f32 x;
    lane_f32 y;
    lane_f32 z;
};

struct lane_dv3
{
    lane_f64 x;
    lane_f64 y;
    lane_f64 z;
};

struct lane_u32
{
    __m128i v;
    lane_u32 &operator=(u32 A);
};

internal lane_u32
operator<(lane_f32 a, lane_f32 b)
{
    lane_u32 result;
    
    result.v = _mm_castps_si128(_mm_cmplt_ps(a.v, b.v));
    
    return(result);
}

internal lane_u32
operator<=(lane_f32 a, lane_f32 b)
{
    lane_u32 result;
    
    result.v = _mm_castps_si128(_mm_cmple_ps(a.v, b.v));
    
    return(result);
}

internal lane_u32
operator>(lane_f32 a, lane_f32 b)
{
    lane_u32 result;
    
    result.v = _mm_castps_si128(_mm_cmpgt_ps(a.v, b.v));
    
    return(result);
}

internal lane_u32
operator>=(lane_f32 a, lane_f32 b)
{
    lane_u32 result;
    
    result.v = _mm_castps_si128(_mm_cmpge_ps(a.v, b.v));
    
    return(result);
}

internal lane_u32
operator==(lane_f32 a, lane_f32 b)
{
    lane_u32 result;
    
    result.v = _mm_castps_si128(_mm_cmpeq_ps(a.v, b.v));
    
    return(result);
}

internal lane_u32
operator==(lane_u32 a, lane_u32 b)
{
    lane_u32 result;
    
    result.v = _mm_cmpeq_epi32(a.v, b.v);
    
    return(result);
}

internal lane_u32
operator!=(lane_f32 a, lane_f32 b)
{
    lane_u32 result;
    
    result.v = _mm_castps_si128(_mm_cmpneq_ps(a.v, b.v));
    
    return(result);
}

internal lane_u32
operator!=(lane_u32 a, lane_u32 b)
{
    lane_u32 result;
    
    // TODO: What's the most efficient way to invert this comparison?
    result.v = _mm_xor_si128(_mm_cmpeq_epi32(a.v, b.v), _mm_set1_epi32(0xFFFFFFFF));
    
    return(result);
}

internal lane_u32
operator^(lane_u32 a, lane_u32 b)
{
    lane_u32 result;
    
    result.v = _mm_xor_si128(a.v, b.v);
    
    return(result);
}

internal lane_u32
operator&(lane_u32 a, lane_u32 b)
{
    lane_u32 result;
    
    result.v = _mm_and_si128(a.v, b.v);
    
    return(result);
}

internal lane_f32
operator&(lane_u32 a, lane_f32 b)
{
    lane_f32 result;
    
    result.v = _mm_and_ps(_mm_castsi128_ps(a.v), b.v);
    
    return(result);
}

internal lane_u32
and_not(lane_u32 a, lane_u32 b)
{
    lane_u32 result;
    
    result.v = _mm_andnot_si128(a.v, b.v);
    
    return(result);
}

internal lane_u32
operator|(lane_u32 a, lane_u32 b)
{
    lane_u32 result;
    
    result.v = _mm_or_si128(a.v, b.v);
    
    return(result);
}

internal lane_u32
operator<<(lane_u32 a, u32 Shift)
{
    lane_u32 result;
    
    result.v = _mm_slli_epi32(a.v, Shift);
    
    return(result);
}

internal lane_u32
operator>>(lane_u32 a, u32 Shift)
{
    lane_u32 result;
    
    result.v = _mm_srli_epi32(a.v, Shift);
    
    return(result);
}

internal lane_f32
operator+(lane_f32 a, lane_f32 b)
{
    lane_f32 result;
    
    result.v = _mm_add_ps(a.v, b.v);
    
    return(result);
}

internal lane_u32
operator+(lane_u32 a, lane_u32 b)
{
    lane_u32 result;
    
    result.v = _mm_add_epi32(a.v, b.v);
    
    return(result);
}

internal lane_f32
operator-(lane_f32 a, lane_f32 b)
{
    lane_f32 result;
    
    result.v = _mm_sub_ps(a.v, b.v);
    
    return(result);
}

internal lane_f64
operator-(lane_f64 a, lane_f64 b)
{
    lane_f64 result;
    
    result.v = _mm_sub_pd(a.v, b.v);
    
    return(result);
}

internal lane_f32
operator*(lane_f32 a, lane_f32 b)
{
    lane_f32 result;
    
    result.v = _mm_mul_ps(a.v, b.v);
    
    return(result);
}

internal lane_f64
operator*(lane_f64 a, lane_f64 b)
{
    lane_f64 result;
    
    result.v = _mm_mul_pd(a.v, b.v);
    
    return(result);
}

internal lane_f32
operator/(lane_f32 a, lane_f32 b)
{
    lane_f32 result;
    
    result.v = _mm_div_ps(a.v, b.v);
    
    return(result);
}

internal lane_f64
cast_lane_f64(lane_f32 a)
{
    lane_f64 result;
    
    result.v = _mm_castps_pd(a.v);
    
    return result;
}

internal lane_f32
cast_lane_f32(lane_f64 a)
{
    lane_f32 result;
    
    result.v = _mm_castpd_ps(a.v);
    
    return result;
}

internal lane_dv3
cast_lane_dv3(lane_v3 a)
{
    lane_dv3 result;
    
    result.x.v = _mm_castps_pd(a.x.v);
    result.y.v = _mm_castps_pd(a.y.v);
    result.z.v = _mm_castps_pd(a.z.v);
    
    return result;
}

internal lane_u32
lane_u32_from_u32(u32 r0, u32 r1, u32 r2, u32 r3, 
                  u32, u32, u32, u32,
                  u32, u32, u32, u32,
                  u32, u32, u32, u32)
{
    lane_u32 result;
    
    result.v = _mm_setr_epi32(r0, r1, r2, r3);
    
    return(result);
}

internal lane_u32
lane_u32_from_u32(u32 r0, u32 r1, u32 r2, u32 r3)
{
    lane_u32 result;
    
    result.v = _mm_setr_epi32(r0, r1, r2, r3);
    
    return(result);
}

internal lane_f32
lane_f32_from_u32(lane_u32 a)
{
    lane_f32 result;
    
    result.v = _mm_cvtepi32_ps(a.v);
    
    return(result);
}

internal lane_u32
lane_u32_from_u32(u32 a)
{
    lane_u32 result;
    
    result.v = _mm_set1_epi32(a);
    
    return(result);
}

internal lane_u32
lane_f32_from_u32(lane_f32 a)
{
    lane_u32 result;
    
    result.v = _mm_cvtps_epi32(a.v);
    
    return(result);
}

internal lane_f32
lane_f32_from_u32(u32 a)
{
    lane_f32 result;
    
    result.v = _mm_set1_ps((f32)a);
    
    return(result);
}

internal lane_f32
lane_f32_from_f32(f32 a)
{
    lane_f32 result;
    
    result.v = _mm_set1_ps(a);
    
    return(result);
}

internal lane_f32
lane_f32_from_f32(f32 a, f32 b, f32 c, f32 d)
{
    lane_f32 result;
    
    result.v = _mm_set_ps(a, b, c, d);
    
    return(result);
}

internal lane_f32
square_root(lane_f32 a)
{
    lane_f32 result;
    
    result.v = _mm_sqrt_ps(a.v);
    
    return(result);
}

inline void
conditional_assign(lane_f32 *dest, lane_u32 mask, lane_f32 src)
{
    __m128 mask_ps = _mm_castsi128_ps(mask.v);
    dest->v = _mm_or_ps(_mm_andnot_ps(mask_ps, dest->v), 
                        _mm_and_ps(mask_ps, src.v));
}

inline lane_f32
fmin(lane_f32 a, lane_f32 b)
{
    lane_f32 result;
    
    result.v = _mm_min_ps(a.v, b.v);
    
    return(result);
}

inline lane_f32
fmax(lane_f32 a, lane_f32 b)
{
    lane_f32 result;
    
    result.v = _mm_max_ps(a.v, b.v);
    
    return(result);
}

inline lane_f32
wpow(lane_f32 a, lane_f32 exp)
{
    lane_f32 result;
    
    result.v = _mm_pow_ps(a.v, exp.v);
    
    return result;
}

internal lane_f32
wabs(lane_f32 a)
{
    lane_f32 result;
    lane_f32 mask = lane_f32_from_f32(-0.0f);
    result.v = _mm_andnot_ps(mask.v, a.v);
    
    return result;
}

internal lane_f32
gather_f32_(void *base_ptr, u32 stride, lane_u32 indices)
{
    u32 *v = (u32 *)&indices.v;
    lane_f32 result;
    result.v = _mm_setr_ps(*(f32 *)((u8 *)base_ptr + v[0]*stride),
                           *(f32 *)((u8 *)base_ptr + v[1]*stride),
                           *(f32 *)((u8 *)base_ptr + v[2]*stride),
                           *(f32 *)((u8 *)base_ptr + v[3]*stride));
    
    return result;
}

internal b32x
mask_is_zeroed(lane_u32 a)
{
    int mask = _mm_movemask_epi8(a.v);
    
    return mask == 0;
}

internal u64
horizontal_add(lane_u32 a)
{
    u32 *v = (u32 *)&a.v;
    u64 result = (u64)v[0] + (u64)v[1] + (u64)v[2] + (u64)v[3];
    
    return result;
}

internal f32
horizontal_add(lane_f32 a)
{
    f32 *v = (f32 *)&a.v;
    f32 result = v[0] + v[1] + v[2] + v[3];
    
    return result;
}

// TODO: How can I do this kind of shuffle with SIMD instructions?
// I can't find any instructions that don't require integer constants
internal lane_v3
permute(lane_v3 v, lane_u32 i, lane_u32 j, lane_u32 k)
{
    // 4 v3's
    // 4 i's
    // 4 j's
    // 4 k's
    
    lane_v3 result;
    
    // r0 := { v.x[i[0]], v.y[i[0], v.z[i[0]] };
    // r1 := { v.x[i[1]], v.y[i[1], v.z[i[1]] };
    // r2 := { v.x[i[2]], v.y[i[2], v.z[i[2]] };
    // r3 := { v.x[i[3]], v.y[i[3], v.z[i[3]] };
    
    result.x = 
    {
        // f32 f32 f32 f32
        extract_f32(v.x, extract_u32(i, 0)),
        extract_f32(v.x, extract_u32(i, 1)),
        extract_f32(v.x, extract_u32(i, 2)),
        extract_f32(v.x, extract_u32(i, 3)),
    };
    
    result.y = 
    {
        // f32 f32 f32 f32
        extract_f32(v.y, extract_u32(j, 0)),
        extract_f32(v.y, extract_u32(j, 1)),
        extract_f32(v.y, extract_u32(j, 2)),
        extract_f32(v.y, extract_u32(j, 3)),
    };
    
    result.z = 
    {
        // f32 f32 f32 f32
        extract_f32(v.z, extract_u32(k, 0)),
        extract_f32(v.z, extract_u32(k, 1)),
        extract_f32(v.z, extract_u32(k, 2)),
        extract_f32(v.z, extract_u32(k, 3)),
    };
    
    return result;
}

#endif //LANE_4X_H
