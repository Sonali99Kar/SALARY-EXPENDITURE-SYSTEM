#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include <limits>
#include <cmath>
#include <algorithm>
#include <sstream>

using namespace std;

// ============================================================================
// UI Formatting & Console Helpers
// ============================================================================
namespace UI {
    void header(const string& title) {
        cout << "\n======================================================================\n";
        cout << "  " << title << "\n";
        cout << "======================================================================\n";
    }

    void divider(char ch = '-', int len = 70) {
        cout << string(len, ch) << "\n";
    }
}

// ============================================================================
// Robust Input Handling
// ============================================================================
namespace InputHelper {
    double getNonNegativeDouble(const string& prompt) {
        double val;
        while (true) {
            cout << prompt;
            if (cin >> val && val >= 0.0) {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return val;
            }
            cout << "  [!] Invalid input. Please enter a positive number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }

    int getInteger(const string& prompt, int minVal = 0, int maxVal = 999999) {
        int val;
        while (true) {
            cout << prompt;
            if (cin >> val && val >= minVal && val <= maxVal) {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return val;
            }
            cout << "  [!] Please enter a valid number (" << minVal << " to " << maxVal << ").\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }

    string getLine(const string& prompt) {
        string val;
        while (true) {
            cout << prompt;
            getline(cin >> ws, val);
            if (!val.empty()) return val;
            cout << "  [!] Field cannot be empty.\n";
        }
    }
}

// ============================================================================
// Core Domain Models (OOP: Polymorphism & Encapsulation)
// ============================================================================
enum class ExpenseClassification { Need, Want, Investment };

string classificationToString(ExpenseClassification c) {
    switch (c) {
        case ExpenseClassification::Need: return "Need (Essential)";
        case ExpenseClassification::Want: return "Want (Discretionary)";
        case ExpenseClassification::Investment: return "Investment/Debt";
    }
    return "Other";
}

class Expense {
protected:
    string category;
    string description;
    double amount;
    ExpenseClassification classification;

public:
    Expense(string cat, string desc, double amt, ExpenseClassification cls)
        : category(move(cat)), description(move(desc)), amount(amt), classification(cls) {}

    virtual ~Expense() = default;

    virtual string getType() const = 0;

    double getAmount() const noexcept { return amount; }
    string getCategory() const { return category; }
    string getDescription() const { return description; }
    ExpenseClassification getClassification() const noexcept { return classification; }

    virtual void displayRow(int index) const {
        cout << left << setw(4) << index
             << setw(15) << category
             << setw(22) << (description.length() > 20 ? description.substr(0, 17) + "..." : description)
             << setw(12) << getType()
             << setw(16) << classificationToString(classification)
             << right << fixed << setprecision(2) << setw(12) << amount << "\n";
    }

    virtual void serialize(ofstream& out) const {
        out << getType() << '\n'
            << static_cast<int>(classification) << '\n'
            << category << '\n'
            << description << '\n'
            << amount << '\n';
    }
};

class FixedExpense : public Expense {
public:
    FixedExpense(string cat, string desc, double amt, ExpenseClassification cls)
        : Expense(move(cat), move(desc), amt, cls) {}

    string getType() const override { return "Fixed"; }
};

class VariableExpense : public Expense {
public:
    VariableExpense(string cat, string desc, double amt, ExpenseClassification cls)
        : Expense(move(cat), move(desc), amt, cls) {}

    string getType() const override { return "Variable"; }
};

// ============================================================================
// Salary & Income Structure
// ============================================================================
class Salary {
private:
    double basic;
    double hraAndAllowances;
    double bonuses;
    double taxDeductions;
    double retirementDeductions; // PF/NPS

public:
    Salary() : basic(0), hraAndAllowances(0), bonuses(0), taxDeductions(0), retirementDeductions(0) {}

    void promptInput() {
        UI::header("Monthly Income Configuration");
        basic = InputHelper::getNonNegativeDouble("Enter Basic Salary (Rs.): ");
        hraAndAllowances = InputHelper::getNonNegativeDouble("Enter Allowances & HRA (Rs.): ");
        bonuses = InputHelper::getNonNegativeDouble("Enter Performance Bonus / Other Monthly Income (Rs.): ");
        taxDeductions = InputHelper::getNonNegativeDouble("Enter Estimated Monthly Tax Deductions (TDS) (Rs.): ");
        retirementDeductions = InputHelper::getNonNegativeDouble("Enter EPF / NPS Retirement Deductions (Rs.): ");

        cout << "\n[OK] Income structure successfully updated.\n";
    }

    double getGross() const noexcept { return basic + hraAndAllowances + bonuses; }
    double getTotalDeductions() const noexcept { return taxDeductions + retirementDeductions; }
    double getNet() const noexcept { return max(0.0, getGross() - getTotalDeductions()); }

    void display() const {
        UI::header("Compensation & Tax Breakdown");
        cout << fixed << setprecision(2);
        cout << left << setw(35) << "  Basic Salary:" << "Rs. " << setw(15) << right << basic << "\n";
        cout << left << setw(35) << "  HRA & Allowances:" << "Rs. " << setw(15) << right << hraAndAllowances << "\n";
        cout << left << setw(35) << "  Bonus / Variable Inflow:" << "Rs. " << setw(15) << right << bonuses << "\n";
        UI::divider();
        cout << left << setw(35) << "  GROSS SALARY:" << "Rs. " << setw(15) << right << getGross() << "\n";
        cout << left << setw(35) << "  Tax Deducted (TDS):" << "Rs. " << setw(15) << right << taxDeductions << "\n";
        cout << left << setw(35) << "  Provident Fund / Retirals:" << "Rs. " << setw(15) << right << retirementDeductions << "\n";
        UI::divider();
        cout << left << setw(35) << "  TAKE-HOME (NET SALARY):" << "Rs. " << setw(15) << right << getNet() << "\n";
    }

    void serialize(ofstream& out) const {
        out << basic << '\n' << hraAndAllowances << '\n' << bonuses << '\n'
            << taxDeductions << '\n' << retirementDeductions << '\n';
    }

    bool deserialize(ifstream& in) {
        return static_cast<bool>(in >> basic >> hraAndAllowances >> bonuses >> taxDeductions >> retirementDeductions);
    }
};

// ============================================================================
// Goal Allocation & Algorithmic Simulation
// ============================================================================
class FinancialGoal {
private:
    string name;
    double targetAmount;
    double currentSaved;
    int priorityLevel; // 1: Highest (Emergency fund), 2: Medium, 3: Long term

public:
    FinancialGoal(string n, double target, double current, int priority)
        : name(move(n)), targetAmount(target), currentSaved(current), priorityLevel(priority) {}

    string getName() const { return name; }
    double getTarget() const noexcept { return targetAmount; }
    double getSaved() const noexcept { return currentSaved; }
    double getRemaining() const noexcept { return max(0.0, targetAmount - currentSaved); }
    int getPriority() const noexcept { return priorityLevel; }

    void addFunds(double amount) { currentSaved += amount; }

    void serialize(ofstream& out) const {
        out << name << '\n' << targetAmount << '\n' << currentSaved << '\n' << priorityLevel << '\n';
    }

    static unique_ptr<FinancialGoal> deserialize(ifstream& in) {
        string n;
        double t, s;
        int p;
        in >> ws;
        if (!getline(in, n)) return nullptr;
        if (in >> t >> s >> p) {
            return make_unique<FinancialGoal>(n, t, s, p);
        }
        return nullptr;
    }
};

// ============================================================================
// Core Salary Management System Engine
// ============================================================================
class SalaryManagementEngine {
private:
    Salary salary;
    vector<unique_ptr<Expense>> expenses;
    vector<unique_ptr<FinancialGoal>> goals;
    double monthlyBudgetLimit = 0.0;
    double liquidSavingsAccount = 0.0;

public:
    SalaryManagementEngine() = default;

    void configureSalary() { salary.promptInput(); }
    void showSalary() const { salary.display(); }

    void setBudgetLimit() {
        UI::header("Budget Envelope Setup");
        monthlyBudgetLimit = InputHelper::getNonNegativeDouble("Set Max Monthly Expense Ceiling (Rs.): ");
        liquidSavingsAccount = InputHelper::getNonNegativeDouble("Current Liquid Emergency Cash in Bank (Rs.): ");
        cout << "[OK] Spending ceiling registered.\n";
    }

    void addExpenseRecord() {
        UI::header("Log New Expenditure");
        string cat = InputHelper::getLine("Category (e.g., Rent, Food, Utilities, Leisure, EMIs): ");
        string desc = InputHelper::getLine("Description/Vendor: ");
        double amt = InputHelper::getNonNegativeDouble("Amount (Rs.): ");

        cout << "\nExpense Nature:\n  1. Fixed Commitment\n  2. Variable Discretionary\n";
        int tChoice = InputHelper::getInteger("Selection (1-2): ", 1, 2);

        cout << "\nExpense Classification (50/30/20 Standard):\n";
        cout << "  1. Need (Survival, bills, rent, groceries)\n";
        cout << "  2. Want (Dining out, streaming, travel, hobby)\n";
        cout << "  3. Investment / Debt Repayment (SIP, loan prepayment)\n";
        int cChoice = InputHelper::getInteger("Selection (1-3): ", 1, 3);

        ExpenseClassification cls = ExpenseClassification::Need;
        if (cChoice == 2) cls = ExpenseClassification::Want;
        else if (cChoice == 3) cls = ExpenseClassification::Investment;

        if (tChoice == 1) {
            expenses.push_back(make_unique<FixedExpense>(cat, desc, amt, cls));
        } else {
            expenses.push_back(make_unique<VariableExpense>(cat, desc, amt, cls));
        }
        cout << "[OK] Expense logged in ledger.\n";
    }

    double getTotalExpenditure() const {
        double sum = 0;
        for (const auto& e : expenses) sum += e->getAmount();
        return sum;
    }

    double getMonthlySurplus() const {
        return salary.getNet() - getTotalExpenditure();
    }

    void renderLedger() const {
        UI::header("Monthly Expense Ledger");
        if (expenses.empty()) {
            cout << "No transactions recorded for this cycle.\n";
            return;
        }

        cout << left << setw(4) << "#"
             << setw(15) << "Category"
             << setw(22) << "Description"
             << setw(12) << "Type"
             << setw(16) << "Class"
             << right << setw(12) << "Amount (Rs.)\n";
        UI::divider();

        int idx = 1;
        for (const auto& exp : expenses) {
            exp->displayRow(idx++);
        }
        UI::divider();
        cout << fixed << setprecision(2);
        cout << left << setw(69) << "  TOTAL EXPENDITURE:" << "Rs. " << setw(12) << right << getTotalExpenditure() << "\n";
    }

    void deleteExpenseRecord() {
        renderLedger();
        if (expenses.empty()) return;
        int idx = InputHelper::getInteger("Enter transaction # to delete (0 to abort): ", 0, static_cast<int>(expenses.size()));
        if (idx == 0) return;
        expenses.erase(expenses.begin() + (idx - 1));
        cout << "[OK] Transaction eliminated.\n";
    }

    // ------------------------------------------------------------------------
    // Algorithmic Feature: 50/30/20 Rule Analysis + Plain Text Progress Bar
    // ------------------------------------------------------------------------
    void analyze50_30_20Rule() const {
        UI::header("50/30/20 Budget Optimization Analyzer");
        double netIncome = salary.getNet();
        if (netIncome <= 0) {
            cout << "[!] Net income must be entered to perform 50/30/20 analytics.\n";
            return;
        }

        double needsSpent = 0, wantsSpent = 0, investmentSpent = 0;
        for (const auto& exp : expenses) {
            if (exp->getClassification() == ExpenseClassification::Need) needsSpent += exp->getAmount();
            else if (exp->getClassification() == ExpenseClassification::Want) wantsSpent += exp->getAmount();
            else investmentSpent += exp->getAmount();
        }

        double surplus = getMonthlySurplus();
        double totalSavingsAndSurplus = investmentSpent + max(0.0, surplus);

        double pctNeeds = (needsSpent / netIncome) * 100.0;
        double pctWants = (wantsSpent / netIncome) * 100.0;
        double pctSavings = (totalSavingsAndSurplus / netIncome) * 100.0;

        auto printBar = [](double actualPct, double targetPct) {
            int filled = min(40, static_cast<int>(actualPct / 2.5));
            cout << "  [";
            for (int i = 0; i < filled; ++i) cout << "#";
            for (int i = filled; i < 40; ++i) cout << " ";
            cout << "] " << fixed << setprecision(1) << actualPct << "% (Benchmark: " << targetPct << "%)\n";
        };

        cout << "\n1. Needs (Target: <= 50% | Essential Living)\n";
        cout << "   Current: Rs. " << needsSpent << "\n";
        printBar(pctNeeds, 50.0);

        cout << "\n2. Wants (Target: <= 30% | Discretionary Lifestyle)\n";
        cout << "   Current: Rs. " << wantsSpent << "\n";
        printBar(pctWants, 30.0);

        cout << "\n3. Savings & Investments (Target: >= 20% | Future Wealth)\n";
        cout << "   Current (Investments + Surplus): Rs. " << totalSavingsAndSurplus << "\n";
        printBar(pctSavings, 20.0);

        cout << "\nDiagnostic Verdict:\n";
        if (pctNeeds > 55.0) cout << "  * High Fixed Overhead: Your non-negotiable living costs are choking savings.\n";
        if (pctWants > 35.0) cout << "  * Lifestyle Creep: High discretionary spending identified. Trim non-essential items.\n";
        if (pctSavings >= 25.0) cout << "  * High Velocity Saver: Superb discipline! Channel excess into high-yield vehicles.\n";
    }

    // ------------------------------------------------------------------------
    // Algorithmic Feature: Emergency Runway & Cash Burn Diagnostic
    // ------------------------------------------------------------------------
    void calculateEmergencyRunway() const {
        UI::header("Survival Burn Rate & Emergency Runway");
        double monthlyBurn = getTotalExpenditure();
        if (monthlyBurn <= 0) {
            cout << "No expenses logged to compute burn rate.\n";
            return;
        }

        double months = liquidSavingsAccount / monthlyBurn;
        cout << fixed << setprecision(2);
        cout << "  Monthly Burn Rate     : Rs. " << monthlyBurn << "\n";
        cout << "  Liquid Bank Cushion   : Rs. " << liquidSavingsAccount << "\n";
        cout << "  Emergency Runway      : ";

        if (months < 3.0) {
            cout << months << " Months (CRITICAL: Below 3 months threshold!)\n";
            cout << "  Action Plan: Build a baseline 3-6 month safety net before aggressive investing.\n";
        } else if (months <= 6.0) {
            cout << months << " Months (Adequate Baseline)\n";
            cout << "  Action Plan: Healthy cushion. Maintain liquidity against unexpected medical/loss shocks.\n";
        } else {
            cout << months << " Months (Fortified Stability)\n";
            cout << "  Action Plan: Strong foundation. Excess emergency funds may be reinvested.\n";
        }
    }

    // ------------------------------------------------------------------------
    // Algorithmic Feature: Multi-Goal Waterfall Allocation Simulator
    // ------------------------------------------------------------------------
    void addGoal() {
        UI::header("Register Savings Target");
        string name = InputHelper::getLine("Target Goal Name (e.g., Laptop, Emergency Corpus, Vacation): ");
        double target = InputHelper::getNonNegativeDouble("Target Price / Corpus (Rs.): ");
        double current = InputHelper::getNonNegativeDouble("Initial Capital Already Allocated (Rs.): ");
        int p = InputHelper::getInteger("Priority Tier (1=Crucial/Immediate, 2=Medium-term, 3=Aspirational): ", 1, 3);

        goals.push_back(make_unique<FinancialGoal>(name, target, current, p));
        cout << "[OK] Goal registered in financial roadmap.\n";
    }

    void simulateGoalCascade() const {
        UI::header("Multi-Goal Waterfall Dispatch Simulation");
        if (goals.empty()) {
            cout << "No active purchase goals registered.\n";
            return;
        }

        double surplus = getMonthlySurplus();
        cout << fixed << setprecision(2);
        cout << "Current Net Monthly Investable Surplus: Rs. " << surplus << "\n\n";

        if (surplus <= 0) {
            cout << "[!] Cannot run waterfall simulation without positive monthly cash surplus.\n";
            return;
        }

        vector<FinancialGoal*> sortedGoals;
        for (const auto& g : goals) sortedGoals.push_back(g.get());
        sort(sortedGoals.begin(), sortedGoals.end(), [](FinancialGoal* a, FinancialGoal* b) {
            return a->getPriority() < b->getPriority();
        });

        cout << left << setw(5) << "Tier" 
             << setw(25) << "Goal Objective" 
             << setw(15) << "Target (Rs.)" 
             << setw(15) << "Funded (Rs.)" 
             << setw(15) << "Timeline\n";
        UI::divider();

        double cumulativeMonths = 0;
        for (const auto* g : sortedGoals) {
            double needed = g->getRemaining();
            double monthsForThisGoal = (needed <= 0) ? 0 : (needed / surplus);
            cumulativeMonths += monthsForThisGoal;

            cout << left << setw(5) << g->getPriority()
                 << setw(25) << g->getName()
                 << setw(15) << g->getTarget()
                 << setw(15) << g->getSaved();

            if (needed <= 0) {
                cout << "Fully Funded\n";
            } else {
                cout << ceil(cumulativeMonths) << " Month(s)\n";
            }
        }
        cout << "\n(*) Uses Waterfall Capital Allocation: higher-priority targets are cleared sequentially.\n";
    }

    // ------------------------------------------------------------------------
    // Algorithmic Feature: Compound Wealth Projection Simulator
    // ------------------------------------------------------------------------
    void simulateWealthGrowth() const {
        UI::header("Surplus Compounding & Wealth Trajectory");
        double surplus = getMonthlySurplus();
        if (surplus <= 0) {
            cout << "Positive monthly surplus required to project investment returns.\n";
            return;
        }

        cout << fixed << setprecision(2);
        cout << "Deploying Monthly Investable Surplus of: Rs. " << surplus << "\n";
        double expectedAnnualReturn = InputHelper::getNonNegativeDouble("Assumed Annualized ROI % (e.g., 12% Equity Index, 7% Debt): ");
        int horizonYears = InputHelper::getInteger("Investment Horizon (in Years): ", 1, 40);

        double monthlyRate = (expectedAnnualReturn / 100.0) / 12.0;
        int totalMonths = horizonYears * 12;

        double futureValue = 0;
        if (monthlyRate == 0) {
            futureValue = surplus * totalMonths;
        } else {
            futureValue = surplus * ((pow(1 + monthlyRate, totalMonths) - 1) / monthlyRate) * (1 + monthlyRate);
        }

        double totalInvested = surplus * totalMonths;
        double wealthGain = futureValue - totalInvested;

        cout << "\n--- Wealth Projection After " << horizonYears << " Years ---\n";
        cout << left << setw(35) << "Total Capital Outlay:" << "Rs. " << setw(15) << right << totalInvested << "\n";
        cout << left << setw(35) << "Estimated Capital Gains:" << "Rs. " << setw(15) << right << wealthGain << "\n";
        UI::divider();
        cout << left << setw(35) << "PROJECTED CORPUS:" << "Rs. " << setw(15) << right << futureValue << "\n";
    }

    // ------------------------------------------------------------------------
    // CSV Export & Persistent State Storage
    // ------------------------------------------------------------------------
    void exportToCSV() const {
        ofstream csvFile("salary_management_export.csv");
        if (!csvFile) {
            cout << "[!] Failed to generate CSV.\n";
            return;
        }

        csvFile << "Category,Description,Type,Classification,Amount\n";
        for (const auto& e : expenses) {
            csvFile << "\"" << e->getCategory() << "\","
                    << "\"" << e->getDescription() << "\","
                    << "\"" << e->getType() << "\","
                    << "\"" << classificationToString(e->getClassification()) << "\","
                    << e->getAmount() << "\n";
        }
        cout << "[OK] Ledger exported successfully as 'salary_management_export.csv'.\n";
    }

    void persistState() const {
        ofstream out("salary_management_state.dat");
        if (!out) {
            cout << "[!] File I/O Error saving binary/text state.\n";
            return;
        }
        salary.serialize(out);
        out << monthlyBudgetLimit << '\n';
        out << liquidSavingsAccount << '\n';
        out << expenses.size() << '\n';
        for (const auto& e : expenses) e->serialize(out);

        out << goals.size() << '\n';
        for (const auto& g : goals) g->serialize(out);

        cout << "[OK] All records securely saved to disk ('salary_management_state.dat').\n";
    }

    void restoreState() {
        ifstream in("salary_management_state.dat");
        if (!in) {
            cout << "No previous state database detected.\n";
            return;
        }

        if (!salary.deserialize(in)) {
            cout << "[!] Corrupted state file.\n";
            return;
        }

        in >> monthlyBudgetLimit >> liquidSavingsAccount;
        size_t expCount = 0;
        in >> expCount;

        expenses.clear();
        for (size_t i = 0; i < expCount; ++i) {
            string type, cat, desc;
            int clsInt;
            double amt;
            in >> ws;
            getline(in, type);
            in >> clsInt;
            in >> ws;
            getline(in, cat);
            getline(in, desc);
            in >> amt;

            ExpenseClassification cls = static_cast<ExpenseClassification>(clsInt);
            if (type == "Fixed") {
                expenses.push_back(make_unique<FixedExpense>(cat, desc, amt, cls));
            } else {
                expenses.push_back(make_unique<VariableExpense>(cat, desc, amt, cls));
            }
        }

        size_t goalCount = 0;
        in >> goalCount;
        goals.clear();
        for (size_t i = 0; i < goalCount; ++i) {
            auto g = FinancialGoal::deserialize(in);
            if (g) goals.push_back(move(g));
        }

        cout << "[OK] State successfully reconstructed from disk.\n";
    }
};

// ============================================================================
// Main Execution Loop & Clean Terminal Interface
// ============================================================================
int main() {
    SalaryManagementEngine engine;
    engine.restoreState();

    while (true) {
        cout << "\n======================================================================\n";
        cout << "              SALARY MANAGEMENT SYSTEM (C++ ENGINE)                \n";
        cout << "======================================================================\n";
        cout << " 1. Configure Monthly Salary & Deductions\n";
        cout << " 2. View Salary & Tax Summary\n";
        cout << " 3. Log an Expenditure (Fixed/Variable)\n";
        cout << " 4. View Master Monthly Expense Ledger\n";
        cout << " 5. Delete an Expense Record\n";
        cout << " 6. Set Budget Envelopes & Cash Cushion\n";
        cout << " 7. Run 50/30/20 Rule Analytical Benchmark\n";
        cout << " 8. Run Emergency Survival Runway & Burn-Rate Check\n";
        cout << " 9. Register a Financial Goal / Purchase Target\n";
        cout << "10. Run Multi-Goal Waterfall Savings Simulation\n";
        cout << "11. Run Compound Wealth / SIP Growth Projection\n";
        cout << "12. Export Ledger to CSV (Analytics-Ready)\n";
        cout << "13. Save State to Disk\n";
        cout << " 0. Save & Exit Program\n";
        UI::divider();

        int choice = InputHelper::getInteger("Enter Choice (0-13): ", 0, 13);
        switch (choice) {
            case 1:  engine.configureSalary(); break;
            case 2:  engine.showSalary(); break;
            case 3:  engine.addExpenseRecord(); break;
            case 4:  engine.renderLedger(); break;
            case 5:  engine.deleteExpenseRecord(); break;
            case 6:  engine.setBudgetLimit(); break;
            case 7:  engine.analyze50_30_20Rule(); break;
            case 8:  engine.calculateEmergencyRunway(); break;
            case 9:  engine.addGoal(); break;
            case 10: engine.simulateGoalCascade(); break;
            case 11: engine.simulateWealthGrowth(); break;
            case 12: engine.exportToCSV(); break;
            case 13: engine.persistState(); break;
            case 0:
                engine.persistState();
                cout << "\nSession committed. Thank you for using Salary Management System.\n";
                return 0;
        }
    }
}