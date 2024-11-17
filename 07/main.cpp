#include <iostream>
#include <map>
#include <numeric>
#include <stdexcept>
#include <vector>
#include <ranges>

using std::string, std::vector, std::map;
namespace ranges = std::ranges;
namespace views = std::ranges::views;

class Product {
  public:
    string name;
    string id;
    double weight;
    double price;
    unsigned daysStored;

    virtual double calculateStorageFee() const {
        return price * weight;
    }
    virtual void displayInfo() const {
        std::cout << *this;
    }
    friend std::ostream &operator<<(std::ostream &os, const Product &p) {
        os << "name = \"" << p.name << "\"\n";
        os << "\tid = \"" << p.id << "\"\n";
        os << "\tweight = " << p.weight << "\n";
        os << "\tprice = " << p.price << "\n";
        os << "\tin storage = " << p.daysStored << "\n";
        return os;
    }
    Product(string name, string id, double weight, double price,
            unsigned daysStored)
        : name(name), id(id), weight(weight), price(price),
          daysStored(daysStored) {
    }
    Product(const Product &p)
        : name(p.name), id(p.id), weight(p.weight), price(p.price),
          daysStored(p.daysStored) {
    }
    Product operator=(const Product &p) {
        return Product(p);
    }
    virtual ~Product() {
    }
};

class PerishableProduct : public Product {
  public:
    unsigned expirationDate;

    double calculateStorageFee() const override {
        double exp_percent = daysStored * 1.0 / expirationDate;
        return Product::calculateStorageFee() * (1 + exp_percent);
    }
    PerishableProduct(string name, string id, double weight, double price,
                      unsigned daysStored, unsigned expirationDate)
        : Product(name, id, weight, price, daysStored),
          expirationDate(expirationDate) {
    }
    PerishableProduct(const PerishableProduct &p)
        : Product(p), expirationDate(p.expirationDate) {
    }
};

class ElectronicProduct : public Product {
  public:
    unsigned warrantyPeriod;
    double powerRating;

    void displayInfo() const override {
        std::cout << *this;
    }
    friend std::ostream &operator<<(std::ostream &os,
                                    const ElectronicProduct &p) {
        os << static_cast<Product>(p);
        os << "\twarranty period = " << p.warrantyPeriod << "\n";
        os << "\tpower rating = " << p.powerRating << "\n";
        return os;
    }
    ElectronicProduct(string name, string id, double weight, double price,
                      unsigned daysStored, unsigned warrantyPeriod,
                      double powerRating)
        : Product(name, id, weight, price, daysStored),
          warrantyPeriod(warrantyPeriod), powerRating(powerRating) {
    }
    ElectronicProduct(const ElectronicProduct &p)
        : Product(p), warrantyPeriod(p.warrantyPeriod),
          powerRating(p.powerRating) {
    }
};

class BuildingMaterial : public Product {
  public:
    bool flammability;
    double calculateStorageFee() const override {
        return Product::calculateStorageFee() * (1.5 * flammability);
    }
    BuildingMaterial(string name, string id, double weight, double price,
                     unsigned daysStored, bool flammability)
        : Product(name, id, weight, price, daysStored),
          flammability(flammability) {
    }

    BuildingMaterial(const BuildingMaterial &p)
        : Product(p), flammability(p.flammability) {
    }
};

class Warehouse {
    map<string, Product *> products;

  public:
    Warehouse() {
    }

    template <typename T,
              typename = std::enable_if_t<std::is_base_of_v<Product, T>>>
    vector<const T *> getProductsByType() const {
        return products | views::transform([](std::pair<string, Product *> p) {
                   return dynamic_cast<const T *>(p.second);
               }) |
               views::filter([](const T *p) { return p != nullptr; }) |
               ranges::to<vector>();
    }

    vector<const PerishableProduct *> getExpiringProducts(unsigned days) const {
        return getProductsByType<PerishableProduct>() |
               views::filter([=](const PerishableProduct *p) {
                   return p->daysStored + days >= p->expirationDate;
               }) |
               ranges::to<vector>();
    }

    template <typename T,
              typename = std::enable_if_t<std::is_base_of_v<Product, T> &
                                          !std::is_same_v<T, Product>>>
    void addProduct(const T &p) {
        if (products.contains(p.id))
            throw std::invalid_argument("product with id already exists");
        products[p.id] = new T(p);
    }
    bool containsProduct(const string &id) {
        return products.contains(id);
    }
    void removeProduct(const string &id) {
        if (!products.contains(id))
            return;
        free(products[id]);
        products.erase(id);
    }
    const Product *getProduct(const string &id) const {
        return (*this)[id];
    }
    Product *getProduct(const string &id) {
        return (*this)[id];
    }

    double totalPrice() const {
        return std::accumulate(
            std::begin(products), std::end(products), 0,
            [](int val, const std::pair<string, Product *> &p) {
                return val + p.second->calculateStorageFee();
            });
    }

    template <typename T,
              typename = std::enable_if_t<std::is_base_of_v<Product, T> &
                                          !std::is_same_v<T, Product>>>
    Warehouse &operator+=(const T &p) {
        this->addProduct(p);
        return *this;
    }
    Warehouse &operator-=(const string &id) {
        this->removeProduct(id);
        return *this;
    }
    Product *operator[](const string &id) {
        if (!products.contains(id))
            return nullptr;
        return products.at(id);
    }
    const Product *operator[](const string &id) const {
        if (!products.contains(id))
            return nullptr;
        return products.at(id);
    }
    friend std::ostream &operator<<(std::ostream &os, const Warehouse &w) {
        os << "Perishable Products\n";
        for (auto v : w.getProductsByType<PerishableProduct>())
            os << *v;
        os << "\n";

        os << "Electronic Products\n";
        for (auto v : w.getProductsByType<ElectronicProduct>())
            os << *v;
        os << "\n";

        os << "Building Materials\n";
        for (auto v : w.getProductsByType<BuildingMaterial>())
            os << *v;
        os << "\n";
        return os;
    }

    void displayInventory() const {
        std::cout << *this;
    }
    ~Warehouse() {
        for (auto v : products)
            delete v.second;
    }
};

int main(void) {
    Warehouse w;
    Product p("product", "21", 12.4, 42, 12);
    PerishableProduct pp("Perishable", "12", 12.4, 42, 12, 42);
    ElectronicProduct ep("Electronic", "elec", 12.4, 42, 12, 2, 4);
    w.addProduct(pp);
    w.addProduct(ep);
    w.displayInventory();
    return 0;
}
