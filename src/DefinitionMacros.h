#define E_serialized 101
#define E_guile 102
#define E_default 103

// TODO I'm not sure how this expands, it won't work
#define E_defaultParm(value) E_default, value

#define E_Void(extraInfo)

#define defineStructAndExtras_h(name, fields)           \
    struct name { fileds(defineStructField) };          \
    constructor_h(name)                                 \
    destructor_h(name)                                  \
    serialize_h(name)                                   \
    guile_h(name, fields)

#define defineExtras_c(name, fields)            \
    constructor_c(name, fields)                 \
    destructor_c(name, fields)                  \
    serialize_c(name, fields)                   \
    guile_c(name, fields)

#define defineStructField(type, name)      \
    type name;

#define constructor_h(name)                     \
    struct name ## * name ## _create();

#define destructor_h(name)                          \
    void name ## _destroy(struct name ## * this);

#define constructor_c(name, fields)             \
    struct prefix * name ## _create() {         \
        const int size = sizeof(struct name);   \
        struct struct* this = malloc(size);     \
        fields(constructor_initfield);          \
        return newValue;                        \
    }

#define destructor_c(name, fields)                  \
    void prefix ## _destroy(struct prefix * this) { \
        prefix ## _destroyMembers(this);            \
            free(this);                             \
    }

#define constructor_initField(type, name, ...)  \



#define serialize_h(fileds)



#define guile_h(fileds) \
    fields(guile_h_field)

#define guile_h_field(type, name, ...)          \
    if (is_extraField(E_guile, __VA_ARGS__))   \
    SCM name ## _guileGet()                     \
    SCM name ## _guileSet()                     \
    endif

#define is_extraField(type, first, ...) \
    if !(first)                        \
    0                                   \
    elif (first == E_guile)            \
    1                                   \
    elif                               \
    is_extraField(type, __VA_ARGS__)    \
    endif


#define is_guileField(first, ...)               \


#define ExampleFields(F)                                    \
    F(int, exampleInt, E_serialized, E_guile)               \
    F(char*, exampleString, E_defaultParm("hello"))
