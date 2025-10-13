//
// Created by Denis on 09.10.2025.
//

#ifndef BERESTALANGUAGE_BUILTINARRAY_H
#define BERESTALANGUAGE_BUILTINARRAY_H

#pragma once
#include "builtin/core/IBuiltinFunction.h"

struct BuiltinArrayIsArray : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "array_is_array";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& filename, int line) override;
};

struct BuiltinArrayLength : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "array_length";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& filename, int line) override;
};

struct BuiltinArrayGet : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "array_get";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& filename, int line) override;
};

struct BuiltinArraySet : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "array_set";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& filename, int line) override;
};

struct BuiltinArrayPush : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "array_push";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& filename, int line) override;
};

struct BuiltinArrayLast : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "array_last";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& filename, int line) override;
};

struct BuiltinArrayPop : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "array_pop";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& filename, int line) override;
};

struct BuiltinArrayInsert : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "array_insert";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& filename, int line) override;
};

struct BuiltinArrayDelete : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "array_delete";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& filename, int line) override;
};

struct BuiltinArraySlice : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "array_slice";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& filename, int line) override;
};

struct BuiltinArrayConcat : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "array_concat";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& filename, int line) override;
};

struct BuiltinArrayReverse : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "array_reverse";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& filename, int line) override;
};

struct BuiltinArrayIndexOf : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "array_index_of";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& filename, int line) override;
};

struct BuiltinArrayContains : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "array_contains";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& filename, int line) override;
};

struct BuiltinArrayJoin : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "array_join";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& filename, int line) override;
};

struct BuiltinArrayResize : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "array_resize";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& filename, int line) override;
};

struct BuiltinArrayFill : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "array_fill";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& filename, int line) override;
};

struct BuiltinArraySort : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "array_sort";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& filename, int line) override;
};

struct BuiltinArrayShuffle : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "array_shuffle";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& filename, int line) override;
};

void register_builtin_array();


#endif //BERESTALANGUAGE_BUILTINARRAY_H
