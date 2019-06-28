#ifndef TEST_PROP_CLASS_H_
#define TEST_PROP_CLASS_H_

namespace test
{

template <typename T>
class PropClass
{
private:
    T beta_;
    T gamma_;
    T delta_;
    T epsilon_;

public:
    T alpha;

    T beta()
    {
        return beta_;
    }

    void set_beta(const T beta)
    {
        beta_ = beta;
    }

    T &gamma()
    {
        return gamma_;
    }

    void set_gamma(const T &gamma)
    {
        gamma_ = gamma;
    }

    T *delta()
    {
        return &delta_;
    }

    void set_delta(const T *delta)
    {
        delta_ = *delta;
    }

    constexpr static auto properties = std::make_tuple(
        bound::property(&PropClass::alpha, "alpha"),
        bound::property(&PropClass::beta, "beta"),
        bound::property(&PropClass::set_beta, "beta"),
        bound::property(&PropClass::gamma, "gamma"),
        bound::property(&PropClass::set_gamma, "gamma"),
        bound::property(&PropClass::delta, "delta"),
        bound::property(&PropClass::set_delta, "delta"));
};

} // namespace test

#endif