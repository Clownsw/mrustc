/*
 * MRustC - Rust Compiler
 * - By John Hodge (Mutabah/thePowersGang)
 *
 * include/int128.cpp
 * - Compiler-agnostic 128-bit integers
 */
#pragma once
#include <stdint.h>
#include <iostream>
#include "../common.hpp"

class U128
{
    friend class S128;
    uint64_t    lo;
    uint64_t    hi;
public:
    U128()
        : lo(0), hi(0)
    {
    }
    explicit U128(uint64_t lo, uint64_t hi=0)
        : lo(lo)
        , hi(hi)
    {
    }

    static U128 max() { return U128(UINT64_MAX,UINT64_MAX); }

    uint64_t get_lo() const { return lo; }
    uint64_t get_hi() const { return hi; }

    bool is_u64() const { return hi == 0; }
    uint64_t truncate_u64() const { return lo; }
    double to_double() const { return hi * static_cast<double>(UINT64_MAX) + lo; }

    U128 operator~() const { return U128(~lo, ~hi); }
    U128 operator+(U128 x) const { U128 rv(0); add128_o(*this, x, &rv); return rv; }
    U128 operator-(U128 x) const { U128 rv(0); sub128_o(*this, x, &rv); return rv; }
    U128 operator|(U128 x) const { return U128(lo | x.lo, hi | x.hi); }
    U128 operator&(U128 x) const { return U128(lo & x.lo, hi & x.hi); }
    U128 operator^(U128 x) const { return U128(lo ^ x.lo, hi ^ x.hi); }
    U128 operator*(U128 x) const { U128 rv(0); mul128_o(*this, x, &rv); return rv; }
    U128 operator/(U128 x) const { U128 rv(0); div128_o(*this, x, &rv, nullptr); return rv; }
    U128 operator%(U128 x) const { U128 rv(0); div128_o(*this, x, nullptr, &rv); return rv; }

    U128 operator+(unsigned x) const { return *this + U128(x); }
    U128 operator-(unsigned x) const { return *this - U128(x); }
    U128 operator*(unsigned x) const { return *this * U128(x); }
    U128 operator|(unsigned x) const { return *this | U128(x); }
    U128 operator&(unsigned x) const { return *this & U128(x); }
    U128 operator^(unsigned x) const { return *this ^ U128(x); }
    U128 operator/(unsigned x) const { return *this / U128(x); }
    U128 operator%(unsigned x) const { return *this % U128(x); }

    U128& operator+=(unsigned x) { *this = *this + x; return *this; }
    U128& operator+=(U128 x) { *this = *this + x; return *this; }

    U128& operator*=(unsigned x) { *this = *this * x; return *this; }
    U128& operator*=(U128     x) { *this = *this * x; return *this; }

    U128& operator|=(unsigned x) { *this = *this | x; return *this; }
    U128& operator|=(U128     x) { *this = *this | x; return *this; }

    U128& operator&=(unsigned x) { *this = *this & x; return *this; }
    U128& operator&=(U128     x) { *this = *this & x; return *this; }

    U128& operator<<=(unsigned bits) { *this = *this << bits; return *this; }
    U128& operator>>=(unsigned bits) { *this = *this >> bits; return *this; }

    U128 operator<<(unsigned bits) const {
        if(bits == 0)
            return *this;
        if(bits >= 128)
            return U128(0);
        if(bits >= 64)
            return U128(0, lo << (bits-64));
        else
            return U128(lo << bits, (hi << bits) | (lo >> (64-bits)));
    }
    U128 operator>>(unsigned bits) const {
        if(bits == 0)
            return *this;
        if(bits >= 128)
            return U128(0);
        if(bits >= 64)
            return U128(hi >> (bits-64), 0);
        else
            return U128(lo >> bits | (hi << (64-bits)), hi >> bits);
    }

    Ordering ord(const U128& x) const { int c = cmp128(*this, x); if(c == 0) return OrdEqual; return c < 0 ? OrdLess : OrdGreater; }
    bool operator< (const U128& x) const { return cmp128(*this, x) <  0; }
    bool operator<=(const U128& x) const { return cmp128(*this, x) <= 0; }
    bool operator> (const U128& x) const { return cmp128(*this, x) >  0; }
    bool operator>=(const U128& x) const { return cmp128(*this, x) >= 0; }
    bool operator==(const U128& x) const { return cmp128(*this, x) == 0; }
    bool operator!=(const U128& x) const { return cmp128(*this, x) != 0; }

    bool operator< (unsigned x) const { return *this <  U128(x); }
    bool operator<=(unsigned x) const { return *this <= U128(x); }
    bool operator> (unsigned x) const { return *this >  U128(x); }
    bool operator>=(unsigned x) const { return *this >= U128(x); }
    bool operator==(unsigned x) const { return *this == U128(x); }
    bool operator!=(unsigned x) const { return *this != U128(x); }

