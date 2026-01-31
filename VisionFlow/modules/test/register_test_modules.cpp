#include "register_test_modules.h"

#include "modules/module_registry.h"
#include "modules/module_definition.h"

#include "const_int_module.h"
#include "add_one_module.h"
#include "print_module.h"

void registerTestModules()
{
    ModuleRegistry& registry = ModuleRegistry::instance();


    // ================= ConstInt =================
    {
        ModuleDefinition def;
        def.moduleType = "ConstInt";
        def.displayName = "Const Int";


        def.ports = {
            { "out", PortDirection::Output, DataType::Any }
        };


        def.defaultParameters = {
            { "value", 1 }
        };


        registry.registerModule(
                    def,
                    []() { return std::make_unique<ConstIntModule>(); }
        );
    }


    // ================= AddOne =================
    {
        ModuleDefinition def;
        def.moduleType = "AddOne";
        def.displayName = "Add One";


        def.ports = {
            { "in", PortDirection::Input, DataType::Any },
            { "out", PortDirection::Output, DataType::Any }
        };


        registry.registerModule(
                    def,
                    []() { return std::make_unique<AddOneModule>(); }
        );
    }


    // ================= Print =================
    {
        ModuleDefinition def;
        def.moduleType = "Print";
        def.displayName = "Print";


        def.ports = {
            { "in", PortDirection::Input, DataType::Any }
        };


        registry.registerModule(
                    def,
                    []() { return std::make_unique<PrintModule>(); }
        );
    }
}
