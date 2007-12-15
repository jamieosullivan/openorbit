

#include <stdlib.h>
#include <check.h>
#include "libgencds/object-manager.h"

typedef struct {
    int a;
    int b;
    int c;
} test_object_t;

#define TEST_ARR_LEN 10

typedef struct {
    test_object_t *tobj;
    int arr[TEST_ARR_LEN];
} test_object_oref_t;


typedef struct {
    void (*foo)(test_object_t *);
    void (*bar)(test_object_t *, int);
    void (*baz)(test_object_t *, float);
} test_iface_t;

void*
toref_const(void)
{
    return malloc(sizeof(test_object_oref_t));
}


void*
to_const(void)
{
    return malloc(sizeof(test_object_t));
}

void
to_dest(void *obj)
{
    return free(obj);
}


void
foo_impl(test_object_t *self)
{
    
}

void
bar_impl(test_object_t *self, int ugh)
{

}

void
baz_impl(test_object_t *self, float mooh)
{

}


START_TEST(test_interfaces)
{
    test_iface_t iface;
    iface.foo = foo_impl;
    iface.bar = bar_impl;
    iface.baz = baz_impl;
    
    om_ctxt_t *ctxt = om_new_ctxt();
    fail_if(ctxt == NULL, "om_new_ctxt returned NULL");
    
    om_meta_iface_t *mi = om_new_meta_interface(ctxt, "test-iface");
    fail_if(mi == NULL, "om_new_meta_iface returned NULL");
    
    om_reg_method(mi, "foo", offsetof(test_iface_t, foo));
    om_reg_method(mi, "bar", offsetof(test_iface_t, bar));
    om_reg_method(mi, "baz", offsetof(test_iface_t, baz));
    
    om_class_t *cls = om_new_class(ctxt, "test-class", to_const, to_dest,
                                   sizeof(test_object_t));
    
    /* Interface "gaaah" is not registered as meta interface */
    int res = om_reg_iface(cls, "gaaah", &iface);
    fail_if(res == 0, "The interface gaaah, should not be registerable");
    
    res = om_reg_iface(cls, "test-iface", &iface);
    fail_unless(res == 0, "Failed registering test-iface");
    
    void* iface_ptr = om_get_concrete_iface(cls, "test-iface");
    fail_unless(iface_ptr == &iface, "Interface pointer is wrong...");
    
    void* method_ptr =  om_get_concrete_method(cls, "test-iface", "foo");
    fail_unless(method_ptr == foo_impl, "method foo does not match foo_impl");
    
    method_ptr = om_get_concrete_method(cls, "test-iface", "baz");
    fail_unless(method_ptr == baz_impl, "method baz does not match baz_impl");
 
    om_delete_ctxt(ctxt);   
}
END_TEST

START_TEST(test_objects)
{
    om_ctxt_t *ctxt = om_new_ctxt();
    fail_if(ctxt == NULL, "om_new_ctxt returned NULL");
   
    om_class_t *cls = om_new_class(ctxt, "test-class", to_const, to_dest,
                                   sizeof(test_object_t));
    fail_if(cls == NULL, "No test-class was created in context");
    
    om_reg_prop(cls, "a", OM_INT, offsetof(test_object_t, a));
    om_reg_prop(cls, "b", OM_INT, offsetof(test_object_t, b));
    om_reg_prop(cls, "c", OM_INT, offsetof(test_object_t, c));
    
    om_object_t *obj0 = om_new_object(ctxt, "test-class", "foo");
    fail_if(obj0 == NULL, "The object named foo of test-class is null");
    
    om_object_t *obj1 = om_get_object(ctxt, "foo");
    
    fail_unless(obj0 == obj1, "The object returned for key foo differs from "
                              "the created object!");
    
    om_object_t *obj2 = om_get_object_from_ptr(ctxt, om_get_object_data(obj0));
    
    fail_unless(obj0 == obj2, "The object returned by reverse lookup differs "
                              "from the created object!");
 
    
    om_delete_object(obj0);
    om_object_t *obj3 = om_get_object(ctxt, "foo");
    fail_unless(obj3 == NULL, "om_get_object returned dead object");
    
    om_delete_ctxt(ctxt);    
}
END_TEST


