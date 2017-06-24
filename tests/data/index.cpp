#include <string>


namespace parser{
class Index
{
public:
    Index() = default;
    ~Index() = default;

    void bar();
    void foobar();
private:
    std::string name;
};

void Index::bar()
{
}

void foo()
{}

void Index::foobar()
{
    bar();
}

Index foo(const int &i)
{
    Index part;
    Index name;
    std::string s;
    name = part;
    name.bar();
    foo();
    return part;
}
}
