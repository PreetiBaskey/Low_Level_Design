/*
  Requirements : 
  1. Initialize pizzas (base pizzas)
  2. Add toppings
  3. Calculate final price
  4. Support business rules
    i.   Discounts
    ii.  Mutually exclusive toppings
    iii. Possibly combo offers
  5. System should be extensible

  High-Level Architecture
  1. Core Entities
  2. Pricing Engine
  3. Rules Engine
  4. Decorators for Toppings
*/
#include<iostream>
#include<unordered_set>
#include<vector>
#include<string>
#include<stdexcept>
#include<memory>

using namespace std;

class Pizza {
    public:
        virtual ~Pizza() = default;
        virtual string getDescription() = 0;
        virtual double getPrice() = 0;
        virtual unordered_set<string> getToppings() = 0;
};

class Margherita : public Pizza {
    public:
        string getDescription() {
            return "Margherita";
        }
        
        double getPrice() {
            return 200.0;
        }
        
        unordered_set<string> getToppings() {
            return {};
        }
};

class Farmhouse : public Pizza {
    public:
        string getDescription() {
            return "Farmhouse";
        }
        
        double getPrice() {
            return 300.0;
        }
        
        unordered_set<string> getToppings() {
            return {};
        }
};

class ToppingDecorator : public Pizza {
    protected:
        shared_ptr<Pizza> pizza;
    public:
        ToppingDecorator(shared_ptr<Pizza> p) : pizza(p) {}
};

class ExtraCheese : public ToppingDecorator {
    public:
        ExtraCheese(shared_ptr<Pizza> p) : ToppingDecorator(p) {}
        
        string getDescription() {
            return pizza->getDescription() + " , Extra Cheese";
        }
        
        double getPrice() {
            return pizza->getPrice() + 40.0;
        }
        
        unordered_set<string> getToppings() {
            auto toppings = pizza->getToppings();
            toppings.insert("ExtraCheese");
            
            return toppings;
        }
};

class Mushroom : public ToppingDecorator {
    public:
        Mushroom(shared_ptr<Pizza> p) : ToppingDecorator(p) {}
        
        string getDescription() {
            return pizza->getDescription() + ", Mushroom";
        }
        
        double getPrice() {
            return pizza->getPrice() + 30.0;
        }
        
        unordered_set<string> getToppings() {
            auto toppings = pizza->getToppings();
            toppings.insert("Mushroom");
            return toppings;
        }
};

class Chicken : public ToppingDecorator {
    public:
        Chicken(shared_ptr<Pizza> p) : ToppingDecorator(p) {}
        
        string getDescription() {
            return pizza->getDescription() + ", Chicken";
        }
        
        double getPrice() {
            return pizza->getPrice() + 60.0;
        }
        
        unordered_set<string> getToppings() {
            auto toppings = pizza->getToppings();
            toppings.insert("Chicken");
            
            return toppings;
        }
};

class PricingRule {
    public:
        virtual ~PricingRule() = default;
        virtual double apply(shared_ptr<Pizza> pizza, double currentPrice) = 0;
};

class PercentageDiscountRule : public PricingRule {
    private:
        double percentage;
    public:
        PercentageDiscountRule(double percent) : percentage(percent) {}
        
        double apply(shared_ptr<Pizza> pizza, double currentPrice) {
            return currentPrice * (1 - percentage/100.0);
        }
};

class MutualExclusionRule : public PricingRule {
    private:
        string topping1;
        string topping2;
    public:
        MutualExclusionRule(string t1, string t2) : topping1(t1), topping2(t2) {}
        
        double apply(shared_ptr<Pizza> pizza, double currentPrice) {
            auto toppings = pizza->getToppings();
            
            if(toppings.count(topping1) && toppings.count(topping2)) {
            throw runtime_error("mutually exclusive toppings selected !!");
            }
            
            return currentPrice;
        }
};

class PricingEngine {
    private:
        vector<shared_ptr<PricingRule>> rules;
    public:
        void addRule(shared_ptr<PricingRule> rule) {
            rules.push_back(rule);
        }
        
        double calculate(shared_ptr<Pizza> pizza) {
            double price = pizza->getPrice();
            
            for(auto &rule: rules) {
                price = rule->apply(pizza, price);
            }
            
            return price;
        }
};

int main() {
    
    shared_ptr<Pizza> pizza = make_shared<Margherita>();
    
    pizza = make_shared<ExtraCheese>(pizza);
    pizza = make_shared<Mushroom>(pizza);
    pizza = make_shared<Chicken>(pizza);
    
    cout<<"Order :- "<<pizza->getDescription()<<endl;
    cout<<"Base price :- "<<pizza->getPrice()<<endl;
    
    PricingEngine engine;
    
    engine.addRule(make_shared<PercentageDiscountRule>(10));
    engine.addRule(make_shared<MutualExclusionRule>("Mushroom", "Chicken"));
    
    try {
        double finalPrice = engine.calculate(pizza);
        cout<<"Final Price :- "<<finalPrice<<endl;
    }
    catch(exception &e) {
        cout<<"Error : "<<e.what()<<endl;
    }
    
    return 0;
}
