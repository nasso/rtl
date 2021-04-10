/*
** EPITECH PROJECT, 2021
** B-OOP-400-TLS-4-1-tekspice-nassim.gharbaoui
** File description:
** option
*/

#include "rtl/Option.hpp"
#include "gtest/gtest.h"
#include <cstring>
#include <functional>
#include <string>
#include <unordered_set>

using rtl::none;
using rtl::Option;
using rtl::some;

class Unique {
public:
    Unique()
        : m_val(0)
    {
    }

    explicit Unique(int val)
        : m_val(val)
    {
    }

    Unique(const Unique&) = delete;

    Unique(Unique&& other)
        : m_val(other.m_val)
    {
    }

    ~Unique()
    {
    }

    bool operator==(const Unique& rhs) const
    {
        return m_val == rhs.m_val;
    }

    bool operator==(int rhs) const
    {
        return m_val == rhs;
    }

    int get() const
    {
        return m_val;
    }

private:
    int m_val = 0;
};

::std::ostream& operator<<(::std::ostream& os, const Unique& unique)
{
    return os << "Unique(" << unique.get() << ")";
}

TEST(option, default_constructor_is_none)
{
    Option<Unique> opt;

    ASSERT_TRUE(!opt);
}

TEST(option, some_constructor)
{
    Option<Unique> opt = some(Unique(4));

    ASSERT_TRUE(opt);
    ASSERT_EQ(opt.unwrap(), 4);
    ASSERT_TRUE(!opt);
}

TEST(option, some_default_constructor)
{
    auto opt = some(Unique());

    ASSERT_TRUE(opt.is_some());
    ASSERT_EQ(opt.unwrap(), 0);
}

TEST(option, replace_some)
{
    Option<std::string> opt;

    ASSERT_TRUE(opt.replace("hello").is_none());
    ASSERT_TRUE(opt);
    ASSERT_EQ(opt.unwrap(), "hello");
}

TEST(option, replace_none)
{
    auto opt = some<std::string>("hello");

    ASSERT_TRUE(opt.is_some());
    ASSERT_EQ(opt.unwrap(), "hello");
    ASSERT_TRUE(opt.replace("hi").is_none());
    ASSERT_TRUE(opt.is_some());
    ASSERT_EQ(opt.unwrap(), "hi");
}

TEST(option, map_simple)
{
    auto opt = some(3);
    auto dbl = opt.map([](int v) { return v * 2; });

    ASSERT_TRUE(dbl.is_some());
    ASSERT_EQ(dbl.unwrap(), 6);
}

TEST(option, expect_none)
{
    ASSERT_ANY_THROW(some(3).expect_none("bad!"));
}

TEST(option, unwrap_none)
{
    ASSERT_ANY_THROW(none<int>().unwrap());
}

TEST(option, unwrap_or)
{
    ASSERT_EQ(none<int>().unwrap_or_default(), 0);
    ASSERT_EQ(some(8).unwrap_or_default(), 8);
    ASSERT_EQ(none<int>().unwrap_or(3), 3);
    ASSERT_EQ(some(8).unwrap_or(3), 8);
    ASSERT_EQ(none<int>().unwrap_or_else([]() { return 3; }), 3);
    ASSERT_EQ(some(8).unwrap_or_else([]() { return 3; }), 8);
}

TEST(option, unwrap_or_operator)
{
    ASSERT_EQ(none<int>() || 3, 3);
    ASSERT_EQ(some(8) || 3, 8);
    ASSERT_EQ(
        none<int>() || []() { return 3; }, 3);
    ASSERT_EQ(
        some(8) || []() { return 3; }, 8);
}

TEST(option, stream_operator)
{
    std::stringstream ss;

    ss << some(9) << ", " << none<int>();
    ASSERT_EQ(ss.str(), "Some(9), None");
}

TEST(option, as_ref)
{
    Option<int> opt = some(3);
    Option<const int&> ref = opt.as_ref();

    ASSERT_EQ(ref.unwrap(), 3);
}

TEST(option, as_mut)
{
    Option<int> opt = some(3);
    Option<int&> mut = opt.as_mut();

    ASSERT_TRUE(mut);
    mut.unwrap() = 5;
    ASSERT_EQ(opt.unwrap(), 5);
}

TEST(option, as_ref_none)
{
    Option<const int&> ref = none<int>().as_ref();

    ASSERT_TRUE(!ref);
}

TEST(option, as_mut_none)
{
    Option<int&> mut = none<int>().as_mut();

    ASSERT_TRUE(!mut);
}

TEST(option, as_ref_from_const)
{
    const Option<int> opt = some(3);
    Option<const int&> ref = opt.as_ref();

    ASSERT_EQ(ref.unwrap(), 3);
}

TEST(option, map_as_ref)
{
    const Option<Unique> mbOrig = some(Unique(3));
    Option<const Unique&> mbRef = mbOrig.as_ref();
    Option<Unique> mbDoubled
        = mbRef.map([](const Unique& u) { return Unique(u.get() * 2); });

    ASSERT_TRUE(mbOrig);
    ASSERT_TRUE(!mbRef);
    ASSERT_TRUE(mbDoubled);
    ASSERT_EQ(mbDoubled, some(Unique(6)));
}

TEST(option, unique_ptr)
{
    Option<std::unique_ptr<int>> opt = some(std::make_unique<int>(3));

    ASSERT_TRUE(opt);
    ASSERT_EQ(*opt.unwrap(), 3);
    opt.replace(new int(56));
    opt.replace(new int(139));
    ASSERT_TRUE(opt);
    ASSERT_EQ(*opt.unwrap(), 139);
    opt.replace(std::make_unique<int>(42));
}

