#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <set>

namespace mt {

    struct Ingredient {
        std::string name;
        double volume;

        Ingredient() : name("Неизвестно"), volume(0) {}
        Ingredient(const std::string& n, double v) : name(n), volume(v) {
            if (v < 0)
                throw std::invalid_argument("Объем ингредиента < 0");
        }

        void print() const {
            std::cout << "  - " << name << " (" << volume << ")\n";
        }
    };

    
    class Product {
    private:
        std::string name_;
        std::string description_;
        double price_;
        int expiration_days_;

        bool is_price_valid(double price) const { return price >= 0; }

    public:
        std::string place_;
        std::vector<Ingredient> composition;

        Product()
            : name_("Не задано"), description_("Не задано"),
              price_(0), expiration_days_(0), place_("Не указано") {}

        Product(const std::string& name,
                const std::string& description,
                double price,
                int expiration_days,
                const std::string& place,
                const std::vector<Ingredient>& comp)
            : name_(name), description_(description), price_(price),
              expiration_days_(expiration_days), place_(place), composition(comp) {

            if (!is_price_valid(price_))
                throw std::invalid_argument("Цена < 0");
            if (expiration_days_ < 0)
                throw std::invalid_argument("Срок < 0");
        }

        Product(const Product& other) = default;
        Product& operator=(const Product& other) = default;
        ~Product() { composition.clear(); }

        void set_name(const std::string& name) { name_ = name; }
        std::string get_name() const { return name_; }

        void set_description(const std::string& desc) { description_ = desc; }
        std::string get_description() const { return description_; }

        void set_price(double price) { 
            if (!is_price_valid(price)) throw std::invalid_argument("Цена < 0");
            price_ = price;
        }
        double get_price() const { return price_; }

        void set_expiration_days(int days) { expiration_days_ = days; }
        int get_expiration_days() const { return expiration_days_; }

        void set_place(const std::string& place) { place_ = place; }
        std::string get_place() const { return place_; }

        void print_info() const {
            std::cout << "Название: " << name_ << "\n"
                      << "Описание: " << description_ << "\n"
                      << "Цена: " << price_ << "\n"
                      << "Срок: " << expiration_days_ << "\n"
                      << "Место: " << place_ << "\n"
                      << "Состав:\n";
            for (const auto& i : composition) i.print();
            std::cout << "\n";
        }

        void reduce_expiration(int days) {
            expiration_days_ = std::max(0, expiration_days_ - days);
        }

        Product operator+(const Product& other) const {
            std::vector<Ingredient> new_comp;
            std::set<std::string> unique;
            for (const auto& i : composition)
                if (unique.insert(i.name).second) new_comp.push_back(i);
            for (const auto& i : other.composition)
                if (unique.insert(i.name).second) new_comp.push_back(i);

            return Product(
                "смесь " + name_ + " и " + other.name_,
                description_,
                (price_ + other.price_) * 0.9,
                std::min(expiration_days_, other.expiration_days_),
                place_,
                new_comp
            );
        }

        Product operator-(const Product& other) const {
            std::vector<Ingredient> new_comp;
            for (const auto& i : composition) {
                bool found = false;
for (const auto& j : other.composition)
                    if (i.name == j.name) found = true;
                if (!found) new_comp.push_back(i);
            }
            new_comp.emplace_back("консервант Т1000", 1.0);

            return Product(
                "выбор " + name_ + " без " + other.name_,
                description_,
                price_ * 0.9,
                std::max(0, expiration_days_ - 2),
                place_,
                new_comp
            );
        }

        Product& operator-=(const Product& other) {
            *this = *this - other;
            return *this;
        }
    };

    class Commission {
    private:
        std::string address_;
        std::vector<std::string> banned_;
        int check_day_;

    public:
        Commission() : address_("Не указано"), check_day_(0) {}
        Commission(const std::string& addr, const std::vector<std::string>& banned, int day)
            : address_(addr), banned_(banned), check_day_(day) {}
        Commission(const Commission& other) = default;
        Commission& operator=(const Commission& other) = default;
        ~Commission() { banned_.clear(); }

        void print_info() const {
            std::cout << "Комиссия\n";
            std::cout << "Адрес: " << address_ << "\n";
            std::cout << "Срок проверки: " << check_day_ << "\n";
            std::cout << "Запрещенные ингредиенты:\n";
            for (const auto& b : banned_) std::cout << "  - " << b << "\n";
            std::cout << "\n";
        }

        bool check_product(const Product& product) const {
            if (product.get_expiration_days() < check_day_) {
                std::cout << "Срок годности истек\n";
                return false;
            }
            for (const auto& ing : product.composition)
                for (const auto& banned : banned_)
                    if (ing.name == banned) {
                        std::cout << "Обнаружен запрещенный ингредиент: " << banned << "\n";
                        return false;
                    }
            return true;
        }
    };

} // namespace mt
