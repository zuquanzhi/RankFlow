#ifndef BINARYSEARCHTREE_H
#define BINARYSEARCHTREE_H

#include <QObject>
#include <QList>
#include <functional>
#include <algorithm>
#include <stdexcept>
#include "teamdata.h"

template<typename T>
struct TreeNode {
    T data;
    TreeNode* left;
    TreeNode* right;
    
    TreeNode(const T& value) : data(value), left(nullptr), right(nullptr) {}
    ~TreeNode() {
        delete left;
        delete right;
    }
};

// 二叉搜索树模板类，支持自定义比较函数
template<typename T>
class BinarySearchTree
{
public:
    using CompareFunc = std::function<bool(const T&, const T&)>;
    
    explicit BinarySearchTree(CompareFunc compareFunc)
        : m_root(nullptr), m_compare(compareFunc), m_size(0)
    {
    }
    
    ~BinarySearchTree()
    {
        clear();
    }
    
    // 基本操作
    void insert(const T& data)
    {
        m_root = insertHelper(m_root, data);
        m_size++;
    }
    
    bool search(const T& data) const
    {
        return searchHelper(m_root, data);
    }
    
    void remove(const T& data)
    {
        m_root = removeHelper(m_root, data);
    }
    
    void clear()
    {
        clearHelper(m_root);
        m_root = nullptr;
        m_size = 0;
    }
    
    // 遍历操作
    QList<T> inorderTraversal() const
    {
        QList<T> result;
        inorderHelper(m_root, result);
        return result;
    }
    
    QList<T> preorderTraversal() const
    {
        QList<T> result;
        preorderHelper(m_root, result);
        return result;
    }
    
    QList<T> postorderTraversal() const
    {
        QList<T> result;
        postorderHelper(m_root, result);
        return result;
    }
    
    // 查询操作
    QList<T> findRange(const T& min, const T& max) const
    {
        QList<T> result;
        rangeHelper(m_root, min, max, result);
        return result;
    }
    
    T findMin() const
    {
        if (m_root == nullptr) {
            throw std::runtime_error("Tree is empty");
        }
        
        TreeNode<T>* minNode = findMinNode(m_root);
        return minNode->data;
    }
    
    T findMax() const
    {
        if (m_root == nullptr) {
            throw std::runtime_error("Tree is empty");
        }
        
        TreeNode<T>* maxNode = findMaxNode(m_root);
        return maxNode->data;
    }
    
    // 统计信息
    int size() const { return m_size; }
    
    int height() const
    {
        return heightHelper(m_root);
    }
    
    bool isEmpty() const { return m_root == nullptr; }
    
private:
    TreeNode<T>* m_root;
    CompareFunc m_compare;
    int m_size;
    
    // 私有辅助函数
    TreeNode<T>* insertHelper(TreeNode<T>* node, const T& data)
    {
        if (node == nullptr) {
            return new TreeNode<T>(data);
        }
        
        if (m_compare(data, node->data)) {
            node->left = insertHelper(node->left, data);
        } else {
            node->right = insertHelper(node->right, data);
        }
        
        return node;
    }
    
    bool searchHelper(TreeNode<T>* node, const T& data) const
    {
        if (node == nullptr) {
            return false;
        }
        
        if (!m_compare(data, node->data) && !m_compare(node->data, data)) {
            return true; // 相等
        }
        
        if (m_compare(data, node->data)) {
            return searchHelper(node->left, data);
        } else {
            return searchHelper(node->right, data);
        }
    }
    
    TreeNode<T>* removeHelper(TreeNode<T>* node, const T& data)
    {
        if (node == nullptr) {
            return nullptr;
        }
        
        if (m_compare(data, node->data)) {
            node->left = removeHelper(node->left, data);
        } else if (m_compare(node->data, data)) {
            node->right = removeHelper(node->right, data);
        } else {
            // 找到要删除的节点
            m_size--;
            
            if (node->left == nullptr) {
                TreeNode<T>* temp = node->right;
                node->left = nullptr; // 防止析构函数递归删除
                node->right = nullptr;
                delete node;
                return temp;
            } else if (node->right == nullptr) {
                TreeNode<T>* temp = node->left;
                node->left = nullptr;
                node->right = nullptr;
                delete node;
                return temp;
            }
            
            // 有两个子节点的情况
            TreeNode<T>* successor = findMinNode(node->right);
            node->data = successor->data;
            node->right = removeHelper(node->right, successor->data);
        }
        
        return node;
    }
    
