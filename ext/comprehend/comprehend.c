#include <ruby.h>

#define RARRAY_SHARED_ROOT_FLAG FL_USER5
#define ARY_SHARED_ROOT_P(ary) (FL_TEST((ary), RARRAY_SHARED_ROOT_FLAG))
# define ARY_EMBED_P(ary) \
     FL_TEST((ary), RARRAY_EMBED_FLAG)!=0
#define ARY_SET_EMBED_LEN(ary, n) do { \
    long tmp_n = (n); \
    RBASIC(ary)->flags &= ~RARRAY_EMBED_LEN_MASK; \
    RBASIC(ary)->flags |= (tmp_n) << RARRAY_EMBED_LEN_SHIFT; \
} while (0)
#define ARY_SET_HEAP_LEN(ary, n) do { \
    RARRAY(ary)->as.heap.len = (n); \
} while (0)
#define ARY_SET_LEN(ary, n) do { \
    if (ARY_EMBED_P(ary)) { \
        ARY_SET_EMBED_LEN((ary), (n)); \
    } \
    else { \
        ARY_SET_HEAP_LEN((ary), (n)); \
    } \
} while (0)
#define ARY_CAPA(ary) (ARY_EMBED_P(ary) ? RARRAY_EMBED_LEN_MAX : \
		       ARY_SHARED_ROOT_P(ary) ? RARRAY_LEN(ary) : RARRAY(ary)->as.heap.aux.capa)
#define ARY_DEFAULT_SIZE 16
#define ARY_SET_CAPA(ary, n) do { \
  RARRAY(ary)->as.heap.aux.capa = (n); \
} while (0)

static void ary_resize_capa(VALUE ary, long capacity)
{
    if (capacity > RARRAY_EMBED_LEN_MAX) {
        if (ARY_EMBED_P(ary)) {
            long len = ARY_EMBED_LEN(ary);
            VALUE *ptr = ALLOC_N(VALUE, (capacity));
            MEMCPY(ptr, ARY_EMBED_PTR(ary), VALUE, len);
            FL_UNSET_EMBED(ary);
            ARY_SET_PTR(ary, ptr);
            ARY_SET_HEAP_LEN(ary, len);
        }
        else {
            REALLOC_N(RARRAY(ary)->as.heap.ptr, VALUE, (capacity));
        }
        ARY_SET_CAPA(ary, (capacity));
    }
    else {
        if (!ARY_EMBED_P(ary)) {
            long len = RARRAY_LEN(ary);
            VALUE *ptr = RARRAY_PTR(ary);
            if (len > capacity) len = capacity;
            MEMCPY(RARRAY(ary)->as.ary, ptr, VALUE, len);
            FL_SET_EMBED(ary);
            ARY_SET_LEN(ary, len);
            xfree(ptr);
        }
    }
}

static VALUE rb_ary_comprehend_bang(VALUE ary)
{
    VALUE *p, *t, *end;
    long n;

    rb_ary_modify(ary);
    p = t = RARRAY_PTR(ary);
    end = p + RARRAY_LEN(ary);

    while (t < end) {
        *t = rb_yield(*t);
        if (NIL_P(*t)) t++;
        else *p++ = *t++;
    }
    n = p - RARRAY_PTR(ary);
    if (RARRAY_LEN(ary) == n) {
        return Qnil;
    }
    ARY_SET_LEN(ary, n);
    if (n * 2 < ARY_CAPA(ary) && ARY_DEFAULT_SIZE * 2 < ARY_CAPA(ary)) {
        ary_resize_capa(ary, n * 2);
    }

    return ary;
}

static VALUE rb_ary_comprehend(ary)
    VALUE ary;
{
    ary = rb_ary_dup(ary);
    rb_ary_comprehend_bang(ary);
    return ary;
}

void Init_comprehend(void) {
  rb_define_method(rb_cArray, "comprehend!", rb_ary_comprehend_bang, 0);
  rb_define_method(rb_cArray, "comprehend", rb_ary_comprehend, 0);
}
