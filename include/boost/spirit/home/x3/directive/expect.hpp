/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(SPIRIT_EXPECT_MARCH_16_2012_1024PM)
#define SPIRIT_EXPECT_MARCH_16_2012_1024PM

#include <boost/spirit/home/x3/support/context.hpp>
#include <boost/spirit/home/x3/core/parser.hpp>
#include <boost/spirit/home/x3/core/detail/parse_into_container.hpp>
#include <boost/spirit/home/x3/directive/with.hpp>

#include <boost/throw_exception.hpp>
#include <stdexcept>

namespace boost { namespace spirit { namespace x3
{
    template <typename Iterator>
    struct expectation_failure : std::runtime_error
    {
    public:

        expectation_failure(Iterator where, std::string const& which)
          : std::runtime_error("boost::spirit::x3::expectation_failure")
          , where_(where), which_(which)
        {}
        ~expectation_failure() throw() {}

        std::string which() const { return which_; }
        Iterator const& where() const { return where_; }

    private:

        Iterator where_;
        std::string which_;
    };

    namespace detail {
        inline bool has_expectation_failure_impl(unused_type) {
            return false;
        }

        inline bool has_expectation_failure_impl(bool& failure) {
            return failure;
        }

        template <typename Iterator, typename Subject>
        void handle_expectation_failure_impl(
            Iterator const& where,
            Subject const& subject,
            unused_type
        ) {
            boost::throw_exception(
                expectation_failure<Iterator>(where, what(subject)));
        }

        template <typename Iterator, typename Subject>
        void handle_expectation_failure_impl(
            Iterator const&,
            Subject const&,
            bool& failure
        ) {
            failure = true;
        }
    }

    template <typename Context> 
    bool has_expectation_failure(Context const& context) {
        return detail::has_expectation_failure_impl(
            x3::get<expectation_failure_tag>(context));
    }

    template <typename Iterator, typename Subject, typename Context>
    void handle_expectation_failure(
        Iterator const& where,
        Subject const& subject,
        Context const& context
    ) {
        detail::handle_expectation_failure_impl(
            where, subject, x3::get<expectation_failure_tag>(context));
    }

    template <typename Subject>
    struct expect_directive : unary_parser<Subject, expect_directive<Subject>>
    {
        typedef unary_parser<Subject, expect_directive<Subject> > base_type;
        static bool const is_pass_through_unary = true;

        expect_directive(Subject const& subject)
          : base_type(subject) {}

        template <typename Iterator, typename Context
          , typename RContext, typename Attribute>
        bool parse(Iterator& first, Iterator const& last
          , Context const& context, RContext& rcontext, Attribute& attr) const
        {
            Iterator save = first;
            bool r = this->subject.parse(first, last, context, rcontext, attr);

            if (!r)
            {
                handle_expectation_failure(first, this->subject, context);
                first = save;
            }
            return r;
        }
    };

    struct expect_gen
    {
        template <typename Subject>
        expect_directive<typename extension::as_parser<Subject>::value_type>
        operator[](Subject const& subject) const
        {
            return { as_parser(subject) };
        }
    };

    auto const expect = expect_gen{};
}}}

namespace boost { namespace spirit { namespace x3 { namespace detail
{
    // Special case handling for expect expressions.
    template <typename Subject, typename Context, typename RContext>
    struct parse_into_container_impl<expect_directive<Subject>, Context, RContext>
    {
        template <typename Iterator, typename Attribute>
        static bool call(
            expect_directive<Subject> const& parser
          , Iterator& first, Iterator const& last
          , Context const& context, RContext& rcontext, Attribute& attr)
        {
            Iterator save = first;
            bool r = parse_into_container(
                parser.subject, first, last, context, rcontext, attr);

            if (!r)
            {
                handle_expectation_failure(first, parser.subject, context);
                first = save;
            }
            return r;
        }
    };
}}}}

#endif
