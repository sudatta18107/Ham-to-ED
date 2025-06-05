#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <limits>
#include <cassert>

// Function: generateBinaryStrings
// Generates all binary strings of the given length.
std::vector<std::string> generateBinaryStrings(int length) {
    std::vector<std::string> result;
    int total = 1 << length; // 2^length
    for (int i = 0; i < total; i++) {
        std::string s;
        // Build string bit-by-bit (most-significant bit first)
        for (int j = length - 1; j >= 0; j--) {
            s.push_back(((i >> j) & 1) ? '1' : '0');
        }
        result.push_back(s);
    }
    return result;
}

// Function: embed_with_array
// Given binary strings x and y and an array A, produces the embedding.
// For each i from 0 to A.size()-1, append x[i] then append the next A[i] bits from y.
// Finally, append the last bit of x.
std::string embed_with_array(const std::string &x, const std::string &y, const std::vector<int> &A) {
    std::string result;
    int pointer = 0; // points to the next bit in y
    for (size_t i = 0; i < A.size(); i++) {
        // Append the i-th bit of x.
        result.push_back(x[i]);
        // Append the next A[i] bits from y.
        int k = A[i];
        result.append(y.substr(pointer, k));
        pointer += k;
    }
    // Append the last bit of x.
    result.push_back(x.back());
    return result;
}

// Recursive helper to generate valid A vectors of given length with a target sum.
// Allowed values are in [0, 6]. Later we check the consecutive restriction.
void generateValidARec(int pos, int length, int sum, int targetSum,
                       std::vector<int>& current, std::vector<std::vector<int>>& result) {
    if (pos == length) {
        if (sum == targetSum) {
            bool valid = true;
            // Check each consecutive pair for forbidden combinations: (0,0), (0,1) or (1,0).
            for (int i = 0; i < length - 1; i++) {
                if ((current[i] == 0 && (current[i+1] == 0 || current[i+1] == 1)) ||
                    (current[i] == 1 && current[i+1] == 0)) {
                    valid = false;
                    break;
                }
            }
            if (valid) {
                result.push_back(current);
            }
        }
        return;
    }
    for (int val = 0; val <= 8; val++) {
        if (sum + val > targetSum)
            break; // early break since val increases from 0 to 6.
        current[pos] = val;
        generateValidARec(pos + 1, length, sum + val, targetSum, current, result);
    }
}

// Function: generate_valid_A
// Generates all vectors A of length 7 with each element in {0,...,6}, summing to 9,
// and such that consecutive entries are not (0,0), (0,1), or (1,0).
std::vector<std::vector<int>> generate_valid_A() {
    std::vector<std::vector<int>> result;
    int length = 9;
    int targetSum = 12;
    std::vector<int> current(length, 0);
    generateValidARec(0, length, 0, targetSum, current, result);
    return result;
}

// Function: hamming_distance
// Computes the Hamming distance between two same-length strings.
int hamming_distance(const std::string &s1, const std::string &s2) {
    int distance = 0;
    assert(s1.size() == s2.size());
    for (size_t i = 0; i < s1.size(); i++) {
        if (s1[i] != s2[i])
            distance++;
    }
    return distance;
}

// Function: edit_distance
// Computes the Levenshtein distance between two strings using dynamic programming.
int edit_distance(const std::string &s1, const std::string &s2) {
    int m = s1.size(), n = s2.size();
    std::vector<std::vector<int>> dp(m+1, std::vector<int>(n+1, 0));
    
    // Initialize boundaries.
    for (int i = 0; i <= m; i++)
        dp[i][0] = i;
    for (int j = 0; j <= n; j++)
        dp[0][j] = j;
    
    // Fill the table.
    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (s1[i-1] == s2[j-1])
                dp[i][j] = dp[i-1][j-1];
            else {
                dp[i][j] = 1 + std::min({dp[i-1][j],     // deletion
                                         dp[i][j-1],     // insertion
                                         dp[i-1][j-1]}); // substitution
            }
        }
    }
    return dp[m][n];
}


int main() {
    // Set n = 8 so that x is of length 8 and y is of length n+1 = 9.
    int n = 9;
    
    // Generate all binary strings for x of length n.
    std::vector<std::string> bin_x_list = generateBinaryStrings(n);
    // Generate all binary strings for y of length n+1.
    std::vector<std::string> bin_y_list = generateBinaryStrings(n + 1);
    // Generate all valid A arrays.
    std::vector<std::vector<int>> valid_A_list = generate_valid_A();
     
    // For each A and for each y, compute embedded strings for every x.
    for (const auto &A : valid_A_list) {
        for (const auto &y : bin_y_list) {
            std::vector<std::string> embedded_strings;
            for (const auto &x : bin_x_list) {
                embedded_strings.push_back(embed_with_array(x, y, A));
            }
            bool valid = true;
            // Example check: compare every pair of embedded strings.
            for (size_t i = 0; i < embedded_strings.size(); i++) {
                for (size_t j = i + 1; j < embedded_strings.size(); j++) {
                    if (edit_distance(embedded_strings[i], embedded_strings[j]) != 
                        hamming_distance(embedded_strings[i], embedded_strings[j])) {
                        valid = false;
                        break;
                    }
                }
                if (!valid) break;
            }
            // If the property holds for all pairs, print the valid y and A.
            if (valid) {
                std::cout << "Valid embedding with y = " << y << " and A = [";
                for (size_t i = 0; i < A.size(); i++) {
                    std::cout << A[i] << (i < A.size()-1 ? ", " : "");
                }
                std::cout << "]" << std::endl;
                // Break out of the y-loop if only one valid y is desired per A.
                break;
            }
        }
    }
    
    return 0;
}


