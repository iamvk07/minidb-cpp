/**
 * MiniDB — Lightweight In-Memory Database Engine
 * Author: Vedant Kadam
 * GitHub: github.com/iamvk07
 *
 * A simple in-memory database supporting basic SQL-like operations.
 * Demonstrates: hash maps, sorting, file I/O, string parsing, templates
 */

#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <functional>

// ── ROW ──────────────────────────────────────────────────────────────────────

struct Row {
    int id;
    std::unordered_map<std::string, std::string> fields;

    Row(int id) : id(id) {}

    std::string get(const std::string& key) const {
        auto it = fields.find(key);
        return it != fields.end() ? it->second : "";
    }

    void set(const std::string& key, const std::string& value) {
        fields[key] = value;
    }
};

// ── TABLE ─────────────────────────────────────────────────────────────────────

class Table {
private:
    std::string name;
    std::vector<std::string> columns;
    std::vector<Row> rows;
    int nextId = 1;

public:
    explicit Table(const std::string& name, const std::vector<std::string>& cols)
        : name(name), columns(cols) {}

    // INSERT
    int insert(const std::unordered_map<std::string, std::string>& data) {
        Row row(nextId++);
        for (const auto& col : columns) {
            auto it = data.find(col);
            row.set(col, it != data.end() ? it->second : "");
        }
        rows.push_back(row);
        return row.id;
    }

    // SELECT with optional WHERE predicate
    std::vector<Row> select(
        const std::function<bool(const Row&)>& predicate = nullptr,
        const std::string& orderBy = "",
        bool ascending = true,
        int limit = -1
    ) const {
        std::vector<Row> result;
        for (const auto& row : rows) {
            if (!predicate || predicate(row)) {
                result.push_back(row);
            }
        }

        // ORDER BY
        if (!orderBy.empty()) {
            std::sort(result.begin(), result.end(), [&](const Row& a, const Row& b) {
                const std::string& va = a.get(orderBy);
                const std::string& vb = b.get(orderBy);
                // Try numeric comparison first
                try {
                    double da = std::stod(va), db = std::stod(vb);
                    return ascending ? da < db : da > db;
                } catch (...) {
                    return ascending ? va < vb : va > vb;
                }
            });
        }

        // LIMIT
        if (limit > 0 && (int)result.size() > limit) {
            result.resize(limit);
        }

        return result;
    }

    // UPDATE
    int update(
        const std::function<bool(const Row&)>& predicate,
        const std::unordered_map<std::string, std::string>& updates
    ) {
        int count = 0;
        for (auto& row : rows) {
            if (predicate(row)) {
                for (const auto& [key, val] : updates) {
                    row.set(key, val);
                }
                count++;
            }
        }
        return count;
    }

    // DELETE
    int remove(const std::function<bool(const Row&)>& predicate) {
        int before = rows.size();
        rows.erase(
            std::remove_if(rows.begin(), rows.end(), predicate),
            rows.end()
        );
        return before - rows.size();
    }

    // COUNT
    int count(const std::function<bool(const Row&)>& predicate = nullptr) const {
        if (!predicate) return rows.size();
        return std::count_if(rows.begin(), rows.end(), predicate);
    }

    // PRINT table to console
    void print(const std::vector<Row>& data) const {
        if (data.empty()) { std::cout << "  (empty)\n"; return; }

        // Column widths
        std::unordered_map<std::string, size_t> widths;
        widths["id"] = 4;
        for (const auto& col : columns) widths[col] = col.size();
        for (const auto& row : data) {
            widths["id"] = std::max(widths["id"], std::to_string(row.id).size());
            for (const auto& col : columns)
                widths[col] = std::max(widths[col], row.get(col).size());
        }

        // Header
        auto printSep = [&]() {
            std::cout << "  +--" << std::string(widths["id"], '-') << "--";
            for (const auto& col : columns)
                std::cout << "+" << std::string(widths[col]+2, '-');
            std::cout << "+\n";
        };

        printSep();
        std::cout << "  | " << std::left << std::setw(widths["id"]) << "id" << "  ";
        for (const auto& col : columns)
            std::cout << "| " << std::setw(widths[col]) << col << " ";
        std::cout << "|\n";
        printSep();

        for (const auto& row : data) {
            std::cout << "  | " << std::setw(widths["id"]) << row.id << "  ";
            for (const auto& col : columns)
                std::cout << "| " << std::setw(widths[col]) << row.get(col) << " ";
            std::cout << "|\n";
        }
        printSep();
    }

