#include <algorithm>
#include <string>
#include <cctype>
#include <vector>
#include <iterator>

// Copy with a transformation:
std::vector<int> in{1, 2, 3, 4, 5, 6, 7};
std::vector<double> out;

std::transform(in.begin(), in.end(),
    std::back_inserter(out),
    [](int v) { return static_cast<double>(v)/2; });
// out ~= {0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5}

// Reduce two ranges into one:
std::vector<int> in1{1, 2, 3, 4, 5};
std::vector<int> in2{0, 1, 0, 1, 0};
std::vector<int> reduction;

std::transform(in1.begin(), in1.end(), // first range
    in2.begin(), // second range, number of elements from first range
    std::back_inserter(reduction),
    [](int e1, int e2) { return e1*e2; });
// reduction == {0, 2, 0, 4, 0}

// The output range can be one of the input ranges:
std::string str = "welcome to somewhere";
std::transform(str.begin(), str.end(), // input range
    str.begin(), // Note, can't be offset (only r1.begin(), r2.begin())
    [](char c) { return std::toupper(c); });
// str == "WELCOME TO SOMEWHERE"

// Take care when working with expensive to copy types.
struct Expensive{};
std::vector<Expensive> data(10);
std::transform(data.begin(), data.end(),
    data.begin(),
    [](const Expensive& v) { return v; }); // copy
// To modify element in-place, use std::for_each or a range-for.