    bool bit(unsigned idx) const {
        if(idx <  64) return ((lo >> idx) & 1) != 0;
        if(idx < 128) return ((hi >> (idx - 64)) & 1) != 0;
        return false;
    }

    friend std::ostream& operator<<(::std::ostream& os, const U128& x) {
        if( x.hi == 0 ) {
            os << x.lo;
        }
        //else if( (os.flags() & std::ios_base::hex) != 0 && true ) {
        //    os << x.hi << "_" << x.lo;
        //}
        else {
            char output[40+1];
            auto v = x;
            unsigned i = 0;
            const char* chars = (os.flags() & std::ios_base::uppercase) ? "0123456789ABCDEF" : "0123456789abcdef";
            switch( os.flags() & std::ios_base::basefield )
            {
            case std::ios_base::hex:
                while( v.hi > 0 && v.lo > 0 ) {
                    output[i++] = chars[ (v.lo & 0xF) ];
                    v >>= 4u;
                }
                break;
            case std::ios_base::oct:
                while( v.hi > 0 && v.lo > 0 ) {
                    output[i++] = chars[ (v.lo & 7) ];
                    v >>= 3u;
                }
                break;
            case std::ios_base::dec:
            default:
                while( v.hi > 0 && v.lo > 0 ) {
                    U128    v2(0), rem(0);
                    div128_o(v, U128(10), &v2, &rem);
                    output[i++] = chars[ (rem.lo % 10) ];
                    v = v2;
                }
                break;
            }
            for(auto v = os.width(); v > i; v--)
                os << ' ';
            while(i --)
                os << output[i];
        }
        return os;
    }
private:
    // TODO: All of these are functionally identical to code in `codegen_c.cpp` - could it be shared?
    static int cmp128(U128 a, U128 b) { if(a.hi != b.hi) return a.hi < b.hi ? -1 : 1; if(a.lo != b.lo) return a.lo < b.lo ? -1 : 1; return 0; }
    static bool add128_o(U128 a, U128 b, U128* o) { o->lo = a.lo + b.lo; o->hi = a.hi + b.hi + (o->lo < a.lo ? 1 : 0); return (o->hi < a.hi); }
    static bool sub128_o(U128 a, U128 b, U128* o) { o->lo = a.lo - b.lo; o->hi = a.hi - b.hi - (o->lo > a.lo ? 1 : 0); return (o->hi > a.hi); }
    static bool mul128_o(U128 a, U128 b, U128* o) {
        bool of = false;
        o->hi = 0; o->lo = 0;
        for(int i=0;i<128;i++)
        {
            uint64_t m = (1ull << (i % 64));
            if(a.hi==0&&a.lo<m)   break;
            if(i>=64&&a.hi<m) break;
            if( m & (i >= 64 ? a.hi : a.lo) ) of |= add128_o(*o, b, o);
            b.hi = (b.hi << 1) | (b.lo >> 63);
            b.lo = (b.lo << 1);
        }
        return of;
    }
    // Long division
    static inline bool div128_o(U128 a, U128 b, U128* q, U128* r) {
        if(a.hi == 0 && b.hi == 0) {
            if(q) { q->hi=0; q->lo = a.lo / b.lo; }
            if(r) { r->hi=0; r->lo = a.lo % b.lo; }
            return false;
        }
        if(cmp128(a, b) < 0) {
            if(q) { q->hi=0; q->lo=0; }
            if(r) *r = a;
            return false;
        }
        U128 a_div_2( (a.lo>>1)|(a.hi << 63), a.hi>>1 );
        int shift = 0;
        while( cmp128(a_div_2, b) >= 0 && shift < 128 ) {
            shift += 1;
            b.hi = (b.hi<<1)|(b.lo>>63); b.lo <<= 1;
        }
        if(shift == 128) return true;   // true = overflowed
        U128 mask( /*lo=*/(shift >= 64 ? 0 : (1ull << shift)), /*hi=*/(shift < 64 ? 0 : 1ull << (shift-64)) );
        shift ++;
        if(q) { q->hi = 0; q->lo = 0; }
        while(shift--) {
            if( cmp128(a, b) >= 0 ) { if(q) add128_o(*q, mask, q); sub128_o(a, b, &a); }
            mask.lo = (mask.lo >> 1) | (mask.hi << 63); mask.hi >>= 1;
            b.lo = (b.lo >> 1) | (b.hi << 63); b.hi >>= 1;
        }
        if(r) *r = a;
        return false;
    }
};
class S128
{
    U128    inner;
public:
    S128() {}
    explicit S128(int64_t v):    inner(v, v < 0 ? UINT64_MAX : 0) {}
    S128(U128 v): inner(v) {}

