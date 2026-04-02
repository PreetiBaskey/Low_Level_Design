/*  Imagine you have a call center with three levels of employees: respondent manager, and director. 
    An incoming telephone call must be first allocated to a respondent who is free. 
    If the respondent can't handle the call, he or she must escalate the call to a manager. 
    If the manager is not free or not able to handle it, then the call should be escalated 
    to a director. Design the classes and data structures for this problem. Implement a method 
    dispatchCall() which assigns a call to the first available employee */


#include <iostream>
#include <vector>
#include <queue>
using namespace std;

// ---------------- ENUM ----------------
enum Rank {
    RESPONDENT = 0,
    MANAGER,
    DIRECTOR
};

// Forward declarations
class Employee;
class CallHandler;

// ---------------- CALL CLASS ----------------
class Call {
private:
    Rank rank;
    Employee* handler;

public:
    Call() {
        rank = RESPONDENT;
        handler = nullptr;
    }

    void setHandler(Employee* emp) {
        handler = emp;
    }

    Rank getRank() {
        return rank;
    }

    void incrementRank() {
        if (rank == RESPONDENT) rank = MANAGER;
        else if (rank == MANAGER) rank = DIRECTOR;
    }
};

// ---------------- EMPLOYEE BASE CLASS ----------------
class Employee {
protected:
    Rank rank;
    Call* currentCall;
    CallHandler* callHandler;

public:
    Employee(CallHandler* handler, Rank r) {
        this->callHandler = handler;
        this->rank = r;
        this->currentCall = nullptr;
    }

    bool isFree() {
        return currentCall == nullptr;
    }

    Rank getRank() {
        return rank;
    }

    void receiveCall(Call* call) {
        currentCall = call;
        call->setHandler(this);
        cout << "Call assigned to employee of rank: " << rank << endl;
    }

    void completeCall();

    void escalateCall();

    virtual ~Employee() {}
};

// ---------------- DERIVED CLASSES ----------------
class Respondent : public Employee {
public:
    Respondent(CallHandler* handler) : Employee(handler, RESPONDENT) {}
};

class Manager : public Employee {
public:
    Manager(CallHandler* handler) : Employee(handler, MANAGER) {}
};

class Director : public Employee {
public:
    Director(CallHandler* handler) : Employee(handler, DIRECTOR) {}
};

// ---------------- CALL HANDLER ----------------
class CallHandler {
private:
    static const int LEVELS = 3;

    vector<vector<Employee*>> employees;   // employees[level]
    vector<queue<Call*>> callQueues;       // waiting calls[level]

public:
    CallHandler(int numRespondents, int numManagers, int numDirectors) {
        employees.resize(LEVELS);
        callQueues.resize(LEVELS);

        // Create employees
        for (int i = 0; i < numRespondents; i++)
            employees[RESPONDENT].push_back(new Respondent(this));

        for (int i = 0; i < numManagers; i++)
            employees[MANAGER].push_back(new Manager(this));

        for (int i = 0; i < numDirectors; i++)
            employees[DIRECTOR].push_back(new Director(this));
    }

    // Get first available employee
    Employee* getHandlerForCall(Call* call) {
        for (int level = call->getRank(); level < LEVELS; level++) {
            for (Employee* emp : employees[level]) {
                if (emp->isFree()) {
                    return emp;
                }
            }
        }
        return nullptr;
    }

    // MAIN FUNCTION
    void dispatchCall(Call* call) {
        Employee* emp = getHandlerForCall(call);

        if (emp != nullptr) {
            emp->receiveCall(call);
        } else {
            cout << "No available employee. Call queued at level: " << call->getRank() << endl;
            callQueues[call->getRank()].push(call);
        }
    }

    // Assign queued calls when employee becomes free
    void assignCall(Employee* emp) {
        for (int level = emp->getRank(); level >= 0; level--) {
            if (!callQueues[level].empty()) {
                Call* call = callQueues[level].front();
                callQueues[level].pop();
                emp->receiveCall(call);
                return;
            }
        }
    }
};

// ---------------- EMPLOYEE METHODS ----------------
void Employee::completeCall() {
    if (currentCall != nullptr) {
        cout << "Call completed by employee of rank: " << rank << endl;
        currentCall = nullptr;
        callHandler->assignCall(this);
    }
}

void Employee::escalateCall() {
    if (currentCall != nullptr) {
        cout << "Escalating call from rank: " << rank << endl;
        currentCall->incrementRank();
        callHandler->dispatchCall(currentCall);
        currentCall = nullptr;
    }
}

// ---------------- MAIN ----------------
int main() {
    CallHandler handler(2, 1, 1);

    Call* c1 = new Call();
    Call* c2 = new Call();
    Call* c3 = new Call();
    Call* c4 = new Call();

    handler.dispatchCall(c1);
    handler.dispatchCall(c2);
    handler.dispatchCall(c3);
    handler.dispatchCall(c4);

    return 0;
}
