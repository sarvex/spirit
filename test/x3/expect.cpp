/*=============================================================================
    Copyright (c) 2001-2013 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include <boost/detail/lightweight_test.hpp>
#include <boost/spirit/home/x3.hpp>
#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/include/at.hpp>
#include <boost/spirit/home/x3/binary.hpp>

#include <string>
#include <iostream>
#include "test.hpp"

namespace
{
    template <typename Char, typename Parser>
    bool test_nothrow(Char const* in, Parser const& p, bool full_match = true)
    {
        using namespace boost::spirit::x3;
        Char const* last = in;
        while (*last)
            last++;
        return parse(in, last, with<expectation_failure_tag>(false)[p])
            && (!full_match || (in == last));
    }

    template <typename Char, typename Parser, typename Skipper>
    bool test_nothrow(Char const* in, Parser const& p
        , Skipper const& s, bool full_match = true)
    {
        using namespace boost::spirit::x3;
        Char const* last = in;
        while (*last)
            last++;
        return phrase_parse(in, last, with<expectation_failure_tag>(false)[p], s)
            && (!full_match || (in == last));
    }

    template <typename Char, typename Parser, typename Attr>
    bool test_attr_nothrow(Char const* in, Parser const& p
        , Attr& attr, bool full_match = true)
    {
        using namespace boost::spirit::x3;
        Char const* last = in;
        while (*last)
            last++;
        return parse(in, last, with<expectation_failure_tag>(false)[p], attr)
            && (!full_match || (in == last));
    }

    template <typename Char, typename Parser, typename Attr, typename Skipper>
    bool test_attr_nothrow(Char const* in, Parser const& p
        , Attr& attr, Skipper const& s, bool full_match = true)
    {
        using namespace boost::spirit::x3;
        Char const* last = in;
        while (*last)
            last++;
        return phrase_parse(in, last, with<expectation_failure_tag>(false)[p], s, attr)
            && (!full_match || (in == last));
    }
}

int
main()
{
    using namespace boost::spirit;
    using namespace boost::spirit::x3::ascii;
    using boost::spirit::x3::lit;
    using boost::spirit::x3::expect;
    using spirit_test::test;
    using spirit_test::test_attr;
    using boost::spirit::x3::expectation_failure;
    using boost::spirit::x3::lexeme;
    using boost::spirit::x3::no_case;
    using boost::spirit::x3::no_skip;
    using boost::spirit::x3::omit;
    using boost::spirit::x3::raw;
    using boost::spirit::x3::skip;
    using boost::spirit::x3::seek;
    using boost::spirit::x3::repeat;
    using boost::spirit::x3::matches;
    using boost::spirit::x3::eps;
    using boost::spirit::x3::eoi;
    using boost::spirit::x3::eol;
    using boost::spirit::x3::attr;
    using boost::spirit::x3::dword;
    using boost::spirit::x3::int_;
    using boost::spirit::x3::symbols;
    using boost::spirit::x3::confix;

    {
        try
        {
            BOOST_TEST((test("aa", char_ >> expect[char_])));
            BOOST_TEST((test("aaa", char_ >> expect[char_ >> char_('a')])));
            BOOST_TEST((test("xi", char_('x') >> expect[char_('i')])));
            BOOST_TEST((!test("xi", char_('y') >> expect[char_('o')]))); // should not throw!
            BOOST_TEST((test("xin", char_('x') >> expect[char_('i') >> char_('n')])));
            BOOST_TEST((!test("xi", char_('x') >> expect[char_('o')])));
        }
        catch (expectation_failure<char const*> const& x)
        {
            std::cout << "expected: " << x.which();
            std::cout << " got: \"" << x.where() << '"' << std::endl;
        }
    }

    {
        try
        {
            BOOST_TEST((test("aa", char_ > char_)));
            BOOST_TEST((test("aaa", char_ > char_ > char_('a'))));
            BOOST_TEST((test("xi", char_('x') > char_('i'))));
            BOOST_TEST((!test("xi", char_('y') > char_('o')))); // should not throw!
            BOOST_TEST((test("xin", char_('x') > char_('i') > char_('n'))));
            BOOST_TEST((!test("xi", char_('x') > char_('o'))));
        }
        catch (expectation_failure<char const*> const& x)
        {
            std::cout << "expected: " << x.which();
            std::cout << " got: \"" << x.where() << '"' << std::endl;
        }
    }

    {
        try
        {
            BOOST_TEST((!test("ay:a", char_ > char_('x') >> ':' > 'a')));
        }
        catch (expectation_failure<char const*> const& x)
        {
            std::cout << "expected: " << x.which();
            std::cout << " got: \"" << x.where() << '"' << std::endl;
        }
    }

    { // Test that attributes with > (sequences) work just like >> (sequences)

        using boost::fusion::vector;
        using boost::fusion::at_c;

        {
            vector<char, char, char> attr;
            BOOST_TEST((test_attr(" a\n  b\n  c",
                char_ > char_ > char_, attr, space)));
            BOOST_TEST((at_c<0>(attr) == 'a'));
            BOOST_TEST((at_c<1>(attr) == 'b'));
            BOOST_TEST((at_c<2>(attr) == 'c'));
        }

        {
            vector<char, char, char> attr;
            BOOST_TEST((test_attr(" a\n  b\n  c",
                char_ > char_ >> char_, attr, space)));
            BOOST_TEST((at_c<0>(attr) == 'a'));
            BOOST_TEST((at_c<1>(attr) == 'b'));
            BOOST_TEST((at_c<2>(attr) == 'c'));
        }

        {
            vector<char, char, char> attr;
            BOOST_TEST((test_attr(" a, b, c",
                char_ >> ',' > char_ >> ',' > char_, attr, space)));
            BOOST_TEST((at_c<0>(attr) == 'a'));
            BOOST_TEST((at_c<1>(attr) == 'b'));
            BOOST_TEST((at_c<2>(attr) == 'c'));
        }
    }

    {
        try
        {
            BOOST_TEST((test(" a a", char_ > char_, space)));
            BOOST_TEST((test(" x i", char_('x') > char_('i'), space)));
            BOOST_TEST((!test(" x i", char_('x') > char_('o'), space)));
        }
        catch (expectation_failure<char const*> const& x)
        {
            std::cout << "expected: " << x.which();
            std::cout << " got: \"" << x.where() << '"' << std::endl;
        }
    }

    {
        try
        {
            BOOST_TEST((test("bar", expect[lit("foo")])));
        }
        catch (expectation_failure<char const*> const& x)
        {
            std::cout << "expected: " << x.which();
            std::cout << " got: \"" << x.where() << '"' << std::endl;
        }
    }

    { // Test expect in skipper
        BOOST_TEST((test_nothrow("accbeabfcdg", repeat(7)[alpha], lit('a') > 'b' | lit('c') > 'd')));
        std::string attr;
        BOOST_TEST((test_attr_nothrow("accbeabfcdg", repeat(7)[alpha], attr, lit('a') > 'b' | lit('c') > 'd')));
        BOOST_TEST((attr == "accbefg"));
    }

    { // Test expect in auxilary parsers
        BOOST_TEST((test_nothrow("a12", lit('a') > eps > +digit)));
        BOOST_TEST((!test_nothrow("a12", lit('a') > eps(false) > +digit)));
        BOOST_TEST((test_nothrow("a12", lit('a') > +digit > eoi)));
        BOOST_TEST((!test_nothrow("a12", lit('a') > eoi > +digit)));
        BOOST_TEST((test_nothrow("a12\n", lit('a') > +digit > eol)));
        BOOST_TEST((!test_nothrow("a12\n", lit('a') > eol > +digit)));
        int n = 0;
        BOOST_TEST((test_attr_nothrow("abc", lit("abc") > attr(12) > eoi, n)));
        BOOST_TEST((12 == n));
    }

    { // Test expect in binary, numeric, char, string parsers
        BOOST_TEST((test_nothrow("12abcd", +digit > dword)));
        BOOST_TEST((!test_nothrow("12abc", +digit > dword)));
        BOOST_TEST((test_nothrow("abc12", +alpha > int_)));
        BOOST_TEST((!test_nothrow("abc", +alpha > int_)));
        BOOST_TEST((test_nothrow("12a", +digit > lit('a'))));
        BOOST_TEST((!test_nothrow("12a", +digit > lit('b'))));
        symbols<> s;
        s.add("cat");
        BOOST_TEST((test_nothrow("12cat", +digit > s)));
        BOOST_TEST((!test_nothrow("12dog", +digit > s)));
    }

    { // Test expect in confix
        BOOST_TEST((test_nothrow("[12cat]", confix('[', ']')[+digit > lit("cat")])));
        BOOST_TEST((!test_nothrow("[12dog]", confix('[', ']')[+digit > lit("cat")])));
    }

    { // Test expect in expect
        BOOST_TEST((test_nothrow("abc", lit('a') >> expect[lit('b') >> 'c'])));
        BOOST_TEST((!test_nothrow("abc", lit('a') >> expect[lit('b') >> 'd'])));
        BOOST_TEST((!test_nothrow("abc", lit('a') >> expect[lit('b') > 'd'])));
    }

    { // Test expect in lexeme
        BOOST_TEST((test_nothrow("12 ab", int_ >> lexeme[lit('a') > 'b'], space)));
        BOOST_TEST((!test_nothrow("12 a b", int_ >> lexeme[lit('a') > 'b'], space)));
    }

    { // Test expect in matches
        BOOST_TEST((test_nothrow("ab", matches[lit('a') >> 'b'])));
        BOOST_TEST((test_nothrow("ac", matches[lit('a') >> 'b'] >> "ac")));
        BOOST_TEST((test_nothrow("ab", matches[lit('a') > 'b'])));
        BOOST_TEST((!test_nothrow("ac", matches[lit('a') > 'b'] >> "ac")));
        bool attr = false;
        BOOST_TEST((test_attr_nothrow("ab", matches[lit('a') > 'b'], attr)));
        BOOST_TEST((true == attr));
    }

    { // Test expect in no_case
        BOOST_TEST((test_nothrow("12 aB", int_ >> no_case[lit('a') > 'b'], space)));
        BOOST_TEST((!test_nothrow("12 aB", int_ >> no_case[lit('a') > 'c'], space)));
    }

    { // Test expect in no_skip
        BOOST_TEST((test_nothrow("12 3ab", int_ >> int_ >> no_skip[lit('a') > 'b'], space)));
        BOOST_TEST((!test_nothrow("12 3ab", int_ >> int_ >> no_skip[lit('a') > 'c'], space)));
    }

    { // Test expect in omit
        BOOST_TEST((test_nothrow("ab", omit[lit('a') > 'b'])));
        BOOST_TEST((!test_nothrow("ab", omit[lit('a') > 'c'])));
    }

    { // Test expect in raw
        BOOST_TEST((test_nothrow("ab", raw[lit('a') > 'b'])));
        BOOST_TEST((!test_nothrow("ab", raw[lit('a') > 'c'])));
    }

    { // Test expect in repeat
        BOOST_TEST((test_nothrow("ababac", repeat(1, 3)[lit('a') >> 'b'] >> "ac" | +alpha)));
        BOOST_TEST((!test_nothrow("ababac", repeat(1, 3)[lit('a') > 'b'] | +alpha)));
        BOOST_TEST((!test_nothrow("acab", repeat(2, 3)[lit('a') > 'b'] | +alpha)));
        BOOST_TEST((test_nothrow("bcab", repeat(2, 3)[lit('a') > 'b'] | +alpha)));
    }

    { // Test expect in seek
        BOOST_TEST((test_nothrow("a1b1c1", seek[lit('c') > '1'])));
        BOOST_TEST((!test_nothrow("a1b1c2c1", seek[lit('c') > '1'])));
    }

    { // Test expect in skip
        BOOST_TEST((test_nothrow("ab[]c[]d", skip(lit('[') > ']')[+alpha])));
        BOOST_TEST((!test("ab[]c[5]d", skip(lit('[') > ']')[+alpha])));
        BOOST_TEST((test_nothrow("a1[]b2c3[]d4", skip(lit('[') > ']')[+(alpha > digit)])));
        BOOST_TEST((!test_nothrow("a1[]b2c3[]d", skip(lit('[') > ']')[+(alpha > digit)])));
    }

    { // Test expect in alternative
        BOOST_TEST((test_nothrow("ac", lit('a') >> 'b' | "ac")));
        BOOST_TEST((!test_nothrow("ac", lit('a') > 'b' | "ac")));
        BOOST_TEST((test_nothrow("ac", lit('a') >> 'b' | lit('a') >> 'd' | "ac")));
        BOOST_TEST((!test_nothrow("ac", lit('a') >> 'b' | lit('a') > 'd' | "ac")));
    }

    { // Test expect in and predicate
        BOOST_TEST((test_nothrow("abc", lit('a') >> &(lit('b') > 'c') >> "bc")));
        BOOST_TEST((!test_nothrow("abc", lit('a') >> &(lit('b') > 'd') >> "bc")));
    }

    { // Test expect in difference
        BOOST_TEST((test_nothrow("bcac", *(char_ - (lit('a') >> 'b')))));
        BOOST_TEST((test_nothrow("bcab", *(char_ - (lit('a') > 'b')) >> "ab")));
        BOOST_TEST((!test_nothrow("bcac", *(char_ - (lit('a') > 'b')) >> "ab")));
    }

    { // Test expect in kleene
        BOOST_TEST((test_nothrow("abac", *(lit('a') >> 'b') >> "ac")));
        BOOST_TEST((!test_nothrow("abac", *(lit('a') > 'b') >> "ac")));
        BOOST_TEST((test_nothrow("abbc", *(lit('a') > 'b') >> "bc")));
    }

    { // Test expect in list
        BOOST_TEST((test_nothrow("ab::ab::ac", (lit('a') >> 'b') % (lit(':') >> ':') >> "::ac")));
        BOOST_TEST((!test_nothrow("ab::ab::ac", (lit('a') > 'b') % (lit(':') >> ':') >> "::ac")));
        BOOST_TEST((test_nothrow("ab::ab:ac", (lit('a') > 'b') % (lit(':') >> ':') >> ":ac")));
        BOOST_TEST((!test_nothrow("ab::ab:ab", (lit('a') >> 'b') % (lit(':') > ':') >> ":ab")));
    }

    { // Test expect in not predicate
        BOOST_TEST((test_nothrow("[ac]", lit('[') >> !(lit('a') >> 'b') >> +alpha >> ']')));
        BOOST_TEST((test_nothrow("[bc]", lit('[') >> !(lit('a') > 'b') >> +alpha >> ']')));
        BOOST_TEST((!test_nothrow("[ac]", lit('[') >> !(lit('a') > 'b') >> +alpha >> ']')));
    }

    { // Test expect in optional
        BOOST_TEST((test_nothrow("ac", -(lit('a') >> 'b') >> "ac")));
        BOOST_TEST((test_nothrow("ab", -(lit('a') > 'b'))));
        BOOST_TEST((!test_nothrow("ac", -(lit('a') > 'b') >> "ac")));
    }

    { // Test expect in plus
        BOOST_TEST((test_nothrow("abac", +(lit('a') >> 'b') >> "ac")));
        BOOST_TEST((test_nothrow("abbc", +(lit('a') > 'b') >> "bc")));
        BOOST_TEST((!test_nothrow("abac", +(lit('a') > 'b') >> "ac")));
    }

    return boost::report_errors();
}