    static S128 max() { return S128(U128(UINT64_MAX, INT64_MAX)); }
    static S128 min() { return S128(U128(0, INT64_MIN)); }

    bool is_i64() const { return inner.hi == ((inner.lo >> 63) ? UINT64_MAX : 0); }
    int64_t truncate_i64() const { /*assert(inner.hi == 0 || inner.hi == UINT64_MAX);*/ return inner.lo; }
    double to_double() const { return (*this < 0 ? -1.0 : 1.0) * this->u_abs().to_double(); }
    U128 get_inner() const { return inner; }

    S128 operator~() const { return S128(~inner); }
    S128 operator-() const { return S128(~inner) + S128(1); }
    S128 operator+(S128 x) const { return S128(inner + x.inner); }
    S128 operator-(S128 x) const { return S128(inner - x.inner); }
    S128 operator|(S128 x) const { return S128(inner | x.inner); }
    S128 operator&(S128 x) const { return S128(inner & x.inner); }
    S128 operator^(S128 x) const { return S128(inner ^ x.inner); }
    S128 operator*(S128 x) const {
        auto ret_neg = is_neg() != x.is_neg();
        auto rv_u = u_abs() * x.u_abs();
        return ret_neg ? -S128(rv_u) : S128(rv_u);
    }
    S128 operator/(S128 x) const {
        auto ret_neg = is_neg() != x.is_neg();
        auto rv_u = u_abs() / x.u_abs();
        return ret_neg ? -S128(rv_u) : S128(rv_u);
    }
    S128 operator%(S128 x) const {
        auto ret_neg = is_neg() != x.is_neg();
        auto rv_u = u_abs() % x.u_abs();
        return ret_neg ? -S128(rv_u) : S128(rv_u);
    }

    bool is_neg() const { return (inner >> 127).truncate_u64() != 0; }
    /// Unsigned absolute value (handles MIN correctly)
    U128 u_abs() const { if(inner.hi == UINT64_MAX && inner.lo == 0) return inner; if(is_neg()) return (-*this).inner; else return (*this).inner; }

    Ordering ord(const S128& x) const { int c = cmp128s(this->inner, x.inner); if(c == 0) return OrdEqual; return c < 0 ? OrdLess : OrdGreater; }
    bool operator< (const S128& x) const { return cmp128s(this->inner, x.inner) <  0; }
    bool operator<=(const S128& x) const { return cmp128s(this->inner, x.inner) <= 0; }
    bool operator> (const S128& x) const { return cmp128s(this->inner, x.inner) >  0; }
    bool operator>=(const S128& x) const { return cmp128s(this->inner, x.inner) >= 0; }
    bool operator==(const S128& x) const { return cmp128s(this->inner, x.inner) == 0; }
    bool operator!=(const S128& x) const { return cmp128s(this->inner, x.inner) != 0; }

    bool operator< (int x) const { return *this <  S128(x); }
    bool operator<=(int x) const { return *this <= S128(x); }
    bool operator> (int x) const { return *this >  S128(x); }
    bool operator>=(int x) const { return *this >= S128(x); }
    bool operator==(int x) const { return *this == S128(x); }
    bool operator!=(int x) const { return *this != S128(x); }

    S128& operator<<=(unsigned bits) { *this = *this << bits; return *this; }
    S128& operator>>=(unsigned bits) { *this = *this >> bits; return *this; }

    S128 operator<<(unsigned bits) const {
        return S128(inner << bits);
    }
    S128 operator>>(unsigned bits) const {
        if(bits == 0)
            return *this;
        if(bits >= 128)
            return *this < 0 ? S128(-1) : S128(0);
        if(bits >= 64)
           return S128(U128(inner.hi >> (bits-64), *this < 0 ? UINT64_MAX : 0));
        return S128(U128(inner.lo >> bits | (inner.hi << (64-bits)), static_cast<uint64_t>(static_cast<int64_t>(inner.hi) >> bits)));
    }

    void fmt(::std::ostream& os) const {
        if( is_i64() ) {
            os << static_cast<int64_t>(inner.lo);
        }
        else {
            if( *this < 0 ) {
                os << '-';
                os << (-*this).inner;
            }
            else {
                os << inner;
            }
        }
    }
    friend std::ostream& operator<<(::std::ostream& os, const S128& x) {
        x.fmt(os);
        return os;
    }

private:
    static inline int cmp128s(U128 a, U128 b) { if(a.hi != b.hi) return (int64_t)a.hi < (int64_t)b.hi ? -1 : 1; if(a.lo != b.lo) return a.lo < b.lo ? -1 : 1; return 0; }
};

