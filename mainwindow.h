#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <unordered_map>
#include <stack>
#include <queue>
#include <set>
#include <map>
#include <list>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();

private:
    Ui::MainWindow *ui;
    std::unordered_map<QChar, QString> morseMap;
    std::unordered_map<QString, QChar> reverseMorseMap;
    std::list<QString> historyList; // Linked list for history
    std::queue<QString> conversionQueue;
    std::set<QString> uniqueMorseSet;
    std::map<QChar, int> frequencyMap;

    struct TreeNode {
        QString value;
        TreeNode* left;
        TreeNode* right;
        TreeNode(QString val) : value(val), left(nullptr), right(nullptr) {}
    };
    TreeNode* morseTreeRoot;

    void initMorseMap();
    void initializeReverseMorseCodeMap();
    void buildMorseTree();
    void traverseMorseTree(TreeNode* node, QString &result);
    QString textToMorse(const QString& text);
    QString morseToText(const QString& morseCode);
    void processQueue();
    void logFrequency();
    void saveHistory();
    void loadHistory();
    void printHistory();
};

#endif // MAINWINDOW_H
