#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <utility>
#include <stdexcept>
#include "../Graph/Graph.h"
#include <unordered_map>

class GraphLoader {
public:
    static std::vector<Edge> loadFromCSV(const std::string& filename) {
        std::vector<Edge> edges;
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("failed to open file: " + filename);
        }

        static const std::vector<std::string> paramNames = {
            "Rj", "Qjstat", "Hvj", "Fj", "Pj", "aj", "Lj", "Kj"
        };

        std::string line;
        int auto_id = 0;
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string f, t, idstr;
            if (!std::getline(ss, f, ',')) continue;
            if (!std::getline(ss, t, ',')) continue;

            // trim spaces
            auto trim = [](std::string& s){
                size_t a = s.find_first_not_of(" \t");
                size_t b = s.find_last_not_of(" \t");
                if (a == std::string::npos) { s.clear(); return; }
                s = s.substr(a, b - a + 1);
            };

            trim(f); trim(t);
            int from = std::stoi(f);
            int to = std::stoi(t);
            int id;

            if (std::getline(ss, idstr, ',')) {
                trim(idstr);
                if (!idstr.empty()) {
                    id = std::stoi(idstr);
                } else {
                    id = auto_id++;
                }
            } else {
                id = auto_id++;
            }

            std::unordered_map<std::string, double> attributes;
            for (const auto& param : paramNames) {
                if (std::getline(ss, idstr, ',')) {
                    trim(idstr);
                    if (!idstr.empty()) {
                        attributes[param] = std::stod(idstr);
                    }
                }
            }

            edges.push_back(Edge{from, to, id, attributes});
        }
        return edges;
    }

    static std::vector<Edge> loadFromTXT(const std::string& filename) {
        std::vector<Edge> edges;
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("failed to open file: " + filename);
        }

        static const std::vector<std::string> paramNames = {
            "Rj", "Qjstat", "Hvj", "Fj", "Pj", "aj", "Lj", "Kj"
        };

        std::string line;
        int auto_id = 0;
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            std::stringstream ss(line);
            std::string f, t, idstr;
            if (!(ss >> f)) continue;
            if (!(ss >> t)) continue;
            int from = std::stoi(f);
            int to = std::stoi(t);
            int id;
            if (ss >> idstr) {
                id = std::stoi(idstr);
            } else {
                id = auto_id++;
            }

            auto trim = [](std::string& s){
                size_t a = s.find_first_not_of(" \t");
                size_t b = s.find_last_not_of(" \t");
                if (a == std::string::npos) { s.clear(); return; }
                s = s.substr(a, b - a + 1);
            };

            std::unordered_map<std::string, double> attributes;
            for (const auto& param : paramNames) {
                if (std::getline(ss, idstr, ',')) {
                    trim(idstr);
                    if (!idstr.empty()) {
                        attributes[param] = std::stod(idstr);
                    }
                }
            }

            edges.push_back(Edge{from, to, id, attributes});
        }
        return edges;
    }

    static std::string getFileExtension(const std::string& filename) {
        size_t dot_pos = filename.find_last_of('.');
        if (dot_pos == std::string::npos) return "";
        return filename.substr(dot_pos + 1);
    }

    static std::vector<Edge> loadAuto(const std::string& filename) {
    std::string ext = getFileExtension(filename);
        if (ext == "csv") {
            return loadFromCSV(filename);
        } else if (ext == "txt") {
            return loadFromTXT(filename);
        } else {
            throw std::runtime_error("unknown file extension: " + ext);
        }
    }
};