    TreeNode<T>* findMinNode(TreeNode<T>* node) const
    {
        while (node->left != nullptr) {
            node = node->left;
        }
        return node;
    }
    
    TreeNode<T>* findMaxNode(TreeNode<T>* node) const
    {
        while (node->right != nullptr) {
            node = node->right;
        }
        return node;
    }
    
    void inorderHelper(TreeNode<T>* node, QList<T>& result) const
    {
        if (node == nullptr) {
            return;
        }
        
        inorderHelper(node->left, result);
        result.append(node->data);
        inorderHelper(node->right, result);
    }
    
    void preorderHelper(TreeNode<T>* node, QList<T>& result) const
    {
        if (node == nullptr) {
            return;
        }
        
        result.append(node->data);
        preorderHelper(node->left, result);
        preorderHelper(node->right, result);
    }
    
    void postorderHelper(TreeNode<T>* node, QList<T>& result) const
    {
        if (node == nullptr) {
            return;
        }
        
        postorderHelper(node->left, result);
        postorderHelper(node->right, result);
        result.append(node->data);
    }
    
    void rangeHelper(TreeNode<T>* node, const T& min, const T& max, QList<T>& result) const
    {
        if (node == nullptr) {
            return;
        }
        
        // 如果当前节点在范围内
        if (!m_compare(node->data, min) && !m_compare(max, node->data)) {
            result.append(node->data);
        }
        
        // 递归搜索左子树（如果有可能包含范围内的值）
        if (!m_compare(node->data, min)) {
            rangeHelper(node->left, min, max, result);
        }
        
        // 递归搜索右子树（如果有可能包含范围内的值）
        if (!m_compare(max, node->data)) {
            rangeHelper(node->right, min, max, result);
        }
    }
    
    int heightHelper(TreeNode<T>* node) const
    {
        if (node == nullptr) {
            return 0;
        }
        
        int leftHeight = heightHelper(node->left);
        int rightHeight = heightHelper(node->right);
        
        return 1 + std::max(leftHeight, rightHeight);
    }
    
    void clearHelper(TreeNode<T>* node)
    {
        if (node != nullptr) {
            clearHelper(node->left);
            clearHelper(node->right);
            delete node;
        }
    }
};

// 专门用于团队数据的查询树管理器
class TeamQueryTree : public QObject
{
    Q_OBJECT
    
public:
    enum SortCriteria {
        ByTeamId,
        ByTeamName,
        ByTotalScore,
        ByLastSubmitTime,
        BySolvedProblems,
        ByAccuracy
    };
    
    explicit TeamQueryTree(QObject *parent = nullptr);
    ~TeamQueryTree();
    
    // 数据管理
    void buildTree(const QList<TeamData>& teams, SortCriteria criteria);
    void addTeam(const TeamData& team);
    void removeTeam(const QString& teamId);
    void updateTeam(const TeamData& team);
    void clear();
    
    // 查询功能
    QList<TeamData> getAllTeams() const;
    QList<TeamData> getTeamsInRange(const QString& minValue, const QString& maxValue) const;
    QList<TeamData> getTeamsInScoreRange(int minScore, int maxScore) const;
    QList<TeamData> getTopTeams(int count) const;
    QList<TeamData> getBottomTeams(int count) const;
    
    // 搜索功能
    TeamData findTeam(const QString& teamId) const;
    QList<TeamData> searchByName(const QString& namePattern) const;
    QList<TeamData> searchBySolvedProblems(int minSolved) const;
    QList<TeamData> searchByAccuracy(double minAccuracy) const;
    
    // 统计信息
    int totalTeams() const;
    SortCriteria currentCriteria() const { return m_currentCriteria; }
    
signals:
    void treeRebuilt(SortCriteria criteria);
    void teamAdded(const QString& teamId);
    void teamRemoved(const QString& teamId);
    void teamUpdated(const QString& teamId);

private:
    SortCriteria m_currentCriteria;
    QList<TeamData> m_teams; // 保持原始数据副本用于其他类型查询
    
    // 辅助函数
    bool matchesPattern(const QString& text, const QString& pattern) const;
};

#endif // BINARYSEARCHTREE_H
