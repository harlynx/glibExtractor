#include <catch2/catch.hpp>
#include "glib_defs.hxx"
#include "glib_file.hxx"
#include "glib_file_validator.hxx"

using res = glib_file_validation_result;

constexpr auto TAG { "glib_file_validator" };

constexpr uint32_t len(const char* str) {
    return *str ? 1 + len(str + 1) : 0;
}

TEST_CASE("returns no_label if the file has no label", TAG) {
    const glib_file_validator v;
    glib_file f;
    f.set_container({ 'H', 'E', 'L', 'L', 'O' });
    REQUIRE(v.validate(f) == res::no_label);
}

TEST_CASE("returns label_too_long if the label exceeds the max length", TAG) {
    constexpr auto s {
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    };
    static_assert(len(s) > FIXED_LABEL_SIZE);

    const glib_file f(s, { 'H', 'E', 'L', 'L', 'O' });

    REQUIRE(glib_file_validator::validate(f) == res::label_too_long);
}

TEST_CASE("returns no_data if the file has no body", TAG) {
    const glib_file f("hello");

    REQUIRE(glib_file_validator::validate(f) == res::no_data);
}

TEST_CASE("returns data_too_large if the data exceeds the max size", TAG) {
    // FIXME: dangerous way of testing - need to introduce a seam to mock
    // the size so that the test does not allocate anything
    std::vector<char> super_vector(MAX_RECORD_SIZE + 1, 0);
    const glib_file f("hello", std::move(super_vector));

    REQUIRE(glib_file_validator::validate(f) == res::data_too_large);
}


TEST_CASE("returns ok if no errors were found", TAG) {
    const glib_file f("hello", { 'H', 'E', 'L', 'L', 'O' });

    REQUIRE(glib_file_validator::validate(f) == res::ok);
}