TEST(option, hash_owned)
{
    Option<int> owned;
    Option<int&> mut;
    Option<const int&> ref;

    std::hash<Option<int>>()(owned);
    std::hash<Option<int&>>()(mut);
    std::hash<Option<const int&>>()(ref);
}

TEST(option, hash_set)
{
    std::unordered_set<Option<std::string>> set;

    set.emplace();
    set.emplace();
    set.emplace(some("hi"));
    set.emplace(some("hello"));
    ASSERT_NE(set.find(none<std::string>()), set.end());
    ASSERT_NE(set.find(some("hi")), set.end());
    ASSERT_NE(set.find(some("hello")), set.end());
    ASSERT_EQ(set.find(some("blabla")), set.end());
}

TEST(option, flatten)
{
    ASSERT_EQ(none<Option<int>>().flatten(), none<int>());
    ASSERT_EQ(some(none<int>()).flatten(), none<int>());
    ASSERT_EQ(some(some(3)).flatten(), some(3));
}

TEST(option, and_then)
{
    auto sqr = [](int v) { return some(v * v); };
    auto nonify = [](int) { return none<int>(); };

    ASSERT_EQ(none<int>().and_then(sqr), none<int>());
    ASSERT_EQ(none<int>().and_then(nonify), none<int>());
    ASSERT_EQ(some(5).and_then(sqr), some(25));
    ASSERT_EQ(some(5).and_then(nonify), none<int>());
}

TEST(option, pipe_some)
{
    auto opt = some(5);

    ASSERT_EQ(
        opt | [](int n) { return n * n; } | [](int n) { return n + 1; } |
            [](int n) { return n / 2; } | [](int n) { return n - 5; },
        some((5 * 5 + 1) / 2 - 5));
}

TEST(option, pipe_none)
{
    auto opt = none<int>();

    ASSERT_TRUE((
        opt | [](int n) { return n * n; } | [](int n) { return n + 1; } |
        [](int n) { return n / 2; } |
        [](int n) {
            return n - 5;
        }).is_none());
}

TEST(option, pipe_some_void)
{
    auto opt = some(5);
    int piped = 0;

    ASSERT_TRUE(
        opt | [](int n) { return n * n; } | [](int n) { return n + 1; } |
        [](int n) { return n / 2; } | [](int n) { return n - 5; } |
        [&](int n) { piped = n; });

    ASSERT_EQ(piped, (5 * 5 + 1) / 2 - 5);
}

TEST(option, pipe_none_void)
{
    auto opt = none<int>();
    int piped = 0;

    ASSERT_FALSE(
        opt | [](int n) { return n * n; } | [](int n) { return n + 1; } |
        [](int n) { return n / 2; } | [](int n) { return n - 5; } |
        [&](int n) { piped = n; });

    ASSERT_EQ(piped, 0);
}

static int return_five()
{
    return 5;
}

TEST(option, function_pointer)
{
    Option<int (*)()> opt;

    ASSERT_TRUE(opt.is_none());

    opt.replace(&return_five);
    ASSERT_EQ(opt, some(&return_five));
}

TEST(option, member_pointer)
{
    Option<decltype(&std::string::size)> opt;

    ASSERT_TRUE(opt.is_none());

    opt.replace(&std::string::size);
    ASSERT_TRUE(opt == some(&std::string::size));
}

TEST(option, function_call_coalescing)
{
    auto opt = some([](int n) { return n * 2; });

    ASSERT_EQ(opt(3), some(6));

    opt.take();
    ASSERT_EQ(opt(5), none<int>());
}

TEST(option, member_access)
{
    struct A {
        int field;

        int method() const
        {
            return field;
        }
    };

    auto val = some<A>({ 3 });

    ASSERT_EQ(val[&A::field].copied(), some(3));
    ASSERT_EQ(val[&A::method](), some(3));

    {
        const auto& valref = val;

        ASSERT_EQ(valref[&A::field].copied(), some(3));
        ASSERT_EQ(valref[&A::method](), some(3));
    }

    ASSERT_EQ(some<A>({ 5 })[&A::field], some(5));
    ASSERT_EQ(some<A>({ 5 })[&A::method](), some(5));
    ASSERT_EQ(some<A>({ 5 }) | &A::method, some(5));

    auto nothing = none<A>();
    ASSERT_TRUE(nothing[&A::field].is_none());
    ASSERT_TRUE(nothing[&A::method]().is_none());
    ASSERT_TRUE((nothing | &A::method).is_none());

    {
        auto valasref = val.as_ref();

        // FIXME:
        // ASSERT_EQ(valasref[&A::field].copied(), some(3));
        // ASSERT_EQ(valasref[&A::method](), some(3));
    }

    {
        auto valasmut = val.as_mut();

        // FIXME:
        // ASSERT_EQ(valasmut[&A::field].copied(), some(3));
        // ASSERT_EQ(valasmut[&A::method](), some(3));
    }
}

TEST(option, example)
{
    // none
    rtl::Option<std::string> opt;

    // implicit cast to false for "None" values
    ASSERT_TRUE(!opt);

    // assign some value
    opt = rtl::some("hello");

    // implicit cast to true for "Some" values
    ASSERT_TRUE(opt);

    // unwrap takes ownership, leaving "None"
    ASSERT_EQ(opt.unwrap(), "hello");
    ASSERT_EQ(opt.unwrap_or(" world"), " world");

    // alternative syntax to assign a value
    opt = rtl::some("im here");

    // as_ref can be used to "borrow" the value (immutably) instead:
    rtl::Option<size_t> mapped
        = opt.as_ref()
              .map([](const std::string& name) { return name + ", too!"; })
              .map([](const std::string& str) { return str.size(); });

    ASSERT_EQ(mapped.expect("what?!"), std::strlen("im here, too!"));

    // set to none
    opt = rtl::none<std::string>();
}
