#ifndef GROUP_H
#define GROUP_H

#include <string>
#include <vector>

class Group {
public:
    // Constructor using member initializer list
    Group(const std::string& principleName, const std::vector<std::string>& members)
        : groupName(principleName), members(members) {}

    // Getters
    std::string getGroupName() const { return groupName; }
    std::vector<std::string> getMembers() const { return members; }

private:
    std::string groupName;
    std::vector<std::string> members;
};

#endif // GROUP_H
