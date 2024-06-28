#ifndef INTERVAL_H
#define INTERVAL_H

class Interval {
public:
    double min, max;

    Interval() : min(+infinity), max(-infinity) {} // Default interval is empty

    Interval(double _min, double _max) : min(_min), max(_max) {}

    Interval(const Interval& a, const Interval& b) {
        // Create the interval tightly enclosing the two input intervals.
        min = a.min <= b.min ? a.min : b.min;
        max = a.max >= b.max ? a.max : b.max;
    }


    bool contains(double x) const {
        return min <= x && x <= max;
    }

    bool surrounds(double x) const {
        return min < x && x < max;
    }

    double clamp(double x) const {
        if (x < min) return min;
        if (x > max) return max;
        return x;
    }

    Interval expand(double delta) const {
        auto padding = delta/2;
        return Interval(min - padding, max + padding);
    }

    double size() const {
        return max - min + 1;
    }

//    static const interval empty, universe;
};

//const static interval empty   (+std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity());
//const static interval universe(-std::numeric_limits<double>::infinity(), +std::numeric_limits<double>::infinity());

Interval operator+(const Interval& ival, double displacement) {
    return Interval(ival.min + displacement, ival.max + displacement);
}

Interval operator+(double displacement, const Interval& ival) {
    return ival + displacement;
}


#endif