START_TEST(test_properties)
{
    om_ctxt_t *ctxt = om_new_ctxt();
    fail_if(ctxt == NULL, "om_new_ctxt returned NULL");

    om_class_t *cls = om_new_class(ctxt, "test-class", to_const, to_dest,
                                   sizeof(test_object_t));
    fail_if(cls == NULL, "No test-class was created in context");
    om_reg_prop(cls, "a", OM_INT, offsetof(test_object_t, a));
    om_reg_prop(cls, "b", OM_INT, offsetof(test_object_t, b));
    om_reg_prop(cls, "c", OM_INT, offsetof(test_object_t, c));

    cls = om_new_class(ctxt, "test-class-oref", toref_const, to_dest,
                       sizeof(test_object_oref_t));

    fail_if(cls == NULL, "No test-oref-class was created in context");
    om_reg_prop(cls, "tobj", OM_OBJECT, offsetof(test_object_oref_t, tobj));
    om_reg_static_array_prop(cls, "arr", OM_INT,
                             offsetof(test_object_oref_t, arr), TEST_ARR_LEN);
    
    
    
    om_object_t *obj0 = om_new_object(ctxt, "test-class", "foo");
    test_object_t *obj0_data = om_get_object_data(obj0);
    om_object_t *obj1 = om_new_object(ctxt, "test-class-oref", "bar");
    test_object_oref_t *obj1_data = om_get_object_data(obj1);
    
    
    /* Init object slots */
    obj0_data->a = 0;
    obj0_data->b = 0;
    obj0_data->c = 0;
    
    obj1_data->tobj = NULL;
    
    for (int i = 0 ; i < TEST_ARR_LEN ; i ++) {
        obj1_data->arr[i] == 0;
    }
    
    
    fail_unless(obj0_data->a == om_get_int_prop(obj0, "a"), "Prop a in not 0");
    fail_unless(obj0_data->b == om_get_int_prop(obj0, "b"), "Prop b in not 0");
    fail_unless(obj0_data->c == om_get_int_prop(obj0, "c"), "Prop c in not 0");

    fail_unless(obj1_data->tobj == om_get_object_prop(obj1, "tobj"),
                "Prop tobj in not NULL");

    for (int i = 0 ; i < TEST_ARR_LEN ; i ++) {
        fail_unless(om_get_int_idx_prop(obj1, "arr", i) == 0,
                    "Arr prop with index %d in not 0", i);
    }

    
    om_set_int_prop(obj0, "a", 0x71010101);
    om_set_int_prop(obj0, "b", 0xf0202020);
    om_set_int_prop(obj0, "c", 0x73030303);
    
    om_set_object_prop(obj1, "tobj", obj0_data);
    
    om_set_int_idx_prop(obj1, "arr", 3, 0xaaaaaaaa);
        
    fail_unless(obj0_data->a == 0x71010101, "A is %x (set_int_prop failed)",
                obj0_data->a);
    fail_unless(obj0_data->b == 0xf0202020, "B is %x (set_int_prop failed)",
                obj0_data->b);
    fail_unless(obj0_data->c == 0x73030303, "C is %x (set_int_prop failed)",
                obj0_data->c);
    
    fail_unless(obj1_data->tobj == obj0_data, "TOBJ is %x", obj1_data->tobj);

    fail_unless(obj1_data->arr[3] == 0xaaaaaaaa, "ARR[3] is %x "
                "(set_int_idx failed)", obj1_data->arr[3]);
    
    om_delete_object(obj0);
    om_delete_ctxt(ctxt);    
}
END_TEST

Suite
*test_suite (void)
{
    Suite *s = suite_create ("Object Manager");
    
    /* Core test case */
    TCase *tc_core = tcase_create ("Core");

    tcase_add_test(tc_core, test_interfaces);
    tcase_add_test(tc_core, test_objects);
    tcase_add_test(tc_core, test_properties);
 
    
    suite_add_tcase(s, tc_core);
    
    return s;
}
