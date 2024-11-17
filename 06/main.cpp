#include <compare>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <new>
#include <stdexcept>

class vector {
    double *ptr;
    size_t cap;
    size_t len;
    vector() : ptr(NULL), cap(0), len(0) {
    }

  public:
    struct iterator {
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = double;
        using pointer = double *;
        using reference = double &;
        iterator(pointer ptr) : ptr(ptr) {
        }
        reference operator*() const {
            return *ptr;
        }
        pointer operator->() {
            return ptr;
        }
        iterator &operator++() {
            ptr++;
            return *this;
        }
        iterator operator++(int) {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }
        iterator operator+(int a) {
            return iterator(ptr + a);
        }
        iterator operator-(int a) {
            return iterator(ptr - a);
        }
        iterator operator+=(int a) {
            ptr += a;
            return *this;
        }
        iterator operator-=(int a) {
            ptr -= a;
            return *this;
        }

        friend bool operator==(const iterator &a, const iterator &b) {
            return a.ptr == b.ptr;
        };
        friend bool operator!=(const iterator &a, const iterator &b) {
            return a.ptr != b.ptr;
        };

      private:
        pointer ptr;
    };
    double &at(size_t index) {
        if (index >= len) {
            throw std::out_of_range("");
        }
        return ptr[index];
    }
    double &front() {
        return ptr[0];
    }
    double &back() {
        return ptr[len - 1];
    }
    double *data() {
        return ptr;
    }
    bool empty() const {
        return len == 0;
    }
    size_t size() const {
        return len;
    }
    void reserve(size_t num) {
        if (num <= cap)
            return;
        double *new_ptr = (double *)(ptr ? realloc(ptr, num * sizeof(double))
                                         : malloc(num * sizeof(double)));
        if (!new_ptr)
            throw std::bad_alloc();
        ptr = new_ptr;
        cap = num;
    }
    size_t capacity() const {
        return cap;
    }
    void shrink_to_fit() {
        if (len == 0) {
            free(ptr);
            ptr = NULL;
            cap = 0;
            return;
        }
        double *new_ptr = (double *)realloc(ptr, len * sizeof(double));
        if (!new_ptr)
            throw std::bad_alloc();
        ptr = new_ptr;
        cap = len;
    }
    void clear() {
        free(ptr);
        ptr = NULL;
        len = 0;
    }
    void insert(size_t index, double elem) {
        this->reserve(len + 1);
        for (size_t i = len; i > index; i--) {
            ptr[i] = ptr[i - 1];
        }
        ptr[index] = elem;
        len++;
    }
    void erase(size_t index) {
        for (size_t i = index; i < len; i++) {
            ptr[i] = ptr[i + 1];
        }
        len--;
    }
    void push_back(double elem) {
        this->insert(len, elem);
    }
    void pop_back() {
        this->erase(len - 1);
    }
    void resize(size_t size, double elem) {
        if (size <= len) {
            len = size;
            return;
        }
        this->reserve(size);
        for (size_t i = len; i < size; i++)
            ptr[i] = elem;
        len = size;
    }
    std::partial_ordering operator<=>(const vector &b) const {
        auto s = len <=> b.len;
        if (s != std::partial_ordering::equivalent)
            return s;
        for (size_t i = 0; i < len; i++) {
            auto d = ptr[i] <=> b.ptr[i];
            if (d != std::partial_ordering::equivalent)
                return d;
        }
        return std::partial_ordering::equivalent;
    }

    bool operator==(const vector &b) const {
        return this->operator<=>(b) == std::partial_ordering::equivalent;
    }

    iterator begin() {
        return ptr;
    }
    iterator end() {
        return ptr + len;
    }

    vector(size_t n, double elem) : vector() {
        this->resize(n, elem);
    }
    vector(size_t n) : vector(n, 0) {
    }
    vector(iterator begin, iterator end) : vector() {
        this->reserve(std::distance(begin, end));
        for (; begin != end; begin++) {
            this->push_back(*begin);
        }
    }
    vector(std::initializer_list<double> l) : vector() {
        this->reserve(l.size());
        for (auto val : l)
            this->push_back(val);
    }
    ~vector() {
        if (ptr)
            free(ptr);
    }
};

#define PRINT(str, expr)                                                       \
    {                                                                          \
        using std::cout, std::endl;                                            \
        cout << str << " = {\n\tcap = " << expr.capacity()                     \
             << "\n\tsize = " << expr.size() << "\n\t{";                       \
        for (auto v : expr) {                                                  \
            cout << v << " ";                                                  \
        };                                                                     \
        cout << "}\n}" << endl;                                                \
    }

void print(std::partial_ordering value) {
    value < 0   ? std::cout << "less\n"
    : value > 0 ? std::cout << "greater\n"
                : std::cout << "equal\n";
}

int main(void) {
    using std::cout, std::endl;
    {
        cout << "initialization" << endl;
        vector a(10, 4);
        PRINT("size + element ", a);
        vector b(10);
        PRINT("size", b);
        vector c = {0, 12, -41.3, 12, 4, 5};
        PRINT("list", c);
        vector d(c.begin() + 2, c.end() - 1);
        PRINT("iter", d);
    }
    {
        cout << "reserve + shrink" << endl;
        vector a = {0, 12, -41.3, 12, 4, 5};
        PRINT("list", a);
        a.reserve(24);
        PRINT("reserved", a);
        a.shrink_to_fit();
        PRINT("shrunk", a);
    }
    {
        cout << "modify" << endl;
        vector a = {0, 12, -41.3, 12, 4, 5};
        PRINT("list", a);
        a.insert(2, 3.14);
        a.insert(5, 3.1415);
        PRINT("added", a);
        a.erase(4);
        a.pop_back();
        PRINT("erased", a);
        a.resize(12, 0.001);
        PRINT("resized", a);
    }
    {
        cout << "equality" << endl;
        vector a = {0, 12, 4};
        vector b = {0, 12, 4, 4};
        vector c = {0, 12, 2};
        PRINT("a", a);
        PRINT("b", b);
        PRINT("c", c);
        cout << "a <=> a = ";
        print(a <=> a);
        cout << "a <=> b = ";
        print(a <=> b);
        cout << "a <=> c = ";
        print(a <=> c);
    }
    return 0;
}
