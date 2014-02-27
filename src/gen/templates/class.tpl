#ifndef {{classname}}_HPP
#define {{classname}}_HPP
{{#includes}}
{{include}}
{{/includes}}
{{beginnamespaces}}

class {{classname}} {

public:
    {{classname}}() : {{parentclasses}}
    {}
    virtual ~{{classname}}() {}

};

{{endnamespaces}}
#endif // {{classname}}_HPP
