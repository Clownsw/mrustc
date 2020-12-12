/*
 * MRustC - Rust Compiler
 * - By John Hodge (Mutabah/thePowersGang)
 *
 * expand/crate_type.cpp
 * - #![crate_type] handling
 */
#include <synext.hpp>
#include <ast/crate.hpp>

class Decorator_CrateType:
    public ExpandDecorator
{
public:
    AttrStage stage() const override { return AttrStage::Pre; }

    void handle(const Span& sp, const AST::Attribute& mi, AST::Crate& crate) const override {
        //if( crate.m_crate_type != AST::Crate::Type::Unknown ) {
        //    //ERROR(sp, E0000, "Multiple #![crate_type] attributes");
        //    return ;
        //}
        if( !mi.has_string() ) {
            ERROR(sp, E0000, "#![crate_type] requires a string argument");
        }
        const auto& name = mi.string();
        if( name == "rlib" || name == "lib" ) {
            crate.m_crate_type = AST::Crate::Type::RustLib;
        }
        else if( name == "dylib" || name == "rdylib" ) {
            crate.m_crate_type = AST::Crate::Type::RustDylib;
        }
        else {
            ERROR(sp, E0000, "Unknown crate type '" << name << "'");
        }
    }
};

class Decorator_CrateName:
    public ExpandDecorator
{
public:
    AttrStage stage() const override { return AttrStage::Pre; }

    void handle(const Span& sp, const AST::Attribute& mi, AST::Crate& crate) const override {
        //if( crate.m_crate_name != "" ) {
        //    ERROR(sp, E0000, "Multiple #![crate_name] attributes");
        //}
        if( !mi.has_string() || mi.string() == "" ) {
            ERROR(sp, E0000, "#![crate_name] requires a non-empty string argument");
        }
        crate.m_crate_name = mi.string();
    }
};

class Decorator_Feature:
    public ExpandDecorator
{
public:
    AttrStage stage() const override { return AttrStage::Pre; }

    void handle(const Span& sp, const AST::Attribute& mi, AST::Crate& crate) const override {
    }
};
STATIC_DECORATOR("feature", Decorator_Feature)


class Decorator_Allocator:
    public ExpandDecorator
{
public:
    AttrStage stage() const override { return AttrStage::Pre; }

    void handle(const Span& sp, const AST::Attribute& mi, AST::Crate& crate) const override {
        // TODO: Check for an existing allocator crate
        crate.m_lang_items.insert(::std::make_pair( "mrustc-allocator", AST::AbsolutePath() ));
    }

    void handle(const Span& sp, const AST::Attribute& mi, ::AST::Crate& crate, const AST::AbsolutePath& path, AST::Module& mod, slice<const AST::Attribute> attrs, AST::Item&i) const override {
        if( ! i.is_Function() ) {
            ERROR(sp, E0000, "#[allocator] can only be put on functions and the crate - found on " << i.tag_str());
        }
        // TODO: Ensure that this is an extern { fn }
        // TODO: Does this need to do anything?
    }
};
class Decorator_PanicRuntime:
    public ExpandDecorator
{
public:
    AttrStage stage() const override { return AttrStage::Pre; }

    void handle(const Span& sp, const AST::Attribute& mi, AST::Crate& crate) const override {
        // TODO: Check for an existing panic_runtime crate
        crate.m_lang_items.insert(::std::make_pair( "mrustc-panic_runtime", AST::AbsolutePath() ));
    }
};
class Decorator_NeedsPanicRuntime:
    public ExpandDecorator
{
public:
    AttrStage stage() const override { return AttrStage::Pre; }

    void handle(const Span& sp, const AST::Attribute& mi, AST::Crate& crate) const override {
        crate.m_lang_items.insert(::std::make_pair( "mrustc-needs_panic_runtime", AST::AbsolutePath() ));
    }
};

STATIC_DECORATOR("crate_type", Decorator_CrateType)
STATIC_DECORATOR("crate_name", Decorator_CrateName)

STATIC_DECORATOR("allocator", Decorator_Allocator)
STATIC_DECORATOR("panic_runtime", Decorator_PanicRuntime)
STATIC_DECORATOR("needs_panic_runtime", Decorator_NeedsPanicRuntime)


