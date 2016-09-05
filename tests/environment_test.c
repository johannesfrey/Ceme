#include "../dbg.h"
#include "../memory.h"
#include "../symbol_table.h"
#include "../environment.h"

int
test_environment()
{
    object_p val_obtained;
    object_p val_foo_number, val_foo_number2, val_bar_number, val_string;
    object_p entry_obtained;
    object_p local_env;

    object_p key_foo = symbol_table_get_or_put("foo");
    object_p key_bar = symbol_table_get_or_put("bar");
    
    // Global env
    val_obtained = global_env_get_value(key_foo);
    check(val_obtained == NULL, "There shouldn't be anything in the global env at this point");
    entry_obtained = global_env_get_entry(key_foo);
    check(entry_obtained == nil_object, "There shouldn't be anything in the global env at this point");

    val_foo_number = alloc_number(0xF000);
    global_env_put(key_foo, val_foo_number);
    val_obtained = global_env_get_value(key_foo);
    check(val_obtained == val_foo_number, "Error in global env lookup (CREATE)");
    val_foo_number2 = alloc_number(0x000F);
    global_env_set(key_foo, val_foo_number2);
    val_obtained = global_env_get_value(key_foo);
    check(val_obtained == val_foo_number2, "Error in global env lookup (MODIFY)");
    
    val_string = alloc_string("BAR");
    global_env_put(key_bar, val_string);
    val_obtained = global_env_get_value(key_bar);
    check(val_obtained == val_string, "Error in global env lookup");

    // Local env
    val_bar_number = alloc_number(0xBAAA);
    local_env = alloc_env(1, global_env);
    local_env_put(local_env, key_bar, val_bar_number);
    val_obtained = local_env_get_value(local_env, key_bar);
    check(val_obtained == val_bar_number, "Error in global env lookup");
    entry_obtained = local_env_get_entry(local_env, key_bar);
    check(entry_obtained->any.tag == T_ENV_BINDING, "(Local) environment entry should be of type cons");

    return 0;

error:
    return -1;
}

int
main()
{
    init_wellknown_objects();
    init_symbol_table();
    init_global_env();
    check(test_environment() == 0, "test_symbol_table failed\n");

    return 0;

error:
    return -1;
}
