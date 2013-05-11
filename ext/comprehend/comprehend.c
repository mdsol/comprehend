#include <ruby.h>

//begin copypasta of core ruby code that can't be easily included
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

VALUE rb_enum_values_pack(int argc, VALUE *argv)
{
  if (argc == 0) return Qnil;
  if (argc == 1) return argv[0];
  return rb_ary_new4(argc, argv);
}

//end copypasta

/*
 * call-seq:
 *    ary.comprehend! { |item| block } ->  ary or nil
 *
 * Invokes the given block once for each element of +self+.
 * If the result is nil, delete the element, otherwise replace it with the result.
 * Returns +self+ if any elements were deleted, otherwise nil.
 *
 *    a = [1, 2, 3]
 *    a.comprehend! { |num| num.to_s if num < 3 } #=> ["1", "2"]
 *    a                                           #=> ["1", "2"]
 *    a.comprehend! { |str| str.to_i }            #=> nil
 *    a                                           #=> [1, 2]
 */

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

/*
 * call-seq:
 *    ary.comprehend { |item| block } ->  new_ary
 *
 * Invokes the given block once for each element of +self+.
 * Creates a new array with all non-nil values returned by the block.
 *
 *    a = [1, 2, 3]
 *    a.comprehend { |num| num.to_s if num < 3 }  #=> ["1", "2"]
 *    a                                           #=> [1, 2, 3]
 */

static VALUE rb_ary_comprehend(ary)
    VALUE ary;
{
    ary = rb_ary_dup(ary);
    rb_ary_comprehend_bang(ary);
    return ary;
}

static VALUE project_i(VALUE i, VALUE hash, int argc, VALUE *argv)
{
  rb_hash_aset(hash, rb_enum_values_pack(argc, argv), rb_yield_values2(argc, argv));
  return Qnil;
}

static ID id_each;

/*
 * call-seq:
 *    enum.project { |item| block } ->  hash
 *
 * Invokes the given block once for each element of +self+.
 * Creates a new hash with the elements as keys and the results as values.
 *
 *    (1..3).project { |num| num**2 }  #=> { 1 => 1, 2 => 4, 3 => 9 }
 */

static VALUE enum_project(VALUE obj)
{
  VALUE hash;
  hash = rb_hash_new();
  rb_block_call(obj, id_each, 0, 0, project_i, hash);
  return hash;
}

void Init_comprehend(void) {
  id_each = rb_intern("each");
  rb_define_method(rb_cArray, "comprehend!", rb_ary_comprehend_bang, 0);
  rb_define_method(rb_cArray, "comprehend", rb_ary_comprehend, 0);
  rb_define_method(rb_mEnumerable, "project", enum_project, 0);
}