    // CSV export
    void exportCSV(const std::string& filename) const {
        std::ofstream file(filename);
        file << "id";
        for (const auto& col : columns) file << "," << col;
        file << "\n";
        for (const auto& row : rows) {
            file << row.id;
            for (const auto& col : columns) file << "," << row.get(col);
            file << "\n";
        }
        std::cout << "  Exported " << rows.size() << " rows to " << filename << "\n";
    }

    std::string getName() const { return name; }
    const std::vector<std::string>& getColumns() const { return columns; }
    int size() const { return rows.size(); }
};

// ── DATABASE ──────────────────────────────────────────────────────────────────

class Database {
private:
    std::string name;
    std::unordered_map<std::string, Table*> tables;

public:
    explicit Database(const std::string& name) : name(name) {}

    ~Database() {
        for (auto& [_, table] : tables) delete table;
    }

    Table& createTable(const std::string& tableName, const std::vector<std::string>& cols) {
        if (tables.count(tableName)) throw std::runtime_error("Table already exists: " + tableName);
        tables[tableName] = new Table(tableName, cols);
        std::cout << "  Table '" << tableName << "' created.\n";
        return *tables[tableName];
    }

    Table& getTable(const std::string& tableName) {
        auto it = tables.find(tableName);
        if (it == tables.end()) throw std::runtime_error("Table not found: " + tableName);
        return *it->second;
    }

    void listTables() const {
        std::cout << "\n  Database: " << name << "\n";
        std::cout << "  Tables: " << tables.size() << "\n";
        for (const auto& [name, table] : tables)
            std::cout << "    - " << name << " (" << table->size() << " rows)\n";
    }

    std::string getName() const { return name; }
};

// ── DEMO ─────────────────────────────────────────────────────────────────────

int main() {
    std::cout << "\n";
    std::cout << "  ╔══════════════════════════════════╗\n";
    std::cout << "  ║   MiniDB — In-Memory Database    ║\n";
    std::cout << "  ║   by Vedant Kadam                ║\n";
    std::cout << "  ╚══════════════════════════════════╝\n\n";

    Database db("studentdb");

    // Create students table
    Table& students = db.createTable("students", {"name", "major", "gpa", "year"});

    // Insert records
    std::cout << "\n  INSERT 6 students...\n";
    students.insert({{"name","Alice Chen"},{"major","Computer Science"},{"gpa","3.9"},{"year","3"}});
    students.insert({{"name","Bob Smith"},{"major","Mathematics"},{"gpa","3.4"},{"year","2"}});
    students.insert({{"name","Carol Davis"},{"major","Computer Science"},{"gpa","3.7"},{"year","4"}});
    students.insert({{"name","Dave Wilson"},{"major","Physics"},{"gpa","3.1"},{"year","1"}});
    students.insert({{"name","Eve Johnson"},{"major","Computer Science"},{"gpa","3.8"},{"year","2"}});
    students.insert({{"name","Vedant Kadam"},{"major","Computer Science"},{"gpa","3.5"},{"year","3"}});

    // SELECT all
    std::cout << "\n  SELECT * FROM students:\n";
    auto all = students.select();
    students.print(all);

    // SELECT WHERE
    std::cout << "\n  SELECT * FROM students WHERE major = 'Computer Science':\n";
    auto csStudents = students.select([](const Row& r) {
        return r.get("major") == "Computer Science";
    });
    students.print(csStudents);

    // SELECT with ORDER BY
    std::cout << "\n  SELECT * FROM students ORDER BY gpa DESC LIMIT 3:\n";
    auto topStudents = students.select(nullptr, "gpa", false, 3);
    students.print(topStudents);

    // COUNT
    std::cout << "\n  COUNT WHERE major = 'Computer Science': "
              << students.count([](const Row& r){ return r.get("major") == "Computer Science"; })
              << "\n";

    // UPDATE
    std::cout << "\n  UPDATE students SET year = '4' WHERE name = 'Vedant Kadam'\n";
    int updated = students.update(
        [](const Row& r){ return r.get("name") == "Vedant Kadam"; },
        {{"year", "4"}}
    );
    std::cout << "  " << updated << " row(s) updated.\n";

    // DELETE
    std::cout << "\n  DELETE FROM students WHERE gpa < '3.2'\n";
    int deleted = students.remove([](const Row& r){
        try { return std::stod(r.get("gpa")) < 3.2; } catch(...) { return false; }
    });
    std::cout << "  " << deleted << " row(s) deleted.\n";

    // Final state
    std::cout << "\n  Final table state:\n";
    students.print(students.select());

    // Export
    students.exportCSV("students_export.csv");

    // DB info
    db.listTables();

    std::cout << "\n  Done.\n\n";
    return 0;
}
